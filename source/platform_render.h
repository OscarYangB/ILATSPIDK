#pragma once

#include "definitions.h"
#include <string_view>

struct SDL_Surface;
struct SDL_Texture;
enum class ImageFile;
enum class YAnchor;
enum class XAnchor;
class Vector2;
struct Colour;

bool start_window();
void destroy_window();

void start_render();
void end_render();
void render_sprite(ImageFile image_file, float from_x, float from_y, float from_w, float from_h, float to_x, float to_y, float to_w, float to_h, const Colour& tint);
void render_nine_slice(ImageFile image_file, float from_x, float from_y, float from_w, float from_h, float to_x, float to_y, float to_w, float to_h,
					   float slice_x, float slice_y, float slice_w, float slice_h, float window_scale);

void load_sprite(int index);
void unload_sprite(int index);

int window_width();
int window_height();

void render_text(std::string_view text, float x, float y, float w, float h, float size, u8 mask, u8 r, u8 g, u8 b, u8 a, XAnchor x_align, YAnchor y_align, bool render_background);

void platform_debug_draw(const Vector2& start, const Vector2& end);

void enable_vsync();
void disable_vsync();
