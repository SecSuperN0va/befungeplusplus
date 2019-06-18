#include "grid.h"
#include "befunge_error.h"
#include <stdlib.h>
#include <string.h>

char** createEmptyGrid(int columns, int rows) {
	int i;
	char** grid;

	grid = (char**)calloc(rows, sizeof(char*));

	for (i = 0; i < rows; i++) {
		grid[i] = (char*)calloc(columns, sizeof(char));
	}

	return grid;
}

void freeGrid(char** grid, int rows) {
	int i;

	for (i = 0; i < rows; i++) {
		free(grid[i]);
	}
	return;
}

PBEFUNGE_GRID CreateBefungeGrid(int columns, int rows) {
	PBEFUNGE_GRID gridStruct;

	gridStruct = (PBEFUNGE_GRID)calloc(1, sizeof(BEFUNGE_GRID));

	gridStruct->columns = columns;
	gridStruct->rows = rows;
	gridStruct->grid = createEmptyGrid(gridStruct->columns, gridStruct->rows);

	return gridStruct;
}

void PopulateGridWithProgram(PBEFUNGE_GRID grid, char* programString) {
	int columnCount = 0;
	int rowCount = 0;
	unsigned int ptr = 0;
	char c = '\0';
	int i, j = 0;
	// Fill the entire grid with NOPS
	for (i = 0; i < grid->rows; i++) {
		for (j = 0; j < grid->columns; j++) {
			grid->grid[i][j] = 0x20;
		}
	}

	while (ptr < strlen(programString)) {
		if (rowCount >= grid->rows || columnCount >= grid->columns) {
			ERROR_MESSAGE("Incorrect dimensions supplied, too many rows or columns for grid.")
			break;
		}
		if ((ptr < strlen(programString) - 1
			&& (programString[ptr] == '\r' && programString[ptr + 1] == '\n'))
			|| (programString[ptr] == '\r' || programString[ptr] == '\n')
			|| columnCount == grid->columns) {
			if (programString[ptr] == '\r' && programString[ptr + 1] == '\n') {
				ptr++;
			}
			ptr++;
			rowCount++;
			columnCount = 0;
			continue;
		}
		c = programString[ptr++];
		grid->grid[rowCount][columnCount++] = c;
	}

	return;
}