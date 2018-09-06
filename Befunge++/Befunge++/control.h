#pragma once
#include <stdio.h>
#include "grid.h"
#include "meta.h"
#include <stdbool.h>


#define DIR_RIGHT 0
#define DIR_LEFT 1
#define DIR_UP 2
#define DIR_DOWN 3

#define AXIS_X 0
#define AXIS_Y 1

#define N_DIMENSIONS 2

#define DEFAULT_STACK_SIZE 2048
#define DEFAULT_OUTPUT_SIZE 4096


typedef struct BefungeControl {
	PBEFUNGE_METADATA meta;
	int tickDelay;
	int direction;
	int position[N_DIMENSIONS];
	char* stack;
	char* output;
	int stackPointer;
	int outputSize;
	int outputMaxSize;
	bool showState;
	FILE* outputFile;
	PBEFUNGE_GRID gridStruct;
} BEFUNGE_CONTROL, *PBEFUNGE_CONTROL;


void InitialiseControlSystem(PBEFUNGE_CONTROL control, char* programString, int tickDelay, FILE* outputFile, bool showState, PBEFUNGE_METADATA metadata);

void OutputString(PBEFUNGE_CONTROL control, char* str, int len);

void PrintOutputState(PBEFUNGE_CONTROL control);

void LoadProgramString(PBEFUNGE_CONTROL control, char* programString);

void PrintProgramState(PBEFUNGE_CONTROL control);

void PrintStackState(PBEFUNGE_CONTROL control);

void Push(PBEFUNGE_CONTROL control, char value);

void Pop(PBEFUNGE_CONTROL control, char* out);

char GetCommand(PBEFUNGE_CONTROL control);

void TakeStep(PBEFUNGE_CONTROL control);

void SetDirection(PBEFUNGE_CONTROL control, int direction);

void FlipDirection(PBEFUNGE_CONTROL control);

void Put(PBEFUNGE_CONTROL control);

void Get(PBEFUNGE_CONTROL control);