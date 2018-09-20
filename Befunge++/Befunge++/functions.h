#pragma once
#include "meta.h"

typedef struct FunctionDefinition {
	int id;
	int argumentCount; 
	GRID_POINT entrypoint;
} FUNCTION_DEFINITION, *PFUNCTION_DEFINITION;

typedef struct FunctionList {
	void* previous;
	void* pFunction;
	void* next;
} FUNCTION_LIST, *PFUNCTION_LIST;

void InitialiseFunctionDefinition(PFUNCTION_DEFINITION *functionDefinition);

PFUNCTION_DEFINITION LoadFunctionDefinition(FILE* fp);

PFUNCTION_LIST LoadAllFunctionDefinitions(FILE* fp);