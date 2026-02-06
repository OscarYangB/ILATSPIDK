#include "button.h"
#include "entt/entt.hpp"
#include "render.h"
#include "game.h"
#include "input.h"

void update_button() {
	auto buttons = ecs.view<const SpriteComponent, Button, const AnchoredTransformComponent>();

	for (auto [entity, sprite, button, transform] : buttons.each()) {
		Vector2 position = transform.render_position();

		if (mouse_x > position.x && mouse_x < position.x + transform.render_width() &&
			mouse_y > position.y && mouse_y < position.y + transform.render_height()) {
			if (!button.is_hovered) {
				button.is_hovered = true;
				if (button.on_hover) button.on_hover();
			}

			if (input_down_this_frame(InputType::MOUSE_CLICK)) {
				handle_input(InputType::MOUSE_CLICK);
				if (button.on_click) button.on_click();

				button.is_clicked = true;
			} else {
				button.is_clicked = false;
			}
		} else if (button.is_hovered) {
			if (button.on_unhover) button.on_unhover();
			button.is_hovered = false;
		}
	}
}
