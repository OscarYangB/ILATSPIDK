#pragma once
#include <random>

using RandomGeneratorType = std::mt19937;
extern RandomGeneratorType random_generator;

void init_random();
float random_float(float min_inclusive = 0.f, float min_exclusive = 1.f);
int random_integer(int min, int max);
