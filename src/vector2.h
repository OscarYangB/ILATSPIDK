#pragma once

struct Vector2 {
	float x;
	float y;

	Vector2 operator+(const Vector2& other);
	Vector2 operator-(const Vector2& other);
	float operator*(const Vector2& other);
	Vector2 operator*(const float other);
	Vector2 operator/(const float other);
	Vector2& operator+=(const Vector2& other);
	Vector2& operator-=(const Vector2& other);
	Vector2& operator*=(const float other);
	Vector2& operator/=(const float other);
	Vector2 normalized();
	float magnitude();

	static inline Vector2 zero() { return Vector2{0.f, 0.f}; }
	static inline Vector2 one() { return Vector2{1.f, 1.f}; }
	static inline Vector2 up() { return Vector2{0.f, 1.f}; }
	static inline Vector2 down() { return Vector2{0.f, -1.f}; }
	static inline Vector2 left() { return Vector2{-1.f, 0.f}; }
	static inline Vector2 right() { return Vector2{1.f, 0.f}; }
};
