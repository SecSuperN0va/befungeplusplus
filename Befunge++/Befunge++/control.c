#include "control.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "srcutil.h"
#include "befunge_error.h"
#include "Commands.h"
#include "functions.h"

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
	(*settings)->hProgramIn = stdin;
	(*settings)->hProgramOut = stdout;
	return;
}

void InitialiseInstructionPointerState(PINSTRUCTION_POINTER_STATE *ipState, POSITION entrypoint) {
	int i = 0;
	*ipState = (PINSTRUCTION_POINTER_STATE)calloc(1, sizeof(INSTRUCTION_POINTER_STATE));
	(*ipState)->direction = DIR_RIGHT;
	
	for (i = 0; i < N_DIMENSIONS; i++) {
		(*ipState)->position[i] = entrypoint[i];
	}
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

void InitialiseFungeInstance(PFUNGE_INSTANCE instance, void* parent, char* programString, int tickDelay, FILE* outputFile, bool showState, bool singleStep, POSITION entrypoint, GRID_DIMENSIONS dimensions) {

	instance->parent = parent;

	InitialiseStaticControlSettings(&(instance->staticSettings), showState, singleStep, tickDelay);
	InitialiseDynamicControlSettings(&(instance->dynamicSettings));
	InitialiseInstructionPointerState(&(instance->ipState), entrypoint);
	InitialiseRegisters(instance->registers);
	InitialiseFungeStack(&(instance->stackState));
	instance->gridStruct = CreateBefungeGrid(dimensions.columns, dimensions.rows);
	instance->outputFile = outputFile;

	instance->output = (char*)calloc(DEFAULT_OUTPUT_SIZE, sizeof(char));
	instance->outputSize = 0;

	if (programString == NULL) {
		DEBUG_MESSAGE("Loaded program string from control system");
		LoadProgramString(instance, ((PBEFUNGE_CONTROL)instance->parent)->originalProgramString);
	}
	else {
		DEBUG_MESSAGE("Loaded program string from supplied string");
		LoadProgramString(instance, programString);
	}
	return;
}

void InitialiseControlSystem(PBEFUNGE_CONTROL control, char* programString, PFUNCTION_LIST functions, int tickDelay, FILE* outputFile, bool showState, PBEFUNGE_METADATA metadata, bool singleStep) {
	POSITION entrypoint;

	entrypoint[AXIS_X] = metadata->context.entrypoint.column;
	entrypoint[AXIS_Y] = metadata->context.entrypoint.row;

	control->meta = metadata;
	control->functions = functions;

	control->originalProgramString = programString;

	InitialiseInstanceList(&(control->firstInstance));
	CreateInstance(control->firstInstance);
	control->firstInstance->previous = NULL;
	InitialiseFungeInstance(((PINSTANCE_LIST)((PINSTANCE_LIST)control->firstInstance)->next)->pInstance, (void*) control, control->originalProgramString, tickDelay, outputFile, showState, singleStep, entrypoint, control->meta->dimensions);
	
	return;
}

void InitialiseInstanceList(PINSTANCE_LIST *list) {
	*list = (PINSTANCE_LIST)calloc(1, sizeof(INSTANCE_LIST));
	(*list)->next = NULL;
	(*list)->pInstance = NULL;
	(*list)->previous = NULL;
	return;
}

void CreateInstance(PINSTANCE_LIST list) {
	PINSTANCE_LIST current = list;
	while (current->next != NULL) {
		current = current->next;
	}

	current->next = (PINSTANCE_LIST)calloc(1, sizeof(INSTANCE_LIST));
	((PINSTANCE_LIST)current->next)->pInstance = (PFUNGE_INSTANCE)calloc(1, sizeof(FUNGE_INSTANCE));
	((PINSTANCE_LIST)current->next)->next = NULL;
	((PINSTANCE_LIST)current->next)->previous = current;

	return;
}

/*
	VISUALISATION METHODS
*/

void OutputString(PFUNGE_INSTANCE instance, char* str, int len) {
	if (strlen(str) < (unsigned)(instance->staticSettings->outputMaxSize - instance->outputSize)) {
		memcpy(
			instance->output + instance->outputSize,
			str,
			len
		);
		instance->outputSize += len;
		fwrite(str, 1, len, instance->dynamicSettings->hProgramOut);
		if (instance->outputFile != NULL || instance->outputFile == stdout || instance->outputFile == stderr) {
			fwrite(str, 1, len, instance->outputFile);
		}
	}
	else {
		if (instance->staticSettings->showState) {
			char* format = "Cannot output \"%s\". Not enough space in output buffer";
			int outputLen = strlen(format) + len;
			char* msg = (char*)calloc(outputLen, sizeof(char));
			snprintf(msg, outputLen, format, str);
			ERROR_MESSAGE(msg);
		}
	}
	return;
}

void OutputRegistersState(PFUNGE_INSTANCE instance) {
	fprintf(stderr, "REG_A: %08x\n", instance->registers[REG_A]);
	fprintf(stderr, "REG_B: %08x\n", instance->registers[REG_B]);
	fprintf(stderr, "REG_C: %08x\n", instance->registers[REG_C]);
	fprintf(stderr, "REG_D: %08x\n", instance->registers[REG_D]);
	return;
}

void PrintOutputState(PFUNGE_INSTANCE instance) {
	int total_required = 0;
	char* buffer = NULL;
	const char title[] = "Output:\n============\n";

	total_required += strlen(title);
	total_required += instance->outputSize;
	total_required += 1;

	buffer = (char*)calloc(total_required, sizeof(char));

	strncat(buffer, title, strlen(title));
	strncat(buffer, instance->output, instance->outputSize);

	fwrite(buffer, 1, total_required, stderr);
	return;
}

void PrintProgramState(PFUNGE_INSTANCE instance) {
	int column, row, c;
	char* formatString = " %c ";
	int total_required = 0;
	char* buffer = NULL;
	const char title[] = "Program State:\n==============\n";
	char tmpBuf[32] = { 0 };
	bool isSelected = false;

	total_required += strlen(title);
	total_required += (instance->gridStruct->rows + 2) * (instance->gridStruct->columns + 3); // For each cell
	total_required += 12; // to account for the ansi escape codes around the selected cell
	total_required += 1;

	buffer = (char*)calloc(total_required, sizeof(char));

	strncat(buffer, title, strlen(title));

	// Draw top line
	for (c = 0; c < instance->gridStruct->columns + 2; c++) {
		strncat(buffer, "_", 1);
	}
	strncat(buffer, "\n", 1);

	// Fill the grid
	for (row = 0; row < instance->gridStruct->rows; row++) {
		strncat(buffer, "|", 1);
		for (column = 0; column < instance->gridStruct->columns; column++) {
			if (column == instance->ipState->position[AXIS_X] && row == instance->ipState->position[AXIS_Y]) {
				isSelected = true;
			}
			else {
				isSelected = false;
			}
			if (0x20 <= instance->gridStruct->grid[row][column] && instance->gridStruct->grid[row][column] <= 0x80) {
				if (isSelected) 
					snprintf(tmpBuf, sizeof(tmpBuf), "%c[101;1m%c%c[%dm", 0x1B, instance->gridStruct->grid[row][column], 0x1b, 0);
				else 
					snprintf(tmpBuf, sizeof(tmpBuf), "%c", instance->gridStruct->grid[row][column]);

				//if(isSelected)
				//	strncat_s(buffer, total_required, "\e[1m", 4);
				strncat(buffer, tmpBuf, sizeof(tmpBuf));
				//if (isSelected)
				//	strncat_s(buffer, total_required, "\e[0m", 4);
				memset(tmpBuf, 0, sizeof(tmpBuf));

			}
			else {
				if (isSelected)
					snprintf(tmpBuf, sizeof(tmpBuf), "%c[101;1m.%c[%dm", 0x1B, 0x1b, 0);
				else
					snprintf(tmpBuf, sizeof(tmpBuf), ".");
				strncat(buffer, tmpBuf, sizeof(tmpBuf));
				memset(tmpBuf, 0, sizeof(tmpBuf));
			}
				

		}
		strncat(buffer, "|\n", sizeof(tmpBuf));
	}

	// Draw top line
	for (c = 0; c < instance->gridStruct->columns + 2; c++) {
		strncat(buffer, "_", 1);
	}
	strncat(buffer, "\n", 1);

	fprintf(stderr, buffer);

	return;
}

void PrintStackState(PFUNGE_INSTANCE instance) {
	int stackWindowMaxSize = 25;
	int c, d = 0;
	int total_required = 0;
	char* buffer = NULL;
	const char title[] = "Stack State:\n============\n";
	char tmpBuf[41] = { 0 };

	if (stackWindowMaxSize >= instance->stackState->stackPointer) {
		c = instance->stackState->stackPointer - 1;
	}
	else {
		c = stackWindowMaxSize;
	}

	total_required += strlen(title);
	total_required += (c * sizeof(tmpBuf)) + (sizeof(tmpBuf) * 2) + sizeof(tmpBuf);
	total_required += 1;

	buffer = (char*)calloc(total_required, sizeof(char));

	strncat(buffer, title, strlen(title));
	

	snprintf(tmpBuf, sizeof(tmpBuf), "   %d   \n", instance->stackState->stackPointer);
	strncat(buffer, tmpBuf, strlen(tmpBuf));
	memset(tmpBuf, 0, strlen(tmpBuf));

	strncat(buffer, "|------------|\n", strlen(title));

	for (; c >= 0; c--) {
		snprintf(tmpBuf, sizeof(tmpBuf), "|  %08x  | (%-6d)\n", instance->stackState->stack[c], c);
		strncat(buffer, tmpBuf, strlen(tmpBuf));
		memset(tmpBuf, 0, strlen(tmpBuf));
	}
	strncat(buffer, "|------------|\n", strlen(title));


	fprintf(stderr, buffer);
}

/*
	PRIMITIVE METHODS
*/

char GetCommand(PFUNGE_INSTANCE instance) {
	return instance->gridStruct->grid[instance->ipState->position[AXIS_Y]][instance->ipState->position[AXIS_X]];
}

void Push(PFUNGE_INSTANCE instance, STACK_ITEM_TYPE value) {
	char buf[10] = { 0 };
	instance->stackState->stack[instance->stackState->stackPointer++] = value;
	return;
}

void Pop(PFUNGE_INSTANCE instance, STACK_ITEM_TYPE* out) {
	char buf[10] = { 0 };

	if (instance->stackState->stackPointer > 0){
		STACK_ITEM_TYPE value = instance->stackState->stack[--instance->stackState->stackPointer];
		if (out != NULL) {
			*out = value;
		}
	}
	else {
		ERROR_MESSAGE("Nothing on stack to pop");
	}
	
	return;
}

void SetDirection(PFUNGE_INSTANCE instance, int direction) {
	instance->ipState->direction = direction;
}

void Put(PFUNGE_INSTANCE instance) {
	//char row, column, value;
	STACK_ITEM_TYPE row, column, value;

	// retrieve two values from the stack
	Pop(instance, &row);
	Pop(instance, &column);
	Pop(instance, &value);

	if (0 > row || row >= instance->gridStruct->rows) {
		char* rowErrorformat = "Put: Selected row out of range: %d";
		char* rowErrorStr = (char*)calloc(strlen(rowErrorformat) + 10, sizeof(char));
		snprintf(rowErrorStr, strlen(rowErrorformat) + 10, rowErrorformat, row);
		ERROR_MESSAGE(rowErrorStr);
		return;
	}

	if (0 > column || column >= instance->gridStruct->columns) {
		char* colErrorformat = "Put: Selected column out of range: %d";
		char* colErrorStr = (char*)calloc(strlen(colErrorformat) + 10, sizeof(char));
		snprintf(colErrorStr, strlen(colErrorformat) + 10, colErrorformat, column);
		ERROR_MESSAGE(colErrorStr);
		return;
	}

	instance->gridStruct->grid[row][column] = (char) value;

	return;
}

void Get(PFUNGE_INSTANCE instance) {
	STACK_ITEM_TYPE row, column;

	// retrieve two values from the stack
	Pop(instance, &row);
	Pop(instance, &column);

	if (0 > row || row >= instance->gridStruct->rows) {
		char* rowErrorformat = "Get: Selected row out of range: %d";
		char* rowErrorStr = (char*)calloc(strlen(rowErrorformat) + 10, sizeof(char));
		snprintf(rowErrorStr, strlen(rowErrorformat) + 10, rowErrorformat, row);
		ERROR_MESSAGE(rowErrorStr);
		return;
	}

	if (0 > column || column >= instance->gridStruct->columns) {
		char* colErrorformat = "Get: Selected column out of range: %d";
		char* colErrorStr = (char*)calloc(strlen(colErrorformat) + 10, sizeof(char));
		snprintf(colErrorStr, strlen(colErrorformat) + 10, colErrorformat, column);
		ERROR_MESSAGE(colErrorStr);
		return;
	}

	Push(instance, instance->gridStruct->grid[row][column]);

	return;
}

/*
	SETTING CHANGE METHODS
*/

void ToggleStringMode(PFUNGE_INSTANCE instance) {
	instance->dynamicSettings->stringMode = !instance->dynamicSettings->stringMode;
	return;
}

void FlipDirection(PFUNGE_INSTANCE instance) {
	if (instance->ipState->direction == DIR_RIGHT)
		SetDirection(instance, DIR_LEFT);
	else if (instance->ipState->direction == DIR_LEFT)
		SetDirection(instance, DIR_RIGHT);
	else if (instance->ipState->direction == DIR_UP)
		SetDirection(instance, DIR_DOWN);
	else if (instance->ipState->direction == DIR_DOWN)
		SetDirection(instance, DIR_UP);
	else
		ERROR_MESSAGE("Seriously... What the fuck?\n")

}

void SetProgramInput(PFUNGE_INSTANCE instance, FILE* hInput) {
	instance->dynamicSettings->hProgramIn = hInput;
	return;
}

void SetProgramOutput(PFUNGE_INSTANCE instance, FILE* hOutput) {
	instance->dynamicSettings->hProgramOut = hOutput;
	return;
}

void ResetProgramInput(PFUNGE_INSTANCE instance) {
	if (instance->dynamicSettings->hProgramIn != stdin && instance->dynamicSettings->hProgramIn != NULL) {
		fclose(instance->dynamicSettings->hProgramIn);
		SetProgramInput(instance, (FILE*)stdin);
	}
	else {
		ERROR_MESSAGE("Cannot close/reset program input. Either already reset or double close attempted.")
	}
	return;
}

void ResetProgramOutput(PFUNGE_INSTANCE instance) {
	if (instance->dynamicSettings->hProgramOut != stdout && instance->dynamicSettings->hProgramOut != NULL) {
		fclose(instance->dynamicSettings->hProgramOut);
		SetProgramOutput(instance, (FILE*)stdout);
	}
	else {
		ERROR_MESSAGE("Cannot close/reset program output. Either already reset or double close attempted.")
	}
	return;
}

/*
	CONTROL METHODS
*/

void LoadProgramString(PFUNGE_INSTANCE instance, char* programString) {
	int columnCount = 0;
	int rowCount = 0;
	unsigned int ptr = 0;
	char c = '\0';
	int i, j = 0;
	// Fill the entire grid with NOPS
	for (i = 0; i < instance->gridStruct->rows; i++) {
		for (j = 0; j < instance->gridStruct->columns; j++) {
			instance->gridStruct->grid[i][j] = 0x20;
		}
	}

	while (ptr < strlen(programString)) {
		if ((ptr < strlen(programString) - 1
			&& (programString[ptr] == '\r' && programString[ptr + 1] == '\n'))
			|| (programString[ptr] == '\r' || programString[ptr] == '\n')
			|| columnCount == instance->gridStruct->columns) {
			/*while (programString[ptr] == '\r' || programString[ptr] == '\n') {
				ptr++;
			}*/ 
			ptr++;
			rowCount++;
			columnCount = 0;
			continue;
		}
		c = programString[ptr++];
		instance->gridStruct->grid[rowCount][columnCount++] = c;
	}

	return;
}

void TakeStep(PFUNGE_INSTANCE instance) {
	switch (instance->ipState->direction) {
	case DIR_RIGHT:
		instance->ipState->position[AXIS_X]++;
		if (instance->ipState->position[AXIS_X] == instance->gridStruct->columns) {
			instance->ipState->position[AXIS_X] = 0;
		}
		break;
	case DIR_LEFT:
		instance->ipState->position[AXIS_X]--;
		if (instance->ipState->position[AXIS_X] == -1) {
			instance->ipState->position[AXIS_X] = instance->gridStruct->columns -1;
		}
		break;
	case DIR_UP:
		instance->ipState->position[AXIS_Y]--;
		if (instance->ipState->position[AXIS_Y] == -1) {
			instance->ipState->position[AXIS_Y] = instance->gridStruct->rows - 1;
		}
		break;
	case DIR_DOWN:
		instance->ipState->position[AXIS_Y]++;
		if (instance->ipState->position[AXIS_Y] == instance->gridStruct->rows) {
			instance->ipState->position[AXIS_Y] = 0;
		}
		break;
	default:
		ERROR_MESSAGE("How the fuck did this happen?");
		break;
	}
}

void BackStep(PFUNGE_INSTANCE instance) {
	int origDirection = instance->ipState->direction;
	int backStepDirection = NULL;

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

	SetDirection(instance, backStepDirection);
	TakeStep(instance, backStepDirection);
	SetDirection(instance, origDirection);

	return;
}

int ProcessTick(PFUNGE_INSTANCE instance) {
	int retval = STATUS_OK;
	bool commandStatus = false;
	char command = GetCommand(instance);
	
	if (command != CMD_END) {
		if (instance->dynamicSettings->stringMode) {
			// If the current context is string mode process the command as if it were ASCII, unless command is " char
			if (command == CMD_STRING) {
				ToggleStringMode(instance);
			}
			else {
				Push(instance, (STACK_ITEM_TYPE)command);
			}
			commandStatus = true;
		}
		else if (command >= 0x30 && command <=0x39) {
			Push(instance, (STACK_ITEM_TYPE)(command - 0x30));
			commandStatus = true;
		}
		else {
			if (commandStatus = AcceptCommand(instance, command)) {
				// Command success
				retval = STATUS_OK;
			}
			else {
				retval = STATUS_BAD_COMMAND_RESULT;
			}
		}
	}
	else {
		retval = STATUS_TERMINATED;
	}

	return retval;
}

bool AcceptCommand(PFUNGE_INSTANCE instance, char commandChar) {
	int counter = 0;
	int commandOrdinal = 0;
	bool status = false;
	bool commandHasBeenExecuted = false;

	for (counter = 0; counter < MAX_ORD; counter++) {
		if (commandChar == commandCharLookup[counter]) {
			// Command Character matches, we can process the command
			commandOrdinal = counter;
			status = commandTable[commandOrdinal](instance);
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
			ERROR_MESSAGE(msg);
		}
		else {
			ERROR_MESSAGE("Instruction appears to have failed (based on return value)");
		}
	}

	return status;
}

bool HasActiveInstances(PBEFUNGE_CONTROL control) {
	if (control->firstInstance->pInstance != NULL || control->firstInstance->next != NULL) {
		return true;
	}
	return false;
}

void RegisterInstanceTermination(PFUNGE_INSTANCE pInstance) {

	PINSTANCE_LIST current = ((PBEFUNGE_CONTROL)pInstance->parent)->firstInstance;
	bool success = false;
	do {
		if (current->pInstance == pInstance) {
			// Found the instance that has requested termination! reasign links in list to remove current entry
			free(pInstance);
			current->pInstance = NULL;
			if (current->previous != NULL) {
				((PINSTANCE_LIST)current->previous)->next = current->next;
			}
			else {
				if (current->next != NULL) {
					((PINSTANCE_LIST)current->next)->previous = NULL;
				}
			}
			if (current->next != NULL) {
				((PINSTANCE_LIST)current->next)->previous = current->previous;
			}
			else {
				if (current->previous != NULL) {
					((PINSTANCE_LIST)current->previous)->next = NULL;
				}
			}
			success = true;
			break;
		}
		current = current->next;
	} while (current != NULL);


	if (success) {
		INFO_MESSAGE("Successfully terminated and removed instance from control list");
	}
	else {
		ERROR_MESSAGE("Failed to remove instance from control list");
	}
	return;
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



