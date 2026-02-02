#pragma once

#include "entt/entt.hpp"
#include "game.h"
#include <cmath>
#include <math.h>

template <typename T>
struct ContinuousAnimation {
	T* to_animate;
	T (*curve)(T current_value, double delta_time, double time_elapsed);

	void update(double time_elapsed) {
		*to_animate = curve(*to_animate, delta_time, time_elapsed); // Need to be careful of segfaults when using this
	}
};

struct Updater : entt::type_list<void(double time_elapsed)> {
	template<typename Base>
	struct type : Base {
		void update(double time_elapsed) {
			return entt::poly_call<0>(*this, time_elapsed);
		}
	};

	template<typename Type>
	using impl = entt::value_list<&Type::update>;
};

struct Animation {
	double duration;
	double delay;
	double time_elapsed;
	entt::poly<Updater> updater;

	bool is_finished() const;
	void progress_time();
};

extern std::vector<Animation> animations;

void play_animation(double duration, double delay, entt::poly<Updater> updater);
void update_generic_animation();

template<typename T>
T linear_curve(T rate, T current_value, double delta_time, double time_elapsed) {
	return current_value + rate * delta_time;
}

template<typename T>
T sinusoid_curve(T amplitude, T frequency, T phase, T current_value, double delta_time, double time_elapsed) {
	return current_value + amplitude * std::sin(frequency * M_PI * 2.0 * time_elapsed + phase);
}
