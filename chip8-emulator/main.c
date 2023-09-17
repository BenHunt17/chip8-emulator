#include <stdio.h>
#include <stdlib.h>

#include "chip8.h"

int main(int argc, char* argv[]) {
	if (argc != 2) {
		printf("Expected 1 argument for the program path\n");
		return 1;
	}

	FILE* file;
	fopen_s(&file, argv[1], "rb");
	if (file == NULL) {
		printf("File was not succesfully opened\n");
		return 1;
	}

	fseek(file, 0, SEEK_END);
	int fileSize = ftell(file);
	fseek(file, 0, SEEK_SET);

	uint8_t* program = malloc(fileSize);
	if (program == NULL) {
		printf("Program buffer was not successfully allocated\n");
		return 1;
	}

	fread(program, fileSize, 1, file);

	fclose(file);

	chip8Start(program, fileSize);

	free(program);

	return 0;
}

