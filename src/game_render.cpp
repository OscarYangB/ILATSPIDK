#include "game_render.h"
#include "platform_render.h"
#include "game.h"

Vector2 camera_position = {0.0f, 0.0f};
float camera_scale = 1.0f;

void render_system() {
	start_render();

	auto sprites = ecs.view<const Sprite, const Transform>();

	for (auto [entity, sprite, transform] : sprites.each()) {
		Vector2 pixel_position = world_to_pixel(transform.position);
		AtlasData atlas_data = {sprite.atlas_index, sprite.width, sprite.height};
	    render_sprite(sprite.name, pixel_position.x, pixel_position.y, camera_scale, &atlas_data);
	}

	end_render();
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

static float window_scale() {
	return window_height() / 1080.0f;
}

Vector2 world_to_pixel(Vector2 in) {
	return Vector2{(in.x - camera_position.x) * camera_scale * window_scale() + window_width() / 2.0f,
				   (-in.y + camera_position.y) * camera_scale * window_scale() + window_height() / 2.0f};
}

Vector2 pixel_to_world(Vector2 in) {
	return Vector2{(in.x / camera_scale) + camera_position.x, -((in.y / camera_scale) - camera_position.y)};
}
