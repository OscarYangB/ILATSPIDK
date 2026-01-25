#pragma once

struct Button {
	void (*on_hover)() = nullptr;
	void (*on_click)() = nullptr;
	void (*on_unhover)() = nullptr;

	bool is_hovered;
	bool is_clicked;
};

void button_system();
