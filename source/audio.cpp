#include "audio.h"
#include "audio_data.h"
#include <SDL3/SDL.h>
#include <cassert>
#include <vector>
#include "dsp.h"

static SDL_AudioStream* audio_stream = nullptr;
static std::vector<PlayingAudio> playing_audio{};
static std::vector<float> audio_buffer{};
static std::vector<float> reverb_buffer{};

constexpr u32 SAMPLE_RATE = 44100;

Reverb reverb{};
bool do_reverb;
float reverb_left{};
float reverb_right{};

void audio_stream_callback(void* userdata, SDL_AudioStream* stream, int additional_amount, int total_amount) {
	u32 bytes_requested = total_amount + additional_amount;
	u32 samples_requested = bytes_requested / 4;
	if (audio_buffer.size() < samples_requested) {
		audio_buffer.resize(samples_requested);
		reverb_buffer.resize(samples_requested);
	}
	for (int i = 0; i < samples_requested; i++) {
		audio_buffer[i] = 0;
		reverb_buffer[i] = 0;
	}

	for (PlayingAudio& audio : playing_audio) {
		audio.time_position = audio.position;
		u32 samples_to_play = SDL_min(samples_requested, audio.remaining_samples());
		u32 buffer_index = 0;
		while (buffer_index < samples_to_play) {
			float sample = (static_cast<float>(audio.data[audio.position++]) / std::numeric_limits<i8>::max()) * audio.volume;
			if (audio.fade_duration > 0) {
				float progress = static_cast<double>(audio.fade_progress) / static_cast<double>(audio.fade_duration);
				sample *= audio.fade == Fade::IN ? progress : 1.f - progress;
				audio.fade_progress++;
				if (audio.fade == Fade::IN && audio.fade_progress > audio.fade_duration) {
					audio.fade_progress = 0;
					audio.fade_duration = 0;
				}
			}
			reverb_buffer[buffer_index] += sample * (1.f - audio.pan) * audio.reverb;
			audio_buffer[buffer_index++] += sample * (1.f - audio.pan);
			reverb_buffer[buffer_index] += sample * audio.pan * audio.reverb;
			audio_buffer[buffer_index++] += sample * audio.pan;
		}
	}

	for (int i = 0; i < samples_requested; i++) {
		if (do_reverb) {
			reverb.process(reverb_buffer[i], reverb_left, reverb_right);
		}
		do_reverb = !do_reverb;
		audio_buffer[i] += reverb_left;
		audio_buffer[++i] += reverb_right;
	}

	SDL_PutAudioStreamData(stream, audio_buffer.data(), bytes_requested);

	std::erase_if(playing_audio, [](const PlayingAudio& audio) {
		return audio.position == audio.length || (audio.fade == Fade::OUT && audio.fade_progress > audio.fade_duration);
	});
}

double get_audio_time(AudioFile audio_file) {
	SDL_LockAudioStream(audio_stream);
	for (int i = 0; i < playing_audio.size(); i++) {
		if (playing_audio[i].file == audio_file) {
			SDL_UnlockAudioStream(audio_stream);
			return static_cast<double>(playing_audio[i].time_position) / static_cast<double>(SAMPLE_RATE);
		}
	}
	SDL_UnlockAudioStream(audio_stream);
	return 0.0;
}

void init_audio() {
	SDL_AudioSpec spec;
	spec.channels = 2;
	spec.format = SDL_AUDIO_F32LE;
	spec.freq = SAMPLE_RATE;
	audio_stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, audio_stream_callback, nullptr);
	SDL_ResumeAudioStreamDevice(audio_stream);
}

void play_audio(AudioFile audio_file, float volume, float pan, float reverb, float fade_duration) {
	SDL_LockAudioStream(audio_stream);
	// Mono signed 8-bit PCM
	u8 audio_index = static_cast<u8>(audio_file);
	u32 fade_samples = fade_duration * SAMPLE_RATE;
	playing_audio.emplace_back(audio_file, reinterpret_cast<const i8*>(audio_data[audio_index]), static_cast<u32>(audio_sizes[audio_index]), volume, pan, reverb, fade_samples);
	SDL_UnlockAudioStream(audio_stream);
}

void stop_audio(AudioFile audio_file, float duration) {
	SDL_LockAudioStream(audio_stream);
	if (duration == 0.f) {
		std::erase_if(playing_audio, [audio_file](const PlayingAudio& audio) { return audio.file == audio_file; });
	} else {
		for (PlayingAudio& audio : playing_audio) {
			if (audio.file == audio_file) {
				audio.fade_progress = 0;
				audio.fade_duration = duration * SAMPLE_RATE;
				audio.fade = Fade::OUT;
			}
		}
	}
	SDL_UnlockAudioStream(audio_stream);
}
