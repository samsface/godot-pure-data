#include "pure_data_audio_stream_player.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#define PD_INTERNAL

#include "z_libpd.h"

using namespace godot;

void PureDataAudioStream::_bind_methods()
{
}

PureDataAudioStream::PureDataAudioStream() {
}


PureDataAudioStream::~PureDataAudioStream()
{
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
