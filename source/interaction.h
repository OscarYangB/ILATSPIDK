#pragma once

#include "vector2.h"

enum class InteractionType {
	INTERACT,
	PLAYER_ENTER,
};

struct InteractionComp {
	Box box{};
	void (*on_interact)() = nullptr;
	bool enabled = true;
	InteractionType type = InteractionType::INTERACT;
};

void update_interact();
void update_player_enter();
