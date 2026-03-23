#include "combat_ui.h"
#include "game.h"
#include "render.h"
#include "combat.h"
#include "button.h"
#include "input.h"
#include "character_animation.h"
#include "animation.h"
#include "image_utils.h"

#include <iostream>

static std::vector<entt::entity> gamebars{};
static std::vector<entt::entity> hand_buttons{};
static std::vector<entt::entity> hand_sprites{};
static std::optional<entt::entity> dragged_card{};
static std::vector<u8> draw_animation_queue{};
static std::vector<entt::entity> healthbars{};

constexpr u8 HEALTHBAR_WIDTH = get_sprite_dimensions(Sprite::HEALTHBAR_OUTLINE_1).w;
constexpr u8 HEALTHBAR_HEIGHT = get_sprite_dimensions(Sprite::HEALTHBAR_OUTLINE_1).h;

void create_healthbars() {
	healthbars.clear();
	healthbars.reserve(combat->characters.size());

	for (entt::entity character_entity : combat->characters) {
		auto& character_transform = ecs.get<TransformComponent>(character_entity);
		auto& character_component = ecs.get<CharacterComponent>(character_entity);

		auto entity = ecs.create();
		auto& transform = ecs.emplace<TransformComponent>(entity);
		transform.position = {0.f, -50.f};
		character_transform.add_child(character_entity, entity);
		auto& sprite = ecs.emplace<SpriteComponent>(entity);
		sprite.sprites = { Sprite::HEALTHBAR_OUTLINE_1, Sprite::HEALTHBAR_DYNAMIC_1, Sprite::HEALTHBAR_GOOD_1};
		switch (character_component.data->type) {
		case CharacterType::GOOD: sprite.sprites.at(2) = Sprite::HEALTHBAR_GOOD_1; break;
		case CharacterType::EVIL: sprite.sprites.at(2) = Sprite::HEALTHBAR_EVIL_1; break;
		case CharacterType::FURNITURE: sprite.sprites.at(2) = Sprite::HEALTHBAR_NEUTRAL_1; break;
		}
		sprite.masks[1] = {{0.f, 0.f}, {HEALTHBAR_WIDTH, HEALTHBAR_HEIGHT}};
		healthbars.push_back(entity);

		float health_units = character_component.max_health / 100.f;
		int number_of_health_units = std::ceil(health_units);
		float health_unit_width = HEALTHBAR_WIDTH / health_units;
		float divider_offset = (health_units - number_of_health_units) * health_unit_width;
		for (int i = 1; i < number_of_health_units; i++) {
			float x_position = (float)i * health_unit_width;
			if (HEALTHBAR_WIDTH - x_position < 10.f) break;
			entt::entity divider = ecs.create();
			auto& divider_transform = ecs.emplace<TransformComponent>(divider);
			transform.add_child(entity, divider);
			divider_transform.position.x = x_position;
			auto& divider_sprite = ecs.emplace<SpriteComponent>(divider);
			divider_sprite.sprites = {Sprite::HEALTHBAR_DIVIDER_1};
		}
	}
}

void destroy_healthbars() {
	for (entt::entity healthbar : healthbars) ecs.destroy(healthbar);
	healthbars.clear();
}

void refresh_health_bar(entt::entity character) {
	CharacterComponent character_component = ecs.get<CharacterComponent>(character);
	float health = character_component.health / character_component.max_health;

	for (entt::entity entity : healthbars) {
		auto& transform = ecs.get<TransformComponent>(entity);
		if (transform.parent == character) {
			auto& sprite = ecs.get<SpriteComponent>(entity);
			sprite.masks[2] = {{0.f, 0.f}, {HEALTHBAR_WIDTH * health, HEALTHBAR_HEIGHT}};

			play_animation(0.2, 0.0, &SpriteComponent::masks, entity, [](Animation& animation, std::unordered_map<u8, Box> starting_value) {
				std::unordered_map<u8, Box> new_value = starting_value;
				float new_width = fast_start_curve(starting_value[2].width(), animation, starting_value[1].width());
				new_value[1] = {{0.f, 0.f}, {new_width, HEALTHBAR_HEIGHT}};
				return new_value;
			});
			break;
		}
	}
}

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
	for (entt::entity gamebar : gamebars) ecs.destroy(gamebar);
	gamebars.clear();
}

void cycle_gamebar_animation(u8 index) {
	auto& sprite = ecs.get<SpriteComponent>(gamebars.at(index));
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
			animation.finish_behaviour = FinishBehaviour::DESTROY_ENTITY;
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
	animation.finish_behaviour = FinishBehaviour::DESTROY_ENTITY;

	// TEST
	ecs.get<CharacterComponent>(combat->characters.at(1)).damage(100.f);
	refresh_health_bar(combat->characters.at(1));
	// TEST
}

void ui_start_combat() {
	create_gamebar();
	create_healthbars();
}

constexpr double CARD_HOVER_WIDTH = 145.f;
constexpr double CARD_HOVER_HEIGHT = 200.f;
constexpr double CARD_HOVER_EXPANDED_HEIGHT = 250.f;

constexpr u16 CARD_SPRITE_WIDTH = 150;
constexpr u16 CARD_SPRITE_HEIGHT = 200;

constexpr u16 CARD_SPRITE_SHOWN_OFFSET = 3;
constexpr u16 CARD_SPRITE_HIDDEN_OFFSET = 100;

float card_x_offset(u8 hand_size, u8 index) {
	float starting_position = -(CARD_HOVER_WIDTH * hand_size) / 2.f + (CARD_HOVER_WIDTH / 2.f);
	return starting_position + CARD_HOVER_WIDTH * index;
}

void on_card_hover(entt::entity hovered_entity) {
	auto& hovered_card = ecs.get<HandCardComponent>(hovered_entity);
	u8 index = hovered_card.index;
	std::cout << "Card is hovered: " << hovered_card.get_card()->name.get() << "\n";

	ecs.get<AnchoredTransformComponent>(hovered_entity).height = CARD_HOVER_EXPANDED_HEIGHT;

	stop_animation(hovered_card.animation_id);
	hovered_card.animation_id = start_animation_group();
	play_animation(0.04, 0.0, &AnchoredTransformComponent::scale, hovered_card.sprite_entity, [](Animation& animation, float starting_value) {
		return smooth_curve(2.f, animation, starting_value);
	});
	play_animation(0.04, 0.0, &AnchoredTransformComponent::relative_position, hovered_card.sprite_entity, [index](Animation& animation, Vector2 starting_value) {
		float x_target = card_x_offset(hand_sprites.size(), index);
		return Vector2{smooth_curve(x_target, animation, starting_value.x), smooth_curve<float>(CARD_SPRITE_SHOWN_OFFSET, animation, starting_value.y)};
	});
	end_animation_group();

	for (int i = 0; i < hand_sprites.size(); i++) {
		entt::entity sprite_entity = hand_sprites.at(i);
		int distance_from_hovered = std::abs(index - i);
		ecs.get<AnchoredTransformComponent>(sprite_entity).sort_order = hand_sprites.size() - 1 - distance_from_hovered;

		if (i != index) {
			auto& hand_card = ecs.get<HandCardComponent>(hand_buttons.at(i));
			stop_animation(hand_card.animation_id);

			hand_card.animation_id = start_animation_group();
			float distance_weight = distance_from_hovered / 25.f;
			play_animation(0.06, 0.0, &AnchoredTransformComponent::scale, sprite_entity, [distance_weight](Animation& animation, float starting_value) {
				return smooth_curve(1.f - distance_weight * 1.f, animation, starting_value);
			});
			float x_offset = i > index ? 70.f : -70.f;
			float x_offset_coeff = i > index ? -60.f : 60.f;
			x_offset += x_offset_coeff * (distance_from_hovered / 2.5f);
			// float x_offset = i > index ? -200.f : 200.f;
			// x_offset *= distance_weight;
			play_animation(0.10, 0.0, &AnchoredTransformComponent::relative_position, sprite_entity, [i, x_offset, distance_from_hovered](Animation& animation, Vector2 starting_value) {
				float x_target = card_x_offset(hand_sprites.size(), i) + x_offset;
				float y_target = CARD_SPRITE_SHOWN_OFFSET + 2.0f - 12.f * distance_from_hovered;
				return Vector2{smooth_curve(x_target, animation, starting_value.x), smooth_curve(y_target, animation, starting_value.y)};
			});
			end_animation_group();
		}
	}
}

void on_card_unhover(entt::entity entity) {
	for (int i = 0; i < hand_sprites.size(); i++) {
		entt::entity sprite_entity = hand_sprites.at(i);

		auto& hand_card = ecs.get<HandCardComponent>(hand_buttons.at(i));
		stop_animation(hand_card.animation_id);

		hand_card.animation_id = start_animation_group();
		play_animation(0.04, 0.0, &AnchoredTransformComponent::scale, sprite_entity, [](Animation& animation, float starting_value) {
			return smooth_curve(1.f, animation, starting_value);
		});
		play_animation(0.04, 0.0, &AnchoredTransformComponent::relative_position, sprite_entity, [i](Animation& animation, Vector2 starting_value) {
			float x_target = card_x_offset(hand_sprites.size(), i);
			return Vector2{smooth_curve(x_target, animation, starting_value.x), smooth_curve<float>(CARD_SPRITE_HIDDEN_OFFSET, animation, starting_value.y)};
		});
		end_animation_group();

		ecs.get<AnchoredTransformComponent>(hand_buttons.at(i)).height = CARD_HOVER_HEIGHT;
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
			Card card = character->hand.at(i);

			entt::entity entity = ecs.create();
			sprite_entity = entity;
			hand_sprites.push_back(entity);
			auto& transform = ecs.emplace<AnchoredTransformComponent>(entity);
			transform.x_anchor = HorizontalAnchor::CENTER;
			transform.y_anchor = VerticalAnchor::BOTTOM;
			transform.relative_position = {card_x_offset(character->hand.size(), i), CARD_SPRITE_HIDDEN_OFFSET};
			transform.height = CARD_SPRITE_HEIGHT; transform.width = CARD_SPRITE_WIDTH;
			auto& sprite = ecs.emplace<SpriteComponent>(entity);
			{ // Sprite Children
				entt::entity art_entity = ecs.create();
				auto& art_transform = ecs.emplace<AnchoredTransformComponent>(art_entity);
				transform.add_child(entity, art_entity);
				art_transform.width = 136; // 99
				art_transform.height = 96; // 70
				art_transform.relative_position = {0.f, 20.f};
				art_transform.x_anchor = HorizontalAnchor::CENTER;
				auto& art_sprite = ecs.emplace<SpriteComponent>(art_entity);
				art_sprite.sprites = {Sprite::TEST_BACKGROUND};

				entt::entity frame_entity = ecs.create();
				auto& frame_transform = ecs.emplace<AnchoredTransformComponent>(frame_entity);
				transform.add_child(entity, frame_entity);
				frame_transform.height = CARD_SPRITE_HEIGHT; frame_transform.width = CARD_SPRITE_WIDTH;
				auto& sprite = ecs.emplace<SpriteComponent>(frame_entity);
				switch (card->card_type) {
					case CardType::PSYCHIC: sprite.sprites = {Sprite::CARD_PSYCHIC_1, Sprite::CARD_PSYCHIC_LVL_1}; break;
					case CardType::MAGIC: sprite.sprites = {Sprite::CARD_MAGIC_1, Sprite::CARD_MAGIC_LVL_1}; break;
					case CardType::GROOVE: sprite.sprites = {Sprite::CARD_GROOVE_1, Sprite::CARD_GROOVE_LVL_1}; break;
				}

				entt::entity name_entity = ecs.create();
				auto& name_transform = ecs.emplace<AnchoredTransformComponent>(name_entity);
				name_transform.width = CARD_SPRITE_WIDTH;
				name_transform.height = 200.f;
				name_transform.relative_position = {0.f, 10.f};
				transform.add_child(entity, name_entity);
				auto& name = ecs.emplace<TextComponent>(name_entity);
				name.text = card->name;
				name.colour = WHITE;
				name.size = 24;
				name.x_align = HorizontalAnchor::CENTER;

				entt::entity description_entity = ecs.create();
				auto& description_transform = ecs.emplace<AnchoredTransformComponent>(description_entity);
				constexpr float DESCRIPTION_MARGIN = 10.f;
				description_transform.width = CARD_SPRITE_WIDTH - 2.f * DESCRIPTION_MARGIN;
				description_transform.height = 50.f;
				description_transform.relative_position = {DESCRIPTION_MARGIN, 115.f};
				transform.add_child(entity, description_entity);
				auto& description = ecs.emplace<TextComponent>(description_entity);
				description.text = card->description;
				description.colour = WHITE;
				description.size = 16;

				entt::entity cost_entity = ecs.create();
				auto& cost_transform = ecs.emplace<AnchoredTransformComponent>(cost_entity);
				cost_transform.width = CARD_SPRITE_WIDTH;
				cost_transform.height = 200.f;
				cost_transform.relative_position = {123.5f, 7.f};
				transform.add_child(entity, cost_entity);
				auto& cost = ecs.emplace<TextComponent>(cost_entity);
				cost.text = {number_to_string((card->cost))};
				cost.colour = BLACK;
				cost.size = 32;
			}
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
	destroy_gamebar();
	destroy_healthbars();
}

void play_queued_draw_animations() {
	if (draw_animation_queue.empty()) {
		return;
	}

	CharacterComponent* character = combat.value().get_active_character();

	for (int i = 0; i < draw_animation_queue.size(); i++) {
		u8 index = draw_animation_queue.at(i);
		CardType type = character->hand.at(index)->card_type;

		constexpr double DURATION = 0.2;
		double delay = 0.02 * i;

		entt::entity entity = ecs.create();
		auto& transform = ecs.emplace<AnchoredTransformComponent>(entity);
		transform.x_anchor = HorizontalAnchor::CENTER;
		transform.y_anchor = VerticalAnchor::BOTTOM;
		transform.relative_position = {card_x_offset(character->hand.size(), index) + 400.f, -100.f};
		transform.height = CARD_SPRITE_HEIGHT; transform.width = CARD_SPRITE_WIDTH;
		transform.sort_order = 1;
		auto& sprite = ecs.emplace<SpriteComponent>(entity);
		sprite.sprites = {Sprite::NONE};

		play_animation(DURATION, delay, &AnchoredTransformComponent::relative_position, entity, [index](Animation& animation, Vector2 starting_value) {
			Vector2 target = ecs.get<AnchoredTransformComponent>(hand_sprites.at(index)).relative_position;
			return Vector2{smooth_curve(target.x, animation, starting_value.x), smooth_curve(target.y, animation, starting_value.y)};
		});

		play_animation(DURATION, delay, &AnchoredTransformComponent::scale, entity, [index](Animation& animation, float starting_value) {
			float target = ecs.get<AnchoredTransformComponent>(hand_sprites.at(index)).scale;
			return smooth_curve(target, animation, starting_value);
		});

		auto& animation = ecs.emplace<CycleAnimationComponent>(entity);
		switch (type) {
			case CardType::PSYCHIC: animation.sprites = {Sprite::CARD_PSYCHIC_2, Sprite::CARD_PSYCHIC_3, Sprite::CARD_PSYCHIC_4, Sprite::CARD_PSYCHIC_5, Sprite::CARD_PSYCHIC_6,Sprite::CARD_PSYCHIC_7, Sprite::CARD_PSYCHIC_8, Sprite::CARD_PSYCHIC_9, Sprite::CARD_PSYCHIC_10 }; break;
			case CardType::MAGIC: animation.sprites = {Sprite::CARD_MAGIC_2, Sprite::CARD_MAGIC_3, Sprite::CARD_MAGIC_4, Sprite::CARD_MAGIC_5, Sprite::CARD_MAGIC_6,Sprite::CARD_MAGIC_7, Sprite::CARD_MAGIC_8, Sprite::CARD_MAGIC_9, Sprite::CARD_MAGIC_10 }; break;
			case CardType::GROOVE: animation.sprites = {Sprite::CARD_GROOVE_2, Sprite::CARD_GROOVE_3, Sprite::CARD_GROOVE_4, Sprite::CARD_GROOVE_5, Sprite::CARD_GROOVE_6,Sprite::CARD_GROOVE_7, Sprite::CARD_GROOVE_8, Sprite::CARD_GROOVE_9, Sprite::CARD_GROOVE_10 }; break;
		}
		animation.finish_behaviour = FinishBehaviour::DESTROY_ENTITY;
		animation.frequency = 1.0 / ((DURATION) / (double)animation.sprites.size());
		animation.delay = delay;

		play_animation(DURATION + delay - 0.01, 0.f, &SpriteComponent::visible, hand_sprites.at(index), [entity](Animation& animation, bool starting_value) {
			return animation.is_finished();
		});
	}

	draw_animation_queue.clear();
}


void ui_on_turn_start() {
	dragged_card.reset();
	refresh_hand_buttons();
	play_queued_draw_animations();
}

void play_draw_animation(int index) {
	draw_animation_queue.push_back(index);
}

Card HandCardComponent::get_card() {
	// Assumes the active character
	return combat.value().get_active_character()->hand.at(index);
}
