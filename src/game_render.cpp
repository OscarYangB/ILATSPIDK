#include "game_render.h"
#include "platform_render.h"
#include "game.h"

Vector2 camera_position = {3.0f, 2.0f};
float camera_scale = 1.0f;
// Maybe another coefficient to scale the camera based on the resolution

void render_system() {
	auto sprites = ecs.view<const Sprite, const Transform>();

	for (auto [entity, sprite, transform] : sprites.each()) {
		Vector2 pixel_position = world_to_pixel(transform.position);
		AtlasData atlas_data = {sprite.atlas_index, sprite.width, sprite.height};
	    render_sprite(sprite.name, pixel_position.x, pixel_position.y, camera_scale, &atlas_data);
	}
}

void render_ui_system() {
	auto anchored_sprites = ecs.view<const Sprite, const AnchoredTransform>();

	// For UI
	for (auto [entity, sprite, anchor] : anchored_sprites.each()) {
		int x = anchor.position.x;
		int y = anchor.position.y;

		switch(anchor.y_anchor) {
			case VerticalAnchor::TOP: {
			} break;
			case VerticalAnchor::CENTER: {
				// TODO
			} break;
			case VerticalAnchor::BOTTOM: {
				y += window_height();
			} break;
		}

		switch(anchor.x_anchor) {
			case HorizontalAnchor::LEFT: {
			} break;
			case HorizontalAnchor::CENTER: {
				// TODO
			} break;
			case HorizontalAnchor::RIGHT: {
				x += window_width();
			} break;
		}
	}
}

Vector2 world_to_pixel(Vector2 in) { // Flip y because I want the up direction to be positive y
	return Vector2{(in.x - camera_position.x) * camera_scale, (in.y + camera_position.y) * camera_scale};
}

Vector2 pixel_to_world(Vector2 in) {
	return Vector2{(in.x / camera_scale) + camera_position.x, (in.y / camera_scale) - camera_position.y};
}
