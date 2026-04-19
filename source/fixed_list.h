#pragma once

#include <array>
#include <cassert>

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
		assert(current_size < max_size);

		array[current_size] = value;
		current_size++;
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
		assert(index < current_size);
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

	bool contains(const T& t) {
		for (int i = 0; i < current_size; i++) {
			if (t == at(i)) {
				return true;
			}
		}

		return false;
	}

	void erase(size_t index) {
		assert(index < current_size);
		for (size_t i = index; i < current_size - 1; i++) {
			array[i] = std::move(array[i+1]);
		}
		current_size--;
	}

	template<typename FunctionType>
	void erase_if(FunctionType should_erase) {
		for (size_t i = 0; i < current_size;) {
			if (should_erase(array[i])) {
				erase(i);
			} else {
				i++;
			}
		}
	}
};
