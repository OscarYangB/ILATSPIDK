#include "platform_render.h"
#include <SDL3/SDL.h>
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_surface.h"
#include "SDL3/SDL_video.h"
#include "image_assets.h"
#include "render.h"
#include <cstdlib>
#define SDL_STB_FONT_IMPL
#include "../external/sdl-stb-font/sdlStbFont.h"

static SDL_Window* window = nullptr;
static SDL_Renderer* renderer = nullptr;

static SDL_Texture* loaded_sprites[NUMBER_OF_IMAGES] {};

bool start_window() {
    SDL_SetAppMetadata("I Love All The Strange People I Don't Know", "0.1", "");

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return false;
    }

    if (!SDL_CreateWindowAndRenderer("I Love All The Strange People I Don't Know", 1920, 1080, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
		SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return false;
    }

	SDL_SetRenderVSync(renderer, 1); // Make this a setting
    SDL_SetRenderLogicalPresentation(renderer, 1920, 1080, SDL_LOGICAL_PRESENTATION_DISABLED);

    return true;
}

void destroy_window() {
    SDL_DestroyWindow(window);
    SDL_Quit();
}

// Should probably be async
void load_sprite(int index) {
	if (loaded_sprites[index] != nullptr) {
		return; // Sprite already loaded
	}

	SDL_IOStream* stream = SDL_IOFromConstMem(image_data[index], image_sizes[index]);
	SDL_Surface* surface = SDL_LoadPNG_IO(stream, true);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_DestroySurface(surface);
	loaded_sprites[index] = texture;
}

static SDL_Texture* get_sprite(ImageAsset image_asset) {
	u8 image_index = static_cast<u8>(image_asset);
	return loaded_sprites[image_index];
}

void unload_sprite(int index) {;
	if (loaded_sprites[index] == nullptr) {
		return; // Sprite wasn't loaded
	}

	SDL_DestroyTexture(loaded_sprites[index]);
	loaded_sprites[index] = nullptr;
}

void start_render() {
	SDL_RenderClear(renderer);
}

void end_render() {
	SDL_RenderPresent(renderer);
}

void render_sprite(ImageAsset image_asset, float from_x, float from_y, float from_w, float from_h, float to_x, float to_y, float to_w, float to_h) {
	SDL_Texture* texture = get_sprite(image_asset);
	if (texture == nullptr) return;
	SDL_FRect from_rect = {from_x, from_y, from_w, from_h};
	SDL_FRect to_rect = {to_x, to_y, to_w, to_h};
	SDL_RenderTexture(renderer, texture, &from_rect, &to_rect);
}

void render_nine_slice(ImageAsset image_asset, u32 atlas_x, u32 atlas_y, u32 atlas_w, u32 atlas_h, float x, float y, float w, float h,
					   float slice_x, float slice_y, float slice_w, float slice_h, float window_scale) {
	SDL_Texture* texture = get_sprite(image_asset);
	if (texture == nullptr) return;

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

void render_text(const char* text, u16 x, u16 y, u16 w, u16 h, u8 r, u8 g, u8 b, u8 size, u16 mask, HorizontalAnchor x_align, VerticalAnchor y_align) {
	sdl_stb_font_cache font_cache;
	font_cache.faceSize = size;
	font_cache.bindRenderer(renderer);
	font_cache.loadFont(atkinson_hyperlegible, sizeof atkinson_hyperlegible);
	std::vector<sttfont_formatted_text> broken_string;
	font_cache.breakString(text, broken_string, w);

	u16 total_height = 0.f;
	for (auto& string : broken_string) total_height += font_cache.getTextHeight(string);
	if (y_align == VerticalAnchor::CENTER) y += h / 2 - total_height / 2;
	if (y_align == VerticalAnchor::BOTTOM) y += h - total_height;

	u16 height_accumulation = y;
	u16 mask_index = mask;
	for (auto& string : broken_string) {
		std::string masked_string = mask == 0 ? string.getString() : string.substr(0, std::min(static_cast<u16>(string.size()), mask_index));
		mask_index -= std::min(static_cast<u16>(string.size()), mask_index);
		sdl_stb_prerendered_text text_render;
		font_cache.renderTextToObject(&text_render, masked_string);

		u16 render_x = x;
		u16 text_width = font_cache.getTextWidth(string);
		if (x_align == HorizontalAnchor::CENTER) render_x += w / 2 - text_width / 2;
		if (x_align == HorizontalAnchor::RIGHT) render_x += w - text_width;

		text_render.drawWithColorMod(render_x, height_accumulation, r, g, b, 255);
		text_render.freeTexture();
		height_accumulation += font_cache.getTextHeight(string);
	}
}

void platform_debug_draw(const Vector2& start, const Vector2& end) {
#ifndef NDEBUG
	Vector2 pixel_start = world_to_pixel(start);
	Vector2 pixel_end = world_to_pixel(end);
	SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
	SDL_RenderLine(renderer, pixel_start.x, pixel_start.y, pixel_end.x, pixel_end.y);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
#endif
}
