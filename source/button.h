#pragma once

#include "entt/entt.hpp"

struct ButtonComponent {
	void (*on_hover)(entt::entity button) = nullptr;
	void (*on_click)(entt::entity button) = nullptr;
	void (*on_unhover)(entt::entity button) = nullptr;

	bool is_hovered;
	bool is_clicked;
};

void update_button();
