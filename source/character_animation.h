#pragma once

#include "entt/entt.hpp"
#include "image_data.h"
#include "fixed_list.h"

enum class FinishBehaviour {
	LOOP,
	DESTROY_ENTITY,
	DESTROY_COMPONENT,
};

struct CycleAnimationComponent {
	FixedList<Sprite, 10> sprites{};
	double frequency{};
	FinishBehaviour finish_behaviour = FinishBehaviour::LOOP;

	u8 index = 0;
	double timer = 0.0;
	double delay = 0.0;
};

void update_cycle_animations();
