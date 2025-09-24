#include "pure_data_audio_effect.h"

#include <godot_cpp/core/class_db.hpp>

#define PD_INTERNAL

#include "z_libpd.h"

using namespace godot;

void AudioEffectPureDataInstance::_process(const AudioFrame *p_src_frames, AudioFrame *p_dst_frames, int32_t p_frame_count) {
	if (!base->pd_instance) {
		return;
	}

	int in_channel_count = base->pd_instance->get_in_channel_count();
	for (int i = 0; i < p_frame_count; i++) {
		int frame_index = i * in_channel_count;

		AudioFrame f = p_src_frames[i];

		inbuf_[frame_index] = f.left;
		inbuf_[frame_index + 1] = f.right;
	}

	int ticks = p_frame_count / libpd_blocksize();

	if (::libpd_process_float(ticks, inbuf_, outbuf_) != 0) {
		return;
	}

	int out_channel_count = base->pd_instance->get_out_channel_count();
	for (int i = 0; i < p_frame_count; i++) {
		int frame_index = i * out_channel_count;
		Vector2 v = Vector2(outbuf_[frame_index], outbuf_[frame_index + 1])
							.clamp(Vector2(-1, -1), Vector2(1, 1));

		AudioFrame f;
		f.left = v.x;
		f.right = v.y;

		p_dst_frames[i] = f;
	}
}

bool AudioEffectPureDataInstance::_process_silence() const {
	return true;
}

////////////////

void AudioEffectPureData::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_instance", "instance"), &AudioEffectPureData::set_instance);
	ClassDB::bind_method(D_METHOD("get_instance"), &AudioEffectPureData::get_instance);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "instance", PROPERTY_HINT_NODE_TYPE, "PureDataInstance"), "set_instance", "get_instance");
}

Ref<AudioEffectInstance> AudioEffectPureData::_instantiate() {
	Ref<AudioEffectPureDataInstance> instance;
	instance.instantiate();
	instance->base = Ref<AudioEffectPureData>(this);
	return instance;
}

void AudioEffectPureData::set_instance(PureDataInstance *instance) {
	pd_instance = instance;
}

PureDataInstance *AudioEffectPureData::get_instance() const {
	return pd_instance;
}
