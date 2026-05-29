#include "random.h"

RandomGeneratorType random_generator;

void init_random() {
	std::random_device device{};
	random_generator = std::mt19937{device()};
}

float random_float(float min_inclusive, float min_exclusive) {
	std::uniform_real_distribution<float> distribution(0.f, 1.f);
	return distribution(random_generator);
}

int random_integer(int min_inclusive, int max_inclusive) {
	std::uniform_int_distribution<int> distribution(min_inclusive, max_inclusive);
	return distribution(random_generator);
}
