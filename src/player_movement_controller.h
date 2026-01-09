#pragma once

enum class CharacterDirection {
	UP,
	DOWN,
	LEFT,
	RIGHT
};

struct PlayerMovementComponent {
	float speed;
	CharacterDirection direction;
	bool is_moving;
};

void update_movement();
