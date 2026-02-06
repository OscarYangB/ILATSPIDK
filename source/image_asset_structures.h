#pragma once

#include "definitions.h"

struct AtlasData {
	u16 x;
	u16 y;
	u16 w;
	u16 h;

	u16 visible_left;
	u16 visible_up;
	u16 visible_right;
	u16 visible_down;
};
