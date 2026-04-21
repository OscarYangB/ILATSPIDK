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

template<typename ComponentType, typename FunctionType>
std::tuple<entt::entity, ComponentType&> find_component(FunctionType&& function) {
	auto view = ecs.view<ComponentType>();
	for (auto [entity, component] : view.each()) {
		if (function(component)) {
			return {entity, component};
		}
	}

	throw std::runtime_error("Entity not found");
}

template<typename... ComponentTypes>
void destroy_entities() {
	auto view = ecs.view<ComponentTypes...>();
	for (entt::entity entity : view) {
		ecs.destroy(entity);
	}
}

template<typename... ComponentTypes>
auto get_first_component() {
	auto view = ecs.view<ComponentTypes...>();
	assert(++view.begin() == view.end());
	return *view.each().begin();
}
