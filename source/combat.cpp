#include "combat.h"
#include <algorithm>
#include "game.h"
#include "combat_ui.h"
#include <iostream>

std::optional<Combat> combat = std::nullopt;

void CharacterComponent::init_from_data(const CharacterDataComponent& new_data) {
	data = &new_data;
	health = data->starting_health;
	max_health = data->starting_health;
	shield = data->starting_shield;
	deck = data->starting_deck;
	hand = {};
	status_effects = {};
	played_card = std::nullopt;
}

void CharacterComponent::heal(float amount) {
	health += amount;
	health = std::clamp(health, 0.f, max_health);
}

void CharacterComponent::damage(float amount) {
	float damage_to_shield = std::min(shield, amount);
	shield -= damage_to_shield;
	health -= amount - damage_to_shield;
	health = std::clamp(health, 0.f, max_health);
}

void CharacterComponent::draw() {
	if (deck.empty()) {
		return;
	}

	Card card = deck.front();
	deck.pop_back();
	hand.push_back(card);
	std::cout << "drew: " << card->name << " \n";
}

void CharacterComponent::play_card(u8 hand_index, const Characters& targets) {
	Card card = hand[hand_index];
	hand.erase(hand.begin() + hand_index);

	played_card.emplace();
	played_card->card = card;
	played_card->bars_until_activate = card->cost;
	played_card->targets = targets;
	card->play(*this, targets);
}

void CharacterComponent::on_bar_end() {
	if (!played_card.has_value()) {
		return;
	}

	played_card->bars_until_activate--;

	if (played_card->bars_until_activate <= 0) {
		played_card->card->activate(*this, played_card->targets);
		played_card.reset();
	}
}

void CharacterComponent::on_turn_start() {
	draw();
}

void update_combat() {
	if (!combat.has_value()) {
		return;
	}

	combat->update();
}

void start_combat() {
	Characters characters{};
	auto view = ecs.view<CharacterDataComponent>();
	for (auto [entity, data] : view.each()) {
		auto& new_character = ecs.emplace<CharacterComponent>(entity);
		new_character.init_from_data(data);
		characters.push_back(&new_character);
	}

	combat = {characters, 0};

	create_gamebar();

	std::cout << "combat started" << "\n";
}

void end_combat() {
	ecs.clear<CharacterComponent>();
	combat.reset();

	std::cout << "combat ended" << "\n";
}

void Combat::update() {
	timer += delta_time;

	if (timer >= SECONDS_PER_BAR) { // Bar ends
		timer -= SECONDS_PER_BAR;
		bar_index++;
		characters[turn_index]->on_bar_end();

		std::cout << "Bar has ended" << "\n";

		if (bar_index >= BARS_PER_TURN) { // Turn ends
			bar_index = 0;
			turn_index++;

			std::cout << "turn has ended" << "\n";

			if (turn_index >= characters.size()) { // Cycle ends
				turn_index = 0;
				std::cout << "cycle has ended" << "\n";
			}

			characters[turn_index]->on_turn_start();
		}
	}

	update_gamebar();
}

float Combat::get_bar_progress() {
	return timer / SECONDS_PER_BAR;
}

std::vector<Card> make_cards(std::vector<CardID> ids) {
	std::vector<Card> result = {};
	for (CardID id : ids) result.push_back(&card_data[static_cast<u8>(id)]);
	return result;
}
