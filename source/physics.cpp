#include "physics.h"

Box Box::operator+(const Vector2& offset) const {
	return {left_top + offset, right_bottom + offset};
}

Vector2 Box::center() {
	return {(left_top.x + right_bottom.x) / 2.f, (right_bottom.y + left_top.y) / 2.f};
}

bool is_colliding(const Vector2& first_position, const Vector2& second_position, const BoxColliderComponent& first_collider, const BoxColliderComponent& second_collider) {
	Vector2 first_left_top = first_position + first_collider.box.left_top;
	Vector2 first_right_bottom = first_position + first_collider.box.right_bottom;
	Vector2 second_left_top = second_position + second_collider.box.left_top;
	Vector2 second_right_bottom = second_position + second_collider.box.right_bottom;

	if (first_right_bottom.x < second_left_top.x) return false;
	if (first_left_top.x > second_right_bottom.x) return false;
	if (first_right_bottom.y > second_left_top.y) return false;
	if (first_left_top.y < second_right_bottom.y) return false;
	return true;
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
