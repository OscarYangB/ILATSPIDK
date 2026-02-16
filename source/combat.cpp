#include "combat.h"
#include <algorithm>
#include "game.h"

Combat combat{};
bool in_combat = false;

bool PlayedCard::is_valid() {
	return card != nullptr;
}

void PlayedCard::invalidate() {
	bars_until_activate = 0;
	card = nullptr;
	targets.clear();
}

void CharacterComponent::init_from_data(const CharacterDataComponent& new_data) {
	data = &new_data;
	health = data->starting_health;
	max_health = data->starting_health;
	shield = data->starting_shield;
	hand = {};
	status_effects = {};
	played_card = {};
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
}

void CharacterComponent::play_card(u8 hand_index, const std::vector<CharacterComponent>& targets) {
	Card card = hand[hand_index];
	hand.erase(hand.begin() + hand_index);
	played_card.card = card;
	played_card.bars_until_activate = card->cost;
	played_card.targets = targets;
	card->play(*this, targets);
}

void CharacterComponent::on_bar_end() {
	if (!played_card.is_valid()) {
		return;
	}

	played_card.bars_until_activate--;

	if (played_card.bars_until_activate <= 0) {
		played_card.card->activate(*this, played_card.targets);
		played_card.invalidate();
	}
}

void CharacterComponent::on_turn_start() {
	draw();
}

void start_combat(const Characters& characters) {
	in_combat = true;
	combat = {characters, 0};
}

void Combat::update() {
	timer += delta_time;

	if (timer >= SECONDS_PER_BAR) { // Bar ends
		timer -= SECONDS_PER_BAR;
		bar_index++;
		characters[turn_index].on_bar_end();

		if (bar_index >= BARS_PER_TURN) { // Turn ends
			bar_index = 0;
			turn_index++;
			characters[turn_index].on_turn_start();

			if (turn_index >= characters.size()) { // Cycle ends
				turn_index = 0;
			}
		}
	}
}

std::vector<Card> make_cards(std::vector<CardID> ids) {
	std::vector<Card> result = {};
	for (CardID id : ids) result.push_back(&card_data[static_cast<u8>(id)]);
	return result;
}
