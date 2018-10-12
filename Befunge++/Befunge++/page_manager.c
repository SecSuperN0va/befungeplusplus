#include "page_manager.h"
#include "configparser.h"
#include "grid.h"
#include <string.h>
#include <stdlib.h>
#include "befunge_error.h"

void InitialisePageControl(PPAGE_CONTROL *pageControl) {
	PPAGE_CONTROL control = (PPAGE_CONTROL)calloc(1, sizeof(PAGE_CONTROL));
	control->id = -1;
	control->dimensions.rows = 0;
	control->dimensions.columns = 0;
	control->grid = NULL;

	*pageControl = control;
}

void InitialisePageControlList(PPAGE_CONTROL_LIST *list) {
	*list = (PPAGE_CONTROL_LIST)calloc(1, sizeof(PAGE_CONTROL_LIST));
	(*list)->next = NULL;
	(*list)->pPageControl = NULL;
	(*list)->previous = NULL;
	return;
}

size_t GetNextPageStartOffset(FILE* fp) {
	size_t pageOffset = -1;
	size_t maxOffset = -1;
	size_t tmpOffset = -1;
	char pageTag[] = "[PAGE]";
	char* tmp = calloc(sizeof(pageTag), sizeof(char));
	size_t origPos = ftell(fp);

	// Calculate size
	fseek(fp, 0L, SEEK_END);
	maxOffset = ftell(fp);
	fseek(fp, origPos, SEEK_SET);


	while (ftell(fp) < maxOffset) {
		tmpOffset = ftell(fp);
		fread(tmp, sizeof(char), sizeof(pageTag), fp);
		if (!strncmp(pageTag, tmp, sizeof(pageTag) -1)) {
			pageOffset = tmpOffset;
			fseek(fp, pageOffset, SEEK_SET);
			break;
		}
		else {
			pageOffset = -1;
		}
		fseek(fp, 1 - sizeof(pageTag), SEEK_CUR);
	}

	fseek(fp, origPos, SEEK_SET);

	return pageOffset;
}

PPAGE_CONTROL LoadSinglePage(FILE* fp) {
	config_option_t config;
	PPAGE_CONTROL control = 0;
	char* programString = NULL;
	size_t pageSize = 0;
	size_t nextPageOffset = -1;

	InitialisePageControl(&control);

	if (fp != NULL) {
		if ((config = ReadConfigSectionOfFile(fp)) != NULL) {
			// Program contains functions
			do {
				if (!strcmp("page.id", config->key)) {
					control->id = atoi(config->value);
				}
				if (!strcmp("page.dimensions.rows", config->key)) {
					control->dimensions.rows = atoi(config->value);
				}
				if (!strcmp("page.dimensions.columns", config->key)) {
					control->dimensions.columns = atoi(config->value);
				}
				if (!strcmp("page.name", config->key)) {
					control->name = (char*)calloc(1, strlen(config->value));
					memcpy(control->name, config->value, strlen(config->value));
				}

				config = config->prev;
			} while (config != NULL);
		}
		else {
			// Program does not contain functions
			if (control)
				free(control);
			control = 0;
		}
	}
	else {
		ERROR_MESSAGE("No file handle provided. No pages present.\n");
		if (control)
			free(control);
		control = 0;
	}

	if (control) {
		// Allocate the necessary buffer
		nextPageOffset = GetNextPageStartOffset(fp);
		if (nextPageOffset != -1) {
			pageSize = nextPageOffset - ftell(fp);
		}
		else {
			// Identify the remaining file size so we can allocate a buffer of the correct size
			size_t origPos = ftell(fp);
			fseek(fp, 0L, SEEK_END);
			pageSize = ftell(fp) - origPos;
			fseek(fp, origPos, SEEK_SET);
		}

		programString = (char*)calloc(1, pageSize * sizeof(char) + 2);
		size_t a = fread(programString, 1, pageSize, fp);

		int ptr = strlen(programString);
		while (programString[ptr - 1] == '\r' || programString[ptr - 1] == '\n') {
			programString[ptr - 1] = '\0';
			ptr--;
		}

		control->grid = CreateBefungeGrid(control->dimensions.columns, control->dimensions.rows);
		PopulateGridWithProgram(control->grid, programString);
		DEBUG_MESSAGE("Loaded program into page...");
	}

	return control;
}

PPAGE_CONTROL_LIST LoadAllPages(FILE* fp) {
	PPAGE_CONTROL_LIST list, firstEntry, newEntry;
	PPAGE_CONTROL control;
	size_t pageOffset = -1;

	InitialisePageControlList(&firstEntry);
	list = firstEntry;

	while (pageOffset = GetNextPageStartOffset(fp), pageOffset != -1, fseek(fp, pageOffset, SEEK_SET), (control = LoadSinglePage(fp)) != NULL) {
		newEntry = (PPAGE_CONTROL_LIST)calloc(1, sizeof(PAGE_CONTROL_LIST));
		newEntry->previous = firstEntry;
		newEntry->next = NULL;
		newEntry->pPageControl = control;
		firstEntry->next = newEntry;

		firstEntry = newEntry;
	}

	return list;
}