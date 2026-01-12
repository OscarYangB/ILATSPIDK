#include "kerry_anim_controller.h"
#include "game.h"
#include "game_render.h"
#include "player_movement_controller.h"
#include "platform_render.h" // Should not use platform_render
#include <array>

// This entire file is disgusting dont look at it its just a test avert your eyes please
enum KerryAnimation {
	KERRY_FRONT_CAPE_1,
	KERRY_FRONT_CAPE_2,
	KERRY_FRONT_CAPE_3,
	KERRY_FRONT_CAPE_4,
	KERRY_FRONT_CAPE_5,
	KERRY_FRONT_CAPE_6,
	KERRY_FRONT_RIGHT_LEG_1,
	KERRY_FRONT_RIGHT_LEG_2,
	KERRY_FRONT_RIGHT_LEG_3,
	KERRY_FRONT_RIGHT_LEG_4,
	KERRY_FRONT_LEFT_LEG_1,
	KERRY_FRONT_LEFT_LEG_2,
	KERRY_FRONT_LEFT_LEG_3,
	KERRY_FRONT_LEFT_LEG_4,
	KERRY_FRONT_FLOWER_1,
	KERRY_FRONT_FLOWER_2,
	KERRY_FRONT_FLOWER_3,
	KERRY_FRONT_FLOWER_4,
	KERRY_FRONT_FLOWER_5,
	KERRY_FRONT_FLOWER_6,
	KERRY_FRONT_BODY_1,
	KERRY_FRONT_BODY_2,
	KERRY_FRONT_BODY_3,
	KERRY_FRONT_BODY_BLINK,

	KERRY_RIGHT_FLOWER_1,
	KERRY_RIGHT_FLOWER_2,
	KERRY_RIGHT_FLOWER_3,
	KERRY_RIGHT_FLOWER_4,
	KERRY_RIGHT_FLOWER_5,
	KERRY_RIGHT_FLOWER_6,
	KERRY_RIGHT_RIGHT_LEG_1,
	KERRY_RIGHT_RIGHT_LEG_2,
	KERRY_RIGHT_RIGHT_LEG_3,
	KERRY_RIGHT_RIGHT_LEG_4,
	KERRY_RIGHT_LEFT_LEG_1,
	KERRY_RIGHT_LEFT_LEG_2,
	KERRY_RIGHT_LEFT_LEG_3,
	KERRY_RIGHT_LEFT_LEG_4,
	KERRY_RIGHT_BODY_1,
	KERRY_RIGHT_BODY_2,
	KERRY_RIGHT_BODY_3,
	KERRY_RIGHT_BODY_BLINK,
	KERRY_RIGHT_CAPE_1,
	KERRY_RIGHT_CAPE_2,
	KERRY_RIGHT_CAPE_3,
	KERRY_RIGHT_CAPE_4,
	KERRY_RIGHT_CAPE_5,
	KERRY_RIGHT_CAPE_6,

	KERRY_LEFT_FLOWER_1,
	KERRY_LEFT_FLOWER_2,
	KERRY_LEFT_FLOWER_3,
	KERRY_LEFT_FLOWER_4,
	KERRY_LEFT_FLOWER_5,
	KERRY_LEFT_FLOWER_6,
	KERRY_LEFT_RIGHT_LEG_1,
	KERRY_LEFT_RIGHT_LEG_2,
	KERRY_LEFT_RIGHT_LEG_3,
	KERRY_LEFT_RIGHT_LEG_4,
	KERRY_LEFT_LEFT_LEG_1,
	KERRY_LEFT_LEFT_LEG_2,
	KERRY_LEFT_LEFT_LEG_3,
	KERRY_LEFT_LEFT_LEG_4,
	KERRY_LEFT_BODY_1,
	KERRY_LEFT_BODY_2,
	KERRY_LEFT_BODY_3,
	KERRY_LEFT_BODY_BLINK,
	KERRY_LEFT_CAPE_1,
	KERRY_LEFT_CAPE_2,
	KERRY_LEFT_CAPE_3,
	KERRY_LEFT_CAPE_4,
	KERRY_LEFT_CAPE_5,
	KERRY_LEFT_CAPE_6,

	KERRY_BACK_FLOWER_1,
	KERRY_BACK_FLOWER_2,
	KERRY_BACK_FLOWER_3,
	KERRY_BACK_FLOWER_4,
	KERRY_BACK_FLOWER_5,
	KERRY_BACK_FLOWER_6,
	KERRY_BACK_BODY_1,
	KERRY_BACK_BODY_2,
	KERRY_BACK_BODY_3,
	KERRY_BACK_RIGHT_LEG_1,
	KERRY_BACK_RIGHT_LEG_2,
	KERRY_BACK_RIGHT_LEG_3,
	KERRY_BACK_RIGHT_LEG_4,
	KERRY_BACK_LEFT_LEG_1,
	KERRY_BACK_LEFT_LEG_2,
	KERRY_BACK_LEFT_LEG_3,
	KERRY_BACK_LEFT_LEG_4,
	KERRY_BACK_CAPE_1,
	KERRY_BACK_CAPE_2,
	KERRY_BACK_CAPE_3,
	KERRY_BACK_CAPE_4,
	KERRY_BACK_CAPE_5,
	KERRY_BACK_CAPE_6,
};

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

void update_animation() {
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
	auto view = ecs.view<Sprite, PlayerMovementComponent, Transform>();

	for (auto [entity, sprite, movement, transform] : view.each()) {
		std::array<u8, 5> sprite_order;
		u8 body_sprite;
		u8 left_leg_sprite;
		u8 right_leg_sprite;
		u8 flower_sprite;
		u8 cape_sprite;

		if (!movement.is_moving) leg_moving_index = 0;

		switch(movement.direction) {
			case CharacterDirection::UP: {
				body_sprite = KERRY_BACK_BODY_1 + body_index;
				left_leg_sprite = KERRY_BACK_LEFT_LEG_1 + get_moving_left_leg_sprite();
				right_leg_sprite = KERRY_BACK_RIGHT_LEG_1 + get_moving_right_leg_sprite();
				flower_sprite = KERRY_BACK_FLOWER_1 + wind_index;
				cape_sprite = KERRY_BACK_CAPE_1 + wind_index;
				sprite_order = {flower_sprite, body_sprite, left_leg_sprite, right_leg_sprite, cape_sprite};
			} break;
			case CharacterDirection::DOWN: {
				body_sprite = KERRY_FRONT_BODY_1 + body_index;
				left_leg_sprite = KERRY_FRONT_LEFT_LEG_1 + get_moving_left_leg_sprite();
				right_leg_sprite = KERRY_FRONT_RIGHT_LEG_1 + get_moving_right_leg_sprite();
				flower_sprite = KERRY_FRONT_FLOWER_1 + wind_index;
				cape_sprite = KERRY_FRONT_CAPE_1 + wind_index;
				sprite_order = {cape_sprite, left_leg_sprite, right_leg_sprite, flower_sprite, body_sprite};
			} break;
			case CharacterDirection::LEFT: {
				body_sprite = KERRY_LEFT_BODY_1 + body_index;
				left_leg_sprite = KERRY_LEFT_LEFT_LEG_1 + get_moving_left_leg_sprite();
				right_leg_sprite = KERRY_LEFT_RIGHT_LEG_1 + get_moving_right_leg_sprite();
				flower_sprite = KERRY_LEFT_FLOWER_1 + wind_index;
				cape_sprite = KERRY_LEFT_CAPE_1 + wind_index;
				sprite_order = {flower_sprite, left_leg_sprite, right_leg_sprite, body_sprite, cape_sprite};
			} break;
			case CharacterDirection::RIGHT: {
				body_sprite = KERRY_RIGHT_BODY_1 + body_index;
				left_leg_sprite = KERRY_RIGHT_LEFT_LEG_1 + get_moving_left_leg_sprite();
				right_leg_sprite = KERRY_RIGHT_RIGHT_LEG_1 + get_moving_right_leg_sprite();
				flower_sprite = KERRY_RIGHT_FLOWER_1 + wind_index;
				cape_sprite = KERRY_RIGHT_CAPE_1 + wind_index;
				sprite_order = {flower_sprite, left_leg_sprite, right_leg_sprite, body_sprite, cape_sprite};
			} break;
		}

		for (u8 index : sprite_order) {
			Vector2 position = world_to_pixel(transform.position);
			float width = sprite.render_width();
			float height = sprite.render_height();
			render_sprite(sprite.image_asset, position.x, position.y, width, height, index, sprite.width, sprite.height);
		}
	}
}
