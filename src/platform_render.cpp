#include "platform_render.h"
#include <SDL3/SDL.h>
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_surface.h"
#include "SDL3/SDL_video.h"
#include <cstdlib>
#include <unordered_map>

static SDL_Window* window = nullptr;
static SDL_Renderer* renderer = nullptr;

static std::unordered_map<const char*, SDL_Texture*> loaded_sprites {};

bool start_window() {
    SDL_SetAppMetadata("I Love All The Strange People I Don't Know", "0.1", "");

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return false;
    }

    if (!SDL_CreateWindowAndRenderer("I Love All The Strange People I Don't Know", 640, 480, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
		SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return false;
    }

    SDL_SetRenderLogicalPresentation(renderer, 640, 480, SDL_LOGICAL_PRESENTATION_DISABLED);

    return true;
}

void destroy_window() {
    SDL_DestroyWindow(window);
    SDL_Quit();
}

// Lazy load the textures into memory for now.
// Could be interesting to do some sort of reference counting
// There could be a component to async load all the resources an entity needs
static SDL_Texture* load_sprite(const char* name) {
	if (loaded_sprites.find(name) == loaded_sprites.end()) {
		SDL_Surface* surface = SDL_LoadPNG(name);
		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_DestroySurface(surface);
		loaded_sprites[name] = texture;
	}

	return loaded_sprites[name];
}

static void unload_sprite(const char* name) {
	SDL_DestroyTexture(loaded_sprites[name]);
	loaded_sprites.erase(name);
}

void start_render() {
	SDL_RenderClear(renderer);
}

void end_render() {
	SDL_RenderPresent(renderer);
}

void render_sprite(const char* name, float x, float y, float w, float h, u8 index, u32 atlas_w, u32 atlas_h) {
	SDL_Texture* texture = load_sprite(name);

	int atlas_x = (index * atlas_w) % texture->w;
	int atlas_y = ((index * atlas_w) / texture->w) * atlas_h;
	SDL_FRect from_rect = {(float)atlas_x, (float)atlas_y, (float)atlas_w, (float)atlas_h};
	SDL_FRect to_rect = {x, y, w, h};

	SDL_RenderTexture(renderer, texture, &from_rect, &to_rect);
}

int window_width() {
	int w, h = 0;
	SDL_GetWindowSize(window, &w, &h);
	return w;
}

int window_height() {
	int w, h = 0;
	SDL_GetWindowSize(window, &w, &h);
	return h;
}
