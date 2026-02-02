#include "animation.h"

bool Animation::is_finished() const {
	return time_elapsed - delay >= duration;
}

void Animation::progress_time() {
	time_elapsed += std::min(delta_time, duration - (time_elapsed - delay));
}

std::vector<Animation> animations {};

void play_animation(double duration, double delay, entt::poly<Updater> updater) {
	animations.push_back(Animation{duration, delay, 0.f, updater});
}

void update_generic_animation() {
	for (auto& animation : animations) {
		if (animation.time_elapsed > animation.delay) {
			animation.updater->update(animation.time_elapsed - animation.delay);
		}

		animation.progress_time();
	}

	std::erase_if(animations, [](const Animation& animation) {
		return animation.is_finished();
	});
}
