#include "Commands.h"
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "Syscalls.h"
#include "befunge_error.h"


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
}

bool CmdNop(PFUNGE_INSTANCE instance) {
	TakeStep(instance);
	while (GetCommand(instance) == CMD_NOP) {
		TakeStep(instance);
	}
	BackStep(instance);
	return true;
}

bool CmdAdd(PFUNGE_INSTANCE instance) {
	STACK_ITEM_TYPE a, b;
	Pop(instance, &a);
	Pop(instance, &b);
	Push(instance, a + b);
	return true;
}

bool CmdSubtract(PFUNGE_INSTANCE instance) {
	STACK_ITEM_TYPE a, b;
	Pop(instance, &a);
	Pop(instance, &b);
	Push(instance, b - a);
	return true;
}

bool CmdMultiply(PFUNGE_INSTANCE instance) {
	STACK_ITEM_TYPE a, b;
	Pop(instance, &a);
	Pop(instance, &b);
	Push(instance, a * b);
	return true;
}

bool CmdDivide(PFUNGE_INSTANCE instance) {
	STACK_ITEM_TYPE a, b;
	Pop(instance, &a);
	Pop(instance, &b);
	Push(instance, b / a);
	return true;
}

bool CmdModulo(PFUNGE_INSTANCE instance) {
	STACK_ITEM_TYPE a, b;
	Pop(instance, &a);
	Pop(instance, &b);
	Push(instance, b % a);
	return true;
}

bool CmdNot(PFUNGE_INSTANCE instance) {
	STACK_ITEM_TYPE a;
	Pop(instance, &a);
	if (a == 0) {
		Push(instance, (STACK_ITEM_TYPE)1);
	}
	else {
		Push(instance, (STACK_ITEM_TYPE)0);
	}
	return true;
}

bool CmdGreaterThan(PFUNGE_INSTANCE instance) {
	STACK_ITEM_TYPE a, b;
	Pop(instance, &a);
	Pop(instance, &b);

	if (b > a) {
		Push(instance, 1);
	}
	else {
		Push(instance, 0);
	}
	return true;
}

bool CmdMoveRight(PFUNGE_INSTANCE instance) {
	SetDirection(instance, DIR_RIGHT);
	return true;
}

bool CmdMoveLeft(PFUNGE_INSTANCE instance) {
	SetDirection(instance, DIR_LEFT);
	return true;
}

bool CmdMoveUp(PFUNGE_INSTANCE instance) {
	SetDirection(instance, DIR_UP);
	return true;
}

bool CmdMoveDown(PFUNGE_INSTANCE instance) {
	SetDirection(instance, DIR_DOWN);
	return true;
}

bool CmdMoveRand(PFUNGE_INSTANCE instance) {
	int a;
	srand((unsigned)time(0));
	a = rand() % 4;	// random number between 0 and the number of directions available (4)
	SetDirection(instance, a);
	return true;
}

bool CmdHorizontalNullCheck(PFUNGE_INSTANCE instance) {
	STACK_ITEM_TYPE a;
	Pop(instance, &a);
	if (a == 0) {
		SetDirection(instance, DIR_RIGHT);
	}
	else {
		SetDirection(instance, DIR_LEFT);
	}
	return true;
}

bool CmdVerticalNullCheck(PFUNGE_INSTANCE instance) {
	STACK_ITEM_TYPE a;
	Pop(instance, &a);
	if (a == 0) {
		SetDirection(instance, DIR_DOWN);
	}
	else {
		SetDirection(instance, DIR_UP);
	}
	return true;
}

bool CmdStringMode(PFUNGE_INSTANCE instance) {
	ToggleStringMode(instance);
	return true;
}

bool CmdDuplicate(PFUNGE_INSTANCE instance) {
	STACK_ITEM_TYPE a;
	Pop(instance, &a);
	Push(instance, a);
	Push(instance, a);
	return true;
}

bool CmdSwap(PFUNGE_INSTANCE instance) {
	STACK_ITEM_TYPE a, b;
	Pop(instance, &a);
	Pop(instance, &b);
	Push(instance, a);
	Push(instance, b);
	return true;
}

bool CmdDiscard(PFUNGE_INSTANCE instance) {
	Pop(instance, NULL);
	return true;
}

bool CmdIntegerOut(PFUNGE_INSTANCE instance) {
	STACK_ITEM_TYPE a;
	char tempBuffer[20];
	Pop(instance, &a);
	snprintf(tempBuffer, sizeof(tempBuffer), "%d ", (unsigned int)a);
	OutputString(instance, tempBuffer, strlen(tempBuffer));
	return true;
}

bool CmdASCIIOut(PFUNGE_INSTANCE instance) {
	STACK_ITEM_TYPE a;
	char tempBuffer[20];
	Pop(instance, &a);
	snprintf(tempBuffer, sizeof(tempBuffer), "%c", a);
	OutputString(instance, tempBuffer, 1);
	return true;
}

bool CmdBridge(PFUNGE_INSTANCE instance) {
	TakeStep(instance);
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
	Push(instance, a - 0x30);
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
	Push(instance, a);
	return true;
}

bool CmdTerminate(PFUNGE_INSTANCE instance) {
	// TODO: Add termination sequence!
	RegisterInstanceTermination(instance);
	return true;
}

bool CmdLoadReg(PFUNGE_INSTANCE instance) {
	STACK_ITEM_TYPE registerId, value;
	Pop(instance, &registerId);
	Pop(instance, &value);

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
	Pop(instance, &registerId);
	
	if (registerId < REGISTER_COUNT) {
		value = instance->registers[registerId];
		Push(instance, value);
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


	Pop(instance, &OsiId);

	switch (OsiId) {
	case OSI_FILE_OPEN:
		// Pop the file mode from the stack
		Pop(instance, &a);
		fileMode = (int)a;

		// Pop the file path from the stack
		do {
			Pop(instance, &a);
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
		Pop(instance, &a);
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
}

bool CmdFork(PFUNGE_INSTANCE instance) {
	CreateInstance(((PBEFUNGE_CONTROL)instance->parent)->firstInstance);
	PINSTANCE_LIST entry = ((PBEFUNGE_CONTROL)instance->parent)->firstInstance;
	while (entry->next != NULL) {
		entry = entry->next;
	}
	POSITION entrypoint;
	entrypoint[AXIS_X] = 0;
	entrypoint[AXIS_Y] = 0;

	GRID_DIMENSIONS dims;
	dims.columns = 18;
	dims.rows = 1;
	InitialiseFungeInstance(entry->pInstance, (void*)instance->parent, ">\"TESTING\",,,,,,,@", 0, NULL, true, true, entrypoint, dims);

	return;
}