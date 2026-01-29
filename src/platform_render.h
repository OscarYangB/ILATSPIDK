#pragma once

#include "definitions.h"

struct SDL_Surface;
struct SDL_Texture;
enum class ImageAsset;

bool start_window();
void destroy_window();

void start_render();
void end_render();
void render_sprite(ImageAsset image_asset, float from_x, float from_y, float from_w, float from_h, float to_x, float to_y, float to_w, float to_h);
void render_nine_slice(ImageAsset image_asset, u32 atlas_x, u32 atlas_y, u32 atlas_w, u32 atlas_h, float x, float y, float w, float h,
					   float slice_x, float slice_y, float slice_w, float slice_h, float window_scale);

void load_sprite(int index);
void unload_sprite(int index);

int window_width();
int window_height();

void render_text(const char* text, u16 x, u16 y, u16 w, u8 r, u8 g, u8 b, u8 size);
