#include "audio.h"
#include "audio_data.h"
#include <SDL3/SDL.h>
#include <vector>

static SDL_AudioStream* audio_stream = nullptr;
static std::vector<PlayingAudio> playing_audio{};

constexpr int SAMPLE_RATE = 44100;

void audio_stream_callback(void* userdata, SDL_AudioStream* stream, int additional_amount, int total_amount) {
	for (PlayingAudio& audio : playing_audio) {
		u32 samples_to_play = SDL_min(total_amount + additional_amount, audio.remaining_samples());
		SDL_PutAudioStreamData(stream, audio.data + audio.position, samples_to_play);
		audio.time_position = audio.position;
		audio.position += samples_to_play;
	}

	std::erase_if(playing_audio, [](const PlayingAudio& audio) {
		if (audio.position == audio.length) {
			SDL_free(audio.data);
			return true;
		}
		return false;
	});
}

double get_audio_time(AudioFile audio_file) {
	for (int i = 0; i < playing_audio.size(); i++) {
		if (playing_audio[i].file == audio_file) {
			return (static_cast<double>(playing_audio[i].time_position) * (8.0 / 32.0) * 0.5) / static_cast<double>(SAMPLE_RATE);
		}
	}

	return 0.0;
}

void init_audio() {
	SDL_AudioSpec spec;
	spec.channels = 2;
	spec.format = SDL_AUDIO_S32LE; // All played audio needs to be in this format
	spec.freq = SAMPLE_RATE;
	audio_stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, audio_stream_callback, nullptr);
	SDL_ResumeAudioStreamDevice(audio_stream);
}

// Could do reference counted loading. Maybe a component that keeps a file in memory if an entity needs it during its lifetime.
// Definitely should not load file if it is already loaded. Should just read from memory.
void play_audio(AudioFile audio_file) {
	u8* data;
	u32 length;

	u8 audio_index = static_cast<u8>(audio_file);
	SDL_IOStream* input_stream = SDL_IOFromConstMem(audio_data[audio_index], audio_sizes[audio_index]);
	SDL_AudioSpec spec;
	SDL_LoadWAV_IO(input_stream, true, &spec, &data, &length);

	playing_audio.push_back(PlayingAudio{audio_file, data, length, 0});
}

void stop_audio(AudioFile audio_file) {
	std::erase_if(playing_audio, [audio_file](const PlayingAudio& audio) {return audio.file == audio_file; });
}
