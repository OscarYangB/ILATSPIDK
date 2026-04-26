#include "button.h"
#include "render.h"
#include "game.h"
#include "input.h"

bool is_button_hovered(const ButtonComp& button, const UITransformComp& transform) {
	Vector2 position = transform.render_position();
	return get_pixel_mouse_x() > position.x && get_pixel_mouse_x() < position.x + transform.render_width() &&
		   get_pixel_mouse_y() > position.y && get_pixel_mouse_y() < position.y + transform.render_height();
}

void update_button() {
	auto buttons = ecs.view<ButtonComp, const UITransformComp>();

	for (auto [entity, button, transform] : buttons.each()) {
		if (!is_button_hovered(button, transform) && button.is_hovered) {
			button.is_hovered = false;
			if (button.on_unhover) {
				button.on_unhover(entity);
			}
		}
	}

	for (auto [entity, button, transform] : buttons.each()) {
		if (is_button_hovered(button, transform) && !button.is_hovered && button.is_enabled) {
			button.is_hovered = true;
			if (button.on_hover) {
				button.on_hover(entity);
			}
		}
	}

	for (auto [entity, button, transform] : buttons.each()) {
		if (is_button_hovered(button, transform) && input_down_this_frame(InputType::MOUSE_CLICK) && button.is_enabled) {
			handle_input(InputType::MOUSE_CLICK);
			if (button.on_click) {
				button.on_click(entity);
			}
		}
	}
}
