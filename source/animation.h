#pragma once

#include "game.h"
#include "definitions.h"

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
};

template <typename T>
using Curve = T (*)(Animation& animation, T starting_value);

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

template <typename T>
struct PointerAnimation {
	/* When using this, make sure this pointer remains valid throughout the animation
	   (ie. it's a global variable or the owner stops the animation when it dies) */
	T* to_animate{};
	Curve<T> curve{};
	T starting_value;

	void update(Animation& animation) {
		*to_animate = curve(animation, starting_value);
	}
};

extern std::vector<Animation> animations;
u64 register_animation(Animation animation);

u64 play_animation(double duration, double delay, entt::poly<Updater> updater);
void update_generic_animation();
void stop_animation(u64 id);
bool animation_playing(u64 id);

template <typename MemberType, typename ComponentType, typename CurveType>
u64 play_animation(double duration, double delay, MemberType ComponentType::* member, entt::entity entity, CurveType curve) {
	MemberType starting_value = ecs.get<ComponentType>(entity).*member;
	return register_animation(Animation{duration, delay, ComponentAnimation<MemberType, ComponentType>{entity, curve, member, starting_value}});
}

template <typename T, typename CurveType>
u64 play_animation(double duration, double delay, T* to_animate, CurveType curve) {
	return register_animation(Animation{duration, delay, PointerAnimation<T>{to_animate, curve, *to_animate}});
}

template <typename T>
T linear_curve(T rate, Animation& animation, T starting_value) {
	return starting_value + rate * animation.get_animation_time();
}

template <typename T>
T smooth_curve(T target_value, Animation& animation, T starting_value) {
	// Smooth interpolate between starting_value and target_value using animation completion rate
	return starting_value;
}


template <typename T>
T sinusoid_curve(float amplitude, float frequency, float phase, Animation& animation, T starting_value) {
	double w = frequency * M_PI * 2.0;
	return starting_value + amplitude * std::sin(w * animation.get_animation_time() + phase);
}

template <typename T>
T linear_increment(double rate, T multiplier, Animation& animation, T starting_value) {
	return starting_value + std::floor(animation.get_animation_time() * rate) * multiplier;
}
