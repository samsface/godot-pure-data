#pragma once

#include "pure_data_instance.h"
#include <godot_cpp/classes/audio_effect.hpp>
#include <godot_cpp/classes/audio_effect_instance.hpp>

namespace godot {

class AudioEffectPureData;

class AudioEffectPureDataInstance : public AudioEffectInstance {
	GDCLASS(AudioEffectPureDataInstance, AudioEffectInstance);

	friend class AudioEffectPureData;
	Ref<AudioEffectPureData> base;

private:
	std::array<float, BUFFER_SIZE> inbuf_;
	std::array<float, BUFFER_SIZE> outbuf_;

	bool processed = false;

protected:
	static void _bind_methods() {};

public:
	virtual void _process(const AudioFrame *p_src_frames, AudioFrame *p_dst_frames, int32_t p_frame_count);
	virtual bool _process_silence() const override;
};

class AudioEffectPureData : public AudioEffect {
	GDCLASS(AudioEffectPureData, AudioEffect);

	friend class AudioEffectPureDataInstance;
	PureDataInstance *pd_instance = nullptr;

protected:
	static void _bind_methods();

public:
	virtual Ref<AudioEffectInstance> _instantiate() override;

	void set_instance(PureDataInstance *instance);
	PureDataInstance *get_instance() const;
};

}; //namespace godot
