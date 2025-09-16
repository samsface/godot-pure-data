#pragma once

#include <godot_cpp/classes/audio_stream_generator_playback.hpp>
#include <godot_cpp/classes/audio_stream_generator.hpp>

namespace godot {

class PureDataAudioStream : public AudioStreamGenerator {
	GDCLASS(PureDataAudioStream, AudioStreamGenerator)

private:
	double time_passed;
	float inbuf_[1];
	float outbuf_[44100 * 2];
	Ref<AudioStreamGeneratorPlayback> playback;

protected:
	static void _bind_methods();

public:
	PureDataAudioStream();
	~PureDataAudioStream();

	void _process(double delta);

	virtual Ref<AudioStreamPlayback> instantiate_playback();
};

};
