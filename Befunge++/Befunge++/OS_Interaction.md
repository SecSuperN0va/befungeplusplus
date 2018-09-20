#Befunge++ OS Interaction

##OSI Command
Operating System features can be accessed using the 'o' opcode. 

When executed, an OSI ID is popped from the stack and used to lookup the relevant action to take.

Additional arguments may then be popped from the stack based on the OSI ID selected. See below for details on the specific OSI ID's

## File Access
File access is acheived by effectively redirecting the standard input and output commands. When a file is opened (OSI\_FILE\_OPEN) a handle to the relevant file is created and stored within the BEFUNGE_CONTROL structure, 
replacing the previous handle for the mode of operation selected (stdin/stdout by default).

When a subsequent '.', ',', '&', or '~' opcode is executed, the input or output is sent/received using the stored file handle, rather than stdin/stdout as per usual.

When file access is complete, or no longer required, OSI\_FILE\_CLOSE can be used to close the file handle, and return the BEFUNGE_CONTROL structure's input/output handles to their original state (stdin/stdout).


###OSI\_FILE\_OPEN
*OSI ID:* 1
*ARGS:* 
- mode: boolean popped from stack. (0 for read, 1 for write)
- file_path: string popped from stack.
*Comments:*

###OSI\_FILE\_CLOSE
*OSI ID:* 2
*ARGS:* 
- mode: boolean popped from stack. (0 for read, 1 for write).

*Comments:* 
The `mode` argument is used to identify which handle within the BEFUNGE_CONTROL structure to close and revert to its original state. 
If `mode` is 0, then the handle designated for input will be closed and replaced with stdin.
If `mode` is 1, then the handle designated for output will be closed and replaced with stdout.

