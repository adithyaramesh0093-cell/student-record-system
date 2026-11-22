/*
 * student_list.h - the singly linked list of students.
 *
 *   head -> [roll|name|%] -> [roll|name|%] -> [roll|name|%] -> NULL
 *
 * Every node is created with malloc (see student_create) and released with
 * free. All the list logic lives here - add, find, remove, search, sort and
 * "smallest free roll number" - and none of it prints or reads the keyboard,
 * which keeps it easy to test.
 *
 * Typical use:
 *     StudentList list;
 *     student_list_init(&list);
 *     ...
 *     student_list_free(&list);       <- always call this when finished
 *
 * The list is meant for classroom-sized data (hundreds of students): adding
 * walks to the end of the list, exactly as a simple linked list does.
 */
#ifndef STUDENT_LIST_H
#define STUDENT_LIST_H

#include <stddef.h>

#include "status.h"
#include "student.h"

typedef struct {
    Student *head;      /* first node, or NULL when the list is empty */
    size_t   count;     /* number of nodes                            */
} StudentList;

typedef enum {
    SORT_BY_NAME,        /* A-Z, ignoring upper/lower case  */
    SORT_BY_PERCENTAGE   /* lowest percentage first         */
} SortField;

void student_list_init(StudentList *list);

/* Frees every node. The list is empty (and reusable) afterwards. */
void student_list_free(StudentList *list);

/*
 * Adds `node` at the END of the list.
 * On success the list owns the node. On failure (STATUS_DUPLICATE_ROLL) the
 * caller still owns it and must free() it.
 */
Status student_list_append(StudentList *list, Student *node);

/* Returns the node with this roll number, or NULL. The pointer stays valid
 * until that record is removed or the list is freed. */
Student *student_list_find_by_roll(const StudentList *list, int roll);

/* Unlinks and frees the node with this roll number. STATUS_NOT_FOUND if none. */
Status student_list_remove(StudentList *list, int roll);

/* The smallest positive roll number that no student is using (1 for an empty
 * list). Returns 0 only if memory ran out. */
int student_list_next_roll(const StudentList *list);

/*
 * Searches create a NEW list holding COPIES of the matching records, in list
 * order. `results` is initialised by the function; the caller must call
 * student_list_free(results) afterwards (even when nothing matched).
 *
 *  - by name: case-insensitive "contains" match; an empty query is invalid
 *  - by percentage: exact match (compared to 2 decimals)
 */
Status student_list_search_name(const StudentList *list, const char *query,
                                StudentList *results);
Status student_list_search_percentage(const StudentList *list, float percentage,
                                      StudentList *results);

/* Re-orders the nodes (stable merge sort - nothing is copied or allocated). */
void student_list_sort(StudentList *list, SortField field);

#endif /* STUDENT_LIST_H */
