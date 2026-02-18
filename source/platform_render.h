#pragma once

#include "definitions.h"

struct SDL_Surface;
struct SDL_Texture;
enum class ImageFile;
enum class VerticalAnchor;
enum class HorizontalAnchor;
class Vector2;
struct Colour;

bool start_window();
void destroy_window();

void start_render();
void end_render();
void render_sprite(ImageFile image_file, float from_x, float from_y, float from_w, float from_h, float to_x, float to_y, float to_w, float to_h, const Colour& tint);
void render_nine_slice(ImageFile image_file, u32 atlas_x, u32 atlas_y, u32 atlas_w, u32 atlas_h, float x, float y, float w, float h,
					   float slice_x, float slice_y, float slice_w, float slice_h, float window_scale);

void load_sprite(int index);
void unload_sprite(int index);

int window_width();
int window_height();

void render_text(const char* text, u16 x, u16 y, u16 w, u16 h, u8 r, u8 g, u8 b, u8 size, u16 mask, HorizontalAnchor x_align, VerticalAnchor y_align);

void platform_debug_draw(const Vector2& start, const Vector2& end);
