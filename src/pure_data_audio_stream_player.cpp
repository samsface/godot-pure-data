#include "pure_data_audio_stream_player.h"

#include <godot_cpp/core/class_db.hpp>

#define PD_INTERNAL

#include "z_libpd.h"

using namespace godot;

void AudioStreamPureData::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("set_instance", "instance"), &AudioStreamPureData::set_instance);
	ClassDB::bind_method(D_METHOD("get_instance"), &AudioStreamPureData::get_instance);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "instance", PROPERTY_HINT_NODE_TYPE, "PureDataInstance"), "set_instance", "get_instance");
}

AudioStreamPureData::AudioStreamPureData() {
}


AudioStreamPureData::~AudioStreamPureData()
{
}

Ref<AudioStreamPlayback> AudioStreamPureData::_instantiate_playback() const {
	Ref<AudioStreamPureDataPlayback> playback;
	playback.instantiate();
	playback->audiostream = this;
	return playback;
}

String AudioStreamPureData::_get_stream_name() const {
	return "PureData";
}

double AudioStreamPureData::_get_length() const {
	return 0;
}

bool AudioStreamPureData::_is_monophonic() const {
	return true;
}

void AudioStreamPureData::set_instance(PureDataInstance *instance) {
	pd_instance = instance;
}

PureDataInstance* AudioStreamPureData::get_instance() const {
	return pd_instance;
}

////////////////

AudioStreamPureDataPlayback::AudioStreamPureDataPlayback() {
	audiostream = nullptr;
	active = false;
	mixed = 0;
}

float AudioStreamPureDataPlayback::_get_stream_sampling_rate() const {
	if (audiostream && audiostream->get_instance()) {
		return audiostream->get_instance()->get_sample_rate();
	}
	return 0;
}

int AudioStreamPureDataPlayback::_mix_resampled(AudioFrame *p_buffer, int p_frames) {
	if (!active || !audiostream || !audiostream->get_instance()) {
		return 0;
	}

	int ticks = p_frames / libpd_blocksize();

	if(::libpd_process_float(ticks, inbuf_, outbuf_) != 0) {
		return 0;
	}

	int channel_count = audiostream->get_instance()->get_out_channel_count();
	for(int i = 0; i < p_frames; i++) {
		int frame_index = i * channel_count;
		Vector2 v = Vector2(outbuf_[frame_index], outbuf_[frame_index + 1])
				.clamp(Vector2(-1, -1), Vector2(1, 1));

		AudioFrame f;
		f.left = v.x;
		f.right = v.y;

		p_buffer[i] = f;
	}

	mixed += p_frames / _get_stream_sampling_rate();
	return p_frames;
}

void AudioStreamPureDataPlayback::_start(double p_from_pos) {
	if (mixed == 0.0) {
		begin_resample();
	}
	active = true;
	mixed = 0.0;
}

void AudioStreamPureDataPlayback::_stop() {
	active = false;
}

bool AudioStreamPureDataPlayback::_is_playing() const {
	return active;
}

int AudioStreamPureDataPlayback::_get_loop_count() const {
	return 0;
}

double AudioStreamPureDataPlayback::_get_playback_position() const {
	return mixed;
}

void AudioStreamPureDataPlayback::_seek(double p_time) {
	//no seek possible
}

void AudioStreamPureDataPlayback::_bind_methods() {
}
