#pragma once

#include "definitions.h"

struct SDL_Surface;
struct SDL_Texture;

bool start_window();
void destroy_window();

void start_render();
void end_render();
void render_sprite(const char* name, float x, float y, float w, float h, u8 index, u32 atlas_w, u32 atlas_h);
void render_nine_slice(const char* name, float x, float y, float w, float h, u8 index, u32 atlas_w, u32 atlas_h,
					   float slice_x, float slice_y, float slice_w, float slice_h, float window_scale);

int window_width();
int window_height();
