#pragma once

struct Vector2 {
  float x;
  float y;

  Vector2 operator+(const Vector2 other);
  Vector2 operator-(const Vector2 other);
  float operator*(const Vector2 other);
  Vector2 normalized();
  float magnitude();
};
