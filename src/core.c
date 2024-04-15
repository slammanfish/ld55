#include "game/core.h"

float quad_vertices[] = {
	 1.0, 1.0, 0.0, 1.0, 1.0,  // top right
     1.0, 0.0, 0.0, 1.0, 0.0,  // bottom right
	 0.0, 0.0, 0.0, 0.0, 0.0,  // bottom left
     0.0, 1.0, 0.0, 0.0, 1.0   // top left 
};

unsigned int quad_indices[] = { 
    0, 1, 3, 
    1, 2, 3  
};

const char* vertex_shader_code = 
	"#version 460 core\n"
	"layout(location = 0) in vec3 a_pos;\n"
	"layout(location = 1) in vec2 a_uv;\n"
	"uniform mat4 model;\n"
	"uniform mat4 proj;\n"
	"out vec2 uv;\n"
	"void main() {\n"
	"gl_Position = proj * model * vec4(a_pos, 1.0);\n"
	"uv = a_uv;\n"
	"}\n\0";

const char* fragment_shader_code = 
	"#version 460 core\n"
	"out vec4 frag_colour;\n"
	"in vec2 uv;\n"
	"uniform sampler2D img;\n"
	"uniform vec4 col;\n"
	"void main() {\n"
	"vec4 final = col * texture(img, uv);\n"
	"if (final.a < 0.1) {\n"
	"discard;\n"
	"}\n"
	"frag_colour = final;\n"
	"}\n\0";

int randi(int x, int y) {
	return rand() % (y - x + 1) + x;
}

void state_init(state_t* state, const char* title) {
	ALLOC(state);	

	// init window
	
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    
    state->quit = false;

	state->width = 960;
	state->height = 540;

	state->resized = false;

	state->aspect_ratio = (float) state->width / (float) state->height;

	state->type = STATE_MENU;

    state->window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        state->width,
        state->height,
        SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | 
		SDL_WINDOW_ALLOW_HIGHDPI
    );

	state->gl = SDL_GL_CreateContext(state->window);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	gladLoadGLLoader(SDL_GL_GetProcAddress);

    printf("GL Vender: %s\n", glGetString(GL_VENDOR));
    printf("GL Renderer: %s\n", glGetString(GL_RENDERER));
    printf("GL Version: %s\n", glGetString(GL_VERSION));
    printf("GL Shading Language: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    printf("----------------------------------\n");

	SDL_RaiseWindow(state->window);

	// init shader
	
	unsigned int vs, fs;

	vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vertex_shader_code, NULL);
	glCompileShader(vs);

	fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fragment_shader_code, NULL);
	glCompileShader(fs);

	state->shader = glCreateProgram();
	glAttachShader(state->shader, vs);
	glAttachShader(state->shader, fs);
	glLinkProgram(state->shader);

	glDeleteShader(vs);
	glDeleteShader(fs);
	
	// init buffer

	unsigned int vbo, ebo;
	
	glGenVertexArrays(1, &state->buffer);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(state->buffer);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quad_indices), quad_indices, GL_STATIC_DRAW);	
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (sizeof(float) * 3));	

	glBindVertexArray(0);

	texture_init(&state->_0, ".res/font/0.png");
	texture_init(&state->_1, ".res/font/1.png");
	texture_init(&state->_2, ".res/font/2.png");
	texture_init(&state->_3, ".res/font/3.png");
	texture_init(&state->_4, ".res/font/4.png");
	texture_init(&state->_5, ".res/font/5.png");
	texture_init(&state->_6, ".res/font/6.png");
	texture_init(&state->_7, ".res/font/7.png");
	texture_init(&state->_8, ".res/font/8.png");
	texture_init(&state->_9, ".res/font/9.png");

	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

	state->music = Mix_LoadMUS(".res/sound/music.wav");
	state->innocent_sound = Mix_LoadWAV(".res/sound/innocent.wav");
	state->guilty_sound = Mix_LoadWAV(".res/sound/guilty.wav");

	state->time.start_time = SDL_GetPerformanceCounter();
    state->time.current_time = state->time.start_time;
    state->time.last_time = 0.0;
    state->time.delta = 0.0;
    state->time.elapsed = 0.0;

	glm_ivec2_zero(state->mouse.pos);
}

bool lmb(state_t* state) {
	return state->mouse.state & SDL_BUTTON(1);
}

bool rmb(state_t* state) {
	return state->mouse.state & SDL_BUTTON(3);
}

void texture_init(unsigned int* texture, const char* path) {
	glGenTextures(1, texture);
    glBindTexture(GL_TEXTURE_2D, *texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//stbi_set_flip_vertically_on_load(true); 

    int w, h, c;
    char* data = stbi_load(path, &w, &h, &c, 0);
    
    if (!data) {
        printf("Failed to load texture: %s\n", path);
        return;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
}

void quad_draw(state_t* state, quad_t* quad) {
	glUseProgram(state->shader);
	
	mat4 transform;
	glm_mat4_identity(transform);

	glm_translate(transform, quad->pos);
	glm_scale(transform, (vec3) { quad->size[0], quad->size[1], 1.0 });

	glUniformMatrix4fv(
		glGetUniformLocation(state->shader, "proj"), 1, 
		GL_FALSE, state->camera.matrix
	);	

	glUniformMatrix4fv(
		glGetUniformLocation(state->shader, "model"), 1, 
		GL_FALSE, transform
	);

	glUniform4f(
		glGetUniformLocation(state->shader, "col"),
		quad->col[0], quad->col[1], quad->col[2], quad->col[3]
	);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, quad->texture);

	glBindVertexArray(state->buffer);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void quad_pos(quad_t* quad, vec2 pos) {
	glm_vec2_copy(pos, quad->pos);
}

void quad_col(quad_t* quad, vec4 col) {
	glm_vec4_copy(col, quad->col);
}

bool quad_hovered_on(state_t* state, quad_t* quad) {
	bool x = state->mouse.pos[0] > quad->pos[0] && state->mouse.pos[0] < quad->pos[0] + quad->size[0];
	bool y = state->mouse.pos[1] > quad->pos[1] && state->mouse.pos[1] < quad->pos[1] + quad->size[1];

	return x && y;
}

void number_draw(state_t* state, quad_t* quad, int num) {
	switch (num) {
		case 0:
			quad->texture = state->_0;
			break;
		case 1:
			quad->texture = state->_1;
			break;
		case 2:
			quad->texture = state->_2;
			break;
		case 3:
			quad->texture = state->_3;
			break;
		case 4:
			quad->texture = state->_4;
			break;
		case 5:
			quad->texture = state->_5;
			break;
		case 6:
			quad->texture = state->_6;
			break;
		case 7:
			quad->texture = state->_7;
			break;
		case 8:
			quad->texture = state->_8;
			break;
		case 9:
			quad->texture = state->_9;
			break;
	}
	quad_draw(state, quad);
}
