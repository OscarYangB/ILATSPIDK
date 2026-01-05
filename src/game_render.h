#pragma once

struct Position {
	float x;
	float y;
};

struct Sprite {
	const char* name;
};

struct Position;
struct Sprite;

extern Position camera_position;

void render_system();
Position world_to_pixel(Position in);
Position pixel_to_world(Position in);
