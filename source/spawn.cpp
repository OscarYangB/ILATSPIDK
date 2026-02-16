#include "spawn.h"
#include "game.h"
#include "render.h"
#include "movement_controller.h"
#include "physics.h"
#include "collider_data.h"
#include "combat.h"
#include "character_animation.h"

entt::entity spawn_player() {
	const entt::entity entity = ecs.create();

	auto& sprite = ecs.emplace<SpriteComponent>(entity);
	sprite.sprites = {Sprite::KERRY, Sprite::KERRY, Sprite::KERRY, Sprite::KERRY, Sprite::KERRY};

	ecs.emplace<TransformComponent>(entity);

	auto& movement = ecs.emplace<PlayerMovementComponent>(entity);
	movement.speed = 200.f;

	auto& collider = ecs.emplace<BoxColliderComponent>(entity);
	collider = KERRY_COLLIDER;

	auto& character = ecs.emplace<CharacterDataComponent>(entity);
	character.starting_health = 100.f;
	character.type = CharacterType::GOOD;
	character.starting_deck = make_cards({ CardID::FIREBALL, CardID::FIREBALL, CardID::FIREBALL, CardID::FIREBALL });

	player_character = entity;
	return entity;
}

entt::entity spawn_grakeny() { // WIP
	const entt::entity entity = ecs.create();

	auto& sprite = ecs.emplace<SpriteComponent>(entity);
	sprite.sprites = {};

	ecs.emplace<TransformComponent>(entity);

	auto& collider = ecs.emplace<BoxColliderComponent>(entity);
	//collider = KERRY_COLLIDER;

	auto& animation = ecs.emplace<CycleAnimationComponent>(entity);
	animation.sprites = {{Sprite::CAPTAIN_ORANGE, Sprite::KERRY_DOWN_BODY_2}};

	auto& character = ecs.emplace<CharacterDataComponent>(entity);
	character.starting_health = 100.f;
	character.type = CharacterType::EVIL;
	character.starting_deck = make_cards({ CardID::FIREBALL, CardID::FIREBALL, CardID::FIREBALL, CardID::FIREBALL });

	return entity;
}
