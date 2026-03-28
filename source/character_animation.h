#pragma once

#include "entt/entt.hpp"
#include "image_data.h"
#include "fixed_list.h"

enum class FinishBehaviour {
	LOOP,
	DESTROY_ENTITY,
	DESTROY_COMPONENT,
};

struct CycleAnimationComp {
	FixedList<Sprite, 10> sprites{};
	double frequency{};
	FinishBehaviour finish_behaviour = FinishBehaviour::LOOP;

	u8 index = 0;
	double timer = 0.0;
	double delay = 0.0;
};

struct CharacterAnimationComp {
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
void update_cycle_animations();
