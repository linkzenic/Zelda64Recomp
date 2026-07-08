#ifndef __MODDING_H__
#define __MODDING_H__

// Do not edit these defines. They use special section names that the recomp mod tool recognizes for specific modding functionality.

#ifdef __cplusplus
#   define EXTERNC extern "C"
#else
#   define EXTERNC
#endif

// The RECOMP_IMPORT has the following attributes:
//   noinline: Prevent import function definitions from being inlined, allowing the mod tool to find relocs. 
//   weak: Allow import definitions to be in headers without triggering multiple definition errors.
#define RECOMP_IMPORT(mod, func) \
    _Pragma("GCC diagnostic push") \
    _Pragma("GCC diagnostic ignored \"-Wunused-parameter\"") \
    _Pragma("GCC diagnostic ignored \"-Wreturn-type\"") \
    EXTERNC __attribute__((noinline, weak, used, section(".recomp_import." mod))) func {} \
    _Pragma("GCC diagnostic pop")

#define RECOMP_EXPORT EXTERNC __attribute__((retain, section(".recomp_export")))

#define RECOMP_PATCH EXTERNC __attribute__((retain, section(".recomp_patch")))

#define RECOMP_FORCE_PATCH EXTERNC __attribute__((retain, section(".recomp_force_patch")))

#define RECOMP_DECLARE_EVENT(func) \
    _Pragma("GCC diagnostic push") \
    _Pragma("GCC diagnostic ignored \"-Wunused-parameter\"") \
    EXTERNC __attribute__((retain, noinline, weak, used, section(".recomp_event"))) void func {} \
    _Pragma("GCC diagnostic pop")

#define RECOMP_CALLBACK(mod, event) __attribute__((retain, section(".recomp_callback." mod ":" #event)))

#define RECOMP_HOOK(func) __attribute__((retain, section(".recomp_hook." func)))

#define RECOMP_HOOK_RETURN(func) __attribute__((retain, section(".recomp_hook_return." func)))

#endif
