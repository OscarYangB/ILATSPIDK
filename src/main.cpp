#include "entt/entity/fwd.hpp"
#include <SDL3/SDL.h>
#include <iostream>
#include <entt/entt.hpp>

typedef struct {
	int x;
	int y;
} Position, Velocity;

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;

entt::registry ecs;

bool start()
{
    SDL_SetAppMetadata("I Love All The Strange People I don't know", "0.1", "");

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return false;
    }

    if (!SDL_CreateWindowAndRenderer("examples/renderer/clear", 640, 480, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return false;
    }
	
    SDL_SetRenderLogicalPresentation(renderer, 640, 480, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    return true;
}

void update()
{
    const double now = ((double)SDL_GetTicks()) / 1000.0;  /* convert from milliseconds to seconds. */
    /* choose the color for the frame we will draw. The sine wave trick makes it fade between colors smoothly. */
    const float red = (float)(0.5 + 0.5 * SDL_sin(now));
    const float green = (float)(0.5 + 0.5 * SDL_sin(now + SDL_PI_D * 2 / 3));
    const float blue = (float)(0.5 + 0.5 * SDL_sin(now + SDL_PI_D * 4 / 3));
    SDL_SetRenderDrawColorFloat(renderer, red, green, blue, SDL_ALPHA_OPAQUE_FLOAT);  /* new color, full alpha. */

    /* clear the window to the draw color. */
    SDL_RenderClear(renderer);

    /* put the newly-cleared rendering on the screen. */
    SDL_RenderPresent(renderer);


	auto query = ecs.view<const Position>();
	for (auto entity: query) {
		auto &pos = query.get<Position>(entity);
		//		std::cout << std::to_string(pos.x);
	}
}

int main(int argc, char* argv[]) {
	const auto entity = ecs.create();
	ecs.emplace<Position>(entity, 2, 3);
	
    bool done = false;

    bool started = start();

    if (!started) {
        return 1;
    }

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

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
