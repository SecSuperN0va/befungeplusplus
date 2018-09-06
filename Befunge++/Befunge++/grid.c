#include "grid.h"
#include <stdlib.h>

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


void populateGrid(PBEFUNGE_GRID grid) {
	int i, j, counter = 0;

	for (j = 0; j < grid->rows; j++) {
		for (i = 0; i < grid->columns; i++) {
			grid->grid[i][j] = counter++;
		}
	}
	return;
}