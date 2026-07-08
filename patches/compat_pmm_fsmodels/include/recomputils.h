#ifndef __RECOMPUTILS_H__
#define __RECOMPUTILS_H__

#include "modding.h"
#include "PR/ultratypes.h"

RECOMP_IMPORT("*", void* recomp_alloc(unsigned long size));
RECOMP_IMPORT("*", void recomp_free(void* memory));
RECOMP_IMPORT("*", int recomp_printf(const char* fmt, ...));

// These functions let you get the return value of a function from within a return hook for that function.
// Calling these outside of a return hook will give an undefined result.
// Calling the function for a return type that doesn't match the function's actual return type also gives an undefined result.
RECOMP_IMPORT("*", s32 recomphook_get_return_s32());
RECOMP_IMPORT("*", u32 recomphook_get_return_u32());
RECOMP_IMPORT("*", void* recomphook_get_return_ptr());
RECOMP_IMPORT("*", s16 recomphook_get_return_s16());
RECOMP_IMPORT("*", u16 recomphook_get_return_u16());
RECOMP_IMPORT("*", s8 recomphook_get_return_s8());
RECOMP_IMPORT("*", u8 recomphook_get_return_u8());
RECOMP_IMPORT("*", s64 recomphook_get_return_s64());
RECOMP_IMPORT("*", u64 recomphook_get_return_u64());
RECOMP_IMPORT("*", float recomphook_get_return_float());
RECOMP_IMPORT("*", double recomphook_get_return_double());

typedef enum {
    // The dependency was found and the version requirement was met.
    DEPENDENCY_STATUS_FOUND = 0,
    // The ID given is not a dependency of the mod in question.
    DEPENDENCY_STATUS_INVALID_DEPENDENCY = 1,
    // The dependency was not found.
    DEPENDENCY_STATUS_NOT_FOUND = 2,
    // The dependency was found, but the version requirement was not met.
    DEPENDENCY_STATUS_WRONG_VERSION = 3
} DependencyStatus;

// Checks the status of the provided dependency. This is only relevant for optional dependencies,
// as the game wouldn't have started if a normal dependency wasn't met.
// Use the return value of this to prevent calling imported functions from optional dependencies that are missing.
// Try to avoid calling this function repeatedly, as it may incur some overhead. Instead, call it once at startup
// and store the result into a variable for reading later on.
RECOMP_IMPORT("*", DependencyStatus recomp_is_dependency_met(const char* dependency_id));

#endif
