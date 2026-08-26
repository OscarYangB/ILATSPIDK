#pragma once

#include "../basic/definitions.h"

struct SpriteAtlasTransform {
	u16 x{};
	u16 y{};
	u16 w{};
	u16 h{};

	u16 visible_left{};
	u16 visible_up{};
	u16 visible_right{};
	u16 visible_down{};

	u16 centroid_x;
	u16 centroid_y;
};

struct ImageDimensions {
	u16 width{};
	u16 height{};
};
