#include "combat.h"
#include <algorithm>
#include "game.h"
#include <random>
#include "card_data.h"

void CharacterComp::init_from_data(const CharacterDataComp& new_data) {
	data = &new_data;
	health = data->starting_health;
	max_health = data->starting_health;
	shield = data->starting_shield;
	deck = data->starting_deck;
	std::shuffle(deck.begin(), deck.end(), std::mt19937{std::random_device{}()});
	hand = {};
	status_effects = {};
	played_card = std::nullopt;
}

void CharacterComp::heal(float amount) {
	health += amount;
	health = std::clamp(health, 0.f, max_health);
	UI::refresh_health_bar(*this, true);
}

void CharacterComp::damage(float amount) {
	float damage_to_shield = std::min(shield, amount);
	shield -= damage_to_shield;
	health -= amount - damage_to_shield;
	health = std::clamp(health, 0.f, max_health);
	UI::refresh_health_bar(*this, false);
}

void CharacterComp::draw(u8 amount) {
	for (int i = 0; i < amount; i++) {
		if (deck.empty()) {
			break;
		}

		Card card = deck.back();
		deck.pop_back();
		hand.push_back(card);
		UI::add_hand_visual(*this, hand.size() - 1);
	}

	UI::play_queued_draw_animations();
}

void CharacterComp::play_card(u8 hand_index, entt::entity target) {
	Card card = hand.at(hand_index);
	hand.erase(hand.begin() + hand_index);

	played_card.emplace();
	played_card.value().card = card;
	played_card.value().bars_until_activate = card.data->cost;
	played_card.value().target = target;
	card.data->play(*this, target);

	UI::destroy_hand_visual(*this, hand_index);
}

void CharacterComp::on_bar_end() {
	if (!played_card.has_value()) {
		return;
	}

	played_card.value().bars_until_activate--;

	if (played_card.value().bars_until_activate <= 0) {
		played_card.value().card.data->activate(*this, played_card.value().target);
		played_card.reset();
	}
}

void CharacterComp::on_turn_start() {
	draw();
}

bool is_in_combat() {
	return ecs.ctx().contains<CombatSingleton>();
}

void update_combat() {
	if (!is_in_combat()) {
		return;
	}

	get_combat().update();
}

void start_combat() {
	FixedList<entt::entity, 20> characters{};
	auto view = ecs.view<CharacterDataComp>();
	for (auto [entity, data] : view.each()) { // Currently based on order in which characters were added to ecs
		auto& new_character = add_component(entity, CharacterComp{});
		new_character.entity = entity;
		new_character.init_from_data(data);
		characters.push_back(entity);
	}

	auto& combat = ecs.ctx().emplace<CombatSingleton>(characters);
	UI::start_combat();

	for (auto [entity, character] : ecs.view<CharacterComp>().each()) {
		character.draw(5);
	}

	get_combat().get_active_character()->on_turn_start();
	UI::on_turn_start();
}

void end_combat() {
	ecs.clear<CharacterComp>();
	ecs.ctx().erase<CombatSingleton>();
}

CombatSingleton& get_combat() {
	return ecs.ctx().get<CombatSingleton>();
}

void CombatSingleton::update() {
	timer += delta_time;

	if (timer >= SECONDS_PER_BAR) { // Bar ends
		timer -= SECONDS_PER_BAR;
		bar_index++;
		get_active_character()->on_bar_end();

		if (bar_index >= BARS_PER_TURN) { // Turn ends
			bar_index = 0;
			turn_index++;

			if (turn_index >= characters.size()) { // Cycle ends
				turn_index = 0;
			}

			get_active_character()->on_turn_start();
			UI::on_turn_start();
		}

		UI::on_bar_end();
	}

	UI::update_combat();
}

CharacterComp* CombatSingleton::get_active_character() {
	return &ecs.get<CharacterComp>(get_active_character_entity());
}

entt::entity CombatSingleton::get_active_character_entity() {
	return characters.at(turn_index);
}

float CombatSingleton::get_bar_progress() {
	return timer / SECONDS_PER_BAR;
}

float CombatSingleton::get_discrete_bar_progress() {
	return std::floorf(get_bar_progress() * 4.f) / 4.f;
}

std::vector<Card> make_cards(std::vector<CardID> ids) {
	std::vector<Card> result = {};
	for (CardID id : ids) result.push_back(Card{&card_data[static_cast<u8>(id)]});
	return result;
}
