# Befunge++ Modularisation

## Module Specification

In order to achieve modularisation of befunge++ programs, we can define small standalone programs (or functions) within the bf++ source, then reference them by a numerical ID within a calling program.

Functions should be separated by two new line characters within the bf++ source.

Each function will be assigned a numeric identifier based on their position in the source file. Alternatively, it is possible to use the `function.identifier` metadata tag in order 
to specify function identifiers explicitly (supports out-of-sequence assignment).

## Forking

Forking can be used to effectively run subroutines within a program, using either fire-and-forget processing (for distinct, isolated tasks) or wait-for-result processing (for modularisation of functionality).


When the fork instruction ('F') is executed, a value is popped from the stack to determine the identifier of the function to be executed. A further value is popped from the stack to determine whether FAF or WFR will apply. 
Once the correct function has been identified and loaded, the relevant metadata is queried to identify how many arguments should be passed from the current stack to the stack for the callee. 
(NB. a metadata argument count tag of -1 refers to copy arguments up to and including a null byte. This is used to pass strings into functions.)

As an example, the following demonstrates the state of the stack when executing a function listed as ID 5, whose metadata stipulates it takes 3 arguments on the stack, and is expecting to return a result (and thus should be waited for):

```
   5
   1		(0 for FAF, 1 for WFR)
<arg_1>
<arg_2>
<arg_3>
```

The function identifier of 5 is popped from the stack.
The FAF/WFR indicator is popped from the stack.
Metadata associated with function 5 indicates that 3 arguments should be passed to the callee.
3 values are popped from the stack, a new stack is created for the callee, and the values are pushed on in reverse order (to preserve original ordering on the stack).

This would result in the callee's stack resembling the following prior to first opcode execution:

```
<arg_1>
<arg_2>
<arg_3>
```

It would be the responsibility of the callee to preserve these values as necessary, for example by popping the values from the stack and loading them into numbered registers.