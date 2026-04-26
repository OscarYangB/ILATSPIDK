#pragma once

#include "entt/entt.hpp"

using ButtonCallback = void(*)(entt::entity button);

struct ButtonComp {
	ButtonCallback on_hover = nullptr;
	ButtonCallback on_click = nullptr;
	ButtonCallback on_unhover = nullptr;
	bool is_enabled = true;

	bool is_hovered;
};

void update_button();
