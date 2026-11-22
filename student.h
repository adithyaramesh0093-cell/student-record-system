/*
 * student.h - one student record (a linked-list node) and its validation rules.
 *
 * This is the "model" layer: it knows what a valid record looks like and
 * nothing else. No printing, no files, no menus.
 */
#ifndef STUDENT_H
#define STUDENT_H

#include <stdbool.h>
#include <stddef.h>

#include "status.h"

#define STUDENT_NAME_SIZE      32          /* 31 characters + the '\0'  */
#define STUDENT_MIN_ROLL       1
#define STUDENT_MAX_ROLL       999999999
#define STUDENT_MAX_PERCENTAGE 100.0f

/* One node of the singly linked list. */
typedef struct Student {
    int             roll;                  /* unique, assigned automatically */
    char            name[STUDENT_NAME_SIZE];
    float           percentage;            /* 0.00 - 100.00                  */
    struct Student *next;                  /* next node, or NULL at the end  */
} Student;

/*
 * Allocates (malloc) and fills a new node, validating every value.
 * On success *out points to the node and the caller owns it - it must end up
 * in a list or be released with free().
 * On failure *out is NULL and the status says why.
 * Percentages are rounded to 2 decimals (that is how they are stored on disk).
 */
Status student_create(Student **out, int roll, const char *name, float percentage);

/* A name must be 1-31 characters, with no leading/trailing spaces and no
 * control characters (tabs, newlines, ...). */
bool student_name_is_valid(const char *name);

/* Rounds a percentage (>= 0) to 2 decimals. */
float student_round_percentage(float percentage);

/* Text for the table: at least one decimal, no pointless zeros.
 * 85.5 -> "85.5", 91 -> "91.0", 85.75 -> "85.75". `size` should be >= 8. */
void student_format_percentage(float percentage, char *buf, size_t size);

#endif /* STUDENT_H */
