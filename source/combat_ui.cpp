#include "combat_ui.h"
#include "game.h"
#include "render.h"
#include "combat.h"

static std::vector<entt::entity> gamebars{};

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
