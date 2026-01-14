#pragma once

class SDL_AudioStream;
enum class AudioAsset;

extern SDL_AudioStream* audio_stream;

void init_audio();
void play_audio(AudioAsset audio_asset);
