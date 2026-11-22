#include "util.h"

#include <ctype.h>
#include <errno.h>
#include <float.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

char *util_trim(char *s)
{
    while (isspace((unsigned char)*s)) {
        s++;
    }

    size_t len = strlen(s);
    while (len > 0 && isspace((unsigned char)s[len - 1])) {
        s[--len] = '\0';
    }
    return s;
}

bool util_parse_int(const char *s, int *out)
{
    char *end = NULL;

    errno = 0;
    long value = strtol(s, &end, 10);

    if (end == s || errno == ERANGE || value < INT_MIN || value > INT_MAX) {
        return false;
    }
    while (isspace((unsigned char)*end)) {
        end++;
    }
    if (*end != '\0') {
        return false;               /* trailing junk such as "12abc" */
    }

    *out = (int)value;
    return true;
}

bool util_parse_float(const char *s, float *out)
{
    char *end = NULL;

    errno = 0;
    double value = strtod(s, &end);

    if (end == s || errno == ERANGE) {
        return false;
    }
    /* This comparison is false for NaN, so it rejects "nan" and "inf" too. */
    if (!(value >= -FLT_MAX && value <= FLT_MAX)) {
        return false;
    }
    while (isspace((unsigned char)*end)) {
        end++;
    }
    if (*end != '\0') {
        return false;
    }

    *out = (float)value;
    return true;
}

int util_stricmp(const char *a, const char *b)
{
    while (*a != '\0' && *b != '\0') {
        int ca = tolower((unsigned char)*a);
        int cb = tolower((unsigned char)*b);
        if (ca != cb) {
            return ca - cb;
        }
        a++;
        b++;
    }
    return tolower((unsigned char)*a) - tolower((unsigned char)*b);
}

bool util_icontains(const char *haystack, const char *needle)
{
    size_t needle_len = strlen(needle);
    if (needle_len == 0) {
        return true;
    }

    for (; *haystack != '\0'; haystack++) {
        size_t i = 0;
        while (i < needle_len && haystack[i] != '\0' &&
               tolower((unsigned char)haystack[i]) ==
               tolower((unsigned char)needle[i])) {
            i++;
        }
        if (i == needle_len) {
            return true;
        }
    }
    return false;
}
