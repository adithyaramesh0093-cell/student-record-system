#include "student.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool student_name_is_valid(const char *name)
{
    size_t len = strlen(name);

    if (len == 0 || len >= STUDENT_NAME_SIZE) {
        return false;
    }
    if (isspace((unsigned char)name[0]) || isspace((unsigned char)name[len - 1])) {
        return false;
    }
    for (size_t i = 0; i < len; i++) {
        if (iscntrl((unsigned char)name[i])) {
            return false;
        }
    }
    return true;
}

float student_round_percentage(float percentage)
{
    /* Plain integer maths, so no math library is needed. */
    double hundredths = (double)(long)((double)percentage * 100.0 + 0.5);
    return (float)(hundredths / 100.0);
}

void student_format_percentage(float percentage, char *buf, size_t size)
{
    snprintf(buf, size, "%.2f", (double)percentage);

    /* "85.50" -> "85.5", "91.00" -> "91.0", but "85.75" stays as it is. */
    size_t len = strlen(buf);
    if (len > 0 && buf[len - 1] == '0') {
        buf[len - 1] = '\0';
    }
}

Status student_create(Student **out, int roll, const char *name, float percentage)
{
    if (out == NULL) {
        return STATUS_INVALID_INPUT;
    }
    *out = NULL;

    if (name == NULL || roll < STUDENT_MIN_ROLL || roll > STUDENT_MAX_ROLL) {
        return STATUS_INVALID_INPUT;
    }
    if (!student_name_is_valid(name)) {
        return STATUS_INVALID_INPUT;
    }
    /* Written this way so that NaN is rejected as well. */
    if (!(percentage >= 0.0f && percentage <= STUDENT_MAX_PERCENTAGE)) {
        return STATUS_INVALID_INPUT;
    }

    Student *node = calloc(1, sizeof *node);        /* zero-filled, so next == NULL */
    if (node == NULL) {
        return STATUS_OUT_OF_MEMORY;
    }

    node->roll = roll;
    snprintf(node->name, sizeof node->name, "%s", name);
    node->percentage = student_round_percentage(percentage);

    *out = node;
    return STATUS_OK;
}
