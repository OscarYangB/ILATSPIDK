#include "combat_ui.h"
#include "game.h"
#include "render.h"
#include "combat.h"
#include "button.h"
#include "input.h"
#include "character_animation.h"
#include "animation.h"

#include <iostream>

static std::vector<entt::entity> gamebars{};
static std::vector<entt::entity> hand_buttons{};
static std::vector<entt::entity> hand_sprites{};
std::optional<entt::entity> dragged_card{};

void create_gamebar() {
	for (int i = 0; i < BARS_PER_TURN; i++) {
		auto entity = ecs.create();

		auto& sprite_component = ecs.emplace<SpriteComponent>(entity);
		Sprite sprite = i == 0 ? Sprite::GAMEBAR_END_1 : Sprite::GAMEBAR_START_1;
		sprite_component.sprites = {sprite, sprite};
		sprite_component.tints[0] = {0, 0, 0, 255};

		auto& transform = ecs.emplace<AnchoredTransformComponent>(entity);
		transform.x_anchor = HorizontalAnchor::CENTER;
		transform.y_anchor = VerticalAnchor::TOP;
		float width = sprite_component.visible_bounding_box().width();
		transform.relative_position = { (1 - i) * width, 0.f};

		gamebars.push_back(entity);
	}
}

void destroy_gamebar() {
	for (entt::entity entity : gamebars) {
		ecs.destroy(entity);
	}

	gamebars.clear();
}

void cycle_gamebar_animation(u8 index) {
	auto& sprite = ecs.get<SpriteComponent>(gamebars[index]);
	Sprite next_sprite;
	switch (sprite.sprites.at(0)) {
		case Sprite::GAMEBAR_START_1:
			next_sprite = Sprite::GAMEBAR_START_2;
			break;
		case Sprite::GAMEBAR_START_2:
			next_sprite = Sprite::GAMEBAR_START_3;
			break;
		case Sprite::GAMEBAR_START_3:
			next_sprite = Sprite::GAMEBAR_START_1;
			break;
		case Sprite::GAMEBAR_END_1:
			next_sprite = Sprite::GAMEBAR_END_2;
			break;
		case Sprite::GAMEBAR_END_2:
			next_sprite = Sprite::GAMEBAR_END_3;
			break;
		case Sprite::GAMEBAR_END_3:
			next_sprite = Sprite::GAMEBAR_END_1;
			break;
		default:
			break;
	}

	sprite.sprites.at(0) = next_sprite;
	sprite.sprites.at(1) = next_sprite;
}

double vibrate_timer = 0.0;
constexpr double VIBRATIONS_PER_BAR = 4;
constexpr double SECONDS_PER_VIBRATION = SECONDS_PER_BAR / VIBRATIONS_PER_BAR;

void update_gamebar() {
	for (int i = 0; i < gamebars.size(); i++) {
		auto& sprite = ecs.get<SpriteComponent>(gamebars.at(i));
		Box box = sprite.visible_bounding_box();

		if (combat.value().bar_index > i) {  // Bar is already used
			sprite.tints[1] = {255, 255, 255, 0};
			sprite.tints[0] = {0, 0, 0, 50};
		} else if (combat.value().bar_index == i) {  // Current bar
			sprite.tints[1] = {255, 255, 255, 255};
			sprite.tints[0] = {0, 0, 0, 255};
			box.right_bottom.x -= box.width() * combat.value().get_discrete_bar_progress();
		} else { // Bar hasn't been used
			sprite.tints[1] = {255, 255, 255, 255};
			sprite.tints[0] = {0, 0, 0, 255};
		}

		sprite.masks[1] = box;
	}

	vibrate_timer += delta_time;
	if (vibrate_timer > SECONDS_PER_VIBRATION) {
		vibrate_timer -= SECONDS_PER_VIBRATION;
		cycle_gamebar_animation(combat.value().bar_index);

		if (combat.value().get_discrete_bar_progress() != 0) {
			entt::entity entity = ecs.create();
			auto& sprite = ecs.emplace<SpriteComponent>(entity);
			sprite.sprites = {Sprite::GAMEBAR_TICK_EFFECT_1};
			sprite.tints[0] = {255, 255, 255, 200};
			auto& transform = ecs.emplace<AnchoredTransformComponent>(entity);
			transform = ecs.get<AnchoredTransformComponent>(gamebars.at(combat.value().bar_index));
			transform.relative_position.x += ecs.get<SpriteComponent>(gamebars.at(combat.value().bar_index)).visible_bounding_box().width() * (1.f - combat.value().get_discrete_bar_progress());
			transform.sort_order = 1;
			auto& animation = ecs.emplace<CycleAnimationComponent>(entity);
			animation.sprites = {Sprite::GAMEBAR_TICK_EFFECT_1, Sprite::GAMEBAR_TICK_EFFECT_2, Sprite::GAMEBAR_TICK_EFFECT_3, Sprite::GAMEBAR_TICK_EFFECT_4};
			animation.frequency = 12.f;
			animation.destroy_on_finish = true;
		}
	}
}

void ui_on_bar_end() {
	for (int i = 0; i < gamebars.size(); i++) {
		cycle_gamebar_animation(i);
	}

	entt::entity entity = ecs.create();
	auto& sprite = ecs.emplace<SpriteComponent>(entity);
	sprite.sprites = {Sprite::GAMEBAR_BAR_EFFECT_1};
	sprite.tints[0] = {255, 255, 255, 200};
	auto& transform = ecs.emplace<AnchoredTransformComponent>(entity);
	transform = ecs.get<AnchoredTransformComponent>(gamebars.at(combat.value().bar_index));
	auto& animation = ecs.emplace<CycleAnimationComponent>(entity);
	animation.sprites = {Sprite::GAMEBAR_BAR_EFFECT_1, Sprite::GAMEBAR_BAR_EFFECT_2, Sprite::GAMEBAR_BAR_EFFECT_3, Sprite::GAMEBAR_BAR_EFFECT_4};
	animation.frequency = 12.f;
	animation.destroy_on_finish = true;
}

void ui_start_combat() {
	create_gamebar();
}

constexpr double CARD_HOVER_WIDTH = 140.f;
constexpr double CARD_HOVER_HEIGHT = 250.f;

constexpr u16 CARD_SPRITE_WIDTH = 150;
constexpr u16 CARD_SPRITE_HEIGHT = 200;

constexpr u16 CARD_SPRITE_EXPANDED_WIDTH = 300;
constexpr u16 CARD_SPRITE_EXPANDED_HEIGHT = 400;

constexpr u16 CARD_SPRITE_SHOWN_OFFSET = 3;
constexpr u16 CARD_SPRITE_HIDDEN_OFFSET = 100;

float card_x_offset(u8 hand_size, u8 index) {
	float starting_position = -(CARD_HOVER_WIDTH * hand_size) / 2.f + (CARD_HOVER_WIDTH / 2.f);
	return starting_position + CARD_HOVER_WIDTH * index;
}

void on_card_hover(entt::entity hovered_entity) {
	auto& hovered_card = ecs.get<HandCardComponent>(hovered_entity);
	std::cout << "Card is hovered: " << hovered_card.get_card()->name.get() << "\n";

	stop_animation(hovered_card.animation_id);
	hovered_card.animation_id = start_animation_group();
	play_animation(0.04, 0.0, &AnchoredTransformComponent::width, hovered_card.sprite_entity, [](Animation& animation, u16 starting_value) {
		return smooth_curve<u16>(CARD_SPRITE_EXPANDED_WIDTH, animation, starting_value);
	});
	play_animation(0.04, 0.0, &AnchoredTransformComponent::height, hovered_card.sprite_entity, [](Animation& animation, u16 starting_value) {
		return smooth_curve<u16>(CARD_SPRITE_EXPANDED_HEIGHT, animation, starting_value);
	});
	play_animation(0.04, 0.0, &AnchoredTransformComponent::relative_position, hovered_card.sprite_entity, [](Animation& animation, Vector2 starting_value) {
		return Vector2{starting_value.x, smooth_curve<float>(CARD_SPRITE_SHOWN_OFFSET, animation, starting_value.y)};
	});
	end_animation_group();

	for (int i = 0; i < hand_sprites.size(); i++) {
		entt::entity sprite_entity = hand_sprites.at(i);
		int distance_from_hovered = std::abs(hovered_card.index - i);
		ecs.get<AnchoredTransformComponent>(sprite_entity).sort_order = hand_sprites.size() - 1 - distance_from_hovered;

		if (i != hovered_card.index) {
			auto& hand_card = ecs.get<HandCardComponent>(hand_buttons.at(i));
			stop_animation(hand_card.animation_id);

			hand_card.animation_id = start_animation_group();
			float distance_weight = distance_from_hovered / 25.f;
			play_animation(0.10, 0.0, &AnchoredTransformComponent::width, sprite_entity, [distance_weight](Animation& animation, u16 starting_value) {
				return smooth_curve<u16>(CARD_SPRITE_WIDTH - distance_weight * CARD_SPRITE_WIDTH, animation, starting_value);
			});
			play_animation(0.10, 0.0, &AnchoredTransformComponent::height, sprite_entity, [distance_weight](Animation& animation, u16 starting_value) {
				return smooth_curve<u16>(CARD_SPRITE_HEIGHT - distance_weight * CARD_SPRITE_HEIGHT, animation, starting_value);
			});
			float x_offset = i > hovered_card.index ? -200.f : 200.f;
			x_offset *= distance_weight;
			play_animation(0.10, 0.0, &AnchoredTransformComponent::relative_position, sprite_entity, [i, x_offset, distance_from_hovered](Animation& animation, Vector2 starting_value) {
				return Vector2{card_x_offset(hand_sprites.size(), i) + x_offset, smooth_curve<float>(CARD_SPRITE_SHOWN_OFFSET + 2.0f - 13.5f * distance_from_hovered, animation, starting_value.y)};
			});
			end_animation_group();
		}
	}
}

void on_card_unhover(entt::entity entity) {
	for (int i = 0; i < hand_sprites.size(); i++) {
		entt::entity sprite_entity = hand_sprites.at(i);
		auto& sprite_transform = ecs.get<AnchoredTransformComponent>(sprite_entity);

		auto& hand_card = ecs.get<HandCardComponent>(hand_buttons.at(i));
		stop_animation(hand_card.animation_id);

		hand_card.animation_id = start_animation_group();
		play_animation(0.04, 0.0, &AnchoredTransformComponent::width, sprite_entity, [](Animation& animation, u16 starting_value) {
			return smooth_curve<u16>(CARD_SPRITE_WIDTH, animation, starting_value);
		});
		play_animation(0.04, 0.0, &AnchoredTransformComponent::height, sprite_entity, [](Animation& animation, u16 starting_value) {
			return smooth_curve<u16>(CARD_SPRITE_HEIGHT, animation, starting_value);
		});
		play_animation(0.04, 0.0, &AnchoredTransformComponent::relative_position, sprite_entity, [i](Animation& animation, Vector2 starting_value) {
			return Vector2{card_x_offset(hand_sprites.size(), i), smooth_curve<float>(CARD_SPRITE_HIDDEN_OFFSET, animation, starting_value.y)};
		});
		end_animation_group();
	}
}

void on_card_click(entt::entity entity) {
	if (!combat.value().get_active_character()->played_card.has_value()) { // Can't play if a card is queued
		dragged_card = entity;
	}
}

void refresh_hand_buttons() {
	for (entt::entity entity : hand_buttons) ecs.destroy(entity);
	hand_buttons.clear();
	for (entt::entity entity : hand_sprites) ecs.destroy(entity);
	hand_sprites.clear();

	CharacterComponent* character = combat.value().get_active_character();
	for (int i = 0; i < character->hand.size(); i++) {
		entt::entity sprite_entity{};
		{ // Sprite
			entt::entity entity = ecs.create();
			sprite_entity = entity;
			Card card = character->hand.at(i);

			auto& sprite = ecs.emplace<SpriteComponent>(entity);
			switch (character->hand.at(i)->card_type) {
				case CardType::PSYCHIC: sprite.sprites = {Sprite::CARD_PSYCHIC_1, Sprite::CARD_PSYCHIC_LVL_1}; break;
				case CardType::MAGIC: sprite.sprites = {Sprite::CARD_MAGIC_1, Sprite::CARD_MAGIC_LVL_1}; break;
				case CardType::GROOVE: sprite.sprites = {Sprite::CARD_GROOVE_1, Sprite::CARD_GROOVE_LVL_1}; break;
			}

			auto& transform = ecs.emplace<AnchoredTransformComponent>(entity);
			transform.x_anchor = HorizontalAnchor::CENTER;
			transform.y_anchor = VerticalAnchor::BOTTOM;
			transform.relative_position = {card_x_offset(character->hand.size(), i), CARD_SPRITE_HIDDEN_OFFSET};
			transform.height = CARD_SPRITE_HEIGHT;
			transform.width = CARD_SPRITE_WIDTH;

			auto& name = ecs.emplace<TextComponent>(entity);
			name.text = card->name;
			name.colour = WHITE;
			name.size = 18;
			name.x_align = HorizontalAnchor::CENTER;

			// auto& description = ecs.emplace<TextComponent>(entity);
			// description.text = card->description;
			// description.colour = WHITE;
			// description.size = 18;
			// description.x_align = HorizontalAnchor::LEFT;
			// description.y_align = VerticalAnchor::BOTTOM;

			// auto& cost = ecs.emplace<TextComponent>(entity);
			// cost.text = {std::to_string(card->cost).c_str()};
			// cost.colour = WHITE;
			// cost.size = 18;
			// cost.x_align = HorizontalAnchor::RIGHT;

			hand_sprites.push_back(entity);
		}
		{ // Button
			entt::entity entity = ecs.create();

			auto& transform = ecs.emplace<AnchoredTransformComponent>(entity);
			transform.x_anchor = HorizontalAnchor::CENTER;
			transform.y_anchor = VerticalAnchor::BOTTOM;
			transform.relative_position = {card_x_offset(character->hand.size(), i), 0.f};
			transform.width = CARD_HOVER_WIDTH;
			transform.height = CARD_HOVER_HEIGHT;

			// debug
			// auto& sprite = ecs.emplace<SpriteComponent>(entity);
			// sprite.sprites = {Sprite::TEST_BUTTON};
			// sprite.tints[0] = {255, 255, 255, 50};
			//

			auto& button = ecs.emplace<ButtonComponent>(entity);
			button.on_hover = on_card_hover;
			button.on_click = on_card_click;
			button.on_unhover = on_card_unhover;

			auto& card = ecs.emplace<HandCardComponent>(entity);
			card.index = i;
			card.sprite_entity = sprite_entity;

			hand_buttons.push_back(entity);
		}
	}
}


void update_drag() {
	if (!input_held(InputType::MOUSE_CLICK) && dragged_card.has_value()) {
		auto& hand_card = ecs.get<HandCardComponent>(*dragged_card);
		auto& hand = combat.value().get_active_character()->hand;
		Card card = hand_card.get_card();

		// TODO check if dragged far enough and with UI

		// TODO targetting
		combat.value().get_active_character()->play_card(hand_card.index, {});
		dragged_card.reset();

		// TODO remove card from hand with animation
		refresh_hand_buttons();
	}
}

void ui_update_combat() {
	update_gamebar();
	update_drag();
}

void ui_end_combat() {
	end_combat();
}

void ui_on_turn_start() {
	dragged_card.reset();
	refresh_hand_buttons();
}

Card HandCardComponent::get_card() {
	// Assumes the active character
	return combat.value().get_active_character()->hand[index];
}
