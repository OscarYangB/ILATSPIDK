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

  float render_width() const;
  float render_height() const;
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
  Vector2 relative_position;

  void get_render_dimensions(float sprite_w, float sprite_h, float* x, float* y, float* w, float* h) const;
};

extern Vector2 camera_position;
extern float camera_scale;

void render_system();
Vector2 world_to_pixel(Vector2 in);
