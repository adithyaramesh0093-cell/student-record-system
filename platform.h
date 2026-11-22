/*
 * platform.h - the only place that contains operating-system specific code.
 */
#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdbool.h>

/* Moves file `from` to `to`, replacing `to` if it already exists.
 * (Plain rename() cannot replace an existing file on Windows.) */
bool platform_replace_file(const char *from, const char *to);

#endif /* PLATFORM_H */
