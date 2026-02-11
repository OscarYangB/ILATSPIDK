#pragma once

#include "dialog_structures.h"
#include "entt/entt.hpp"

void start_dialog(const Dialog& new_dialog);
void progress_dialog();
void end_dialog();

void update_dialog_input();
bool in_dialog();

struct DialogVisitor {
	u16 index = 0;
	bool proceed = false;

	void operator()(const DialogLine&);
	void operator()(const DialogChoice&);
	void operator()(const DialogCheck&);
	void operator()(const DialogFunction&);
	void operator()(const DialogJump&);
};

struct ChoiceButton {
	entt::entity button = entt::null;
	entt::entity text = entt::null;
	u16 jump_index = 0;
};
