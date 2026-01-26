#include "input.h"
#include "SDL3/SDL_keyboard.h"
#include "SDL3/SDL_mouse.h"
#include "SDL3/SDL_scancode.h"
#include "SDL3/SDL_events.h"
#include <queue>

static Input inputs[InputType::_count] = {};
static std::queue<InputEvent> input_events = {};
float mouse_x = 0.0f;
float mouse_y = 0.0f;

static void update_input_state(InputType input_type, bool is_down) {
	if (inputs[input_type].is_down != is_down) {
		inputs[input_type].is_down = is_down;
		inputs[input_type].is_handled = false;
	}
}

void update_input() {
	const bool* key_states = SDL_GetKeyboardState(nullptr);

	update_input_state(UP, key_states[SDL_SCANCODE_W]);
	update_input_state(DOWN, key_states[SDL_SCANCODE_S]);
	update_input_state(LEFT, key_states[SDL_SCANCODE_A]);
	update_input_state(RIGHT, key_states[SDL_SCANCODE_D]);
	update_input_state(INVENTORY, key_states[SDL_SCANCODE_E]);
	update_input_state(INTERACT, key_states[SDL_SCANCODE_SPACE]);

	SDL_MouseButtonFlags mouse_state = SDL_GetMouseState(&mouse_x, &mouse_y);
	update_input_state(MOUSE_CLICK, (mouse_state & SDL_BUTTON_LEFT) > 0);
}

void handle_input_event(SDL_KeyboardEvent event) {
	if (event.repeat) {
		return;
	}

	if (event.key == SDLK_Z) {
		input_events.push({InputType::RHYTHM_1, event.timestamp, event.down});
	}

	if (event.key == SDLK_X) {
		input_events.push({InputType::RHYTHM_2, event.timestamp, event.down});
	}
}

bool input_held(InputType input_type) {
	return inputs[input_type].is_down;
}

bool input_down_this_frame(InputType input_type) {
	return inputs[input_type].is_down && !inputs[input_type].is_handled;
}

bool input_released_this_frame(InputType input_type) {
	return !inputs[input_type].is_down && !inputs[input_type].is_handled;
}

void handle_input(InputType input_type) {
	inputs[input_type].is_handled = true;
}

void input_end_frame() {
	for (Input input : inputs) {
		input.is_handled = true;
	}
}
