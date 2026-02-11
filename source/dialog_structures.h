#pragma once
#include "definitions.h"
#include <variant>

enum class Speaker;

struct DialogLine {
	const char* line;
	Speaker speaker;
};

struct DialogChoice {
	const char* line;
	u16 next_choice;
};

struct DialogCheck {
	bool (*check)() = nullptr;
	u16 failure_pointer;
};

struct DialogFunction {
	void (*function)() = nullptr;
};

struct DialogJump {
	u16 index;
};

using Dialog = std::variant<DialogLine, DialogChoice, DialogCheck, DialogFunction, DialogJump>;
