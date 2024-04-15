#define STB_IMAGE_IMPLEMENTATION
#include "game/core.h"
#include "game/state.h"

void update(state_t* state, menu_t* menu, game_t* game);
void render(state_t* state, menu_t* menu, game_t* game);

quad_t background;

int main(int argc, char* argv[]) {
	state_t state;
	state_init(&state, "Alleged");

	menu_t menu;
	menu_init(&state, &menu);

	game_t game;
	game_init(&state, &game);

	background = (quad_t) {
		{ state.width, state.height }, { 0.0, 0.0 },
		{ 1.0, 1.0, 1.0, 1.0 }
	};
	
	texture_init(&background.texture, ".res/textures/court.png");
        
    while (true) {
		state.resized = false;
        while (SDL_PollEvent(&state.event)) {
            switch (state.event.type) {
                case SDL_QUIT:
                    state.quit = true;
                    break;
				case SDL_WINDOWEVENT:
					if (state.event.window.event == SDL_WINDOWEVENT_RESIZED) {
						state.width = state.event.window.data1;
						state.height = state.event.window.data2;
						state.aspect_ratio = (float) state.width / (float) state.height;
						background.size[0] = state.width;
						background.size[1] = state.height;
						state.resized = true;
					}
					break;
            }
        }

        if (state.quit) {
            break;
        }

        SDL_PumpEvents();

		state.time.last_time = state.time.current_time;
		state.time.current_time = SDL_GetPerformanceCounter();
		state.time.delta = (double) ((state.time.current_time - state.time.last_time) / (double) SDL_GetPerformanceFrequency());
		state.time.elapsed = (double) ((state.time.current_time - state.time.start_time) / (double) SDL_GetPerformanceFrequency());
		
		state.mouse.state = SDL_GetMouseState(&state.mouse.pos[0], &state.mouse.pos[1]);

		glViewport(0, 0, state.width, state.height);
		glm_ortho(0, state.width, state.height, 0, -1.0, 1.0, state.camera.matrix);

		glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

		if (!Mix_PlayingMusic()) {
			Mix_PlayMusic(state.music, -1);
		}
		
		update(&state, &menu, &game);
		render(&state, &menu, &game);

        SDL_GL_SwapWindow(state.window);
    }

	menu_free(&state, &menu);
	game_free(&state, &game);	

	Mix_Quit();
    SDL_DestroyWindow(state.window);
    SDL_Quit();

	FREE(&state);

    return 0;
}

void update(state_t* state, menu_t* menu, game_t* game) {
	switch (state->type) {
		case STATE_MENU:
			menu_update(state, menu);
			break;
		case STATE_GAME:
			game_update(state, game);
			break;
	}	
}

void render(state_t* state, menu_t* menu, game_t* game) {
	quad_draw(state, &background);

	switch (state->type) {
		case STATE_MENU:
			menu_render(state, menu);
			break;
		case STATE_GAME:
			game_render(state, game);
			break;
	}	
}
