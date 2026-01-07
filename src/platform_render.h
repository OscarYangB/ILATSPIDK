#pragma once

#include "definitions.h"

struct SDL_Surface;
struct SDL_Texture;

bool start_window();
void destroy_window();

void start_render();
void end_render();
void render_sprite(const char* name, float x, float y, float w, float h, u8 index, u32 atlas_w, u32 atlas_h);

int window_width();
int window_height();
