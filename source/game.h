#pragma once

#include "entt/entt.hpp"

extern entt::registry ecs;
extern double delta_time;

template<typename T> // So that I can pass in a constructor with designated initializers without repeating the type
decltype(auto) add_component(entt::entity entity, T&& component) {
	if constexpr (std::is_empty_v<T>) {
		ecs.emplace<T>(entity);
	} else {
		return ecs.emplace<T>(entity, std::forward<T>(component));
	}
}
