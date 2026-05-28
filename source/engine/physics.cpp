#include "physics.h"

bool is_colliding(const Box& first, const Box& second) {
	// debug_draw(first);
	// debug_draw(second);

	if (first.right_bottom.x < second.left_top.x) return false;
	if (first.left_top.x > second.right_bottom.x) return false;
	if (first.right_bottom.y > second.left_top.y) return false;
	if (first.left_top.y < second.right_bottom.y) return false;
	return true;
}

bool is_colliding(const Vector2& polygon_position, const PolygonColliderComp& polygon_collider, const Box& box) {
	for (int i = 0; i < polygon_collider.points.size(); i++) {
		Vector2 start = polygon_collider.points[i] + polygon_position;
		Vector2 end = polygon_collider.points[(i+1) % polygon_collider.points.size()] + polygon_position;
		if (point_in_box(box, start) ||
			line_segments_intersect(box.left_top, box.right_top(), start, end) ||
			line_segments_intersect(box.left_top, box.left_bottom(), start, end) ||
			line_segments_intersect(box.right_bottom, box.right_top(), start, end) ||
			line_segments_intersect(box.right_bottom, box.left_bottom(), start, end)) {
			return true;
		}
	}
	return false;
}

bool is_colliding(const Vector2& first_position, const Vector2& second_position, const PolygonColliderComp& first_collider, const PolygonColliderComp& second_collider) {
	for (int i = 0; i < first_collider.points.size(); i++) {
		Vector2 first_start = first_collider.points[i] + first_position;
		Vector2 first_end = first_collider.points[(i+1) % first_collider.points.size()] + first_position;
		for (int j = 0; j < second_collider.points.size(); j++) {
			Vector2 second_start = second_collider.points[j] + second_position;
			Vector2 second_end = second_collider.points[(j+1) % second_collider.points.size()] + second_position;
			if (line_segments_intersect(first_start, first_end, second_start, second_end)) {
				return true;
			}
		}
	}
	return false;
}

bool point_in_box(const Box& box, const Vector2& point) {
	return point.x > box.left_top.x &&
		   point.x < box.right_bottom.x &&
		   point.y > box.right_bottom.y &&
		   point.y < box.left_top.y;
}

// From Graphics Gems III Chapter IV Part 6
bool line_segments_intersect(const Vector2& start_1, const Vector2& end_1, const Vector2& start_2, const Vector2& end_2) {
	Vector2 A = end_1 - start_1;
	Vector2 B = start_2 - end_2;
	Vector2 C = start_1 - start_2;

	float denominator = A.y * B.x - A.x * B.y;

	if (denominator == 0.f) {
		return false; // Line segments are collinear
	}

	float alpha = (B.y * C.x - B.x * C.y);

	if (denominator > 0.f) {
		if (alpha < 0.f || alpha > denominator) {
			return false;
		}
	} else if (alpha > 0.f || alpha < denominator) {
		return false;
	}

	float beta  = (A.x * C.y - A.y * C.x);

	if (denominator > 0.f) {
		if (beta < 0.f || beta > denominator) {
		return false;
		}
	} else if (beta > 0.f || beta < denominator) {
		return false;
	}

	return true;
}
