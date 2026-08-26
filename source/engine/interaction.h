#pragma once

#include "../basic/shapes.h"

enum class InteractionType {
	INTERACT,
	PLAYER_ENTER,
};

struct InteractionComp {
	Box box{};
	void (*on_interact)() = nullptr;
	bool (*can_interact)() = nullptr;
	bool enabled = true;
	bool is_player_inside = false;
	InteractionType type = InteractionType::INTERACT;
	float highlight = false;
	float highlight_time = 0.f;
};

void update_interact();
void update_player_enter();
