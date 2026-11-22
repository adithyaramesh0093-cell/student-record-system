/* stud_del.c - D/d : Delete a Record */
#include <stdio.h>

#include "stud_common.h"
#include "stud_ops.h"

bool stud_del(StudentList *list)
{
    char choice;

    if (list->count == 0) {
        puts("  There are no records to delete.");
        return true;
    }

    puts("R/r : Delete using Roll Number");
    puts("N/n : Delete using Name");
    QUIT_ON_EOF(input_choice("Enter Your Choice: ", "rn", &choice));

    /* Roll number: delete directly.
     * Name: show the matches first, then ask which roll number to delete. */
    Student   *target = NULL;
    FindResult found  = (choice == 'r')
                        ? stud_find_by_roll(list, &target)
                        : stud_find_by_search(list, FIND_BY_NAME, &target);

    if (found == FIND_EOF) {
        return false;
    }
    if (found == FIND_NONE) {
        return true;
    }

    /* Copy what we want to print: the node is gone after the removal. */
    int  roll = target->roll;
    char name[STUDENT_NAME_SIZE];
    snprintf(name, sizeof name, "%s", target->name);

    Status status = student_list_remove(list, roll);
    if (status != STATUS_OK) {
        printf("  Could not delete the record: %s.\n", status_message(status));
        return true;
    }

    printf("  Record deleted: Roll Number %d (%s).\n", roll, name);
    puts("  (Choose V/v to save your changes to disk.)");
    return true;
}
