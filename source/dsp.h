#pragma once

#include "definitions.h"
#include <cmath>
#include <vector>

using SampleType = float;
constexpr u32 SAMPLE_RATE = 44100;

struct Delay {
	float feedback = 0.f;
	std::vector<SampleType> buffer{};
	u32 index = 0;

	inline Delay() {};

	inline Delay(float feedback, float length) {
		this->feedback = feedback;
		buffer.resize(SAMPLE_RATE * length);
	}

	inline SampleType process(SampleType sample) {
		SampleType result = buffer[index];
		buffer[index] = sample + buffer[index] * feedback;
		index++;
		index %= buffer.size();
		return result;
	}
};

static constexpr u8 CHANNELS = 8;
static constexpr SampleType HADAMARD[CHANNELS][CHANNELS] = {{1, 1, 1, 1, 1, 1, 1, 1},
															{1,-1, 1,-1, 1,-1, 1,-1},
															{1, 1,-1,-1, 1, 1,-1,-1},
															{1,-1,-1, 1, 1,-1,-1, 1},
															{1, 1, 1, 1,-1,-1,-1,-1},
															{1,-1, 1,-1,-1, 1,-1, 1},
															{1, 1,-1,-1,-1,-1, 1, 1},
															{1,-1,-1, 1,-1, 1, 1,-1}};

struct Diffuser {
	std::array<Delay, CHANNELS> delays{};

	inline Diffuser(std::array<float, CHANNELS> delay_lengths) {
		for (int i = 0; i < CHANNELS; i++) {
			delays[i] = Delay(0.f, delay_lengths[i]);
		}
	}

	inline std::array<SampleType, CHANNELS> process(std::array<SampleType, CHANNELS> samples) {
		for (int i = 0; i < CHANNELS; i++) {
			samples[i] = delays[i].process(samples[i]);
		}

		for (int i = 0; i < CHANNELS; i++) {
			for (int j = 0; j < CHANNELS; j++) {
				if (i != j) {
					samples[i] += samples[j] * HADAMARD[i][j];
				}
			}
			samples[i] /= std::sqrt(CHANNELS);
		}
		return samples;
	}
};

struct Reverb {
	std::array<Delay, CHANNELS> delays = {Delay(0.80f, 0.100f), Delay(0.81f, 0.110f), Delay(0.82f, 0.120f), Delay(0.83f, 0.140f),
										  Delay(0.84f, 0.150f), Delay(0.85f, 0.160f), Delay(0.86f, 0.180f), Delay(0.87f, 0.200f)};
	Diffuser diffuser1 = Diffuser({0.005f, 0.007f, 0.011f, 0.012f, 0.013f, 0.017f, 0.018f, 0.020f});
	Diffuser diffuser2 = Diffuser({0.005f, 0.010f, 0.030f, 0.039f, 0.041f, 0.055f, 0.067f, 0.080f});

	inline void process(SampleType sample, SampleType& left_out, SampleType& right_out) {
		std::array<SampleType, CHANNELS> samples = {sample, sample, sample, sample, sample, sample, sample, sample};
		samples = diffuser1.process(samples);
		samples = diffuser2.process(samples);

		static constexpr double multiplier = -1.f / CHANNELS;
		double sum = 0;
		for (Delay& delay : delays) {
			sum += delay.buffer[delay.index];
		}
		sum *= multiplier;

		for (int i = 0; i < CHANNELS; i++) {
			samples[i] = delays[i].process(samples[i] + sum);
		}

		left_out = samples[0];
		right_out = samples[1];
	}
};
