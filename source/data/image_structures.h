#pragma once

#include "../engine/definitions.h"

struct SpriteAtlasTransform {
	u16 x{};
	u16 y{};
	u16 w{};
	u16 h{};

	u16 visible_left{};
	u16 visible_up{};
	u16 visible_right{};
	u16 visible_down{};
};

struct ImageDimensions {
	u16 width{};
	u16 height{};
};
