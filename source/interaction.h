#pragma once

#include "vector2.h"

struct InteractionComp {
	Box box{};
	void (*on_interact)() = nullptr;
};

void update_interact();
