#include "game_audio.h"
#include "SDL3/SDL_audio.h"
#include "SDL3/SDL_iostream.h"
#include "game_assets.h"
#include "definitions.h"

SDL_AudioStream* audio_stream = nullptr;

void init_audio() {
}

void audio_stream_callback(void* userdata, SDL_AudioStream* stream, int additional_amount, int total_amount) {

}

void play_audio(AudioAsset audio_asset) {
	SDL_AudioSpec spec;

	u8* data;
	u32 length;

	u8 audio_index = static_cast<u8>(audio_asset);
	SDL_IOStream* input_stream = SDL_IOFromConstMem(audio_data[audio_index], audio_sizes[audio_index]);
	SDL_LoadWAV_IO(input_stream, true, &spec, &data, &length);

	audio_stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, nullptr, nullptr);

	SDL_free(data);

	SDL_PutAudioStreamData(audio_stream, data, length);

	SDL_ResumeAudioStreamDevice(audio_stream);
}
