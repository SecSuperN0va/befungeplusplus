#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>

#include "control.h"
#include "arguments.h"
#include "meta.h"

#ifdef _WIN32
#include <Windows.h>
#define SLEEP(s) Sleep(s)
#else
#include<unistd.h>
#define SLEEP(s) usleep(1000 * s)
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


int tick(PBEFUNGE_CONTROL control) {
	char command = '\0';

	//char a, b = '\0';
	unsigned long int a, b = 0;
	int v = 0;

	char tempBuffer[32] = { 0 };

	bool stringMode = false;

	while (command != '@') {
		a = b = 0;
		memset(tempBuffer, 0, sizeof(tempBuffer));
		if (control->showState) {
			clrscr();
			PrintProgramState(control);
			fprintf(stderr, "\n");
			PrintOutputState(control);
			fprintf(stderr, "\n");
			fprintf(stderr, "\n");
			PrintStackState(control);
			fprintf(stderr, "\n");
		}
		
		// Get the command character
		command = GetCommand(control);

		if (stringMode && command != '\"') {
			// We are in string mode and the character's ascii value should be pushed to the stack.
			Push(control, command);
		}
		else {
			if (0x30 <= command && command <= 0x39) {
				// integer value needs pushing to the stack.
				Push(control, command - 0x30);
			}
			else {
				switch (command) {
				case '+':
					// Pop a and b then push a+b
					Pop(control, &a);
					Pop(control, &b);
					Push(control, a + b);
					break;
				case '-':
					// Pop a and b then push b-a
					Pop(control, &a);
					Pop(control, &b);
					Push(control, b - a);
					break;
				case '*':
					// Pop a and b then push a*b
					Pop(control, &a);
					Pop(control, &b);
					Push(control, a * b);
					break;
				case '/':
					// Pop a and b then push b/a rounded towards 0
					Pop(control, &a);
					Pop(control, &b);
					Push(control, b / a);
					break;
				case '%':
					// Pop a and b then push the remainder of the integer division of b/a
					Pop(control, &a);
					Pop(control, &b);
					Push(control, b % a);
					break;
				case '!':
					// Pop a value. If the value is Zero push 1 else push Zero.
					Pop(control, &a);
					if (a == 0) {
						Push(control, 1);
					}
					else {
						Push(control, 0);
					}
					break;
				case '`':
					// Pop a and b then push 1 if b > a else Zero
					Pop(control, &a);
					Pop(control, &b);

					if (b > a) {
						Push(control, 1);
					}
					else {
						Push(control, 0);
					}
					break;
				case '>':
					// Start moving right
					SetDirection(control, DIR_RIGHT);
					break;
				case '<':
					// Start moving left
					SetDirection(control, DIR_LEFT);
					break;
				case '^':
					// Start moving up
					SetDirection(control, DIR_UP);
					break;
				case 'v':
					// Start moving down
					SetDirection(control, DIR_DOWN);
					break;
				case '?':
					// Start moving in a random cardinal direction
					srand((unsigned)time(0));
					a = rand() % 4;	// random number between 0 and the number of directions available (4)
					SetDirection(control, a);
					break;
				case '_':
					// Pop a value, move right if value == 0 left otherwise
					Pop(control, &a);
					if (a == 0) {
						SetDirection(control, DIR_RIGHT);
					}
					else {
						SetDirection(control, DIR_LEFT);
					}
					break;
				case '|':
					// Pop a value move down in value == 0 up otherwise
					Pop(control, &a);
					if (a == 0) {
						SetDirection(control, DIR_DOWN);
					}
					else {
						SetDirection(control, DIR_UP);
					}
					break;
				case '"':
					// Start String mode, push eash character's ASCII value all the way up to the next "
					stringMode = !stringMode;
					break;
				case ':':
					// Duplicate value on top of stack
					Pop(control, &a);
					Push(control, a);
					Push(control, a);
					break;
				case '\\':
					// Swap two values on top of the stack
					Pop(control, &a);
					Pop(control, &b);
					Push(control, a);
					Push(control, b);
					break;
				case '$':
					// Pop a value from the stack and discard it
					Pop(control, NULL);
					break;
				case '.':
					// Pop a value and output as an integer followed by a space
					Pop(control, &a);
					snprintf(tempBuffer, sizeof(tempBuffer), "%d ", (unsigned int) a);
					OutputString(control, tempBuffer, strlen(tempBuffer));
					break;
				case ',':
					// Pop a value and output as ASCII character
					Pop(control, &a);
					snprintf(tempBuffer, sizeof(tempBuffer), "%c", a);
					OutputString(control, tempBuffer, 1);
					break;
				case '#':
					// Bridge: Skip next call
					TakeStep(control);
					break;
				case 'p':
					// A "put" call: Pop y, x, v, then change the character at (x,y) in the program to the character with ASCII value v
					Put(control);
					break;
				case 'g':
					// A "get" call: Pop y and x then push ASCII value of the character at that position in the program
					Get(control);
					break;
				case '&':
					// Ask user for a number and push its actual value
					//fprintf(stderr, "Please enter a single digit number: ");
					a = fgetc(stdin);
					// Flush newline character
					fgetc(stdin);
					while (a < 0x30 || a > 0x39) {
						fprintf(stderr, "Invalid Number! Please enter a single digit number: ");
						a = fgetc(stdin);
						// Flush newline character
						fgetc(stdin);
					}
					Push(control, a - 0x30);
					break;
				case '~':
					// Ask user for a character and push its ASCII value
					//fprintf(stderr, "Please enter an ASCII character: ");
					a = fgetc(stdin);
					// Flush newline character
					fgetc(stdin);
					while (a < 0x20 || a > 0x7F) {
						fprintf(stderr, "Invalid Character! Please enter an ASCII character: ");
						a = fgetc(stdin);
						// Flush newline character
						fgetc(stdin);
					}
					Push(control, a);
					break;
				case '@':
					// End program
					break;
				case ' ':
					// NOP
					break;
				default:
					if (control->showState) fprintf(stderr, "Invalid opcode: %c\n", command);
				}
			}
		}

		// Increment the necessary internal tracking values
		TakeStep(control);

		// Apply tick delay if necessary
		if (control->tickDelay) {
			SLEEP(control->tickDelay);
		}
	}

	return -1;
}

void runProgram(char* programString, int tickDelay, FILE* outputFile, bool showState, PBEFUNGE_METADATA metadata) {
	BEFUNGE_CONTROL controlSystem;

	// Hard code grid size for now
	// TODO: Change this
	InitialiseControlSystem(&controlSystem, programString, tickDelay, outputFile, showState, metadata);

	while (tick(&controlSystem) != -1) {

	}


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
					
					// Identify the remaining file size so we can allocate a buffer of the correct size
					size_t origPos = ftell(fp);
					fseek(fp, 0L, SEEK_END);
					programFileSize = ftell(fp) - origPos;
					fseek(fp, origPos, SEEK_SET);

					// Allocate the necessary buffer
					programString = (char*)calloc(1, programFileSize * sizeof(char));
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

				//Specify a default program string
				programString = "<              vv  ,,,,,\"Hello\"<>48*,          vv,,,,,,\"World!\"<>25*,@";
			}

			if (args.args[ARG_OUTPUT_FILE] != NULL) {
				//Open a handle to the file
				if ((fp = fopen(args.args[ARG_OUTPUT_FILE], "w")) != 0) {
					fprintf(stderr, "Opened output file for writing...\n");
				}
				else {
					fprintf(stderr, "Error opening file! %s\n", args.args[ARG_OUTPUT_FILE]);
					ofp = NULL;
				}
			}

			// Start the interpretter with the necessary args
			runProgram(programString, tickDelay, ofp, (bool) args.args[ARG_TOGGLE_OUTPUT], metadata);
			if (ofp != NULL)
				fclose(ofp);
		}
		else {
			fprintf(stderr, "Argument processing failed!\n");
			printHelp(argv[0]);
			returnCode = -3;
		}
	}
	else {
		// Invalid args
		fprintf(stderr, "Invalid arguments!");
		printHelp(argv[0]);
		returnCode = -1;
	}

	return returnCode;
}