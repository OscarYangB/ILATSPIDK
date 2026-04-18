#include "collider_data.h"
#include "image_data.h"
#include "audio.h"
#include "render.h"
#include <SDL3/SDL.h>
#include <iostream>
#include <entt/entt.hpp>
#include "game.h"
#include "platform_render.h"
#include "input.h"
#include "button.h"
#include "movement_controller.h"
#include "interaction.h"
#include "animation.h"
#include "dialog_data.h"
#include "dialog.h"
#include "spawn.h"
#include "character_animation.h"
#include "combat.h"
#include "random.h"

void button_hovered() {
	std::cout << "button hovered\n";
}

void button_clicked() {
	std::cout << "button clicked\n";
}

void start() {
	refresh_window_scale();
	init_audio();
	init_random();

	entt::entity grakeny = spawn_grakeny();
	ecs.get<TransformComp>(grakeny).position = Vector2(300.f, 300.f);
	spawn_player();

	{ // TABLE
		const entt::entity entity = ecs.create();
		auto& sprite = add_component(entity, SpriteComp{.sprites = {Sprite::TABLE}});
		add_component(entity, TransformComp{.position = {0.f, 0.f}});
		add_component(entity, BoxColliderComp{TABLE_COLLIDER});
		add_component(entity, InteractionComp{ .box = sprite.bounding_box(), .on_interact = [](){ start_dialog(TABLE_DIALOG[0]); }});
	}
	{ // Background
		const entt::entity background = ecs.create();
		add_component(background, SpriteComp{.sprites = {Sprite::TEST_BACKGROUND}});
		add_component(background, TransformComp{.position = Vector2{-1000.0f, 700.0f}});
	}
}

static void update_process_input() {
	switch(get_current_input_mode()) {
		case InputMode::EXPLORE: {
			update_interact();
			update_movement();
			break;
		}
		case InputMode::DIALOG: {
			update_dialog();
			break;
		}
		case InputMode::MENU: {
			break;
		}
		case InputMode::COMBAT: {
			break;
		}
	}

	update_button();
	input_end_frame();
}

static void update() {
	update_input();
	update_process_input();

	auto [entity, sprite, transform] = *ecs.view<SpriteComp, TransformComp, PlayerCharacterComp>().each().begin();
	camera_position = Vector2::lerp(camera_position, sprite.bounding_box().center() + transform.position, 0.1f); // This is some bullshit

	update_combat();

	update_generic_animation();
	update_character_animation();
	update_cycle_animations();

	update_sprite_resources();
	update_render();
}

int main(int argc, char* argv[]) {
    bool done = false;

    bool started = start_window();

    if (!started) {
        return 1;
    }

	start();
    std::cout << "Initialized" << "\n";

	u64 start_frame_time = 0.0;

    while (!done) {
		const u64 frame_time = 8000000; // In nanoseconds
		const u64 time_elapsed = SDL_GetTicksNS() - start_frame_time;
		if (time_elapsed < frame_time) {
			//std::cout << "sleeping for: " + std::to_string(frame_time - time_elapsed) << "\n";
			//SDL_DelayNS(frame_time - time_elapsed); // Do this if based on a setting or if VSync is busted
		}

		delta_time = (SDL_GetTicksNS() - start_frame_time) / 1000000000.0;
		start_frame_time = SDL_GetTicksNS();

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
			switch(event.type) {
			case SDL_EVENT_QUIT: done = true; break;
			case SDL_EVENT_WINDOW_RESIZED: refresh_window_scale(); break;
			case SDL_EVENT_KEY_DOWN: handle_input_event(event.key); break;
			}
        }

        update();
    }

	destroy_window();
    return 0;
}
