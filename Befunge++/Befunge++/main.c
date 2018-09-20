#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#include "control.h"
#include "arguments.h"
#include "meta.h"
#include "functions.h"
#include "befunge_error.h"
#include "Commands.h"

#ifdef _WIN32
	#include <Windows.h>
	#include "SyscallWindows.h"
	#define SLEEP(s) Sleep(s)
#define SHOW_SYSCALLS GetAndShowWindowsSyscallTable
	
#else
	#include<unistd.h>
	#define SLEEP(s) usleep(1000 * s)
	#define SHOW_SYSCALLS() return 0
#endif

void clrscr()
{
	system("@cls||clear");
}

char* strHelpMessage = 
" --------------------------\n"
"|< v\"WELCOME TO BEFUNGE++\"0|\n"
"|  >       >:#, _@         |\n"
" ---------------------------\n"
"Usage: %s -i <source file> [OPTIONS]\n"
"\tOPTIONS:\n"
"\t\t-d <tick_delay>\t\tSpecify a number of milliseconds to wait between ticks during execution.\n"
"\t\t-v\t\t\tUsed to run the specified program in 'visualisation mode'. (used for debugging and showing off)\n";


void printHelp(char* progName) {
	fprintf(stderr, strHelpMessage, progName);
	return;
}

int StartController(PBEFUNGE_CONTROL control) {
	int tickStatus = STATUS_OK;
	int instanceCounter = 0;
	PINSTANCE_LIST listEntry;
	PFUNGE_INSTANCE instance;
	char command = '\0';

	while (HasActiveInstances(control)) {
		listEntry = control->firstInstance;
		while (listEntry != NULL) {
			if (listEntry->pInstance == NULL) {
				listEntry = listEntry->next;
				continue;
			}
			instance = listEntry->pInstance;
			if (instance->staticSettings->showState) {
				clrscr();
				fprintf(stderr, "INSTANCE %d\n", instanceCounter);
				PrintProgramState(instance);
				fprintf(stderr, "\n");
				PrintOutputState(instance);
				fprintf(stderr, "\n");
				fprintf(stderr, "\n");
				OutputRegistersState(instance);
				fprintf(stderr, "\n");
				fprintf(stderr, "\n");
				PrintStackState(instance);
				fprintf(stderr, "\n");
			}

			if (instance->staticSettings->singleStepMode) {
				fprintf(stdout, "Waiting for step command (any key) ...\n");
				command = getc(stdin);
				switch (command) {
				default:
					break;
				}
			}

			tickStatus = ProcessTick(instance);

			if (tickStatus == STATUS_TERMINATED) {
				RegisterInstanceTermination(instance);
			}
			else if (tickStatus == STATUS_OK) {
				// Increment the necessary internal tracking values
				TakeStep(instance);

				// Apply tick delay if necessary
				if (instance->staticSettings->tickDelay) {
					SLEEP(instance->staticSettings->tickDelay);
				}

			}
			else {
				ERROR_MESSAGE("WTF??? I didn't even make any other error codes!");
			}

			// Move to the next instance
			listEntry = listEntry->next;			
		}
	}

	return -1;
}


void runProgram(char* programString, PFUNCTION_LIST functions, int tickDelay, FILE* outputFile, bool showState, PBEFUNGE_METADATA metadata, bool singleStep) {
	BEFUNGE_CONTROL controlSystem;

	RunPopulation();
	InitialiseControlSystem(&controlSystem, programString, functions, tickDelay, outputFile, showState, metadata, singleStep);

	StartController(&controlSystem);

	return;
}


int main(int argc, char* argv[]) {
	int returnCode = 0;
	FILE * fp = NULL;
	FILE * ofp = NULL;
	size_t programFileSize = 0;
	int tickDelay = 0;
	int err = 0;

	PBEFUNGE_METADATA metadata;
	PFUNCTION_LIST functions;

	char* programString = { 0 };

	if (argc == 1) {
		// TODO: Accept user input for programString?
		printHelp(argv[0]);
		returnCode = -1;
	}
	else if (argc > 1) {
		ARGUMENTS args;
		InitialiseArgumentStruct(&args);
		if (ProcessArguments(&args, argc, argv)) {
			if (args.args[ARG_TICK_DELAY] != NULL) {
				tickDelay = atoi(args.args[ARG_TICK_DELAY]);
				// fprintf(stderr, "Accepted tickDelay of %d\n", tickDelay);
			}
			else {
				// fprintf(stderr, "No tick delay specified (-d <delay_ms>)\n");
			}
			if (args.args[ARG_PROGRAM_FILE] != NULL) {
				// Open handle to the file
				if ((fp = fopen(args.args[ARG_PROGRAM_FILE], "r")) != NULL) {
					// Read any config from the top of the file
					metadata = LoadProgramMetadata(fp);
					functions = LoadAllFunctionDefinitions(fp);
					
					// Identify the remaining file size so we can allocate a buffer of the correct size
					size_t origPos = ftell(fp);
					fseek(fp, 0L, SEEK_END);
					programFileSize = ftell(fp) - origPos;
					fseek(fp, origPos, SEEK_SET);

					// Allocate the necessary buffer
					programString = (char*)calloc(1, programFileSize * sizeof(char) + 1);
					fread(programString, 1, programFileSize, fp);

					fclose(fp);
				}
				else {
					fprintf(stderr, "Error opening file! %s\n", args.args[ARG_PROGRAM_FILE]);
					return -1;
				}

			}
			else {
				// Load default metadata
				metadata = LoadProgramMetadata(fp);
				functions = LoadAllFunctionDefinitions(fp);

				//Specify a default program string
				programString = "<              vv  ,,,,,\"Hello\"<>48*,          vv,,,,,,\"World!\"<>25*,@\0";
			}

			if (args.args[ARG_OUTPUT_FILE] != NULL) {
				//Open a handle to the file
				if ((fp = fopen(args.args[ARG_OUTPUT_FILE], "w")) != 0) {
					ERROR_MESSAGE("Opened output file for writing...");
				}
				else {
					fprintf(stderr, "Error opening file! %s\n", args.args[ARG_OUTPUT_FILE]);
					ofp = NULL;
				}
			}

			// Start the interpretter with the necessary args
			runProgram(programString, functions, tickDelay, ofp, (bool) args.args[ARG_TOGGLE_OUTPUT], metadata, args.args[ARG_SINGLE_STEP]);
			if (ofp != NULL)
				fclose(ofp);
		}
		else {
			ERROR_MESSAGE("Argument processing failed!");
			printHelp(argv[0]);
			returnCode = -3;
		}
	}
	else {
		// Invalid args
		ERROR_MESSAGE("Invalid arguments!");
		printHelp(argv[0]);
		returnCode = -1;
	}

	return returnCode;
}