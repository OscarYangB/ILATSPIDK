#include "game_render.h"
#include "platform_render.h"
#include "game.h"

Vector2 camera_position = {0.0f, 0.0f};
float camera_scale = 1.0f; // Gameplay code can write to this

static float window_scale() {
	return window_height() / 1080.0f;
}

static float render_scale() {
	return camera_scale * window_scale();
}

void render_system() {
	start_render();

	auto sprites = ecs.view<const Sprite, const Transform>();
	for (auto [entity, sprite, transform] : sprites.each()) {
		Vector2 position = world_to_pixel(transform.position);
	    render_sprite(sprite.name, position.x, position.y, sprite.render_width(), sprite.render_height(),
					  sprite.atlas_index, sprite.width, sprite.height);
	}

	auto ui_sprites = ecs.view<const Sprite, const AnchoredTransform>();
	for (auto [entity, sprite, transform] : ui_sprites.each()) {
		float x, y, w, h;
		transform.get_render_dimensions(sprite.width, sprite.height, &x, &y, &w, &h);
	    render_sprite(sprite.name, x, y, w, h, sprite.atlas_index, sprite.width, sprite.height);
	}

	end_render();
}

Vector2 world_to_pixel(Vector2 in) {
	return Vector2{(in.x - camera_position.x) * render_scale() + window_width() / 2.0f,
				   (-in.y + camera_position.y) * render_scale() + window_height() / 2.0f};
}

void AnchoredTransform::get_render_dimensions(float sprite_w, float sprite_h, float* x, float* y, float* w, float* h) const  {
    *x = relative_position.x * window_scale();
	*y = relative_position.y * window_scale();
	*w = sprite_w * window_scale();
	*h = sprite_h * window_scale();

	switch(y_anchor) {
		case VerticalAnchor::TOP: break;
		case VerticalAnchor::CENTER: {
			*y += (window_height() - *h) / 2.0f;
		} break;
		case VerticalAnchor::BOTTOM: {
			*y += window_height() - *h;
		} break;
	}

	switch(x_anchor) {
		case HorizontalAnchor::LEFT: break;
		case HorizontalAnchor::CENTER: {
			*x += (window_width() - *w) / 2.0f;
		} break;
		case HorizontalAnchor::RIGHT: {
			*x += window_width() - *w;
		} break;
	}
}

float Sprite::render_width() const {
	return width * render_scale();
}

float Sprite::render_height() const {
	return height * render_scale();
}
