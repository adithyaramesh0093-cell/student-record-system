/*
 * Student Record Management System - mini project
 *
 * main.c only wires things together:
 *   1. load student.dat into the linked list,
 *   2. show the menu and call the stud_*() function the user chooses,
 *   3. free the memory and exit.
 *
 * Nothing is written to disk until the user chooses V/v (Save) or S/s when
 * exiting.
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "input.h"
#include "status.h"
#include "stud_ops.h"
#include "student_list.h"

#define BACKUP_FILE (STUDENT_DATA_FILE ".bak")

static void print_menu(void)
{
    puts("");
    puts("**** STUDENT RECORD MENU ****");
    puts("A/a : Add New Record");
    puts("D/d : Delete a Record");
    puts("S/s : Show the List");
    puts("M/m : Modify a Record");
    puts("V/v : Save");
    puts("T/t : Sort the List");
    puts("E/e : Exit");
}

/* Saves and tells the user what happened. Returns true on success. */
static bool save_records(const StudentList *list)
{
    Status status = stud_save(list, STUDENT_DATA_FILE);

    if (status != STATUS_OK) {
        printf("  Could not save to %s: %s.\n", STUDENT_DATA_FILE, status_message(status));
        return false;
    }
    printf("  %lu record(s) saved to %s.\n", (unsigned long)list->count, STUDENT_DATA_FILE);
    return true;
}

/*
 * E/e : Exit. Asks whether to save first.
 * Returns true when the program should end, false to stay in the menu.
 * Sets *input_ok to false if the keyboard input ended.
 */
static bool exit_menu(const StudentList *list, bool *input_ok)
{
    char choice;

    puts("S/s : Save and Exit");
    puts("E/e : Exit Without Saving");
    if (input_choice("Enter Your Choice: ", "se", &choice) == INPUT_EOF) {
        *input_ok = false;
        return true;
    }

    if (choice == 's' && !save_records(list)) {
        puts("  Not exiting, so your records are not lost. Fix the problem and try again.");
        return false;
    }
    return true;
}

/* Loads student.dat. Returns false if the program must not start. */
static bool load_records(StudentList *list)
{
    Status status = stud_load(list, STUDENT_DATA_FILE);

    if (status == STATUS_BAD_FORMAT) {
        /* Keep the damaged file: a later Save would otherwise overwrite it. */
        if (stud_backup(STUDENT_DATA_FILE, BACKUP_FILE) != STATUS_OK) {
            fprintf(stderr, "%s is damaged and a backup could not be made. "
                            "Please move or delete it and start again.\n",
                    STUDENT_DATA_FILE);
            return false;
        }
        printf("WARNING: %s is damaged and could not be loaded.\n"
               "         A copy was saved as %s. Starting with an empty list.\n",
               STUDENT_DATA_FILE, BACKUP_FILE);
    } else if (status != STATUS_OK) {
        /* Do NOT continue: a later Save would overwrite the file we could not read. */
        fprintf(stderr, "Could not read %s: %s.\n", STUDENT_DATA_FILE, status_message(status));
        return false;
    }

    printf("Loaded %lu record(s) from %s.\n", (unsigned long)list->count, STUDENT_DATA_FILE);
    return true;
}

int main(void)
{
    StudentList list;
    bool        input_ok = true;     /* false once the keyboard input has ended */
    bool        finished = false;    /* true once the user chose to exit        */

    student_list_init(&list);
    if (!load_records(&list)) {
        student_list_free(&list);
        return EXIT_FAILURE;
    }

    while (input_ok && !finished) {
        char choice;

        print_menu();
        if (input_choice("Enter Your Choice: ", "adsmvte", &choice) == INPUT_EOF) {
            input_ok = false;
            break;
        }

        switch (choice) {
        case 'a': input_ok = stud_add(&list);               break;
        case 'd': input_ok = stud_del(&list);               break;
        case 's': stud_show(&list);                         break;
        case 'm': input_ok = stud_mod(&list);               break;
        case 'v': save_records(&list);                      break;
        case 't': input_ok = stud_sort(&list);              break;
        case 'e': finished = exit_menu(&list, &input_ok);   break;
        }
    }

    if (!input_ok) {
        puts("\nInput ended - exiting without saving.");
    }

    student_list_free(&list);
    return EXIT_SUCCESS;
}
