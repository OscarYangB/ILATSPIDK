#include "button.h"
#include "entt/entt.hpp"
#include "game_render.h"
#include "game.h"
#include "game_input.h"

struct Button {
	bool is_hovered;
	bool is_clicked;

	void (*on_hover)();
	void (*on_click)();
};

void button_system() {
	auto buttons = ecs.view<const Sprite, Button, const AnchoredTransform>();

	for (auto [entity, sprite, button, transform] : buttons.each()) {
		float x, y, w, h;
		transform.get_render_dimensions(sprite.width, sprite.height, &x, &y, &w, &h);

		if (mouse_x > x && mouse_x < x + w &&
			mouse_y > y && mouse_y < y + h) {
			if (!button.is_hovered) {
				button.is_hovered = true;
				button.on_hover();
			}

			if (inputs[MOUSE_CLICK].isDown) {
				if (!inputs[MOUSE_CLICK].isHandled) {
					inputs[MOUSE_CLICK].isHandled = true;
					button.on_click();
				}

				button.is_clicked = true;
			} else {
				button.is_clicked = false;
			}
		} else {
			button.is_hovered = false;
		}
	}
}
