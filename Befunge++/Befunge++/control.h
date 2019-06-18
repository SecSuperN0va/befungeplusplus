#pragma once
#include <stdio.h>
#include "grid.h"
#include "meta.h"
#include "page_manager.h"
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
#define STATUS_CALLED -3

#define REG_A 0
#define REG_B 1
#define REG_C 2
#define REG_D 3

#define REGISTER_COUNT 4

typedef STACK_ITEM_TYPE REGISTERS[REGISTER_COUNT];

/*
	CORE STRUCTURES
*/

typedef enum BefungeMode {
	MODE_STANDARD = 0,
	MODE_PAGED,
	MODE_FUNCTIONS,
} BEFUNGE_MODE;

typedef struct StaticControlSettings {
	bool singleStepMode;
	bool showState;
	int outputMaxSize;
	int tickDelay;
} STATIC_CONTROL_SETTINGS, *PSTATIC_CONTROL_SETTINGS;

typedef struct DynamicControlSettings {
	bool stringMode;
	int depth;

	// FILE_ACCESS variables
	FILE* hProgramOut;
	FILE* hProgramIn;
} DYNAMIC_CONTROL_SETTINGS, *PDYNAMIC_CONTROL_SETTINGS;

typedef struct InstructionPointerState {
	int direction;
	GRID_POINT position;
} INSTRUCTION_POINTER_STATE, *PINSTRUCTION_POINTER_STATE;

typedef struct FungeStackState {
	STACK_ITEM_TYPE *stack;
	int stackPointer;
} FUNGE_STACK_STATE, *PFUNGE_STACK_STATE;

typedef struct FungeOutputState {
	char* output;
	int outputSize;
	FILE* outputFile;
} FUNGE_OUTPUT_STATE, *PFUNGE_OUTPUT_STATE;

typedef struct FungePageManager {
	PPAGE_CONTROL_LIST pPageControlList;
	PPAGE_CONTROL pCurrentPageControl;
	PFUNGE_STACK_STATE pStackState;
	REGISTERS registers;
	PINSTRUCTION_POINTER_STATE pIpState;
} FUNGE_PAGE_MANAGER, *PFUNGE_PAGE_MANAGER;

typedef struct BefungeProgramConfig {
	BEFUNGE_MODE mode;
	union programContent {
		char *lpProgramString;
		PPAGE_CONTROL_LIST pPages;
	};
	unsigned int tickDelay;

	FILE * pOutFile;
	bool isVisualiseModeActive;
	bool isSingleSteModeActive;

	PBEFUNGE_METADATA pMetadata;
} BEFUNGE_PROGRAM_CONFIG, *PBEFUNGE_PROGRAM_CONFIG;

typedef struct BefungeCoreControl {
	PBEFUNGE_PROGRAM_CONFIG pConfig;
	PSTATIC_CONTROL_SETTINGS pStaticSettings;
	PDYNAMIC_CONTROL_SETTINGS pDynamicSettings;
	PFUNGE_PAGE_MANAGER pManager;
	BEFUNGE_MODE mode;
	PFUNGE_OUTPUT_STATE pOutputState;
	bool hasTerminated;
} BEFUNGE_CORE_CONTROL, *PBEFUNGE_CORE_CONTROL;


/*
	POPULATION METHODS
*/

void RunPopulation();

/*
	INITIALISATION METHODS
*/
void InitialiseStaticControlSettings(PSTATIC_CONTROL_SETTINGS *settings, bool showState, bool singleStep, int tickDelay);
void InitialiseDynamicControlSettings(PDYNAMIC_CONTROL_SETTINGS *settings);
void InitialiseInstructionPointerState(PINSTRUCTION_POINTER_STATE *ipState, GRID_POINT *entrypoint);
void InitialiseRegisters(REGISTERS registers);
void InitialiseFungeStack(PFUNGE_STACK_STATE *stackStruct);
void InitialiseOutputState(PFUNGE_OUTPUT_STATE *outputState);
void InitialiseCoreControl(PBEFUNGE_CORE_CONTROL pControl, PBEFUNGE_PROGRAM_CONFIG pConfig);
void InitialisePageManager(PFUNGE_PAGE_MANAGER *pPageManager, PPAGE_CONTROL_LIST pPageList, PBEFUNGE_PROGRAM_CONFIG pConfig);


/*
	VISUALISATION METHODS
*/
void OutputString(PBEFUNGE_CORE_CONTROL pcontrol, char* str, int len);
void OutputRegistersState(PBEFUNGE_CORE_CONTROL pcontrol);
void PrintOutputState(PBEFUNGE_CORE_CONTROL pcontrol);
void PrintProgramState(PBEFUNGE_CORE_CONTROL pcontrol);
void PrintStackState(PBEFUNGE_CORE_CONTROL pcontrol);


/*
	PRIMITIVE METHODS
*/
char GetCommand(PBEFUNGE_CORE_CONTROL pControl);
void Push(PBEFUNGE_CORE_CONTROL pControl, STACK_ITEM_TYPE value);
void Pop(PBEFUNGE_CORE_CONTROL pControl, STACK_ITEM_TYPE* out);
void SetDirection(PBEFUNGE_CORE_CONTROL pControl, int direction);
void Put(PBEFUNGE_CORE_CONTROL pControl);
void Get(PBEFUNGE_CORE_CONTROL pControl);


/*
	SETTING CHANGE METHODS
*/
void ToggleStringMode(PBEFUNGE_CORE_CONTROL pControl);
void FlipDirection(PBEFUNGE_CORE_CONTROL pControl);
void SetProgramInput(PBEFUNGE_CORE_CONTROL pControl, FILE* hInput);
void SetProgramOutput(PBEFUNGE_CORE_CONTROL pControl, FILE* hOutput);
void ResetProgramInput(PBEFUNGE_CORE_CONTROL pControl);
void ResetProgramOutput(PBEFUNGE_CORE_CONTROL pControl);


/*
	CONTROL METHODS
*/
void LoadProgramString(PBEFUNGE_CORE_CONTROL pControl, char* programString);
void TakeStep(PBEFUNGE_CORE_CONTROL pControl);
void BackStep(PBEFUNGE_CORE_CONTROL pControl);
int ProcessTick(PBEFUNGE_CORE_CONTROL pControl);
bool AcceptCommand(PBEFUNGE_CORE_CONTROL pControl, char commandChar);


/*
	MISC METHODS
*/
bool IsPrintable(char value);
















