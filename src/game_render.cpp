#include "game_render.h"
#include "entt/entity/fwd.hpp"
#include "kerry_anim_controller.h"
#include "platform_render.h"
#include "game.h"
#include "player_movement_controller.h"

Vector2 camera_position = {0.0f, 0.0f};
float camera_scale = 1.0f; // Gameplay code can write to this

static float window_scale() {
	if ((float)window_width() / (float)window_height() < 1920.0f / 1080.f) {
		return (float)window_width() / 1920.0f; // Zoom out if aspect ratio is narrower than expected
	} else {
		return window_height() / 1080.0f;
	}
}

static float render_scale() {
	return camera_scale * window_scale();
}

void render_system() {
	start_render();

	auto sprites = ecs.view<const Sprite, const Transform>(entt::exclude_t<PlayerMovementComponent>()); // Temporary exclusion
	for (auto [entity, sprite, transform] : sprites.each()) {
		Vector2 position = world_to_pixel(transform.position);
		float width = sprite.render_width();
		float height = sprite.render_height();
		if (position.x > window_width() || position.y > window_height()) continue; // Cull off screen stuff
		if (position.x + width < 0.f || position.y + height < 0.f) continue;
	    render_sprite(sprite.image_asset, position.x, position.y, width, height, sprite.atlas_index, sprite.width, sprite.height);
	}

	update_animation(); // This is for a temporary test

	auto ui_sprites = ecs.view<const Sprite, const AnchoredTransform>();
	for (auto [entity, sprite, transform] : ui_sprites.each()) {
		// This seems quite bad. Check if theres a better way to do this in a query maybe
		if (NineSliceComponent* slice = ecs.try_get<NineSliceComponent>(entity); slice != nullptr) {
			Vector2 position = transform.render_position();
			render_nine_slice(sprite.image_asset, position.x, position.y, transform.render_width(), transform.render_height(),
							  sprite.atlas_index, sprite.width, sprite.height, slice->x, slice->y, slice->w, slice->h, window_scale());
			continue;
		}

		Vector2 position = transform.render_position();
	    render_sprite(sprite.image_asset, position.x, position.y, transform.render_width(), transform.render_height(),
					  sprite.atlas_index, sprite.width, sprite.height);
	}

	render_text("fuck", 500, 500, 255, 0, 0);

	end_render();
}

Vector2 world_to_pixel(Vector2 in) {
	return Vector2{(in.x - camera_position.x) * render_scale() + window_width() / 2.0f,
				   (-in.y + camera_position.y) * render_scale() + window_height() / 2.0f};
}

Vector2 AnchoredTransform::render_position() const  {
    float x = relative_position.x * window_scale();
	float y = relative_position.y * window_scale();

	switch(y_anchor) {
		case VerticalAnchor::TOP: break;
		case VerticalAnchor::CENTER: {
			y += (window_height() - render_height()) / 2.0f;
		} break;
		case VerticalAnchor::BOTTOM: {
			y += window_height() - render_height();
		} break;
	}

	switch(x_anchor) {
		case HorizontalAnchor::LEFT: break;
		case HorizontalAnchor::CENTER: {
			x += (window_width() - render_width()) / 2.0f;
		} break;
		case HorizontalAnchor::RIGHT: {
			x += window_width() - render_width();
		} break;
	}

	return Vector2{x, y};
}

float Sprite::render_width() const {
	return width * render_scale();
}

float Sprite::render_height() const {
	return height * render_scale();
}

float AnchoredTransform::render_width() const {
	return width * window_scale();
}

float AnchoredTransform::render_height() const {
	return height * window_scale();
}
