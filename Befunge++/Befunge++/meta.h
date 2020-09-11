#pragma once
#include <stdio.h>

#define DEFAULT_ENTRYPOINT_ROW 0
#define DEFAULT_ENTRYPOINT_COLUMN 0

#define DEFAULT_DIMENSIONS_ROWS 25
#define DEFAULT_DIMENSIONS_COLUMNS 80

#define DEFAULT_INITIAL_PAGE_ID 0


typedef struct GridPoint {
	int row;
	int column;
} GRID_POINT, *PGRID_POINT;

#define DEFAULT_ENTRY_POINT GRID_POINT{0, 0};


typedef struct GridDimensions {
	int rows;
	int columns;
} GRID_DIMENSIONS, *PGRID_DIMENSIONS;

typedef struct BefungeContext {
	GRID_POINT entrypoint;	// Point on the grid to start execution from
	int threadCount;		// Number of concurrent threads to run across program
	int threadHeadStart;	// Number of ticks between individual thread starts
	int initialPageId;		// ID of the page to start execution from
} BEFUNGE_CONTEXT, *PBEFUNGE_CONTEXT;

typedef struct BefungeMetadata {
	BEFUNGE_CONTEXT context;
} BEFUNGE_METADATA, *PBEFUNGE_METADATA;

void InitialiseProgramMetadataDefaults(PBEFUNGE_METADATA *metadata);

PBEFUNGE_METADATA LoadProgramMetadata(FILE *fp);