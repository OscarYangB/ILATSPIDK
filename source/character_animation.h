#pragma once

#include "entt/entt.hpp"
#include "image_data.h"

struct CycleAnimationComponent {
	std::vector<Sprite> sprites{};
	double frequency{};
	bool destroy_on_finish = false;

	u8 index = 0;
	double timer = 0.f;
};

void update_cycle_animations();
