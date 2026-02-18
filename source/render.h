#pragma once

#include "image_data.h"
#include "vector2.h"
#include "definitions.h"
#include "entt/entt.hpp"

enum class ImageAsset;

struct TransformComponent {
	Vector2 position = {0.f, 0.f};

	bool move(const entt::entity& entity_to_move, const Vector2& new_position);
	bool can_move(const entt::entity& entity_to_move, const Vector2& new_position);
};

struct Colour {
	u8 r = 255;
	u8 g = 255;
	u8 b = 255;
};

struct SpriteComponent {
	std::vector<Sprite> sprites;
	std::unordered_map<u8, Colour> tints = {};
	std::unordered_map<u8, Box> masks = {};

	Box bounding_box();
	Box visible_bounding_box();
};

enum class VerticalAnchor {
	TOP,
	BOTTOM,
	CENTER,
};

enum class HorizontalAnchor {
	LEFT,
	RIGHT,
	CENTER,
};

struct AnchoredTransformComponent {
	HorizontalAnchor x_anchor;
	VerticalAnchor y_anchor;
	Vector2 relative_position = Vector2::zero();
	u16 width = 0;
	u16 height = 0;

	Vector2 render_position() const;
	float render_width() const;
	float render_height() const;
};

struct NineSliceComponent {
	u16 x;
	u16 y;
	u16 w;
	u16 h;
};

struct TextComponent {
	const char* text;
	u8 r = 0;
	u8 g = 0;
	u8 b = 0;
	u8 size = 50;
	u16 mask = 0; // How many characters to display? 0 means all

	HorizontalAnchor x_align = HorizontalAnchor::LEFT;
	VerticalAnchor y_align = VerticalAnchor::TOP;
};

extern Vector2 camera_position;
extern float camera_scale;

void update_render();
Vector2 world_to_pixel(const Vector2& in);
void update_sprite_resources();

void debug_draw(const Vector2& start, const Vector2& end);
