#pragma once
#include "definitions.h"
#include <vector>

class SDL_AudioStream;
enum class AudioAsset;

struct PlayingAudio {
	u8* data;
	u32 length;
	u32 position;

	inline u32 remaining_samples() { return length - position; }
};

extern SDL_AudioStream* audio_stream;
extern std::vector<PlayingAudio> playing_audio;

void init_audio();
void audio_stream_callback(void* userdata, SDL_AudioStream* stream, int additional_amount, int total_amount);
void play_audio(AudioAsset audio_asset);
