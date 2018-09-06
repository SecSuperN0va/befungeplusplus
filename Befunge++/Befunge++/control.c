#include "control.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "srcutil.h"

void InitialiseControlSystem(PBEFUNGE_CONTROL control, char* programString, int tickDelay, FILE* outputFile, bool showState, PBEFUNGE_METADATA metadata) {
	control->meta = metadata;
	
	control->tickDelay = tickDelay;
	
	control->direction = DIR_RIGHT;
	control->position[AXIS_X] = metadata->context.entrypoint.column;
	control->position[AXIS_Y] = metadata->context.entrypoint.row;

	control->stack = (char*)calloc(DEFAULT_STACK_SIZE, sizeof(char));
	control->stackPointer = 0;

	control->outputFile = outputFile;

	control->output = (char*)calloc(DEFAULT_OUTPUT_SIZE, sizeof(char));
	control->outputMaxSize = DEFAULT_OUTPUT_SIZE;
	control->outputSize = 0;

	control->showState = showState;

	int gridColumns = control->meta->dimensions.columns;
	int gridRows = control->meta->dimensions.rows;

	control->gridStruct = CreateBefungeGrid(gridColumns, gridRows);

	LoadProgramString(control, programString);

	//scrSetColors(scrGreen, scrBlack);

	return;
}

void OutputString(PBEFUNGE_CONTROL control, char* str, int len) {
	if (strlen(str) < (unsigned)(control->outputMaxSize - control->outputSize)) {
		memcpy(
			control->output + control->outputSize,
			str,
			len
		);
		control->outputSize += len;
		fprintf(stdout, str);
		if (control->outputFile != NULL) {
			fwrite(str, 1, len, control->outputFile);
		}
	}
	else {
		if(control->showState) fprintf(stderr, "Cannot output \"%s\". Not enough space in output buffer", str);
	}
	return;
}

void PrintOutputState(PBEFUNGE_CONTROL control) {
	int total_required = 0;
	char* buffer = NULL;
	const char title[] = "Output:\n============\n";

	total_required += strlen(title);
	total_required += control->outputSize;
	total_required += 1;

	buffer = (char*)calloc(total_required, sizeof(char));

	strncat(buffer, title, strlen(title));
	strncat(buffer, control->output, control->outputSize);

	fprintf(stderr, buffer);
	return;
}

void LoadProgramString(PBEFUNGE_CONTROL control, char* programString) {
	int columnCount = 0;
	int rowCount = 0;
	unsigned int ptr = 0;
	char c = '\0';
	int i, j = 0;
	// Fill the entire grid with NOPS
	for (i = 0; i < control->gridStruct->rows; i++) {
		for (j = 0; j < control->gridStruct->columns; j++) {
			control->gridStruct->grid[i][j] = 0x20;
		}
	}


	while (ptr < strlen(programString)) {
		if ((ptr < strlen(programString) - 1
			&& (programString[ptr] == '\r' && programString[ptr + 1] == '\n'))
			|| (programString[ptr] == '\r' || programString[ptr] == '\n')
			|| columnCount == control->gridStruct->columns) {
			while (programString[ptr] == '\r' || programString[ptr] == '\n') {
				ptr++;
			} 
			rowCount++;
			columnCount = 0;
			continue;
		}
		/*if (programstring[ptr] == '\r' || programstring[ptr] == '\n' || columncount == control->gridstruct->columns) {
			ptr++;
			rowcount++;
			columncount = 0;
			continue;
		}*/
		c = programString[ptr++];
		control->gridStruct->grid[rowCount][columnCount++] = c;
	}

	return;
}

void PrintProgramState(PBEFUNGE_CONTROL control) {
	int column, row, c;
	char* formatString = " %c ";
	int total_required = 0;
	char* buffer = NULL;
	const char title[] = "Program State:\n==============\n";
	char tmpBuf[32] = { 0 };
	bool isSelected = false;

	total_required += strlen(title);
	total_required += (control->meta->dimensions.rows + 2) * (control->meta->dimensions.columns + 3); // For each cell
	total_required += 12; // to account for the ansi escape codes around the selected cell
	total_required += 1;

	buffer = (char*)calloc(total_required, sizeof(char));

	strncat(buffer, title, strlen(title));

	// Draw top line
	for (c = 0; c < control->gridStruct->columns + 2; c++) {
		strncat(buffer, "_", 1);
	}
	strncat(buffer, "\n", 1);

	// Fill the grid
	for (row = 0; row < control->gridStruct->rows; row++) {
		strncat(buffer, "|", 1);
		for (column = 0; column < control->gridStruct->columns; column++) {
			if (column == control->position[AXIS_X] && row == control->position[AXIS_Y]) {
				isSelected = true;
			}
			else {
				isSelected = false;
			}
			if (0x20 <= control->gridStruct->grid[row][column] && control->gridStruct->grid[row][column] <= 0x80) {
				if (isSelected) 
					snprintf(tmpBuf, sizeof(tmpBuf), "%c[101;1m%c%c[%dm", 0x1B, control->gridStruct->grid[row][column], 0x1b, 0);
				else 
					snprintf(tmpBuf, sizeof(tmpBuf), "%c", control->gridStruct->grid[row][column]);

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
	for (c = 0; c < control->gridStruct->columns + 2; c++) {
		strncat(buffer, "_", 1);
	}
	strncat(buffer, "\n", 1);

	fprintf(stderr, buffer);

	return;
}

void PrintStackState(PBEFUNGE_CONTROL control) {
	int stackWindowMaxSize = 25;
	int c, d = 0;
	int total_required = 0;
	char* buffer = NULL;
	const char title[] = "Stack State:\n============\n";
	char tmpBuf[20] = { 0 };

	if (stackWindowMaxSize >= control->stackPointer) {
		c = control->stackPointer;
	}
	else {
		c = stackWindowMaxSize;
	}

	total_required += strlen(title);
	total_required += (c * sizeof(tmpBuf)) + (sizeof(tmpBuf) * 2) + sizeof(tmpBuf);
	total_required += 1;

	buffer = (char*)calloc(total_required, sizeof(char));

	strncat(buffer, title, strlen(title));
	

	snprintf(tmpBuf, sizeof(tmpBuf), "   %d   \n", control->stackPointer);
	strncat(buffer, tmpBuf, strlen(tmpBuf));
	memset(tmpBuf, 0, strlen(tmpBuf));

	strncat(buffer, "|------|\n", strlen(title));

	for (; c != 0; c--) {
		snprintf(tmpBuf, sizeof(tmpBuf), "|  %02x  |\n", control->stack[c]);
		strncat(buffer, tmpBuf, strlen(tmpBuf));
		memset(tmpBuf, 0, strlen(tmpBuf));
	}
	strncat(buffer, "|------|\n", strlen(title));


	fprintf(stderr, buffer);
}

bool IsPrintable(char value) {
	if (value >= 0x20 && value <= 0x7F) {
		return true;
	}
	return false;
}

void FormatPrintableValue(char value, char* buf, size_t bufSize) {
	if (IsPrintable(value)) {
		snprintf(buf, bufSize, "'%c'", value);
	}
	else {
		snprintf(buf, bufSize, "0x%02x", value);
	}
	return;
}

void Push(PBEFUNGE_CONTROL control, char value) {
	char buf[10] = { 0 };
	FormatPrintableValue(value, buf, sizeof(buf));
	if (control->showState) fprintf(stderr, "Push(%s)\n", buf);
	control->stack[control->stackPointer++] = value;
	return;
}

void Pop(PBEFUNGE_CONTROL control, char* out) {
	char buf[10] = { 0 };

	if (control->stackPointer > 0){
		char value = control->stack[--control->stackPointer];
		FormatPrintableValue(value, buf, sizeof(buf));
		if (control->showState) fprintf(stderr, "Pop(%s)\n", buf);
		if (out != NULL) {
			*out = value;
		}
	}
	else {
		fprintf(stderr, "Nothing on stack to pop\n");
	}
	
	return;
}

char GetCommand(PBEFUNGE_CONTROL control) {
	return control->gridStruct->grid[control->position[AXIS_Y]][control->position[AXIS_X]];
}

void TakeStep(PBEFUNGE_CONTROL control) {
	switch (control->direction) {
	case DIR_RIGHT:
		control->position[AXIS_X]++;
		if (control->position[AXIS_X] == control->gridStruct->columns) {
			control->position[AXIS_X] = 0;
		}
		break;
	case DIR_LEFT:
		control->position[AXIS_X]--;
		if (control->position[AXIS_X] == -1) {
			control->position[AXIS_X] = control->gridStruct->columns -1;
		}
		break;
	case DIR_UP:
		control->position[AXIS_Y]--;
		if (control->position[AXIS_Y] == -1) {
			control->position[AXIS_Y] = control->gridStruct->rows - 1;
		}
		break;
	case DIR_DOWN:
		control->position[AXIS_Y]++;
		if (control->position[AXIS_Y] == control->gridStruct->rows) {
			control->position[AXIS_Y] = 0;
		}
		break;
	default:
		fprintf(stderr, "How the fuck did this happen?\n");
		break;
	}
}

void SetDirection(PBEFUNGE_CONTROL control, int direction) {
	if (control->showState) fprintf(stderr, "Setting direction: %d", direction);
	control->direction = direction;
}

void FlipDirection(PBEFUNGE_CONTROL control) {
	if (control->direction == DIR_RIGHT)
		SetDirection(control, DIR_LEFT);
	else if (control->direction == DIR_LEFT)
		SetDirection(control, DIR_RIGHT);
	else if (control->direction == DIR_UP)
		SetDirection(control, DIR_DOWN);
	else if (control->direction == DIR_DOWN)
		SetDirection(control, DIR_UP);
	else
		fprintf(stderr, "Seriously... What the fuck?\n");

}

void Put(PBEFUNGE_CONTROL control) {
	char row, column, value;

	// retrieve two values from the stack
	Pop(control, &row);
	Pop(control, &column);
	Pop(control, &value);

	if (0 > row >= control->gridStruct->rows) {
		fprintf(stderr, "Put: Selected row out of range: %d", row);
		return;
	}

	if (0 > column >= control->gridStruct->columns) {
		fprintf(stderr, "Put: Selected column out of range: %d", column);
		return;
	}

	control->gridStruct->grid[row][column] = value;

	return;
}

void Get(PBEFUNGE_CONTROL control) {
	char row, column;

	// retrieve two values from the stack
	Pop(control, &row);
	Pop(control, &column);

	if (0 > row >= control->gridStruct->rows) {
		fprintf(stderr, "Put: Selected row out of range: %d", row);
		return;
	}

	if (0 > column >= control->gridStruct->columns) {
		fprintf(stderr, "Put: Selected column out of range: %d", column);
		return;
	}

	Push(control, control->gridStruct->grid[row][column]);

	return;
}