#include "spawn.h"
#include "game.h"
#include "render.h"
#include "movement_controller.h"
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
	character.starting_deck = make_cards({ CardID::FIREBALL, CardID::SATURN, CardID::MIND_READ, CardID::HEAL });

	player_character = entity;
	return entity;
}

entt::entity spawn_grakeny() {
	const entt::entity entity = ecs.create();

	auto& sprite = ecs.emplace<SpriteComponent>(entity);
	sprite.sprites = {Sprite::GRAKENY_1};

	ecs.emplace<TransformComponent>(entity);

	auto& collider = ecs.emplace<BoxColliderComponent>(entity);
	collider = GRAKENY_COLLIDER;

	auto& animation = ecs.emplace<CycleAnimationComponent>(entity);
	animation.sprites = {{Sprite::GRAKENY_1, Sprite::GRAKENY_2, Sprite::GRAKENY_3}};
	animation.frequency = 2.f;

	auto& character = ecs.emplace<CharacterDataComponent>(entity);
	character.starting_health = 100.f;
	character.type = CharacterType::EVIL;
	character.starting_deck = make_cards({ });

	return entity;
}
