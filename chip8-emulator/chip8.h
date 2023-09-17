#pragma once

#include <stdint.h>

#define MEMORY_SIZE 4096
#define FONT_SET_SIZE 80
#define REGISTER_COUNT 16
#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32
#define STACK_SIZE 16
#define KEY_COUNT 16

#define CHARACTER_SPRITE_SIZE 5

#define FONT_SET_INDEX 0x50
#define PROGRAM_INDEX 0x200


typedef struct {
	uint8_t memory[MEMORY_SIZE];
	uint8_t registers[REGISTER_COUNT];
	uint16_t i;
	uint16_t pc;
	uint8_t gfx[SCREEN_WIDTH * SCREEN_HEIGHT];
	uint8_t delayTimer;
	uint8_t soundTimer; //Haven't implemented actual sound as I'm lazy
	uint16_t stack[STACK_SIZE];
	uint8_t sp;
	uint8_t keys[KEY_COUNT];
	uint8_t keyWasDown;
} Chip8State;

void chip8Start(uint8_t* program, int programSize);

void initialiseMemory(uint8_t* memory, uint8_t* program, int programSize);