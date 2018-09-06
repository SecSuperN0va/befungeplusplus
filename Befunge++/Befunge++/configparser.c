#include "configparser.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>


config_option_t ReadConfigSectionOfFile(FILE *fp) {

	config_option_t last_co_addr = NULL;
	bool hasPassedNewLine = false;

	// Check if file has config
	char c;
	size_t origPos = ftell(fp);
	c = fgetc(fp);
	fseek(fp, origPos, SEEK_SET);

	if (c == '[') {	// '[' indicates the file starts with "[META]" which means it contains config
		/* Do nothing (to move the cursor to the end of the line).*/
		while (fgetc(fp) != '\n') {}

		while (1) {
			config_option_t co = NULL;
			if ((co = (config_option_t)calloc(1, sizeof(config_option))) == NULL)
				continue;
			memset(co, 0, sizeof(config_option));
			co->prev = last_co_addr;

			origPos = ftell(fp);
			c = fgetc(fp);
			fseek(fp, origPos, SEEK_SET);
			if (c != '\r' && c != '\n') {
				//int size = (3 + (2 * CONFIG_ARG_MAX_BYTES)) * sizeof(char);
				//char* formattedString = (char*)calloc(size, 1);
				//snprintf(formattedString, size, "%s = %s", co->key, co->value);
				if (fscanf(fp, "%s = %s", co->key, co->value) != 2)
				{
					if (feof(fp)) {
						break;
					}
					if (co->key[0] == '#') {
						/* Do nothing (to move the cursor to the end of the line).*/
						while (fgetc(fp) != '\n') {}

						free(co);
						continue;
					}
					free(co);
					continue;
				}
				//fprintf(stderr, "Key: %s Value: %s\n", co->key, co->value);
				last_co_addr = co;

				/* Do nothing (to move the cursor to the end of the line).*/
				while (fgetc(fp) != '\n') {}
			}
			else {
				//fprintf(stdout, "Found two new lines in a row! End of config");
				// Re-read the newline character to get the cursor in the correct location to start reading the program.
				while ((c = fgetc(fp)) != '\n') {}
				break;
			}
		}
	}
	else {
		fprintf(stderr, "File has no config!\n");
	}
	return last_co_addr;
}