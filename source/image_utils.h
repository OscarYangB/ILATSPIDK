#pragma once

#include "image_data.h"

constexpr SpriteAtlasTransform get_sprite_dimensions(Sprite sprite) {
	return sprite_atlas_transform[static_cast<int>(sprite)];
}

constexpr int sprite_to_image_file_index(Sprite sprite) {
	return static_cast<int>(sprite_to_image_file[static_cast<int>(sprite)]);
}
