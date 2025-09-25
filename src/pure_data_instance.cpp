#include "pure_data_instance.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#define PD_INTERNAL

#include <z_libpd.h>
#include <z_print_util.h>

using namespace godot;

class PureDataStatic {
	std::vector<PureDataInstance *> instances_;

public:
	static PureDataStatic &get_instance() {
		static PureDataStatic instance;

		::libpd_set_banghook(&PureDataStatic::banghook);
		::libpd_set_floathook(&PureDataStatic::floathook);

		return instance;
	}

	static void banghook(char const *receiver) {
		for (auto instance : get_instance().instances_) {
			instance->emit_signal("bang", receiver);
		}
	}

	static void floathook(char const *receiver, float value) {
		for (auto instance : get_instance().instances_) {
			instance->emit_signal("float", receiver, value);
		}
	}

	static void register_instance(PureDataInstance *instance) {
		get_instance().instances_.push_back(instance);
	}
};

static void _print(const char *s) {
	UtilityFunctions::print(s);
}

void PureDataInstance::_bind_methods() {
	ClassDB::bind_method(D_METHOD("is_initialized"), &PureDataInstance::is_initialized);
	ClassDB::bind_method(D_METHOD("start_message"), &PureDataInstance::start_message);
	ClassDB::bind_method(D_METHOD("send_bang"), &PureDataInstance::send_bang);
	ClassDB::bind_method(D_METHOD("send_float"), &PureDataInstance::send_float);
	ClassDB::bind_method(D_METHOD("add_float"), &PureDataInstance::add_float);
	ClassDB::bind_method(D_METHOD("add_symbol"), &PureDataInstance::add_symbol);
	ClassDB::bind_method(D_METHOD("finish_list"), &PureDataInstance::finish_list);
	ClassDB::bind_method(D_METHOD("finish_message"), &PureDataInstance::finish_message);
	ClassDB::bind_method(D_METHOD("get_array_size"), &PureDataInstance::get_array_size);
	ClassDB::bind_method(D_METHOD("set_array_size"), &PureDataInstance::set_array_size);
	ClassDB::bind_method(D_METHOD("write_array"), &PureDataInstance::write_array);
	ClassDB::bind_method(D_METHOD("read_array"), &PureDataInstance::read_array);
	ClassDB::bind_method(D_METHOD("start_gui"), &PureDataInstance::start_gui);
	ClassDB::bind_method(D_METHOD("bind"), &PureDataInstance::bind);
	ADD_SIGNAL(MethodInfo("bang", PropertyInfo(Variant::STRING, "receiver")));
	ADD_SIGNAL(MethodInfo("float", PropertyInfo(Variant::STRING, "receiver"), PropertyInfo(Variant::FLOAT, "value")));

	ClassDB::bind_method(D_METHOD("set_in_channel_count", "channels"), &PureDataInstance::set_in_channel_count);
	ClassDB::bind_method(D_METHOD("get_in_channel_count"), &PureDataInstance::get_in_channel_count);
	ClassDB::bind_method(D_METHOD("set_out_channel_count", "channels"), &PureDataInstance::set_out_channel_count);
	ClassDB::bind_method(D_METHOD("get_out_channel_count"), &PureDataInstance::get_out_channel_count);
	ClassDB::bind_method(D_METHOD("set_sample_rate", "hz"), &PureDataInstance::set_sample_rate);
	ClassDB::bind_method(D_METHOD("get_sample_rate"), &PureDataInstance::get_sample_rate);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "in_channel_count"), "set_in_channel_count", "get_in_channel_count");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "out_channel_count"), "set_out_channel_count", "get_out_channel_count");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "sample_rate", PROPERTY_HINT_RANGE, "20,192000,1,suffix:Hz"), "set_sample_rate", "get_sample_rate");
}

PureDataInstance::PureDataInstance() {
	initialized_ = ::libpd_init() == 0;
	if (!initialized_) {
		return;
	}

	initialized_ = init_audio() == 0;
	if (!initialized_) {
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

PureDataInstance::~PureDataInstance() {
}

int PureDataInstance::init_audio() {
	std::fill_n(in_buffer, std::size(in_buffer), 0);
	std::fill_n(out_buffer, std::size(out_buffer), 0);

	// Initialize Pure Data
	return ::libpd_init_audio(in_channel_count, out_channel_count, sample_rate);
}

std::array<float, BUFFER_SIZE> PureDataInstance::_process_audio(bool process, int ticks) {
	if (process) {
		::libpd_process_float(ticks, in_buffer, out_buffer);

		// Clear buffer
		std::fill_n(in_buffer, std::size(in_buffer), 0);

		// Write output to the output array
		for (int i = 0; i < BUFFER_SIZE; ++i) {
			out_array[i] = out_buffer[i];
		}
	}

	return out_array;
}

void PureDataInstance::add_input(std::array<float, BUFFER_SIZE> &input) {
	for (int i = 0; i < BUFFER_SIZE; i++) {
		in_buffer[i] += input[i];
	}
}

bool PureDataInstance::is_initialized() const {
	return initialized_;
}

bool PureDataInstance::send_bang(String receiver) {
	return ::libpd_bang(receiver.utf8().get_data()) == 0;
}

bool PureDataInstance::send_float(String receiver, float value) {
	return ::libpd_float(receiver.utf8().get_data(), value) == 0;
}

bool PureDataInstance::start_message(int max_length) {
	if (message_guard_) {
		ERR_PRINT("STARTING TWO MESSAGES AT ONCE!!!");
	}

	message_guard_ = true;
	return ::libpd_start_message(max_length) == 0;
}

void PureDataInstance::add_float(float value) {
	::libpd_add_float(value);
}

void PureDataInstance::add_symbol(String value) {
	::libpd_add_symbol(value.utf8().get_data());
}

bool PureDataInstance::finish_list(String receiver) {
	return ::libpd_finish_list(receiver.utf8().get_data()) == 0;
}

bool PureDataInstance::finish_message(String receiver, String message) {
	if (!message_guard_) {
		ERR_PRINT("NEVER STARTED MESSAGE");
	}

	message_guard_ = false;

	return ::libpd_finish_message(receiver.utf8().get_data(), message.utf8().get_data()) == 0;
}

bool PureDataInstance::start_gui(String string) {
	return ::libpd_start_gui(string.utf8()) == 0;
}

void PureDataInstance::bind(String string) {
	::libpd_bind(string.utf8());
}

int PureDataInstance::get_array_size(String string) {
	return ::libpd_arraysize(string.utf8());
}

int PureDataInstance::set_array_size(String string, int size) {
	return ::libpd_resize_array(string.utf8(), size);
}

int PureDataInstance::write_array(String array_name, int offset, PackedFloat32Array src, int n) {
	return ::libpd_write_array(array_name.utf8(), offset, src.ptr(), n);
}

PackedFloat32Array PureDataInstance::read_array(String array_name, int offset, int n) {
	PackedFloat32Array dest;
	dest.resize(n);
	::libpd_read_array(dest.ptrw(), array_name.utf8(), offset, n);

	return dest;
}

int PureDataInstance::get_in_channel_count() const {
	return in_channel_count;
}

void PureDataInstance::set_in_channel_count(int count) {
	in_channel_count = count;
	init_audio();
}

int PureDataInstance::get_out_channel_count() const {
	return out_channel_count;
}

void PureDataInstance::set_out_channel_count(int count) {
	out_channel_count = count;
	init_audio();
}

int PureDataInstance::get_sample_rate() const {
	return sample_rate;
}

void PureDataInstance::set_sample_rate(int rate) {
	sample_rate = rate;
	init_audio();
}
