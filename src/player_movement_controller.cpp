#include "player_movement_controller.h"
#include "game.h"
#include "input.h"
#include "render.h"

void update_movement() {
	auto view = ecs.view<TransformComponent, PlayerMovementComponent>();

	for (auto [entity, transform, movement] : view.each()) {
		Vector2 movement_direction = Vector2::zero();

		if (input_held(RIGHT) && !input_held(LEFT)) {
			movement.direction = CharacterDirection::RIGHT;
			movement_direction.x = 1.f;
		} else if (input_held(LEFT) && !input_held(RIGHT)) {
			movement.direction = CharacterDirection::LEFT;
			movement_direction.x = -1.f;
		}

		if (input_held(UP) && !input_held(DOWN)) {
			movement.direction = CharacterDirection::UP;
			movement_direction.y = 1.f;
		} else if (input_held(DOWN) && !input_held(UP)) {
			movement.direction = CharacterDirection::DOWN;
			movement_direction.y = -1.f;
		}

		if (movement_direction.magnitude() > 0.f) {
			transform.move(entity, transform.position + movement_direction.normalized() * movement.speed * delta_time);
			movement.is_moving = true;
		} else {
			movement.is_moving = false;
		}
	}
}
