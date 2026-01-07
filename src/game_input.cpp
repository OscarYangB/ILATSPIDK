#include "game_input.h"
#include "SDL3/SDL_keyboard.h"
#include "SDL3/SDL_mouse.h"
#include "SDL3/SDL_scancode.h"
#include "SDL3/SDL_events.h"

Input inputs[InputType::_count] = {};
std::queue<InputEvent> input_events = {};
float mouse_x = 0.0f;
float mouse_y = 0.0f;

void update_input() {
	const bool* key_states = SDL_GetKeyboardState(nullptr);

	inputs[UP].isDown = key_states[SDL_SCANCODE_W];
	inputs[DOWN].isDown = key_states[SDL_SCANCODE_S];
	inputs[LEFT].isDown = key_states[SDL_SCANCODE_A];
	inputs[RIGHT].isDown = key_states[SDL_SCANCODE_D];
	inputs[INVENTORY].isDown = key_states[SDL_SCANCODE_E];
	inputs[INTERACT].isDown = key_states[SDL_SCANCODE_SPACE];

	SDL_MouseButtonFlags mouse_state = SDL_GetMouseState(&mouse_x, &mouse_y);
	inputs[MOUSE_CLICK].isDown = (mouse_state & SDL_BUTTON_LEFT) > 0;
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
