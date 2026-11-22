#include "stud_common.h"

#include <stdio.h>

#include "status.h"
#include "student.h"

InputResult stud_prompt_name(const char *prompt, char *out, size_t size)
{
    for (;;) {
        InputResult result = input_text(prompt, out, size);
        if (result != INPUT_OK || student_name_is_valid(out)) {
            return result;
        }
        puts("  The name cannot contain tabs or other control characters.");
    }
}

FindResult stud_find_by_roll(StudentList *list, Student **out)
{
    int roll;

    if (input_int("Enter Roll Number: ", STUDENT_MIN_ROLL, STUDENT_MAX_ROLL, &roll)
            == INPUT_EOF) {
        return FIND_EOF;
    }

    *out = student_list_find_by_roll(list, roll);
    if (*out == NULL) {
        printf("  No record with Roll Number %d.\n", roll);
        return FIND_NONE;
    }
    return FIND_OK;
}

FindResult stud_find_by_search(StudentList *list, SearchKind kind, Student **out)
{
    StudentList matches;
    Status      status;

    if (kind == FIND_BY_NAME) {
        char query[STUDENT_NAME_SIZE];
        if (input_text("Enter Name to search: ", query, sizeof query) == INPUT_EOF) {
            return FIND_EOF;
        }
        status = student_list_search_name(list, query, &matches);
    } else {
        float percentage;
        if (input_float("Enter Percentage to search: ", 0.0f, STUDENT_MAX_PERCENTAGE,
                        &percentage) == INPUT_EOF) {
            return FIND_EOF;
        }
        status = student_list_search_percentage(list, percentage, &matches);
    }

    if (status != STATUS_OK) {
        printf("  Search failed: %s.\n", status_message(status));
        student_list_free(&matches);
        return FIND_NONE;
    }
    if (matches.count == 0) {
        puts("  No matching records found.");
        student_list_free(&matches);
        return FIND_NONE;
    }

    puts("Matching records:");
    stud_print_table(&matches);

    FindResult result = FIND_NONE;
    int        roll;

    if (input_int("Enter Roll Number: ", STUDENT_MIN_ROLL, STUDENT_MAX_ROLL, &roll)
            == INPUT_EOF) {
        result = FIND_EOF;
    } else if (student_list_find_by_roll(&matches, roll) == NULL) {
        puts("  That Roll Number is not in the list above.");
    } else {
        /* `matches` holds copies - hand back the real record from the list. */
        *out   = student_list_find_by_roll(list, roll);
        result = FIND_OK;
    }

    student_list_free(&matches);
    return result;
}
