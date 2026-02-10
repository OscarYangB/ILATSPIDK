#pragma once
#include "dialog_structures.h"

enum class Speaker {
	WIZARD,
	LADYBUG,
	NIELON,
	KERRY,
};

void give_flower();
bool has_flower();

constexpr Dialog SAMPLE_DIALOG[] {
/* 001 */	DialogLine { "Do you know where to find the orb?", Speaker::WIZARD },
/* 002 */	DialogChoice { "Orange: Yes!", 5 },
/* 003 */	DialogJump { 0 },
/* 004 */	DialogJump { 29 },
/* 005 */	DialogChoice { "Orange: No", 0 },
/* 006 */	DialogLine { "Where is it?", Speaker::WIZARD },
/* 007 */	DialogChoice { "Orange: It's off the coast of Kamlin?", 22 },
/* 008 */	DialogLine { "And where is that?", Speaker::WIZARD },
/* 009 */	DialogChoice { "Orange: Huh?", 12 },
/* 010 */	DialogLine { "Don't be smart with me, dolt!", Speaker::WIZARD },
/* 011 */	DialogJump { 14 },
/* 012 */	DialogChoice { "Orange: I don't know what you're talking about!", 0 },
/* 013 */	DialogLine { "You will!", Speaker::WIZARD },
/* 014 */	DialogLine { "Shut up!", Speaker::LADYBUG },
/* 015 */	DialogLine { "That's no way to talk to me.", Speaker::WIZARD },
/* 016 */	DialogChoice { "Ladybug: Shut up!!", 19 },
/* 017 */	DialogJump { 0 },
/* 018 */	DialogJump { 21 },
/* 019 */	DialogChoice { "Orange: Haha!", 0 },
/* 020 */	DialogJump { 1 },
/* 021 */	DialogJump { 29 },
/* 022 */	DialogChoice { "Orange: It's behind you?", 26 },
/* 023 */	DialogLine { "Wrong!", Speaker::WIZARD },
/* 024 */	DialogJump { 0 },
/* 025 */	DialogJump { 29 },
/* 026 */	DialogChoice { "Orange: It's in the museum of modern political science!", 0 },
/* 027 */	DialogLine { "Yes!", Speaker::WIZARD },
/* 028 */	DialogJump { 0 },
};

constexpr Dialog SAMPLE_DIALOGUE[] {
/* 001 */	DialogLine { "Ah, there we are. Can you hear me?", Speaker::NIELON },
/* 002 */	DialogChoice { "Kerry: Yes, I can", 5 },
/* 003 */	DialogLine { "Excellent! Let's proceed.", Speaker::NIELON },
/* 004 */	DialogJump { 10 },
/* 005 */	DialogChoice { "Kerry: No, I can't", 0 },
/* 006 */	DialogLine { "Oh boy that's no good. Let me try something...", Speaker::NIELON },
/* 007 */	DialogLine { "Alright.", Speaker::KERRY },
/* 008 */	DialogLine { "Okay now can you hear me?", Speaker::NIELON },
/* 009 */	DialogJump { 2 },
/* 010 */	DialogLine { "Do you have that flower in your inventory?", Speaker::NIELON },
/* 011 */	DialogCheck { has_flower, 15 },
/* 012 */	DialogLine { "Yes", Speaker::KERRY },
/* 013 */	DialogJump { 0 },
/* 014 */	DialogJump { 18 },
/* 015 */	DialogChoice { "", 0 },
/* 016 */	DialogLine { "No", Speaker::KERRY },
/* 017 */	DialogFunction { give_flower },
/* 018 */	DialogLine { "Chungus.", Speaker::NIELON },
};

