#include "combat_ui.h"
#include "game.h"
#include "render.h"
#include "combat.h"
#include "button.h"
#include "input.h"
#include "character_animation.h"
#include "animation.h"
#include "image_utils.h"
#include "movement_controller.h"

constexpr u8 HEALTHBAR_WIDTH = get_sprite_dimensions(Sprite::HEALTHBAR_OUTLINE_1).w;
constexpr u8 HEALTHBAR_HEIGHT = get_sprite_dimensions(Sprite::HEALTHBAR_OUTLINE_1).h;

void create_healthbars() {
	for (entt::entity character_entity : get_combat().characters) {
		auto& character_transform = ecs.get<TransformComponent>(character_entity);
		auto& character_component = ecs.get<CharacterComponent>(character_entity);

		auto entity = ecs.create();
		auto& transform = ecs.emplace<TransformComponent>(entity);
		transform.position = {10.f, -50.f};
		character_transform.add_child(character_entity, entity);
		auto& sprite = ecs.emplace<SpriteComponent>(entity);
		sprite.sprites = { Sprite::HEALTHBAR_OUTLINE_1, Sprite::HEALTHBAR_DYNAMIC_1, Sprite::HEALTHBAR_GOOD_1};
		switch (character_component.data->type) {
		case CharacterType::GOOD: sprite.sprites.at(2) = Sprite::HEALTHBAR_GOOD_1; break;
		case CharacterType::EVIL: sprite.sprites.at(2) = Sprite::HEALTHBAR_EVIL_1; break;
		case CharacterType::FURNITURE: sprite.sprites.at(2) = Sprite::HEALTHBAR_NEUTRAL_1; break;
		}
		sprite.masks[1] = {{0.f, 0.f}, {HEALTHBAR_WIDTH, HEALTHBAR_HEIGHT}};
		ecs.emplace<HealthbarComponent>(entity);

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
	auto view = ecs.view<HealthbarComponent>();
	ecs.destroy(view.begin(), view.end());
}

void refresh_health_bar(const CharacterComponent& character, bool is_heal) {
	float health = character.health / character.max_health;

	for (auto [entity, transform, sprite] : ecs.view<HealthbarComponent, TransformComponent, SpriteComponent>().each()) {
		if (&ecs.get<CharacterComponent>(transform.parent) == &character) {
			sprite.masks[2] = {{0.f, 0.f}, {HEALTHBAR_WIDTH * health, HEALTHBAR_HEIGHT}};
			sprite.sprites[1] = is_heal ? Sprite::HEALTHBAR_HEAL_1 : Sprite::HEALTHBAR_DYNAMIC_1;

			play_animation(0.2, 0.0, &SpriteComponent::masks, entity, [](Animation& animation, auto starting_value) {
				auto new_value = starting_value;
				float new_width = fast_start_curve(starting_value.at(2).value().width(), animation, starting_value.at(1).value().width());
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

		auto& gamebar = ecs.emplace<GamebarComponent>(entity);
		gamebar.index = i;
	}
}

void destroy_gamebar() {
	auto view = ecs.view<GamebarComponent>();
	ecs.destroy(view.begin(), view.end());
}

void cycle_gamebar_animation(u8 index) {
	auto view = ecs.view<GamebarComponent, SpriteComponent>();
	SpriteComponent* sprite{};
	for (auto [entity, gamebar, current_sprite] : view.each()) {
		if (gamebar.index == index) {
			sprite = &current_sprite;
			break;
		}
	}
	if (!sprite) {
		return;
	}
	Sprite next_sprite;
	switch (sprite->sprites.at(0)) {
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

	sprite->sprites.at(0) = next_sprite;
	sprite->sprites.at(1) = next_sprite;
}

constexpr double VIBRATIONS_PER_BAR = 4;
constexpr double SECONDS_PER_VIBRATION = SECONDS_PER_BAR / VIBRATIONS_PER_BAR;

void update_gamebar() {
	auto view = ecs.view<GamebarComponent, SpriteComponent, AnchoredTransformComponent>();
	for (auto [entity, gamebar, sprite, transform] : view.each()) {
		Box box = sprite.visible_bounding_box();

		if (get_combat().bar_index > gamebar.index) {  // Bar is already used
			sprite.tints[1] = {255, 255, 255, 0};
			sprite.tints[0] = {0, 0, 0, 50};
		} else if (get_combat().bar_index == gamebar.index) {  // Current bar
			sprite.tints[1] = {255, 255, 255, 255};
			sprite.tints[0] = {0, 0, 0, 255};
			box.right_bottom.x -= box.width() * get_combat().get_discrete_bar_progress();
		} else { // Bar hasn't been used
			sprite.tints[1] = {255, 255, 255, 255};
			sprite.tints[0] = {0, 0, 0, 255};
		}

		sprite.masks[1] = box;

		gamebar.vibrate_timer += delta_time;
		if (gamebar.vibrate_timer > SECONDS_PER_VIBRATION) {
			gamebar.vibrate_timer -= SECONDS_PER_VIBRATION;
			cycle_gamebar_animation(get_combat().bar_index);

			if (get_combat().get_discrete_bar_progress() != 0) {
				if (gamebar.index == get_combat().bar_index) {
					entt::entity fx_entity = ecs.create();
					auto& fx_sprite = ecs.emplace<SpriteComponent>(fx_entity);
					fx_sprite.sprites = {Sprite::GAMEBAR_TICK_EFFECT_1};
					fx_sprite.tints[0] = {255, 255, 255, 200};
					auto& fx_transform = ecs.emplace<AnchoredTransformComponent>(fx_entity);
					fx_transform = transform;
					fx_transform.relative_position.x += sprite.visible_bounding_box().width() * (1.f - get_combat().get_discrete_bar_progress());
					fx_transform.sort_order = 1;
					auto& fx_animation = ecs.emplace<CycleAnimationComponent>(fx_entity);
					fx_animation.sprites = {Sprite::GAMEBAR_TICK_EFFECT_1, Sprite::GAMEBAR_TICK_EFFECT_2, Sprite::GAMEBAR_TICK_EFFECT_3, Sprite::GAMEBAR_TICK_EFFECT_4};
					fx_animation.frequency = 12.f;
					fx_animation.finish_behaviour = FinishBehaviour::DESTROY_ENTITY;
				}
			}
		}
	}
}

void ui_on_bar_end() {
	for (int i = 0; i < BARS_PER_TURN; i++) {
		cycle_gamebar_animation(i);
	}

	AnchoredTransformComponent* gamebar_transform{};
	auto view = ecs.view<GamebarComponent, AnchoredTransformComponent>();
	for (auto [entity, gamebar, transform] : view.each()) {
		if (gamebar.index == get_combat().bar_index) {
			gamebar_transform = &transform;
		}
	}
	if (gamebar_transform == nullptr) {
		return;
	}

	entt::entity entity = ecs.create();
	auto& sprite = ecs.emplace<SpriteComponent>(entity);
	sprite.sprites = {Sprite::GAMEBAR_BAR_EFFECT_1};
	sprite.tints[0] = {255, 255, 255, 200};
	auto& transform = ecs.emplace<AnchoredTransformComponent>(entity);
	transform = *gamebar_transform;
	auto& animation = ecs.emplace<CycleAnimationComponent>(entity);
	animation.sprites = {Sprite::GAMEBAR_BAR_EFFECT_1, Sprite::GAMEBAR_BAR_EFFECT_2, Sprite::GAMEBAR_BAR_EFFECT_3, Sprite::GAMEBAR_BAR_EFFECT_4};
	animation.frequency = 12.f;
	animation.finish_behaviour = FinishBehaviour::DESTROY_ENTITY;

	// TEST
	CharacterComponent& character = ecs.get<CharacterComponent>(get_combat().characters.at(0));
	character.damage(40.f);
	refresh_health_bar(character, false);
	// TEST
}

void ui_start_combat() {
	create_gamebar();
	create_healthbars();

	auto view = ecs.view<CharacterComponent, PlayerMovementComponent>();
	for (auto [entity, character, movement] : view.each()) {
		movement.direction = CharacterDirection::DOWN;
	}
	input_mode_stack.push(InputMode::COMBAT);
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
	auto& hovered_button = ecs.get<HandButtonComponent>(hovered_entity);
	u8 index = hovered_button.index;
	u8 hand_size = get_combat().get_active_character()->hand.size();

	entt::entity card_entity = entt::null;
	HandCardComponent* hovered_card = nullptr;
	auto view = ecs.view<HandCardComponent, AnchoredTransformComponent>();
	for (auto [entity, card, transform] : view.each()) {
		if (card.index == index) {
			card_entity = entity;
			hovered_card = &card;
		}
	}
	assert(hovered_card != nullptr);

	ecs.get<AnchoredTransformComponent>(hovered_entity).height = CARD_HOVER_EXPANDED_HEIGHT;

	stop_animation(hovered_card->animation_id);
	hovered_card->animation_id = start_animation_group();
	play_animation(0.04, 0.0, &AnchoredTransformComponent::scale, card_entity, [](Animation& animation, float starting_value) {
		return smooth_curve(2.f, animation, starting_value);
	});
	play_animation(0.04, 0.0, &AnchoredTransformComponent::relative_position, card_entity, [index, hand_size](Animation& animation, Vector2 starting_value) {
		float x_target = card_x_offset(hand_size, index);
		return Vector2{smooth_curve(x_target, animation, starting_value.x), smooth_curve<float>(CARD_SPRITE_SHOWN_OFFSET, animation, starting_value.y)};
	});
	end_animation_group();

	for (auto [entity, card, transform] : view.each()) {
		u8 current_index = card.index;
		int distance_from_hovered = std::abs(current_index - index);
		transform.sort_order = hand_size - 1 - distance_from_hovered;

		if (current_index != index) {
			stop_animation(card.animation_id);

			card.animation_id = start_animation_group();
			float distance_weight = distance_from_hovered / 25.f;
			play_animation(0.06, 0.0, &AnchoredTransformComponent::scale, entity, [distance_weight](Animation& animation, float starting_value) {
				return smooth_curve(1.f - distance_weight * 1.f, animation, starting_value);
			});
			float x_offset = current_index > index ? 70.f : -70.f;
			float x_offset_coeff = current_index > index ? -60.f : 60.f;
			x_offset += x_offset_coeff * (distance_from_hovered / 2.5f);
			play_animation(0.10, 0.0, &AnchoredTransformComponent::relative_position, entity, [current_index, x_offset, distance_from_hovered, hand_size](Animation& animation, Vector2 starting_value) {
				float x_target = card_x_offset(hand_size, current_index) + x_offset;
				float y_target = CARD_SPRITE_SHOWN_OFFSET + 2.0f - 12.f * distance_from_hovered;
				return Vector2{smooth_curve(x_target, animation, starting_value.x), smooth_curve(y_target, animation, starting_value.y)};
			});
			end_animation_group();
		}
	}
}

void on_card_unhover(entt::entity entity) {
	u8 hand_size = get_combat().get_active_character()->hand.size();

	for (auto [entity, card, transform] : ecs.view<HandCardComponent, AnchoredTransformComponent>().each()) {
		u8 index = card.index;
		stop_animation(card.animation_id);

		card.animation_id = start_animation_group();
		play_animation(0.04, 0.0, &AnchoredTransformComponent::scale, entity, [](Animation& animation, float starting_value) {
			return smooth_curve(1.f, animation, starting_value);
		});
		play_animation(0.04, 0.0, &AnchoredTransformComponent::relative_position, entity, [hand_size, index](Animation& animation, Vector2 starting_value) {
			float x_target = card_x_offset(hand_size, index);
			return Vector2{smooth_curve(x_target, animation, starting_value.x), smooth_curve<float>(CARD_SPRITE_HIDDEN_OFFSET, animation, starting_value.y)};
		});
		end_animation_group();
	}

	ecs.get<AnchoredTransformComponent>(entity).height = CARD_HOVER_HEIGHT;
}

void on_card_click(entt::entity entity) {
	if (get_combat().get_active_character()->played_card.has_value()) { // Can't play if a card is queued
		return;
	}

	u8 index = ecs.get<HandButtonComponent>(entity).index;

	HandCardComponent* hovered_card = nullptr;
	auto view = ecs.view<HandCardComponent, AnchoredTransformComponent>();
	for (auto [entity, card, transform] : view.each()) {
		if (card.index == index) {
			hovered_card = &card;
		}
	}
	assert(hovered_card != nullptr);

	hovered_card->is_dragged = true;
}

// 1. HandCardComponent and unpositioned visuals are created/destroyed whenever hand state changes: ui_add_hand_visual() ui_destroy_hand_visual()
// 2. Buttons are destroyed and recreated whenever active hand state changes: refresh_hand_buttons()
// 3. visuals are repositioned when turn changes or hand state changes: position_hand_visuals()

void refresh_hand_buttons() {
	auto view = ecs.view<HandButtonComponent>();
	ecs.destroy(view.begin(), view.end());

	CharacterComponent* character = get_combat().get_active_character();
	for (int i = 0; i < character->hand.size(); i++) {
		entt::entity entity = ecs.create();

		auto& transform = ecs.emplace<AnchoredTransformComponent>(entity);
		transform.x_anchor = HorizontalAnchor::CENTER;
		transform.y_anchor = VerticalAnchor::BOTTOM;
		transform.relative_position = {card_x_offset(character->hand.size(), i), 0.f};
		transform.width = CARD_HOVER_WIDTH;
		transform.height = CARD_HOVER_HEIGHT;

		auto& button = ecs.emplace<ButtonComponent>(entity);
		button.on_hover = on_card_hover;
		button.on_click = on_card_click;
		button.on_unhover = on_card_unhover;

		auto& hand_button = ecs.emplace<HandButtonComponent>(entity);
		hand_button.index = i;
	}
}

void ui_play_queued_draw_animations() {
	entt::entity character_entity = get_combat().get_active_character_entity();
	CharacterComponent* character = get_combat().get_active_character();
	auto view = ecs.view<HandCardComponent>();
	u8 counter = view.size() - 1;

	for (auto [card_entity, card] : view.each()) {
		if (card.owning_character != character_entity) {
			continue;
		}

		if (!card.queue_draw_animation) {
			continue;
		}

		CardType type = character->hand.at(card.index)->card_type;

		constexpr double DURATION = 0.2;
		double delay = 0.02 * counter;

		entt::entity fx_entity = ecs.create();
		ecs.emplace<AnchoredTransformComponent>(fx_entity, AnchoredTransformComponent{.x_anchor = HorizontalAnchor::CENTER, .y_anchor = VerticalAnchor::BOTTOM,
																					  .relative_position = {card_x_offset(character->hand.size(), card.index) + 400.f, -100.f},
																					  .width = CARD_SPRITE_WIDTH, .height = CARD_SPRITE_HEIGHT, .sort_order = 1});
		auto& sprite = ecs.emplace<SpriteComponent>(fx_entity, SpriteComponent{.sprites = {Sprite::NONE}});

		play_animation(DURATION, delay, &AnchoredTransformComponent::relative_position, fx_entity, [card_entity](Animation& animation, Vector2 starting_value) {
			Vector2 target = ecs.get<AnchoredTransformComponent>(card_entity).relative_position;
			return Vector2{smooth_curve(target.x, animation, starting_value.x), smooth_curve(target.y, animation, starting_value.y)};
		});

		play_animation(DURATION, delay, &AnchoredTransformComponent::scale, fx_entity, [card_entity](Animation& animation, float starting_value) {
			float target = ecs.get<AnchoredTransformComponent>(card_entity).scale;
			return smooth_curve(target, animation, starting_value);
		});

		auto& animation = ecs.emplace<CycleAnimationComponent>(fx_entity);
		switch (type) {
		case CardType::PSYCHIC: animation.sprites = {Sprite::CARD_PSYCHIC_2, Sprite::CARD_PSYCHIC_3, Sprite::CARD_PSYCHIC_4, Sprite::CARD_PSYCHIC_5, Sprite::CARD_PSYCHIC_6,Sprite::CARD_PSYCHIC_7, Sprite::CARD_PSYCHIC_8, Sprite::CARD_PSYCHIC_9, Sprite::CARD_PSYCHIC_10 }; break;
		case CardType::MAGIC: animation.sprites = {Sprite::CARD_MAGIC_2, Sprite::CARD_MAGIC_3, Sprite::CARD_MAGIC_4, Sprite::CARD_MAGIC_5, Sprite::CARD_MAGIC_6,Sprite::CARD_MAGIC_7, Sprite::CARD_MAGIC_8, Sprite::CARD_MAGIC_9, Sprite::CARD_MAGIC_10 }; break;
		case CardType::GROOVE: animation.sprites = {Sprite::CARD_GROOVE_2, Sprite::CARD_GROOVE_3, Sprite::CARD_GROOVE_4, Sprite::CARD_GROOVE_5, Sprite::CARD_GROOVE_6,Sprite::CARD_GROOVE_7, Sprite::CARD_GROOVE_8, Sprite::CARD_GROOVE_9, Sprite::CARD_GROOVE_10 }; break;
		}
		animation.finish_behaviour = FinishBehaviour::DESTROY_ENTITY;
		animation.frequency = 1.0 / ((DURATION) / (double)animation.sprites.size());
		animation.delay = delay;

		play_animation(DURATION + delay - 0.01, 0.f, &SpriteComponent::visible, card_entity, [](Animation& animation, bool starting_value) {
			return animation.is_finished();
		});

		counter--;
		card.queue_draw_animation = false;
	}
}

void position_hand_visuals(bool from_current_position) {
	entt::entity character = get_combat().get_active_character_entity();
	CharacterComponent* character_component = get_combat().get_active_character();
	for (auto [entity, card, transform, sprite] : ecs.view<HandCardComponent, AnchoredTransformComponent, SpriteComponent>().each()) {
		if (card.owning_character != character) {
			sprite.visible = false;
			continue;
		}

		sprite.visible = true;
		float x_position = card_x_offset(character_component->hand.size(), card.index);
		Vector2 start_position = from_current_position ? transform.relative_position : Vector2{x_position, CARD_SPRITE_HIDDEN_OFFSET + 100.f};
		Vector2 new_position = {x_position, CARD_SPRITE_HIDDEN_OFFSET};

		play_animation(0.04, 0.0, &AnchoredTransformComponent::relative_position, entity, [new_position, start_position](Animation& animation, Vector2 starting_value) {
			return fast_start_curve(new_position, animation, start_position);
		});
	}
}

void ui_add_hand_visual(const CharacterComponent& character, u8 index) {
	Card card = character.hand.at(index);

	entt::entity entity = ecs.create();
	auto& transform = ecs.emplace<AnchoredTransformComponent>(entity, AnchoredTransformComponent{.x_anchor = HorizontalAnchor::CENTER, .y_anchor = VerticalAnchor::BOTTOM,
																								 .width = CARD_SPRITE_WIDTH, .height = CARD_SPRITE_HEIGHT });
	ecs.emplace<SpriteComponent>(entity); // To access visibility for children
	ecs.emplace<HandCardComponent>(entity, HandCardComponent{.index = index, .owning_character = character.entity, .queue_draw_animation = true});
	{ // Sprite Children
		entt::entity art = ecs.create();
		ecs.emplace<AnchoredTransformComponent>(art, AnchoredTransformComponent{.x_anchor = HorizontalAnchor::CENTER,
																				.relative_position = {0.f, 20.f}, .width = 136, .height = 96,});
		transform.add_child(entity, art);
		ecs.emplace<SpriteComponent>(art, SpriteComponent{.sprites = {Sprite::TEST_BACKGROUND}});

		entt::entity frame = ecs.create();
		ecs.emplace<AnchoredTransformComponent>(frame, AnchoredTransformComponent{.width = CARD_SPRITE_WIDTH, .height = CARD_SPRITE_HEIGHT});
		transform.add_child(entity, frame);
		auto& sprite = ecs.emplace<SpriteComponent>(frame);
		switch (card->card_type) {
		case CardType::PSYCHIC: sprite.sprites = {Sprite::CARD_PSYCHIC_1, Sprite::CARD_PSYCHIC_LVL_1}; break;
		case CardType::MAGIC: sprite.sprites = {Sprite::CARD_MAGIC_1, Sprite::CARD_MAGIC_LVL_1}; break;
		case CardType::GROOVE: sprite.sprites = {Sprite::CARD_GROOVE_1, Sprite::CARD_GROOVE_LVL_1}; break;
		}

		entt::entity name = ecs.create();
		ecs.emplace<AnchoredTransformComponent>(name, AnchoredTransformComponent{.relative_position = {0.f, 10.f},
																				 .width = CARD_SPRITE_WIDTH, .height = 200});
		transform.add_child(entity, name);
		ecs.emplace<TextComponent>(name, TextComponent{.text = card->name, .colour = WHITE, .size = 24, .x_align = HorizontalAnchor::CENTER});

		entt::entity description = ecs.create();
		constexpr float DESCRIPTION_MARGIN = 10.f;
		ecs.emplace<AnchoredTransformComponent>(description, AnchoredTransformComponent{.relative_position = {DESCRIPTION_MARGIN, 115.f},
																						.width = CARD_SPRITE_WIDTH - 2 * (u16)DESCRIPTION_MARGIN, .height = 50});
		transform.add_child(entity, description);
		ecs.emplace<TextComponent>(description, TextComponent{.text = card->description, .colour = WHITE, .size = 16});

		entt::entity cost = ecs.create();
		ecs.emplace<AnchoredTransformComponent>(cost, AnchoredTransformComponent{.relative_position = {123.5, 7.f}, .width = CARD_SPRITE_WIDTH,
																				 .height = 200});
		transform.add_child(entity, cost);
		ecs.emplace<TextComponent>(cost, TextComponent{.text {number_to_string(card->cost)}, .colour = BLACK, .size = 32});
	}

	if (character.entity == get_combat().get_active_character_entity()) {
		refresh_hand_buttons();
		position_hand_visuals(true);
	}
}

void ui_destroy_hand_visual(const CharacterComponent& character, u8 index) {
	auto view = ecs.view<HandCardComponent>();

	for (auto [entity, card] : view.each()) {
		if (card.owning_character == character.entity && card.index > index) {
			card.index--;
		}
	}

	for (auto [entity, card] : view.each()) {
		auto& owning_character = ecs.get<CharacterComponent>(card.owning_character);
		if (card.owning_character == character.entity && card.index == index) {
			ecs.destroy(entity);
			break;
		}
	}

	if (character.entity == get_combat().get_active_character_entity()) {
		refresh_hand_buttons();
		position_hand_visuals(true);
	}
}

void update_drag() {
	HandCardComponent* dragged_card = nullptr;
	for (auto [entity, card] : ecs.view<HandCardComponent>().each()) {
		if (card.is_dragged) {
			dragged_card = &card;
		}
	}

	if (!input_held(InputType::MOUSE_CLICK) && dragged_card) {
		Card card = dragged_card->get_card();

		// TODO check if dragged far enough and with UI

		// TODO targetting
		get_combat().get_active_character()->play_card(dragged_card->index, {});
		dragged_card->is_dragged = false;

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
	input_mode_stack.pop();
}

void ui_on_turn_start() {
	for (auto [entity, card] : ecs.view<HandCardComponent>().each()) card.is_dragged = false;
	refresh_hand_buttons();
	ui_play_queued_draw_animations();
	position_hand_visuals(false);
}

Card HandCardComponent::get_card() {
	// Assumes the active character
	return get_combat().get_active_character()->hand.at(index);
}
