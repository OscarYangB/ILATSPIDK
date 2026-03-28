#include "input.h"
#include <queue>
#include <SDL3/SDL.h>
#include "platform_render.h"

static Input inputs[NUMBER_OF_INPUT_TYPES] {};
static std::queue<InputEvent> input_events {};

static float mouse_x {0.0f};
static float mouse_y {0.0f};
static std::vector<InputMode> input_mode_stack = {InputMode::EXPLORE};

float get_mouse_x() {
	return mouse_x;
}

float get_mouse_y() {
	return mouse_y;
}

void push_input_mode(InputMode mode) {
	input_mode_stack.push_back(mode);
}

void pop_input_mode(InputMode mode) {
	for (int i = input_mode_stack.size() - 1; i >= 0; i--) {
		if (input_mode_stack.at(i) == mode) {
			input_mode_stack.erase(input_mode_stack.begin() + i);
			return;
		}
	}
}

static void update_input_state(InputType input_type, bool is_down) {
	int index = static_cast<int>(input_type);
	if (inputs[index].is_down != is_down) {
		inputs[index].is_down = is_down;
		inputs[index].is_handled = false;
	}
}

void update_input() {
	const bool* key_states = SDL_GetKeyboardState(nullptr);

	update_input_state(InputType::UP, key_states[SDL_SCANCODE_W]);
	update_input_state(InputType::DOWN, key_states[SDL_SCANCODE_S]);
	update_input_state(InputType::LEFT, key_states[SDL_SCANCODE_A]);
	update_input_state(InputType::RIGHT, key_states[SDL_SCANCODE_D]);
	update_input_state(InputType::INVENTORY, key_states[SDL_SCANCODE_E]);
	update_input_state(InputType::INTERACT, key_states[SDL_SCANCODE_SPACE]);

	SDL_MouseButtonFlags mouse_state = SDL_GetMouseState(&mouse_x, &mouse_y);
	update_input_state(InputType::MOUSE_CLICK, (mouse_state & SDL_BUTTON_LEFT) > 0);
}

void handle_input_event(SDL_KeyboardEvent event) {
	if (event.repeat) {
		return;
	}

	if (event.key == SDLK_Z) {
		//input_events.push({InputType::RHYTHM_1, event.timestamp, event.down});
		enable_vsync();
	}

	if (event.key == SDLK_X) {
		//input_events.push({InputType::RHYTHM_2, event.timestamp, event.down});
		disable_vsync();
	}
}

bool input_held(InputType input_type) {
	int index = static_cast<int>(input_type);
	return inputs[index].is_down;
}

bool input_down_this_frame(InputType input_type) {
	int index = static_cast<int>(input_type);
	return inputs[index].is_down && !inputs[index].is_handled;
}

bool input_released_this_frame(InputType input_type) {
	int index = static_cast<int>(input_type);
	return !inputs[index].is_down && !inputs[index].is_handled;
}

void handle_input(InputType input_type) {
	inputs[static_cast<int>(input_type)].is_handled = true;
}

void input_end_frame() {
	for (Input& input : inputs) {
		input.is_handled = true;
	}
}

InputMode get_current_input_mode() {
	return input_mode_stack.back();
}
