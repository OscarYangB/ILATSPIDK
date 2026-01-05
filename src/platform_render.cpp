#include <SDL3/SDL.h>
#include "SDL3/SDL_render.h"
#include "platform_render.h"

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;

bool start_window() {
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

void destroy_window() {
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void render_sprite(const char* name, float x, float y) {
	SDL_RenderClear(renderer);
	
	SDL_Surface* surface = SDL_LoadPNG(name);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

	SDL_FRect fromRect = {0.0f, 0.0f, (float)texture->w, (float)texture->h};
	SDL_FRect toRect = {x, y, (float)texture->w, (float)texture->h};
	
	SDL_RenderTexture(renderer, texture, &fromRect, &toRect);
	SDL_RenderPresent(renderer);
}
