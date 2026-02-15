#include "dialog.h"
#include "game.h"
#include "render.h"
#include "input.h"
#include "button.h"

entt::entity background = entt::null;
entt::entity dialog_text = entt::null;
const Dialog* dialog = nullptr;
DialogVisitor visitor {};

constexpr double DIALOG_ANIMATION_RATE = 50.0;
constexpr double DIALOG_ANIMATION_DELTA = 1.0 / DIALOG_ANIMATION_RATE;
constexpr double PUNCTUATION_PAUSE = 0.18;
double dialog_animation_timer = DIALOG_ANIMATION_DELTA;
bool is_dialog_animating = false;

std::vector<ChoiceButton> choice_buttons = {};

constexpr float WIDTH = 800.f;
constexpr float HEIGHT = 400.f;
constexpr float X_MARGIN = 50.f;
constexpr float Y_MARGIN = 50.f;
constexpr float CHOICE_BUTTON_HEIGHT = 100.f;

void start_dialog(const Dialog& new_dialog) {
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
			return;
		}
	} while (visitor.proceed);
}

void end_dialog() {
	ecs.destroy(background);
	ecs.destroy(dialog_text);
	background = entt::null;
	dialog_text = entt::null;
}

static void update_dialog_input() {
	if (visitor.index == 0) {
		return;
	}

	if (choice_buttons.size() > 0) {
		return;
	}

	if (is_dialog_animating) {
		return;
	}

	if (input_down_this_frame(InputType::INTERACT)) {
		handle_input(InputType::INTERACT);
		progress_dialog();
	}
}

static void update_dialog_animation() {
	if (dialog_text == entt::null) {
		return;
	}

	auto& text_component = ecs.get<TextComponent>(dialog_text);

	if (text_component.mask == 0) {
		return;
	}

	dialog_animation_timer -= delta_time;

	if (dialog_animation_timer > 0.0) {
		return;
	}

	// Play sound from tone row based on chance

	text_component.mask++;
	if (text_component.mask >= strlen(text_component.text)) {
		text_component.mask = 0;
		is_dialog_animating = false;
		return;
	}
	char last_character = text_component.text[text_component.mask - 1];
	bool punctuation = last_character == '.' || last_character == '?' || last_character == '!';
	dialog_animation_timer += punctuation ? PUNCTUATION_PAUSE : DIALOG_ANIMATION_DELTA;
}

void update_dialog() {
	update_dialog_input();
	update_dialog_animation();
}

void delete_choice_buttons() {
	for (const ChoiceButton& button : choice_buttons) {
		ecs.destroy(button.entity);
	}

	choice_buttons.clear();
}

void choice_made() {
	for (const ChoiceButton& button : choice_buttons) {
		const ButtonComponent& button_component = ecs.get<ButtonComponent>(button.entity);
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

	auto entity = ecs.create();
	auto& text_transform = ecs.emplace<AnchoredTransformComponent>(entity);
	text_transform = transform;
	auto& text_component = ecs.emplace<TextComponent>(entity);
	text_component.text = choice_text;
	auto& button_component = ecs.emplace<ButtonComponent>(entity);
	button_component.on_click = choice_made;

	choice_buttons.push_back(ChoiceButton{entity, jump_index});
}


void DialogVisitor::operator()(const DialogLine& line) {
	TextComponent& text = ecs.get<TextComponent>(dialog_text);
	text.text = line.line;
	index++;

	text.mask = 1;
	dialog_animation_timer = DIALOG_ANIMATION_DELTA;
	is_dialog_animating = true;
}

void DialogVisitor::operator()(const DialogChoice& choice) {
	ecs.get<TextComponent>(dialog_text).text = "";

	const DialogChoice* current_choice = &choice;
	u16 current_index = index + 1;
	while (true) {
		if (current_choice->check == nullptr || current_choice->check()) {
			make_choice_button(current_choice->line, current_index);
		}

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
