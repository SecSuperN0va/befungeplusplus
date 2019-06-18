#include "meta.h"
#include "configparser.h"
#include <string.h>
#include <stdlib.h>
#include "befunge_error.h"

void InitialiseProgramMetadataDefaults(PBEFUNGE_METADATA *metadata) {
	PBEFUNGE_METADATA meta = (PBEFUNGE_METADATA)calloc(1, sizeof(BEFUNGE_METADATA));

	meta->context.entrypoint.row = DEFAULT_ENTRYPOINT_ROW;
	meta->context.entrypoint.column = DEFAULT_ENTRYPOINT_COLUMN;

	meta->context.initialPageId = DEFAULT_INITIAL_PAGE_ID;

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
				if (!strcmp("context.entrypoint.row", config->key)) {
					meta->context.entrypoint.row = atoi(config->value);
				}
				if (!strcmp("context.entrypoint.column", config->key)) {
					meta->context.entrypoint.column = atoi(config->value);
				}
				if (!strcmp("context.initialPageId", config->key)) {
					meta->context.initialPageId = atoi(config->value);
				}	

				config = config->prev;
			} while (config != NULL);
		}
		else {
			// Program does not contain metadata
		}
	}
	else {
		ERROR_MESSAGE("No file handle provided. Default metadata applies.\n");
	}
	return meta;
}