#include "chip8.h"

#define DEFAULT_CLOCK_SPEED 500
#define MAX_CLOCK_SPEED 3000
#define MIN_CLOCK_SPEED 100

#define TITLE_LENGTH 28

void PeripheralsLaunch(Chip8State *state, int pixelScaleFactor);