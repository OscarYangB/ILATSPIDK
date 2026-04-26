#pragma once
#include "entt/entt.hpp"

struct PlayerCharacterComp {};

void save_game();
void load_game();
void new_game();

entt::entity spawn_player();
entt::entity spawn_grakeny();
