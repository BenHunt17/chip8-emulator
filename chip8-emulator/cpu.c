#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "cpu.h"

void cpuCycle(Chip8State* state) {
	//fetch
	uint16_t opcode = (state->memory[state->pc] << 8) | state->memory[state->pc + 1];
	state->pc += 2;

	uint8_t overflow = 0x0;
	uint8_t keyIsPressed = 0x0;

	//decode + execute
	switch (opcode & 0xF000) {
		case 0x0000:
			switch (opcode) {
				case CLS:
					//Clear screen
					memset(state->gfx, 0, SCREEN_WIDTH * SCREEN_HEIGHT);
					break;
				case RET:
					//Return from subroutine
					state->sp--;
					state->pc = state->stack[state->sp]; 
			}
			break;
		case JP:
			//Jump to instruction at address
			state->pc = opcode & 0x0FFF;
			break;
		case CALL:
			//Call a subroutine at address
			state->stack[state->sp] = state->pc;
			state->sp++;
			state->pc = opcode & 0x0FFF;
			break;
		case SEB:
			//Skip next instruction if register x equals value
			if (state->registers[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)) {
				state->pc += 2;
			}
			break;
		case SNEB:
			//Skip next instruction if register x doesn't equal value
			if (state->registers[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)) {
				state->pc += 2;
			}
			break;
		case SER:
			//Skip next instruction if register x equals register y
			if (state->registers[(opcode & 0x0F00) >> 8] == state->registers[(opcode & 0x00F0) >> 4]) {
				state->pc += 2;
			}
			break;
		case LDB:
			//Load value into register x
			state->registers[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
			break;
		case ADDB:
			//Add value to register x and store in register x
			state->registers[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
			break;
		case 0x8000:
			switch (opcode & 0xF00F) {
				case LDR:
					//Load register y into register x
					state->registers[(opcode & 0x0F00) >> 8] = state->registers[(opcode & 0x00F0) >> 4];
					break;
				case OR:
					//Perform bitwise OR on registers x and y then store result in register x
					state->registers[(opcode & 0x0F00) >> 8] |= state->registers[(opcode & 0x00F0) >> 4];
					state->registers[0xF] = 0x0;
					break;
				case AND:
					//Perform bitwise AND on registers x and y then store result in register x
					state->registers[(opcode & 0x0F00) >> 8] &= state->registers[(opcode & 0x00F0) >> 4];
					state->registers[0xF] = 0x0;
					break;
				case XOR:
					//Perform bitwise XOR on registers x and y then store result in register x
					state->registers[(opcode & 0x0F00) >> 8] ^= state->registers[(opcode & 0x00F0) >> 4];
					state->registers[0xF] = 0x0;
					break;
				case ADDR:
					//Adds register x to y. If result overflows then set last register to 1. else set to 0. then store lowest 8 bits of the result in register x
					if (state->registers[(opcode & 0x0F00) >> 8] + state->registers[(opcode & 0x00F0) >> 4] > 0xFF) {
						overflow = 0x1;
					}
					state->registers[(opcode & 0x0F00) >> 8] += state->registers[(opcode & 0x00F0) >> 4];
					state->registers[(opcode & 0x0F00) >> 8] &= 0xFF;

					if (overflow) {
						state->registers[0xF] = 0x1;
					}
					else
					{
						state->registers[0xF] = 0x0;
					}
					break;
				case SUBR:
					//register y is subtracted from register x then stored in register x. If x > y then set last register to 1 (not borrow)
					if (state->registers[(opcode & 0x0F00) >> 8] > state->registers[(opcode & 0x00F0) >> 4]) {
						overflow = 0x1;
					}
					state->registers[(opcode & 0x0F00) >> 8] -= state->registers[(opcode & 0x00F0) >> 4];

					if (overflow) {
						state->registers[0xF] = 0x1;
					}
					else
					{
						state->registers[0xF] = 0x0;
					}
					break;
				case SHR:
					//Shift bits in register x right by one then set last register to 1 if a bit was lost.
					overflow = state->registers[(opcode & 0x0F00) >> 8] & 0x1;
					state->registers[(opcode & 0x0F00) >> 8] >>= 0x1;

					state->registers[0xF] = overflow;
					break;
				case SUBN:
					//register x is subtracted form register y then stored in register x. If y > x then set last register to 1 (not borrow)
					if (state->registers[(opcode & 0x00F0) >> 4] > state->registers[(opcode & 0x0F00) >> 8]) {
						overflow = 0x1;
					}
					state->registers[(opcode & 0x0F00) >> 8] = state->registers[(opcode & 0x00F0) >> 4] - state->registers[(opcode & 0x0F00) >> 8];

					if (overflow) {
						state->registers[0xF] = 0x1;
					}
					else
					{
						state->registers[0xF] = 0x0;
					}
					break;
				case SHL:
					//Shift bits in register x left by one then set last register to 1 if a bit was lost.
					overflow = state->registers[(opcode & 0x0F00) >> 8] >> 7;
					state->registers[(opcode & 0x0F00) >> 8] <<= 0x1;

					state->registers[0xF] = overflow;
					break;
			}
			break;
		case SNER:
			//Skip next instruction if register x is not equal to register y
			if (state->registers[(opcode & 0x0F00) >> 8] != state->registers[(opcode & 0x00F0) >> 4]) {
				state->pc += 2;
			}
			break;
		case LDI:
			//Load address into register I
			state->i = opcode & 0x0FFF;
			break;
		case JPRB:
			//Add value of register 0 and a literal and jump to the memory addressed by the result
			state->pc = (opcode & 0x0FFF) + state->registers[0];
			break;
		case RND:
			//Generate a random number between 0 and 255 then AND with a literal before storing in register x
			state->registers[(opcode & 0x0F00) >> 8] = ((rand() % 256) & (opcode & 0x00FF));
			break;
		case DRAW:
			//Display an n bit sprite starting for address register I at screen pos register x, register y. register f is set to 1 if there is a collision
			state->registers[0xF] = 0x0;
			for (int row = 0; row < (opcode & 0x000F); row++) {
				uint8_t pixelRow = state->memory[state->i + row];
				for (int column = 0; column < 8; column++) {
					uint16_t screenIndex =
						((state->registers[(opcode & 0x00F0) >> 4] + row)) * SCREEN_WIDTH +
						((state->registers[(opcode & 0x0F00) >> 8] + column)); //Note - taking MOD of each coordinate since sprites should wrap if they exit the bounds
					uint8_t pixel = (pixelRow >> (7 - column)) & 0x1;
					if (pixel == 0x1) {
						if ((state->gfx[screenIndex] ^ pixel) == 0x0) {
							state->registers[0xF] = 0x1;
						}
						state->gfx[screenIndex] ^= pixel;
					}
				}
			}
			break;
		case 0xE000:
			switch (opcode & 0xF0FF) {
				case SKD:
					//Skip next instruction if key x is pressed
					if (state->keys[state->registers[(opcode & 0x0F00) >> 8]] == 0x1) {
						state->pc += 2;
					}
					break;
				case SKR:
					//Skip next instruction of key x is not pressed
					if (state->keys[state->registers[(opcode & 0x0F00) >> 8]] == 0x0) {
						state->pc += 2;
					}
					break;
			}
			break;
		case 0xF000:
			switch (opcode & 0xF0FF) {
				case LDRT:
					//Set register x to value of delay timer
					state->registers[(opcode & 0x0F00) >> 8] = state->delayTimer;
					break;
				case LDK:
					//Wait for a keypress then store the key value into register x. Stop further execution until key is pressed
					state->pc -= 2;
					for (uint8_t i = 0; i < KEY_COUNT; i++) {
						if (state->keys[i]) {
							state->keyWasDown = 0x1;
							state->registers[(opcode & 0x0F00) >> 8] = i;
							keyIsPressed = 0x1;
							break;
						}
					}

					if (state->keyWasDown && !keyIsPressed) {
						state->keyWasDown = 0x0;
						state->pc += 2;
					}

					break;
				case LDTR:
					//Set the delay timer to the value of register x
					state->delayTimer = state->registers[(opcode & 0x0F00) >> 8];
					break;
				case LDST:
					//Set the sound timer to the value of register x
					state->soundTimer = state->registers[(opcode & 0x0F00) >> 8];
					break;
				case ADDI:
					//Add the value of register x to the index register
					state->i += state->registers[(opcode & 0x0F00) >> 8];
					break;
				case LDF:
					//Set the index register to the location of a sprite which index is stored in register x
					state->i = state->registers[(opcode & 0x0F00) >> 8] * CHARACTER_SPRITE_SIZE + FONT_SET_INDEX;
					break;
				case LDBC:
					//Store binary coded decimal representation of value at register x in memory with addresses I, I+1 and I+2
					state->memory[state->i] = (int)(floor(((float)(state->registers[(opcode & 0x0F00) >> 8])) / 100)) % 10;
					state->memory[state->i + 1] = (int)(floor(((float)(state->registers[(opcode & 0x0F00) >> 8])) / 10)) % 10;
					state->memory[state->i + 2] = state->registers[(opcode & 0x0F00) >> 8] % 10;
					break;
				case LDRM:
					//Stores registers 0 through x into memory starting at location I
					for (uint8_t i = 0; i <= ((opcode & 0x0F00) >> 8); i++) {
						state->memory[state->i + i] = state->registers[i];
					}
					state->i += (((opcode & 0x0F00) >> 8) + 1);
					break;
				case LDMR:
					//Read values into registers 0 through x from memory starting at location I
					for (uint8_t i = 0; i <= ((opcode & 0x0F00) >> 8); i++) {
						state->registers[i] = state->memory[state->i + i];
					}
					state->i += (((opcode & 0x0F00) >> 8) + 1);
					break;
			}
			break;
	}
}