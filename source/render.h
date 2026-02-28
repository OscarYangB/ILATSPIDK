#pragma once

#include "image_data.h"
#include "vector2.h"
#include "definitions.h"
#include "entt/entt.hpp"
#include "text.h"

enum class ImageAsset;

struct TransformComponent {
	Vector2 position{};

	bool move(entt::entity entity_to_move, const Vector2& new_position);
	bool can_move(entt::entity entity_to_move, const Vector2& new_position);
};

struct Colour {
	u8 r = 255;
	u8 g = 255;
	u8 b = 255;
	u8 a = 255;
};

struct SpriteComponent {
	std::vector<Sprite> sprites{};
	std::unordered_map<u8, Colour> tints{};
	std::unordered_map<u8, Box> masks{};

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
	HorizontalAnchor x_anchor{};
	VerticalAnchor y_anchor{};
	Vector2 relative_position{};
	u16 width{};
	u16 height{};
	u8 sort_order{};

	Vector2 render_position() const;
	float render_width() const;
	float render_height() const;
};

struct NineSliceComponent {
	u16 x{};
	u16 y{};
	u16 w{};
	u16 h{};
};

struct TextComponent {
	Text text{};
	u8 r{};
	u8 g{};
	u8 b{};
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
