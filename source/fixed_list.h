#pragma once

#include <array>

template<typename T, size_t max_size>
struct FixedList {
	std::array<T, max_size> array{};
	size_t size;

	void push_back(T value) {
		array[size] = value;
		size++;
		if (size > max_size) {
			throw::std::out_of_range("Out of max size!");
		}
	}

	T pop_back() {
		T return_value = array[size - 1];
		size--;
		return return_value;
	}

	T operator[](size_t index) {
		if (index >= size) {
			throw std::out_of_range("Out of the defined range!");
		}
		return array[index];
	}
};
