#ifndef _CORE_H
#define _CORE_H

#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <stdbool.h>
#include <cglm/cglm.h>
#include <stb_image.h>
#include <stdlib.h>

#define ALLOC(ptr) memset(ptr, 0, sizeof(*ptr))
#define FREE(ptr) free(ptr); ptr == NULL

#define PI          3.1415926535897932384626433832795
#define PI_2        (PI / 2.0)
#define PI_3        (PI / 3.0)
#define PI_4        (PI / 4.0)
#define PI_6        (PI / 6.0)
#define TAU         (PI * 2.0)
#define DEG2RAD     (PI / 180.0)

int randi(int x, int y);

typedef enum {
	STATE_MENU,
	STATE_GAME
} state_type_e;

typedef struct {
    SDL_Window* window;
    SDL_GLContext gl;
    SDL_Event event;

	state_type_e type;

	unsigned int width, height;
	float aspect_ratio;
	bool resized;
	
	struct {
		vec2 pos;
		mat4 matrix;
	} camera;
	
	unsigned int buffer;
	unsigned int shader;

	struct {
		double delta;
		double elapsed;

		unsigned long long start_time;
		unsigned long long current_time;
		unsigned long long last_time;
	} time;

	struct {
		ivec2 pos;
		unsigned int state;
	} mouse;

	unsigned int _0, _1, _2, _3, _4, _5, _6, _7, _8, _9;

	Mix_Music* music;
	Mix_Chunk* innocent_sound;
	Mix_Chunk* guilty_sound;

    bool quit;
} state_t;

void state_init(state_t* state, const char* title);

bool lmb(state_t* state);
bool rmb(state_t* state);

void texture_init(unsigned int* texture, const char* path);

typedef struct {
	vec2 size, pos;
	vec4 col;
	unsigned int texture;
} quad_t;

void quad_draw(state_t* state, quad_t* quad);
void quad_pos(quad_t* quad, vec2 pos);
void quad_col(quad_t* quad, vec4 col);
bool quad_hovered_on(state_t* state, quad_t* quad);

void number_draw(state_t* state, quad_t* quad, int num);

#endif
