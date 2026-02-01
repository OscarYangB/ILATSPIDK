#pragma once
#include "vector2.h"
#include "entt/entt.hpp"
#include "game.h"
#include "render.h"

struct Box {
	Vector2 top_left;
	Vector2 bottom_right;

	inline float x0() { return top_left.x; };
	inline float x1() { return bottom_right.x; };
	inline float y0() { return bottom_right.y; };
	inline float y1() { return top_left.y; };
};

struct BoxColliderComponent {
	Box box;
};

bool is_colliding(const Vector2& first_position, const Vector2& second_position, const BoxColliderComponent& first_collider, const BoxColliderComponent& second_collider);
bool line_segments_intersect(const Vector2& start_1, const Vector2& end_1, const Vector2& start_2, const Vector2& end_2);

template<typename BoxComponent>
void raytest(std::vector<BoxComponent*>& out, const Vector2& position, const Vector2& direction, float length) {
	auto view = ecs.view<BoxComponent, TransformComponent>();

	//debug_draw(position, position + direction * length);

	for (auto [entity, component, transform] : view.each()) {
		Vector2 bottom_left = Vector2{component.box.x0(), component.box.y0()} + transform.position;
		Vector2 bottom_right = Vector2{component.box.x1(), component.box.y0()} + transform.position;
		Vector2 top_left = Vector2{component.box.x0(), component.box.y1()} + transform.position;
		Vector2 top_right = Vector2{component.box.x1(), component.box.y1()} + transform.position;

		/*
		debug_draw(top_left, top_right);
		debug_draw(top_right, bottom_right);
		debug_draw(bottom_right, bottom_left);
		debug_draw(bottom_left, top_left);
		*/

		if (line_segments_intersect(position, position + direction * length, top_left, top_right) ||
			line_segments_intersect(position, position + direction * length, top_right, bottom_right) ||
			line_segments_intersect(position, position + direction * length, bottom_right, bottom_left) ||
			line_segments_intersect(position, position + direction * length, bottom_left, top_left)) {
			out.push_back(&component);
		}
	}
}
