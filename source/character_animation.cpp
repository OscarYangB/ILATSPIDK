#include "character_animation.h"
#include "render.h"
#include "game.h"
#include "movement_controller.h"
#include "input.h"

void update_cycle_animations() {
	auto view = ecs.view<CycleAnimComp, SpriteComp>();

	for (auto [entity, animation, sprite] : view.each()) {
		float delta = delta_time;
		if (animation.delay > 0.0) {
			animation.delay -= delta_time;

			if (animation.delay <= 0.0) {
				delta = -animation.delay;
			} else {
				continue;
			}
		}

		const double animation_delta = 1.0 / animation.frequency;
		animation.timer += delta;
		if (animation.timer > animation_delta) {
			animation.timer -= animation_delta;
			animation.index++;
			if (animation.index >= animation.sprites.size()) {
				switch (animation.finish_behaviour) {
					case FinishBehaviour::LOOP: animation.index %= animation.sprites.size(); break;
					case FinishBehaviour::DESTROY_COMPONENT: ecs.remove<CycleAnimComp>(entity); break;
					case FinishBehaviour::DESTROY_ENTITY: ecs.destroy(entity); break;
				}
				continue;
			}
		}

		sprite.sprites.at(0) = animation.sprites.at(animation.index);
	}
}


u8 CharacterAnimComp::get_moving_left_leg_sprite() {
	if (leg_moving_index < 3) {
		return 0;
	} else {
		return leg_moving_index == 5 ? 2 : leg_moving_index - 1;
	}
}

u8 CharacterAnimComp::get_moving_right_leg_sprite() {
	if (leg_moving_index < 3) {
		return leg_moving_index == 2 ? 2 : leg_moving_index + 2;
	} else {
		return 0;
	}
}

void update_character_animation() {
	auto view = ecs.view<SpriteComp, const PlayerMovementComp, const TransformComp, CharacterAnimComp>();

	for (auto [entity, sprite, movement, transform, animation] : view.each()) {
		animation.timer += delta_time;
		if (animation.timer > 0.4) {
			animation.timer -= 0.4;
			animation.body_index++; animation.body_index %= 3;
			animation.leg_idle_index++; animation.leg_idle_index %= 2;
			animation.wind_index++; animation.wind_index %= 6;
		}

		animation.leg_timer += delta_time;
		if (animation.leg_timer > 0.1) {
			animation.leg_timer -= 0.1;
			animation.leg_moving_index++; animation.leg_moving_index %= 6;
		}

		std::array<u8, 5> sprite_order{};
		u8 body_sprite{};
		u8 left_leg_sprite{};
		u8 right_leg_sprite{};
		u8 flower_sprite{};
		u8 cape_sprite{};

		if (!movement.is_moving || get_current_input_mode() != InputMode::EXPLORE) animation.leg_moving_index = 0;

		switch(movement.direction) {
			case CharacterDirection::UP: {
				body_sprite = static_cast<u32>(Sprite::KERRY_UP_BODY_1) + animation.body_index;
				left_leg_sprite = static_cast<u32>(Sprite::KERRY_UP_LEFT_LEG_1) + animation.get_moving_left_leg_sprite();
				right_leg_sprite = static_cast<u32>(Sprite::KERRY_UP_RIGHT_LEG_1) + animation.get_moving_right_leg_sprite();
				flower_sprite = static_cast<u32>(Sprite::KERRY_UP_FLOWER_1) + animation.wind_index;
				cape_sprite = static_cast<u32>(Sprite::KERRY_UP_CAPE_1) + animation.wind_index;
				sprite_order = {flower_sprite, body_sprite, left_leg_sprite, right_leg_sprite, cape_sprite};
			} break;
			case CharacterDirection::DOWN: {
				body_sprite = static_cast<u32>(Sprite::KERRY_DOWN_BODY_1) + animation.body_index;
				left_leg_sprite = static_cast<u32>(Sprite::KERRY_DOWN_LEFT_LEG_1) + animation.get_moving_left_leg_sprite();
				right_leg_sprite = static_cast<u32>(Sprite::KERRY_DOWN_RIGHT_LEG_1) + animation.get_moving_right_leg_sprite();
				flower_sprite = static_cast<u32>(Sprite::KERRY_DOWN_FLOWER_1) + animation.wind_index;
				cape_sprite = static_cast<u32>(Sprite::KERRY_DOWN_CAPE_1) + animation.wind_index;
				sprite_order = {cape_sprite, left_leg_sprite, right_leg_sprite, flower_sprite, body_sprite};
			} break;
			case CharacterDirection::LEFT: {
				body_sprite = static_cast<u32>(Sprite::KERRY_LEFT_BODY_1) + animation.body_index;
				left_leg_sprite = static_cast<u32>(Sprite::KERRY_LEFT_LEFT_LEG_1) + animation.get_moving_left_leg_sprite();
				right_leg_sprite = static_cast<u32>(Sprite::KERRY_LEFT_RIGHT_LEG_1) + animation.get_moving_right_leg_sprite();
				flower_sprite = static_cast<u32>(Sprite::KERRY_LEFT_FLOWER_1) + animation.wind_index;
				cape_sprite = static_cast<u32>(Sprite::KERRY_LEFT_CAPE_1) + animation.wind_index;
				sprite_order = {flower_sprite, left_leg_sprite, right_leg_sprite, body_sprite, cape_sprite};
			} break;
			case CharacterDirection::RIGHT: {
				body_sprite = static_cast<u32>(Sprite::KERRY_RIGHT_BODY_1) + animation.body_index;
				left_leg_sprite = static_cast<u32>(Sprite::KERRY_RIGHT_LEFT_LEG_1) + animation.get_moving_left_leg_sprite();
				right_leg_sprite = static_cast<u32>(Sprite::KERRY_RIGHT_RIGHT_LEG_1) + animation.get_moving_right_leg_sprite();
				flower_sprite = static_cast<u32>(Sprite::KERRY_RIGHT_FLOWER_1) + animation.wind_index;
				cape_sprite = static_cast<u32>(Sprite::KERRY_RIGHT_CAPE_1) + animation.wind_index;
				sprite_order = {flower_sprite, left_leg_sprite, right_leg_sprite, body_sprite, cape_sprite};
			} break;
		}

		for (int i = 0; i < sprite.sprites.size(); i++) {
			Sprite index = static_cast<Sprite>(sprite_order[i]);
			sprite.sprites.at(i) = index;
		}
	}
}
