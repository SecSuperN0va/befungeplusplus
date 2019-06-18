#pragma once
#include <stdio.h>

#define ERR_SUCCESS 0 
#define ERR_INVALID_PROGRAM_FILE 1
#define ERR_INVALID_OUTPUT_FILE 2
#define ERR_INVALID_ARGS 3
#define ERR_ARG_PROCESSING_FAILED 4
#define ERR_INVALID_PAGING 5

#define INFO_MESSAGE(fmt, ...) fprintf(stderr, "[%s] [INFO] (%s:%d) " fmt "\n", get_time_string(), __FILE__, __LINE__, ##__VA_ARGS__);
#define DEBUG_MESSAGE(fmt, ...) fprintf(stderr, "[%s] [DEBUG] (%s:%d) " fmt "\n", get_time_string(), __FILE__, __LINE__,  ##__VA_ARGS__);
#define ERROR_MESSAGE(fmt, ...) { fprintf(stderr, "[%s] [ERROR] (%s:%d) " fmt "\n", get_time_string(), __FILE__, __LINE__,  ##__VA_ARGS__); getc(stdin); }

#define MAX_MESSAGE_SIZE 2048

char* get_time_string();