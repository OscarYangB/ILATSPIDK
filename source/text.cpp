#include "text.h"

Language language = Language::ENGLISH;

const char* Text::get() const {
	switch (language) {
		case Language::ENGLISH: return english;
		case Language::FRENCH: return french;
	}

	return "";
}
