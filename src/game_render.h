#pragma once

#include "vector2.h"
#include "definitions.h"

struct Transform {
  Vector2 position;
};

struct Sprite {
  const char* name;
  u16 width;
  u16 height;
  u8 atlas_index;
};

enum class VerticalAnchor {
  TOP,
  BOTTOM,
  CENTER,
};

enum class HorizontalAnchor {
  LEFT,
  RIGHT,
  CENTER,
};

struct AnchoredTransform {
  VerticalAnchor y_anchor;
  HorizontalAnchor x_anchor;
  Vector2 position;
};

extern Vector2 camera_position;
extern float camera_scale;

void render_system();
Vector2 world_to_pixel(Vector2 in);
Vector2 pixel_to_world(Vector2 in);
