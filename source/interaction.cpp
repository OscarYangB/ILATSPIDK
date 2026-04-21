#include "interaction.h"
#include "game.h"
#include "input.h"
#include "movement_controller.h"
#include "render.h"
#include "physics.h"
#include "spawn.h"

void update_interact() {
	if (!input_down_this_frame(InputType:: INTERACT)) {
		return;
	}
	handle_input(InputType::INTERACT);

	constexpr float INTERACTION_RANGE = 100.f;

	auto [entity, movement, transform, sprite] = get_first_component<PlayerMovementComp, TransformComp, SpriteComp, PlayerCharacterComp>();

	Vector2 direction = Vector2::down();
	switch (movement.direction) {
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

	std::vector<InteractionComp*> results {};
	raytest<InteractionComp>(results, transform.position + sprite.bounding_box().center(), direction, INTERACTION_RANGE);
	for (InteractionComp* component : results) {
		if (component && component->on_interact) component->on_interact();
	}
}
