#pragma once

#include "definitions.h"
#include "combat.h"
#include "entt/entt.hpp"

void ui_start_combat();
void ui_update_combat();
void ui_end_combat();
void ui_on_bar_end();
void ui_on_turn_start();

void play_draw_animation(int index);

struct HandCardComponent {
	u8 index{};
	entt::entity sprite_entity{};
	u32 animation_id{};

	Card get_card();
};
