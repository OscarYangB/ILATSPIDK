#include "interaction.h"
#include "game.h"
#include "input.h"
#include "player_movement_controller.h"
#include "render.h"

void update_interact() {
	if (!input_down_this_frame(InputType:: INTERACT)) {
		return;
	}
	handle_input(InputType::INTERACT);

	constexpr float INTERACTION_RANGE = 100.f;

	auto [player_movement, player_transform, player_sprite] = ecs.get<PlayerMovementComponent, TransformComponent, SpriteComponent>(player_character);

	Vector2 direction = Vector2::down();
	switch (player_movement.direction) {
	case CharacterDirection::UP:
		direction = Vector2::up();
		break;
	case CharacterDirection::DOWN:
		direction = Vector2::down();
		break;
	case CharacterDirection::LEFT:
		direction = Vector2::left();
		break;
	case CharacterDirection::RIGHT:
		direction = Vector2::right();
		break;
	}

	std::vector<InteractionComponent*> results {};
	raytest<InteractionComponent>(results, player_transform.position + player_sprite.bounding_box().center(), direction, INTERACTION_RANGE);
	for (InteractionComponent* component : results) {
		if (component && component->on_interact) component->on_interact();
	}
}
