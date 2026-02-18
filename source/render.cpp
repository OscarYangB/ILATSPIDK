#include "render.h"
#include "image_data.h"
#include "platform_render.h"
#include "game.h"
#include "physics.h"

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


struct DebugLine {
	Vector2 start;
	Vector2 end;
	double time_left;
};
std::vector<DebugLine> debug_lines {};
void debug_draw(const Vector2& start, const Vector2& end) {
#ifndef NDEBUG
	debug_lines.push_back({start, end, 0.3});
#endif
}

void draw_debug_lines() {
#ifndef NDEBUG
	for (DebugLine& line : debug_lines) {
		platform_debug_draw(line.start, line.end);
		line.time_left -= delta_time;
	}

	std::erase_if(debug_lines, [](const DebugLine& line) { return line.time_left < 0.f; });
#endif
}

void render_fps_counter() {
	render_text(std::to_string(static_cast<int>(1.0 / delta_time)).c_str(), 200, 200, 200, 200, 255, 0, 0, 50, 0, HorizontalAnchor::LEFT, VerticalAnchor::TOP);
}

void update_render() {
	start_render();

	ecs.sort<SpriteComponent>([](const entt::entity& first, const entt::entity& second) { // Sort render order using y position
		// true -> second is above
		// false -> first is above

		// UI
		AnchoredTransformComponent* first_anchored_transform = ecs.try_get<AnchoredTransformComponent>(first);
		if (first_anchored_transform) return false;
		AnchoredTransformComponent* second_anchored_transform = ecs.try_get<AnchoredTransformComponent>(second);
		if (second_anchored_transform) return true;

		BoxColliderComponent* first_collider = ecs.try_get<BoxColliderComponent>(first);
		BoxColliderComponent* second_collider = ecs.try_get<BoxColliderComponent>(second);
		TransformComponent* first_transform = ecs.try_get<TransformComponent>(first);
		TransformComponent* second_transform = ecs.try_get<TransformComponent>(second);

		// Background
		if (!first_collider || !first_transform) return true;
		if (!second_collider || !second_transform) return false;

		// Foreground
		return first_collider->box.left_top.y + first_transform->position.y > second_collider->box.left_top.y + second_transform->position.y;
	});

	auto sprites = ecs.view<SpriteComponent>();
	for (auto [entity, sprite_component] : sprites.each()) {
		TransformComponent* transform = ecs.try_get<TransformComponent>(entity);
		AnchoredTransformComponent* anchored_transform = ecs.try_get<AnchoredTransformComponent>(entity);
		NineSliceComponent* nine_slice = ecs.try_get<NineSliceComponent>(entity);

		Vector2 position = anchored_transform ? anchored_transform->render_position() : world_to_pixel(transform->position);

		for (int i = 0; i < sprite_component.sprites.size(); i++) {
			u16 index = static_cast<u16>(sprite_component.sprites[i]);
			u16 atlas_x = sprite_atlas_transform[index].x;
			u16 atlas_y = sprite_atlas_transform[index].y;
			u16 atlas_w = sprite_atlas_transform[index].w;
			u16 atlas_h = sprite_atlas_transform[index].h;
			u16 render_w; u16 render_h;
			if (anchored_transform) {
				render_w = anchored_transform->render_width();
				render_h = anchored_transform->render_height();
				if (render_w == 0) render_w = atlas_w * window_scale();
				if (render_h == 0) render_h = atlas_h * window_scale();
			} else {
				render_w = atlas_w * render_scale();
				render_h = atlas_h * render_scale();
			}

			if (position.x > window_width() || position.y > window_height()) continue;
			if (position.x + render_w < 0.f || position.y + render_h < 0.f) continue;

			Colour tint = sprite_component.tints.contains(i) ? sprite_component.tints[i] : Colour{};
			if (sprite_component.tints.contains(i)) {
				Box mask = sprite_component.masks[i];
				atlas_x += mask.left_top.x;
				atlas_y += mask.left_top.y;
				atlas_w = mask.width();
				atlas_h = mask.height();
			}

			if (nine_slice) {
				render_nine_slice(sprite_to_image_file[index], atlas_x, atlas_y, atlas_w, atlas_h, position.x, position.y, render_w, render_h,
								  nine_slice->x, nine_slice->y, nine_slice->w, nine_slice->h, window_scale());
			} else {
				render_sprite(sprite_to_image_file[index], atlas_x, atlas_y, atlas_w, atlas_h, position.x, position.y, render_w, render_h, tint);
			}
		}
	}

	auto text_view = ecs.view<TextComponent, const AnchoredTransformComponent>();
	for (auto [entity, text, transform] : text_view.each()) {
		Vector2 position = transform.render_position();
		render_text(text.text, position.x, position.y, transform.render_width(), transform.render_height(), text.r, text.b, text.g, text.size * window_scale(),
					text.mask, text.x_align, text.y_align);
	}

	draw_debug_lines();
	//render_fps_counter();

	end_render();
}

void update_sprite_resources() { // Going to load/unload the textures based on what existing entities need. Watch out--could become a performance concern
	bool is_loaded[NUMBER_OF_IMAGES];
	auto view = ecs.view<SpriteComponent>();

	for (auto [entity, sprite] : view.each()) {
		for (Sprite sprite : sprite.sprites) {
			is_loaded[sprite_to_image_file_index(sprite)] = true;
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

Vector2 world_to_pixel(const Vector2& in) {
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

bool TransformComponent::move(const entt::entity& entity_to_move, const Vector2& new_position) {
	if (can_move(entity_to_move, new_position)) {
		position = new_position;
		return true;
	}

	return false;
}

bool TransformComponent::can_move(const entt::entity& entity_to_move, const Vector2& new_position) {
	if (BoxColliderComponent* collider_to_move = ecs.try_get<BoxColliderComponent>(entity_to_move); collider_to_move != nullptr) {
		auto view = ecs.view<BoxColliderComponent, TransformComponent>();

		for (auto [entity, collider, transform] : view.each()) {
			if (entity == entity_to_move) continue;

			if (is_colliding(transform.position, new_position, collider, *collider_to_move)) {
				return false;
			}
		}

		return true;
	}

	return true; // No collider on entity_to_move
}

Box SpriteComponent::bounding_box() {
	int index = static_cast<int>(sprites[0]);
	u16 w = sprite_atlas_transform[index].w;
	u16 h = sprite_atlas_transform[index].h;

	for (int i = 1; i < sprites.size(); i++) {
		index = static_cast<int>(sprites[i]);
		w = std::max(w, sprite_atlas_transform[index].w);
		h = std::max(h, sprite_atlas_transform[index].h);
	}

	return {{0.f, 0.f}, {(float)w, -((float)h)}};
}

Box SpriteComponent::visible_bounding_box() {
	int index = static_cast<int>(sprites[0]);
	u16 left = sprite_atlas_transform[index].visible_left; // These use downward-positive y-coordinates
	u16 right = sprite_atlas_transform[index].visible_right;
	u16 up = sprite_atlas_transform[index].visible_up;
	u16 down = sprite_atlas_transform[index].visible_down;

	for (int i = 1; i < sprites.size(); i++) {
		index = static_cast<int>(sprites[i]);
		left = std::min(left, sprite_atlas_transform[index].visible_left);
		right = std::max(right, sprite_atlas_transform[index].visible_right);
		up = std::min(up, sprite_atlas_transform[index].visible_up);
		down = std::max(down, sprite_atlas_transform[index].visible_down);
	}

	return {{(float)left, -((float)up)}, {(float)right, -((float)down)}};
}
