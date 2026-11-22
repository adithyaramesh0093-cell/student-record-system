/* stud_mod.c - M/m : Modify a Record */
#include <stdio.h>

#include "stud_common.h"
#include "stud_ops.h"

bool stud_mod(StudentList *list)
{
    char choice;

    if (list->count == 0) {
        puts("  There are no records to modify.");
        return true;
    }

    /* Step 1: find the record. */
    puts("R/r : Roll Number");
    puts("N/n : Name");
    puts("P/p : Percentage");
    QUIT_ON_EOF(input_choice("Enter Your Choice: ", "rnp", &choice));

    Student   *target = NULL;
    FindResult found;
    if (choice == 'r') {
        found = stud_find_by_roll(list, &target);       /* modify directly */
    } else {
        found = stud_find_by_search(list, choice == 'n' ? FIND_BY_NAME
                                                        : FIND_BY_PERCENTAGE, &target);
    }
    if (found == FIND_EOF) {
        return false;
    }
    if (found == FIND_NONE) {
        return true;
    }

    /* Step 2: choose the field and change it. */
    char percentage_text[16];
    student_format_percentage(target->percentage, percentage_text, sizeof percentage_text);
    printf("Selected: Roll Number %d, Name %s, Percentage %s\n",
           target->roll, target->name, percentage_text);

    puts("N/n : Name");
    puts("P/p : Percentage");
    QUIT_ON_EOF(input_choice("Enter Your Choice: ", "np", &choice));

    if (choice == 'n') {
        char name[STUDENT_NAME_SIZE];
        QUIT_ON_EOF(stud_prompt_name("Enter New Name: ", name, sizeof name));
        snprintf(target->name, sizeof target->name, "%s", name);
    } else {
        float percentage;
        QUIT_ON_EOF(input_float("Enter New Percentage: ", 0.0f, STUDENT_MAX_PERCENTAGE,
                                &percentage));
        target->percentage = student_round_percentage(percentage);
    }

    puts("  Record updated.");
    puts("  (Choose V/v to save your changes to disk.)");
    return true;
}
