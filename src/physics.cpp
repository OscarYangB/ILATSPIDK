#include "physics.h"

bool is_colliding(const Vector2& first_position, const Vector2& second_position, const BoxColliderComponent& first_collider, const BoxColliderComponent& second_collider) {
	Vector2 first_top_left = first_position + first_collider.box.top_left;
	Vector2 first_bottom_right = first_position + first_collider.box.bottom_right;
	Vector2 second_top_left = second_position + second_collider.box.top_left;
	Vector2 second_bottom_right = second_position + second_collider.box.bottom_right;

	if (first_bottom_right.x < second_top_left.x) return false;
	if (first_top_left.x > second_bottom_right.x) return false;
	if (first_bottom_right.y > second_top_left.y) return false;
	if (first_top_left.y < second_bottom_right.y) return false;
	return true;
}

// From Graphics Gems III Chapter IV Part 6
bool line_segments_intersect(const Vector2& start_1, const Vector2& end_1, const Vector2& start_2, const Vector2& end_2) {
	Vector2 A = end_1 - start_1;
	Vector2 B = start_2 - end_2;
	Vector2 C = start_1 - start_2;

	float denominator = A.y * B.x - A.x * B.y;

	if (denominator <= 0.f) {
		return false;
	}

	float alpha = (B.y * C.x - B.x * C.y) / denominator;
	float beta  = (A.x * C.y - A.y * C.x) / denominator;

	if (alpha > 0.f && alpha < 1.f && beta > 0.f && beta < 1.f) {
		return true;
	}

	return false;
}
