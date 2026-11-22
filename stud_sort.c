/* stud_sort.c - T/t : Sort the List */
#include <stdio.h>

#include "stud_common.h"
#include "stud_ops.h"

bool stud_sort(StudentList *list)
{
    char choice;

    if (list->count == 0) {
        puts("  There are no records to sort.");
        return true;
    }

    puts("N/n : Sort by Name");
    puts("P/p : Sort by Percentage");
    QUIT_ON_EOF(input_choice("Enter Your Choice: ", "np", &choice));

    if (choice == 'n') {
        student_list_sort(list, SORT_BY_NAME);
        puts("  List sorted by name (A-Z).");
    } else {
        student_list_sort(list, SORT_BY_PERCENTAGE);
        puts("  List sorted by percentage (lowest to highest).");
    }

    stud_show(list);
    return true;
}
