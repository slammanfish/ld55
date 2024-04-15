#include "game/state.h"

void menu_init(state_t* state, menu_t* menu) {
	ALLOC(menu);

	menu->play_button = (quad_t) {
		{ 340.0, 120.0 }, { 310.0, 210.0 },
		{ 1.0, 1.0, 1.0, 1.0 }
	};

	texture_init(&menu->play_button.texture, ".res/textures/play.png");

	menu->title = (quad_t) {
		{ 620.0, 130.0 }, { 170.0, 30.0 },
		{ 1.0, 1.0, 1.0, 1.0 }
	};

	texture_init(&menu->title.texture, ".res/textures/alleged.png");
}

void menu_update(state_t* state, menu_t* menu) {
	if (quad_hovered_on(state, &menu->play_button)) {
		quad_col(&menu->play_button, (vec4) { 0.5, 0.5, 0.5, 1.0 });

		if (lmb(state)) {
			state->type = STATE_GAME;
		}
	} else {
		quad_col(&menu->play_button, (vec4) { 1.0, 1.0, 1.0, 1.0 });
	}
}

void menu_render(state_t* state, menu_t* menu) {
	quad_draw(state, &menu->play_button);
	quad_draw(state, &menu->title);
}

void menu_free(state_t* state, menu_t* menu) {
	FREE(menu);
}
