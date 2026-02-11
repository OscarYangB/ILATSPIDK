#include "dialog.h"
#include "game.h"
#include "render.h"
#include "input.h"
#include "button.h"

entt::entity background = entt::null;
entt::entity dialog_text = entt::null;
u16 text_index = 0; // TODO
const Dialog* dialog = nullptr;
DialogVisitor visitor;

std::vector<ChoiceButton> choice_buttons = {};

constexpr float WIDTH = 800.f;
constexpr float HEIGHT = 400.f;
constexpr float X_MARGIN = 50.f;
constexpr float Y_MARGIN = 50.f;
constexpr float CHOICE_BUTTON_HEIGHT = 100.f;

void start_dialog(const Dialog& new_dialog) {
	text_index = 0;
	visitor.index = 1;
	dialog = &new_dialog;

	{
		background = ecs.create();
		auto& transform = ecs.emplace<AnchoredTransformComponent>(background);
		transform.x_anchor = HorizontalAnchor::CENTER; transform.y_anchor = VerticalAnchor::BOTTOM; transform.width = WIDTH; transform.height = HEIGHT;
		auto& nine = ecs.emplace<NineSliceComponent>(background);
		nine.x = 40; nine.y = 30; nine.w = 320; nine.h = 150;
		auto& sprite = ecs.emplace<SpriteComponent>(background);
		sprite.renderable = {SpriteGroup<1>{AtlasIndex::TEST_BUTTON}};
	}
	{
		dialog_text = ecs.create();
		auto& transform = ecs.emplace<AnchoredTransformComponent>(dialog_text);
		transform.x_anchor = HorizontalAnchor::CENTER; transform.y_anchor = VerticalAnchor::BOTTOM; transform.width = WIDTH - X_MARGIN; transform.height = HEIGHT - Y_MARGIN;
		auto& text_component = ecs.emplace<TextComponent>(dialog_text);
		text_component.text = "test";
	}

	progress_dialog();
}

bool in_dialog() {
	return visitor.index != 0;
}

void progress_dialog() {
	do {
		visitor.proceed = false;
		std::visit(visitor, *(dialog + visitor.index - 1));
		if (visitor.index == 0) {
			end_dialog();
			break;
		}
	} while (visitor.proceed);
}

void end_dialog() {
	ecs.destroy(background);
	ecs.destroy(dialog_text);
	background = entt::null;
	dialog_text = entt::null;
}

void update_dialog_input() {
	if (visitor.index == 0) {
		return;
	}

	if (choice_buttons.size() > 0) {
		return;
	}

	if (input_down_this_frame(InputType::INTERACT)) {
		handle_input(InputType::INTERACT);
		progress_dialog();
	}
}

void delete_choice_buttons() {
	for (const ChoiceButton& button : choice_buttons) {
		ecs.destroy(button.button);
		ecs.destroy(button.text);
	}

	choice_buttons.clear();
}

void choice_made() {
	for (const ChoiceButton& button : choice_buttons) {
		const ButtonComponent& button_component = ecs.get<ButtonComponent>(button.button);
		if (button_component.is_hovered) {
			visitor.index = button.jump_index;
			delete_choice_buttons();
			progress_dialog();
			return;
		}
	}
}

void make_choice_button(const char* choice_text, u16 jump_index) {
	AnchoredTransformComponent transform;
	transform.x_anchor = HorizontalAnchor::CENTER; transform.y_anchor = VerticalAnchor::BOTTOM; transform.width = WIDTH; transform.height = CHOICE_BUTTON_HEIGHT;
	transform.relative_position = { X_MARGIN, -1.f - CHOICE_BUTTON_HEIGHT * choice_buttons.size()};

	auto text = ecs.create();
	auto& text_transform = ecs.emplace<AnchoredTransformComponent>(text);
	text_transform = transform;
	auto& text_component = ecs.emplace<TextComponent>(text);
	text_component.text = choice_text;

	auto button = ecs.create();
	auto& button_component = ecs.emplace<ButtonComponent>(button);
	button_component.on_click = choice_made;
	auto& button_transform = ecs.emplace<AnchoredTransformComponent>(button);
	button_transform = transform;

	choice_buttons.push_back(ChoiceButton{button, text, jump_index});
}


void DialogVisitor::operator()(const DialogLine& line) {
	ecs.get<TextComponent>(dialog_text).text = line.line;
	index++;
}

void DialogVisitor::operator()(const DialogChoice& choice) {
	ecs.get<TextComponent>(dialog_text).text = "";

	const DialogChoice* current_choice = &choice;
	u16 current_index = index + 1;
	while (true) {
		make_choice_button(current_choice->line, current_index);

		if (current_choice->next_choice == 0) {
			break;
		}

		const DialogChoice& next_choice = std::get<DialogChoice>(dialog[current_choice->next_choice - 1]);
		current_index = current_choice->next_choice + 1;
		current_choice = &next_choice;
	}

	index++;
}

void DialogVisitor::operator()(const DialogCheck& check) {
	proceed = true;

	if (!check.check) {
		index = check.failure_pointer;
		return;
	}

	if (check.check()) {
		index++;
	} else {
		index = check.failure_pointer;
	}
}

void DialogVisitor::operator()(const DialogFunction& function) {
	if (function.function) function.function();
	index++;
	proceed = true;
}

void DialogVisitor::operator()(const DialogJump& jump) {
	index = jump.index;
	proceed = true;
}
