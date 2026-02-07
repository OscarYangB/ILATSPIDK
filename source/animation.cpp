#include "animation.h"

bool Animation::is_finished() const {
	return get_animation_time() >= duration;
}

double Animation::get_delta_time() const {
	return std::min(FIXED_DELTA_TIME, duration - get_animation_time());
}

void Animation::progress_time() {
	time_elapsed += get_delta_time();
}

std::vector<Animation> animations {};

void play_animation(double duration, double delay, entt::poly<Updater> updater) {
	animations.push_back(Animation{duration, delay, updater});
}

void update_generic_animation() {
	for (auto& animation : animations) {
		if (animation.time_elapsed > animation.delay) {
			animation.updater->update(animation);
		}

		animation.progress_time();
	}

	std::erase_if(animations, [](const Animation& animation) {
		return animation.is_finished();
	});
}

double Animation::get_animation_time() const {
	return std::max(0.0, time_elapsed - delay);
}
