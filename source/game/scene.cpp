#include "scene.h"
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

#include <SDL3/SDL_iostream.h>
#include <SDL3/SDL_filesystem.h>

constexpr const char* company_name = "bright_yang";
constexpr const char* game_name = "a_basket_full_of_gold";

enum class SceneTransitionDirection {
	IN,
	OUT,
};

using SceneFunction = void(*)();
constexpr float SCENE_TRANSITION_TIME = 0.3f;

struct SceneTransitionSingleton {
	SceneTransitionDirection scene_transition_direction{};
	float scene_transition_timer = 0.f;
	SceneFunction scene_function = nullptr;
	Vector2 scene_transition_position{};
};

struct SceneSingleton {
	std::vector<entt::entity> scene_entities{};
};

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

template<typename... Components>
entt::entity scene_create(Components&&... components) {
	auto entity = ecs.create();
	ecs.ctx().get<SceneSingleton>().scene_entities.push_back(entity);
	add_component(entity, TransformComp{});

	([entity, &components...]() {
		add_component(entity, std::forward<Components>(components));
	}(), ...);

	return entity;
}

void free_camera() {
	ecs.ctx().get<CameraSingleton>().camera_follow = true;
	auto [player, player_transform, sprite] = get_first_component<TransformComp, SpriteComp, PlayerCharacterComp>();
	ecs.ctx().get<CameraSingleton>().camera_position = player_transform.position + sprite.bounding_box().center();
}

void center_scene(Sprite background) {
	ecs.ctx().get<CameraSingleton>().camera_follow = false;
	const auto& dimensions = image_dimensions[static_cast<size_t>(sprite_to_image_file[static_cast<size_t>(background)])];
	ecs.ctx().get<CameraSingleton>().camera_position = {dimensions.width / 2.f, -dimensions.height / 2.f};
}

void transition_scene(SceneFunction new_scene, const Vector2& new_position) {
	auto& singleton = ecs.ctx().emplace<SceneTransitionSingleton>();
	singleton.scene_function = new_scene;
	singleton.scene_transition_position = new_position;
	singleton.scene_transition_timer = 0.f;
	singleton.scene_transition_direction = SceneTransitionDirection::OUT;
}

void update_transition_scene() {
	if (!ecs.ctx().contains<SceneTransitionSingleton>()) {
		return;
	}
	auto& singleton = ecs.ctx().get<SceneTransitionSingleton>();

	float brightness = singleton.scene_transition_timer / SCENE_TRANSITION_TIME;
	if (singleton.scene_transition_direction == SceneTransitionDirection::OUT) {
		brightness = 1.f - brightness;
	}
	singleton.scene_transition_timer += delta_time;
	if (singleton.scene_transition_timer > SCENE_TRANSITION_TIME) {
		if (singleton.scene_transition_direction == SceneTransitionDirection::OUT) {
			for (entt::entity entity : ecs.ctx().get<SceneSingleton>().scene_entities) {
				ecs.destroy(entity);
			}
			ecs.ctx().get<SceneSingleton>().scene_entities.clear();
			auto [player, player_transform, player_box] = get_first_component<TransformComp, BoxColliderComp, PlayerCharacterComp>();
			player_transform.position = singleton.scene_transition_position - player_box.box.center();
			singleton.scene_function();
			singleton.scene_transition_timer = 0.f;
			singleton.scene_transition_direction = SceneTransitionDirection::IN;
			brightness = 0.f;
		} else {
			brightness = 1.f;
			ecs.ctx().erase<SceneTransitionSingleton>();
		}
	}
	ecs.ctx().get<CameraSingleton>().brightness = brightness;
}

void office_hall();

void office_reception() {
	scene_create(SpriteComp{.sprites = {Sprite::RECEPTION_BACKGROUND_1}});
	scene_create(SpriteComp{.sprites = {Sprite::RECEPTION_WALL_1}});
	scene_create(SpriteComp{.sprites = {Sprite::RECEPTION_SOFA_1}},
				 BoxColliderComp{.box = LD::RECEPTION_SOFA_1});
	scene_create(SpriteComp{.sprites = {Sprite::RECEPTION_SOFA2_1}},
				 BoxColliderComp{.box = LD::RECEPTION_SOFA_2});
	scene_create(SpriteComp{.sprites = {Sprite::RECEPTION_TABLE_1}},
				 BoxColliderComp{.box = LD::RECEPTION_TABLE});
	scene_create(SpriteComp{.sprites = {Sprite::RECEPTION_DESK_1}},
				 PolygonColliderComp{.polygon = LD::RECEPTION_DESK});
	scene_create(PolygonColliderComp{.polygon = LD::RECEPTION_BORDER});
	scene_create(InteractionComp{.box = LD::RECEPTION_TO_HALLWAY,
								 .on_interact = [](){transition_scene(office_hall, LD::OFFICE_HALL_RECEPTION_ENTRANCE);},
								 .type = InteractionType::PLAYER_ENTER});
	center_scene(Sprite::RECEPTION_BACKGROUND_1);
}

inline static bool office_lights_on = false;

void office_turn_on_lights() {
	ecs.ctx().erase<TintSingleton>();
	office_lights_on = true;
}

void enrette_office() {
	scene_create(SpriteComp{.sprites = {Sprite::ENRETTEOFFICE_BG_1}});
	entt::entity border = scene_create(SpriteComp{.sprites = {Sprite::ENRETTEOFFICE_BORDER_1}});
	entt::entity light_switch = scene_create(SpriteComp{.sprites = {Sprite::ENRETTEOFFICE_SWITCH_1}},
											 InteractionComp{.box = LD::ENRETTEOFFICE_LIGHT_SWITCH,
															 .on_interact = [](){start_dialog(LIGHT_SWITCH);},
															 .can_interact = [](){return !office_lights_on;},
															 .type = InteractionType::INTERACT});
	scene_create(SpriteComp{.sprites = {Sprite::ENRETTEOFFICE_CHARGING_1}},
				 InteractionComp{.box = LD::ENRETTEOFFICE_CHARGING,
								 .on_interact = [](){if (office_lights_on) start_dialog(CHARGING_STATION);},
								 .type = InteractionType::INTERACT},
				 BoxColliderComp{.box = LD::ENRETTEOFFICE_CHARGING});
	scene_create(SpriteComp{.sprites = {Sprite::ENRETTEOFFICE_SHELF_1}},
				 BoxColliderComp{.box = LD::ENRETTEOFFICE_SHELF_BOX});
	scene_create(SpriteComp{.sprites = {Sprite::ENRETTEOFFICE_DRAWER_1}},
				 BoxColliderComp{.box = LD::ENRETTEOFFICE_DRAWER_BOX});
	scene_create(SpriteComp{.sprites = {Sprite::ENRETTEOFFICE_TABLE_1}},
				 BoxColliderComp{.box = LD::ENRETTEOFFICE_TABLE_BOX},
				 InteractionComp{.box = LD::ENRETTEOFFICE_TABLE_BOX,
								 .on_interact = [](){start_dialog(DESK);},
								 .can_interact = [](){return office_lights_on;},
								 .type = InteractionType::INTERACT});
	scene_create(SpriteComp{.sprites = {Sprite::ENRETTEOFFICE_CHAIR_1}},
				 BoxColliderComp{.box = LD::ENRETTEOFFICE_CHAIR_BOX});
	scene_create(SpriteComp{.sprites = {Sprite::ENRETTEOFFICE_COUCH_1}},
				 BoxColliderComp{.box = LD::ENRETTEOFFICE_COUCH_BOX});
	scene_create(SpriteComp{.sprites = {Sprite::NONE}},
				 CycleAnimComp{.sprites = {Sprite::ENRETTEOFFICE_VENT_1, Sprite::ENRETTEOFFICE_VENT_2,
										   Sprite::ENRETTEOFFICE_VENT_3}, .frequency = 24.f});
	scene_create(SpriteComp{.sprites = {Sprite::ENRETTEOFFICE_GARBAGE_1}},
				 BoxColliderComp{.box = LD::ENRETTEOFFICE_GARBAGE_BOX});
	scene_create(SpriteComp{.sprites = {Sprite::ENRETTEOFFICE_BOOKS_1}},
				 BoxColliderComp{.box = LD::ENRETTEOFFICE_BOOKS_BOX},
				 InteractionComp{.box = LD::ENRETTEOFFICE_BOOKS_BOX,
								 .on_interact = [](){start_dialog(BOOKS);},
								 .can_interact = [](){return office_lights_on;},
								 .type = InteractionType::INTERACT});
	scene_create(PolygonColliderComp{.polygon = LD::ENRETTEOFFICE_BORDER});
	scene_create(InteractionComp{.box = LD::ENRETTEOFFICE_DOOR_BOX,
								 .on_interact = [](){start_dialog(DOOR);},
								 .can_interact = [](){return office_lights_on;},
								 .type = InteractionType::PLAYER_ENTER});
	scene_create(SpriteComp{.sprites = Sprite::ENRETTEOFFICE_CALENDAR_1},
				 InteractionComp{.box = LD::ENRETTEOFFICE_CALENDAR,
								 .on_interact = [](){start_dialog(CALENDAR);},
								 .can_interact = [](){return office_lights_on;},
								 .type = InteractionType::INTERACT});
	auto [player_entity] = get_first_component<PlayerCharacterComp>();
	if (!office_lights_on) {
		ecs.ctx().emplace<TintSingleton>(TintSingleton{.tint = Colour{15, 15, 20, 255}, .excluded_entities = {player_entity, border, light_switch}});
	}
	center_scene(Sprite::ENRETTEOFFICE_BG_1);
}

void office_go_to_hallway() {
	transition_scene(office_hall, LD::OFFICE_HALL_OFFICE_ENTRANCE);
}

void office_hall() {
	scene_create(SpriteComp{.sprites = {Sprite::HALLWAY_1}});
	scene_create(PolygonColliderComp{.polygon = LD::OFFICE_HALL_BORDER});
	scene_create(InteractionComp{.box = LD::OFFICE_HALL_TO_RECEPTION,
								 .on_interact = [](){transition_scene(office_reception, LD::RECEPTION_HALLWAY_POSITION);},
								 .type = InteractionType::PLAYER_ENTER});
	scene_create(InteractionComp{.box = LD::OFFICE_HALL_TO_OFFICE,
								 .on_interact = [](){transition_scene(enrette_office, LD::ENRETTEOFFICE_ENTRANCE_POSITION);},
								 .type = InteractionType::PLAYER_ENTER});
	free_camera();
}

void new_game() {
	push_input_mode(InputMode::EXPLORE);

	ecs.ctx().emplace<CameraSingleton>();
	ecs.ctx().emplace<SceneSingleton>();

	entt::entity grakeny = spawn_grakeny();
	ecs.get<TransformComp>(grakeny).position = LD::ENRETTEOFFICE_ENEMY_POSITION;

	auto player = spawn_player();
	ecs.get<TransformComp>(player).position = LD::RECEPTION_ELEVATOR_ENTRANCE;

	//load_game();

	{ // TABLE
	  auto entity = ecs.create();
	  auto& sprite = add_component(entity, SpriteComp{.sprites = {Sprite::TABLE}});
	  add_component(entity, TransformComp{.position = {100.f, -100.f}});
	  add_component(entity, BoxColliderComp{LD::KERRY_COLLISION});
	  //add_component(entity, InteractionComp{ .box = sprite.bounding_box(), .on_interact = [](){ start_dialog(TABLE_DIALOG); }});
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
	add_component(entity, BoxColliderComp{LD::KERRY_COLLISION});
	add_component(entity, CharacterDataComp{.name = {"Kerry"}, .starting_health = 150.f, .type = CharacterType::GOOD,
		.deck = make_cards({ CardID::FIREBALL, CardID::SATURN, CardID::MIND_READ, CardID::SATURN, CardID::SATURN, CardID::GRENADE, CardID::GRENADE, CardID::HEAL, CardID::GRENADE })});
	add_component(entity, CharacterAnimComp{});
	add_component(entity, PlayerCharacterComp{});
	add_component(entity, PerspectiveComp{});
	return entity;
}

entt::entity spawn_grakeny() {
	const entt::entity entity = ecs.create();
	add_component(entity, SpriteComp{.sprites = {Sprite::GRAKENY_1}});
	add_component(entity, TransformComp{});
	add_component(entity, BoxColliderComp{LD::KERRY_COLLISION});
	add_component(entity, CycleAnimComp{.sprites = {Sprite::GRAKENY_1, Sprite::GRAKENY_2, Sprite::GRAKENY_3}, .frequency = 2.f});
	add_component(entity, CharacterDataComp{.name = {"Grakeny"}, .starting_health = 50.f, .type = CharacterType::EVIL, .deck = make_cards({CardID::GRENADE})});
	return entity;
}

bool is_scene_transitioning() {
	return ecs.ctx().contains<SceneTransitionSingleton>();
}
