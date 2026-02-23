#pragma once

#include "definitions.h"
#include "combat.h"

void ui_start_combat();
void ui_update_combat();
void ui_end_combat();
void ui_on_bar_end();
void ui_on_turn_start();

struct HandCardComponent {
	u8 index;

	Card get_card();
};
