#include "spawn.h"
#include "SDL3/SDL_filesystem.h"
#include "dialog_data.h"
#include "game.h"
#include "render.h"
#include "movement_controller.h"
#include "collider_data.h"
#include "combat.h"
#include "character_animation.h"
#include "card_data.h"
#include "SDL3/SDL_iostream.h"
#include "platform_render.h"
#include "interaction.h"
#include "dialog.h"

constexpr const char* company_name = "bright_yang";
constexpr const char* game_name = "a_basket_full_of_gold";

static std::string get_save_location() {
	const char* save_directory = SDL_GetPrefPath(company_name, game_name);
	std::string save_file_location = save_directory;
	save_file_location += "save.save";
	return save_file_location;
}

void save_game() {
	auto [entity, transform] = get_first_component<TransformComp, PlayerCharacterComp>();
	static constexpr size_t data_size = sizeof(decltype(transform.position));

	SDL_IOStream* stream = SDL_IOFromFile(get_save_location().data(), "w");
	size_t result = SDL_WriteIO(stream, &transform.position, data_size);

	if (result != data_size) {
		show_error("Couldn't save game!");
	}
	SDL_CloseIO(stream);
}

void load_game() {
	auto [entity, transform] = get_first_component<TransformComp, PlayerCharacterComp>();

	SDL_IOStream* stream = SDL_IOFromFile(get_save_location().data(), "r");
	if (stream == nullptr) {
		return;
	}

	size_t data_size = sizeof(decltype(transform.position));
	char data[data_size];
	size_t result = SDL_ReadIO(stream, &data, data_size);
	if (result != data_size) {
		show_error("Couldn't load game!");
	}

	std::memcpy(&transform.position, &data, data_size);
	SDL_CloseIO(stream);
}


void new_game() {
	  entt::entity grakeny = spawn_grakeny();
	  ecs.get<TransformComp>(grakeny).position = Vector2(300.f, 300.f);

	  //entt::entity grakeny_2 = spawn_grakeny();
	  //ecs.get<TransformComp>(grakeny_2).position = Vector2(-300.f, 300.f);

	  spawn_player();

	  // TEST
	  load_game();
	  // ENDTEST

	  { // TABLE
	  const entt::entity entity = ecs.create();
	  auto& sprite = add_component(entity, SpriteComp{.sprites = {Sprite::TABLE}});
	  add_component(entity, TransformComp{.position = {0.f, 0.f}});
	  add_component(entity, BoxColliderComp{TABLE_COLLIDER});
	  add_component(entity, InteractionComp{ .box = sprite.bounding_box(), .on_interact = [](){ start_dialog(TABLE_DIALOG[0]); }});
	  }
	  { // Background
	  const entt::entity background = ecs.create();
	  add_component(background, SpriteComp{.sprites = {Sprite::TEST_BACKGROUND}});
	  add_component(background, TransformComp{.position = Vector2{-1000.0f, 700.0f}});
	  }
}

entt::entity spawn_player() {
	const entt::entity entity = ecs.create();
	add_component(entity, SpriteComp{.sprites = {Sprite::NONE, Sprite::NONE, Sprite::NONE, Sprite::NONE, Sprite::NONE}});
	add_component(entity, TransformComp{});
	add_component(entity, PlayerMovementComp{.speed = 200.f});
	add_component(entity, BoxColliderComp{KERRY_COLLIDER});
	add_component(entity, CharacterDataComp{.name = {"Kerry"}, .starting_health = 150.f, .type = CharacterType::GOOD,
		.inventory = make_cards({ CardID::FIREBALL, CardID::SATURN, CardID::MIND_READ, CardID::SATURN, CardID::SATURN, CardID::GRENADE, CardID::GRENADE, CardID::HEAL, CardID::GRENADE })});
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
	add_component(entity, CharacterDataComp{.name = {"Grakeny"}, .starting_health = 50.f, .type = CharacterType::EVIL, .inventory = make_cards({CardID::GRENADE})});
	return entity;
}
