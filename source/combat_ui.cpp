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
#include "animation.h"
#include "physics.h"

void create_queue_preview() {
	entt::entity queue_preview = ecs.create();
	auto& queue_transform = add_component(queue_preview, UITransformComp{.x_anchor = XAnchor::CENTER, .y_anchor = YAnchor::TOP, .sort_order = 1});
	Sprite queue_sprite{};
	add_component(queue_preview, SpriteComp{.sprites = {Sprite::NONE, Sprite::NONE}, .tint = {255, 255, 255, 140}});
	add_component(queue_preview, QueueComp{});
}

void destroy_queue_preview() {
	auto view = ecs.view<QueueComp>();
	ecs.destroy(view.begin(), view.end());
}

void set_queue_preview_info(Card card, bool is_preview, UITransformComp& transform, SpriteComp& sprite) {
	if (is_preview && !card.can_play()) {
		sprite.sprites[0] = Sprite::NONE;
		sprite.sprites[1] = Sprite::NONE;
		return;
	}
	u8 remaining_bars = is_preview ? card.data->cost : get_combat().get_active_character()->played_card.value().bars_until_activate;
	u8 gamebar_index = get_combat().bar_index + remaining_bars - 1;
	auto [gamebar_entity, gamebar] = find_component<GamebarComp>([gamebar_index](auto& gamebar){ return gamebar.index == gamebar_index; });
	Sprite new_sprite;
	switch(card.data->cost) {
	case 1: new_sprite = Sprite::QUEUE_1_1; break;
	case 2: new_sprite = Sprite::QUEUE_2_1; break;
	case 3: new_sprite = Sprite::QUEUE_3_1; break;
	case 4: new_sprite = Sprite::QUEUE_4_1; break;
	default: new_sprite = Sprite::NONE; break;
	}
	sprite.sprites[1] = Sprite::QUEUE_STAR_1;
	sprite.sprites[0] = new_sprite;
	transform.relative_position = ecs.get<UITransformComp>(gamebar_entity).relative_position;
	sprite.tint.a = is_preview ? 140 : 255;
}

void update_queue_preview() {
	auto [entity, transform, sprite] = *ecs.view<QueueComp, UITransformComp, SpriteComp>().each().begin();
	Sprite new_sprite;
	Vector2 new_position;
	if (get_combat().get_active_character()->played_card.has_value()) {
		set_queue_preview_info(get_combat().get_active_character()->played_card.value().card, false, transform, sprite);
	} else if (get_combat().ui.dragged_card != entt::null) {
		set_queue_preview_info(ecs.get<HandCardComp>(get_combat().ui.dragged_card).get_card(), true, transform, sprite);
	} else if (get_combat().ui.hovered_card != entt::null) {
		set_queue_preview_info(ecs.get<HandCardComp>(get_combat().ui.hovered_card).get_card(), true, transform, sprite);
	} else if (sprite.tint.a == 255 && sprite.sprites[1] != Sprite::NONE) {
		sprite.sprites[1] = Sprite::NONE;
  		add_component(entity, CycleAnimComp{.sprites = {Sprite::QUEUE_STAR_2, Sprite::QUEUE_STAR_3, Sprite::NONE}, .frequency = 10.f,
											.finish_behaviour = FinishBehaviour::DESTROY_COMPONENT});
	} else {
		sprite.sprites[0] = Sprite::NONE;
		sprite.sprites[1] = Sprite::NONE;
	}
}

static constexpr float DOT_DISTANCE = 60.f;
static constexpr float ARROWS_PER_UNIT = 1.f / DOT_DISTANCE;
constexpr u8 NUMBER_OF_ARROW_DOTS = (SCREEN_SPACE_HEIGHT + SCREEN_SPACE_WIDTH) / DOT_DISTANCE;
constexpr u16 ARROW_DOT_WIDTH = get_sprite_dimensions(Sprite::ARROW_DOT_1).w;
constexpr u16 ARROW_DOT_HEIGHT = get_sprite_dimensions(Sprite::ARROW_DOT_1).h;

void create_arrow() {
	for (u8 i = 0; i < NUMBER_OF_ARROW_DOTS; i++) {
		entt::entity entity = ecs.create();
		add_component(entity, UITransformComp{.sort_order = 10});
		add_component(entity, SpriteComp{.sprites = {Sprite::NONE}});
		add_component(entity, ArrowComp{.index = i});
		auto sprites = i == 0 ? decltype(CycleAnimComp::sprites){Sprite::ARROW_ARROW_1, Sprite::ARROW_ARROW_2, Sprite::ARROW_ARROW_3, Sprite::ARROW_ARROW_4} :
								decltype(CycleAnimComp::sprites){Sprite::ARROW_DOT_1, Sprite::ARROW_DOT_2, Sprite::ARROW_DOT_3, Sprite::ARROW_DOT_4};
		add_component(entity, CycleAnimComp{.sprites = sprites, .frequency = 4.f, .index = static_cast<u8>(i % sprites.current_size)});
	}
}

void destroy_arrow() {
	auto view = ecs.view<ArrowComp>();
	ecs.destroy(view.begin(), view.end());
}

float curved_interpolate(float a, float b, float t) {
	return a + (b - a) * sqrt(t);
}

void update_arrow() {
	if (!get_combat().ui.target_position.has_value()) {
		return;
	}

	Vector2 target_position = get_combat().ui.target_position.value();

	for (auto [entity, transform, arrow] : ecs.view<UITransformComp, ArrowComp>().each()) {
		static constexpr float x_start = SCREEN_SPACE_WIDTH / 2.f;
		float x_end = target_position.x - ARROW_DOT_WIDTH / 2.f;
		float y_distance = abs(SCREEN_SPACE_HEIGHT - target_position.y);
		float x_distance = abs(x_start - x_end);
		float distance = std::hypot(x_distance, y_distance);
		float number_of_dots_on_screen = distance / DOT_DISTANCE;
		float interpolation = 1.f - static_cast<float>(arrow.index) / number_of_dots_on_screen;
		//float x_offset = arrow.index == 0 ? 0.f : (x_end - x_start) * 0.05f * (y_distance / SCREEN_SPACE_HEIGHT);
		transform.relative_position = {curved_interpolate(x_start, x_end, interpolation),
									   target_position.y + arrow.index * y_distance / number_of_dots_on_screen};
	}
}

constexpr u8 HEALTHBAR_WIDTH = get_sprite_dimensions(Sprite::HEALTHBAR_OUTLINE_1).w;
constexpr u8 HEALTHBAR_HEIGHT = get_sprite_dimensions(Sprite::HEALTHBAR_OUTLINE_1).h;

void create_healthbars() {
	for (entt::entity character_entity : get_combat().characters) {
		auto& character_transform = ecs.get<TransformComp>(character_entity);
		auto& character_component = ecs.get<CharacterComp>(character_entity);

		auto entity = ecs.create();
		auto& transform = add_component(entity, TransformComp{.position = {10.f, -50.f}});
		character_transform.add_child(character_entity, entity);
		auto& sprite = add_component(entity, SpriteComp{.sprites = { Sprite::HEALTHBAR_OUTLINE_1, Sprite::HEALTHBAR_DYNAMIC_1, Sprite::HEALTHBAR_GOOD_1}});
		switch (character_component.data->type) {
		case CharacterType::GOOD: sprite.sprites.at(2) = Sprite::HEALTHBAR_GOOD_1; break;
		case CharacterType::EVIL: sprite.sprites.at(2) = Sprite::HEALTHBAR_EVIL_1; break;
		case CharacterType::FURNITURE: sprite.sprites.at(2) = Sprite::HEALTHBAR_NEUTRAL_1; break;
		}
		sprite.masks[1] = {{0.f, 0.f}, {HEALTHBAR_WIDTH, HEALTHBAR_HEIGHT}};
		add_component(entity, HealthbarComp{});

		float health_units = character_component.max_health / 100.f;
		int number_of_health_units = std::ceil(health_units);
		float health_unit_width = HEALTHBAR_WIDTH / health_units;
		float divider_offset = (health_units - number_of_health_units) * health_unit_width;
		for (int i = 1; i < number_of_health_units; i++) {
			float x_position = (float)i * health_unit_width;
			if (HEALTHBAR_WIDTH - x_position < 10.f) break;
			entt::entity divider = ecs.create();
			add_component(divider, TransformComp{.position = {x_position, 0.f}});
			transform.add_child(entity, divider);
			add_component(divider, SpriteComp{.sprites = {Sprite::HEALTHBAR_DIVIDER_1}});
		}
	}
}

void destroy_healthbars() {
	auto view = ecs.view<HealthbarComp>();
	ecs.destroy(view.begin(), view.end());
}

void UI::refresh_health_bar(const CharacterComp& character, bool is_heal) {
	float health = character.health / character.max_health;

	for (auto [entity, transform, sprite] : ecs.view<HealthbarComp, TransformComp, SpriteComp>().each()) {
		if (transform.parent == character.entity) {
			sprite.masks[2] = {{0.f, 0.f}, {HEALTHBAR_WIDTH * health, HEALTHBAR_HEIGHT}};
			sprite.sprites[1] = is_heal ? Sprite::HEALTHBAR_HEAL_1 : Sprite::HEALTHBAR_DYNAMIC_1;

			play_animation(0.2, 0.0, &SpriteComp::masks, entity, [](Animation& animation, auto starting_value) {
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
	for (u8 i = 0; i < BARS_PER_TURN; i++) {
		auto entity = ecs.create();

		Sprite sprite = i == 0 ? Sprite::GAMEBAR_END_1 : Sprite::GAMEBAR_START_1;
		auto& sprite_component = add_component(entity, SpriteComp{.sprites = {sprite, sprite}});
		sprite_component.tints[0] = {0, 0, 0, 255};

		float width = sprite_component.visible_bounding_box().width();
		add_component(entity, UITransformComp{.x_anchor = XAnchor::CENTER, .y_anchor = YAnchor::TOP, .relative_position = { (1 - i) * width, 0.f}});

		add_component(entity, GamebarComp{.index = i});
	}
}

void destroy_gamebar() {
	auto view = ecs.view<GamebarComp>();
	ecs.destroy(view.begin(), view.end());
}

void cycle_gamebar_animation(u8 index) {
	auto [entity, gamebar] = find_component<GamebarComp>([index](auto& gamebar){ return gamebar.index == index; });
	auto& sprite = ecs.get<SpriteComp>(entity);

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

constexpr double VIBRATIONS_PER_BAR = 4;
constexpr double SECONDS_PER_VIBRATION = SECONDS_PER_BAR / VIBRATIONS_PER_BAR;

void update_cards_can_play() {
	auto view = ecs.view<HandCardComp, SpriteComp>();
	for (auto [entity, card, sprite] : view.each()) {
		if (card.owning_character != get_combat().get_active_character_entity()) {
			continue;
		}
		sprite.tint = card.get_card().can_play() ? Colour{255, 255, 255, 255} : Colour{140, 140, 140, 255};
	}
}

void update_gamebar() {
	auto view = ecs.view<GamebarComp, SpriteComp, UITransformComp>();
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

			if (get_combat().get_discrete_bar_progress() != 0 && gamebar.index == get_combat().bar_index) {
				entt::entity fx_entity = ecs.create();
				auto& fx_sprite = add_component(fx_entity, SpriteComp{.sprites = {Sprite::GAMEBAR_TICK_EFFECT_1}});
				fx_sprite.tints[0] = {255, 255, 255, 200};
				auto& fx_transform = add_component(fx_entity, UITransformComp{transform});
				fx_transform.relative_position.x += sprite.visible_bounding_box().width() * (1.f - get_combat().get_discrete_bar_progress());
				fx_transform.sort_order = 1;
				add_component(fx_entity, CycleAnimComp{
						.sprites = {Sprite::GAMEBAR_TICK_EFFECT_1, Sprite::GAMEBAR_TICK_EFFECT_2, Sprite::GAMEBAR_TICK_EFFECT_3, Sprite::GAMEBAR_TICK_EFFECT_4},
						.frequency = 12.f, .finish_behaviour = FinishBehaviour::DESTROY_ENTITY});
			}
		}
	}
}

void UI::on_bar_end() {
	for (int i = 0; i < BARS_PER_TURN; i++) {
		cycle_gamebar_animation(i);
	}

	u8 bar_index = get_combat().bar_index;
	auto [gamebar_entity, gamebar] = find_component<GamebarComp>([bar_index](auto& gamebar){ return gamebar.index == bar_index; });
	auto& gamebar_transform = ecs.get<UITransformComp>(gamebar_entity);

	entt::entity entity = ecs.create();
	auto& sprite = add_component(entity, SpriteComp{.sprites = {Sprite::GAMEBAR_BAR_EFFECT_1}});
	sprite.tints[0] = {255, 255, 255, 200};
	add_component(entity, UITransformComp{gamebar_transform});
	add_component(entity, CycleAnimComp{
			.sprites = {Sprite::GAMEBAR_BAR_EFFECT_1, Sprite::GAMEBAR_BAR_EFFECT_2, Sprite::GAMEBAR_BAR_EFFECT_3, Sprite::GAMEBAR_BAR_EFFECT_4},
			.frequency = 12.f, .finish_behaviour = FinishBehaviour::DESTROY_ENTITY});

	// TEST
	CharacterComp& character = ecs.get<CharacterComp>(get_combat().characters.at(0));
	character.damage(40.f);
	refresh_health_bar(character, false);
	// TEST
}

void UI::start_combat() {
	create_gamebar();
	create_healthbars();

	auto view = ecs.view<CharacterComp, PlayerMovementComp>();
	for (auto [entity, character, movement] : view.each()) {
		movement.direction = CharacterDirection::DOWN;
	}

	push_input_mode(InputMode::COMBAT);
	create_queue_preview();
}

constexpr u16 CARD_HOVER_WIDTH = 145;
constexpr u16 CARD_HOVER_HEIGHT = 200;
constexpr u16 CARD_HOVER_EXPANDED_HEIGHT = 250;

constexpr u16 CARD_SPRITE_WIDTH = 150;
constexpr u16 CARD_SPRITE_HEIGHT = 200;

constexpr u16 CARD_SPRITE_SHOWN_OFFSET = 3;
constexpr u16 CARD_SPRITE_HIDDEN_OFFSET = 100;
constexpr u16 CARD_SPRITE_OFF_SCREEN_OFFSET = 400;

float card_x_offset(u8 hand_size, u8 index) {
	float starting_position = -(CARD_HOVER_WIDTH * hand_size) / 2.f + (CARD_HOVER_WIDTH / 2.f);
	return starting_position + CARD_HOVER_WIDTH * index;
}

void attach_card_visual(Card card, entt::entity parent) {
	auto& transform = ecs.get<UITransformComp>(parent);

	entt::entity art = ecs.create();
	add_component(art, UITransformComp{.x_anchor = XAnchor::CENTER, .relative_position = {0.f, 20.f}, .width = 136, .height = 96,});
	transform.add_child(parent, art);
	add_component(art, SpriteComp{.sprites = {Sprite::TEST_BACKGROUND}});

	entt::entity frame = ecs.create();
	add_component(frame, UITransformComp{.width = CARD_SPRITE_WIDTH, .height = CARD_SPRITE_HEIGHT});
	transform.add_child(parent, frame);
	auto& sprite = add_component(frame, SpriteComp{});
	switch (card.data->card_type) {
	case CardType::PSYCHIC: sprite.sprites = {Sprite::CARD_PSYCHIC_1, Sprite::CARD_PSYCHIC_LVL_1}; break;
	case CardType::MAGIC: sprite.sprites = {Sprite::CARD_MAGIC_1, Sprite::CARD_MAGIC_LVL_1}; break;
	case CardType::GROOVE: sprite.sprites = {Sprite::CARD_GROOVE_1, Sprite::CARD_GROOVE_LVL_1}; break;
	}

	entt::entity name = ecs.create();
	add_component(name, UITransformComp{.relative_position = {0.f, 10.f}, .width = CARD_SPRITE_WIDTH, .height = 200});
	transform.add_child(parent, name);
	add_component(name, TextComp{.text = card.data->name, .colour = WHITE, .size = 24, .x_align = XAnchor::CENTER});

	entt::entity description = ecs.create();
	constexpr float DESCRIPTION_MARGIN = 10.f;
	add_component(description, UITransformComp{.relative_position = {DESCRIPTION_MARGIN, 115.f},
											   .width = CARD_SPRITE_WIDTH - 2 * (u16)DESCRIPTION_MARGIN, .height = 50});
	transform.add_child(parent, description);
	add_component(description, TextComp{.text = card.data->description, .colour = WHITE, .size = 16});

	entt::entity cost = ecs.create();
	add_component(cost, UITransformComp{.relative_position = {123.5, 7.f}, .width = CARD_SPRITE_WIDTH, .height = 200});
	transform.add_child(parent, cost);
	add_component(cost, TextComp{.text {number_to_string(card.data->cost)}, .colour = BLACK, .size = 32});
}

void create_card_preview(Card card) {
	entt::entity entity = ecs.create();
	auto& transform = add_component(entity, UITransformComp{.x_anchor = XAnchor::LEFT, .y_anchor = YAnchor::BOTTOM,
															.width = CARD_SPRITE_WIDTH, .height = CARD_SPRITE_HEIGHT, .scale = 2.f });
	add_component(entity, SpriteComp{}); // To access visibility for children
	add_component(entity, CardPreviewComp{});
	attach_card_visual(card, entity);
}

void destroy_card_preview() {
	auto view = ecs.view<CardPreviewComp>();
	ecs.destroy(view.begin(), view.end());
}

void on_card_hover(entt::entity hovered_entity) {
	auto& hovered_button = ecs.get<HandButtonComp>(hovered_entity);
	u8 index = hovered_button.index;
	u8 hand_size = get_combat().get_active_character()->hand.size();

	auto view = ecs.view<HandCardComp, UITransformComp>();
	auto [card_entity, hovered_card] = find_component<HandCardComp>([index](auto& card){ return card.index == index; });
	get_combat().ui.hovered_card = card_entity;
	ecs.get<UITransformComp>(hovered_entity).height = CARD_HOVER_EXPANDED_HEIGHT;

	stop_animation(hovered_card.animation_id);
	hovered_card.animation_id = start_animation_group();
	play_animation(0.04, 0.0, &UITransformComp::scale, card_entity, [](Animation& animation, float starting_value) {
		return smooth_curve(2.f, animation, starting_value);
	});
	play_animation(0.04, 0.0, &UITransformComp::relative_position, card_entity, [index, hand_size](Animation& animation, Vector2 starting_value) {
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
			play_animation(0.06, 0.0, &UITransformComp::scale, entity, [distance_weight](Animation& animation, float starting_value) {
				return smooth_curve(1.f - distance_weight * 1.f, animation, starting_value);
			});
			float x_offset = current_index > index ? 70.f : -70.f;
			float x_offset_coeff = current_index > index ? -60.f : 60.f;
			x_offset += x_offset_coeff * (distance_from_hovered / 2.5f);
			play_animation(0.10, 0.0, &UITransformComp::relative_position, entity, [current_index, x_offset, distance_from_hovered, hand_size](Animation& animation, Vector2 starting_value) {
				float x_target = card_x_offset(hand_size, current_index) + x_offset;
				float y_target = CARD_SPRITE_SHOWN_OFFSET + 2.0f - 12.f * distance_from_hovered;
				return Vector2{smooth_curve(x_target, animation, starting_value.x), smooth_curve(y_target, animation, starting_value.y)};
			});
			end_animation_group();
		}
	}
}

void on_card_unhover(entt::entity entity) {
	get_combat().ui.hovered_card = entt::null;
	float y_target = CARD_SPRITE_HIDDEN_OFFSET;

	u8 index = ecs.get<HandButtonComp>(entity).index;
	auto [card_entity, unhovered_card] = find_component<HandCardComp>([index](auto& card){ return card.index == index; });
	if (card_entity == get_combat().ui.dragged_card) {
		create_arrow();
		create_card_preview(unhovered_card.get_card());
		y_target = CARD_SPRITE_OFF_SCREEN_OFFSET;
	}

	u8 hand_size = get_combat().get_active_character()->hand.size();

	for (auto [entity, card, transform] : ecs.view<HandCardComp, UITransformComp>().each()) {
		u8 index = card.index;
		stop_animation(card.animation_id);

		card.animation_id = start_animation_group();
		play_animation(0.04, 0.0, &UITransformComp::scale, entity, [](Animation& animation, float starting_value) {
			return smooth_curve(1.f, animation, starting_value);
		});
		play_animation(0.04, 0.0, &UITransformComp::relative_position, entity, [hand_size, index, y_target](Animation& animation, Vector2 starting_value) {
			float x_target = card_x_offset(hand_size, index);
			return Vector2{smooth_curve(x_target, animation, starting_value.x), smooth_curve<float>(y_target, animation, starting_value.y)};
		});
		end_animation_group();
	}

	ecs.get<UITransformComp>(entity).height = CARD_HOVER_HEIGHT;
}

void on_card_click(entt::entity entity) {
	u8 index = ecs.get<HandButtonComp>(entity).index;

	auto [card_entity, card] = find_component<HandCardComp>([index](auto& card){ return card.index == index; });

	if (!card.get_card().can_play()) {
		return;
	}

	get_combat().ui.dragged_card = card_entity;

	auto view = ecs.view<HandButtonComp, ButtonComp>();
	for (auto [entity, hand_button, button] : view.each()) {
		if (hand_button.index != index) {
			button.is_enabled = false;
		}
	}

	// TODO play a vfx arrow pointing up animation
}

// 1. HandCardComp and unpositioned visuals are created/destroyed whenever hand state changes: ui_add_hand_visual() ui_destroy_hand_visual()
// 2. Buttons are destroyed and recreated whenever active hand state changes: refresh_hand_buttons()
// 3. visuals are repositioned when turn changes or hand state changes: position_hand_visuals()

void refresh_hand_buttons() {
	auto view = ecs.view<HandButtonComp>();
	ecs.destroy(view.begin(), view.end());

	CharacterComp* character = get_combat().get_active_character();
	for (u8 i = 0; i < character->hand.size(); i++) {
		entt::entity entity = ecs.create();
		add_component(entity, UITransformComp{.x_anchor = XAnchor::CENTER, .y_anchor = YAnchor::BOTTOM, .relative_position = {card_x_offset(character->hand.size(), i), 0.f},
											  .width = CARD_HOVER_WIDTH, .height = CARD_HOVER_HEIGHT});
		add_component(entity, ButtonComp{.on_hover = on_card_hover, .on_click = on_card_click, .on_unhover = on_card_unhover});
		add_component(entity, HandButtonComp{.index = i});
	}
}

void UI::play_queued_draw_animations() {
	entt::entity character_entity = get_combat().get_active_character_entity();
	CharacterComp* character = get_combat().get_active_character();
	auto view = ecs.view<HandCardComp>();
	u8 counter = view.size() - 1;

	for (auto [card_entity, card] : view.each()) {
		if (card.owning_character != character_entity) {
			continue;
		}

		if (!card.queue_draw_animation) {
			continue;
		}

		CardType type = character->hand.at(card.index).data->card_type;

		constexpr double DURATION = 0.2;
		double delay = 0.02 * counter;

		entt::entity fx_entity = ecs.create();
		add_component(fx_entity, UITransformComp{.x_anchor = XAnchor::CENTER, .y_anchor = YAnchor::BOTTOM,
												 .relative_position = {card_x_offset(character->hand.size(), card.index) + 400.f, -100.f},
												 .width = CARD_SPRITE_WIDTH, .height = CARD_SPRITE_HEIGHT, .sort_order = 1});
		auto& sprite = add_component(fx_entity, SpriteComp{.sprites = {Sprite::NONE}});

		play_animation(DURATION, delay, &UITransformComp::relative_position, fx_entity, [card_entity](Animation& animation, Vector2 starting_value) {
			Vector2 target = ecs.get<UITransformComp>(card_entity).relative_position;
			return Vector2{smooth_curve(target.x, animation, starting_value.x), smooth_curve(target.y, animation, starting_value.y)};
		});

		play_animation(DURATION, delay, &UITransformComp::scale, fx_entity, [card_entity](Animation& animation, float starting_value) {
			float target = ecs.get<UITransformComp>(card_entity).scale;
			return smooth_curve(target, animation, starting_value);
		});

		auto& animation = add_component(fx_entity, CycleAnimComp{});
		switch (type) {
		case CardType::PSYCHIC: animation.sprites = {Sprite::CARD_PSYCHIC_2, Sprite::CARD_PSYCHIC_3, Sprite::CARD_PSYCHIC_4, Sprite::CARD_PSYCHIC_5, Sprite::CARD_PSYCHIC_6,Sprite::CARD_PSYCHIC_7, Sprite::CARD_PSYCHIC_8, Sprite::CARD_PSYCHIC_9, Sprite::CARD_PSYCHIC_10 }; break;
		case CardType::MAGIC: animation.sprites = {Sprite::CARD_MAGIC_2, Sprite::CARD_MAGIC_3, Sprite::CARD_MAGIC_4, Sprite::CARD_MAGIC_5, Sprite::CARD_MAGIC_6,Sprite::CARD_MAGIC_7, Sprite::CARD_MAGIC_8, Sprite::CARD_MAGIC_9, Sprite::CARD_MAGIC_10 }; break;
		case CardType::GROOVE: animation.sprites = {Sprite::CARD_GROOVE_2, Sprite::CARD_GROOVE_3, Sprite::CARD_GROOVE_4, Sprite::CARD_GROOVE_5, Sprite::CARD_GROOVE_6,Sprite::CARD_GROOVE_7, Sprite::CARD_GROOVE_8, Sprite::CARD_GROOVE_9, Sprite::CARD_GROOVE_10 }; break;
		}
		animation.finish_behaviour = FinishBehaviour::DESTROY_ENTITY;
		animation.frequency = 1.0 / ((DURATION) / (double)animation.sprites.size());
		animation.delay = delay;

		play_animation(DURATION + delay - 0.01, 0.f, &SpriteComp::visible, card_entity, [](Animation& animation, bool starting_value) {
			return animation.is_finished();
		});

		counter--;
		card.queue_draw_animation = false;
	}
}

void position_hand_visuals(bool from_current_position) {
	entt::entity character = get_combat().get_active_character_entity();
	CharacterComp* character_component = get_combat().get_active_character();
	for (auto [entity, card, transform, sprite] : ecs.view<HandCardComp, UITransformComp, SpriteComp>().each()) {
		if (card.owning_character != character) {
			sprite.visible = false;
			continue;
		}

		sprite.visible = true;
		float x_position = card_x_offset(character_component->hand.size(), card.index);
		Vector2 start_position = from_current_position ? transform.relative_position : Vector2{x_position, CARD_SPRITE_HIDDEN_OFFSET + 100.f};
		Vector2 new_position = {x_position, CARD_SPRITE_HIDDEN_OFFSET};

		stop_animation(card.animation_id);
		card.animation_id = start_animation_group();
		play_animation(0.04, 0.0, &UITransformComp::relative_position, entity, [new_position, start_position](Animation& animation, Vector2 starting_value) {
			return fast_start_curve(new_position, animation, start_position);
		});
		if (from_current_position) {
			play_animation(0.04, 0.0, &UITransformComp::scale, entity, [](Animation& animation, float starting_value) {
				return smooth_curve(1.f, animation, starting_value);
			});
		} else {
			transform.scale = 1.f;
		}
		end_animation_group();
	}
}

void UI::add_hand_visual(const CharacterComp& character, u8 index) {
	Card card = character.hand.at(index);

	entt::entity entity = ecs.create();
	auto& transform = add_component(entity, UITransformComp{.x_anchor = XAnchor::CENTER, .y_anchor = YAnchor::BOTTOM,
															.width = CARD_SPRITE_WIDTH, .height = CARD_SPRITE_HEIGHT });
	add_component(entity, SpriteComp{}); // To access visibility for children
	add_component(entity, HandCardComp{.index = index, .owning_character = character.entity, .queue_draw_animation = true});
	attach_card_visual(card, entity);

	if (character.entity == get_combat().get_active_character_entity()) {
		refresh_hand_buttons();
		position_hand_visuals(true);
	}
}

void UI::destroy_hand_visual(const CharacterComp& character, u8 index) {
	auto view = ecs.view<HandCardComp>();

	for (auto [entity, card] : view.each()) {
		auto& owning_character = ecs.get<CharacterComp>(card.owning_character);
		if (card.owning_character == character.entity && card.index == index) {
			ecs.destroy(entity);
			break;
		}
	}

	for (auto [entity, card] : view.each()) {
		if (card.owning_character == character.entity && card.index > index) {
			card.index--;
		}
	}

	if (character.entity == get_combat().get_active_character_entity()) {
		refresh_hand_buttons();
		position_hand_visuals(true);
	}
}

void stop_drag() {
	get_combat().ui.dragged_card = entt::null;
	get_combat().ui.dragged_above_hand = false;
	destroy_arrow();
	destroy_card_preview();
	position_hand_visuals(false);

	auto view = ecs.view<HandButtonComp, ButtonComp>();
	for (auto [entity, hand_button, button] : view.each()) {
		button.is_enabled = true;
	}
}

void update_drag() {
	if (get_combat().ui.dragged_card == entt::null) {
		return;
	}

	if (get_mouse_y() < SCREEN_SPACE_HEIGHT - CARD_HOVER_EXPANDED_HEIGHT) {
		get_combat().ui.dragged_above_hand = true;
	} else if (get_mouse_y() > SCREEN_SPACE_HEIGHT - CARD_HOVER_HEIGHT && get_combat().ui.dragged_above_hand) {
		stop_drag();
		return;
	}

	HandCardComp dragged_card = ecs.get<HandCardComp>(get_combat().ui.dragged_card);

	if (!dragged_card.get_card().can_play()) {
		stop_drag();
		return;
	}

	entt::entity closest_character = entt::null;
	get_combat().ui.target_position.reset();
	float closest_distance{};
	auto view = ecs.view<CharacterComp, BoxColliderComp, TransformComp>();
	for (auto [entity, character, collider, transform] : view.each()) { // TODO highlight valid targets
		auto valid_target_bitmask = dragged_card.get_card().data->valid_target_bitmask;
		if (valid_target_bitmask == 0) {
			if (entity != get_combat().get_active_character_entity()) {
				continue;
			}
		} else if ((character.data->type & valid_target_bitmask) != character.data->type) {
			continue;
		}
		Vector2 character_screen_position = world_to_pixel(transform.position + collider.box.center());
		float distance_to_mouse = Vector2::distance(character_screen_position, {get_pixel_mouse_x(), get_pixel_mouse_y()});
		if (!get_combat().ui.target_position.has_value() || distance_to_mouse < closest_distance) {
			closest_character = entity;
			get_combat().ui.target_position.emplace(character_screen_position / window_scale());
			closest_distance = distance_to_mouse;
		}
	}

	if (!input_held(InputType::MOUSE_CLICK)) {
		Card card = dragged_card.get_card();

		get_combat().get_active_character()->play_card(dragged_card.index, closest_character);
		get_combat().ui.dragged_card = entt::null;

		refresh_hand_buttons();

		destroy_arrow();
		destroy_card_preview();
	}
}

void UI::update_combat() {
	update_gamebar();
	update_drag();
	update_arrow();
	update_queue_preview();
	update_cards_can_play();
}

void UI::end_combat() {
	destroy_gamebar();
	destroy_healthbars();
	pop_input_mode(InputMode::COMBAT);
	destroy_queue_preview();
}

void UI::on_turn_start() {
	get_combat().ui.hovered_card = entt::null;
	stop_drag();
	refresh_hand_buttons();
	UI::play_queued_draw_animations();
	position_hand_visuals(false);
}

Card HandCardComp::get_card() {
	// Assumes the active character
	return get_combat().get_active_character()->hand.at(index);
}
