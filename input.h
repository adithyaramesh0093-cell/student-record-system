/*
 * input.h - safe keyboard input.
 *
 * scanf() is easy to misuse (buffer overflows, leftover characters in the
 * input). These helpers read a whole line with fgets(), check it, and keep
 * asking until the user types something valid.
 *
 * Every function returns:
 *   INPUT_OK  - `out` now holds a valid value
 *   INPUT_EOF - the input stream ended (Ctrl+Z / Ctrl+D); stop asking
 */
#ifndef INPUT_H
#define INPUT_H

#include <stddef.h>

typedef enum {
    INPUT_OK,
    INPUT_EOF
} InputResult;

/*
 * Menu choice: the user must type exactly ONE character that appears in
 * `valid` (lower-case letters), in either case - "a" and "A" both give 'a'.
 * Anything else prints a hint and asks again.
 */
InputResult input_choice(const char *prompt, const char *valid, char *out);

/* Reads a line of text (trimmed, not empty). It must fit in `out_size` bytes
 * including the terminating '\0'; otherwise the user is asked again. */
InputResult input_text(const char *prompt, char *out, size_t out_size);

/* Reads a whole number between min and max (inclusive). */
InputResult input_int(const char *prompt, int min, int max, int *out);

/* Reads a decimal number between min and max (inclusive). */
InputResult input_float(const char *prompt, float min, float max, float *out);

#endif /* INPUT_H */
