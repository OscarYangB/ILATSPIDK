#pragma once

#include "image_data.h"
#include "vector2.h"
#include "definitions.h"
#include "entt/entt.hpp"
#include "text.h"
#include "game.h"
#include "fixed_list.h"

enum class ImageAsset;

template<typename T>
struct HierarchyComp {
	std::vector<entt::entity> children{};
	entt::entity parent = entt::null;

	void add_child(entt::entity parent, entt::entity child) {
		T& child_transform = ecs.get<T>(child);
		child_transform.parent = parent;
		children.push_back(child);
	}

	void remove_child(entt::entity child) {
		T& child_transform = ecs.get<T>(child);
		child_transform.parent = entt::null;
		std::erase_if(children, [child](entt::entity current_child){ return current_child == child; } );
	}

	static void on_destroy(entt::registry& registry, const entt::entity entt) {
		T& transform = registry.get<T>(entt);

		if (!transform.children.empty()) {
			std::vector<entt::entity> children {transform.children};
			for (entt::entity child : children) {
				ecs.destroy(child); // calls on_destroy to recursively destroy all descendents
			}
		}

		if (transform.parent != entt::null && ecs.valid(transform.parent)) {
			T& parent_transform = registry.get<T>(transform.parent);
			parent_transform.remove_child(entt);
		}
	}
};

struct TransformComp : HierarchyComp<TransformComp> {
	Vector2 position{};

	bool move(entt::entity entity_to_move, const Vector2& new_position);
	bool can_move(entt::entity entity_to_move, const Vector2& new_position);
};

struct Colour {
	u8 r = 255;
	u8 g = 255;
	u8 b = 255;
	u8 a = 255;

	void operator*=(const Colour& other);
};

constexpr Colour BLACK = {0, 0, 0, 255};
constexpr Colour WHITE = {255, 255, 255, 255};

struct SpriteComp {
	static constexpr u8 MAX_SPRITES = 10;
	FixedList<Sprite, MAX_SPRITES> sprites{};
	std::array<std::optional<Colour>, MAX_SPRITES> tints{};
	std::array<std::optional<Box>, MAX_SPRITES> masks{};
	Colour tint{};
	bool visible = true;

	Box bounding_box();
	Box visible_bounding_box();
};

enum class YAnchor {
	TOP,
	BOTTOM,
	CENTER,
};

enum class XAnchor {
	LEFT,
	RIGHT,
	CENTER,
};

struct UITransformComp : HierarchyComp<UITransformComp> {
	XAnchor x_anchor{};
	YAnchor y_anchor{};
	Vector2 relative_position{};
	u16 width{};
	u16 height{};
	u8 sort_order{};
	float scale = 1.f;

	float get_recursive_scale() const;
	float get_parent_scale() const;
	Vector2 render_position() const;
	float render_width() const;
	float render_height() const;
};

struct NineSliceComp {
	u16 x{};
	u16 y{};
	u16 w{};
	u16 h{};
};

struct TextComp {
	Text text{};
	Colour colour = BLACK;
	u8 size = 50;
	u8 mask = 0; // How many characters to display? 0 means all

	XAnchor x_align = XAnchor::LEFT;
	YAnchor y_align = YAnchor::TOP;
};

extern Vector2 camera_position;
extern float camera_scale;

constexpr float SCREEN_SPACE_WIDTH = 1920.f;
constexpr float SCREEN_SPACE_HEIGHT = 1080.f;

void update_render();
Vector2 world_to_pixel(const Vector2& in);
void update_sprite_resources();
float window_scale();

void debug_draw(const Vector2& start, const Vector2& end);
