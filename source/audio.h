#pragma once
#include "definitions.h"

class SDL_AudioStream;
enum class AudioAsset;

struct PlayingAudio {
	u8* data{};
	u32 length{};
	u32 position{};

	inline u32 remaining_samples() { return length - position; }
};

void init_audio();
void play_audio(AudioAsset audio_asset);
