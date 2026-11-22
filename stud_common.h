/*
 * stud_common.h - helpers shared by the stud_*.c menu actions.
 */
#ifndef STUD_COMMON_H
#define STUD_COMMON_H

#include <stddef.h>

#include "input.h"
#include "student_list.h"

/* Return false from the current action if the keyboard input has ended. */
#define QUIT_ON_EOF(call)                       \
    do {                                        \
        if ((call) == INPUT_EOF) {              \
            return false;                       \
        }                                       \
    } while (0)

typedef enum {
    FIND_OK,        /* *out points to the chosen record               */
    FIND_NONE,      /* nothing chosen (a message was already printed) */
    FIND_EOF        /* keyboard input ended                           */
} FindResult;

typedef enum {
    FIND_BY_NAME,
    FIND_BY_PERCENTAGE
} SearchKind;

/* Prints the list as the table required by the assignment (stud_show.c). */
void stud_print_table(const StudentList *list);

/* Asks for a student name until a valid one is typed. */
InputResult stud_prompt_name(const char *prompt, char *out, size_t size);

/* Asks for a roll number and looks it up in the list. */
FindResult stud_find_by_roll(StudentList *list, Student **out);

/*
 * Asks for a name (or a percentage), shows every matching record in a table,
 * then asks for the roll number of the one the user means. The roll number
 * must be one of the records that were shown.
 */
FindResult stud_find_by_search(StudentList *list, SearchKind kind, Student **out);

#endif /* STUD_COMMON_H */
