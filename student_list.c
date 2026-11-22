#include "student_list.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

/* ---------- lifecycle ---------- */

void student_list_init(StudentList *list)
{
    list->head  = NULL;
    list->count = 0;
}

void student_list_free(StudentList *list)
{
    Student *node = list->head;
    while (node != NULL) {
        Student *next = node->next;     /* remember it before freeing the node */
        free(node);
        node = next;
    }
    student_list_init(list);
}

/* ---------- add / find / remove ---------- */

Student *student_list_find_by_roll(const StudentList *list, int roll)
{
    for (Student *node = list->head; node != NULL; node = node->next) {
        if (node->roll == roll) {
            return node;
        }
    }
    return NULL;
}

Status student_list_append(StudentList *list, Student *node)
{
    if (node == NULL) {
        return STATUS_INVALID_INPUT;
    }
    if (student_list_find_by_roll(list, node->roll) != NULL) {
        return STATUS_DUPLICATE_ROLL;
    }

    node->next = NULL;
    if (list->head == NULL) {
        list->head = node;                  /* first node in the list */
    } else {
        Student *last = list->head;
        while (last->next != NULL) {
            last = last->next;
        }
        last->next = node;
    }
    list->count++;
    return STATUS_OK;
}

Status student_list_remove(StudentList *list, int roll)
{
    /*
     * `link` points at whichever pointer refers to the current node: first
     * list->head, then each node's `next`. That way the head node and the
     * other nodes are unlinked with exactly the same code.
     */
    Student **link = &list->head;
    while (*link != NULL && (*link)->roll != roll) {
        link = &(*link)->next;
    }
    if (*link == NULL) {
        return STATUS_NOT_FOUND;
    }

    Student *doomed = *link;
    *link = doomed->next;                   /* skip over the removed node */
    free(doomed);
    list->count--;
    return STATUS_OK;
}

int student_list_next_roll(const StudentList *list)
{
    /*
     * A list with n students uses at most n of the numbers 1..n+1, so the
     * smallest free roll is always <= n+1. We mark the used numbers in that
     * range and take the first one left unmarked.
     */
    size_t limit = list->count + 1;
    unsigned char *used = calloc(limit + 1, 1);
    if (used == NULL) {
        return 0;
    }

    for (const Student *node = list->head; node != NULL; node = node->next) {
        if (node->roll >= 1 && (size_t)node->roll <= limit) {
            used[node->roll] = 1;
        }
    }

    size_t roll = 1;
    while (roll <= limit && used[roll]) {
        roll++;
    }

    free(used);
    return (int)roll;
}

/* ---------- search ---------- */

typedef bool (*Matcher)(const Student *student, const void *argument);

static bool match_name(const Student *student, const void *argument)
{
    return util_icontains(student->name, (const char *)argument);
}

static bool match_percentage(const Student *student, const void *argument)
{
    float wanted = student_round_percentage(*(const float *)argument);
    return student->percentage == wanted;   /* both are rounded the same way */
}

/* Copies every record that `matches` into the new list `results`. */
static Status collect_matches(const StudentList *list, StudentList *results,
                              Matcher matches, const void *argument)
{
    student_list_init(results);

    for (const Student *node = list->head; node != NULL; node = node->next) {
        if (!matches(node, argument)) {
            continue;
        }

        Student *copy = NULL;
        Status status = student_create(&copy, node->roll, node->name, node->percentage);
        if (status == STATUS_OK) {
            status = student_list_append(results, copy);
            if (status != STATUS_OK) {
                free(copy);
            }
        }
        if (status != STATUS_OK) {
            student_list_free(results);
            return status;
        }
    }
    return STATUS_OK;
}

Status student_list_search_name(const StudentList *list, const char *query,
                                StudentList *results)
{
    if (query == NULL || query[0] == '\0') {
        student_list_init(results);
        return STATUS_INVALID_INPUT;
    }
    return collect_matches(list, results, match_name, query);
}

Status student_list_search_percentage(const StudentList *list, float percentage,
                                      StudentList *results)
{
    if (!(percentage >= 0.0f && percentage <= STUDENT_MAX_PERCENTAGE)) {
        student_list_init(results);
        return STATUS_INVALID_INPUT;
    }
    return collect_matches(list, results, match_percentage, &percentage);
}

/* ---------- sorting (merge sort on a linked list) ---------- */

typedef int (*Comparator)(const Student *a, const Student *b);

static int compare_by_roll(const Student *a, const Student *b)
{
    return (a->roll > b->roll) - (a->roll < b->roll);
}

static int compare_by_name(const Student *a, const Student *b)
{
    int result = util_stricmp(a->name, b->name);
    return (result != 0) ? result : compare_by_roll(a, b);     /* tie-break */
}

static int compare_by_percentage(const Student *a, const Student *b)
{
    if (a->percentage < b->percentage) return -1;
    if (a->percentage > b->percentage) return 1;
    return compare_by_roll(a, b);
}

/* Cuts the list in the middle and returns the head of the second half. */
static Student *split_in_half(Student *head)
{
    Student *slow = head;
    Student *fast = head->next;

    while (fast != NULL && fast->next != NULL) {
        slow = slow->next;
        fast = fast->next->next;
    }

    Student *second_half = slow->next;
    slow->next = NULL;
    return second_half;
}

/* Merges two sorted lists into one sorted list by re-linking the nodes. */
static Student *merge_sorted(Student *a, Student *b, Comparator compare)
{
    Student  dummy;
    Student *tail = &dummy;

    memset(&dummy, 0, sizeof dummy);
    while (a != NULL && b != NULL) {
        if (compare(a, b) <= 0) {           /* <= keeps equal items in order */
            tail->next = a;
            a = a->next;
        } else {
            tail->next = b;
            b = b->next;
        }
        tail = tail->next;
    }
    tail->next = (a != NULL) ? a : b;
    return dummy.next;
}

static Student *merge_sort(Student *head, Comparator compare)
{
    if (head == NULL || head->next == NULL) {
        return head;                        /* 0 or 1 nodes: already sorted */
    }

    Student *second_half = split_in_half(head);
    return merge_sorted(merge_sort(head, compare),
                        merge_sort(second_half, compare), compare);
}

void student_list_sort(StudentList *list, SortField field)
{
    Comparator compare = (field == SORT_BY_NAME) ? compare_by_name
                                                 : compare_by_percentage;
    list->head = merge_sort(list->head, compare);
}
