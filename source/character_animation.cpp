#include "character_animation.h"
#include "render.h"
#include "game.h"

void update_cycle_animations() {
	auto view = ecs.view<CycleAnimationComponent, SpriteComponent>();

	for (auto [entity, animation, sprite] : view.each()) {
		const double animation_delta = 1.0 / animation.frequency;
		animation.timer += delta_time;
		if (animation.timer > animation_delta) {
			animation.timer -= animation_delta;
			animation.index++;
			if (animation.index >= animation.sprites.size()) {
				switch (animation.finish_behaviour) {
					case FinishBehaviour::LOOP: animation.index %= animation.sprites.size(); break;
					case FinishBehaviour::DESTROY_COMPONENT: ecs.remove<CycleAnimationComponent>(entity); break;
					case FinishBehaviour::DESTROY_ENTITY: ecs.destroy(entity); break;
				}
				continue;
			}
		}

		sprite.sprites.at(0) = animation.sprites.at(animation.index);
	}
}
