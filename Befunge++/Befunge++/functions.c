#include "functions.h"
#include "configparser.h"
#include <string.h>
#include <stdlib.h>
#include "befunge_error.h"

void InitialiseFunctionDefinition(PFUNCTION_DEFINITION *functionDefinition) {
	PFUNCTION_DEFINITION definition = (PFUNCTION_DEFINITION)calloc(1, sizeof(FUNCTION_DEFINITION));
	definition->id = -1;
	definition->argumentCount = 0;
	definition->entrypoint.row = DEFAULT_ENTRYPOINT_ROW;
	definition->entrypoint.column = DEFAULT_ENTRYPOINT_COLUMN;

	*functionDefinition = definition;
	return;
}

void InitialiseFunctionList(PFUNCTION_LIST *list) {
	*list = (PFUNCTION_LIST)calloc(1, sizeof(FUNCTION_LIST));
	(*list)->next = NULL;
	(*list)->pFunction = NULL;
	(*list)->previous = NULL;
	return;
}

PFUNCTION_DEFINITION LoadFunctionDefinition(FILE* fp) {
	config_option_t config;
	PFUNCTION_DEFINITION definition = 0;

	InitialiseFunctionDefinition(&definition);
	
	if (fp != NULL) {
		if ((config = ReadConfigSectionOfFile(fp)) != NULL) {
			// Program contains functions
			do {
				if (!strcmp("function.id", config->key)) {
					definition->id = atoi(config->value);
				}
				if (!strcmp("function.entrypoint.row", config->key)) {
					definition->entrypoint.row = atoi(config->value);
				}
				if (!strcmp("function.entrypoint.column", config->key)) {
					definition->entrypoint.column = atoi(config->value);
				}
				if (!strcmp("function.arguments.count", config->key)) {
					definition->argumentCount = atoi(config->value);
				}

				config = config->prev;
			} while (config != NULL);
		}
		else {
			// Program does not contain functions
			if (definition)
				free(definition);
			definition = 0;
		}
	}
	else {
		ERROR_MESSAGE("No file handle provided. No functions present.\n");
		if (definition)
			free(definition);
		definition = 0;
	}
	return definition;
}

PFUNCTION_LIST LoadAllFunctionDefinitions(FILE* fp) {
	PFUNCTION_LIST list, firstEntry, newEntry;
	PFUNCTION_DEFINITION definition;

	InitialiseFunctionList(&firstEntry);
	list = firstEntry;

	while ((definition = LoadFunctionDefinition(fp)) != NULL) {
		newEntry = (PFUNCTION_LIST)calloc(1, sizeof(FUNCTION_LIST));
		newEntry->previous = firstEntry;
		newEntry->next = NULL;
		newEntry->pFunction = definition;
		firstEntry->next = newEntry;

		firstEntry = newEntry;
	}

	return list;
}