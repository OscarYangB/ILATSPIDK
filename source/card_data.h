#pragma once

#include "combat.h"

#include <iostream>

enum class CardID {
	FIREBALL,
	SATURN,
	MIND_READ,
	HEAL,
	GRENADE
};

constexpr CardData card_data[] {
	{.name = {"Fireball"}, .description = {"Shoot ball of fire."}, .play_text = {"is shooting a fireball!"},
			.cost = 3, .minigame_level = 0, .enemy_target_bitmask = GOOD, .number_of_targets = 1, .card_type = CardType::MAGIC,
			.play = [](CharacterComp& character, const Characters& targets) {
				std::cout << "Fireball played" << "\n";
			},
			.activate = [](CharacterComp& character, const Characters& targets) {
				std::cout << "Fireball activated" << "\n";
			}},
	{.name= {"Saturn"}, .description = {"The power of Saturn. This text is very long and will create a line break."}, .play_text = {"is evoking the power of Saturn!"},
			 .cost = 3, .minigame_level = 0, .enemy_target_bitmask = GOOD, .number_of_targets = 1, .card_type = CardType::PSYCHIC,
			 .play = [](CharacterComp& character, const Characters& targets) {

			 },
			 .activate = [](CharacterComp& character, const Characters& targets) {

			 }},

	{.name= {"Mind Read"}, .description = {"Read the mind of an enemy"}, .play_text = {"is reading the mind of an enemy."},
			 .cost = 3, .minigame_level = 0, .enemy_target_bitmask = GOOD, .number_of_targets = 1, .card_type = CardType::PSYCHIC,
			 .play = [](CharacterComp& character, const Characters& targets) {

			 },
			 .activate = [](CharacterComp& character, const Characters& targets) {

			 }},
	{.name= {"Heal"}, .description = {"Heal an ally"}, .play_text = {"is healing an ally!"},
			 .cost = 3, .minigame_level = 0, .enemy_target_bitmask = GOOD, .number_of_targets = 1, .card_type = CardType::MAGIC,
			 .play = [](CharacterComp& character, const Characters& targets) {

			 },
			 .activate = [](CharacterComp& character, const Characters& targets) {

			 }},
	{.name= {"Grenade"}, .description = {"Explode an area"}, .play_text = {"is throwing a grenade!"},
			 .cost = 2, .minigame_level = 0, .enemy_target_bitmask = GOOD, .number_of_targets = 1, .card_type = CardType::GROOVE,
			 .play = [](CharacterComp& character, const Characters& targets) {

			 },
			 .activate = [](CharacterComp& character, const Characters& targets) {

			 }},
};

constexpr CardData test = {.name = {"awdawd"}, .description = {"asdasd"}};

std::vector<Card> make_cards(std::vector<CardID> ids);
