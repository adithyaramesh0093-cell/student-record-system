#include "input.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "util.h"

#define LINE_SIZE 256

/*
 * Reads one line from the keyboard into `buf` (without the newline).
 * Sets *too_long when the user typed more than the buffer can hold; the
 * extra characters are thrown away so they do not leak into the next prompt.
 */
static InputResult read_line(char *buf, size_t size, bool *too_long)
{
    *too_long = false;

    if (fgets(buf, (int)size, stdin) == NULL) {
        return INPUT_EOF;
    }

    size_t len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
    } else if (len == size - 1) {
        int ch;
        while ((ch = getchar()) != EOF && ch != '\n') {
            /* discard the rest of the over-long line */
        }
        *too_long = true;
    }
    return INPUT_OK;
}

/* Shows the prompt and returns the trimmed line. */
static InputResult ask(const char *prompt, char *line, size_t size,
                       char **text, bool *too_long)
{
    printf("%s", prompt);
    fflush(stdout);

    if (read_line(line, size, too_long) == INPUT_EOF) {
        return INPUT_EOF;
    }
    *text = util_trim(line);
    return INPUT_OK;
}

InputResult input_choice(const char *prompt, const char *valid, char *out)
{
    char line[LINE_SIZE];

    for (;;) {
        char *text;
        bool  too_long;

        if (ask(prompt, line, sizeof line, &text, &too_long) == INPUT_EOF) {
            return INPUT_EOF;
        }
        if (!too_long && text[0] != '\0' && text[1] == '\0') {
            char c = (char)tolower((unsigned char)text[0]);
            if (strchr(valid, c) != NULL) {
                *out = c;
                return INPUT_OK;
            }
        }

        printf("  Invalid choice. Please enter one of:");
        for (const char *p = valid; *p != '\0'; p++) {
            printf(" %c/%c", toupper((unsigned char)*p), *p);
        }
        printf("\n");
    }
}

InputResult input_text(const char *prompt, char *out, size_t out_size)
{
    char line[LINE_SIZE];

    for (;;) {
        char *text;
        bool  too_long;

        if (ask(prompt, line, sizeof line, &text, &too_long) == INPUT_EOF) {
            return INPUT_EOF;
        }
        if (too_long || strlen(text) >= out_size) {
            printf("  Too long - please use at most %lu characters.\n",
                   (unsigned long)(out_size - 1));
            continue;
        }
        if (*text == '\0') {
            puts("  This cannot be empty.");
            continue;
        }

        snprintf(out, out_size, "%s", text);
        return INPUT_OK;
    }
}

InputResult input_int(const char *prompt, int min, int max, int *out)
{
    char line[LINE_SIZE];

    for (;;) {
        char *text;
        bool  too_long;
        int   value;

        if (ask(prompt, line, sizeof line, &text, &too_long) == INPUT_EOF) {
            return INPUT_EOF;
        }
        if (too_long || !util_parse_int(text, &value) || value < min || value > max) {
            printf("  Please enter a whole number between %d and %d.\n", min, max);
            continue;
        }

        *out = value;
        return INPUT_OK;
    }
}

InputResult input_float(const char *prompt, float min, float max, float *out)
{
    char line[LINE_SIZE];

    for (;;) {
        char  *text;
        bool   too_long;
        float  value;

        if (ask(prompt, line, sizeof line, &text, &too_long) == INPUT_EOF) {
            return INPUT_EOF;
        }
        if (too_long || !util_parse_float(text, &value) || value < min || value > max) {
            printf("  Please enter a number between %.0f and %.0f.\n",
                   (double)min, (double)max);
            continue;
        }

        *out = value;
        return INPUT_OK;
    }
}
