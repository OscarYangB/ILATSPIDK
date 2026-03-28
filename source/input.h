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
	u16 default_binding{}; // TODO
	bool is_down = false;
	bool is_handled = false;
};

struct InputEvent {
	InputType type{};
	u64 time{};
	bool down{};
};

enum class InputMode {
	EXPLORE,
	COMBAT,
	DIALOG,
	MENU
};

void update_input();
void handle_input_event(SDL_KeyboardEvent event);

float get_mouse_x();
float get_mouse_y();
bool input_held(InputType input_type);
bool input_down_this_frame(InputType input_type);
bool input_released_this_frame(InputType input_type);
void handle_input(InputType input_type);
void input_end_frame();

InputMode get_current_input_mode();
void push_input_mode(InputMode mode);
void pop_input_mode(InputMode mode);
