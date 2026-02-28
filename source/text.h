#pragma once

enum class Language {
	ENGLISH,
	FRENCH,
};

extern Language language;

struct Text {
	const char* english = "";
	const char* french = "";

	const char* get() const;
};
