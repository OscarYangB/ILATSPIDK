#include "combat_ui.h"
#include "game.h"
#include "render.h"
#include "combat.h"
#include "button.h"

#include <iostream>

static std::vector<entt::entity> gamebars{};
static std::vector<entt::entity> hand_buttons{};

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
	for (const entt::entity& entity : gamebars) {
		ecs.destroy(entity);
	}

	gamebars.clear();
}

void cycle_gamebar_animation(u8 index) {
	auto& sprite = ecs.get<SpriteComponent>(gamebars[index]);
	Sprite next_sprite;
	switch (sprite.sprites[0]) {
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

	sprite.sprites[0] = next_sprite;
	sprite.sprites[1] = next_sprite;
}

double vibrate_timer = 0.0;
constexpr double VIBRATIONS_PER_BAR = 8;
constexpr double SECONDS_PER_VIBRATION = SECONDS_PER_BAR / VIBRATIONS_PER_BAR;

void update_gamebar() {
	for (int i = 0; i < gamebars.size(); i++) {
		auto& sprite = ecs.get<SpriteComponent>(gamebars[i]);
		Box box = sprite.visible_bounding_box();

		if (combat->bar_index > i) {  // Bar is already used
			sprite.tints[1] = {255, 255, 255, 0};
			sprite.tints[0] = {0, 0, 0, 50};
		} else if (combat->bar_index == i) {  // Current bar
			sprite.tints[1] = {255, 255, 255, 255};
			sprite.tints[0] = {0, 0, 0, 255};
			box.right_bottom.x -= box.width() * combat->get_discrete_bar_progress();
		} else { // Bar hasn't been used
			sprite.tints[1] = {255, 255, 255, 255};
			sprite.tints[0] = {0, 0, 0, 255};
		}

		sprite.masks[1] = box;
	}

	vibrate_timer += delta_time;
	if (vibrate_timer > SECONDS_PER_VIBRATION) {
		vibrate_timer -= SECONDS_PER_VIBRATION;
		cycle_gamebar_animation(combat->bar_index);
	}
}

void ui_on_bar_end() {
	for (int i = 0; i < gamebars.size(); i++) {
		cycle_gamebar_animation(i);
	}
}

void ui_start_combat() {
	create_gamebar();
}

void ui_update_combat() {
	update_gamebar();
}

void ui_end_combat() {
	end_combat();
}

constexpr double CARD_HOVER_WIDTH = 300.f;
constexpr double CARD_HOVER_HEIGHT = 300.f;

void on_card_hover(entt::entity entity) {
	auto& card = ecs.get<HandCardComponent>(entity);
	std::cout << "Card is hovered: " << card.card->name << "\n";
}

void on_card_click(entt::entity button) {

}

void refresh_hand_buttons(const CharacterComponent& character) {
	for (const entt::entity& entity : hand_buttons) {
		ecs.destroy(entity);
	}
	hand_buttons.clear();

	float x_position = -(CARD_HOVER_WIDTH * character.hand.size()) / 2.f + (CARD_HOVER_WIDTH / 2.f);
	for (int i = 0; i < character.hand.size(); i++) {
		entt::entity entity = ecs.create();

		auto& transform = ecs.emplace<AnchoredTransformComponent>(entity);
		transform.x_anchor = HorizontalAnchor::CENTER;
		transform.y_anchor = VerticalAnchor::BOTTOM;
		transform.relative_position = {x_position, 0.f};
		transform.width = CARD_HOVER_WIDTH;
		transform.height = CARD_HOVER_HEIGHT;

		auto& sprite = ecs.emplace<SpriteComponent>(entity);
		sprite.sprites = {Sprite::TEST_BUTTON};

		auto& button = ecs.emplace<ButtonComponent>(entity);
		button.on_hover = on_card_hover;
		button.on_click = on_card_click;

		auto& card = ecs.emplace<HandCardComponent>(entity);
		card.card = character.hand[i];

		hand_buttons.push_back(entity);
		x_position += CARD_HOVER_WIDTH;
	}
}

void ui_on_turn_start() {
	refresh_hand_buttons(*combat->get_active_character());
}
