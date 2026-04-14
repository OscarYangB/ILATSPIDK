#pragma once

#include "definitions.h"
#include "entt/entt.hpp"
#include "vector2.h"

struct CharacterComp;
struct Card;

namespace UI {
	void start_combat();
	void update_combat();
	void end_combat();
	void on_bar_end();
	void on_turn_start();

	void add_hand_visual(const CharacterComp& character, u8 index);
	void destroy_hand_visual(const CharacterComp& character, u8 index);
	void play_queued_draw_animations();

	void refresh_health_bar(const CharacterComp& character, bool is_heal);
}

struct CombatUI {
	entt::entity hovered_card = entt::null;
	entt::entity dragged_card = entt::null;
	std::optional<Vector2> target_position{};
};

struct HandButtonComp {
	u8 index{};
};

struct HandCardComp {
	u8 index{};
	entt::entity owning_character{};

	u32 animation_id{};
	bool queue_draw_animation = false;

	Card get_card();
};

struct GamebarComp {
	u8 index{};
	double vibrate_timer{};
};

struct HealthbarComp {};

struct ArrowComp {
	u8 index;
};

struct CardPreviewComp{};

struct QueueComp{};

struct ActionText{};

struct CancelAreaComp {
	bool enabled = false;
};
