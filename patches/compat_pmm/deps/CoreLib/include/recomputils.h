#ifndef __RECOMPUTILS_H__
#define __RECOMPUTILS_H__

#include "modding.h"

RECOMP_IMPORT("*", void* recomp_alloc(unsigned long size));
RECOMP_IMPORT("*", void recomp_free(void* memory));
RECOMP_IMPORT("*", int recomp_printf(const char* fmt, ...));

#endif
