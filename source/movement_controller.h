#pragma once

enum class CharacterDirection {
	UP,
	DOWN,
	LEFT,
	RIGHT
};

struct PlayerMovementComponent {
	float speed{};
	CharacterDirection direction = CharacterDirection::DOWN;
	bool is_moving = false;
};

void update_movement();
