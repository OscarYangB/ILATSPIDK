#pragma once
#include "definitions.h"

class SDL_AudioStream;
enum class AudioFile;

enum class Fade {
	IN,
	OUT
};

struct PlayingAudio {
	AudioFile file;
	const i8* data{};
	u32 length{};
	float volume = 1.f;
	float pan = 0.5f;
	float reverb = 0.f;
	u32 fade_duration{};
	u32 fade_progress = 0;
	Fade fade = Fade::IN;
	u32 position{};
	u32 time_position{};

	inline u32 remaining_samples() { return length - position; }
};

void init_audio();
void play_audio(AudioFile audio_file, float volume, float pan = 0.5f, float reverb = 0.f, float fade_duration = 0.f);
void stop_audio(AudioFile audio_file, float duration = 0.f);
double get_audio_time(AudioFile audio_file);
