#pragma once

#include <godot_cpp/classes/node.hpp>

namespace godot {

class PureDataInstance : public Node {
	GDCLASS(PureDataInstance, Node)

private:
	double time_passed;
	float initialized_{};
	bool message_guard_{};
	int in_channel_count = 1;
	int out_channel_count = 1;
	int sample_rate = 44100;

	int init_audio();

protected:
	static void _bind_methods();

public:
	PureDataInstance();
	~PureDataInstance();

	bool is_initialized() const;
	bool send_bang(String receiver);
	bool send_float(String receiver, float value);
	bool start_message(int max_length);
	void add_float(float value);
	void add_symbol(String value);
	bool finish_list(String receiver);
	bool finish_message(String receiver, String message);
	void bind(String receiver);
	bool start_gui(String pure_data_bin_dir_path);
	int get_array_size(String array_name);
	int set_array_size(String array_name, int size);
	int write_array(String array_name, int offset, PackedFloat32Array src, int n);
	PackedFloat32Array read_array(String array_name, int offset, int n);

	int get_in_channel_count() const;
	void set_in_channel_count(int count);
	int get_out_channel_count() const;
	void set_out_channel_count(int count);
	int get_sample_rate() const;
	void set_sample_rate(int rate);
};

};
