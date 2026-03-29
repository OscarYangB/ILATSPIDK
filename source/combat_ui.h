#pragma once

#include "definitions.h"
#include "combat.h"
#include "entt/entt.hpp"

void ui_start_combat();
void ui_update_combat();
void ui_end_combat();
void ui_on_bar_end();
void ui_on_turn_start();

void ui_add_hand_visual(const CharacterComp& character, u8 index);
void ui_destroy_hand_visual(const CharacterComp& character, u8 index);
void ui_play_queued_draw_animations();

void refresh_health_bar(const CharacterComp& character, bool is_heal);

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
