#include "libc/stddef.h"

void* memset(void* dest, int value, size_t size) {
    unsigned char* out = (unsigned char*)dest;
    for (size_t i = 0; i < size; ++i) {
        out[i] = (unsigned char)value;
    }
    return dest;
}

void* memcpy(void* dest, const void* src, size_t size) {
    unsigned char* out = (unsigned char*)dest;
    const unsigned char* in = (const unsigned char*)src;
    for (size_t i = 0; i < size; ++i) {
        out[i] = in[i];
    }
    return dest;
}

size_t strlen(const char* str) {
    size_t len = 0;
    while (str[len] != '\0') {
        ++len;
    }
    return len;
}

int strcmp(const char* lhs, const char* rhs) {
    while (*lhs != '\0' && *lhs == *rhs) {
        ++lhs;
        ++rhs;
    }

    return (int)(unsigned char)*lhs - (int)(unsigned char)*rhs;
}

char* strcpy(char* dest, const char* src) {
    char* out = dest;
    while ((*out++ = *src++) != '\0') {}
    return dest;
}
