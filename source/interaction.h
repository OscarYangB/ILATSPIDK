#pragma once

#include "vector2.h"

struct InteractionComp {
	void (*on_interact)() = nullptr;
	Box box{};
};

void update_interact();
