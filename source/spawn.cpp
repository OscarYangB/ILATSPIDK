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
	ecs.emplace<SpriteComp>(entity, SpriteComp{.sprites = {Sprite::NONE, Sprite::NONE, Sprite::NONE, Sprite::NONE, Sprite::NONE}});
	ecs.emplace<TransformComp>(entity);
	ecs.emplace<PlayerMovementComp>(entity, PlayerMovementComp{.speed = 200.f});
	ecs.emplace<BoxColliderComp>(entity, KERRY_COLLIDER);
	ecs.emplace<CharacterDataComp>(entity, CharacterDataComp{.starting_health = 150.f, .type = CharacterType::GOOD,
		.starting_deck = make_cards({ CardID::FIREBALL, CardID::SATURN, CardID::MIND_READ, CardID::SATURN, CardID::SATURN, CardID::GRENADE, CardID::GRENADE, CardID::HEAL, CardID::GRENADE })});

	ecs.emplace<CharacterAnimComp>(entity);
	ecs.emplace<PlayerCharacterComp>(entity);
	return entity;
}

entt::entity spawn_grakeny() {
	const entt::entity entity = ecs.create();
	ecs.emplace<SpriteComp>(entity, SpriteComp{.sprites = {Sprite::GRAKENY_1}});
	ecs.emplace<TransformComp>(entity);
	ecs.emplace<BoxColliderComp>(entity, GRAKENY_COLLIDER);
	ecs.emplace<CycleAnimComp>(entity, CycleAnimComp{.sprites = {Sprite::GRAKENY_1, Sprite::GRAKENY_2, Sprite::GRAKENY_3}, .frequency = 2.f});
	ecs.emplace<CharacterDataComp>(entity, CharacterDataComp{.starting_health = 800.f, .type = CharacterType::EVIL, .starting_deck = make_cards({ })});
	return entity;
}
