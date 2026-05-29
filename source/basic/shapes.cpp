#include "shapes.h"

#include <cmath>

Box Box::operator+(const Vector2& offset) const {
	return {left_top + offset, right_bottom + offset};
}

Vector2 Box::center() const {
	return {(left_top.x + right_bottom.x) / 2.f, (right_bottom.y + left_top.y) / 2.f};
}

float Box::width() const {
	return std::abs(right_bottom.x - left_top.x);
}

float Box::height() const {
	return std::abs(left_top.y - right_bottom.y);
}

bool Box::contains_point(const Vector2& point) const {
	return point.x > left_top.x && point.x < right_bottom.x && point.y > right_bottom.y && point.y < left_top.y;
}

Vector2 Box::left_bottom() const {
	return {left_top.x, right_bottom.y};
}

Vector2 Box::right_top() const {
	return {right_bottom.x, left_top.y};
}

bool Box::is_empty() const {
	return width() == 0 || height() == 0;
}

float Box::area() const {
	return width() * height();
}

Polygon Polygon::operator+(const Vector2& offset) const {
	Polygon result{};
	for (const Vector2& point : points) {
		result.points.push_back(point + offset);
	}
	return result;
}
