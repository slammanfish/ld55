#ifndef _GAMESTATE_H
#define _GAMESTATE_H

#include "game/core.h"
#include <time.h>

typedef struct {
	quad_t play_button;
	quad_t title;
} menu_t;

typedef enum {
	GUESS_NONE,
	GUESS_GUILTY,
	GUESS_INNOCENT
} guess_t;

typedef enum {
	GAME_STAGE_SPAWN,
	GAME_STAGE_WALK,
	GAME_STAGE_STAND,
	GAME_STAGE_LEAVE,
	GAME_STAGE_END
} game_stage_e;

typedef enum {
	EVIDENCE_NONE,
	EVIDENCE_HAIR,
	EVIDENCE_JEWELRY,
	EVIDENCE_JEWELRY_HAIR
} evidence_e;

typedef enum {
	EV_EMPTY_ACC,
	EV_FULL_ACC,
	EV_FINGERPRINTS_MATCH,
	EV_FINGERPRINTS_DIFFERENT,
	EV_CCTV_HAIR,
	EV_CCTV_BALD
} paper_evidence_e;

typedef struct {
	quad_t quad;

	evidence_e evidence;
	paper_evidence_e paper;
} criminal_t;

typedef struct {
	criminal_t criminal;

	quad_t guilty_button;
	quad_t innocent_button;

	quad_t evidence;
	quad_t paper;

	game_stage_e stage;
	guess_t guess;
	
	int score;
	int ones, tens, hundreds;

	quad_t ones_quad, tens_quad, hundreds_quad;

	unsigned int evidence_closed;
	unsigned int evidence_empty_bank;
	unsigned int evidence_full_bank;
	unsigned int evidence_fingerprints_match;
	unsigned int evidence_fingerprints_different;
	unsigned int evidence_cctv_hair;
	unsigned int evidence_cctv_bald;
	unsigned int criminal_default;
	unsigned int criminal_hair;
	unsigned int criminal_drip;
	unsigned int criminal_drip_hair;
} game_t;

void menu_init(state_t* state, menu_t* menu);
void menu_update(state_t* state, menu_t* menu);
void menu_render(state_t* state, menu_t* menu);
void menu_free(state_t* state, menu_t* menu);

bool is_correct(game_t* game);

void increase_score(game_t* game);

void game_init(state_t* state, game_t* game);
void game_update(state_t* state, game_t* game);
void game_render(state_t* state, game_t* game);
void game_free(state_t* state, game_t* game);

#endif
