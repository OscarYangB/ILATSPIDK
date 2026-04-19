#pragma once

#include <vector>
#include "definitions.h"
#include "image_data.h"
#include <optional>
#include "text.h"
#include <entt/entt.hpp>
#include "combat_ui.h"
#include "fixed_list.h"

struct CharacterComp;

enum class CardType {
	PSYCHIC,
	MAGIC,
	GROOVE
};

struct CardData {
	Text name{};
	Text description{};
	Text play_text{};
	u8 cost{};
	u8 minigame_level{}; // 0 means none
	u8 valid_target_bitmask{};
	u8 ai_target_bitmask{};
	CardType card_type;

	void (*play)(CharacterComp& character, CharacterComp& target);
	void (*activate)(CharacterComp& character, CharacterComp& target);
};

struct Card {
	const CardData* data = nullptr;

	bool can_play();
};

struct PlayedCard {
	u8 bars_until_activate{};
	Card card{};
	entt::entity target{};
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

struct CharacterDataComp {
	Text name{};
	float starting_health{};
	float starting_shield = 0.f;
	CharacterType type{};
	std::vector<Card> starting_deck{};
	Sprite icon{};

	static constexpr auto in_place_delete = true;
};

struct CharacterComp {
	entt::entity entity{};
	const CharacterDataComp* data{};
	float health{};
	float max_health{};
	float shield{};
	std::vector<Card> deck{};
	std::vector<Card> hand{};
	std::vector<StatusEffect> status_effects{};
	std::optional<PlayedCard> played_card{};

	u64 low_health_animation_id{};

	void init_from_data(const CharacterDataComp& new_data);
	inline bool is_alive() { return health > 0.f; }
	void heal(float amount);
	void damage(float amount);
	void draw(u8 amount = 1);
	void play_card(u8 hand_index, entt::entity target);
	void queue_card(Card card, entt::entity target);
	void queue_random_card();
	void on_bar_end();
	void on_turn_start();
};

constexpr double BPM = 120.0;
constexpr u8 BEATS_PER_BAR = 4;
constexpr double BEATS_PER_SECOND = BPM / 60.0;
constexpr double SECONDS_PER_BEAT = 1.0 / BEATS_PER_SECOND;
constexpr double SECONDS_PER_BAR = SECONDS_PER_BEAT * BEATS_PER_BAR;
constexpr u8 BARS_PER_TURN = 4;

struct CombatSingleton {
	FixedList<entt::entity, 20> characters{};
	u8 turn_index = 0;
	double timer = 0.f;
	u8 bar_index = 0;
	CombatUI ui{};

	void update();
	float get_bar_progress();
	CharacterComp* get_active_character();
	entt::entity get_active_character_entity();
	float get_discrete_bar_progress();
	u8 get_bars_available();
};

void update_combat();
void start_combat();
void end_combat();
CombatSingleton& get_combat();
bool is_valid_target(const CharacterComp& playing_character, const CharacterComp& target_character, const Card& card);
