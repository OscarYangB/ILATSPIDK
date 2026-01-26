#include "game_render.h"
#include "entt/entity/fwd.hpp"
#include "image_assets.h"
#include "kerry_anim_controller.h"
#include "platform_render.h"
#include "game.h"

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

	update_animation();

	auto sprites = ecs.view<SpriteComponent>();
	for (auto [entity, sprite] : sprites.each()) {
		TransformComponent* transform = ecs.try_get<TransformComponent>(entity);
		AnchoredTransformComponent* anchored_transform = ecs.try_get<AnchoredTransformComponent>(entity);
		NineSliceComponent* nine_slice = ecs.try_get<NineSliceComponent>(entity);

		Vector2 position = anchored_transform ? anchored_transform->render_position() : world_to_pixel(transform->position);

		AtlasIndex* data; int size;
		sprite.renderable->draw(data, size);

		for (int i = 0; i < size; i++) {
			u16 index = static_cast<u16>(data[i]);
			u16 atlas_x = atlas_data[index].x;
			u16 atlas_y = atlas_data[index].y;
			u16 atlas_w = atlas_data[index].w;
			u16 atlas_h = atlas_data[index].h;
			u16 render_w = anchored_transform ? anchored_transform->render_width() : atlas_w * render_scale();
			u16 render_h = anchored_transform ? anchored_transform->render_height() : atlas_h * render_scale();

			if (position.x > window_width() || position.y > window_height()) continue;
			if (position.x + render_w < 0.f || position.y + render_h < 0.f) continue;

			if (nine_slice) {
				render_nine_slice(atlas_to_asset[index], atlas_x, atlas_y, atlas_w, atlas_h, position.x, position.y, render_w, render_h,
								  nine_slice->x, nine_slice->y, nine_slice->w, nine_slice->h, window_scale());
			} else {
				render_sprite(atlas_to_asset[index], atlas_x, atlas_y, atlas_w, atlas_h, position.x, position.y, render_w, render_h);
			}
		}
	}

	auto text_view = ecs.view<const TextComponent, const AnchoredTransformComponent>();
	for (auto [entity, text, transform] : text_view.each()) {
		Vector2 position = transform.render_position();
		render_text(text.text, position.x, position.y, text.r, text.b, text.g, text.size * window_scale());
	}

	end_render();
}

void update_sprite_resources() { // Going to load/unload the textures based on what existing entities need. Watch out--could become a performance concern
	bool is_loaded[NUMBER_OF_IMAGES];
	auto view = ecs.view<SpriteComponent>();

	for (auto [entity, sprite] : view.each()) {
		AtlasIndex* index; int size;
		sprite.renderable->draw(index, size);
		for (int i = 0; i < size; i++) {
			is_loaded[atlas_to_image_index(index[i])] = true;
		}
	}

	for (int i = 0; i < sizeof is_loaded; i++) {
		if (is_loaded[i]) {
			load_sprite(i);
		} else {
			unload_sprite(i);
		}
	}
}

Vector2 world_to_pixel(Vector2 in) {
	return Vector2{(in.x - camera_position.x) * render_scale() + window_width() / 2.0f,
				   (-in.y + camera_position.y) * render_scale() + window_height() / 2.0f};
}

Vector2 AnchoredTransformComponent::render_position() const  {
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

float AnchoredTransformComponent::render_width() const {
	return width * window_scale();
}

float AnchoredTransformComponent::render_height() const {
	return height * window_scale();
}
