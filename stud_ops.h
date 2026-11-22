/*
 * stud_ops.h - the menu actions of the Student Record program.
 *
 * One function per menu item, each in its own source file:
 *
 *     stud_add()   stud_add.c      A/a : Add New Record
 *     stud_del()   stud_del.c      D/d : Delete a Record
 *     stud_show()  stud_show.c     S/s : Show the List
 *     stud_mod()   stud_mod.c      M/m : Modify a Record
 *     stud_sort()  stud_sort.c     T/t : Sort the List
 *     stud_save()  stud_save.c     V/v : Save   (and stud_load() at start-up)
 *
 * The functions that talk to the user return true to keep the program going,
 * or false when the keyboard input has ended (Ctrl+Z / Ctrl+D) and the
 * program should quit.
 */
#ifndef STUD_OPS_H
#define STUD_OPS_H

#include <stdbool.h>

#include "status.h"
#include "student_list.h"

#define STUDENT_DATA_FILE "student.dat"

bool stud_add(StudentList *list);
bool stud_del(StudentList *list);
void stud_show(const StudentList *list);
bool stud_mod(StudentList *list);
bool stud_sort(StudentList *list);

/*
 * File storage (stud_save.c). These two do not print anything.
 *
 * stud_save: writes the whole list to `path`. It writes "<path>.tmp" first
 *            and then swaps it in, so a crash mid-save cannot damage the
 *            existing file.
 * stud_load: adds the records stored in `path` to `list`. A missing file is
 *            NOT an error (first run). If the file is damaged the function
 *            returns STATUS_BAD_FORMAT and leaves the list empty.
 * stud_backup: byte-for-byte copy of a file (to keep a damaged file safe).
 */
Status stud_save(const StudentList *list, const char *path);
Status stud_load(StudentList *list, const char *path);
Status stud_backup(const char *path, const char *backup_path);

#endif /* STUD_OPS_H */
