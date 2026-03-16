#pragma once

#include "image_data.h"

struct Font {
	ImageFile file{};
	float size{};
	float width{};
	float height{};
};
