#pragma once

#include <array>

template<typename T, size_t max_size>
struct FixedList {
	using Array = std::array<T, max_size>;
	Array array{};
	size_t current_size;

	template<typename... Arguments>
	FixedList(Arguments... arguments) : array{arguments...} {
		current_size = sizeof...(Arguments);
	}

	void push_back(T value) {
		array[current_size] = value;
		current_size++;
#ifndef NDEBUG
		if (current_size > max_size) {
			throw::std::out_of_range("Out of max size!");
		}
#endif
	}

	T pop_back() {
		T return_value = array[current_size - 1];
		current_size--;
		return return_value;
	}

	size_t size() const {
		return current_size;
	}

	T& at(size_t index) {
#ifndef NDEBUG
		if (index >= current_size) {
			throw std::out_of_range("Out of the defined range!");
		}
#endif
		return array[index];
	}

	T& operator[](size_t index) {
		return at(index);
	}

	T* begin() {
		return &array[0];
	}

	T* end() {
		if (current_size == 0) {
			return &(array[0]) + 1;
		}

		return (&array[current_size - 1]) + 1;
	}

	bool empty() {
		return current_size == 0;
	}
};
