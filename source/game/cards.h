#pragma once

#include "combat.h"

enum class CardID {
	FIREBALL,
	SATURN,
	MIND_READ,
	HEAL,
	GRENADE
};

constexpr CardData card_data[] {
	{.name = {"Fireball"}, .description = {"Shoot ball of fire."}, .play_text = {"is shooting a fireball!"},
	 .cost = 3, .valid_target_bitmask = EVIL, .ai_target_bitmask = GOOD, .sprite = Sprite::FIREBALL,
	 .play = [](CharacterComp& character, CharacterComp& target) {

	 },
	 .activate = [](CharacterComp& character, CharacterComp& target) {
		 target.damage(50);
	 }},

	{.name= {"Saturn"}, .description = {"The power of Saturn. This text is very long and will create a line break."}, .play_text = {"is evoking the power of Saturn!"},
	 .cost = 3, .valid_target_bitmask = 0, .ai_target_bitmask = GOOD, .sprite = Sprite::SATURN,
	 .play = [](CharacterComp& character, CharacterComp& target) {

	 },
	 .activate = [](CharacterComp& character, CharacterComp& target) {
		 target.heal(100.f);
	 }},

	{.name= {"Mind Read"}, .description = {"Read the mind of an enemy"}, .play_text = {"is reading the mind of an enemy."},
	 .cost = 3, .valid_target_bitmask = GOOD, .ai_target_bitmask = GOOD, .sprite = Sprite::TEST_BACKGROUND,
	 .play = [](CharacterComp& character, CharacterComp& target) {

	 },
	 .activate = [](CharacterComp& character, CharacterComp& target) {

	 }},

	{.name= {"Heal"}, .description = {"Heal an ally"}, .play_text = {"is healing an ally!"},
	 .cost = 2, .valid_target_bitmask = GOOD | EVIL, .ai_target_bitmask = EVIL, .sprite = Sprite::TEST_BACKGROUND,
	 .play = [](CharacterComp& character, CharacterComp& target) {

	 },
	 .activate = [](CharacterComp& character, CharacterComp& target) {
		 target.heal(50.f);
	 }},

	{.name= {"Grenade"}, .description = {"Explode an area"}, .play_text = {"is throwing a grenade!"},
	 .cost = 2, .valid_target_bitmask = EVIL, .ai_target_bitmask = GOOD, .sprite = Sprite::TEST_BACKGROUND,
	 .play = [](CharacterComp& character, CharacterComp& target) {

	 },
	 .activate = [](CharacterComp& character, CharacterComp& target) {
		 target.damage(25);
	 }},
};

std::vector<Card> make_cards(std::vector<CardID> ids);
