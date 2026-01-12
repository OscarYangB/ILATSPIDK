#include "platform_render.h"
#include <SDL3/SDL.h>
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_surface.h"
#include "SDL3/SDL_video.h"
#include <cstdlib>
#include <unordered_map>
#define SDL_STB_FONT_IMPL
#include "../external/sdl-stb-font/sdlStbFont.h"

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

/*
constexpr char data[] {
	#embed "assets/big_file.png"
};

void test_big_image() {
	SDL_IOStream* stream = SDL_IOFromConstMem(&data, sizeof data);
	SDL_Surface* surface = SDL_LoadPNG_IO(stream, true);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_DestroySurface(surface);
	SDL_FRect to_rect = {0.f, 0.f, (float)texture->w, (float)texture->h};

	SDL_RenderTexture(renderer, texture, nullptr, &to_rect);
	SDL_DestroyTexture(texture);
}
*/

void render_sprite(const char* name, float x, float y, float w, float h, u8 index, u32 atlas_w, u32 atlas_h) {
	SDL_Texture* texture = load_sprite(name);

	int atlas_x = (index * atlas_w) % texture->w;
	int atlas_y = ((index * atlas_w) / texture->w) * atlas_h;
	SDL_FRect from_rect = {(float)atlas_x, (float)atlas_y, (float)atlas_w, (float)atlas_h};
	SDL_FRect to_rect = {x, y, w, h};

	SDL_RenderTexture(renderer, texture, &from_rect, &to_rect);
}

void render_nine_slice(const char* name, float x, float y, float w, float h, u8 index, u32 atlas_w, u32 atlas_h,
					   float slice_x, float slice_y, float slice_w, float slice_h, float window_scale) {
	SDL_Texture* texture = load_sprite(name);

	int atlas_x = (index * atlas_w) % texture->w;
	int atlas_y = ((index * atlas_w) / texture->w) * atlas_h;

	float last_segment_width = (float)atlas_w - slice_x - slice_w;
	float last_segment_height = (float)atlas_h - slice_y - slice_h;

	float x_from_locations[4] = {(float)atlas_x, slice_x, slice_w, last_segment_width};
	float x_to_locations[4] = {x, slice_x * window_scale,
							   w - last_segment_width * window_scale - slice_x * window_scale, last_segment_width * window_scale};
	float y_from_locations[4] = {(float)atlas_y, slice_y, slice_h, last_segment_height};
	float y_to_locations[4] = {y, slice_y * window_scale,
							   h - last_segment_height * window_scale - slice_y * window_scale, last_segment_height * window_scale};

	float x_from_location = 0.f;
	float x_to_location = 0.f;
	for (int i = 0; i < 3; i++) {
		x_from_location += x_from_locations[i];
		x_to_location += x_to_locations[i];
		float y_from_location = 0.f;
		float y_to_location = 0.f;
		for (int j = 0; j < 3; j++) {
			y_from_location += y_from_locations[j];
			y_to_location += y_to_locations[j];
			SDL_FRect from_rect = {x_from_location, y_from_location, x_from_locations[i+1], y_from_locations[j+1]};
			SDL_FRect to_rect = {x_to_location, y_to_location, x_to_locations[i+1], y_to_locations[j+1]};
			SDL_RenderTexture(renderer, texture, &from_rect, &to_rect);
		}
	}
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


constexpr char font[] {
	#embed "assets/AtkinsonHyperlegible-Regular.ttf"
};

void render_text(const char* text, u16 x, u16 y, u8 r, u8 g, u8 b) {
	sdl_stb_prerendered_text text_render;

	sdl_stb_font_cache font_cache;
	font_cache.faceSize = 60;
	font_cache.bindRenderer(renderer);
	font_cache.loadFont(font, sizeof font);
	font_cache.renderTextToObject(&text_render, text);

	text_render.drawWithColorMod(x, y, r, g, b, 255);
	text_render.freeTexture();
}
