#pragma once

#include <vector>
#include "definitions.h"
#include "image_data.h"
#include <optional>

struct CharacterComponent;
using Characters = std::vector<CharacterComponent*>;

struct CardData {
	const char* name{};
	const char* description{};
	const char* play_text{};
	u8 cost{};
	u8 minigame_level{}; // 0 means none
	u8 enemy_target_bitmask{};
	u8 number_of_targets{};

	void (*play)(CharacterComponent& character, const Characters& targets);
	void (*activate)(CharacterComponent& character, const Characters& targets);
};

using Card = const CardData*;

struct PlayedCard {
	u8 bars_until_activate{};
	Card card = nullptr;
	Characters targets = {};
};

enum CharacterType : u8 {
	GOOD = 1 << 0,
	EVIL = 1 << 1,
	FURNITURE = 1 << 2,
};

struct StatusEffect {
	float seconds_timer{};
	u8 bar_timer{};
	u8 turn_timer{};
	Sprite icon{};

	void (*on_add)();
	/* void update(); // TODO */
	/* void on_bar_end(); // TODO */
	/* void on_turn_end(); // TODO */
	void (*on_remove)();
};

struct CharacterDataComponent {
	float starting_health{};
	float starting_shield = 0.f;
	CharacterType type{};
	std::vector<Card> starting_deck{};
	Sprite icon{};
};

struct CharacterComponent {
	const CharacterDataComponent* data{};
	float health{};
	float max_health{};
	float shield{};
	std::vector<Card> deck{};
	std::vector<Card> hand{};
	std::vector<StatusEffect> status_effects{};
	std::optional<PlayedCard> played_card{};

	void init_from_data(const CharacterDataComponent& new_data);
	inline bool is_alive() { return health > 0.f; }
	void heal(float amount);
	void damage(float amount);
	void draw();
	void play_card(u8 hand_index, const Characters& targets);
	void on_bar_end();
	void on_turn_start();
};

constexpr double BPM = 135.0;
constexpr u8 BEATS_PER_BAR = 4;
constexpr double BEATS_PER_SECOND = BPM / 60.0;
constexpr double SECONDS_PER_BEAT = 1.0 / BEATS_PER_SECOND;
constexpr double SECONDS_PER_BAR = SECONDS_PER_BEAT * BEATS_PER_BAR;
constexpr u8 BARS_PER_TURN = 4;

struct Combat {
	Characters characters{};
	u8 turn_index = 0;
	double timer = 0.f;
	u8 bar_index = 0;

	void update();
};

extern std::optional<Combat> combat;

void update_combat();
void start_combat();
void end_combat();

enum class CardID {
	FIREBALL,
};

constexpr CardData card_data[] {
	{.name= "Fireball", .description = "Shoot ball of fire", .play_text = "is shooting a fireball!",
	 .cost = 3, .minigame_level = 0, .enemy_target_bitmask = GOOD, .number_of_targets = 1,
	 .play = [](CharacterComponent& character, const Characters& targets) {

	 },
	 .activate = [](CharacterComponent& character, const Characters& targets) {

	 }},
};

std::vector<Card> make_cards(std::vector<CardID> ids);
