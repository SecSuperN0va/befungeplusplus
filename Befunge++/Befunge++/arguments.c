#include "arguments.h"
#include <stdio.h>
#include <stdlib.h>

void InitialiseArgumentStruct(PARGUMENTS args) {
	args->args = (char**)calloc(N_AVAILABLE_ARGS, sizeof(char*));
	return;
}

bool ProcessArguments(PARGUMENTS args, int argc, char** argv) {
	int counter = 0;

	for (counter = 1; counter < argc; counter += 2) {
		// Check for '-' at start of switch
		if (argv[counter][0] != '-') {
			fprintf(stderr, "Invalid switch indicator. Switch should start with '-'. switch: %s\n", argv[counter]);
			return false;
		}
		// Switch on the character
		switch (argv[counter][1]) {
		case 'i':
			// Argument is a file path
			args->args[ARG_PROGRAM_FILE] = argv[counter + 1];
			break;
		case 'd':
			// Argument is tick delay milliseconds
			args->args[ARG_TICK_DELAY] = argv[counter + 1];
			break;
		case 'o':
			// Argument is output file path
			args->args[ARG_OUTPUT_FILE] = argv[counter + 1];
			break;
		case 'v':
			args->args[ARG_TOGGLE_OUTPUT] = (char*)1;
			counter--;
			break;
		default:
			fprintf(stderr, "No valid argument expected for '%s'\n", argv[counter]);
			return false;
		}
	}

	return true;
}