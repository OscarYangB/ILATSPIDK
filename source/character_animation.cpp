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
			animation.index %= animation.sprites.size();
		}

		sprite.sprites[0] = animation.sprites[animation.index];
	}
}
