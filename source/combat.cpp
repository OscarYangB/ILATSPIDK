#include "combat.h"
#include <algorithm>
#include "game.h"
#include "card_data.h"
#include "random.h"
#include "render.h"
#include "animation.h"

void CharacterComp::init_from_data(const CharacterDataComp& new_data) {
	data = &new_data;
	health = data->starting_health;
	max_health = data->starting_health;
	shield = data->starting_shield;
	deck = data->starting_deck;
	std::shuffle(deck.begin(), deck.end(), random_generator);
	hand = {};
	status_effects = {};
	played_card = std::nullopt;
}

void CharacterComp::heal(float amount) {
	if (health == 0.f && amount > 0.f) {
		stop_animation(low_health_animation_id);
		ecs.get<SpriteComp>(entity).tint = Colour::white();
		low_health_animation_id = 0;
	}
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
	if (health == 0.f) {
		if (data->type == CharacterType::GOOD && low_health_animation_id == 0) {
			low_health_animation_id = play_animation(0.0, 0.0, &SpriteComp::tint, entity, [](Animation& animation, Colour starting_value) {
				u8 value = sinusoid_curve(100.0, 2.0, 0.0, animation, 100.0);
				return Colour{255, value, value, 255};
			});
		} else if (data->type != CharacterType::GOOD) {
			get_combat().kill_zero_health_characters(CharacterType::EVIL | CharacterType::FURNITURE);
		}
	}
}

void CharacterComp::die() {
	// TODO Death animation
	ecs.get<SpriteComp>(entity).visible = false;
}

void CharacterComp::draw(u8 amount) {
	if (data->type != CharacterType::GOOD) {
		return;
	}

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
	queue_card(card, target);
	UI::destroy_hand_visual(*this, hand_index);
}

void CharacterComp::queue_card(Card card, entt::entity target) {
	played_card.emplace();
	played_card.value().card = card;
	played_card.value().bars_until_activate = card.data->cost;
	played_card.value().target = target;
	card.data->play(*this, ecs.get<CharacterComp>(target));
}

void CharacterComp::queue_random_card() {
	assert(data->type != CharacterType::GOOD);
	assert(!deck.empty());
	Card card = deck[random_integer(0, deck.size() - 1)];
	std::vector<entt::entity> valid_targets;
	for (auto [entity, character] : ecs.view<CharacterComp>().each()) {
		if (is_valid_target(*this, character, card)) {
			valid_targets.push_back(entity);
		}
	}
	queue_card(card, valid_targets[random_integer(0, valid_targets.size() - 1)]);
}

void CharacterComp::on_bar_end() {
	if (played_card.has_value()) {
		played_card.value().bars_until_activate--;

		if (played_card.value().bars_until_activate <= 0) {
			played_card.value().card.data->activate(*this, ecs.get<CharacterComp>(played_card.value().target));
			played_card.reset();
		}
	}

	if (!played_card.has_value() && data->type != CharacterType::GOOD) {
		queue_random_card();
	}
}

void CharacterComp::on_turn_start() {
	draw();

	if (data->type != CharacterType::GOOD) {
		queue_random_card();
	}
}

bool is_valid_target(const CharacterComp& playing_character, const CharacterComp& target_character, const Card& card) {
	auto valid_target_bitmask = playing_character.data->type == CharacterType::GOOD ? card.data->valid_target_bitmask : card.data->ai_target_bitmask;
	if (valid_target_bitmask == 0) {
		if (target_character.entity != get_combat().get_active_character_entity()) {
			return false;
		}
	} else if ((target_character.data->type & valid_target_bitmask) != target_character.data->type) {
		return false;
	}

	return true;
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

void sort_characters() {
	std::sort(get_combat().characters.begin(), get_combat().characters.end(), [](entt::entity first, entt::entity second) {
		return ecs.get<TransformComp>(first).position.x < ecs.get<TransformComp>(second).position.x;
	});
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
	sort_characters();
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
				kill_zero_health_characters(CharacterType::GOOD);
				sort_characters();
			}

			get_active_character()->on_turn_start();
			UI::on_turn_start();
		}

		UI::on_bar_end();
	}

	UI::update_combat();
	check_combat_end();
}

u8 CombatSingleton::get_bars_available() {
	return BARS_PER_TURN - bar_index;
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

void CombatSingleton::kill_zero_health_characters(u8 type_bitmask) {
	characters.erase_if([type_bitmask](entt::entity character) {
		auto& character_component = ecs.get<CharacterComp>(character);
		if ((character_component.data->type & type_bitmask) != 0 && character_component.health <= 0.f) {
			character_component.die();
			return true;
		}
		return false;
	});
}

void CombatSingleton::check_combat_end() {
	u8 good_count = 0;
	u8 evil_count = 0;
	for (entt::entity character : characters) {
		auto& character_component = ecs.get<CharacterComp>(character);
		if (character_component.health > 0.f) {
			if (character_component.data->type == CharacterType::GOOD) good_count++;
			if (character_component.data->type == CharacterType::EVIL) evil_count++;
		}
	}
	if (good_count == 0 || evil_count == 0) {
		// TODO win/lose screen
		end_combat();

		if (good_count == 0) {
			ecs.clear();
		}
	}
}

std::vector<Card> make_cards(std::vector<CardID> ids) {
	std::vector<Card> result = {};
	for (CardID id : ids) result.push_back(Card{&card_data[static_cast<u8>(id)]});
	return result;
}

bool Card::can_play() {
	return data->cost <= get_combat().get_bars_available() && !get_combat().get_active_character()->played_card.has_value();
}
