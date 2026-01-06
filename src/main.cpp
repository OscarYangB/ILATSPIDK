#include "SDL3/SDL_events.h"
#include "game_render.h"
#include <SDL3/SDL.h>
#include <iostream>
#include <entt/entt.hpp>
#include <SDL3/SDL_surface.h>
#include "game.h"
#include "platform_render.h"
#include "game_input.h"

static u64 start_frame_time = 0.0;

double delta_time() {
	return (SDL_GetTicks() - start_frame_time) / 1000.0;
}

void start() {	
	const entt::entity entity = ecs.create();
	ecs.emplace<Sprite>(entity, "assets/test_image.png");
	ecs.emplace<Position>(entity, 0.0f, 0.0f);
}

void update() {
	render_system();

	// TO REMOVE--this is just to test moving the camera
	if (inputs[UP].isDown) {
		camera_position.y += 20.0f * delta_time();
	} else if (inputs[LEFT].isDown) {
		camera_position.x -= 20.0f * delta_time();
	} else if (inputs[DOWN].isDown) {
		camera_position.y -= 20.0f * delta_time();
	} else if (inputs[RIGHT].isDown) {
		camera_position.x += 20.0f * delta_time();
	} else if (inputs[INTERACT].isDown) {
		camera_scale += 0.5f * delta_time();
	} else if (inputs[INVENTORY].isDown) {
		camera_scale -= 0.5f * delta_time();					
	}
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
		start_frame_time = SDL_GetTicks();
		
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                done = true;
            }		 

			handle_input_event(event.key);			
        }
		
		update_input();
        update();

        // Do game logic, present a frame, etc.
    }

	destroy_window();
    return 0;
}
