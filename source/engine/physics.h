#pragma once

#include "game.h"
#include "render.h"

struct BoxColliderComp {
	Box box{};
};

struct PolygonColliderComp {
	Polygon polygon{};
};

bool is_colliding(const Box& first, const Box& second);
bool is_colliding(const Polygon& polygon, const Box& box);
bool is_colliding(const Polygon& first, const Polygon& second);
bool line_segments_intersect(const Vector2& start_1, const Vector2& end_1, const Vector2& start_2, const Vector2& end_2);
bool point_in_box(const Box& box, const Vector2& vector);

template<typename BoxComp>
void raytest(std::vector<BoxComp*>& out, const Vector2& position, const Vector2& direction, float length) {
	Vector2 line_end = position + direction * length;
	auto view = ecs.view<BoxComp, TransformComp>();

	// debug_draw(position, line_end);

	for (auto [entity, component, transform] : view.each()) {
		Box box = component.box + transform.position;

		// debug_draw(box);

		if (point_in_box(box, position) || point_in_box(box, line_end) ||
			line_segments_intersect(position, line_end, box.left_top, box.right_top()) ||
			line_segments_intersect(position, line_end, box.right_top(), box.right_bottom) ||
			line_segments_intersect(position, line_end, box.right_bottom, box.left_bottom()) ||
			line_segments_intersect(position, line_end, box.left_bottom(), box.left_top)) {
			out.push_back(&component);
		}
	}
}
