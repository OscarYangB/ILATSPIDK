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

	auto& sprite = ecs.emplace<SpriteComp>(entity);
	sprite.sprites = {Sprite::KERRY, Sprite::KERRY, Sprite::KERRY, Sprite::KERRY, Sprite::KERRY};

	ecs.emplace<TransformComp>(entity);

	auto& movement = ecs.emplace<PlayerMovementComp>(entity);
	movement.speed = 200.f;

	auto& collider = ecs.emplace<BoxColliderComp>(entity);
	collider = KERRY_COLLIDER;

	auto& character = ecs.emplace<CharacterDataComp>(entity);
	character.starting_health = 150.f;
	character.type = CharacterType::GOOD;
	character.starting_deck = make_cards({ CardID::FIREBALL, CardID::SATURN, CardID::MIND_READ, CardID::SATURN, CardID::SATURN, CardID::GRENADE, CardID::GRENADE, CardID::HEAL, CardID::GRENADE });

	auto& animation = ecs.emplace<CharacterAnimationComp>(entity);

	ecs.emplace<PlayerCharacterComp>(entity);

	return entity;
}

entt::entity spawn_grakeny() {
	const entt::entity entity = ecs.create();

	auto& sprite = ecs.emplace<SpriteComp>(entity);
	sprite.sprites = {Sprite::GRAKENY_1};

	ecs.emplace<TransformComp>(entity);

	auto& collider = ecs.emplace<BoxColliderComp>(entity);
	collider = GRAKENY_COLLIDER;

	auto& animation = ecs.emplace<CycleAnimationComp>(entity);
	animation.sprites = {Sprite::GRAKENY_1, Sprite::GRAKENY_2, Sprite::GRAKENY_3};
	animation.frequency = 2.f;

	auto& character = ecs.emplace<CharacterDataComp>(entity);
	character.starting_health = 800.f;
	character.type = CharacterType::EVIL;
	character.starting_deck = make_cards({ });

	return entity;
}
