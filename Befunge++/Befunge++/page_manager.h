#pragma once
#include "stdio.h"
#include "grid.h"
#include "meta.h"

typedef struct PageControl {
	int id;
	GRID_DIMENSIONS dimensions;
	PBEFUNGE_GRID grid;
	char* name;

} PAGE_CONTROL, *PPAGE_CONTROL;

typedef struct PageControlList {
	void* previous;
	void* pPageControl;
	void* next;
} PAGE_CONTROL_LIST, *PPAGE_CONTROL_LIST;

void InitialisePageControl(PPAGE_CONTROL *pageControl);

PPAGE_CONTROL LoadSinglePage(FILE *fp);

PPAGE_CONTROL_LIST LoadAllPages(FILE *fp);