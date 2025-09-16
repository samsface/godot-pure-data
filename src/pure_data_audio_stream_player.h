#pragma once

#include <godot_cpp/classes/audio_stream.hpp>
#include <godot_cpp/classes/audio_stream_playback_resampled.hpp>

namespace godot {

class AudioStreamPureData : public AudioStream {
	GDCLASS(AudioStreamPureData, AudioStream)

protected:
	static void _bind_methods();

public:
	AudioStreamPureData();
	~AudioStreamPureData();

	virtual Ref<AudioStreamPlayback> _instantiate_playback() const override;

	virtual String _get_stream_name() const override;
	virtual double _get_length() const override;
	virtual bool _is_monophonic() const override;
};

class AudioStreamPureDataPlayback : public AudioStreamPlaybackResampled {
	GDCLASS(AudioStreamPureDataPlayback, AudioStreamPlaybackResampled);
	friend class AudioStreamPureData;
	bool active;
	float mixed;

private:
	float inbuf_[1];
	float outbuf_[44100 * 2];

protected:
	static void _bind_methods();

public:
	AudioStreamPureDataPlayback();

	virtual int _mix_resampled(AudioFrame *p_buffer, int p_frames) override;
	virtual float _get_stream_sampling_rate() const override;

	virtual void _start(double p_from_pos = 0.0) override;
	virtual void _stop() override;
	virtual bool _is_playing() const override;

	virtual int _get_loop_count() const override;

	virtual double _get_playback_position() const override;
	virtual void _seek(double p_time) override;
};

};
