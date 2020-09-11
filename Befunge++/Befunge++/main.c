#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#include "control.h"
#include "arguments.h"
#include "meta.h"
#include "befunge_error.h"
#include "Commands.h"
#include "page_manager.h"

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

#define DISPLAY_STATE(pControl){ \
	fprintf(stderr, "Current Page: %s\n", pControl->pManager->pCurrentPageControl->name); \
	PrintProgramState(pControl); \
	fprintf(stderr, "\n"); \
	PrintOutputState(pControl); \
	fprintf(stderr, "\n"); \
	fprintf(stderr, "\n"); \
	OutputRegistersState(pControl); \
	fprintf(stderr, "\n"); \
	fprintf(stderr, "\n"); \
	PrintStackState(pControl); \
	fprintf(stderr, "\n"); \
}

#define ACCEPT_DEBUG_COMMAND(){ \
	command = getc(stdin); \
	switch (command) { \
	default: \
		break; \
	} \
}

#define PARAM_EXISTS(_args, paramName) (_args.args[paramName] != NULL)
#define PARAM_GET(_args, paramName) (_args.args[paramName])

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

#define EXIT(progName, rc) {printHelp(progName); return rc;}

int StartPagedController(PBEFUNGE_CORE_CONTROL pControl) {
	int tickStatus = STATUS_OK;
	int instanceCounter = 0;
	char command = '\0';

	while (!pControl->hasTerminated) {
		if (pControl->pConfig->isVisualiseModeActive) {
			clrscr();
			DISPLAY_STATE(pControl);
		}
		if (pControl->pStaticSettings->singleStepMode) {
			fprintf(stdout, "Waiting for step command (any key) ...\n");
			ACCEPT_DEBUG_COMMAND();
		}

		tickStatus = ProcessTick(pControl);

		switch (tickStatus) {
		case STATUS_TERMINATED:
			break;
		case STATUS_OK:
			// Increment the necessary internal tracking values and fall through
			TakeStep(pControl);
		case STATUS_CALLED:
			// Apply tick delay if necessary
			if (pControl->pStaticSettings->tickDelay) {
				SLEEP(pControl->pStaticSettings->tickDelay);
			}
			break;
		default:
			ERROR_MESSAGE("This shouldn't happen. I didn't even make any other error codes!");
			break;
		}
	}

	return -1;
}

void runProgram(PBEFUNGE_PROGRAM_CONFIG pConfig) {
	BEFUNGE_CORE_CONTROL coreControlSystem;

	RunPopulation();

	InitialiseCoreControl(&coreControlSystem, pConfig);

	StartPagedController(&coreControlSystem);
	return;
}


int main(int argc, char* argv[]) {
	int returnCode = 0;
	FILE * fp = NULL;
	size_t programFileSize = 0;
	int err = 0;

	PBEFUNGE_PROGRAM_CONFIG pConfig = (PBEFUNGE_PROGRAM_CONFIG)calloc(1, sizeof(BEFUNGE_PROGRAM_CONFIG));
	pConfig->mode = MODE_STANDARD;

	ARGUMENTS args;

	char* programString = { 0 };

	if (argc < 2) {
		ERROR_MESSAGE("Invalid number of args");
		EXIT(argv[0], ERR_INVALID_ARGS);
	}

	// Process the command line arguments
	if (ProcessArguments(&args, argc, argv)) {
		/* 
		Handle Param: TICK_DELAY
		Command Line: -d <delay_ms>
		*/
		if (PARAM_EXISTS(args, ARG_TICK_DELAY)) {
			pConfig->tickDelay = atoi(PARAM_GET(args, ARG_TICK_DELAY));
		}

		/*
		Handle Param: SINGLE_STEP
		Command Line: -s
		*/
		if (PARAM_GET(args, ARG_SINGLE_STEP)) {
			pConfig->isSingleSteModeActive = true;
		}
		else {
			pConfig->isSingleSteModeActive = false;
		}

		/*
		Handle Param: VISUALISE
		Command Line: -v
		*/
		if (PARAM_GET(args, ARG_TOGGLE_OUTPUT)) {
			pConfig->isVisualiseModeActive = true;
		}
		else {
			pConfig->isVisualiseModeActive = false;
		}

		/*
		Handle Param: OUTPUT_FILE
		Command Line: -o <output_file>
		*/
		if (PARAM_EXISTS(args, ARG_OUTPUT_FILE)) {
			//Open a handle to the file
			if ((pConfig->pOutFile = fopen(PARAM_GET(args, ARG_OUTPUT_FILE), "w")), pConfig->pOutFile == 0) {
				fprintf(stderr, "Error opening output file! %s\n", PARAM_GET(args, ARG_OUTPUT_FILE));
				pConfig->pOutFile = NULL;
			}
		}

		/*
		Handle Param: PROGRAM_FILE
		Command Line: -i <program_file>
		*/
		if (PARAM_EXISTS(args, ARG_PROGRAM_FILE)) {
			// Open handle to the file
			if ((fp = fopen(PARAM_GET(args, ARG_PROGRAM_FILE), "rb")) != NULL) {
				// Read any config from the top of the file
				pConfig->pMetadata = LoadProgramMetadata(fp);
				
				// Attempt Load all pages from file
				size_t firstPageOffset = GetNextPageStartOffset(fp);
				if (firstPageOffset != -1) {
					fseek(fp, firstPageOffset, SEEK_SET);
					pConfig->pPages = LoadAllPages(fp);
					pConfig->mode = MODE_PAGED;
				}
				else {
					/*fprintf(stderr, "No Pages found in file! %s\n", args.args[ARG_PROGRAM_FILE]);
					return ERR_INVALID_PAGING;*/

					// Identify the remaining file size so we can allocate a buffer of the correct size
					size_t origPos = ftell(fp);
					fseek(fp, 0L, SEEK_END);
					programFileSize = ftell(fp) - origPos;
					fseek(fp, origPos, SEEK_SET);

					// Allocate the necessary buffer
					pConfig->lpProgramString = (char*)calloc(1, programFileSize * sizeof(char) + 1);
					fread(pConfig->lpProgramString, 1, programFileSize, fp);
					pConfig->mode = MODE_STANDARD;
				}

				fclose(fp);
			}
			else {
				fprintf(stderr, "Error opening file! %s\n", args.args[ARG_PROGRAM_FILE]);
				return ERR_INVALID_PROGRAM_FILE;
			}

		}

		/*
		Run the Program with all the necessary parameters
		*/

		// Start the interpretter with the necessary args
		runProgram(pConfig);
		//_runProgram(programString, NULL, tickDelay, ofp, (bool)PARAM_GET(args, ARG_TOGGLE_OUTPUT), metadata, PARAM_GET(args, ARG_SINGLE_STEP));
		if (pConfig->pOutFile != NULL)
			fclose(pConfig->pOutFile);
			
	}
	else {
		ERROR_MESSAGE("Argument processing failed!");
		EXIT(argv[0], ERR_ARG_PROCESSING_FAILED);
	}

	return returnCode;
}