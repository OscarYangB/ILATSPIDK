#include "dialog.h"
#include "game.h"
#include "render.h"
#include "input.h"
#include "button.h"

constexpr double PUNCTUATION_PAUSE = 0.18;

constexpr u16 WIDTH = 800;
constexpr u16 HEIGHT = 400;
constexpr u16 X_MARGIN = 50;
constexpr u16 Y_MARGIN = 50;
constexpr u16 CHOICE_BUTTON_HEIGHT = 100;

void start_dialog(const Dialog& new_dialog) {
	push_input_mode(InputMode::DIALOG);

	auto& dialog = ecs.ctx().emplace<DialogSingleton>();
	dialog.visitor.index = 1;
	dialog.dialog = &new_dialog;

	dialog.background = ecs.create();
	auto& background_transform = ecs.emplace<UITransformComp>(dialog.background, UITransformComp{.x_anchor = XAnchor::CENTER, .y_anchor = YAnchor::BOTTOM,
																								 .width = WIDTH, .height = HEIGHT});
	ecs.emplace<NineSliceComp>(dialog.background, NineSliceComp{.x = 40, .y = 30, .w = 320, .h = 150});
	ecs.emplace<SpriteComp>(dialog.background, SpriteComp{.sprites = {Sprite::TEST_BUTTON}});

	dialog.dialog_text = ecs.create();
	ecs.emplace<UITransformComp>(dialog.dialog_text, UITransformComp{.x_anchor = XAnchor::CENTER, .y_anchor = YAnchor::BOTTOM,
																	 .width = WIDTH - X_MARGIN, .height = HEIGHT - Y_MARGIN, .sort_order = 1});
	ecs.emplace<TextComp>(dialog.dialog_text);
	background_transform.add_child(dialog.background, dialog.dialog_text);

	progress_dialog();
}

DialogSingleton& get_dialog() {
	return ecs.ctx().get<DialogSingleton>();
}

bool in_dialog() {
	return get_dialog().visitor.index != 0;
}

void progress_dialog() {
	DialogSingleton& dialog = get_dialog();
	do {
		dialog.visitor.proceed = false;
		std::visit(dialog.visitor, *(dialog.dialog + dialog.visitor.index - 1));
		if (dialog.visitor.index == 0) {
			end_dialog();
			return;
		}
	} while (dialog.visitor.proceed);
}

void end_dialog() {
	pop_input_mode(InputMode::DIALOG);
	ecs.destroy(get_dialog().background);
	ecs.ctx().erase<DialogSingleton>();
}

static void update_dialog_input() {
	DialogSingleton& dialog = get_dialog();

	if (dialog.visitor.index == 0) {
		return;
	}

	if (!ecs.view<DialogChoiceComp>().empty()) {
		return;
	}

	if (dialog.is_dialog_animating) {
		return;
	}

	if (input_down_this_frame(InputType::INTERACT)) {
		handle_input(InputType::INTERACT);
		progress_dialog();
	}
}

static void update_dialog_animation() {
	DialogSingleton& dialog = get_dialog();

	if (dialog.dialog_text == entt::null) {
		return;
	}

	auto& text_component = ecs.get<TextComp>(dialog.dialog_text);

	if (text_component.mask == 0) {
		return;
	}

	dialog.dialog_animation_timer -= delta_time;

	if (dialog.dialog_animation_timer > 0.0) {
		return;
	}

	// Play sound from tone row based on chance

	text_component.mask++;
	if (text_component.mask >= text_component.text.get().length()) { // Won't work for localization
		text_component.mask = 0;
		dialog.is_dialog_animating = false;
		return;
	}
	char last_character = text_component.text.get()[text_component.mask - 1]; // Won't work for localization
	bool punctuation = last_character == '.' || last_character == '?' || last_character == '!';
	dialog.dialog_animation_timer += punctuation ? PUNCTUATION_PAUSE : DIALOG_ANIMATION_DELTA;
}

void update_dialog() {
	update_dialog_input();
	update_dialog_animation();
}

void delete_choice_buttons() {
	auto view = ecs.view<DialogChoiceComp>();
	ecs.destroy(view.begin(), view.end());
}

void choice_made(entt::entity entity) {
	auto& choice = ecs.get<DialogChoiceComp>(entity);
	get_dialog().visitor.index = choice.jump_index;
	delete_choice_buttons();
	progress_dialog();
}

void make_choice_button(const Text& choice_text, u16 jump_index, u8 choice_index) {
	DialogSingleton& dialog = get_dialog();

	auto entity = ecs.create();
	ecs.emplace<UITransformComp>(entity, UITransformComp{.x_anchor = XAnchor::CENTER, .y_anchor = YAnchor::BOTTOM,
														 .relative_position = { X_MARGIN, -1.f - CHOICE_BUTTON_HEIGHT * choice_index},
														 .width = WIDTH, .height = CHOICE_BUTTON_HEIGHT});
	ecs.get<UITransformComp>(dialog.background).add_child(dialog.background, entity);
	ecs.emplace<TextComp>(entity, TextComp{.text = choice_text});
	ecs.emplace<ButtonComp>(entity, ButtonComp{.on_click = choice_made});
	ecs.emplace<DialogChoiceComp>(entity, DialogChoiceComp{.jump_index = jump_index});
}

void DialogVisitor::operator()(const DialogLine& line) {
	DialogSingleton& dialog = get_dialog();

	TextComp& text = ecs.get<TextComp>(dialog.dialog_text);
	text.text = line.line;
	index++;

	text.mask = 1;
	dialog.dialog_animation_timer = DIALOG_ANIMATION_DELTA;
	dialog.is_dialog_animating = true;
}

void DialogVisitor::operator()(const DialogChoice& choice) {
	DialogSingleton& dialog = get_dialog();

	ecs.get<TextComp>(dialog.dialog_text).text = Text{};

	const DialogChoice* current_choice = &choice;
	u16 current_index = index + 1;
	for (int i = 0; ; i++) {
		if (current_choice->check == nullptr || current_choice->check()) {
			make_choice_button(current_choice->line, current_index, i);
		}

		if (current_choice->next_choice == 0) {
			break;
		}

		const DialogChoice& next_choice = std::get<DialogChoice>(dialog.dialog[current_choice->next_choice - 1]);
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
