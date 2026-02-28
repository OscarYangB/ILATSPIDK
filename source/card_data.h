#pragma once

#include "combat.h"

#include <iostream>

enum class CardID {
	FIREBALL,
	SATURN,
	MIND_READ,
	HEAL
};

constexpr CardData card_data[] {
	{.name= {"Fireball"}, .description = {"Shoot ball of fire"}, .play_text = {"is shooting a fireball!"},
			.cost = 3, .minigame_level = 0, .enemy_target_bitmask = GOOD, .number_of_targets = 1, .card_type = CardType::MAGIC,
			.play = [](CharacterComponent& character, const Characters& targets) {
				std::cout << "Fireball played" << "\n";
			},
			.activate = [](CharacterComponent& character, const Characters& targets) {
				std::cout << "Fireball activated" << "\n";
			}},
	{.name= {"Saturn"}, .description = {"The power of Saturn"}, .play_text = {"is evoking the power of Saturn!"},
			 .cost = 3, .minigame_level = 0, .enemy_target_bitmask = GOOD, .number_of_targets = 1, .card_type = CardType::PSYCHIC,
			 .play = [](CharacterComponent& character, const Characters& targets) {

			 },
			 .activate = [](CharacterComponent& character, const Characters& targets) {

			 }},

	{.name= {"Mind Read"}, .description = {"Read the mind of an enemy"}, .play_text = {"is reading the mind of an enemy."},
			 .cost = 3, .minigame_level = 0, .enemy_target_bitmask = GOOD, .number_of_targets = 1, .card_type = CardType::PSYCHIC,
			 .play = [](CharacterComponent& character, const Characters& targets) {

			 },
			 .activate = [](CharacterComponent& character, const Characters& targets) {

			 }},
	{.name= {"Heal"}, .description = {"Heal an ally"}, .play_text = {"is healing an ally!"},
			 .cost = 3, .minigame_level = 0, .enemy_target_bitmask = GOOD, .number_of_targets = 1, .card_type = CardType::MAGIC,
			 .play = [](CharacterComponent& character, const Characters& targets) {

			 },
			 .activate = [](CharacterComponent& character, const Characters& targets) {

			 }},
};

std::vector<Card> make_cards(std::vector<CardID> ids);
