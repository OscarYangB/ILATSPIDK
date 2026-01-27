#pragma once
#include "vector2.h"
#include "entt/entt.hpp"

struct BoxColliderComponent {
	Vector2 top_left;
	Vector2 bottom_right;
};

bool is_colliding(const Vector2& first_position, const Vector2& second_position, const BoxColliderComponent& first_collider, const BoxColliderComponent& second_collider);
