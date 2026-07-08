#include "../include/utils.h"
#include "libc/string.h"
#include "recomputils.h"

int Utils_MemCmp(const void *a, const void *b, size_t size) {
    const char *c = a;
    const char *d = b;

    for (size_t i = 0; i < size; ++i) {
        if (*c != *d) {
            return *c - *d;
        }

        c++;
        d++;
    }

    return 0;
}

char *Utils_StrDup(const char *s) {
    char *newStr = recomp_alloc(strlen(s) + 1);

    char *c = newStr;

    while (*s != '\0') {
        *c = *s;
        s++;
        c++;
    }

    *c = '\0';

    return newStr;
}
