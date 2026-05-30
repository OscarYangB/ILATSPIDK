#include "spawn.h"
#include "dialog.h"
#include "cards.h"

#include "../engine/physics.h"
#include "../data/position_data.h"
#include "../data/dialog_data.h"
#include "../engine/input.h"
#include "../engine/movement_controller.h"
#include "../engine/character_animation.h"
#include "../engine/platform_render.h"
#include "../engine/interaction.h"
#include "../engine/animation.h"

#include <SDL3/SDL_iostream.h>
#include <SDL3/SDL_filesystem.h>

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

entt::entity create_image(Sprite sprite, const Box& collider_box) {
	auto entity = ecs.create();
	add_component(entity, TransformComp{});
	add_component(entity, SpriteComp{.sprites = {sprite}});
	if (!collider_box.is_empty()) {
		add_component(entity, BoxColliderComp{.box = collider_box});
	}
	return entity;
}

entt::entity create_collider(const Box& collider_box) {
	auto entity = ecs.create();
	add_component(entity, TransformComp{});
	add_component(entity, BoxColliderComp{.box = collider_box});
	return entity;
}

entt::entity create_collider(const Polygon& points) {
	auto entity = ecs.create();
	add_component(entity, TransformComp{});
	add_component(entity, PolygonColliderComp{points});
	return entity;
}

void center_scene(Sprite background) {
	ecs.ctx().get<CameraSingleton>().camera_follow = false;
	const auto& dimensions = image_dimensions[static_cast<size_t>(sprite_to_image_file[static_cast<size_t>(background)])];
	ecs.ctx().get<CameraSingleton>().camera_position = {dimensions.width / 2.f, -dimensions.height / 2.f};
}

void enrette_office() {
	// TODO fade out/in
	create_image(Sprite::ENRETTEOFFICE_BG_1, {});
	create_image(Sprite::ENRETTEOFFICE_SHELF_1, ENRETTEOFFICE_SHELF_BOX);
	create_image(Sprite::ENRETTEOFFICE_DRAWER_1, ENRETTEOFFICE_DRAWER_BOX);
	create_image(Sprite::ENRETTEOFFICE_TABLE_1, ENRETTEOFFICE_TABLE_BOX);
	create_image(Sprite::ENRETTEOFFICE_CHAIR_1, ENRETTEOFFICE_CHAIR_BOX);
	create_image(Sprite::ENRETTEOFFICE_COUCH_1, ENRETTEOFFICE_COUCH_BOX);
	auto vent = create_image(Sprite::ENRETTEOFFICE_VENT_1, {});
	add_component(vent, CycleAnimComp{.sprites = {Sprite::ENRETTEOFFICE_VENT_1, Sprite::ENRETTEOFFICE_VENT_2, Sprite::ENRETTEOFFICE_VENT_3}, .frequency = 24.f});
	create_image(Sprite::ENRETTEOFFICE_GARBAGE_1, ENRETTEOFFICE_GARBAGE_BOX);
	create_image(Sprite::ENRETTEOFFICE_BOOKS_1, ENRETTEOFFICE_BOOKS_BOX);
	create_collider(ENRETTEOFFICE_BORDER);
	center_scene(Sprite::ENRETTEOFFICE_BG_1);
}

void new_game() {
	push_input_mode(InputMode::EXPLORE);

	ecs.ctx().emplace<CameraSingleton>();
	play_animation(3.f, 0.f, &CameraSingleton::brightness, entt::null, [](Animation& animation, float starting_value) {
		return linear_curve(1.f, animation, 0.f);
	});

	entt::entity grakeny = spawn_grakeny();
	ecs.get<TransformComp>(grakeny).position = ENRETTEOFFICE_ENEMY_POSITION;

	//entt::entity grakeny_2 = spawn_grakeny();
	//ecs.get<TransformComp>(grakeny_2).position = Vector2(-300.f, 300.f);

	auto player = spawn_player();
	ecs.get<TransformComp>(player).position = ENRETTEOFFICE_PLAYER_POSITION;

	//load_game();

	{ // TABLE
	  auto entity = ecs.create();
	  auto& sprite = add_component(entity, SpriteComp{.sprites = {Sprite::TABLE}});
	  add_component(entity, TransformComp{.position = {0.f, 0.f}});
	  add_component(entity, BoxColliderComp{KERRY_COLLISION});
	  add_component(entity, InteractionComp{ .box = sprite.bounding_box(), .on_interact = [](){ start_dialog(TABLE_DIALOG); }});
	}
	{ // Tutorial trigger
	  auto entity = ecs.create();
	  add_component(entity, TransformComp{});
	  // add_component(entity, InteractionComp{.box = TEST_BOX,
	  // 										.on_interact = [](){ start_dialog(TUTORIAL_2);}, .type = InteractionType::PLAYER_ENTER});
	}

	enrette_office();

	//start_dialog(TUTORIAL_1);
}

entt::entity spawn_player() {
	const entt::entity entity = ecs.create();
	add_component(entity, SpriteComp{.sprites = {Sprite::NONE, Sprite::NONE, Sprite::NONE, Sprite::NONE, Sprite::NONE}});
	add_component(entity, TransformComp{});
	add_component(entity, PlayerMovementComp{.speed = 200.f});
	add_component(entity, BoxColliderComp{KERRY_COLLISION});
	add_component(entity, CharacterDataComp{.name = {"Kerry"}, .starting_health = 150.f, .type = CharacterType::GOOD,
		.inventory = make_cards({ CardID::FIREBALL, CardID::SATURN, CardID::MIND_READ, CardID::SATURN, CardID::SATURN, CardID::GRENADE, CardID::GRENADE, CardID::HEAL, CardID::GRENADE })});
	add_component(entity, CharacterAnimComp{});
	add_component(entity, PlayerCharacterComp{});
	add_component(entity, PerspectiveComp{});
	return entity;
}

entt::entity spawn_grakeny() {
	const entt::entity entity = ecs.create();
	add_component(entity, SpriteComp{.sprites = {Sprite::GRAKENY_1}});
	add_component(entity, TransformComp{});
	add_component(entity, BoxColliderComp{KERRY_COLLISION});
	add_component(entity, CycleAnimComp{.sprites = {Sprite::GRAKENY_1, Sprite::GRAKENY_2, Sprite::GRAKENY_3}, .frequency = 2.f});
	add_component(entity, CharacterDataComp{.name = {"Grakeny"}, .starting_health = 50.f, .type = CharacterType::EVIL, .inventory = make_cards({CardID::GRENADE})});
	return entity;
}
