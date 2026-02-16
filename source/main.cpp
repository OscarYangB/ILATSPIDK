#include "collider_data.h"
#include "image_assets.h"
#include "audio.h"
#include "kerry_anim_controller.h"
#include "physics.h"
#include "render.h"
#include <SDL3/SDL.h>
#include <iostream>
#include <entt/entt.hpp>
#include "game.h"
#include "platform_render.h"
#include "input.h"
#include "button.h"
#include "player_movement_controller.h"
#include "interaction.h"
#include "animation.h"
#include "dialog_data.h"
#include "dialog.h"
#include "spawn.h"

static u64 start_frame_time = 0.0;

void button_hovered() {
	std::cout << "button hovered\n";
}

void button_clicked() {
	std::cout << "button clicked\n";
}

void start() {
	spawn_player();

	{ // TABLE
		const entt::entity entity = ecs.create();
		auto& sprite = ecs.emplace<SpriteComponent>(entity);
		sprite.sprites = {AtlasIndex::TABLE};
		auto& transform = ecs.emplace<TransformComponent>(entity);
		transform.position = {0.f, 0.f};
		auto& collider = ecs.emplace<BoxColliderComponent>(entity);
		collider = TABLE_COLLIDER;
		auto& interaction = ecs.emplace<InteractionComponent>(entity);
		interaction.box = sprite.bounding_box();
		interaction.on_interact = [](){ start_dialog(TABLE_DIALOG[0]); };

		// play_animation(20.0, 0.0, &TransformComponent::position, entity, [](Animation& animation, Vector2 current_value) {
		// 	return Vector2{current_value.x, sinusoid_curve(500.f, 1.f, 0.f, animation, current_value.y)};
		// });
	}
	{ // Background
		const entt::entity background = ecs.create();
		auto& sprite = ecs.emplace<SpriteComponent>(background);
		sprite.sprites = {AtlasIndex::TEST_BACKGROUND};
		auto& transform = ecs.emplace<TransformComponent>(background);
		transform.position = Vector2{-1000.0f, 700.0f};
	}
	{ // Button
		// const entt::entity entity = ecs.create();
		// auto& sprite = ecs.emplace<SpriteComponent>(entity);
		// sprite.renderable = {SpriteGroup<1>{AtlasIndex::TEST_BUTTON}};
		// auto& transform = ecs.emplace<AnchoredTransformComponent>(entity);
		// transform.x_anchor = HorizontalAnchor::CENTER; transform.y_anchor = VerticalAnchor::BOTTOM; transform.width = 800; transform.height = 400;
		// auto& button = ecs.emplace<Button>(entity);
		// button.on_hover = button_hovered; button.on_click = button_clicked;
		// auto& nine = ecs.emplace<NineSliceComponent>(entity);
		// nine.x = 40; nine.y = 30; nine.w = 320; nine.h = 150;
	}
	{ // Text
		// const entt::entity entity = ecs.create();
		// auto& text = ecs.emplace<TextComponent>(entity);
		// text.text = "hello world hello world hello world hellow world hwllo world hellow world hello world hello world hellow world";
		// text.r = 255; text.g = 0; text.b = 100; text.size = 100;
		// text.x_align = HorizontalAnchor::RIGHT;
		// text.y_align = VerticalAnchor::BOTTOM;
		// auto& transform = ecs.emplace<AnchoredTransformComponent>(entity);
		// using namespace entt::literals;
		// transform.x_anchor = HorizontalAnchor::CENTER; transform.y_anchor = VerticalAnchor::BOTTOM; transform.width = 800; transform.height = 400;
	}

	init_audio();
	//play_audio(AudioAsset::SUCCESS_AUDIO);

	//play_animation(10.f, 0.0f, &camera_scale, [](auto... params) { return sinusoid_curve(0.2f, 3.f, 0.f, params...); });
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
	update_character_animation();
	update_sprite_resources();
	update_render();

	auto [sprite, transform] = ecs.get<SpriteComponent, TransformComponent>(player_character);
	camera_position = Vector2::lerp(camera_position, sprite.bounding_box().center() + transform.position, 0.1f); // This is some bullshit
}

double fixed_update_timer = 0.0;

void fixed_update() {
	update_generic_animation();
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
			//std::cout << "sleeping for: " + std::to_string(frame_time - time_elapsed) << "\n";
			//SDL_DelayNS(frame_time - time_elapsed); // Do this if based on a setting or if VSync is busted
		}

		delta_time = (SDL_GetTicksNS() - start_frame_time) / 1000000000.0;
		start_frame_time = SDL_GetTicksNS();

		fixed_update_timer += delta_time;
		while (fixed_update_timer > FIXED_DELTA_TIME) {
			fixed_update();
			fixed_update_timer -= FIXED_DELTA_TIME;
		}

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                done = true;
            }

			handle_input_event(event.key);
        }

        update();
    }

	destroy_window();
    return 0;
}
