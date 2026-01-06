#include "game_render.h"
#include "platform_render.h"
#include "game.h"

Position camera_position = {3.0f, 2.0f};
float camera_scale = 1.0f;

void render_system() {
	auto view = ecs.view<const Sprite, Position>();

	for (auto [entity, sprite, position] : view.each()) {
		Position pixel_position = world_to_pixel(position);
		AtlasData atlas_data = {12, 200, 300};
	    render_sprite(sprite.name, pixel_position.x, pixel_position.y, camera_scale, &atlas_data);
	}
}

Position world_to_pixel(Position in) { // Flip y because I want the up direction to be positive y
	return Position {(in.x - camera_position.x) * camera_scale, (in.y + camera_position.y) * camera_scale};
}

Position pixel_to_world(Position in) {
	return Position {(in.x / camera_scale) + camera_position.x, (in.y / camera_scale) - camera_position.y};
}
