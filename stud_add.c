/* stud_add.c - A/a : Add New Record */
#include <stdio.h>
#include <stdlib.h>

#include "stud_common.h"
#include "stud_ops.h"

bool stud_add(StudentList *list)
{
    char  name[STUDENT_NAME_SIZE];
    float percentage;

    /* The user only types the name and percentage... */
    QUIT_ON_EOF(stud_prompt_name("Enter Student Name: ", name, sizeof name));
    QUIT_ON_EOF(input_float("Enter Percentage: ", 0.0f, STUDENT_MAX_PERCENTAGE,
                            &percentage));

    /* ...the roll number is the smallest positive number not yet used. */
    int roll = student_list_next_roll(list);
    if (roll == 0) {
        puts("  Could not add the record: out of memory.");
        return true;
    }

    Student *node = NULL;
    Status   status = student_create(&node, roll, name, percentage);
    if (status == STATUS_OK) {
        status = student_list_append(list, node);
        if (status != STATUS_OK) {
            free(node);                 /* the list did not take the node */
        }
    }

    if (status != STATUS_OK) {
        printf("  Could not add the record: %s.\n", status_message(status));
        return true;
    }

    printf("  Record added. Roll Number assigned: %d\n", roll);
    puts("  (Choose V/v to save your changes to disk.)");
    return true;
}
