#pragma once

#include "definitions.h"
#include "combat.h"
#include "entt/entt.hpp"

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

struct HandButtonComp {
	u8 index{};
};

struct HandCardComp {
	u8 index{};
	entt::entity owning_character{};

	u32 animation_id{};
	bool queue_draw_animation = false;
	bool is_dragged = false;

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
