#include "kerry_anim_controller.h"
#include "game.h"
#include "render.h"
#include "image_data.h"
#include "movement_controller.h"
#include <array>
#include "input.h"

static u8 body_index;
static u8 leg_idle_index;
static u8 leg_moving_index;
static u8 wind_index;

static double timer = 0.f;
static double leg_timer = 0.f;

u8 get_moving_left_leg_sprite() {
	if (leg_moving_index < 3) {
		return 0;
	} else {
		return leg_moving_index == 5 ? 2 : leg_moving_index - 1;
	}
}

u8 get_moving_right_leg_sprite() {
	if (leg_moving_index < 3) {
		return leg_moving_index == 2 ? 2 : leg_moving_index + 2;
	} else {
		return 0;
	}
}

void update_character_animation() {
	timer += delta_time;
	if (timer > 0.4) {
		timer -= 0.4;
		body_index++; body_index %= 3;
		leg_idle_index++; leg_idle_index %= 2;
		wind_index++; wind_index %= 6;
	}

	leg_timer += delta_time;
	if (leg_timer > 0.1) {
		leg_timer -= 0.1;
		leg_moving_index++; leg_moving_index %= 6;
	}
	auto view = ecs.view<SpriteComponent, const PlayerMovementComponent, const TransformComponent>();

	for (auto [entity, sprite, movement, transform] : view.each()) {
		std::array<u8, 5> sprite_order;
		u8 body_sprite;
		u8 left_leg_sprite;
		u8 right_leg_sprite;
		u8 flower_sprite;
		u8 cape_sprite;

		if (!movement.is_moving || get_current_input_mode() != InputMode::EXPLORE) leg_moving_index = 0;

		switch(movement.direction) {
			case CharacterDirection::UP: {
				body_sprite = static_cast<u32>(Sprite::KERRY_UP_BODY_1) + body_index;
				left_leg_sprite = static_cast<u32>(Sprite::KERRY_UP_LEFT_LEG_1) + get_moving_left_leg_sprite();
				right_leg_sprite = static_cast<u32>(Sprite::KERRY_UP_RIGHT_LEG_1) + get_moving_right_leg_sprite();
				flower_sprite = static_cast<u32>(Sprite::KERRY_UP_FLOWER_1) + wind_index;
				cape_sprite = static_cast<u32>(Sprite::KERRY_UP_CAPE_1) + wind_index;
				sprite_order = {flower_sprite, body_sprite, left_leg_sprite, right_leg_sprite, cape_sprite};
			} break;
			case CharacterDirection::DOWN: {
				body_sprite = static_cast<u32>(Sprite::KERRY_DOWN_BODY_1) + body_index;
				left_leg_sprite = static_cast<u32>(Sprite::KERRY_DOWN_LEFT_LEG_1) + get_moving_left_leg_sprite();
				right_leg_sprite = static_cast<u32>(Sprite::KERRY_DOWN_RIGHT_LEG_1) + get_moving_right_leg_sprite();
				flower_sprite = static_cast<u32>(Sprite::KERRY_DOWN_FLOWER_1) + wind_index;
				cape_sprite = static_cast<u32>(Sprite::KERRY_DOWN_CAPE_1) + wind_index;
				sprite_order = {cape_sprite, left_leg_sprite, right_leg_sprite, flower_sprite, body_sprite};
			} break;
			case CharacterDirection::LEFT: {
				body_sprite = static_cast<u32>(Sprite::KERRY_LEFT_BODY_1) + body_index;
				left_leg_sprite = static_cast<u32>(Sprite::KERRY_LEFT_LEFT_LEG_1) + get_moving_left_leg_sprite();
				right_leg_sprite = static_cast<u32>(Sprite::KERRY_LEFT_RIGHT_LEG_1) + get_moving_right_leg_sprite();
				flower_sprite = static_cast<u32>(Sprite::KERRY_LEFT_FLOWER_1) + wind_index;
				cape_sprite = static_cast<u32>(Sprite::KERRY_LEFT_CAPE_1) + wind_index;
				sprite_order = {flower_sprite, left_leg_sprite, right_leg_sprite, body_sprite, cape_sprite};
			} break;
			case CharacterDirection::RIGHT: {
				body_sprite = static_cast<u32>(Sprite::KERRY_RIGHT_BODY_1) + body_index;
				left_leg_sprite = static_cast<u32>(Sprite::KERRY_RIGHT_LEFT_LEG_1) + get_moving_left_leg_sprite();
				right_leg_sprite = static_cast<u32>(Sprite::KERRY_RIGHT_RIGHT_LEG_1) + get_moving_right_leg_sprite();
				flower_sprite = static_cast<u32>(Sprite::KERRY_RIGHT_FLOWER_1) + wind_index;
				cape_sprite = static_cast<u32>(Sprite::KERRY_RIGHT_CAPE_1) + wind_index;
				sprite_order = {flower_sprite, left_leg_sprite, right_leg_sprite, body_sprite, cape_sprite};
			} break;
		}

		for (int i = 0; i < sprite.sprites.size(); i++) {
			Sprite index = static_cast<Sprite>(sprite_order[i]);
			sprite.sprites[i] = index;
		}
	}
}
