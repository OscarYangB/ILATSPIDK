#include "spawn.h"
#include "game.h"
#include "render.h"
#include "movement_controller.h"
#include "collider_data.h"
#include "combat.h"
#include "character_animation.h"
#include "card_data.h"

entt::entity spawn_player() {
	const entt::entity entity = ecs.create();
	add_component(entity, SpriteComp{.sprites = {Sprite::NONE, Sprite::NONE, Sprite::NONE, Sprite::NONE, Sprite::NONE}});
	add_component(entity, TransformComp{});
	add_component(entity, PlayerMovementComp{.speed = 200.f});
	add_component(entity, BoxColliderComp{KERRY_COLLIDER});
	add_component(entity, CharacterDataComp{.name = {"Kerry"}, .starting_health = 150.f, .type = CharacterType::GOOD,
		.starting_deck = make_cards({ CardID::FIREBALL, CardID::SATURN, CardID::MIND_READ, CardID::SATURN, CardID::SATURN, CardID::GRENADE, CardID::GRENADE, CardID::HEAL, CardID::GRENADE })});
	add_component(entity, CharacterAnimComp{});
	add_component(entity, PlayerCharacterComp{});
	return entity;
}

entt::entity spawn_grakeny() {
	const entt::entity entity = ecs.create();
	add_component(entity, SpriteComp{.sprites = {Sprite::GRAKENY_1}});
	add_component(entity, TransformComp{});
	add_component(entity, BoxColliderComp{GRAKENY_COLLIDER});
	add_component(entity, CycleAnimComp{.sprites = {Sprite::GRAKENY_1, Sprite::GRAKENY_2, Sprite::GRAKENY_3}, .frequency = 2.f});
	add_component(entity, CharacterDataComp{.name = {"Grakeny"}, .starting_health = 800.f, .type = CharacterType::EVIL, .starting_deck = make_cards({ })});
	return entity;
}
