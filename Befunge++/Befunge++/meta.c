#include "meta.h"
#include "configparser.h"
#include <string.h>
#include <stdlib.h>

void InitialiseProgramMetadataDefaults(PBEFUNGE_METADATA *metadata) {
	PBEFUNGE_METADATA meta = (PBEFUNGE_METADATA)calloc(1, sizeof(BEFUNGE_METADATA));

	meta->context.entrypoint.row = DEFAULT_ENTRYPOINT_ROW;
	meta->context.entrypoint.column = DEFAULT_ENTRYPOINT_COLUMN;

	meta->dimensions.rows = DEFAULT_DIMENSIONS_ROWS;
	meta->dimensions.columns = DEFAULT_DIMENSIONS_COLUMNS;

	meta->context.threadCount = DEFAULT_THREAD_COUNT;
	meta->context.threadHeadStart = DEFAULT_THREAD_HEADSTART;

	*metadata = meta;
	return;
}


PBEFUNGE_METADATA LoadProgramMetadata(FILE *fp) {
	config_option_t config;
	PBEFUNGE_METADATA meta = 0;

	InitialiseProgramMetadataDefaults(&meta);

	if (fp != NULL) {
		if ((config = ReadConfigSectionOfFile(fp)) != NULL) {
			// Program contains metadata
			do {
				if (!strcmp("dimensions.rows", config->key)) {
					meta->dimensions.rows = atoi(config->value);
				}
				if (!strcmp("dimensions.columns", config->key)) {
					meta->dimensions.columns = atoi(config->value);
				}
				if (!strcmp("context.entrypoint.row", config->key)) {
					meta->context.entrypoint.row = atoi(config->value);
				}
				if (!strcmp("context.entrypoint.column", config->key)) {
					meta->context.entrypoint.column = atoi(config->value);
				}
				if (!strcmp("context.threadCount", config->key)) {
					meta->context.threadCount = atoi(config->value);
				}
				if (!strcmp("context.threadHeadStart", config->key)) {
					meta->context.threadHeadStart = atoi(config->value);
				}

				config = config->prev;
			} while (config != NULL);
		}
		else {
			// Program does not contain metadata
		}
	}
	else {
		fprintf(stderr, "No file handle provided. Default metadata applies.\n");
	}
	return meta;
}