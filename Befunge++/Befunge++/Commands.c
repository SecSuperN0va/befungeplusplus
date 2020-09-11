#include "Commands.h"
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "Syscalls.h"
#include "befunge_error.h"

# define POP(dest) Pop(pControl, dest) 
# define PUSH(src) Push(pControl, src)
# define STEP() TakeStep(pControl)
# define BACKSTEP() BackStep(pControl)
# define SETDIRECTION(dir) SetDirection(pControl, dir)

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
	//commandCharLookup[ORD_FORK] = CMD_FORK;
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
	//commandTable[ORD_FORK] = FUNC_FORK;
	commandTable[ORD_CALL] = FUNC_CALL;
}

bool CmdNop(PBEFUNGE_CORE_CONTROL pControl) {
	STEP();
	while (GetCommand(pControl) == CMD_NOP) {
		STEP();
	}
	BACKSTEP();
	return true;
}

bool CmdAdd(PBEFUNGE_CORE_CONTROL pControl) {
	STACK_ITEM_TYPE a, b;
	POP(&a);
	POP(&b);
	PUSH(a + b);
	return true;
}

bool CmdSubtract(PBEFUNGE_CORE_CONTROL pControl) {
	STACK_ITEM_TYPE a, b;
	POP(&a);
	POP(&b);
	PUSH(b - a);
	return true;
}

bool CmdMultiply(PBEFUNGE_CORE_CONTROL pControl) {
	STACK_ITEM_TYPE a, b;
	POP(&a);
	POP(&b);
	PUSH(a * b);
	return true;
}

bool CmdDivide(PBEFUNGE_CORE_CONTROL pControl) {
	STACK_ITEM_TYPE a, b;
	POP(&a);
	POP(&b);
	PUSH(b / a);
	return true;
}

bool CmdModulo(PBEFUNGE_CORE_CONTROL pControl) {
	STACK_ITEM_TYPE a, b;
	POP(&a);
	POP(&b);
	PUSH(b % a);
	return true;
}

bool CmdNot(PBEFUNGE_CORE_CONTROL pControl) {
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

bool CmdGreaterThan(PBEFUNGE_CORE_CONTROL pControl) {
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

bool CmdMoveRight(PBEFUNGE_CORE_CONTROL pControl) {
	SETDIRECTION(DIR_RIGHT);
	return true;
}

bool CmdMoveLeft(PBEFUNGE_CORE_CONTROL pControl) {
	SETDIRECTION(DIR_LEFT);
	return true;
}

bool CmdMoveUp(PBEFUNGE_CORE_CONTROL pControl) {
	SETDIRECTION(DIR_UP);
	return true;
}

bool CmdMoveDown(PBEFUNGE_CORE_CONTROL pControl) {
	SETDIRECTION(DIR_DOWN);
	return true;
}

bool CmdMoveRand(PBEFUNGE_CORE_CONTROL pControl) {
	int a;
	srand((unsigned)time(0));
	a = rand() % 4;	// random number between 0 and the number of directions available (4)
	SETDIRECTION(a);
	return true;
}

bool CmdHorizontalNullCheck(PBEFUNGE_CORE_CONTROL pControl) {
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

bool CmdVerticalNullCheck(PBEFUNGE_CORE_CONTROL pControl) {
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

bool CmdStringMode(PBEFUNGE_CORE_CONTROL pControl) {
	ToggleStringMode(pControl);
	return true;
}

bool CmdDuplicate(PBEFUNGE_CORE_CONTROL pControl) {
	STACK_ITEM_TYPE a;
	POP(&a);
	PUSH(a);
	PUSH(a);
	return true;
}

bool CmdSwap(PBEFUNGE_CORE_CONTROL pControl) {
	STACK_ITEM_TYPE a, b;
	POP(&a);
	POP(&b);
	PUSH(a);
	PUSH(b);
	return true;
}

bool CmdDiscard(PBEFUNGE_CORE_CONTROL pControl) {
	POP(NULL);
	return true;
}

bool CmdIntegerOut(PBEFUNGE_CORE_CONTROL pControl) {
	STACK_ITEM_TYPE a;
	char tempBuffer[20];
	POP(&a);
	snprintf(tempBuffer, sizeof(tempBuffer), "%d ", (unsigned int)a);
	OutputString(pControl, tempBuffer, strlen(tempBuffer));
	return true;
}

bool CmdASCIIOut(PBEFUNGE_CORE_CONTROL pControl) {
	STACK_ITEM_TYPE a;
	char tempBuffer[20];
	POP(&a);
	snprintf(tempBuffer, sizeof(tempBuffer), "%c", a);
	OutputString(pControl, tempBuffer, 1);
	return true;
}

bool CmdBridge(PBEFUNGE_CORE_CONTROL pControl) {
	STEP();
	return true;
}

bool CmdPut(PBEFUNGE_CORE_CONTROL pControl) {
	Put(pControl);
	return true;
}

bool CmdGet(PBEFUNGE_CORE_CONTROL pControl) {
	Get(pControl);
	return true;
}

bool CmdIntegerIn(PBEFUNGE_CORE_CONTROL pControl) {
	STACK_ITEM_TYPE a;
	a = fgetc(pControl->pDynamicSettings->hProgramIn);
	// Flush newline character
	if (pControl->pDynamicSettings->hProgramIn == stdin)
		fgetc(pControl->pDynamicSettings->hProgramIn);
	while (a < 0x30 || a > 0x39) {
		fprintf(stderr, "Invalid Number! Please enter a single digit number: ");
		a = fgetc(pControl->pDynamicSettings->hProgramIn);
		// Flush newline character
		if (pControl->pDynamicSettings->hProgramIn == stdin)
			fgetc(pControl->pDynamicSettings->hProgramIn);
	}
	PUSH(a - 0x30);
	return true;
}

bool CmdASCIIIn(PBEFUNGE_CORE_CONTROL pControl) {
	STACK_ITEM_TYPE a;
	a = fgetc(pControl->pDynamicSettings->hProgramIn);
	// Flush newline character
	if (pControl->pDynamicSettings->hProgramIn == stdin)
		fgetc(pControl->pDynamicSettings->hProgramIn);
	while (a < 0x20 || a > 0x7F) {
		fprintf(stderr, "Invalid Character! Please enter an ASCII character: ");
		a = fgetc(pControl->pDynamicSettings->hProgramIn);
		// Flush newline character
		if (pControl->pDynamicSettings->hProgramIn == stdin)
			fgetc(pControl->pDynamicSettings->hProgramIn);
	}
	PUSH(a);
	return true;
}

bool CmdTerminate(PBEFUNGE_CORE_CONTROL pControl) {
	bool status = false;

	if (pControl->pDynamicSettings->depth != 0) {
		STACK_ITEM_TYPE page, row, column, direction;
		POP(&page);
		POP(&row);
		POP(&column);
		POP(&direction);

		pControl->pManager->pIpState->position.column = column;
		pControl->pManager->pIpState->position.row = row;
		pControl->pManager->pIpState->direction = direction;

		pControl->pManager->pCurrentPageControl = PageControlListGetPageById(pControl->pManager->pPageControlList, page);

		pControl->pDynamicSettings->depth--;
		status = false;
	}
	else {
		//RegisterInstanceTermination(pControl);
		pControl->hasTerminated = true;
		status = true;
	}
	return status;
}

bool CmdLoadReg(PBEFUNGE_CORE_CONTROL pControl) {
	STACK_ITEM_TYPE registerId, value;
	POP(&registerId);
	POP(&value);

	if (registerId < REGISTER_COUNT) {
		pControl->pManager->registers[registerId] = value;
	}
	else {
		fprintf(stderr, "Invalid register ID: %d\n", registerId);
		return false;
	}
	return true;
}

bool CmdGetReg(PBEFUNGE_CORE_CONTROL pControl) {
	STACK_ITEM_TYPE registerId, value;
	POP(&registerId);
	
	if (registerId < REGISTER_COUNT) {
		value = pControl->pManager->registers[registerId];
		PUSH(value);
	}
	else {
		fprintf(stderr, "Invalid register ID: %d\n", registerId);
		return false;
	}

	return true;
}

bool CmdOperatingSystemInteraction(PBEFUNGE_CORE_CONTROL pControl) {
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
				SetProgramInput(pControl, p);
			}
			else {
				ERROR_MESSAGE("Could not open file for reading");
				goto ERROR_CASE;
			}
			break;
		case FLAG_FILE_MODE_WRITE:
			p = fopen(filePathBuffer, "w");
			if (p != NULL) {
				SetProgramOutput(pControl, p);
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
			ResetProgramInput(pControl);
			break;
		case FLAG_FILE_MODE_WRITE:
			ResetProgramOutput(pControl);
			break;
		default:
			ERROR_MESSAGE("Invalid mode value for OSI_FILE_CLOSE");
			goto ERROR_CASE;
		}
		break;
	default:
	ERROR_CASE:
		OutputString(pControl, "Can not process OSI ID", 22);
		return false;
	}
	return true;
}


bool CmdCall(PBEFUNGE_CORE_CONTROL pControl) {
	bool status = true;
	STACK_ITEM_TYPE page, column, row, arg_count, val;
	STACK_ITEM_TYPE origA, origB;
	int count = 0;
	int next_row = 0, next_col = 0, calling_page = 0;

	POP(&page);
	POP(&column);
	POP(&row);
	POP(&arg_count);

	// fprintf(stderr, "Row: %d, Column: %d, ArgCount: %d\n", row, column, arg_count);

	origA = pControl->pManager->registers[REG_A];
	origB = pControl->pManager->registers[REG_B];

	pControl->pManager->registers[REG_A] = column;
	pControl->pManager->registers[REG_B] = row;

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
	next_col = pControl->pManager->pIpState->position.column;
	next_row = pControl->pManager->pIpState->position.row;
	calling_page = pControl->pManager->pCurrentPageControl->id;
	// BACKSTEP();

	PUSH(pControl->pManager->pIpState->direction);
	PUSH(next_col);
	PUSH(next_row);
	PUSH(calling_page);

	if (arg_count != 0) {
		for (; count >= 0; count--) {
			PUSH(tmpArgs[count]);
		}
	}

	PPAGE_CONTROL tmp = PageControlListGetPageById(pControl->pManager->pPageControlList, page);
	if (tmp == NULL) {
		ERROR_MESSAGE("Could not locate page with matching ID. Not changing pages");
	}
	else {
		pControl->pManager->pCurrentPageControl = tmp;
	}

	pControl->pManager->pIpState->position.row = pControl->pManager->registers[REG_B];
	pControl->pManager->pIpState->position.column = pControl->pManager->registers[REG_A];

	pControl->pManager->registers[REG_A] = origA;
	pControl->pManager->registers[REG_B] = origB;

	pControl->pDynamicSettings->depth++;

	return true;
}