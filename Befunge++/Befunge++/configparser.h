#pragma once
#include <stdio.h>
#define CONFIG_ARG_MAX_BYTES 128

typedef struct config_option config_option;
typedef config_option* config_option_t;

struct config_option {
	config_option_t prev;
	char key[CONFIG_ARG_MAX_BYTES];
	char value[CONFIG_ARG_MAX_BYTES];
};

config_option_t ReadConfigSectionOfFile(FILE *fp);