#pragma once

#include "../basic/definitions.h"
#include "../basic/text.h"

#include <variant>

enum class Speaker;

struct DialogLine {
	Text line{};
	Speaker speaker{};
};

struct DialogChoice {
	Text line{};
	u16 next_choice{};
	bool (*check)() = nullptr;
};

struct DialogCheck {
	bool (*check)() = nullptr;
	u16 failure_pointer;
};

struct DialogFunction {
	void (*function)() = nullptr;
};

struct DialogJump {
	u16 index{};
};

using Dialog = std::variant<DialogLine, DialogChoice, DialogCheck, DialogFunction, DialogJump>;
