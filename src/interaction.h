#pragma once
#include "physics.h"

struct InteractionComponent {
	void (*on_interact)() = nullptr;
	Box box;
};

void update_interact();
