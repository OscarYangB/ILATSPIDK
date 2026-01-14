#include "SDL3/SDL_events.h"
#include "SDL3/SDL_timer.h"
#include "game_assets.h"
#include "game_audio.h"
#include "game_render.h"
#include <SDL3/SDL.h>
#include <iostream>
#include <entt/entt.hpp>
#include <SDL3/SDL_surface.h>
#include "game.h"
#include "platform_render.h"
#include "game_input.h"
#include "button.h"
#include "player_movement_controller.h"

static u64 start_frame_time = 0.0;

void button_hovered() {
	std::cout << "button hovered\n";
}

void button_clicked() {
	std::cout << "button clicked\n";
}

void start() {
	const entt::entity entity = ecs.create();
	ecs.emplace<Sprite>(entity, ImageAsset::KERRY_IMAGE, u16{200}, u16{300}, u8{12});
	ecs.emplace<Transform>(entity, Vector2{0.0f, 0.0f});
	ecs.emplace<PlayerMovementComponent>(entity, 200.f, CharacterDirection::DOWN);

	const entt::entity entity2 = ecs.create();
	ecs.emplace<Sprite>(entity2, ImageAsset::CAPTAIN_ORANGE_IMAGE, u16{200}, u16{300}, u8{12});
	ecs.emplace<Transform>(entity2, Vector2{500.0f, 500.0f});

	const entt::entity background = ecs.create();
	ecs.emplace<Sprite>(background, ImageAsset::TEST_BACKGROUND_IMAGE, u16{2339}, u16{1654}, u8{0});
	ecs.emplace<Transform>(background, Vector2{-1500.0f, 1000.0f});

	const entt::entity button = ecs.create();
	ecs.emplace<Sprite>(button, ImageAsset::TEST_BUTTON_IMAGE, u16{400}, u16{200}, u8{0});
	ecs.emplace<AnchoredTransform>(button, HorizontalAnchor::CENTER, VerticalAnchor::BOTTOM,
								   Vector2{0.0f, 0.0f}, u16{800}, u16{400});
	ecs.emplace<Button>(button, button_hovered, button_clicked, nullptr);
	ecs.emplace<NineSliceComponent>(button, u16{40}, u16{30}, u16{320}, u16{150});

	init_audio();
	play_audio(AudioAsset::SUCCESS_AUDIO);
}

void update() {
	update_input();
	button_system();
	update_movement();
	render_system();

	// TO REMOVE--this is just to test moving the camera
	// if (input_held(UP)) {
	// 	camera_position.y += 300.0f * delta_time;
	// }
	// if (input_held(LEFT)) {
	// 	camera_position.x -= 300.0f * delta_time;
	// }
	// if (input_held(DOWN)) {
	// 	camera_position.y -= 300.0f * delta_time;
	// }
	// if (input_held(RIGHT)) {
	// 	camera_position.x += 300.0f * delta_time;
	// }
	if (input_held(INTERACT)) {
		camera_scale += 0.5f * delta_time;
	}
	if (input_held(INVENTORY)) {
		camera_scale -= 0.5f * delta_time;
	}

	input_end_frame();
}

int main(int argc, char* argv[]) {
    bool done = false;

    bool started = start_window();

    if (!started) {
        return 1;
    }

	start();
    std::cout << "Initialized";

    while (!done) {
		const u64 frame_time = 8000000; // In nanoseconds
		const u64 time_elapsed = SDL_GetTicksNS() - start_frame_time;
		if (time_elapsed < frame_time) {
			SDL_DelayNS(frame_time - time_elapsed);
		}

		delta_time = (SDL_GetTicksNS() - start_frame_time) / 1000000000.0;
		start_frame_time = SDL_GetTicksNS();

        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                done = true;
            }

			handle_input_event(event.key);
        }

        update();

        // Do game logic, present a frame, etc.
    }

	destroy_window();
    return 0;
}
