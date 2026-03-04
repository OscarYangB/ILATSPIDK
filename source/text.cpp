#include "text.h"

#include <unordered_map>
#include <string>

Language language = Language::ENGLISH;

std::string_view Text::get() const {
	switch (language) {
		case Language::ENGLISH: return english;
		case Language::FRENCH: return french;
	}

	return "";
}

static std::unordered_map<int, std::string> string_map{};

std::string_view number_to_string(int number) {
	if (!string_map.contains(number)) {
		string_map[number] = std::to_string(number);
	}

	return {string_map[number]};
}
