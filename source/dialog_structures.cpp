#include "dialog_structures.h"

u16 DialogVisitor::operator()(const DialogLine&) {
	// Do stuff
	return ++index;
}

u16 DialogVisitor::operator()(const DialogChoice&) {
	// Do stuff
	return ++index;
}

u16 DialogVisitor::operator()(const DialogCheck& check) {
	if (!check.check) return check.failure_pointer;
	return check.check() ? ++index : check.failure_pointer;
}

u16 DialogVisitor::operator()(const DialogFunction& function) {
	if (function.function) function.function();
	return ++index;
}

u16 DialogVisitor::operator()(const DialogJump& jump) {
	return jump.index;
}
