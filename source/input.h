#pragma once

#include "definitions.h"

struct SDL_KeyboardEvent;

enum class InputType {
	UP,
	DOWN,
	LEFT,
	RIGHT,
	INVENTORY,
	INTERACT,
	MOUSE_CLICK,
	RHYTHM_1, // TODO
	RHYTHM_2,
	_count
};

constexpr u8 NUMBER_OF_INPUT_TYPES = static_cast<int>(InputType::_count);

struct Input {
	u16 default_binding; // TODO
	bool is_down;
	bool is_handled;
};

struct InputEvent {
	InputType type;
	u64 time;
	bool down;
};

extern float mouse_x;
extern float mouse_y;

void update_input();
void handle_input_event(SDL_KeyboardEvent event);

bool input_held(InputType input_type);
bool input_down_this_frame(InputType input_type);
bool input_released_this_frame(InputType input_type);
void handle_input(InputType input_type);
void input_end_frame();
