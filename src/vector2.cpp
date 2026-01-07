#include "vector2.h"
#include <cmath>

Vector2 Vector2::operator+(const Vector2 other) {
	return Vector2{x + other.x, y + other.y};
}

Vector2 Vector2::operator-(const Vector2 other) {
	return Vector2{x - other.x, y - other.y};
}

float Vector2::operator*(const Vector2 other) {
	return x * other.x + y * other.y;
}

Vector2 Vector2::normalized() {
	float mag = magnitude();
	return Vector2{x / mag, y / mag};
}

float Vector2::magnitude() {
	return std::sqrt(x * x + y * y);
}
