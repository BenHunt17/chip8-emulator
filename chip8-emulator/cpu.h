#pragma once

#include "chip8.h"

#include <stdint.h>

#define CLS      0x00E0
#define RET      0x00EE
#define JP       0x1000
#define CALL     0x2000
#define SEB      0x3000
#define SNEB     0x4000
#define SER      0x5000
#define LDB      0x6000
#define ADDB     0x7000
#define LDR      0x8000
#define OR       0x8001
#define AND      0x8002
#define XOR      0x8003
#define ADDR     0x8004
#define SUBR     0x8005
#define SHR      0x8006
#define SUBN     0x8007
#define SHL      0x800E
#define SNER     0x9000
#define LDI      0xA000
#define JPRB     0xB000
#define RND      0xC000
#define DRAW     0xD000
#define SKD      0xE09E
#define SKR      0xE0A1
#define LDTR     0xF015
#define LDK      0xF00A
#define LDRT     0xF007
#define LDST     0xF018
#define ADDI     0xF01E
#define LDF      0xF029
#define LDBC     0xF033
#define LDRM     0xF055
#define LDMR     0xF065

void cpuCycle(Chip8State* state);