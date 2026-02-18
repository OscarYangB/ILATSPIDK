#pragma once

struct Vector2 {
	float x;
	float y;

	bool operator==(const Vector2& other) const;
	Vector2 operator+(const Vector2& other) const;
	Vector2 operator-(const Vector2& other) const;
	float operator*(const Vector2& other) const;
	Vector2 operator*(const float other) const;
	Vector2 operator/(const float other) const;
	Vector2& operator+=(const Vector2& other);
	Vector2& operator-=(const Vector2& other);
	Vector2& operator*=(const float other);
	Vector2& operator/=(const float other);
	Vector2 normalized();
	float magnitude();

	static float distance(const Vector2& first, const Vector2& second);
	static Vector2 lerp(const Vector2& first, const Vector2& second, float scalar);

	static constexpr Vector2 zero() { return Vector2{0.f, 0.f}; }
	static constexpr Vector2 one() { return Vector2{1.f, 1.f}; }
	static constexpr Vector2 up() { return Vector2{0.f, 1.f}; }
	static constexpr Vector2 down() { return Vector2{0.f, -1.f}; }
	static constexpr Vector2 left() { return Vector2{-1.f, 0.f}; }
	static constexpr Vector2 right() { return Vector2{1.f, 0.f}; }
};

struct Box {
	Vector2 left_top;
	Vector2 right_bottom;

	Box operator+(const Vector2& offset) const;
	Vector2 center();
	float width();
	float height();
};
