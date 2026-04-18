#pragma once

#include "game.h"
#include "definitions.h"
#include <functional>

struct Animation;

struct Updater : entt::type_list<void(Animation& animation)> {
	template<typename Base>
	struct type : Base {
		void update(Animation& animation) {
			return entt::poly_call<0>(*this, animation);
		}
	};

	template<typename Type>
	using impl = entt::value_list<&Type::update>;
};

struct Animation {
	double duration{};
	double delay{};
	entt::poly<Updater> updater{};
	u32 id{};
	u64 time_started_ms{};

	bool should_remove = false;

	bool is_finished() const;
	double get_animation_time() const;
	double get_progress() const;
};

template <typename T>
using Curve = std::function<T(Animation& animation, T starting_value)>;

template <typename MemberType, typename ComponentType>
struct ComponentAnimation {
	entt::entity entity{};
	Curve<MemberType> curve{};
	MemberType ComponentType::* member{};
	MemberType starting_value;

	void update(Animation& animation) {
		if (!ecs.valid(entity)) {
			animation.should_remove = true;
			return;
		}

		ComponentType* component = ecs.try_get<ComponentType>(entity);
		if (component == nullptr) {
			animation.should_remove = true;
			return;
		}

		component->*member = curve(animation, starting_value);
	}
};

extern std::vector<Animation> animations;
u32 register_animation(Animation animation);

void update_generic_animation();
void stop_animation(u64 id);
bool animation_playing(u64 id);
u32 start_animation_group();
void end_animation_group();

template <typename MemberType, typename ComponentType, typename CurveType>
u64 play_animation(double duration, double delay, MemberType ComponentType::* member, entt::entity entity, CurveType curve) {
	MemberType starting_value = ecs.get<ComponentType>(entity).*member;
	return register_animation(Animation{duration, delay, ComponentAnimation<MemberType, ComponentType>{entity, curve, member, starting_value}});
}

template <typename T>
T linear_curve(T target_value, Animation& animation, T starting_value) {
	if (animation.is_finished()) return target_value;
	float rate = (target_value - starting_value) / animation.duration;
	return starting_value + rate * animation.get_animation_time();
}

template <typename T>
T smooth_curve(T target_value, Animation& animation, T starting_value) {
	if (animation.is_finished()) return target_value;
	double progress = animation.get_progress();
	// Natalya Tatarchuk (2003). "Advanced Real-Time Shader Techniques". AMD. p. 94.
	double value = progress * progress * (3.0 - 2.0 * progress);
	return starting_value + value * (target_value - starting_value);
}

template <typename T>
T fast_start_curve(T target_value, Animation& animation, T starting_value) {
	if (animation.is_finished()) return target_value;
	double progress = animation.get_progress();
	double value = std::pow(progress, 0.3);
	return starting_value + value * (target_value - starting_value);
}

template <typename T>
T fast_end_curve(T target_value, Animation& animation, T starting_value) {
	if (animation.is_finished()) return target_value;
	double progress = animation.get_progress();
	double value = progress * progress * progress;
	return starting_value + value * (target_value - starting_value);
}

template <typename T>
T sinusoid_curve(double amplitude, double frequency, double phase, Animation& animation, T starting_value) {
	const double w = frequency * M_PI * 2.0;
	return starting_value + amplitude * std::cos(w * animation.get_animation_time() + phase);
}

template <typename T>
T linear_increment(T target_value, T multiplier, Animation& animation, T starting_value) {
	if (animation.is_finished()) return target_value;
	float rate = (target_value - starting_value) / animation.duration;
	return starting_value + std::floor(animation.get_animation_time() * rate) * multiplier;
}
