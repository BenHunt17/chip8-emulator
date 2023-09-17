#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>

#include "cpu.h"

#include "Peripherals.h"

uint8_t initialiseSDL(SDL_Window* window, SDL_Renderer** renderer, SDL_Texture** texture, int pixelScaleFactor);

void render(Chip8State* state, SDL_Renderer* renderer, SDL_Texture* texture);

void PeripheralsLaunch(Chip8State* state, int pixelScaleFactor) {
	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;
	SDL_Texture *texture = NULL;
	if (!initialiseSDL(&window, &renderer, &texture, pixelScaleFactor)) {
		printf("Could not initialise SDL. Program unable to run");
		return;
	}

	uint8_t isRunning = 1;
	SDL_Event e;

	char title[TITLE_LENGTH];

	int clockSpeed = DEFAULT_CLOCK_SPEED;

	int cycles = 0;

	while (isRunning) {
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) {
				isRunning = 0;
			}
			else if (e.type == SDL_KEYDOWN) {
				switch (e.key.keysym.sym) {
					case SDLK_LEFT:
						clockSpeed = max(clockSpeed - 100, MIN_CLOCK_SPEED);
						break;
					case SDLK_RIGHT:
						clockSpeed = min(clockSpeed + 100, MAX_CLOCK_SPEED);
						break;
					case SDLK_x:
						state->keys[0x0] = 0x1;
						break;
					case SDLK_1:
						state->keys[0x1] = 0x1;
						break;
					case SDLK_2:
						state->keys[0x2] = 0x1;
						break;
					case SDLK_3:
						state->keys[0x3] = 0x1;
						break;
					case SDLK_q:
						state->keys[0x4] = 0x1;
						break;
					case SDLK_w:
						state->keys[0x5] = 0x1;
						break;
					case SDLK_e:
						state->keys[0x6] = 0x1;
						break;
					case SDLK_a:
						state->keys[0x7] = 0x1;
						break;
					case SDLK_s:
						state->keys[0x8] = 0x1;
						break;
					case SDLK_d:
						state->keys[0x9] = 0x1;
						break;
					case SDLK_z:
						state->keys[0xA] = 0x1;
						break;
					case SDLK_c:
						state->keys[0xB] = 0x1;
						break;
					case SDLK_4:
						state->keys[0xC] = 0x1;
						break;
					case SDLK_r:
						state->keys[0xD] = 0x1;
						break;
					case SDLK_f:
						state->keys[0xE] = 0x1;
						break;
					case SDLK_v:
						state->keys[0xF] = 0x1;
						break;
				}
			}
			if (e.type == SDL_KEYUP) {
				switch (e.key.keysym.sym) {
					case SDLK_x:
						state->keys[0x0] = 0x0;
						break;
					case SDLK_1:
						state->keys[0x1] = 0x0;
						break;
					case SDLK_2:
						state->keys[0x2] = 0x0;
						break;
					case SDLK_3:
						state->keys[0x3] = 0x0;
						break;
					case SDLK_q:
						state->keys[0x4] = 0x0;
						break;
					case SDLK_w:
						state->keys[0x5] = 0x0;
						break;
					case SDLK_e:
						state->keys[0x6] = 0x0;
						break;
					case SDLK_a:
						state->keys[0x7] = 0x0;
						break;
					case SDLK_s:
						state->keys[0x8] = 0x0;
						break;
					case SDLK_d:
						state->keys[0x9] = 0x0;
						break;
					case SDLK_z:
						state->keys[0xA] = 0x0;
						break;
					case SDLK_c:
						state->keys[0xB] = 0x0;
						break;
					case SDLK_4:
						state->keys[0xC] = 0x0;
						break;
					case SDLK_r:
						state->keys[0xD] = 0x0;
						break;
					case SDLK_f:
						state->keys[0xE] = 0x0;
						break;
					case SDLK_v:
						state->keys[0xF] = 0x0;
						break;
					}
			}
		}

		cpuCycle(state);

		if (cycles > (clockSpeed / 60)) {
			if (state->delayTimer > 0) {
				state->delayTimer--;
			}
			if (state->soundTimer > 0) {
				state->soundTimer--;
			}
			cycles = 0;
		}

		snprintf(title, TITLE_LENGTH, "Chip - 8 Emulator | %d Hz", clockSpeed);
		SDL_SetWindowTitle(window, title);

		render(state, renderer, texture);

		cycles++;

		SDL_Delay(1000 / clockSpeed);
	}

	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyTexture(texture);
	SDL_Quit();
}

uint8_t initialiseSDL(SDL_Window **window, SDL_Renderer **renderer, SDL_Texture **texture, int pixelScaleFactor) {
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		printf("SDL failed to initialise: %s\n", SDL_GetError());
		return 0;
	}

	*window = SDL_CreateWindow("Chip-8 Emulator | 3000 Hz", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH * pixelScaleFactor, SCREEN_HEIGHT * pixelScaleFactor, SDL_WINDOW_SHOWN|
		SDL_WINDOW_OPENGL |
		SDL_WINDOW_RESIZABLE |
		SDL_WINDOW_ALLOW_HIGHDPI);
	if (*window == NULL) {
		printf("Window failed to be created: %s\n", SDL_GetError());
		return 0;
	}

	*renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
	if (*renderer == NULL) {
		printf("Renderer failed to be created: %s\n", SDL_GetError());
		return 0;
	}

	*texture = SDL_CreateTexture(*renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT);
	if (*texture == NULL) {
		printf("Texture failed to be created: %s\n", SDL_GetError());
		return 0;
	}

	SDL_SetRenderDrawColor(*renderer, 0, 0, 0, 0);
	SDL_RenderClear(*renderer);
	SDL_RenderPresent(*renderer);

	return 1;
}

void render(Chip8State* state, SDL_Renderer *renderer, SDL_Texture *texture) {
	uint32_t buffer[SCREEN_WIDTH * SCREEN_HEIGHT];
	for (int y = 0; y < SCREEN_HEIGHT; y++) {
		for (int x = 0; x < SCREEN_WIDTH; x++) {
			buffer[y * SCREEN_WIDTH + x] = state->gfx[y * SCREEN_WIDTH + x] ? 0xFFFFFFFF : 0x000000FF;
		}
	}

	SDL_UpdateTexture(texture, NULL, buffer, SCREEN_WIDTH * sizeof(uint32_t));
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}