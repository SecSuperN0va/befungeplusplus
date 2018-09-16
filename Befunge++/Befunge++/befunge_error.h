#pragma once
#include <stdio.h>

#define INFO_MESSAGE(message) fprintf(stderr, "[%s] [INFO] (%s:%d) %s\n", get_time_string(), __FILE__, __LINE__, message);
#define DEBUG_MESSAGE(message) fprintf(stderr, "[%s] [DEBUG] (%s:%d) %s\n", get_time_string(), __FILE__, __LINE__, message);
#define ERROR_MESSAGE(message) fprintf(stderr, "[%s] [ERROR] (%s:%d) %s\n", get_time_string(), __FILE__, __LINE__, message);getc(stdin);

#define MAX_MESSAGE_SIZE 2048

