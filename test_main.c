/*
 * Automated tests for the backend: the Student node, the linked list and the
 * student.dat file. (The menu functions are checked by running the program.)
 *
 * Build and run with:   make test        (Linux / macOS)
 *                       build.bat test   (Windows)
 *
 * No test framework needed: CHECK() counts passes and failures.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "status.h"
#include "stud_ops.h"
#include "student.h"
#include "student_list.h"
#include "util.h"

static int g_checks   = 0;
static int g_failures = 0;

#define CHECK(condition)                                                      \
    do {                                                                      \
        g_checks++;                                                           \
        if (!(condition)) {                                                   \
            g_failures++;                                                     \
            printf("  FAILED  %s:%d  %s\n", __FILE__, __LINE__, #condition);  \
        }                                                                     \
    } while (0)

#define NEAR(a, b) (((a) - (b)) < 0.001f && ((b) - (a)) < 0.001f)

/* True if the list holds exactly these roll numbers, in this order. */
#define ORDER_IS(list, ...) \
    order_is((list), (const int[]){__VA_ARGS__}, sizeof((const int[]){__VA_ARGS__}) / sizeof(int))

#define TEST_FILE "test_student.dat"

static bool order_is(const StudentList *list, const int *rolls, size_t n)
{
    const Student *node = list->head;

    for (size_t i = 0; i < n; i++) {
        if (node == NULL || node->roll != rolls[i]) {
            return false;
        }
        node = node->next;
    }
    return node == NULL && list->count == n;
}

/* Creates a node and appends it, recording a test failure if anything goes wrong. */
static void add(StudentList *list, int roll, const char *name, float percentage)
{
    Student *node = NULL;
    CHECK(student_create(&node, roll, name, percentage) == STATUS_OK);
    if (node != NULL) {
        Status status = student_list_append(list, node);
        CHECK(status == STATUS_OK);
        if (status != STATUS_OK) {
            free(node);
        }
    }
}

/* ---------- small file helpers for the storage tests ---------- */

static size_t read_file(const char *path, unsigned char *buf, size_t capacity)
{
    FILE *fp = fopen(path, "rb");
    size_t n = 0;

    if (fp != NULL) {
        n = fread(buf, 1, capacity, fp);
        fclose(fp);
    }
    return n;
}

static void write_file(const char *path, const unsigned char *buf, size_t n)
{
    FILE *fp = fopen(path, "wb");

    if (fp != NULL) {
        fwrite(buf, 1, n, fp);
        fclose(fp);
    }
}

static void cleanup_test_files(void)
{
    remove(TEST_FILE);
    remove(TEST_FILE ".tmp");
    remove(TEST_FILE ".bak");
}

/* ---------- util ---------- */

static void test_util(void)
{
    int   i;
    float f;
    char  padded[] = "   hello world \t\n";

    puts("util");
    CHECK(strcmp(util_trim(padded), "hello world") == 0);

    CHECK(util_parse_int("42", &i) && i == 42);
    CHECK(util_parse_int("  -7 ", &i) && i == -7);
    CHECK(!util_parse_int("", &i));
    CHECK(!util_parse_int("abc", &i));
    CHECK(!util_parse_int("12abc", &i));
    CHECK(!util_parse_int("99999999999", &i));

    CHECK(util_parse_float("85.5", &f) && NEAR(f, 85.5f));
    CHECK(!util_parse_float("", &f));
    CHECK(!util_parse_float("8x", &f));
    CHECK(!util_parse_float("nan", &f));
    CHECK(!util_parse_float("inf", &f));

    CHECK(util_stricmp("Alice", "aLiCe") == 0);
    CHECK(util_stricmp("Alice", "Bob") < 0);
    CHECK(util_icontains("Alice Smith", "SMITH"));
    CHECK(util_icontains("Alice Smith", ""));
    CHECK(!util_icontains("Alice", "Alicia"));
}

/* ---------- student node ---------- */

static void test_student(void)
{
    Student *s = NULL;
    char     long_name[80];
    char     text[16];

    puts("student node");
    CHECK(student_create(&s, 1, "Arun", 85.5f) == STATUS_OK);
    CHECK(s != NULL && s->roll == 1 && strcmp(s->name, "Arun") == 0);
    CHECK(s != NULL && NEAR(s->percentage, 85.5f) && s->next == NULL);
    free(s);

    CHECK(student_create(&s, 1, "Mary Ann Lee", 0.0f) == STATUS_OK);
    free(s);
    CHECK(student_create(&s, 1, "Smith, Jo", 100.0f) == STATUS_OK);   /* commas are fine now */
    free(s);

    CHECK(student_create(&s, 0, "Arun", 50) == STATUS_INVALID_INPUT && s == NULL);
    CHECK(student_create(&s, -3, "Arun", 50) == STATUS_INVALID_INPUT);
    CHECK(student_create(&s, 1, "", 50) == STATUS_INVALID_INPUT);
    CHECK(student_create(&s, 1, " Arun", 50) == STATUS_INVALID_INPUT);
    CHECK(student_create(&s, 1, "Arun ", 50) == STATUS_INVALID_INPUT);
    CHECK(student_create(&s, 1, "Ar\tun", 50) == STATUS_INVALID_INPUT);
    CHECK(student_create(&s, 1, "Arun", -0.5f) == STATUS_INVALID_INPUT);
    CHECK(student_create(&s, 1, "Arun", 100.5f) == STATUS_INVALID_INPUT);
    CHECK(student_create(&s, 1, NULL, 50) == STATUS_INVALID_INPUT);

    memset(long_name, 'a', sizeof long_name - 1);
    long_name[sizeof long_name - 1] = '\0';
    CHECK(student_create(&s, 1, long_name, 50) == STATUS_INVALID_INPUT);
    long_name[STUDENT_NAME_SIZE - 1] = '\0';                  /* exactly 31 characters */
    CHECK(student_create(&s, 1, long_name, 50) == STATUS_OK);
    free(s);
    long_name[STUDENT_NAME_SIZE - 2] = '\0';                  /* 30 characters */
    CHECK(student_create(&s, 1, long_name, 50) == STATUS_OK);
    free(s);

    CHECK(student_create(&s, 1, "Arun", 85.556f) == STATUS_OK);
    CHECK(NEAR(s->percentage, 85.56f));                        /* rounded to 2 decimals */
    free(s);

    student_format_percentage(85.5f, text, sizeof text);
    CHECK(strcmp(text, "85.5") == 0);
    student_format_percentage(91.0f, text, sizeof text);
    CHECK(strcmp(text, "91.0") == 0);
    student_format_percentage(85.75f, text, sizeof text);
    CHECK(strcmp(text, "85.75") == 0);
    student_format_percentage(100.0f, text, sizeof text);
    CHECK(strcmp(text, "100.0") == 0);
    student_format_percentage(0.0f, text, sizeof text);
    CHECK(strcmp(text, "0.0") == 0);
    student_format_percentage(7.05f, text, sizeof text);
    CHECK(strcmp(text, "7.05") == 0);
}

/* ---------- linked list ---------- */

static void test_list_basic(void)
{
    StudentList list;
    Student    *dup = NULL;

    puts("linked list: append / find / remove");
    student_list_init(&list);
    CHECK(list.head == NULL && list.count == 0);

    add(&list, 1, "Arun", 85.5f);
    add(&list, 2, "Priya", 91.0f);
    add(&list, 3, "Kiran", 60.0f);
    CHECK(ORDER_IS(&list, 1, 2, 3));                           /* appended at the end */

    CHECK(student_create(&dup, 2, "Copy", 10.0f) == STATUS_OK);
    CHECK(student_list_append(&list, dup) == STATUS_DUPLICATE_ROLL);
    free(dup);                                                 /* caller still owns it */
    CHECK(list.count == 3);

    CHECK(student_list_append(&list, NULL) == STATUS_INVALID_INPUT);

    CHECK(student_list_find_by_roll(&list, 2) != NULL);
    CHECK(strcmp(student_list_find_by_roll(&list, 2)->name, "Priya") == 0);
    CHECK(student_list_find_by_roll(&list, 99) == NULL);

    CHECK(student_list_remove(&list, 2) == STATUS_OK);         /* middle */
    CHECK(ORDER_IS(&list, 1, 3));
    CHECK(student_list_remove(&list, 1) == STATUS_OK);         /* head   */
    CHECK(ORDER_IS(&list, 3));
    add(&list, 4, "Meena", 70.0f);
    CHECK(student_list_remove(&list, 4) == STATUS_OK);         /* tail   */
    CHECK(ORDER_IS(&list, 3));
    CHECK(student_list_remove(&list, 3) == STATUS_OK);         /* last remaining node */
    CHECK(list.head == NULL && list.count == 0);
    CHECK(student_list_remove(&list, 3) == STATUS_NOT_FOUND);

    add(&list, 5, "Zed", 1.0f);
    student_list_free(&list);
    CHECK(list.head == NULL && list.count == 0);
    student_list_free(&list);                                  /* freeing twice is harmless */
}

static void test_next_roll(void)
{
    StudentList list;

    puts("linked list: smallest free roll number");
    student_list_init(&list);
    CHECK(student_list_next_roll(&list) == 1);                 /* empty list */

    add(&list, 1, "A", 1);
    add(&list, 2, "B", 1);
    add(&list, 3, "C", 1);
    CHECK(student_list_next_roll(&list) == 4);                 /* no gaps */

    CHECK(student_list_remove(&list, 2) == STATUS_OK);
    CHECK(student_list_next_roll(&list) == 2);                 /* gap is reused */
    add(&list, 2, "D", 1);
    CHECK(student_list_next_roll(&list) == 4);

    CHECK(student_list_remove(&list, 1) == STATUS_OK);
    CHECK(student_list_next_roll(&list) == 1);                 /* gap at the start */
    student_list_free(&list);

    add(&list, 5, "E", 1);                                     /* e.g. loaded from a file */
    add(&list, 9, "F", 1);
    CHECK(student_list_next_roll(&list) == 1);
    add(&list, 1, "G", 1);
    add(&list, 2, "H", 1);
    CHECK(student_list_next_roll(&list) == 3);
    student_list_free(&list);

    /* adding through next_roll never produces a duplicate, even after many deletes */
    for (int i = 0; i < 50; i++) {
        Student *node = NULL;
        int roll = student_list_next_roll(&list);
        CHECK(student_create(&node, roll, "X", 1) == STATUS_OK);
        CHECK(student_list_append(&list, node) == STATUS_OK);
        if (i % 3 == 2) {
            CHECK(student_list_remove(&list, (i % 7) + 1) == STATUS_OK ||
                  student_list_remove(&list, roll) == STATUS_OK);
        }
    }
    student_list_free(&list);
}

static void test_search(void)
{
    StudentList list, found;

    puts("linked list: search");
    student_list_init(&list);
    add(&list, 1, "Arun Kumar", 85.5f);
    add(&list, 2, "Priya", 91.0f);
    add(&list, 3, "ARUN Raj", 85.5f);

    CHECK(student_list_search_name(&list, "arun", &found) == STATUS_OK);
    CHECK(ORDER_IS(&found, 1, 3));
    CHECK(found.head != list.head);                            /* copies, not the same nodes */
    student_list_free(&found);

    CHECK(student_list_search_name(&list, "xyz", &found) == STATUS_OK);
    CHECK(found.count == 0);
    student_list_free(&found);

    CHECK(student_list_search_name(&list, "", &found) == STATUS_INVALID_INPUT);
    student_list_free(&found);

    CHECK(student_list_search_percentage(&list, 85.5f, &found) == STATUS_OK);
    CHECK(ORDER_IS(&found, 1, 3));
    student_list_free(&found);
    CHECK(student_list_search_percentage(&list, 91.0f, &found) == STATUS_OK);
    CHECK(ORDER_IS(&found, 2));
    student_list_free(&found);
    CHECK(student_list_search_percentage(&list, 85.55f, &found) == STATUS_OK);
    CHECK(found.count == 0);
    student_list_free(&found);
    CHECK(student_list_search_percentage(&list, 101.0f, &found) == STATUS_INVALID_INPUT);
    student_list_free(&found);

    student_list_free(&list);
}

static void test_sort(void)
{
    StudentList list;

    puts("linked list: sort");
    student_list_init(&list);
    student_list_sort(&list, SORT_BY_NAME);                    /* empty list: must not crash */
    student_list_sort(&list, SORT_BY_PERCENTAGE);

    add(&list, 1, "charlie", 50.0f);
    student_list_sort(&list, SORT_BY_NAME);                    /* one node */
    CHECK(ORDER_IS(&list, 1));

    add(&list, 2, "Alice", 90.0f);
    add(&list, 3, "bob", 70.0f);
    add(&list, 4, "alice", 60.0f);
    add(&list, 5, "Dave", 70.0f);

    student_list_sort(&list, SORT_BY_NAME);                    /* case-insensitive, ties by roll */
    CHECK(ORDER_IS(&list, 2, 4, 3, 1, 5));

    student_list_sort(&list, SORT_BY_PERCENTAGE);              /* lowest first, ties by roll */
    CHECK(ORDER_IS(&list, 1, 4, 3, 5, 2));

    student_list_sort(&list, SORT_BY_NAME);                    /* sorting again is fine */
    CHECK(ORDER_IS(&list, 2, 4, 3, 1, 5));

    /* the list must still work after sorting */
    CHECK(student_list_remove(&list, 3) == STATUS_OK);
    add(&list, 6, "Zoe", 10.0f);
    CHECK(ORDER_IS(&list, 2, 4, 1, 5, 6));
    student_list_free(&list);

    /* a longer list that is sorted in the opposite order */
    for (int i = 1; i <= 200; i++) {
        char name[16];
        snprintf(name, sizeof name, "n%03d", 201 - i);
        add(&list, i, name, (float)(i % 101));
    }
    student_list_sort(&list, SORT_BY_NAME);
    bool in_order = true;
    for (const Student *s = list.head; s != NULL && s->next != NULL; s = s->next) {
        if (util_stricmp(s->name, s->next->name) > 0) {
            in_order = false;
        }
    }
    CHECK(in_order && list.count == 200 && list.head->roll == 200);
    student_list_sort(&list, SORT_BY_PERCENTAGE);
    in_order = true;
    for (const Student *s = list.head; s != NULL && s->next != NULL; s = s->next) {
        if (s->percentage > s->next->percentage) {
            in_order = false;
        }
    }
    CHECK(in_order && list.count == 200);
    student_list_free(&list);
}

/* ---------- student.dat ---------- */

static void test_save_load(void)
{
    StudentList original, loaded;
    unsigned char bytes[256];

    puts("student.dat: save and load");
    cleanup_test_files();
    student_list_init(&original);
    student_list_init(&loaded);

    add(&original, 1, "Arun", 85.5f);
    add(&original, 2, "Priya Nair", 91.0f);
    add(&original, 7, "Smith, Jo", 40.25f);                    /* order is kept, gaps are fine */

    CHECK(stud_save(&original, TEST_FILE) == STATUS_OK);
    CHECK(stud_save(&original, TEST_FILE) == STATUS_OK);       /* overwriting works */
    CHECK(fopen(TEST_FILE ".tmp", "rb") == NULL);              /* temp file is gone */

    CHECK(read_file(TEST_FILE, bytes, sizeof bytes) == 8 + 3 * 40);   /* documented layout */
    CHECK(memcmp(bytes, "STUD", 4) == 0 && bytes[4] == 3 && bytes[5] == 0);
    CHECK(bytes[8] == 1 && bytes[12] == 'A' && bytes[44] == (8550 & 0xFF));

    CHECK(stud_load(&loaded, TEST_FILE) == STATUS_OK);
    CHECK(ORDER_IS(&loaded, 1, 2, 7));
    CHECK(strcmp(loaded.head->name, "Arun") == 0 && NEAR(loaded.head->percentage, 85.5f));
    CHECK(strcmp(loaded.head->next->name, "Priya Nair") == 0);
    CHECK(strcmp(loaded.head->next->next->name, "Smith, Jo") == 0);
    CHECK(NEAR(loaded.head->next->next->percentage, 40.25f));
    CHECK(loaded.head->percentage == original.head->percentage);

    student_list_free(&loaded);

    /* an empty list saves and loads too */
    student_list_free(&original);
    CHECK(stud_save(&original, TEST_FILE) == STATUS_OK);
    CHECK(stud_load(&loaded, TEST_FILE) == STATUS_OK && loaded.count == 0);

    student_list_free(&loaded);
    cleanup_test_files();
}

static void test_load_problems(void)
{
    StudentList   list, good;
    unsigned char valid[256], broken[256];
    size_t        valid_size;

    puts("student.dat: missing and damaged files");
    cleanup_test_files();

    student_list_init(&list);
    CHECK(stud_load(&list, "does_not_exist_12345.dat") == STATUS_OK);   /* first run */
    CHECK(list.count == 0);

    /* a valid 2-record file to damage in different ways */
    student_list_init(&good);
    add(&good, 1, "Arun", 85.5f);
    add(&good, 2, "Priya", 91.0f);
    CHECK(stud_save(&good, TEST_FILE) == STATUS_OK);
    valid_size = read_file(TEST_FILE, valid, sizeof valid);
    CHECK(valid_size == 8 + 2 * 40);
    student_list_free(&good);

    /* wrong magic bytes (not a student file) */
    memcpy(broken, valid, valid_size);
    memcpy(broken, "XXXX", 4);
    write_file(TEST_FILE, broken, valid_size);
    CHECK(stud_load(&list, TEST_FILE) == STATUS_BAD_FORMAT && list.count == 0);

    /* empty file */
    write_file(TEST_FILE, valid, 0);
    CHECK(stud_load(&list, TEST_FILE) == STATUS_BAD_FORMAT && list.count == 0);

    /* cut short in the middle of the second record */
    write_file(TEST_FILE, valid, valid_size - 5);
    CHECK(stud_load(&list, TEST_FILE) == STATUS_BAD_FORMAT);
    CHECK(list.head == NULL && list.count == 0);               /* nothing half-loaded is kept */

    /* header says 3 records but only 2 are stored */
    memcpy(broken, valid, valid_size);
    broken[4] = 3;
    write_file(TEST_FILE, broken, valid_size);
    CHECK(stud_load(&list, TEST_FILE) == STATUS_BAD_FORMAT && list.count == 0);

    /* extra bytes after the last record */
    memcpy(broken, valid, valid_size);
    broken[valid_size] = 0x7F;
    write_file(TEST_FILE, broken, valid_size + 1);
    CHECK(stud_load(&list, TEST_FILE) == STATUS_BAD_FORMAT && list.count == 0);

    /* second record repeats roll number 1 */
    memcpy(broken, valid, valid_size);
    broken[8 + 40] = 1;
    write_file(TEST_FILE, broken, valid_size);
    CHECK(stud_load(&list, TEST_FILE) == STATUS_BAD_FORMAT && list.count == 0);

    /* roll number 0 */
    memcpy(broken, valid, valid_size);
    broken[8] = 0;
    write_file(TEST_FILE, broken, valid_size);
    CHECK(stud_load(&list, TEST_FILE) == STATUS_BAD_FORMAT && list.count == 0);

    /* name field without a terminating 0 */
    memcpy(broken, valid, valid_size);
    memset(&broken[12], 'a', STUDENT_NAME_SIZE);
    write_file(TEST_FILE, broken, valid_size);
    CHECK(stud_load(&list, TEST_FILE) == STATUS_BAD_FORMAT && list.count == 0);

    /* percentage above 100.00 */
    memcpy(broken, valid, valid_size);
    broken[44] = 0xFF;
    broken[45] = 0xFF;
    write_file(TEST_FILE, broken, valid_size);
    CHECK(stud_load(&list, TEST_FILE) == STATUS_BAD_FORMAT && list.count == 0);

    /* empty name */
    memcpy(broken, valid, valid_size);
    memset(&broken[12], 0, STUDENT_NAME_SIZE);
    write_file(TEST_FILE, broken, valid_size);
    CHECK(stud_load(&list, TEST_FILE) == STATUS_BAD_FORMAT && list.count == 0);

    /* the untouched file still loads */
    write_file(TEST_FILE, valid, valid_size);
    CHECK(stud_load(&list, TEST_FILE) == STATUS_OK && ORDER_IS(&list, 1, 2));
    student_list_free(&list);

    /* backup */
    CHECK(stud_backup(TEST_FILE, TEST_FILE ".bak") == STATUS_OK);
    CHECK(read_file(TEST_FILE ".bak", broken, sizeof broken) == valid_size);
    CHECK(memcmp(broken, valid, valid_size) == 0);
    CHECK(stud_backup("does_not_exist_12345.dat", TEST_FILE ".bak") == STATUS_IO_ERROR);

    cleanup_test_files();
}

int main(void)
{
    test_util();
    test_student();
    test_list_basic();
    test_next_roll();
    test_search();
    test_sort();
    test_save_load();
    test_load_problems();

    printf("\n%d checks, %d failed\n", g_checks, g_failures);
    if (g_failures == 0) {
        puts("ALL TESTS PASSED");
        return 0;
    }
    return 1;
}
