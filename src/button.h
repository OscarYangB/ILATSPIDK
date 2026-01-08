#pragma once

struct Button {
	void (*on_hover)();
	void (*on_click)();
	void (*on_unhover)();

	bool is_hovered;
	bool is_clicked;
};

void button_system();
