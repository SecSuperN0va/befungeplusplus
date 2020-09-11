#include "control.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "srcutil.h"
#include "befunge_error.h"
#include "Commands.h"

/*
	POPULATION METHODS
*/

void RunPopulation() {
	PopulateCommandTable();
	PopulateCommandCharLookup();
	return;
}

/*
	INITIALISATION METHODS
*/

void InitialiseStaticControlSettings(PSTATIC_CONTROL_SETTINGS *settings, bool showState, bool singleStep, int tickDelay) {
	*settings = (PSTATIC_CONTROL_SETTINGS)calloc(1, sizeof(STATIC_CONTROL_SETTINGS));
	(*settings)->showState = showState;
	(*settings)->singleStepMode = singleStep;
	(*settings)->outputMaxSize = DEFAULT_OUTPUT_SIZE;
	(*settings)->tickDelay = tickDelay;
	return;
}

void InitialiseDynamicControlSettings(PDYNAMIC_CONTROL_SETTINGS *settings) {
	*settings = (PDYNAMIC_CONTROL_SETTINGS)calloc(1, sizeof(DYNAMIC_CONTROL_SETTINGS));
	(*settings)->stringMode = false;
	(*settings)->depth = 0;
	(*settings)->hProgramIn = stdin;
	(*settings)->hProgramOut = stdout;
	return;
}

void InitialiseInstructionPointerState(PINSTRUCTION_POINTER_STATE *ipState, GRID_POINT *entrypoint) {
	int i = 0;
	*ipState = (PINSTRUCTION_POINTER_STATE)calloc(1, sizeof(INSTRUCTION_POINTER_STATE));
	(*ipState)->direction = DIR_RIGHT;
	
	(*ipState)->position.column = entrypoint->column;
	(*ipState)->position.row = entrypoint->row;

	return;
}

void InitialiseRegisters(REGISTERS registers) {
	int i;
	for (i = 0; i < REGISTER_COUNT; i++) {
		registers[i] = 0;
	}
}

void InitialiseFungeStack(PFUNGE_STACK_STATE *stackStruct) {
	*stackStruct = (PFUNGE_STACK_STATE)calloc(1, sizeof(FUNGE_STACK_STATE));
	(*stackStruct)->stack = (STACK_ITEM_TYPE*)calloc(DEFAULT_STACK_SIZE, sizeof(STACK_ITEM_TYPE));
	(*stackStruct)->stackPointer = 0;
	return;
}

void InitialiseOutputState(PFUNGE_OUTPUT_STATE *outputState) {
	*outputState = (PFUNGE_OUTPUT_STATE)calloc(1, sizeof(FUNGE_OUTPUT_STATE));
	(*outputState)->output = NULL;
	(*outputState)->outputFile = NULL;
	(*outputState)->outputSize = 0;
	return;
}

void InitialisePageManager(PFUNGE_PAGE_MANAGER *pPageManager, PPAGE_CONTROL_LIST pPageList, PBEFUNGE_PROGRAM_CONFIG pConfig) {
	*pPageManager = (PFUNGE_PAGE_MANAGER)calloc(1, sizeof(FUNGE_PAGE_MANAGER));

	// Configure the registers
	InitialiseRegisters((*pPageManager)->registers);

	// Configure the core program stack
	InitialiseFungeStack(&((*pPageManager)->pStackState));

	// Configure the Instruction pointer to match the defined entrypoint
	InitialiseInstructionPointerState(&((*pPageManager)->pIpState), &(pConfig->pMetadata->context.entrypoint));

	// Configure the current page pointer to match the specified initial page id
	(*pPageManager)->pPageControlList = pPageList;
	(*pPageManager)->pCurrentPageControl = PageControlListGetPageById(
		(*pPageManager)->pPageControlList,
		pConfig->pMetadata->context.initialPageId
	);
}

PPAGE_CONTROL PageControlListGetPageById(PPAGE_CONTROL_LIST pPageControlList, int pageId) {
	PPAGE_CONTROL_LIST current = pPageControlList->next;
	while (current->pPageControl != NULL) {
		if (current->pPageControl->id == pageId) {
			return current->pPageControl;
		}
		if (current->next == NULL) {
			break;
		}
		current = current->next;
	}
	return NULL;
}

void InitialiseCoreControl(PBEFUNGE_CORE_CONTROL pControl, PBEFUNGE_PROGRAM_CONFIG pConfig){
	pControl->pConfig = pConfig;
	InitialiseStaticControlSettings(&(pControl->pStaticSettings), pConfig->isVisualiseModeActive, pConfig->isSingleSteModeActive, pConfig->tickDelay);
	InitialiseDynamicControlSettings(&(pControl->pDynamicSettings));
	InitialiseOutputState(&(pControl->pOutputState));
	pControl->pOutputState->outputFile = pConfig->pOutFile;
	pControl->pOutputState->output = (char*)calloc(DEFAULT_OUTPUT_SIZE, sizeof(char));
	pControl->pOutputState->outputSize = 0;

	pControl->hasTerminated = false;

	if (pConfig->mode == MODE_PAGED) {
		InitialisePageManager(&pControl->pManager, pConfig->pPages, pConfig);
	}
	else {
		INFO_MESSAGE("Requested initialisation of Non-Paged program");
	}
	return;
}

/*
	VISUALISATION METHODS
*/

void OutputString(PBEFUNGE_CORE_CONTROL pControl, char* str, int len) {
	if (strlen(str) < (unsigned)(pControl->pStaticSettings->outputMaxSize - pControl->pOutputState->outputSize)) {
		memcpy(
			pControl->pOutputState->output + pControl->pOutputState->outputSize,
			str,
			len
		);
		pControl->pOutputState->outputSize += len;
		fwrite(str, 1, len, pControl->pDynamicSettings->hProgramOut);
		if (pControl->pOutputState->outputFile != NULL || pControl->pOutputState->outputFile == stdout || pControl->pOutputState->outputFile == stderr) {
			fwrite(str, 1, len, pControl->pOutputState->outputFile);
		}
	}
	else {
		if (pControl->pStaticSettings->showState) {
			char* format = "Cannot output \"%s\". Not enough space in output buffer";
			int outputLen = strlen(format) + len;
			char* msg = (char*)calloc(outputLen, sizeof(char));
			snprintf(msg, outputLen, format, str);
			ERROR_MESSAGE("%s", msg);
		}
	}
	return;
}

void OutputRegistersState(PBEFUNGE_CORE_CONTROL pControl) {
	fprintf(stderr, "REG_A: %08x\n", pControl->pManager->registers[REG_A]);
	fprintf(stderr, "REG_B: %08x\n", pControl->pManager->registers[REG_B]);
	fprintf(stderr, "REG_C: %08x\n", pControl->pManager->registers[REG_C]);
	fprintf(stderr, "REG_D: %08x\n", pControl->pManager->registers[REG_D]);
	return;
}

void PrintOutputState(PBEFUNGE_CORE_CONTROL pControl) {
	int total_required = 0;
	char* buffer = NULL;
	const char title[] = "Output:\n============\n";

	total_required += strlen(title);
	total_required += pControl->pOutputState->outputSize;
	total_required += 1;

	buffer = (char*)calloc(total_required, sizeof(char));

	strncat(buffer, title, strlen(title));
	strncat(buffer, pControl->pOutputState->output, pControl->pOutputState->outputSize);

	fwrite(buffer, 1, total_required, stderr);
	return;
}

void PrintProgramState(PBEFUNGE_CORE_CONTROL pControl) {
	int column, row, c;
	char* formatString = " %c ";
	int total_required = 0;
	char* buffer = NULL;
	const char title[] = "Program State:\n==============\n";
	char tmpBuf[32] = { 0 };
	bool isSelected = false;
	
	total_required += strlen(title);	
	total_required += (pControl->pManager->pCurrentPageControl->grid->rows + 2) * (pControl->pManager->pCurrentPageControl->grid->columns + 3); // For each cell
	total_required += 12; // to account for the ansi escape codes around the selected cell
	total_required += 1;

	buffer = (char*)calloc(total_required, sizeof(char));

	strncat(buffer, title, strlen(title));

	// Draw top line
	for (c = 0; c < pControl->pManager->pCurrentPageControl->grid->columns + 2; c++) {
		strncat(buffer, "_", 1);
	}
	strncat(buffer, "\n", 1);

	// Fill the grid
	for (row = 0; row < pControl->pManager->pCurrentPageControl->grid->rows; row++) {
		strncat(buffer, "|", 1);
		for (column = 0; column < pControl->pManager->pCurrentPageControl->grid->columns; column++) {
			if (column == pControl->pManager->pIpState->position.column && row == pControl->pManager->pIpState->position.row) {
				isSelected = true;
			}
			else {
				isSelected = false;
			}
			if (pControl->pManager->pCurrentPageControl->grid->grid[row][column] == '%') {
				if (isSelected)
					snprintf(tmpBuf, sizeof(tmpBuf), "%c[101;1m%%%%%c[%dm", 0x1B, 0x1b, 0);
				else
					snprintf(tmpBuf, sizeof(tmpBuf), "%%%%");
			}
			else if (0x20 <= pControl->pManager->pCurrentPageControl->grid->grid[row][column] && pControl->pManager->pCurrentPageControl->grid->grid[row][column] <= 0x80) {
				if (isSelected) 
					snprintf(tmpBuf, sizeof(tmpBuf), "%c[101;1m%c%c[%dm", 0x1B, pControl->pManager->pCurrentPageControl->grid->grid[row][column], 0x1b, 0);
				else 
					snprintf(tmpBuf, sizeof(tmpBuf), "%c", pControl->pManager->pCurrentPageControl->grid->grid[row][column]);
			}
			else {
				if (isSelected)
					snprintf(tmpBuf, sizeof(tmpBuf), "%c[101;1m.%c[%dm", 0x1B, 0x1b, 0);
				else
					snprintf(tmpBuf, sizeof(tmpBuf), ".");
				
			}
			strncat(buffer, tmpBuf, sizeof(tmpBuf));
			memset(tmpBuf, 0, sizeof(tmpBuf));
		}
		strncat(buffer, "|\n", sizeof(tmpBuf));
	}

	// Draw top line
	for (c = 0; c < pControl->pManager->pCurrentPageControl->grid->columns + 2; c++) {
		strncat(buffer, "_", 1);
	}
	strncat(buffer, "\n", 1);

	fprintf(stderr, buffer);

	return;
}

void PrintStackState(PBEFUNGE_CORE_CONTROL pControl) {
	int stackWindowMaxSize = 25;
	int c, d = 0;
	int total_required = 0;
	char* buffer = NULL;
	const char title[] = "Stack State:\n============\n";
	char tmpBuf[41] = { 0 };

	if (stackWindowMaxSize >= pControl->pManager->pStackState->stackPointer) {
		c = pControl->pManager->pStackState->stackPointer - 1;
	}
	else {
		c = stackWindowMaxSize;
	}

	total_required += strlen(title);
	total_required += (c * sizeof(tmpBuf)) + (sizeof(tmpBuf) * 2) + sizeof(tmpBuf);
	total_required += 1;

	buffer = (char*)calloc(total_required, sizeof(char));

	strncat(buffer, title, strlen(title));
	

	snprintf(tmpBuf, sizeof(tmpBuf), "   %d   \n", pControl->pManager->pStackState->stackPointer);
	strncat(buffer, tmpBuf, strlen(tmpBuf));
	memset(tmpBuf, 0, strlen(tmpBuf));

	strncat(buffer, "|------------|\n", strlen(title));

	for (; c >= 0; c--) {
		snprintf(tmpBuf, sizeof(tmpBuf), "|  %08x  | (%-6d)\n", pControl->pManager->pStackState->stack[c], c);
		strncat(buffer, tmpBuf, strlen(tmpBuf));
		memset(tmpBuf, 0, strlen(tmpBuf));
	}
	strncat(buffer, "|------------|\n", strlen(title));


	fprintf(stderr, buffer);
	fprintf(stderr, "Function call depth: %d\n", pControl->pDynamicSettings->depth);
}

/*
	PRIMITIVE METHODS
*/

char GetCommand(PBEFUNGE_CORE_CONTROL pControl) {
	return pControl->pManager->pCurrentPageControl->grid->grid[pControl->pManager->pIpState->position.row][pControl->pManager->pIpState->position.column];
}

void Push(PBEFUNGE_CORE_CONTROL pControl, STACK_ITEM_TYPE value) {
	char buf[10] = { 0 };
	pControl->pManager->pStackState->stack[pControl->pManager->pStackState->stackPointer++] = value;
	return;
}

void Pop(PBEFUNGE_CORE_CONTROL pControl, STACK_ITEM_TYPE* out) {
	char buf[10] = { 0 };

	if (pControl->pManager->pStackState->stackPointer > 0){
		STACK_ITEM_TYPE value = pControl->pManager->pStackState->stack[--pControl->pManager->pStackState->stackPointer];
		if (out != NULL) {
			*out = value;
		}
	}
	else {
		ERROR_MESSAGE("Nothing on stack to pop");
	}
	
	return;
}

void SetDirection(PBEFUNGE_CORE_CONTROL pControl, int direction) {
	pControl->pManager->pIpState->direction = direction;
}

void Put(PBEFUNGE_CORE_CONTROL pControl) {
	//char row, column, value;
	STACK_ITEM_TYPE row, column, value;

	// retrieve two values from the stack
	Pop(pControl, &row);
	Pop(pControl, &column);
	Pop(pControl, &value);

	if (0 > row || row >= pControl->pManager->pCurrentPageControl->grid->rows) {
		char* rowErrorformat = "Put: Selected row out of range: %d";
		char* rowErrorStr = (char*)calloc(strlen(rowErrorformat) + 10, sizeof(char));
		snprintf(rowErrorStr, strlen(rowErrorformat) + 10, rowErrorformat, row);
		ERROR_MESSAGE("%s", rowErrorStr);
		return;
	}

	if (0 > column || column >= pControl->pManager->pCurrentPageControl->grid->columns) {
		char* colErrorformat = "Put: Selected column out of range: %d";
		char* colErrorStr = (char*)calloc(strlen(colErrorformat) + 10, sizeof(char));
		snprintf(colErrorStr, strlen(colErrorformat) + 10, colErrorformat, column);
		ERROR_MESSAGE("%s", colErrorStr);
		return;
	}

	pControl->pManager->pCurrentPageControl->grid->grid[row][column] = (char) value;

	return;
}

void Get(PBEFUNGE_CORE_CONTROL pControl) {
	STACK_ITEM_TYPE row, column;

	// retrieve two values from the stack
	Pop(pControl, &row);
	Pop(pControl, &column);

	if (0 > row || row >= pControl->pManager->pCurrentPageControl->grid->rows) {
		char* rowErrorformat = "Get: Selected row out of range: %d";
		char* rowErrorStr = (char*)calloc(strlen(rowErrorformat) + 10, sizeof(char));
		snprintf(rowErrorStr, strlen(rowErrorformat) + 10, rowErrorformat, row);
		ERROR_MESSAGE("%s", rowErrorStr);
		return;
	}

	if (0 > column || column >= pControl->pManager->pCurrentPageControl->grid->columns) {
		char* colErrorformat = "Get: Selected column out of range: %d";
		char* colErrorStr = (char*)calloc(strlen(colErrorformat) + 10, sizeof(char));
		snprintf(colErrorStr, strlen(colErrorformat) + 10, colErrorformat, column);
		ERROR_MESSAGE("%s", colErrorStr);
		return;
	}

	Push(pControl, pControl->pManager->pCurrentPageControl->grid->grid[row][column]);

	return;
}

/*
	SETTING CHANGE METHODS
*/

void ToggleStringMode(PBEFUNGE_CORE_CONTROL pControl) {
	pControl->pDynamicSettings->stringMode = !pControl->pDynamicSettings->stringMode;
	return;
}

void FlipDirection(PBEFUNGE_CORE_CONTROL pControl) {
	if (pControl->pManager->pIpState->direction == DIR_RIGHT)
		SetDirection(pControl, DIR_LEFT);
	else if (pControl->pManager->pIpState->direction == DIR_LEFT)
		SetDirection(pControl, DIR_RIGHT);
	else if (pControl->pManager->pIpState->direction == DIR_UP)
		SetDirection(pControl, DIR_DOWN);
	else if (pControl->pManager->pIpState->direction == DIR_DOWN)
		SetDirection(pControl, DIR_UP);
	else
		ERROR_MESSAGE("Seriously... What the fuck?\n")

}

void SetProgramInput(PBEFUNGE_CORE_CONTROL pControl, FILE* hInput) {
	pControl->pDynamicSettings->hProgramIn = hInput;
	return;
}

void SetProgramOutput(PBEFUNGE_CORE_CONTROL pControl, FILE* hOutput) {
	pControl->pDynamicSettings->hProgramOut = hOutput;
	return;
}

void ResetProgramInput(PBEFUNGE_CORE_CONTROL pControl) {
	if (pControl->pDynamicSettings->hProgramIn != stdin && pControl->pDynamicSettings->hProgramIn != NULL) {
		fclose(pControl->pDynamicSettings->hProgramIn);
		SetProgramInput(pControl, (FILE*)stdin);
	}
	else {
		ERROR_MESSAGE("Cannot close/reset program input. Either already reset or double close attempted.")
	}
	return;
}

void ResetProgramOutput(PBEFUNGE_CORE_CONTROL pControl) {
	if (pControl->pDynamicSettings->hProgramOut != stdout && pControl->pDynamicSettings->hProgramOut != NULL) {
		fclose(pControl->pDynamicSettings->hProgramOut);
		SetProgramOutput(pControl, (FILE*)stdout);
	}
	else {
		ERROR_MESSAGE("Cannot close/reset program output. Either already reset or double close attempted.")
	}
	return;
}

/*
	CONTROL METHODS
*/

void LoadProgramString(PBEFUNGE_CORE_CONTROL pControl, char* programString) {
	int columnCount = 0;
	int rowCount = 0;
	unsigned int ptr = 0;
	char c = '\0';
	int i, j = 0;
	// Fill the entire grid with NOPS
	for (i = 0; i < pControl->pManager->pCurrentPageControl->grid->rows; i++) {
		for (j = 0; j < pControl->pManager->pCurrentPageControl->grid->columns; j++) {
			pControl->pManager->pCurrentPageControl->grid->grid[i][j] = 0x20;
		}
	}

	while (ptr < strlen(programString)) {
		if ((ptr < strlen(programString) - 1
			&& (programString[ptr] == '\r' && programString[ptr + 1] == '\n'))
			|| (programString[ptr] == '\r' || programString[ptr] == '\n')
			|| columnCount == pControl->pManager->pCurrentPageControl->grid->columns) {
			/*while (programString[ptr] == '\r' || programString[ptr] == '\n') {
				ptr++;
			}*/ 
			ptr++;
			rowCount++;
			columnCount = 0;
			continue;
		}
		c = programString[ptr++];
		pControl->pManager->pCurrentPageControl->grid->grid[rowCount][columnCount++] = c;
	}

	return;
}

void TakeStep(PBEFUNGE_CORE_CONTROL pControl) {
	switch (pControl->pManager->pIpState->direction) {
	case DIR_RIGHT:
		pControl->pManager->pIpState->position.column++;
		if (pControl->pManager->pIpState->position.column == pControl->pManager->pCurrentPageControl->grid->columns) {
			pControl->pManager->pIpState->position.column = 0;
		}
		break;
	case DIR_LEFT:
		pControl->pManager->pIpState->position.column--;
		if (pControl->pManager->pIpState->position.column == -1) {
			pControl->pManager->pIpState->position.column = pControl->pManager->pCurrentPageControl->grid->columns -1;
		}
		break;
	case DIR_UP:
		pControl->pManager->pIpState->position.row--;
		if (pControl->pManager->pIpState->position.row == -1) {
			pControl->pManager->pIpState->position.row = pControl->pManager->pCurrentPageControl->grid->rows - 1;
		}
		break;
	case DIR_DOWN:
		pControl->pManager->pIpState->position.row++;
		if (pControl->pManager->pIpState->position.row == pControl->pManager->pCurrentPageControl->grid->rows) {
			pControl->pManager->pIpState->position.row = 0;
		}
		break;
	default:
		ERROR_MESSAGE("How the fuck did this happen?");
		break;
	}
}

void BackStep(PBEFUNGE_CORE_CONTROL pControl) {
	int origDirection = pControl->pManager->pIpState->direction;
	int backStepDirection = (int)NULL;

	switch (origDirection) {
	case DIR_RIGHT:
		backStepDirection = DIR_LEFT;
		break;
	case DIR_LEFT:
		backStepDirection = DIR_RIGHT;
		break;
	case DIR_UP:
		backStepDirection = DIR_DOWN;
		break;
	case DIR_DOWN:
		backStepDirection = DIR_UP;
		break;
	default:
		ERROR_MESSAGE("Shit went down...");
		break;
	}

	SetDirection(pControl, backStepDirection);
	TakeStep(pControl);
	SetDirection(pControl, origDirection);

	return;
}

int ProcessTick(PBEFUNGE_CORE_CONTROL pControl) {
	int retval = STATUS_OK;
	bool commandStatus = false;
	char command = GetCommand(pControl);


	if (pControl->pDynamicSettings->stringMode) {
		// If the current context is string mode process the command as if it were ASCII, unless command is " char
		if (command == CMD_STRING) {
			ToggleStringMode(pControl);
		}
		else {
			Push(pControl, (STACK_ITEM_TYPE)command);
		}
		commandStatus = true;
	}
	else if (command >= 0x30 && command <= 0x39) {
		Push(pControl, (STACK_ITEM_TYPE)(command - 0x30));
		commandStatus = true;
	}
	else {
		commandStatus = AcceptCommand(pControl, command);
	}
	switch (command) {
	case CMD_CALL:
		if (commandStatus) {
			retval = STATUS_CALLED;
		}
		else {
			retval = STATUS_BAD_COMMAND_RESULT;
		}
		break;
	case CMD_END:
		if (commandStatus) {
			retval = STATUS_TERMINATED;
		}
		else {
			retval = STATUS_OK;
		}
		break;
	default:
		if (commandStatus) {
			// Command success
			retval = STATUS_OK;
		}
		else {
			retval = STATUS_BAD_COMMAND_RESULT;
		}
		break;
	}
	return retval;
}

bool AcceptCommand(PBEFUNGE_CORE_CONTROL pControl, char commandChar) {
	int counter = 0;
	int commandOrdinal = 0;
	bool status = false;
	bool commandHasBeenExecuted = false;

	for (counter = 0; counter < MAX_ORD; counter++) {
		if (commandChar == commandCharLookup[counter]) {
			// Command Character matches, we can process the command
			commandOrdinal = counter;
			status = commandTable[commandOrdinal](pControl);
			commandHasBeenExecuted = true;
			break;
		}
	}

	if (status == false) {
		if (!commandHasBeenExecuted) {
			char* format = "Could not find instruction: %c";
			int outputLen = strlen(format) + sizeof(char);
			char* msg = (char*)calloc(outputLen, sizeof(char));
			snprintf(msg, outputLen, format, commandChar);
			ERROR_MESSAGE("%s", msg);
		}
	}

	return status;
}


/*
	MISC METHODS
*/

bool IsPrintable(char value) {
	if (value >= 0x20 && value <= 0x7F) {
		return true;
	}
	return false;
}



