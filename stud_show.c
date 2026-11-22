/* stud_show.c - S/s : Show the List */
#include <stdio.h>
#include <string.h>

#include "stud_common.h"
#include "stud_ops.h"

#define ROLL_COLUMN_WIDTH    12
#define MIN_NAME_COLUMN      13     /* the layout shown in the assignment */
#define PERCENT_COLUMN_WIDTH 13

static void print_dashes(int count)
{
    for (int i = 0; i < count; i++) {
        putchar('-');
    }
    putchar('\n');
}

void stud_print_table(const StudentList *list)
{
    /* The Name column grows if a name is too long for the default width. */
    int name_width = MIN_NAME_COLUMN;
    for (const Student *s = list->head; s != NULL; s = s->next) {
        int needed = (int)strlen(s->name) + 2;
        if (needed > name_width) {
            name_width = needed;
        }
    }
    int total_width = ROLL_COLUMN_WIDTH + name_width + PERCENT_COLUMN_WIDTH;

    print_dashes(total_width);
    printf("%-*s%-*s%s\n", ROLL_COLUMN_WIDTH, "Roll No", name_width, "Name", "Percentage");
    print_dashes(total_width);

    for (const Student *s = list->head; s != NULL; s = s->next) {
        char percentage[16];
        student_format_percentage(s->percentage, percentage, sizeof percentage);
        printf("%-*d%-*s%s\n", ROLL_COLUMN_WIDTH, s->roll, name_width, s->name, percentage);
    }

    print_dashes(total_width);
}

void stud_show(const StudentList *list)
{
    if (list->count == 0) {
        puts("  The list is empty. Choose A/a to add a record.");
        return;
    }
    stud_print_table(list);
}
