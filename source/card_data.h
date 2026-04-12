#pragma once

#include "combat.h"
#include "game.h"

enum class CardID {
	FIREBALL,
	SATURN,
	MIND_READ,
	HEAL,
	GRENADE
};

constexpr CardData card_data[] {
	{.name = {"Fireball"}, .description = {"Shoot ball of fire."}, .play_text = {"is shooting a fireball!"},
	 .cost = 3, .minigame_level = 0, .valid_target_bitmask = EVIL, .ai_target_bitmask = GOOD, .card_type = CardType::MAGIC,
	 .play = [](CharacterComp& character, entt::entity target) {

	 },
	 .activate = [](CharacterComp& character, entt::entity target) {
		 ecs.get<CharacterComp>(target).damage(50);
	 }},

	{.name= {"Saturn"}, .description = {"The power of Saturn. This text is very long and will create a line break."}, .play_text = {"is evoking the power of Saturn!"},
	 .cost = 3, .minigame_level = 0, .valid_target_bitmask = 0, .ai_target_bitmask = GOOD, .card_type = CardType::PSYCHIC,
	 .play = [](CharacterComp& character, entt::entity target) {

	 },
	 .activate = [](CharacterComp& character, entt::entity target) {

	 }},

	{.name= {"Mind Read"}, .description = {"Read the mind of an enemy"}, .play_text = {"is reading the mind of an enemy."},
	 .cost = 3, .minigame_level = 0, .valid_target_bitmask = GOOD, .ai_target_bitmask = GOOD, .card_type = CardType::PSYCHIC,
	 .play = [](CharacterComp& character, entt::entity target) {

	 },
	 .activate = [](CharacterComp& character, entt::entity target) {
		 character.draw(2);
	 }},

	{.name= {"Heal"}, .description = {"Heal an ally"}, .play_text = {"is healing an ally!"},
	 .cost = 2, .minigame_level = 0, .valid_target_bitmask = GOOD, .ai_target_bitmask = EVIL, .card_type = CardType::MAGIC,
	 .play = [](CharacterComp& character, entt::entity target) {

	 },
	 .activate = [](CharacterComp& character, entt::entity target) {
		 ecs.get<CharacterComp>(target).heal(50.f);
	 }},

	{.name= {"Grenade"}, .description = {"Explode an area"}, .play_text = {"is throwing a grenade!"},
	 .cost = 2, .minigame_level = 0, .valid_target_bitmask = EVIL, .ai_target_bitmask = GOOD, .card_type = CardType::GROOVE,
	 .play = [](CharacterComp& character, entt::entity target) {

	 },
	 .activate = [](CharacterComp& character, entt::entity target) {

	 }},
};

constexpr CardData test = {.name = {"awdawd"}, .description = {"asdasd"}};

std::vector<Card> make_cards(std::vector<CardID> ids);
