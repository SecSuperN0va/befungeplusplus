#pragma once
#include <stdbool.h>
#include "control.h"

#define CMD_ADD '+'  // Pop a and b then push a+b
#define CMD_SUB '-'  // Pop a and b then push b-a
#define CMD_MUL '*'  // Pop a and b then push a*b
#define CMD_DIV '/'  // Pop a and b then push b/a rounded towards 0
#define CMD_MOD '%'  // Pop a and b then push the remainder of the integer division of b/a
#define CMD_NOT '!'  // Pop a value. If the value is Zero push 1 else push Zero.
#define CMD_GREATER '`'  // Pop a and b then push 1 if b > a else Zero
#define CMD_RIGHT '>'  // Start moving right
#define CMD_LEFT '<'  // Start moving left
#define CMD_UP '^'  // Start moving up
#define CMD_DOWN 'v'  // Start moving down
#define CMD_RAND '?'  // Start moving in a random cardinal direction
#define CMD_HORIZ_CHECK '_'  // Pop a value, move right if value == 0 left otherwise
#define CMD_VERT_CHECK '|'  // Pop a value move down in value == 0 up otherwise
#define CMD_STRING '\"'  // Start String mode, push eash character's ASCII value all the way up to the next " character
#define CMD_DUP ':'  // Duplicate value on top of stack
#define CMD_SWAP '\\'  // Swap two values on top of the stack
#define CMD_DISCARD '$'  // Pop a value from the stack and discard it
#define CMD_INT_OUT '.'  // Pop a value and output as an integer followed by a space
#define CMD_ASCII_OUT ','  // Pop a value and output as ASCII character
#define CMD_BRIDGE '#'  // Bridge: Skip next call
#define CMD_PUT 'p'  // A "put" call: Pop y, x, v, then change the character at (x,y) in the program to the character with ASCII value v
#define CMD_GET 'g'  // A "get" call: Pop y and x then push ASCII value of the character at that position in the program
#define CMD_INT_IN '&'  // Ask user for a number and push its actual value
#define CMD_ASCII_IN '~'  // Ask user for a character and push its ASCII value
#define CMD_END '@'  // End program
#define CMD_NOP ' '  // No operation

#define CMD_LOAD_REG 'r' // Pop register identifier from stack, Pop value from stack, set register = value.
#define CMD_GET_REG 'R' // Pop register identifier from stack. Push value of specified register. 
#define CMD_SYSCALL 's' // Pop syscall number from stack, run the syscall with the relevant id.

#define CMD_OSI 'o'

#define CMD_FORK 'F'

#define FLAG_FILE_MODE_READ 0
#define FLAG_FILE_MODE_WRITE 1


bool CmdNop(PFUNGE_INSTANCE instance);
bool CmdAdd(PFUNGE_INSTANCE instance);
bool CmdSubtract(PFUNGE_INSTANCE instance);
bool CmdMultiply(PFUNGE_INSTANCE instance);
bool CmdDivide(PFUNGE_INSTANCE instance);
bool CmdModulo(PFUNGE_INSTANCE instance);
bool CmdNot(PFUNGE_INSTANCE instance);
bool CmdGreaterThan(PFUNGE_INSTANCE instance);
bool CmdMoveRight(PFUNGE_INSTANCE instance);
bool CmdMoveLeft(PFUNGE_INSTANCE instance);
bool CmdMoveUp(PFUNGE_INSTANCE instance);
bool CmdMoveDown(PFUNGE_INSTANCE instance);
bool CmdMoveRand(PFUNGE_INSTANCE instance);
bool CmdHorizontalNullCheck(PFUNGE_INSTANCE instance);
bool CmdVerticalNullCheck(PFUNGE_INSTANCE instance);
bool CmdStringMode(PFUNGE_INSTANCE instance);
bool CmdDuplicate(PFUNGE_INSTANCE instance);
bool CmdSwap(PFUNGE_INSTANCE instance);
bool CmdDiscard(PFUNGE_INSTANCE instance);
bool CmdIntegerOut(PFUNGE_INSTANCE instance);
bool CmdASCIIOut(PFUNGE_INSTANCE instance);
bool CmdBridge(PFUNGE_INSTANCE instance);
bool CmdPut(PFUNGE_INSTANCE instance);
bool CmdGet(PFUNGE_INSTANCE instance);
bool CmdIntegerIn(PFUNGE_INSTANCE instance);
bool CmdASCIIIn(PFUNGE_INSTANCE instance);
bool CmdTerminate(PFUNGE_INSTANCE instance);
bool CmdLoadReg(PFUNGE_INSTANCE instance);
bool CmdGetReg(PFUNGE_INSTANCE instance);
bool CmdSyscall(PFUNGE_INSTANCE instance);
bool CmdOperatingSystemInteraction(PFUNGE_INSTANCE instance);
bool CmdFork(PFUNGE_INSTANCE instance);

#define ORD_NOP 0
#define ORD_ADD 1
#define ORD_SUB 2
#define ORD_MUL 3
#define ORD_DIV 4
#define ORD_MOD 5
#define ORD_NOT 6
#define ORD_GREATER 7
#define ORD_RIGHT 8
#define ORD_LEFT 9
#define ORD_UP 10
#define ORD_DOWN 11 
#define ORD_RAND 12
#define ORD_HORIZ_CHECK 13
#define ORD_VERT_CHECK 14
#define ORD_STRING 15
#define ORD_DUP 16
#define ORD_SWAP 17
#define ORD_DISCARD 18
#define ORD_INT_OUT 19
#define ORD_ASCII_OUT 20
#define ORD_BRIDGE 21
#define ORD_PUT 22
#define ORD_GET 23
#define ORD_INT_IN 24
#define ORD_ASCII_IN 25
#define ORD_END 26

// Extended Functionality
#define ORD_LOAD_REG 27
#define ORD_GET_REG 28
#define ORD_SYSCALL 29
#define ORD_OSI 30
#define ORD_FORK 31

#define MAX_ORD 32


int commandCharLookup[];

void PopulateCommandCharLookup();

#define FUNC_NOP CmdNop
#define FUNC_ADD CmdAdd
#define FUNC_SUB CmdSubtract
#define FUNC_MUL CmdMultiply
#define FUNC_DIV CmdDivide
#define FUNC_MOD CmdModulo
#define FUNC_NOT CmdNot
#define FUNC_GREATER CmdGreaterThan
#define FUNC_RIGHT CmdMoveRight
#define FUNC_LEFT CmdMoveLeft
#define FUNC_UP CmdMoveUp
#define FUNC_DOWN CmdMoveDown 
#define FUNC_RAND CmdMoveRand
#define FUNC_HORIZ_CHECK CmdHorizontalNullCheck
#define FUNC_VERT_CHECK CmdVerticalNullCheck
#define FUNC_STRING CmdStringMode
#define FUNC_DUP CmdDuplicate
#define FUNC_SWAP CmdSwap
#define FUNC_DISCARD CmdDiscard
#define FUNC_INT_OUT CmdIntegerOut
#define FUNC_ASCII_OUT CmdASCIIOut
#define FUNC_BRIDGE CmdBridge
#define FUNC_PUT CmdPut
#define FUNC_GET CmdGet
#define FUNC_INT_IN CmdIntegerIn
#define FUNC_ASCII_IN CmdASCIIIn
#define FUNC_END CmdTerminate

// Extended Functionality
#define FUNC_LOAD_REG CmdLoadReg
#define FUNC_GET_REG CmdGetReg
#define FUNC_SYSCALL CmdSyscall
#define FUNC_OSI CmdOperatingSystemInteraction
#define FUNC_FORK CmdFork


bool(*commandTable[])(PBEFUNGE_CONTROL);

void PopulateCommandTable();