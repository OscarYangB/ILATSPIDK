#include "interaction.h"
#include "game.h"
#include "input.h"
#include "movement_controller.h"
#include "render.h"
#include "physics.h"

constexpr double HIGHLIGHT_ANIMATION_TIME = 0.1;

void update_interact() {
	constexpr float INTERACTION_RANGE = 100.f;

	auto [entity, movement, transform, box] = get_first_component<PlayerMovementComp, TransformComp, BoxColliderComp, PlayerCharacterComp>();

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
	raytest<InteractionComp>(results, transform.position + box.box.center() - Vector2{0.f, box.box.height() / 2.f}, direction, INTERACTION_RANGE);
	for (InteractionComp* component : results) {
		if (component && component->type == InteractionType::INTERACT && component->enabled && (component->can_interact == nullptr || component->can_interact()) && component->on_interact != nullptr) {
			if (input_down_this_frame(InputType::INTERACT)) {
				handle_input(InputType::INTERACT);
				component->on_interact();
			} else {
				component->highlight = true;
			}
			break;
		}
	}

	for (auto [entity, sprite, interaction] : ecs.view<SpriteComp, InteractionComp>().each()) {
		interaction.highlight_time = interaction.highlight ? std::min(interaction.highlight_time + delta_time, HIGHLIGHT_ANIMATION_TIME) : 0.f;
		sprite.outline_thickness = interaction.highlight ? 5.f * (interaction.highlight_time / HIGHLIGHT_ANIMATION_TIME) : 0.f;
		interaction.highlight = false;
	}
}

void update_player_enter() {
	auto [player_entity, player_transform, player_collider] = get_first_component<PlayerCharacterComp, TransformComp, BoxColliderComp>();
	for (auto [entity, interaction, transform] : ecs.view<InteractionComp, TransformComp>().each()) {
		bool is_player_in_box = (interaction.box + transform.position).contains_point(player_transform.position + player_collider.box.center());
		if (interaction.enabled && interaction.type == InteractionType::PLAYER_ENTER && !interaction.is_player_inside &&
			(interaction.can_interact == nullptr || interaction.can_interact()) && is_player_in_box && interaction.on_interact) {
			interaction.on_interact();
			interaction.is_player_inside = true;
		} else if (!is_player_in_box && interaction.is_player_inside) {
			interaction.is_player_inside = false;
		}
	}
}
