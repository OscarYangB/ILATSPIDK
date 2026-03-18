#pragma once

#include "definitions.h"

struct CharacterAnimationComponent {
	u8 body_index{};
	u8 leg_idle_index{};
	u8 leg_moving_index{};
	u8 wind_index{};

	double timer = 0.f;
	double leg_timer = 0.f;

	u8 get_moving_left_leg_sprite();
	u8 get_moving_right_leg_sprite();
};

void update_character_animation();
