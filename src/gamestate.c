#include "game/state.h"

bool is_correct(game_t* game) {
	if (game->criminal.paper == EV_EMPTY_ACC) {
		if (game->criminal.evidence == EVIDENCE_JEWELRY || game->criminal.evidence == EVIDENCE_JEWELRY_HAIR) {
			return game->guess == GUESS_GUILTY;
		} else {
			return game->guess == GUESS_INNOCENT;
		}
	}

	if (game->criminal.paper == EV_FULL_ACC) {
		return game->guess == GUESS_INNOCENT;		
	}

	if (game->criminal.paper == EV_FINGERPRINTS_MATCH) {
		return game->guess == GUESS_GUILTY;
	}

	if (game->criminal.paper == EV_FINGERPRINTS_DIFFERENT) {
		return game->guess == GUESS_INNOCENT;
	}

	if (game->criminal.paper == EV_CCTV_BALD) {
		if (game->criminal.evidence == EVIDENCE_NONE || game->criminal.evidence == EVIDENCE_JEWELRY) {
			return game->guess == GUESS_GUILTY;
		} else {
			return game->guess == GUESS_INNOCENT;
		}
	}

	if (game->criminal.paper == EV_CCTV_HAIR) {
		if (game->criminal.evidence == EVIDENCE_HAIR || game->criminal.evidence == EVIDENCE_JEWELRY_HAIR) {
			return game->guess == GUESS_GUILTY;
		} else {
			return game->guess == GUESS_INNOCENT;
		}
	}

	return false;
}

void increase_score(game_t* game) {
	game->score++;
	
	game->ones++;
	
	if (game->ones > 9) {
		game->ones = 0;
		game->tens++;
	}

	if (game->tens > 9) {
		game->tens = 0;
		game->hundreds++;
	}
}

void game_init(state_t* state, game_t* game) {
	ALLOC(game);

	srand(time(NULL));

	game->paper = (quad_t) {
		{ 80.0, 60.0 }, { 120.0, 200.0 },
		{ 1.0, 1.0, 1.0, 1.0 }
	};

	texture_init(&game->paper.texture, ".res/textures/paper.png");

	game->evidence = (quad_t) {
		{ 960.0, 540.0 }, { 0.0, 0.0 },
		{ 1.0, 1.0, 1.0, 1.0 }
	};

	texture_init(&game->evidence_closed, ".res/textures/evidence_closed.png");
	texture_init(&game->evidence_empty_bank, ".res/textures/empty_bank_account.png");
	texture_init(&game->evidence_full_bank, ".res/textures/full_bank_account.png");
	texture_init(&game->evidence_fingerprints_match, ".res/textures/fingerprints_match.png");
	texture_init(&game->evidence_fingerprints_different, ".res/textures/fingerprints_different.png");
	texture_init(&game->evidence_cctv_hair, ".res/textures/cctv_hair.png");
	texture_init(&game->evidence_cctv_bald, ".res/textures/cctv_bald.png");

	game->evidence.texture = game->evidence_closed;

	game->criminal.quad = (quad_t) {
		{ 80.0, 140.0 }, { state->width / 2.0 - 40, -140.0 },
		{ 1.0, 1.0, 1.0, 1.0 }
	};

	texture_init(&game->criminal_default, ".res/textures/criminal.png");
	texture_init(&game->criminal_drip, ".res/textures/criminal_drip.png");
	texture_init(&game->criminal_hair, ".res/textures/criminal_hair.png");
	texture_init(&game->criminal_drip_hair, ".res/textures/criminal_drip_hair.png");

	game->guilty_button = (quad_t) {
		{ 50.0, 70.0 }, { 340.0, 250.0 },
		{ 1.0, 1.0, 1.0, 1.0 }
	};

	texture_init(&game->guilty_button.texture, ".res/textures/guilty_button.png");

	game->innocent_button = (quad_t) {
		{ 50.0, 70.0 }, { 570.0, 250.0 },
		{ 1.0, 1.0, 1.0, 1.0 }
	};

	texture_init(&game->innocent_button.texture, ".res/textures/innocent_button.png");

	game->stage = GAME_STAGE_SPAWN;
	game->guess = GUESS_NONE;
	game->score = 0;
	game->ones = 0;
	game->tens = 0;
	game->hundreds = 0;

	game->ones_quad = (quad_t) {
		{ 30.0, 50.0 }, { 90.0, 10.0 },
		{ 1.0, 1.0, 1.0, 1.0 }
	};

	game->tens_quad = (quad_t) {
		{ 30.0, 50.0 }, { 50.0, 10.0 },
		{ 1.0, 1.0, 1.0, 1.0 }
	};

	game->hundreds_quad = (quad_t) {
		{ 30.0, 50.0 }, { 10.0, 10.0 },
		{ 1.0, 1.0, 1.0, 1.0 }
	};
}

void game_update(state_t* state, game_t* game) {
	switch (game->stage) {
		case GAME_STAGE_SPAWN:	
			quad_pos(&game->criminal, (vec2) { state->width / 2.0 - 40, -70.0 });
			
			game->criminal.evidence = randi(0, 3); 

			if (game->criminal.evidence == EVIDENCE_NONE) {
				game->criminal.quad.texture = game->criminal_default;
			} else if (game->criminal.evidence == EVIDENCE_HAIR) {
				game->criminal.quad.texture = game->criminal_hair;
			} else if (game->criminal.evidence == EVIDENCE_JEWELRY) {
				game->criminal.quad.texture = game->criminal_drip;
			} else if (game->criminal.evidence == EVIDENCE_JEWELRY_HAIR) {
				game->criminal.quad.texture = game->criminal_drip_hair;
			}

			if (game->criminal.evidence == EVIDENCE_JEWELRY || game->criminal.evidence == EVIDENCE_JEWELRY_HAIR) {
				game->criminal.paper = randi(0, 5);
			} else {
				game->criminal.paper = randi(2, 5);
			}


			game->guess = GUESS_NONE;
			game->stage = GAME_STAGE_WALK;
			break;
		case GAME_STAGE_WALK:
			game->criminal.quad.pos[1] += 200.0 * state->time.delta;

			if (game->criminal.quad.pos[1] >= 220.0) {
				game->criminal.quad.pos[1] = 220.0;
				game->stage = GAME_STAGE_STAND;
			}
			break;
		case GAME_STAGE_STAND:
			// display evidence

			if (quad_hovered_on(state, &game->paper)) {
				if (game->criminal.paper == EV_EMPTY_ACC) {
					game->evidence.texture = game->evidence_empty_bank;
				} else if (game->criminal.paper == EV_FULL_ACC) {
					game->evidence.texture = game->evidence_full_bank;
				} else if (game->criminal.paper == EV_FINGERPRINTS_MATCH) {
					game->evidence.texture = game->evidence_fingerprints_match;
				} else if (game->criminal.paper == EV_FINGERPRINTS_DIFFERENT) {
					game->evidence.texture = game->evidence_fingerprints_different;
				} else if (game->criminal.paper == EV_CCTV_BALD) {
					game->evidence.texture = game->evidence_cctv_bald;
				} else if (game->criminal.paper == EV_CCTV_HAIR) {
					game->evidence.texture = game->evidence_cctv_hair;
				}
			} else {
				game->evidence.texture = game->evidence_closed;
			}

			if (quad_hovered_on(state, &game->guilty_button)) {
				quad_col(&game->guilty_button, (vec4) { 0.5, 0.5, 0.5, 1.0 });

				if (lmb(state)) {
					game->guess = GUESS_GUILTY;
					game->stage = GAME_STAGE_LEAVE;
					Mix_PlayChannel(-1, state->guilty_sound, 0);
				}
			} else {
				quad_col(&game->guilty_button, (vec4) { 1.0, 1.0, 1.0, 1.0 });
			}

			if (quad_hovered_on(state, &game->innocent_button)) {
				quad_col(&game->innocent_button, (vec4) { 0.5, 0.5, 0.5, 1.0 });

				if (lmb(state)) {
					game->guess = GUESS_INNOCENT;
					game->stage = GAME_STAGE_LEAVE;
					Mix_PlayChannel(-1, state->innocent_sound, 0);
				}
			} else {
				quad_col(&game->innocent_button, (vec4) { 1.0, 1.0, 1.0, 1.0 });
			}
			break;
		case GAME_STAGE_LEAVE:
			if (game->guess == GUESS_GUILTY) {
				game->criminal.quad.pos[0] -= 400 * state->time.delta;

				if (game->criminal.quad.pos[0] <= -40.0) {
					game->criminal.quad.pos[0] = -40.0;
					game->stage = GAME_STAGE_END;
				}
			}

			if (game->guess == GUESS_INNOCENT) {
				game->criminal.quad.pos[0] += 400 * state->time.delta;

				if (game->criminal.quad.pos[0] >= 920.0) {
					game->criminal.quad.pos[0] = 920.0;
					game->stage = GAME_STAGE_END;
				}
			}
			break;
		case GAME_STAGE_END:
			if (is_correct(game)) {
				game->stage = GAME_STAGE_SPAWN;
				increase_score(game);
			} else {
				quad_pos(&game->criminal.quad, (vec2) { state->width / 2.0 - 40, -70.0 });
				game->stage = GAME_STAGE_SPAWN;
				game->guess = GUESS_NONE;
				game->score = 0;
				game->ones = 0;
				game->tens = 0;
				game->hundreds = 0;
				state->type = STATE_MENU;
			}

			break;
	}
}

void game_render(state_t* state, game_t* game) {
	quad_draw(state, &game->paper);
	quad_draw(state, &game->criminal.quad);

	if (game->stage == GAME_STAGE_STAND) {
		quad_draw(state, &game->guilty_button);
		quad_draw(state, &game->innocent_button);
	}

	quad_draw(state, &game->evidence);

	number_draw(state, &game->ones_quad, game->ones);
	number_draw(state, &game->tens_quad, game->tens);
	number_draw(state, &game->hundreds_quad, game->hundreds);
}

void game_free(state_t* state, game_t* game) {
	FREE(game);
}
