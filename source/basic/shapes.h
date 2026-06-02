#pragma once

#include "vector2.h"
#include "fixed_list.h"

#include <algorithm>

struct Box {
	Vector2 left_top{};
	Vector2 right_bottom{};

	Box operator+(const Vector2& offset) const;
	Vector2 center() const;
	float width() const;
	float height() const;
	bool contains_point(const Vector2& point) const;
	Vector2 left_bottom() const;
	Vector2 right_top() const;
	bool is_empty() const;
	float area() const;

	template<typename... Boxes>
	static Box bounds(Boxes... boxes) {
		Box result{{std::numeric_limits<float>::max(), std::numeric_limits<float>::lowest()},
				   {std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max()}};

		([&] {
			result.left_top.x = std::min(result.left_top.x, boxes.left_top.x);
			result.left_top.y = std::max(result.left_top.y, boxes.left_top.y);
			result.right_bottom.x = std::max(result.right_bottom.x, boxes.right_bottom.x);
			result.right_bottom.y = std::min(result.right_bottom.y, boxes.right_bottom.y);
		}(), ...);

		return result;
	}
};

struct Polygon {
	FixedList<Vector2, 6> points{};

	Polygon operator+(const Vector2& offset) const;
	float top();
};
