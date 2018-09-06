#pragma once

typedef struct BefungeGrid {
	char** grid;
	int columns;
	int rows;
} BEFUNGE_GRID, *PBEFUNGE_GRID;

char** createEmptyGrid(int columns, int rows);

void freeGrid(char** grid, int rows);

PBEFUNGE_GRID CreateBefungeGrid(int columns, int rows);

void populateGrid(PBEFUNGE_GRID grid);