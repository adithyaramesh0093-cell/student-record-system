/*
 * stud_save.c - V/v : Save, and loading at start-up.
 *
 * student.dat is a small BINARY file:
 *
 *     offset  size  content
 *     ------  ----  ---------------------------------------------------
 *       0      4    magic bytes "STUD"  (identifies the file type)
 *       4      4    number of records N
 *       8     40    record 1:  roll (4) | name (32, padded with 0) | percentage (4)
 *      48     40    record 2 ...                                   ... N records
 *
 * Numbers are 32-bit and written little-endian byte by byte, so the file works
 * on any computer. The percentage is stored as hundredths (85.5 -> 8550) so no
 * floating-point bits are written to disk.
 */
#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "platform.h"
#include "stud_ops.h"

#define MAGIC            "STUD"
#define MAGIC_SIZE       4
#define PATH_SIZE        1024
#define MAX_HUNDREDTHS   10000          /* 100.00 % */

/* ---------- reading and writing 32-bit numbers ---------- */

static bool write_u32(FILE *fp, uint32_t value)
{
    unsigned char bytes[4];

    bytes[0] = (unsigned char)(value & 0xFFu);
    bytes[1] = (unsigned char)((value >> 8) & 0xFFu);
    bytes[2] = (unsigned char)((value >> 16) & 0xFFu);
    bytes[3] = (unsigned char)((value >> 24) & 0xFFu);
    return fwrite(bytes, 1, sizeof bytes, fp) == sizeof bytes;
}

static bool read_u32(FILE *fp, uint32_t *value)
{
    unsigned char bytes[4];

    if (fread(bytes, 1, sizeof bytes, fp) != sizeof bytes) {
        return false;
    }
    *value = (uint32_t)bytes[0]
           | ((uint32_t)bytes[1] << 8)
           | ((uint32_t)bytes[2] << 16)
           | ((uint32_t)bytes[3] << 24);
    return true;
}

/* ---------- save ---------- */

static bool write_records(FILE *fp, const StudentList *list)
{
    if (fwrite(MAGIC, 1, MAGIC_SIZE, fp) != MAGIC_SIZE ||
        !write_u32(fp, (uint32_t)list->count)) {
        return false;
    }

    for (const Student *s = list->head; s != NULL; s = s->next) {
        char     name[STUDENT_NAME_SIZE] = {0};          /* zero padded */
        uint32_t hundredths = (uint32_t)(s->percentage * 100.0f + 0.5f);

        memcpy(name, s->name, strlen(s->name));
        if (!write_u32(fp, (uint32_t)s->roll) ||
            fwrite(name, 1, sizeof name, fp) != sizeof name ||
            !write_u32(fp, hundredths)) {
            return false;
        }
    }
    return true;
}

Status stud_save(const StudentList *list, const char *path)
{
    char temp_path[PATH_SIZE];
    int  length = snprintf(temp_path, sizeof temp_path, "%s.tmp", path);
    if (length < 0 || (size_t)length >= sizeof temp_path) {
        return STATUS_INVALID_INPUT;                    /* path too long */
    }

    FILE *fp = fopen(temp_path, "wb");
    if (fp == NULL) {
        return STATUS_IO_ERROR;
    }

    bool ok = write_records(fp, list);
    if (fclose(fp) != 0) {
        ok = false;
    }

    /* Only replace the real file once the new one was written completely. */
    if (!ok || !platform_replace_file(temp_path, path)) {
        remove(temp_path);
        return STATUS_IO_ERROR;
    }
    return STATUS_OK;
}

/* ---------- load ---------- */

/* Reads everything from an open file into `list`. */
static Status read_records(FILE *fp, StudentList *list)
{
    char     magic[MAGIC_SIZE];
    uint32_t count;

    if (fread(magic, 1, MAGIC_SIZE, fp) != MAGIC_SIZE ||
        memcmp(magic, MAGIC, MAGIC_SIZE) != 0 ||
        !read_u32(fp, &count)) {
        return STATUS_BAD_FORMAT;
    }

    for (uint32_t i = 0; i < count; i++) {
        uint32_t roll, hundredths;
        char     name[STUDENT_NAME_SIZE];

        if (!read_u32(fp, &roll) ||
            fread(name, 1, sizeof name, fp) != sizeof name ||
            !read_u32(fp, &hundredths)) {
            return STATUS_BAD_FORMAT;                   /* file is cut short */
        }
        if (roll > INT_MAX || hundredths > MAX_HUNDREDTHS ||
            memchr(name, '\0', sizeof name) == NULL) {
            return STATUS_BAD_FORMAT;
        }

        Student *node = NULL;
        Status   status = student_create(&node, (int)roll, name,
                                         (float)hundredths / 100.0f);
        if (status == STATUS_INVALID_INPUT) {
            return STATUS_BAD_FORMAT;                   /* e.g. roll 0, empty name */
        }
        if (status != STATUS_OK) {
            return status;
        }

        status = student_list_append(list, node);
        if (status != STATUS_OK) {
            free(node);
            return (status == STATUS_DUPLICATE_ROLL) ? STATUS_BAD_FORMAT : status;
        }
    }

    if (fgetc(fp) != EOF) {
        return STATUS_BAD_FORMAT;                       /* unexpected extra bytes */
    }
    return STATUS_OK;
}

Status stud_load(StudentList *list, const char *path)
{
    FILE *fp = fopen(path, "rb");
    if (fp == NULL) {
        /* No file yet simply means "nothing saved so far". */
        return (errno == ENOENT) ? STATUS_OK : STATUS_IO_ERROR;
    }

    Status status = read_records(fp, list);
    fclose(fp);

    if (status != STATUS_OK) {
        student_list_free(list);                        /* never keep half a file */
    }
    return status;
}

/* ---------- backup ---------- */

Status stud_backup(const char *path, const char *backup_path)
{
    FILE *in = fopen(path, "rb");
    if (in == NULL) {
        return STATUS_IO_ERROR;
    }
    FILE *out = fopen(backup_path, "wb");
    if (out == NULL) {
        fclose(in);
        return STATUS_IO_ERROR;
    }

    char   buffer[4096];
    size_t n;
    bool   ok = true;

    while ((n = fread(buffer, 1, sizeof buffer, in)) > 0) {
        if (fwrite(buffer, 1, n, out) != n) {
            ok = false;
            break;
        }
    }
    if (ferror(in)) {
        ok = false;
    }
    fclose(in);
    if (fclose(out) != 0) {
        ok = false;
    }
    return ok ? STATUS_OK : STATUS_IO_ERROR;
}
