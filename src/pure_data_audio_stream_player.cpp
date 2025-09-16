#include "pure_data_audio_stream_player.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#define PD_INTERNAL

#include "z_libpd.h"
#include "z_print_util.h"

using namespace godot;

class PureDataStatic
{
	std::vector<PureDataAudioStream*> instances_;
public:
	static PureDataStatic& get_instance()
	{
		static PureDataStatic instance;

		::libpd_set_banghook(&PureDataStatic::banghook);
		::libpd_set_floathook(&PureDataStatic::floathook);

		return instance;
	}

	static void banghook(char const* receiver)
	{
		for(auto instance : get_instance().instances_)
		{
			instance->emit_signal("bang", receiver);
		}
	}


	static void floathook(char const* receiver, float value)
	{
		for(auto instance : get_instance().instances_)
		{
			instance->emit_signal("float", receiver, value);
		}
	}

	static void register_instance(PureDataAudioStream* instance)
	{
		get_instance().instances_.push_back(instance);
	}
};

static void _print(const char *s)
{
	UtilityFunctions::print(s);
}

void PureDataPatch::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("open"), &PureDataPatch::open);
	ClassDB::bind_method(D_METHOD("close"), &PureDataPatch::close);
}

bool PureDataPatch::open(String path)
{
	handle_ = ::libpd_openfile(path.get_file().utf8().get_data(), path.get_base_dir().utf8().get_data());
	return handle_ != nullptr;
}

void PureDataPatch::close()
{
	::libpd_closefile(handle_);
}

void PureDataAudioStream::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("is_initialized"), &PureDataAudioStream::is_initialized);
	ClassDB::bind_method(D_METHOD("start_message"), &PureDataAudioStream::start_message);
	ClassDB::bind_method(D_METHOD("send_bang"), &PureDataAudioStream::send_bang);
	ClassDB::bind_method(D_METHOD("send_float"), &PureDataAudioStream::send_float);
	ClassDB::bind_method(D_METHOD("add_float"), &PureDataAudioStream::add_float);
	ClassDB::bind_method(D_METHOD("add_symbol"), &PureDataAudioStream::add_symbol);
	ClassDB::bind_method(D_METHOD("finish_list"), &PureDataAudioStream::finish_list);
	ClassDB::bind_method(D_METHOD("finish_message"), &PureDataAudioStream::finish_message);
	ClassDB::bind_method(D_METHOD("get_array_size"), &PureDataAudioStream::get_array_size);
	ClassDB::bind_method(D_METHOD("set_array_size"), &PureDataAudioStream::set_array_size);
	ClassDB::bind_method(D_METHOD("write_array"), &PureDataAudioStream::write_array);
	ClassDB::bind_method(D_METHOD("read_array"), &PureDataAudioStream::read_array);
	ClassDB::bind_method(D_METHOD("start_gui"), &PureDataAudioStream::start_gui);
	ClassDB::bind_method(D_METHOD("bind"), &PureDataAudioStream::bind);
	ADD_SIGNAL(MethodInfo("bang", PropertyInfo(Variant::STRING, "receiver")));
	ADD_SIGNAL(MethodInfo("float", PropertyInfo(Variant::STRING, "receiver"), PropertyInfo(Variant::FLOAT, "value")));
}


PureDataAudioStream::PureDataAudioStream() {
	initialized_ = ::libpd_init() == 0;
	if(!initialized_) {
		return;
	}

	initialized_ = ::libpd_init_audio(1, 2, 44100) == 0;
	if(!initialized_) {
		return;
	}

	//::libpd_set_instancedata(this, nullptr);
	::libpd_set_verbose(1);


	::libpd_set_printhook(::libpd_print_concatenator);
	::libpd_set_concatenated_printhook(_print);

	PureDataStatic::register_instance(this);

	::libpd_start_message(1);
	::libpd_add_float(1.0);
	::libpd_finish_message("pd", "dsp");
}


PureDataAudioStream::~PureDataAudioStream()
{
}

bool PureDataAudioStream::is_initialized() const
{
	return initialized_;
}

bool PureDataAudioStream::send_bang(String receiver)
{
	return ::libpd_bang(receiver.utf8().get_data()) == 0;
}

bool PureDataAudioStream::send_float(String receiver, float value)
{
	return ::libpd_float(receiver.utf8().get_data(), value) == 0;
}

bool PureDataAudioStream::start_message(int max_length)
{
	if(message_guard_)
	{
		ERR_PRINT("STARTING TWO MESSAGES AT ONCE!!!");
	}

	message_guard_ = true;
	return ::libpd_start_message(max_length) == 0;
}

void PureDataAudioStream::add_float(float value)
{
	::libpd_add_float(value);
}

void PureDataAudioStream::add_symbol(String value)
{
	::libpd_add_symbol(value.utf8().get_data());
}

bool PureDataAudioStream::finish_list(String receiver)
{
	return ::libpd_finish_list(receiver.utf8().get_data()) == 0;
}

bool PureDataAudioStream::finish_message(String receiver, String message)
{
	if(!message_guard_)
	{
		ERR_PRINT("NEVER STARTED MESSAGE");
	}

	message_guard_ = false;

	return ::libpd_finish_message(receiver.utf8().get_data(), message.utf8().get_data()) == 0;
}

bool PureDataAudioStream::start_gui(String string)
{
	return ::libpd_start_gui(string.utf8()) == 0;
}

void PureDataAudioStream::bind(String string)
{
	::libpd_bind(string.utf8());
}

int PureDataAudioStream::get_array_size(String string)
{
	return ::libpd_arraysize(string.utf8());
}

int PureDataAudioStream::set_array_size(String string, int size)
{
	return ::libpd_resize_array(string.utf8(), size);
}

int PureDataAudioStream::write_array(String array_name, int offset, PackedFloat32Array src, int n)
{
	return ::libpd_write_array(array_name.utf8(), offset, src.ptr(), n);
}

PackedFloat32Array PureDataAudioStream::read_array(String array_name, int offset, int n)
{
	PackedFloat32Array dest;
	dest.resize(n);
	::libpd_read_array(dest.ptrw(), array_name.utf8(), offset, n);

	return dest;
}


void PureDataAudioStream::_process(double delta)
{
	if(playback->is_playing())
	{
		if(playback.is_valid()) {
			int nframes = std::min(playback->get_frames_available(), 2048);

			int ticks = nframes / libpd_blocksize();


			if(::libpd_process_float(ticks, inbuf_, outbuf_) != 0)
			{
				return;
			}

			for(int i = 0; i < nframes; i++)
			{
				auto v = Vector2(outbuf_[i*2], outbuf_[(i*2)+1]);
				v = v.clamp(Vector2(-1, -1), Vector2(1, 1));

				playback->push_frame(v);
			}
		}
	}
}

Ref<AudioStreamPlayback> PureDataAudioStream::instantiate_playback() {
	playback = AudioStreamGenerator::instantiate_playback();
	return playback;
}
