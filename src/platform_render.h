#pragma once

#include "definitions.h"

struct SDL_Surface;
struct SDL_Texture;

struct AtlasData {
  u8 index;
  int w;
  int h;
};

bool start_window();
void destroy_window();
void render_sprite(const char* name, float x, float y, float scale, const AtlasData* atlas_data = nullptr);
