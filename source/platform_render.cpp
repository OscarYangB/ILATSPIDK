#include "platform_render.h"
#include <SDL3/SDL.h>
#include "image_data.h"
#include "render.h"
#include "font_data.h"
#include "fixed_list.h"

static SDL_Window* window = nullptr;
static SDL_Renderer* renderer = nullptr;

static SDL_Texture* loaded_sprites[NUMBER_OF_IMAGES] {};

void init() {
	for (int i = 0; i < NUMBER_OF_FONTS; i++) {
		load_sprite(static_cast<int>(fonts[i].file));
	}
}

bool start_window() {
    SDL_SetAppMetadata("I Love All The Strange People I Don't Know", "0.1", "");

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        return false;
    }

    if (!SDL_CreateWindowAndRenderer("I Love All The Strange People I Don't Know", 1920, 1080, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        return false;
    }

	//enable_vsync();
    SDL_SetRenderLogicalPresentation(renderer, 1920, 1080, SDL_LOGICAL_PRESENTATION_DISABLED);

	init();

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

	SDL_IOStream* stream = SDL_IOFromConstMem(image_file_data[index], image_file_sizes[index]);
	SDL_Surface* surface = SDL_LoadPNG_IO(stream, true);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_DestroySurface(surface);
	loaded_sprites[index] = texture;
}

static SDL_Texture* get_sprite(ImageFile image_file) {
	u8 image_index = static_cast<u8>(image_file);
	return loaded_sprites[image_index];
}

void unload_sprite(int index) {;
	if (loaded_sprites[index] == nullptr) {
		return; // Sprite wasn't loaded
	}

	for (int i = 0; i < NUMBER_OF_FONTS; i++) {
		if (index == static_cast<int>(fonts[i].file)) {
			return; // Keep fonts permanently loaded for now
		}
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

void render_sprite(ImageFile image_file, float from_x, float from_y, float from_w, float from_h, float to_x, float to_y, float to_w, float to_h, const Colour& tint) {
	SDL_Texture* texture = get_sprite(image_file);
	if (texture == nullptr) return;
	SDL_FRect from_rect = {from_x, from_y, from_w, from_h};
	SDL_FRect to_rect = {to_x, to_y, to_w, to_h};
	SDL_SetTextureColorMod(texture, tint.r, tint.g, tint.b);
	SDL_SetTextureAlphaMod(texture, tint.a);
	SDL_RenderTexture(renderer, texture, &from_rect, &to_rect);
}

void render_nine_slice(ImageFile image_file, u32 atlas_x, u32 atlas_y, u32 atlas_w, u32 atlas_h, float x, float y, float w, float h,
					   float slice_x, float slice_y, float slice_w, float slice_h, float window_scale) {
	SDL_Texture* texture = get_sprite(image_file);
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

void platform_debug_draw(const Vector2& start, const Vector2& end) {
#ifndef NDEBUG
	Vector2 pixel_start = world_to_pixel(start);
	Vector2 pixel_end = world_to_pixel(end);
	SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
	SDL_RenderLine(renderer, pixel_start.x, pixel_start.y, pixel_end.x, pixel_end.y);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
#endif
}

u8 character_to_index(char character) {
	return static_cast<u8>(character) - ENGLISH_STARTING_CHARACTER;
}

constexpr u8 MAX_TEXT_LENGTH = 255;

void render_text(std::string_view text, float x, float y, float w, float h, float size, u8 mask, u8 r, u8 g, u8 b, XAnchor x_align, YAnchor y_align) {
	if (text.empty()) {
		return;
	}

	u8 font_index = 0;
	if (size > fonts[0].height) {
		for (int i = 1; i < NUMBER_OF_FONTS; i++) {
			if (size < fonts[i].height || i == NUMBER_OF_FONTS - 1) {
				font_index = i;
				break;
			}
		}
	}

	//SDL_FRect debug_rect = {x, y, w, h};
	//SDL_RenderRect(renderer, &debug_rect);

	const float right_bound = x + w;
	const float from_width = fonts[font_index].width;
	const float from_height = fonts[font_index].height;
	const float scale = size / from_height;
	const float render_width = from_width * scale;

	float current_x = x;
	float current_y = y;
	const char* first_character = text.data();
	u8 length = mask > 0 ? mask : text.size();
	u8 line_index = 0;
	std::array<u8, MAX_TEXT_LENGTH> line_indices{};
	FixedList<float, MAX_TEXT_LENGTH> line_widths{};
	std::array<float, MAX_TEXT_LENGTH> x_positions{};
	std::array<float, MAX_TEXT_LENGTH> y_positions{};
	int last_space_index = -1;
	for (int i = 0; i < length; i++) {
		char character = *(first_character + i);
		if (character == ' ') last_space_index = i;
		u8 index = character_to_index(character);
		if (i != 0) {
			u8 previous_index = character_to_index(*(first_character + i - 1));
			current_x += kerning[previous_index][index] * (size / 2000.f);
		}
		x_positions[i] = current_x;
		y_positions[i] = current_y;
		line_indices[i] = line_index;
		current_x += (character_widths[index] + 0.5f) * (size / fonts[NUMBER_OF_FONTS - 1].height); // character_widths is based on the pixel width of the last font

		if (current_x > right_bound) { // Line break algorithm
			current_y += size * 0.8f;
			u8 break_index = last_space_index > 0 ? last_space_index + 1 : i + 1;
			float line_width = x_positions[break_index] - x;
			line_widths.push_back(line_width);
			current_x -= line_width;
			line_index++;
			for (int j = break_index; j <= i; j++) {
				x_positions[j] -= line_width;
				y_positions[j] = current_y;
				line_indices[j]++;
			}
		}
	}

	line_widths.push_back(current_x - x);

	float total_height = current_y + size * 0.8f - y;
	for (int i = 0; i < length; i++) {
		float x_align_offset = 0.f;
		float y_align_offset = 0.f;
		switch(x_align) {
			case XAnchor::LEFT: break;
			case XAnchor::CENTER: x_align_offset += (w - line_widths[line_indices[i]]) / 2.0f; break;
			case XAnchor::RIGHT: x_align_offset += w - line_widths[line_indices[i]]; break;
		}
		switch(y_align) {
			case YAnchor::TOP: break;
			case YAnchor::CENTER: y_align_offset += (h - total_height) / 2.0f; break;
			case YAnchor::BOTTOM: y_align_offset += h - total_height; break;
		}
		x_positions[i] += x_align_offset;
		y_positions[i] += y_align_offset;
	}

	SDL_Texture* texture = get_sprite(fonts[font_index].file);
	SDL_SetTextureColorMod(texture, r, g, b);
	for (int i = 0; i < length; i++) {
		char character = *(first_character + i);
		u8 index = character_to_index(character);
		float from_x = index * from_width;
		SDL_FRect to{x_positions[i], y_positions[i], render_width, size};
		SDL_FRect from{from_x, 0.f, from_width, from_height};
		SDL_RenderTexture(renderer, texture, &from, &to); // Investigate performace impact here
	}
}

void enable_vsync() {
	SDL_SetRenderVSync(renderer, 1);
}

void disable_vsync() {
	SDL_SetRenderVSync(renderer, 0);
}
