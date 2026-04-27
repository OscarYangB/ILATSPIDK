#include "audio.h"
#include "audio_data.h"
#include <SDL3/SDL.h>
#include <vector>

static SDL_AudioStream* audio_stream = nullptr;
static std::vector<PlayingAudio> playing_audio{};
static std::vector<i16> audio_buffer{};

constexpr int SAMPLE_RATE = 44100;

void audio_stream_callback(void* userdata, SDL_AudioStream* stream, int additional_amount, int total_amount) {
	u32 bytes_requested = total_amount + additional_amount;
	u32 samples_requested = bytes_requested / 2;
	if (audio_buffer.size() < samples_requested) {
		audio_buffer.resize(samples_requested);
	}
	for (int i = 0; i < audio_buffer.size(); i++) {
		audio_buffer[i] = 0;
	}

	for (PlayingAudio& audio : playing_audio) {
		audio.time_position = audio.position;
		u32 samples_to_play = SDL_min(samples_requested, audio.remaining_samples());
		u32 buffer_index = 0;
		while (buffer_index < samples_to_play) {
			i16 sample = static_cast<i16>(audio.data[audio.position++]) * audio.volume;
			audio_buffer[buffer_index++] += sample * (1.f - audio.pan);
			audio_buffer[buffer_index++] += sample * audio.pan;
		}
	}

	SDL_PutAudioStreamData(stream, audio_buffer.data(), bytes_requested);

	std::erase_if(playing_audio, [](const PlayingAudio& audio) {
		return audio.position == audio.length;
	});
}

double get_audio_time(AudioFile audio_file) {
	for (int i = 0; i < playing_audio.size(); i++) {
		if (playing_audio[i].file == audio_file) {
			return static_cast<double>(playing_audio[i].time_position) / static_cast<double>(SAMPLE_RATE);
		}
	}

	return 0.0;
}

void init_audio() {
	SDL_AudioSpec spec;
	spec.channels = 2;
	spec.format = SDL_AUDIO_S16;
	spec.freq = SAMPLE_RATE;
	audio_stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, audio_stream_callback, nullptr);
	SDL_ResumeAudioStreamDevice(audio_stream);
}

void play_audio(AudioFile audio_file, float volume, float pan) {
	// Files are signed 8-bit mono PCM
	u8 audio_index = static_cast<u8>(audio_file);
	playing_audio.push_back(PlayingAudio{audio_file, reinterpret_cast<const i8*>(audio_data[audio_index]), static_cast<u32>(audio_sizes[audio_index]), volume, pan});
}

void stop_audio(AudioFile audio_file) {
	std::erase_if(playing_audio, [audio_file](const PlayingAudio& audio) {return audio.file == audio_file; });
}
