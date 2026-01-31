#include "SDL3/SDL_events.h"
#include "SDL3/SDL_timer.h"
#include "collider_data.h"
#include "image_assets.h"
#include "audio_assets.h"
#include "audio.h"
#include "physics.h"
#include "render.h"
#include <SDL3/SDL.h>
#include <iostream>
#include <entt/entt.hpp>
#include <SDL3/SDL_surface.h>
#include "game.h"
#include "platform_render.h"
#include "input.h"
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
	{ // Kerry
		const entt::entity entity = ecs.create();
		auto& sprite = ecs.emplace<SpriteComponent>(entity);
		sprite.renderable = {SpriteGroup<5>{}};
		ecs.emplace<TransformComponent>(entity);
		auto& movement = ecs.emplace<PlayerMovementComponent>(entity);
		movement.speed = 200.f;
		auto& collider = ecs.emplace<BoxColliderComponent>(entity);
		collider = KERRY_COLLIDER;
	}
	{ // TABLE
		const entt::entity entity = ecs.create();
		auto& sprite = ecs.emplace<SpriteComponent>(entity);
		sprite.renderable = {SpriteGroup<1>{AtlasIndex::TABLE}};
		auto& transform = ecs.emplace<TransformComponent>(entity);
		transform.position = {0.f, 0.f};
		auto& collider = ecs.emplace<BoxColliderComponent>(entity);
		collider = TABLE_COLLIDER;
	}
	{ // Background
		// const entt::entity background = ecs.create();
		// auto& sprite = ecs.emplace<SpriteComponent>(background);
		// sprite.renderable = {SpriteGroup<1>{AtlasIndex::TEST_BACKGROUND}};
		// auto& transform = ecs.emplace<TransformComponent>(background);
		// transform.position = Vector2{-1500.0f, 1000.0f};
	}
	{ // Button
		const entt::entity entity = ecs.create();
		auto& sprite = ecs.emplace<SpriteComponent>(entity);
		sprite.renderable = {SpriteGroup<1>{AtlasIndex::TEST_BUTTON}};
		auto& transform = ecs.emplace<AnchoredTransformComponent>(entity);
		transform.x_anchor = HorizontalAnchor::CENTER; transform.y_anchor = VerticalAnchor::BOTTOM; transform.width = 800; transform.height = 400;
		auto& button = ecs.emplace<Button>(entity);
		button.on_hover = button_hovered; button.on_click = button_clicked;
		auto& nine = ecs.emplace<NineSliceComponent>(entity);
		nine.x = 40; nine.y = 30; nine.w = 320; nine.h = 150;
	}
	{ // Text
		const entt::entity entity = ecs.create();
		auto& text = ecs.emplace<TextComponent>(entity);
		text.text = "hello world hello world hello world hellow world hwllo world hellow world hello world hello world hellow world";
		text.r = 255; text.g = 0; text.b = 100; text.size = 100;
		text.x_align = HorizontalAnchor::RIGHT;
		text.y_align = VerticalAnchor::BOTTOM;
		auto& transform = ecs.emplace<AnchoredTransformComponent>(entity);
		transform.x_anchor = HorizontalAnchor::CENTER; transform.y_anchor = VerticalAnchor::BOTTOM; transform.width = 800; transform.height = 400;
	}

	init_audio();
	play_audio(AudioAsset::SUCCESS_AUDIO);
}

void update() {
	update_input();
	update_button();
	update_movement();
	update_sprite_resources();
	update_render();
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
			//std::cout << "sleeping for: " + std::to_string(frame_time - time_elapsed);
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
    }

	destroy_window();
    return 0;
}
