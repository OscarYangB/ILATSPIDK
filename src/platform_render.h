#pragma once

#include "definitions.h"

struct SDL_Surface;
struct SDL_Texture;

struct AtlasData {
  u8 index;
  u16 width;
  u16 height;
};

bool start_window();
void destroy_window();

void start_render();
void end_render();
void render_sprite(const char* name, float x, float y, float scale, const AtlasData* atlas_data = nullptr);

int window_width();
int window_height();
