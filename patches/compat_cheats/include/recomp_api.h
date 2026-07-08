#ifndef ZELDA_COMPAT_CHEATS_RECOMP_API_H
#define ZELDA_COMPAT_CHEATS_RECOMP_API_H

#include "patches.h"

#ifdef RECOMP_EXPORT
#undef RECOMP_EXPORT
#endif
#ifdef RECOMP_PATCH
#undef RECOMP_PATCH
#endif
#ifdef RECOMP_FORCE_PATCH
#undef RECOMP_FORCE_PATCH
#endif
#ifdef RECOMP_CALLBACK
#undef RECOMP_CALLBACK
#endif
#ifdef RECOMP_HOOK
#undef RECOMP_HOOK
#endif
#ifdef RECOMP_HOOK_RETURN
#undef RECOMP_HOOK_RETURN
#endif
#ifdef RECOMP_IMPORT
#undef RECOMP_IMPORT
#endif

#ifdef __cplusplus
#define ZELDA_COMPAT_CHEATS_EXTERNC extern "C"
#else
#define ZELDA_COMPAT_CHEATS_EXTERNC
#endif

#define RECOMP_IMPORT(mod, func) ZELDA_COMPAT_CHEATS_EXTERNC func
#define RECOMP_EXPORT ZELDA_COMPAT_CHEATS_EXTERNC __attribute__((retain, section(".recomp_export")))
#define RECOMP_PATCH ZELDA_COMPAT_CHEATS_EXTERNC __attribute__((retain, section(".recomp_patch")))
#define RECOMP_FORCE_PATCH ZELDA_COMPAT_CHEATS_EXTERNC __attribute__((retain, section(".recomp_force_patch")))
#define RECOMP_CALLBACK(mod, event) __attribute__((retain, section(".recomp_callback." mod ":" #event)))
#define RECOMP_HOOK(func) __attribute__((retain, section(".recomp_hook." func)))
#define RECOMP_HOOK_RETURN(func) __attribute__((retain, section(".recomp_hook_return." func)))

RECOMP_IMPORT(".", unsigned long recomp_android_compat_cheats_get_config_u32(const char* key));

static inline unsigned long recomp_get_config_u32(const char* key) {
    return recomp_android_compat_cheats_get_config_u32(key);
}

#endif
