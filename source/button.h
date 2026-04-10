#pragma once

#include "entt/entt.hpp"

struct ButtonComp {
	void (*on_hover)(entt::entity button) = nullptr;
	void (*on_click)(entt::entity button) = nullptr;
	void (*on_unhover)(entt::entity button) = nullptr;
	bool is_enabled = true;

	bool is_hovered;
};

void update_button();
