#include "physics.h"

bool is_colliding(const Vector2& first_position, const Vector2& second_position, const BoxColliderComponent& first_collider, const BoxColliderComponent& second_collider) {
	Vector2 first_top_left = first_position + first_collider.top_left;
	Vector2 first_bottom_right = first_position + first_collider.bottom_right;
	Vector2 second_top_left = second_position + second_collider.top_left;
	Vector2 second_bottom_right = second_position + second_collider.bottom_right;

	if (first_bottom_right.x < second_top_left.x) return false;
	if (first_top_left.x > second_bottom_right.x) return false;
	if (first_bottom_right.y > second_top_left.y) return false;
	if (first_top_left.y < second_bottom_right.y) return false;
	return true;
}
