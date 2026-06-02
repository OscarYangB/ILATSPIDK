#pragma once

#include <entt/entt.hpp>

void save_game();
void load_game();
void new_game();

void update_transition_scene();

entt::entity spawn_player();
entt::entity spawn_grakeny();
