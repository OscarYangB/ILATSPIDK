#include "animation.h"
#include "SDL3/SDL_timer.h"

bool Animation::is_finished() const {
	if (duration == 0.0) {
		return false; // 0.0 means infinite duration
	}

	return get_animation_time() >= duration;
}

std::vector<Animation> animations {};
static u64 id_counter = 0;

u64 register_animation(Animation animation) {
	id_counter++;
	animation.id = id_counter;
	animation.time_started_ms = SDL_GetTicks();
	animations.push_back(animation);

	return id_counter;
}

u64 play_animation(double duration, double delay, entt::poly<Updater> updater) {
	return register_animation(Animation{duration, delay, updater});
}

void stop_animation(u64 id) {
	std::erase_if(animations, [id](const Animation& animation){ return animation.id == id; });
}

bool animation_playing(u64 id) {
	for (const Animation& animation : animations) {
		if (animation.id == id) return true;
	}
	return false;
}

void update_generic_animation() {
	for (auto& animation : animations) {
		animation.updater->update(animation);
	}

	std::erase_if(animations, [](const Animation& animation) {
		return animation.is_finished() || animation.should_remove;
	});
}

double Animation::get_animation_time() const {
	return std::max(0.0, (SDL_GetTicks() - time_started_ms) / 1000.0 - delay);
}
