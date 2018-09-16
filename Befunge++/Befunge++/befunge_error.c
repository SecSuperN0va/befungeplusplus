#include "befunge_error.h"
#include <time.h>
#include <stdlib.h>


char* get_time_string() {
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	char* timeString = (char*)calloc(20, sizeof(char));

	snprintf(timeString, 20 * sizeof(char), "%04d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	return timeString;
}