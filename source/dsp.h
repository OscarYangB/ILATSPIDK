#pragma once

#include "definitions.h"
#include <algorithm>
#include <cmath>
#include <vector>
#include <cassert>

using SampleType = float;
constexpr u32 REVERB_SAMPLE_RATE = 22050;

struct Delay {
	double feedback = 0.0;
	std::vector<SampleType> buffer{};
	u32 write_index = 0;
	SampleType low_pass_state{};
	float read_offset = 0;
	float read_speed = 1.f;

	inline Delay() {};

	inline Delay(double feedback, double length) {
		this->feedback = feedback;
		buffer.resize(REVERB_SAMPLE_RATE * length);
	}

	inline SampleType get_current_sample() {
		float read_index = write_index + read_offset;
		u32 read_index_low = std::floor(read_index);
		u32 read_index_high = std::ceil(read_index);
		float read_interpolation = read_index_low == read_index_high ? 0.f : (read_index - read_index_low) / (read_index_high - read_index_low);
		if (read_index_high >= buffer.size()) {
			read_index_high -= buffer.size();
		}
		if (read_index_low >= buffer.size()) {
			read_index_low -= buffer.size();
		}
		return buffer[read_index_low] * (1.f - read_interpolation) + buffer[read_index_high] * read_interpolation;
	}

	inline SampleType process(SampleType sample) {
		SampleType result = get_current_sample();

		SampleType delayed_sample = sample + buffer[write_index] * feedback;
		static constexpr double CUTOFF = 12000.0;
		static constexpr double LOW_PASS_CONSTANT = (1.0 / REVERB_SAMPLE_RATE) / (1.0 / CUTOFF);
		low_pass_state += LOW_PASS_CONSTANT * (delayed_sample - low_pass_state);

		buffer[write_index] = low_pass_state;
		write_index++;
		if (write_index >= buffer.size()) {
			write_index = 0.f;
		}

		read_offset += read_speed - 1.f;
		read_offset = std::clamp(read_offset, 0.f, static_cast<float>(buffer.size()));

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
	u8 flip;

	inline Diffuser(std::array<double, CHANNELS> delay_lengths, u8 flip, std::array<u8, CHANNELS> order) {
		for (u8 i : order) {
			delays[i] = Delay(0.0, delay_lengths[i]);
		}
		this->flip = flip;
	}

	inline std::array<SampleType, CHANNELS> process(std::array<SampleType, CHANNELS> samples) {
		for (int i = 0; i < CHANNELS; i++) {
			samples[i] = delays[i].process(samples[i]);
			if (i % flip == 0) {
				samples[i] *= -1;
			}
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
	std::array<Delay, CHANNELS> delays = {Delay(0.85f, 0.088f), Delay(0.84f, 0.109f), Delay(0.83f, 0.69f), Delay(0.82f, 0.43f),
										  Delay(0.81f, 0.144f), Delay(0.70f, 0.226f), Delay(0.80f, 0.177f), Delay(0.60f, 0.300f)};
	Diffuser diffuser1 = Diffuser({0.002f, 0.007f, 0.011f, 0.012f, 0.013f, 0.017f, 0.018f, 0.020f}, 2, {7, 6, 1, 3, 4, 0, 2, 5});
	Diffuser diffuser2 = Diffuser({0.004f, 0.008f, 0.009f, 0.010f, 0.019f, 0.021f, 0.029f, 0.040f}, 3, {6, 7, 5, 4, 0, 1, 2, 3});
	Diffuser diffuser3 = Diffuser({0.003f, 0.015f, 0.030f, 0.039f, 0.041f, 0.055f, 0.067f, 0.080f}, 4, {1, 0, 2, 7, 4, 5, 6, 3});

	std::array<u32, CHANNELS> modulation_timers{};

	inline void process(SampleType sample, SampleType& left_out, SampleType& right_out) {
		std::array<SampleType, CHANNELS> samples = {sample, sample, sample, sample, sample, sample, sample, sample};
		samples = diffuser1.process(samples);
		samples = diffuser2.process(samples);
		samples = diffuser3.process(samples);

		for (int i = 0; i < delays.size(); i++) {
			const double frequency = M_PI * 2.0 * 3.0;
			const double period = 1 / frequency;
			const double time = (static_cast<double>(modulation_timers[i]) / REVERB_SAMPLE_RATE);
			modulation_timers[i]++;
			if (time > 1.0 / frequency) {
				modulation_timers[i] = 0;
			}
			delays[i].read_speed = 1.f + 0.002 * std::sin(frequency * time + i * 0.177);
		}

		static constexpr double multiplier = -1.f / CHANNELS;
		double sum = 0;
		for (Delay& delay : delays) {
			sum += delay.get_current_sample();
		}
		sum *= multiplier;

		for (int i = 0; i < CHANNELS; i++) {
			samples[i] = delays[i].process(samples[i] + sum);
		}

		left_out = samples[0];
		right_out = samples[1];
	}
};
