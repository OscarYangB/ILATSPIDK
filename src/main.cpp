#include "game_render.h"
#include <SDL3/SDL.h>
#include <iostream>
#include <entt/entt.hpp>
#include <SDL3/SDL_surface.h>
#include "game.h"
#include "platform_render.h"

void start() {	
	const entt::entity entity = ecs.create();
	ecs.emplace<Sprite>(entity, "assets/test_image.png");
	ecs.emplace<Position>(entity, 0.0f, 0.0f);
}

void update() {
	render_system();
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
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                done = true;
            }
        }

        update();

        // Do game logic, present a frame, etc.
    }

	destroy_window();
    return 0;
}
