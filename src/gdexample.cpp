#include "gdexample.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/audio_stream_generator_playback.hpp>

#include <thread>


using namespace godot;

class PDStatic 
{
	std::vector<GDExample*> instances_;
public:
	static PDStatic& get_instance()
	{
		static PDStatic instance;

		::libpd_set_banghook(&PDStatic::banghook);
		::libpd_set_floathook(&PDStatic::floathook);

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

	static void register_instance(GDExample* instance)
	{
		get_instance().instances_.push_back(instance);
	}
};

static void _print(const char *s)
{
	std::cout << s << std::endl;
}

void PDPatchFile::_bind_methods() 
{
	ClassDB::bind_method(D_METHOD("open"), &PDPatchFile::open);
	ClassDB::bind_method(D_METHOD("close"), &PDPatchFile::close);
}

bool PDPatchFile::open(String path)
{
	handle_ = ::libpd_openfile(path.get_file().utf8().get_data(), path.get_base_dir().utf8().get_data());
	return handle_ != nullptr;
}

void PDPatchFile::close()
{
	::libpd_closefile(handle_);
}

void GDExample::_bind_methods() 
{
	ClassDB::bind_method(D_METHOD("is_initialized"), &GDExample::is_initialized);
	ClassDB::bind_method(D_METHOD("start_message"), &GDExample::start_message);
	ClassDB::bind_method(D_METHOD("send_bang"), &GDExample::send_bang);
	ClassDB::bind_method(D_METHOD("send_float"), &GDExample::send_float);
	ClassDB::bind_method(D_METHOD("add_float"), &GDExample::add_float);
	ClassDB::bind_method(D_METHOD("add_symbol"), &GDExample::add_symbol);
	ClassDB::bind_method(D_METHOD("finish_list"), &GDExample::finish_list);
	ClassDB::bind_method(D_METHOD("finish_message"), &GDExample::finish_message);
	ClassDB::bind_method(D_METHOD("get_array_size"), &GDExample::get_array_size);
	ClassDB::bind_method(D_METHOD("set_array_size"), &GDExample::set_array_size);
	ClassDB::bind_method(D_METHOD("write_array"), &GDExample::write_array);
	ClassDB::bind_method(D_METHOD("read_array"), &GDExample::read_array);
	ClassDB::bind_method(D_METHOD("start_gui"), &GDExample::start_gui);
	ClassDB::bind_method(D_METHOD("bind"), &GDExample::bind);
	ADD_SIGNAL(MethodInfo("bang", PropertyInfo(Variant::STRING, "receiver")));
	ADD_SIGNAL(MethodInfo("float", PropertyInfo(Variant::STRING, "receiver"), PropertyInfo(Variant::FLOAT, "value")));
}


GDExample::GDExample() {
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
	::libpd_set_printhook(_print);

	PDStatic::register_instance(this);

	::libpd_start_message(1);
	::libpd_add_float(1.0);
	::libpd_finish_message("pd", "dsp");
}


GDExample::~GDExample() 
{
}

bool GDExample::is_initialized() const
{
	return initialized_;
}

bool GDExample::send_bang(String receiver)
{
	return ::libpd_bang(receiver.utf8().get_data()) == 0;
}

bool GDExample::send_float(String receiver, float value)
{
	return ::libpd_float(receiver.utf8().get_data(), value) == 0;
}

bool GDExample::start_message(int max_length)
{
	if(message_guard_) 
	{
		ERR_PRINT("STARTING TWO MESSAGES AT ONCE!!!");
	}

	message_guard_ = true;
	return ::libpd_start_message(max_length) == 0;
}

void GDExample::add_float(float value)
{
	::libpd_add_float(value);
}

void GDExample::add_symbol(String value)
{
	::libpd_add_symbol(value.utf8().get_data());
}

bool GDExample::finish_list(String receiver)
{
	return ::libpd_finish_list(receiver.utf8().get_data()) == 0;
}

bool GDExample::finish_message(String receiver, String message)
{
	if(!message_guard_) 
	{
		ERR_PRINT("NEVER STARTED MESSAGE");
	}

	message_guard_ = false;

	return ::libpd_finish_message(receiver.utf8().get_data(), message.utf8().get_data()) == 0;
}

bool GDExample::start_gui(String string)
{
	return ::libpd_start_gui(string.utf8()) == 0;
}

void GDExample::bind(String string)
{
	::libpd_bind(string.utf8());
}

int GDExample::get_array_size(String string)
{
	return ::libpd_arraysize(string.utf8());
}

int GDExample::set_array_size(String string, int size)
{
	return ::libpd_resize_array(string.utf8(), size);
}

int GDExample::write_array(String array_name, int offset, PackedRealArray src, int n)
{
	return ::libpd_write_array(array_name.utf8(), offset, src.ptr(), n);
}

PackedRealArray GDExample::read_array(String array_name, int offset, int n)
{
	PackedRealArray dest;
	dest.resize(n);
	::libpd_read_array(dest.ptrw(), array_name.utf8(), offset, n);

	return dest;
}


void GDExample::_process(double delta) 
{
	if(is_playing()) 
	{
		Ref<AudioStreamGeneratorPlayback> p = get_stream_playback();
		if(p.is_valid()) {	
			int nframes = std::min(p->get_frames_available(), 2048);

			int ticks = nframes / libpd_blocksize();


			if(::libpd_process_float(ticks, inbuf_, outbuf_) != 0)
			{
				return;
			}

			for(int i = 0; i < nframes; i++)
			{
				auto v = Vector2(outbuf_[i*2], outbuf_[(i*2)+1]);
				v = v.clamp(Vector2(-1, -1), Vector2(1, 1));

				p->push_frame(v);
			}
		}
	}
}
