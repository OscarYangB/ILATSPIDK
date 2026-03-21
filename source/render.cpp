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
#ifndef NDEBUG
	render_text(std::to_string(static_cast<int>(1.0 / delta_time)).c_str(), 200, 200, 200, 200, 50, 0, 255, 0, 0, HorizontalAnchor::LEFT, VerticalAnchor::TOP);
#endif
}

void sort_sprites() {
	// true -> second is above
	// false -> first is above

	ecs.sort<AnchoredTransformComponent>([](entt::entity first, entt::entity second) {
		AnchoredTransformComponent& first_transform = ecs.get<AnchoredTransformComponent>(first);
		AnchoredTransformComponent& second_transform = ecs.get<AnchoredTransformComponent>(second);
		return first_transform.sort_order < second_transform.sort_order;
	});

	ecs.sort<TransformComponent>([](entt::entity first, entt::entity second) {
		BoxColliderComponent* first_collider = ecs.try_get<BoxColliderComponent>(first);
		BoxColliderComponent* second_collider = ecs.try_get<BoxColliderComponent>(second);
		TransformComponent& first_transform = ecs.get<TransformComponent>(first);
		TransformComponent& second_transform = ecs.get<TransformComponent>(second);

		// Background
		if (!first_collider) return true;
		if (!second_collider) return false;

		// Foreground
		return first_collider->box.left_top.y + first_transform.position.y > second_collider->box.left_top.y + second_transform.position.y;
	});
}

void render_transform(entt::entity entity) {
	TransformComponent& transform = ecs.get<TransformComponent>(entity);
	SpriteComponent& sprite_component = ecs.get<SpriteComponent>(entity);
	Vector2 world_position = transform.position;

	TransformComponent* parent_transform = &transform;
	while (parent_transform->parent != entt::null) {
		parent_transform = ecs.try_get<TransformComponent>(parent_transform->parent);
		if (parent_transform == nullptr) break;
		world_position += parent_transform->position;
	}

	Vector2 position = world_to_pixel(world_position);

	if (!sprite_component.visible) return;

	for (int i = 0; i < sprite_component.sprites.size(); i++) {
		if (sprite_component.sprites.at(i) == Sprite::NONE) {
			continue;
		}

		u16 index = static_cast<u16>(sprite_component.sprites.at(i));
		u16 atlas_x = sprite_atlas_transform[index].x;
		u16 atlas_y = sprite_atlas_transform[index].y;
		u16 atlas_w = sprite_atlas_transform[index].w;
		u16 atlas_h = sprite_atlas_transform[index].h;

		if (sprite_component.masks.contains(i)) {
			Box mask = sprite_component.masks[i];
			atlas_x += mask.left_top.x;
			atlas_y -= mask.left_top.y;
			atlas_w = mask.width();
			atlas_h = mask.height();
			position.x += mask.left_top.x * window_scale();
			position.y += mask.left_top.y * window_scale();
		}

		u16 render_w = atlas_w * render_scale();
		u16 render_h = atlas_h * render_scale();

		if (position.x > window_width() || position.y > window_height()) continue;
		if (position.x + render_w < 0.f || position.y + render_h < 0.f) continue;

		Colour tint = sprite_component.tints.contains(i) ? sprite_component.tints[i] : Colour{};

		render_sprite(sprite_to_image_file[index], atlas_x, atlas_y, atlas_w, atlas_h, position.x, position.y, render_w, render_h, tint);
	}

	if (!transform.children.empty()) {
		for (entt::entity child : transform.children) {
			render_transform(child);
		}
	}
}

void render_anchored_transform(entt::entity entity) {
	AnchoredTransformComponent& transform = ecs.get<AnchoredTransformComponent>(entity);
	Vector2 position = transform.render_position();
	float render_w = transform.render_width();
	float render_h = transform.render_height();

	if (TextComponent* text = ecs.try_get<TextComponent>(entity); text) {
		render_text(text->text.get(), position.x, position.y, render_w, render_h, text->size * window_scale() * transform.get_recursive_scale(), text->mask,
				   text->colour.r, text->colour.g, text->colour.b, text->x_align, text->y_align);
	} else if (SpriteComponent* sprite_component = ecs.try_get<SpriteComponent>(entity); sprite_component) {
		if (!sprite_component->visible) return;
		NineSliceComponent* nine_slice = ecs.try_get<NineSliceComponent>(entity);

		for (int i = 0; i < sprite_component->sprites.size(); i++) {
			if (sprite_component->sprites.at(i) == Sprite::NONE) {
				continue;
			}
			u16 index = static_cast<u16>(sprite_component->sprites.at(i));
			u16 atlas_x = sprite_atlas_transform[index].x;
			u16 atlas_y = sprite_atlas_transform[index].y;
			u16 atlas_w = sprite_atlas_transform[index].w;
			u16 atlas_h = sprite_atlas_transform[index].h;

			if (sprite_component->masks.contains(i)) {
				Box mask = sprite_component->masks[i];
				atlas_x += mask.left_top.x;
				atlas_y -= mask.left_top.y;
				atlas_w = mask.width();
				atlas_h = mask.height();
				position.x += mask.left_top.x * window_scale();
				position.y -= mask.left_top.y * window_scale();
			}

			if (transform.width == 0) render_w = atlas_w * window_scale();
			if (transform.height == 0) render_h = atlas_h * window_scale();

			if (position.x > window_width() || position.y > window_height()) continue;
			if (position.x + render_w < 0.f || position.y + render_h < 0.f) continue;

			Colour tint = sprite_component->tints.contains(i) ? sprite_component->tints[i] : Colour{};

			if (nine_slice) {
				render_nine_slice(sprite_to_image_file[index], atlas_x, atlas_y, atlas_w, atlas_h, position.x, position.y, render_w, render_h,
								  nine_slice->x, nine_slice->y, nine_slice->w, nine_slice->h, window_scale());
			} else {
				render_sprite(sprite_to_image_file[index], atlas_x, atlas_y, atlas_w, atlas_h, position.x, position.y, render_w, render_h, tint);
			}
		}
	}

	if (!transform.children.empty()) {
		for (entt::entity child : transform.children) {
			render_anchored_transform(child);
		}
	}
}

void render_sprites() {
	auto transforms = ecs.view<TransformComponent, SpriteComponent>();
	for (auto [entity, transform, sprite] : transforms.each()) {
		if (transform.parent != entt::null) continue;
		render_transform(entity);
	}

	auto anchored_transforms = ecs.view<AnchoredTransformComponent>();
	for (auto [entity, transform] : anchored_transforms.each()) {
		if (transform.parent != entt::null) continue;
		render_anchored_transform(entity);
	}
}

void update_render() {
	start_render();

	sort_sprites();
	render_sprites();
	draw_debug_lines();
	render_fps_counter();

	end_render();
}

void update_sprite_resources() { // Going to load/unload the textures based on what existing entities need. Watch out--could become a performance concern
	bool is_loaded[NUMBER_OF_IMAGES];
	auto view = ecs.view<SpriteComponent>();

	for (auto [entity, sprite] : view.each()) {
		for (Sprite sprite : sprite.sprites) {
			if (sprite == Sprite::NONE) continue;
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

Vector2 AnchoredTransformComponent::render_position() const {
	Vector2 parent_position;
	float canvas_width;
	float canvas_height;
	if (parent == entt::null) {
		parent_position = {};
		canvas_width = window_width();
		canvas_height = window_height();
	} else {
		auto& parent_transform = ecs.get<AnchoredTransformComponent>(parent);
		parent_position = parent_transform.render_position();
		canvas_width = parent_transform.render_width();
		canvas_height = parent_transform.render_height();
	}

	Vector2 position_offset = relative_position * window_scale() * get_parent_scale() + parent_position;

	const float scaled_width = render_width();
	const float scaled_height = render_height();
	Vector2 anchor_offset{};

	switch(y_anchor) {
		case VerticalAnchor::TOP: break;
		case VerticalAnchor::CENTER: anchor_offset.y += (canvas_height - scaled_height) / 2.0f; break;
		case VerticalAnchor::BOTTOM: anchor_offset.y += canvas_height - scaled_height; break;
	}

	switch(x_anchor) {
		case HorizontalAnchor::LEFT: break;
		case HorizontalAnchor::CENTER: anchor_offset.x += (canvas_width - scaled_width) / 2.0f; break;
		case HorizontalAnchor::RIGHT: anchor_offset.x += canvas_width - scaled_width; break;
	}

	return position_offset + anchor_offset;
}

float AnchoredTransformComponent::render_width() const {
	return width * window_scale() * get_recursive_scale();
}

float AnchoredTransformComponent::render_height() const {
	return height * window_scale() * get_recursive_scale();
}

bool TransformComponent::move(entt::entity entity_to_move, const Vector2& new_position) {
	if (can_move(entity_to_move, new_position)) {
		position = new_position;
		return true;
	}

	return false;
}

bool TransformComponent::can_move(entt::entity entity_to_move, const Vector2& new_position) {
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
	int index = static_cast<int>(sprites.at(0));
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
	int index = static_cast<int>(sprites.at(0));
	u16 left = sprite_atlas_transform[index].visible_left; // These use downward-positive y-coordinates
	u16 right = sprite_atlas_transform[index].visible_right;
	u16 up = sprite_atlas_transform[index].visible_up;
	u16 down = sprite_atlas_transform[index].visible_down;

	for (int i = 1; i < sprites.size(); i++) {
		index = static_cast<int>(sprites.at(i));
		left = std::min(left, sprite_atlas_transform[index].visible_left);
		right = std::max(right, sprite_atlas_transform[index].visible_right);
		up = std::min(up, sprite_atlas_transform[index].visible_up);
		down = std::max(down, sprite_atlas_transform[index].visible_down);
	}

	return {{(float)left, -((float)up)}, {(float)right, -((float)down)}};
}

void HierarchyComponent::add_child(entt::entity parent, entt::entity child) {
	AnchoredTransformComponent& child_transform = ecs.get<AnchoredTransformComponent>(child);
	child_transform.parent = parent;
	children.push_back(child);
}

void HierarchyComponent::remove_child(entt::entity child) {
	AnchoredTransformComponent& child_transform = ecs.get<AnchoredTransformComponent>(child);
	child_transform.parent = entt::null;
	std::erase_if(children, [child](entt::entity current_child){ return current_child == child; } );
}

void HierarchyComponent::on_destroy(entt::registry& registry, const entt::entity entt) {
	AnchoredTransformComponent& transform = registry.get<AnchoredTransformComponent>(entt);
	std::vector<entt::entity> children {transform.children};
	for (entt::entity child : children) {
		ecs.destroy(child); // calls on_destroy to recursively destroy all descendents
	}

	if (transform.parent != entt::null && ecs.valid(transform.parent)) {
		AnchoredTransformComponent& parent_transform = registry.get<AnchoredTransformComponent>(transform.parent);
		parent_transform.remove_child(entt);
	}
}

float AnchoredTransformComponent::get_recursive_scale() const {
	return get_parent_scale() * scale;
}

float AnchoredTransformComponent::get_parent_scale() const {
	float result = 1.f;
	const AnchoredTransformComponent* transform = this;
	while (true) {
		if (transform->parent == entt::null) break;
		transform = &ecs.get<AnchoredTransformComponent>(transform->parent);
		result *= transform->scale;
	}
	return result;
}
