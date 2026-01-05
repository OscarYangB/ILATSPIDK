#include "game_render.h"
#include "platform_render.h"
#include "game.h"
#include <SDL3/SDL.h>

Position camera_position = {3, 2};

void render_system() {
	auto view = ecs.view<const Sprite, Position>();

	for (auto [entity, sprite, position] : view.each()) {
	    render_sprite(sprite.name, position.x, position.y);
	}
}

Position world_to_pixel(Position in) {
	return in; // TODO
}

Position pixel_to_world(Position in) {
	return in; // TODO
}
