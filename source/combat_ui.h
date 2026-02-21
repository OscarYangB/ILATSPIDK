#pragma once

struct CardData;

void ui_start_combat();
void ui_update_combat();
void ui_end_combat();
void ui_on_bar_end();
void ui_on_turn_start();

struct HandCardComponent {
	const CardData* card{};
};
