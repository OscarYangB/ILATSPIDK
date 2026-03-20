#include "animation.h"
#include "SDL3/SDL_timer.h"

bool Animation::is_finished() const {
	if (duration == 0.0) {
		return false; // 0.0 means infinite duration
	}

	return get_animation_time() >= duration;
}

std::vector<Animation> animations {}; // TODO consider thread safety
static u64 id_counter = 0;
static bool pause_id_increment = false;

u32 register_animation(Animation animation) {
	if (!pause_id_increment) {
		id_counter++;
	}

	animation.id = id_counter;
	animation.time_started_ms = SDL_GetTicks();
	animations.push_back(animation);

	return id_counter;
}

u32 play_animation(double duration, double delay, entt::poly<Updater> updater) {
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

u32 start_animation_group() {
	if (pause_id_increment) {
		throw std::runtime_error("Animation group has already been started!");
	}

	id_counter++;
	pause_id_increment = true;
	return id_counter;
}

void end_animation_group() {
	pause_id_increment = false;
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
	double time = (SDL_GetTicks() - time_started_ms) / 1000.0 - delay;
	return std::clamp(time, 0.0, duration);
}

double Animation::get_progress() const {
	return get_animation_time() / duration;
}
