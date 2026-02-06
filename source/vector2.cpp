#include "vector2.h"
#include <cmath>
#include <stdexcept>

bool Vector2::operator==(const Vector2& other) const {
	return x == other.x && y == other.y;
}

Vector2 Vector2::operator+(const Vector2& other) const {
	return Vector2{x + other.x, y + other.y};
}

Vector2 Vector2::operator-(const Vector2& other) const {
	return Vector2{x - other.x, y - other.y};
}

float Vector2::operator*(const Vector2& other) const {
	return x * other.x + y * other.y;
}

Vector2 Vector2::operator*(const float other) const {
	return Vector2{x * other, y * other};
}

Vector2 Vector2::operator/(const float other) const {
	return Vector2{x / other, y / other};
}

Vector2& Vector2::operator+=(const Vector2& other) {
	this->x += other.x;
	this->y += other.y;
	return *this;
}

Vector2& Vector2::operator-=(const Vector2& other) {
	this->x -= other.x;
	this->y -= other.y;
	return *this;
}

Vector2& Vector2::operator*=(const float other) {
	this->x *= other;
	this->y *= other;
	return *this;
}

Vector2& Vector2::operator/=(const float other) {
	this->x /= other;
	this->y /= other;
	return *this;
}

Vector2 Vector2::normalized() {
	if (x == 0.f && y == 0.f) {
		throw std::runtime_error("Cannot normalize the zero vector!");
	}

	float mag = magnitude();
	return Vector2{x / mag, y / mag};
}

float Vector2::magnitude() {
	return std::sqrt(x * x + y * y);
}

float Vector2::distance(const Vector2& first, const Vector2& second) {
	return (first - second).magnitude();
}
