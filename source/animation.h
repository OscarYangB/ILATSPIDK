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
	u64 id{};

	double time_elapsed = 0.0;
	bool should_remove = false;

	bool is_finished() const;
	void progress_time();
	double get_animation_time() const;
	double get_delta_time() const;
};

template <typename T>
using Curve = T (*)(Animation& animation, T current_value);

template <typename MemberType, typename ComponentType, typename RemoveFunctionType>
struct ComponentAnimation {
	entt::entity entity{};
	Curve<MemberType> curve{};
	MemberType ComponentType::* member{};
	RemoveFunctionType remove_if{};

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

		if (remove_if != nullptr && remove_if(*component)) {
			animation.should_remove = true;
			return;
		}

		component->*member = curve(animation, component->*member);
	}
};

template <typename T>
struct PointerAnimation {
	/* When using this, make sure this pointer remains valid throughout the animation
	   (ie. it's a global variable or the owner stops the animation when it dies) */
	T* to_animate{};
	Curve<T> curve{};

	void update(Animation& animation) {
		*to_animate = curve(animation, *to_animate);
	}
};

extern std::vector<Animation> animations;
u64 register_animation(Animation animation);

u64 play_animation(double duration, double delay, entt::poly<Updater> updater);
void update_generic_animation();
void stop_animation(u64 id);
bool animation_playing(u64 id);

template <typename MemberType, typename ComponentType, typename CurveType, typename RemoveFunctionType>
u64 play_animation(double duration, double delay, MemberType ComponentType::* member, entt::entity entity, CurveType curve, RemoveFunctionType remove_if = nullptr) {
	return register_animation(Animation{duration, delay, ComponentAnimation<MemberType, ComponentType, RemoveFunctionType>{entity, curve, member, remove_if}});
}

template <typename T, typename CurveType>
u64 play_animation(double duration, double delay, T* to_animate, CurveType curve) {
	return register_animation(Animation{duration, delay, PointerAnimation<T>{to_animate, curve}});
}

template <typename T>
T linear_curve(T rate, const Animation& animation, T current_value) {
	return current_value + rate * animation.get_delta_time();
}

template <typename T, typename Curve>
T evaluate_curve_change(const Animation& animation, T current_value, Curve&& curve) {
	T value_at_previous_time = curve(animation.get_animation_time());
	T value_at_current_time = curve(animation.get_animation_time() + animation.get_delta_time());
	return current_value + (value_at_current_time - value_at_previous_time);
}

template <typename T>
T sinusoid_curve(float amplitude, float frequency, float phase, const Animation& animation, T current_value) {
	auto sinusoid_function = [amplitude, frequency, phase] (double time) -> double {
		double w = frequency * M_PI * 2.0;
		return amplitude * std::sin(w * time + phase);
	};
	return evaluate_curve_change(animation, static_cast<double>(current_value), sinusoid_function);
}

template <typename T>
T linear_increment(double rate, T multiplier, const Animation& animation, T current_value) {
	auto increment_function = [rate, multiplier] (double time) -> T {
		return std::floor(time * rate) * multiplier;
	};
	return evaluate_curve_change(animation, current_value, increment_function);
}
