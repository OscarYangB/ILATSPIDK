#pragma once

#include "game.h"

struct Animation;

struct Updater : entt::type_list<void(const Animation& animation)> {
	template<typename Base>
	struct type : Base {
		void update(const Animation& animation) {
			return entt::poly_call<0>(*this, animation);
		}
	};

	template<typename Type>
	using impl = entt::value_list<&Type::update>;
};

struct Animation {
	double duration;
	double delay;
	entt::poly<Updater> updater;

	double time_elapsed = 0.0;

	bool is_finished() const;
	void progress_time();
	double get_animation_time() const;
};

template <typename T>
using Curve = T (*)(const Animation& animation, T current_value);

template <typename MemberType, typename ComponentType>
struct ComponentAnimation {
	entt::entity entity;
	Curve<MemberType> curve;
	MemberType ComponentType::* member;

	void update(const Animation& animation) {
		if (!ecs.valid(entity)) {
			return;
		}

		ComponentType* component = ecs.try_get<ComponentType>(entity);
		if (component == nullptr) {
			return;
		}

		component->*member = curve(animation, component->*member);
	}
};

template <typename T>
struct PointerAnimation {
	/* When using this, make sure this pointer remains valid throughout the animation
	   (ie. it's a global variable or the owner stops the animation when it dies) */
	T* to_animate;
	Curve<T> curve;

	void update(const Animation& animation) {
		*to_animate = curve(animation, *to_animate);
	}
};

extern std::vector<Animation> animations;

void play_animation(double duration, double delay, entt::poly<Updater> updater);
void update_generic_animation();

template <typename MemberType, typename ComponentType, typename CurveType>
void play_animation(double duration, double delay, MemberType ComponentType::* member, entt::entity entity, CurveType curve) {
	animations.push_back(Animation{duration, delay, ComponentAnimation<MemberType, ComponentType>{entity, curve, member}});
}

template <typename T, typename CurveType>
void play_animation(double duration, double delay, T* to_animate, CurveType curve) {
	animations.push_back(Animation{duration, delay, PointerAnimation<T>{to_animate, curve}});
}

template<typename T>
T linear_curve(T rate, const Animation& animation, T current_value) {
	return current_value + rate * delta_time;
}

template<typename T>
T sinusoid_curve(T amplitude, T frequency, T phase, const Animation& animation, T current_value) {
	T f = frequency * M_PI * 2.0;
	return current_value + std::cos(f * animation.get_animation_time() + phase) * amplitude * f * delta_time; // Major  determinism issues when it lags
}
