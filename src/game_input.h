#pragma once

#include "definitions.h"
#include <queue>

struct SDL_KeyboardEvent;

enum InputType {
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

struct Input {
	u16 default_binding; // TODO
	bool isDown;
	bool isHandled;
};

struct InputEvent {
	InputType type;
	u64 time;
	bool down;
};

extern Input inputs[(u8)InputType::_count];
extern std::queue<InputEvent> input_events;

void update_input();
void handle_input_event(SDL_KeyboardEvent event);
