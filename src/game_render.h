#pragma once

#include "vector2.h"
#include "definitions.h"

enum class ImageAsset;

struct Transform {
  Vector2 position;
};

struct Sprite {
  ImageAsset image_asset;
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
  HorizontalAnchor x_anchor;
  VerticalAnchor y_anchor;
  Vector2 relative_position;
  u16 width;
  u16 height;

  Vector2 render_position() const;
  float render_width() const;
  float render_height() const;
};

struct NineSliceComponent {
	u16 x;
	u16 y;
	u16 w;
	u16 h;
};

extern Vector2 camera_position;
extern float camera_scale;

void render_system();
Vector2 world_to_pixel(Vector2 in);
