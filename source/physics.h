#pragma once
#include "vector2.h"
#include "entt/entt.hpp"
#include "game.h"
#include "render.h"

struct BoxColliderComponent {
	Box box{};
};

bool is_colliding(const Vector2& first_position, const Vector2& second_position, const BoxColliderComponent& first_collider, const BoxColliderComponent& second_collider);
bool line_segments_intersect(const Vector2& start_1, const Vector2& end_1, const Vector2& start_2, const Vector2& end_2);
bool point_in_box(const Box& box, const Vector2& vector);

template<typename BoxComponent>
void raytest(std::vector<BoxComponent*>& out, const Vector2& position, const Vector2& direction, float length) {
	Vector2 line_end = position + direction * length;
	auto view = ecs.view<BoxComponent, TransformComponent>();

	//debug_draw(position, line_end);

	for (auto [entity, component, transform] : view.each()) {
		Box offset_box = component.box + transform.position;
		Vector2 bottom_left = {offset_box.left_top.x, offset_box.right_bottom.y};
		Vector2 top_right = {offset_box.right_bottom.x, offset_box.left_top.y};

		/*
		debug_draw(offset_box.left_top, top_right);
		debug_draw(top_right, offset_box.right_bottom);
		debug_draw(offset_box.right_bottom, bottom_left);
		debug_draw(bottom_left, offset_box.left_top);
		*/

		if (point_in_box(offset_box, position) || point_in_box(offset_box, line_end) ||
			line_segments_intersect(position, line_end, offset_box.left_top, top_right) ||
			line_segments_intersect(position, line_end, top_right, offset_box.right_bottom) ||
			line_segments_intersect(position, line_end, offset_box.right_bottom, bottom_left) ||
			line_segments_intersect(position, line_end, bottom_left, offset_box.left_top)) {
			out.push_back(&component);
		}
	}
}
