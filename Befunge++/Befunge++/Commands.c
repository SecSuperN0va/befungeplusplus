#include "Commands.h"
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "Syscalls.h"
#include "befunge_error.h"
#include "functions.h"

# define POP(dest) Pop(instance, dest) 
# define PUSH(src) Push(instance, src)
# define STEP() TakeStep(instance)
# define BACKSTEP() BackStep(instance)
# define SETDIRECTION(dir) SetDirection(instance, dir)

int commandCharLookup[MAX_ORD] = { CMD_NOP };
bool(*commandTable[MAX_ORD])(PBEFUNGE_instance) = { FUNC_NOP };


void PopulateCommandCharLookup() {
	commandCharLookup[ORD_ADD] = CMD_ADD;
	commandCharLookup[ORD_SUB] = CMD_SUB;
	commandCharLookup[ORD_MUL] = CMD_MUL;
	commandCharLookup[ORD_DIV] = CMD_DIV;
	commandCharLookup[ORD_MOD] = CMD_MOD;
	commandCharLookup[ORD_NOT] = CMD_NOT;
	commandCharLookup[ORD_GREATER] = CMD_GREATER;
	commandCharLookup[ORD_RIGHT] = CMD_RIGHT;
	commandCharLookup[ORD_LEFT] = CMD_LEFT;
	commandCharLookup[ORD_UP] = CMD_UP;
	commandCharLookup[ORD_DOWN] = CMD_DOWN;
	commandCharLookup[ORD_RAND] = CMD_RAND;
	commandCharLookup[ORD_HORIZ_CHECK] = CMD_HORIZ_CHECK;
	commandCharLookup[ORD_VERT_CHECK] = CMD_VERT_CHECK;
	commandCharLookup[ORD_STRING] = CMD_STRING;
	commandCharLookup[ORD_DUP] = CMD_DUP;
	commandCharLookup[ORD_SWAP] = CMD_SWAP;
	commandCharLookup[ORD_DISCARD] = CMD_DISCARD;
	commandCharLookup[ORD_INT_OUT] = CMD_INT_OUT;
	commandCharLookup[ORD_ASCII_OUT] = CMD_ASCII_OUT;
	commandCharLookup[ORD_BRIDGE] = CMD_BRIDGE;
	commandCharLookup[ORD_PUT] = CMD_PUT;
	commandCharLookup[ORD_GET] = CMD_GET;
	commandCharLookup[ORD_INT_IN] = CMD_INT_IN;
	commandCharLookup[ORD_ASCII_IN] = CMD_ASCII_IN;
	commandCharLookup[ORD_END] = CMD_END;

	// Extended Functionality
	commandCharLookup[ORD_LOAD_REG] = CMD_LOAD_REG;
	commandCharLookup[ORD_GET_REG] = CMD_GET_REG;
	commandCharLookup[ORD_SYSCALL] = CMD_SYSCALL;
	commandCharLookup[ORD_OSI] = CMD_OSI;
	commandCharLookup[ORD_FORK] = CMD_FORK;
	commandCharLookup[ORD_CALL] = CMD_CALL;
}

void PopulateCommandTable() {
	commandTable[ORD_ADD] = FUNC_ADD;
	commandTable[ORD_SUB] = FUNC_SUB;
	commandTable[ORD_MUL] = FUNC_MUL;
	commandTable[ORD_DIV] = FUNC_DIV;
	commandTable[ORD_MOD] = FUNC_MOD;
	commandTable[ORD_NOT] = FUNC_NOT;
	commandTable[ORD_GREATER] = FUNC_GREATER;
	commandTable[ORD_RIGHT] = FUNC_RIGHT;
	commandTable[ORD_LEFT] = FUNC_LEFT;
	commandTable[ORD_UP] = FUNC_UP;
	commandTable[ORD_DOWN] = FUNC_DOWN;
	commandTable[ORD_RAND] = FUNC_RAND;
	commandTable[ORD_HORIZ_CHECK] = FUNC_HORIZ_CHECK;
	commandTable[ORD_VERT_CHECK] = FUNC_VERT_CHECK;
	commandTable[ORD_STRING] = FUNC_STRING;
	commandTable[ORD_DUP] = FUNC_DUP;
	commandTable[ORD_SWAP] = FUNC_SWAP;
	commandTable[ORD_DISCARD] = FUNC_DISCARD;
	commandTable[ORD_INT_OUT] = FUNC_INT_OUT;
	commandTable[ORD_ASCII_OUT] = FUNC_ASCII_OUT;
	commandTable[ORD_BRIDGE] = FUNC_BRIDGE;
	commandTable[ORD_PUT] = FUNC_PUT;
	commandTable[ORD_GET] = FUNC_GET;
	commandTable[ORD_INT_IN] = FUNC_INT_IN;
	commandTable[ORD_ASCII_IN] = FUNC_ASCII_IN;
	commandTable[ORD_END] = FUNC_END;

	// Extended Functionality
	commandTable[ORD_LOAD_REG] = FUNC_LOAD_REG;
	commandTable[ORD_GET_REG] = FUNC_GET_REG;
	commandTable[ORD_SYSCALL] = FUNC_SYSCALL;
	commandTable[ORD_OSI] = FUNC_OSI;
	commandTable[ORD_FORK] = FUNC_FORK;
	commandTable[ORD_CALL] = FUNC_CALL;
}

bool CmdNop(PFUNGE_INSTANCE instance) {
	STEP();
	while (GetCommand(instance) == CMD_NOP) {
		STEP();
	}
	BACKSTEP();
	return true;
}

bool CmdAdd(PFUNGE_INSTANCE instance) {
	STACK_ITEM_TYPE a, b;
	POP(&a);
	POP(&b);
	PUSH(a + b);
	return true;
}

bool CmdSubtract(PFUNGE_INSTANCE instance) {
	STACK_ITEM_TYPE a, b;
	POP(&a);
	POP(&b);
	PUSH(b - a);
	return true;
}

bool CmdMultiply(PFUNGE_INSTANCE instance) {
	STACK_ITEM_TYPE a, b;
	POP(&a);
	POP(&b);
	PUSH(a * b);
	return true;
}

bool CmdDivide(PFUNGE_INSTANCE instance) {
	STACK_ITEM_TYPE a, b;
	POP(&a);
	POP(&b);
	PUSH(b / a);
	return true;
}

bool CmdModulo(PFUNGE_INSTANCE instance) {
	STACK_ITEM_TYPE a, b;
	POP(&a);
	POP(&b);
	PUSH(b % a);
	return true;
}

bool CmdNot(PFUNGE_INSTANCE instance) {
	STACK_ITEM_TYPE a;
	POP(&a);
	if (a == 0) {
		PUSH((STACK_ITEM_TYPE)1);
	}
	else {
		PUSH((STACK_ITEM_TYPE)0);
	}
	return true;
}

bool CmdGreaterThan(PFUNGE_INSTANCE instance) {
	STACK_ITEM_TYPE a, b;
	POP(&a);
	POP(&b);

	if (b > a) {
		PUSH(1);
	}
	else {
		PUSH(0);
	}
	return true;
}

bool CmdMoveRight(PFUNGE_INSTANCE instance) {
	SETDIRECTION(DIR_RIGHT);
	return true;
}

bool CmdMoveLeft(PFUNGE_INSTANCE instance) {
	SETDIRECTION(DIR_LEFT);
	return true;
}

bool CmdMoveUp(PFUNGE_INSTANCE instance) {
	SETDIRECTION(DIR_UP);
	return true;
}

bool CmdMoveDown(PFUNGE_INSTANCE instance) {
	SETDIRECTION(DIR_DOWN);
	return true;
}

bool CmdMoveRand(PFUNGE_INSTANCE instance) {
	int a;
	srand((unsigned)time(0));
	a = rand() % 4;	// random number between 0 and the number of directions available (4)
	SETDIRECTION(a);
	return true;
}

bool CmdHorizontalNullCheck(PFUNGE_INSTANCE instance) {
	STACK_ITEM_TYPE a;
	POP(&a);
	if (a == 0) {
		SETDIRECTION(DIR_RIGHT);
	}
	else {
		SETDIRECTION(DIR_LEFT);
	}
	return true;
}

bool CmdVerticalNullCheck(PFUNGE_INSTANCE instance) {
	STACK_ITEM_TYPE a;
	POP(&a);
	if (a == 0) {
		SETDIRECTION(DIR_DOWN);
	}
	else {
		SETDIRECTION(DIR_UP);
	}
	return true;
}

bool CmdStringMode(PFUNGE_INSTANCE instance) {
	ToggleStringMode(instance);
	return true;
}

bool CmdDuplicate(PFUNGE_INSTANCE instance) {
	STACK_ITEM_TYPE a;
	POP(&a);
	PUSH(a);
	PUSH(a);
	return true;
}

bool CmdSwap(PFUNGE_INSTANCE instance) {
	STACK_ITEM_TYPE a, b;
	POP(&a);
	POP(&b);
	PUSH(a);
	PUSH(b);
	return true;
}

bool CmdDiscard(PFUNGE_INSTANCE instance) {
	POP(NULL);
	return true;
}

bool CmdIntegerOut(PFUNGE_INSTANCE instance) {
	STACK_ITEM_TYPE a;
	char tempBuffer[20];
	POP(&a);
	snprintf(tempBuffer, sizeof(tempBuffer), "%d ", (unsigned int)a);
	OutputString(instance, tempBuffer, strlen(tempBuffer));
	return true;
}

bool CmdASCIIOut(PFUNGE_INSTANCE instance) {
	STACK_ITEM_TYPE a;
	char tempBuffer[20];
	POP(&a);
	snprintf(tempBuffer, sizeof(tempBuffer), "%c", a);
	OutputString(instance, tempBuffer, 1);
	return true;
}

bool CmdBridge(PFUNGE_INSTANCE instance) {
	STEP();
	return true;
}

bool CmdPut(PFUNGE_INSTANCE instance) {
	Put(instance);
	return true;
}

bool CmdGet(PFUNGE_INSTANCE instance) {
	Get(instance);
	return true;
}

bool CmdIntegerIn(PFUNGE_INSTANCE instance) {
	STACK_ITEM_TYPE a;
	a = fgetc(instance->dynamicSettings->hProgramIn);
	// Flush newline character
	if (instance->dynamicSettings->hProgramIn == stdin)
		fgetc(instance->dynamicSettings->hProgramIn);
	while (a < 0x30 || a > 0x39) {
		fprintf(stderr, "Invalid Number! Please enter a single digit number: ");
		a = fgetc(instance->dynamicSettings->hProgramIn);
		// Flush newline character
		if (instance->dynamicSettings->hProgramIn == stdin)
			fgetc(instance->dynamicSettings->hProgramIn);
	}
	PUSH(a - 0x30);
	return true;
}

bool CmdASCIIIn(PFUNGE_INSTANCE instance) {
	STACK_ITEM_TYPE a;
	a = fgetc(instance->dynamicSettings->hProgramIn);
	// Flush newline character
	if (instance->dynamicSettings->hProgramIn == stdin)
		fgetc(instance->dynamicSettings->hProgramIn);
	while (a < 0x20 || a > 0x7F) {
		fprintf(stderr, "Invalid Character! Please enter an ASCII character: ");
		a = fgetc(instance->dynamicSettings->hProgramIn);
		// Flush newline character
		if (instance->dynamicSettings->hProgramIn == stdin)
			fgetc(instance->dynamicSettings->hProgramIn);
	}
	PUSH(a);
	return true;
}

bool CmdTerminate(PFUNGE_INSTANCE instance) {
	bool status = false;

	if (instance->dynamicSettings->depth != 0) {
		STACK_ITEM_TYPE row, column, direction;
		POP(&row);
		POP(&column);
		POP(&direction);

		instance->ipState->position[AXIS_X] = column;
		instance->ipState->position[AXIS_Y] = row;
		instance->ipState->direction = direction;

		instance->dynamicSettings->depth--;
		status = false;
	}
	else {
		RegisterInstanceTermination(instance);
		status = true;
	}
	return status;
}

bool CmdLoadReg(PFUNGE_INSTANCE instance) {
	STACK_ITEM_TYPE registerId, value;
	POP(&registerId);
	POP(&value);

	if (registerId < REGISTER_COUNT) {
		instance->registers[registerId] = value;
	}
	else {
		fprintf(stderr, "Invalid register ID: %d\n", registerId);
		return false;
	}
	return true;
}

bool CmdGetReg(PFUNGE_INSTANCE instance) {
	STACK_ITEM_TYPE registerId, value;
	POP(&registerId);
	
	if (registerId < REGISTER_COUNT) {
		value = instance->registers[registerId];
		PUSH(value);
	}
	else {
		fprintf(stderr, "Invalid register ID: %d\n", registerId);
		return false;
	}

	return true;
}

bool CmdSyscall(PFUNGE_INSTANCE instance) {
	
	return true;
}

bool CmdOperatingSystemInteraction(PFUNGE_INSTANCE instance) {
	STACK_ITEM_TYPE OsiId;
	STACK_ITEM_TYPE a;
	char c = '\0';
	char filePathBuffer[2048] = { 0 };
	int file_path_length = 0;
	int fileMode = 0;


	POP(&OsiId);

	switch (OsiId) {
	case OSI_FILE_OPEN:
		// Pop the file mode from the stack
		POP(&a);
		fileMode = (int)a;

		// Pop the file path from the stack
		do {
			POP(&a);
			c = (char)a;
			strncat(filePathBuffer, &c, 1);
			file_path_length++;
		} while (c != '\0');

		FILE* p = NULL;
		switch (fileMode) {
		case FLAG_FILE_MODE_READ:
			p = fopen(filePathBuffer, "r");
			if (p != NULL) {
				SetProgramInput(instance, p);
			}
			else {
				ERROR_MESSAGE("Could not open file for reading");
				goto ERROR_CASE;
			}
			break;
		case FLAG_FILE_MODE_WRITE:
			p = fopen(filePathBuffer, "w");
			if (p != NULL) {
				SetProgramOutput(instance, p);
			}
			else {
				ERROR_MESSAGE("Could not open file for writing");
				goto ERROR_CASE;
			}
			break;
		}
		break;
	case OSI_FILE_CLOSE:
		// Pop the mode to operate on.
		POP(&a);
		fileMode = (int)a;
		switch (fileMode) {
		case FLAG_FILE_MODE_READ:
			ResetProgramInput(instance);
			break;
		case FLAG_FILE_MODE_WRITE:
			ResetProgramOutput(instance);
			break;
		default:
			ERROR_MESSAGE("Invalid mode value for OSI_FILE_CLOSE");
			goto ERROR_CASE;
		}
		break;
	default:
	ERROR_CASE:
		OutputString(instance, "Can not process OSI ID", 22);
		return false;
	}
	return true;
}

bool CmdFork(PFUNGE_INSTANCE instance) {
	PFUNCTION_LIST functionEntry = NULL;
	PFUNCTION_DEFINITION functionDefinition = NULL;
	bool status = false;

	CreateInstance(((PBEFUNGE_CONTROL)instance->parent)->firstInstance);
	PINSTANCE_LIST entry = ((PBEFUNGE_CONTROL)instance->parent)->firstInstance;
	while (entry->next != NULL) {
		entry = entry->next;
	}

	// Pop the function identifier
	STACK_ITEM_TYPE functionId = 0;
	POP(&functionId);
	fprintf(stderr, "Starting function %d\n", functionId);

	// Identify the matching function by functionId
	functionEntry = ((PBEFUNGE_CONTROL)instance->parent)->functions;
	functionEntry = functionEntry->next;
	while (functionEntry != NULL) {
		if (((PFUNCTION_DEFINITION)functionEntry->pFunction)->id == functionId) {
			fprintf(stderr, "Found function definition for id: %d\n", functionId);
			functionDefinition = functionEntry->pFunction;
			break;
		}
		functionEntry = functionEntry->next;
	}

	if (functionDefinition != NULL) {
		if ((functionDefinition->entrypoint.row < instance->gridStruct->rows)
			&& (functionDefinition->entrypoint.column < instance->gridStruct->columns)){

			// Pop the FAF/WFR indicator
			STACK_ITEM_TYPE fafwfrIndicator = 0;
			POP(&fafwfrIndicator);

			if (fafwfrIndicator) {
				fprintf(stderr, "Starting function with WFR mechanism\n");
			}
			else {
				fprintf(stderr, "Starting function with FAF mechanism\n");
			}

			POSITION entrypoint;
			entrypoint[AXIS_X] = functionDefinition->entrypoint.column;
			entrypoint[AXIS_Y] = functionDefinition->entrypoint.row;

			GRID_DIMENSIONS dims;
			dims.columns = ((PBEFUNGE_CONTROL)instance->parent)->meta->dimensions.columns;
			dims.rows = ((PBEFUNGE_CONTROL)instance->parent)->meta->dimensions.rows;
			InitialiseFungeInstance(entry->pInstance, (void*)instance->parent, NULL, 0, NULL, instance->staticSettings->showState, instance->staticSettings->singleStepMode, entrypoint, dims);

			// Pop the necessary values from caller stack and push onto callee stack.

			STACK_ITEM_TYPE val = 0;
			int arg_count = functionDefinition->argumentCount;

			STACK_ITEM_TYPE tmpArgs[100] = { 0 };
			if (arg_count == -1) {
				arg_count = 1;
				// Pop string into new instance stack
				while (POP(&val), val != 0) {
					tmpArgs[arg_count++] = val;
				}

				Push(entry->pInstance, 0);

				for (; arg_count != 0; arg_count--) {
					Push(entry->pInstance, tmpArgs[arg_count]);
				}
			}
			else {
				for (; arg_count != 0; arg_count--) {
					POP(&val);
					Push(entry->pInstance, val);
				}
			}

			// SUCCESS!
			status = true;
		}
		else {
			ERROR_MESSAGE("Function definition entrypoint out of bounds!");
			status = false;
		}
	}
	else {
		ERROR_MESSAGE("Could not find function with specified ID.");
		status = false;
	}
	return status;
}

bool CmdCall(PFUNGE_INSTANCE instance) {
	bool status = true;
	STACK_ITEM_TYPE column, row, arg_count, val;
	STACK_ITEM_TYPE origA, origB;
	int count = 0;
	int next_row, next_col = 0;

	POP(&column);
	POP(&row);
	POP(&arg_count);

	// fprintf(stderr, "Row: %d, Column: %d, ArgCount: %d\n", row, column, arg_count);

	origA = instance->registers[REG_A];
	origB = instance->registers[REG_B];

	instance->registers[REG_A] = column;
	instance->registers[REG_B] = row;

	STACK_ITEM_TYPE * tmpArgs = NULL;
	
	if (arg_count == -1) {
		tmpArgs = (STACK_ITEM_TYPE*)calloc(256, sizeof(STACK_ITEM_TYPE));
		// Pop each character of a string from the stack.
		for (count = 0; POP(&val), val != 0; count++) {
			tmpArgs[count] = val;
		}
		/*count++;
		tmpArgs[count] = 0;*/
	}
	else if (arg_count != 0) {
		tmpArgs = (STACK_ITEM_TYPE*)calloc(arg_count, sizeof(STACK_ITEM_TYPE));
		
		// Pop args from stack.
		for (count = 0; count < arg_count; count++) {
			POP(&tmpArgs[count]);
		}
		count--;
	}

	// Push the return co-ordinates to the stack
	// STEP();
	next_col = instance->ipState->position[AXIS_X];
	next_row = instance->ipState->position[AXIS_Y];
	// BACKSTEP();

	PUSH(instance->ipState->direction);
	PUSH(next_col);
	PUSH(next_row);

	if (arg_count != 0) {
		for (; count >= 0; count--) {
			PUSH(tmpArgs[count]);
		}
	}

	instance->ipState->position[AXIS_Y] = instance->registers[REG_B];
	instance->ipState->position[AXIS_X] = instance->registers[REG_A];

	instance->registers[REG_A] = origA;
	instance->registers[REG_B] = origB;

	instance->dynamicSettings->depth++;

	return true;
}