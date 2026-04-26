#include "main_menu.h"
#include "game.h"
#include "button.h"
#include "render.h"
#include "spawn.h"

void button_new_game(entt::entity button) {
	destroy_entities<MainMenuComp>();
	new_game();
}

void create_menu_button(entt::entity parent, ButtonCallback&& callback, std::string_view text) {
	auto entity = ecs.create();
	add_component(entity, SpriteComp{.sprites = {Sprite::TEST_BUTTON}});
	add_component(entity, TextComp{.text = std::string(text), .x_align = XAnchor::CENTER, .y_align = YAnchor::CENTER});
	add_component(entity, ButtonComp{.on_click = callback});
	add_component(entity, NineSliceComp{.x = 40, .y = 30, .w = 320, .h = 150});

	auto& transform = add_component(entity, UITransformComp{.x_anchor = XAnchor::CENTER, .y_anchor = YAnchor::CENTER, .width = 500, .height = 100});
	ecs.get<UITransformComp>(parent).add_child(parent, entity);
}

void create_main_menu() {
	entt::entity parent = ecs.create();
	add_component(parent, UITransformComp{.x_anchor = XAnchor::CENTER, .y_anchor = YAnchor::CENTER, .width = 500, .height = 500});
	add_component(parent, LayoutComp{.axis = Axis::VERTICAL, .spacing = 50.f});
	add_component(parent, MainMenuComp{});

	create_menu_button(parent, nullptr, "Continue");
	create_menu_button(parent, button_new_game, "New Game");
	create_menu_button(parent, nullptr, "Load Game");
	create_menu_button(parent, nullptr, "Settings");

	layout_children(parent);
}
