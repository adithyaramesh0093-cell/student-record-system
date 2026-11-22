/*
 * util.h - small, general-purpose string helpers.
 *
 * Nothing in here knows about students; these are plain C helpers that
 * the standard library does not give us in a portable way.
 */
#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>

/* Removes leading/trailing whitespace. Edits the string in place and returns
 * a pointer to the first non-space character (inside the same buffer). */
char *util_trim(char *s);

/* Strict number parsing: the whole string must be a number (spaces around it
 * are fine). Returns false on garbage, overflow, NaN or infinity. */
bool util_parse_int(const char *s, int *out);
bool util_parse_float(const char *s, float *out);

/* Case-insensitive helpers (strcasecmp / strcasestr are not portable). */
int  util_stricmp(const char *a, const char *b);
bool util_icontains(const char *haystack, const char *needle);

#endif /* UTIL_H */
