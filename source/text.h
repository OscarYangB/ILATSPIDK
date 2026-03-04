#pragma once

#include <string_view>

enum class Language {
	ENGLISH,
	FRENCH,
};

extern Language language;

struct Text {
	std::string_view english = "";
	std::string_view french = "";

	std::string_view get() const;
};

std::string_view number_to_string(int number);
