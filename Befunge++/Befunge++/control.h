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

#define STACK_ITEM_TYPE signed long int

#define STATUS_OK 0
#define STATUS_TERMINATED -1
#define STATUS_BAD_COMMAND_RESULT -2

#define REG_A 0
#define REG_B 1
#define REG_C 2
#define REG_D 3

#define REGISTER_COUNT 4

typedef int POSITION[N_DIMENSIONS];
typedef STACK_ITEM_TYPE REGISTERS[REGISTER_COUNT];

typedef struct StaticControlSettings {
	bool singleStepMode;
	bool showState;
	int outputMaxSize;
	int tickDelay;
} STATIC_CONTROL_SETTINGS, *PSTATIC_CONTROL_SETTINGS;

typedef struct DynamicControlSettings {
	bool stringMode;

	// FILE_ACCESS variables
	FILE* hProgramOut;
	FILE* hProgramIn;
} DYNAMIC_CONTROL_SETTINGS, *PDYNAMIC_CONTROL_SETTINGS;

typedef struct InstructionPointerState {
	int direction;
	POSITION position;
} INSTRUCTION_POINTER_STATE, *PINSTRUCTION_POINTER_STATE;

typedef struct FungeStackState {
	STACK_ITEM_TYPE *stack;
	int stackPointer;
} FUNGE_STACK_STATE, *PFUNGE_STACK_STATE;

typedef struct FungeInstance {
	PSTATIC_CONTROL_SETTINGS staticSettings;
	PDYNAMIC_CONTROL_SETTINGS dynamicSettings;
	PINSTRUCTION_POINTER_STATE ipState;
	PFUNGE_STACK_STATE stackState;
	PBEFUNGE_GRID gridStruct;
	REGISTERS registers;
	char* output;
	int outputSize;
	FILE* outputFile;
	void* parent;
} FUNGE_INSTANCE, *PFUNGE_INSTANCE;

typedef struct InstanceList {
	void* previous;
	void* pInstance;
	void* next;
} INSTANCE_LIST, *PINSTANCE_LIST;

void InitialiseFungeInstance(PFUNGE_INSTANCE instance, void* parent, char* programString, int tickDelay, FILE* outputFile, bool showState, bool singleStep, POSITION entrypoint, GRID_DIMENSIONS dimensions);

typedef struct BefungeControl {
	PBEFUNGE_METADATA meta;
	PINSTANCE_LIST firstInstance;
	int nInstances;
} BEFUNGE_CONTROL, *PBEFUNGE_CONTROL;

/*
	POPULATION METHODS
*/

void RunPopulation();

/*
	INITIALISATION METHODS
*/
void InitialiseStaticControlSettings(PSTATIC_CONTROL_SETTINGS *settings, bool showState, bool singleStep, int tickDelay);
void InitialiseDynamicControlSettings(PDYNAMIC_CONTROL_SETTINGS *settings);
void InitialiseInstructionPointerState(PINSTRUCTION_POINTER_STATE *ipState, POSITION entrypoint);
void InitialiseRegisters(REGISTERS registers);
void InitialiseFungeStack(PFUNGE_STACK_STATE *stackStruct);
void InitialiseControlSystem(PBEFUNGE_CONTROL control, char* programString, int tickDelay, FILE* outputFile, bool showState, PBEFUNGE_METADATA metadata, bool singleStep);
void InitialiseInstanceList(PINSTANCE_LIST *list);
void CreateInstance(PINSTANCE_LIST list);

/*
	VISUALISATION METHODS
*/
void OutputString(PFUNGE_INSTANCE instance, char* str, int len);
void OutputRegistersState(PFUNGE_INSTANCE instance);
void PrintOutputState(PFUNGE_INSTANCE instance);
void PrintProgramState(PFUNGE_INSTANCE instance);
void PrintStackState(PFUNGE_INSTANCE instance);


/*
	PRIMITIVE METHODS
*/
char GetCommand(PFUNGE_INSTANCE instance);
void Push(PFUNGE_INSTANCE instance, STACK_ITEM_TYPE value);
void Pop(PFUNGE_INSTANCE instance, STACK_ITEM_TYPE* out);
void SetDirection(PFUNGE_INSTANCE instance, int direction);
void Put(PFUNGE_INSTANCE instance);
void Get(PFUNGE_INSTANCE instance);


/*
	SETTING CHANGE METHODS
*/
void ToggleStringMode(PFUNGE_INSTANCE instance);
void FlipDirection(PFUNGE_INSTANCE instance);
void SetProgramInput(PFUNGE_INSTANCE instance, FILE* hInput);
void SetProgramOutput(PFUNGE_INSTANCE instance, FILE* hOutput);
void ResetProgramInput(PFUNGE_INSTANCE instance);
void ResetProgramOutput(PFUNGE_INSTANCE instance);

/*
	CONTROL METHODS
*/
void LoadProgramString(PFUNGE_INSTANCE instance, char* programString);
void TakeStep(PFUNGE_INSTANCE instance);
void BackStep(PFUNGE_INSTANCE instance);
int ProcessTick(PFUNGE_INSTANCE instance);
bool AcceptCommand(PFUNGE_INSTANCE instance, char commandChar);

bool HasActiveInstances(PBEFUNGE_CONTROL control);
void RegisterInstanceTermination(PFUNGE_INSTANCE pInstance);

/*
	MISC METHODS
*/
bool IsPrintable(char value);
















