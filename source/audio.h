#pragma once
#include "definitions.h"

class SDL_AudioStream;
enum class AudioFile;

struct PlayingAudio {
	AudioFile file;
	u8* data{};
	u32 length{};
	u32 position{};
	u32 time_position{};

	inline u32 remaining_samples() { return length - position; }
};

void init_audio();
void play_audio(AudioFile audio_file);
void stop_audio(AudioFile audio_file);
double get_audio_time(AudioFile audio_file);
