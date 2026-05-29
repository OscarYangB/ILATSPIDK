#pragma once

#include "../basic/definitions.h"

#include <variant>
#include <string_view>

enum class Language {
	ENGLISH,
	FRENCH,
};

inline Language language = Language::ENGLISH;

struct Text {
	std::string_view english = "";
	std::string_view french = "";

	inline std::string_view get() const {
		switch (language) {
		case Language::ENGLISH: return english;
		case Language::FRENCH: return french;
		}
		return "";
	}
};


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
