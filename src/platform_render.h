#pragma once

struct SDL_Surface;
struct SDL_Texture;

bool start_window();
void destroy_window();
void render_sprite(const char* name, float x, float y);
