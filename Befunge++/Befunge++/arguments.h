#pragma once
#include <stdbool.h>

#define ARG_PROGRAM_FILE 0
#define ARG_TICK_DELAY 1
#define ARG_OUTPUT_FILE 2
#define ARG_TOGGLE_OUTPUT 3
#define ARG_SINGLE_STEP 4

// The following needs to be the same as the number of arguments defined above this line
#define N_AVAILABLE_ARGS 5

typedef struct Arguments {
	char** args;
} ARGUMENTS, *PARGUMENTS;

void InitialiseArgumentStruct(PARGUMENTS args);

bool ProcessArguments(PARGUMENTS args, int argc, char** argv);
