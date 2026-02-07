#pragma once

#include "entt/entt.hpp"

extern entt::registry ecs;
extern double delta_time;

constexpr double FIXED_UPDATE_RATE = 60.0;
constexpr double FIXED_DELTA_TIME = 1.0 / FIXED_UPDATE_RATE;
