#pragma once

#include <godot_cpp/classes/audio_stream_player.hpp>

namespace godot {

class PureDataPatch : public RefCounted {
	GDCLASS(PureDataPatch, RefCounted)

	void *handle_{};

protected:
	static void _bind_methods();

public:
	PureDataPatch() = default;
	~PureDataPatch() = default;

	bool open(String path);
	void close();
};

class PureDataAudioStreamPlayer : public AudioStreamPlayer {
	GDCLASS(PureDataAudioStreamPlayer, AudioStreamPlayer)

private:
	double time_passed;
	float inbuf_[1];
	float outbuf_[44100 * 2];
	float initialized_{};
	bool message_guard_{};

protected:
	static void _bind_methods();

public:
	PureDataAudioStreamPlayer();
	~PureDataAudioStreamPlayer();

	void _process(double delta) override;

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
};
}; //namespace godot
