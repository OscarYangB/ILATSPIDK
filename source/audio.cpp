#include "audio.h"
#include "SDL3/SDL_audio.h"
#include "SDL3/SDL_iostream.h"
#include "SDL3/SDL_stdinc.h"
#include "audio_data.h"

SDL_AudioStream* audio_stream = nullptr;
std::vector<PlayingAudio> playing_audio;

void init_audio() {
	SDL_AudioSpec spec;
	spec.channels = 2;
	spec.format = SDL_AUDIO_S32LE; // All played audio needs to be in this format
	spec.freq = 44100;
	audio_stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, audio_stream_callback, nullptr);
	SDL_ResumeAudioStreamDevice(audio_stream);
}

void audio_stream_callback(void* userdata, SDL_AudioStream* stream, int additional_amount, int total_amount) {
	for (int i = 0; i < playing_audio.size(); i++) {
		u32 samples_to_play = SDL_min(total_amount, playing_audio[i].remaining_samples());
		SDL_PutAudioStreamData(stream, playing_audio[i].data + playing_audio[i].position, samples_to_play);
		playing_audio[i].position += samples_to_play;
	}

	std::erase_if(playing_audio, [](const PlayingAudio& audio) {
		if (audio.position == audio.length) {
			SDL_free(audio.data);
			return true;
		}
		return false;
	});
}

// Could do reference counted loading. Maybe a component that keeps a file in memory if an entity needs it during its lifetime.
// Definitely should not load file if it is already loaded. Should just read from memory.
void play_audio(AudioAsset audio_asset) {
	u8* data;
	u32 length;

	u8 audio_index = static_cast<u8>(audio_asset);
	SDL_IOStream* input_stream = SDL_IOFromConstMem(audio_data[audio_index], audio_sizes[audio_index]);
	SDL_AudioSpec spec;
	SDL_LoadWAV_IO(input_stream, true, &spec, &data, &length);

	playing_audio.push_back(PlayingAudio{data, length, 0});
}
