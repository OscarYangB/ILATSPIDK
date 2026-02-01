#pragma once

#include "image_assets.h"
#include "vector2.h"
#include "definitions.h"
#include "entt/entt.hpp"

enum class ImageAsset;

struct TransformComponent {
	Vector2 position = {0.f, 0.f};

	bool move(const entt::entity& entity_to_move, const Vector2& new_position);
	bool can_move(const entt::entity& entity_to_move, const Vector2& new_position);
};

struct Renderable : entt::type_list<void(AtlasIndex*&, int&), void(AtlasIndex& data, int& index)> {
	template<typename Base>
	struct type : Base {
		void draw(AtlasIndex*& data, int& size) {
			return entt::poly_call<0>(*this, data, size);
		}

		void write(AtlasIndex& data, int& index) {
			return entt::poly_call<1>(*this, data, index);
		}
	};

	template<typename Type>
	using impl = entt::value_list<&Type::draw, &Type::write>;
};

template<int N>
struct SpriteGroup {
	std::array<AtlasIndex, N> array = {};

	void draw(AtlasIndex*& data, int& size) {
		data = array.data();
		size = array.size();
	}

	void write(AtlasIndex& data, int& index) {
		array[index] = data;
	}
};

struct SpriteComponent {
	entt::poly<Renderable> renderable;

	void bounding_box(u16& out_w, u16& out_h);
	void bounding_box_center(float& out_w, float& out_h);
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
  Vector2 relative_position;
  u16 width;
  u16 height;

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
	u8 r;
	u8 g;
	u8 b;
	u8 size;

	HorizontalAnchor x_align = HorizontalAnchor::LEFT;
	VerticalAnchor y_align = VerticalAnchor::TOP;
};

extern Vector2 camera_position;
extern float camera_scale;

void update_render();
Vector2 world_to_pixel(const Vector2& in);
void update_sprite_resources();

void debug_draw(const Vector2& start, const Vector2& end);
