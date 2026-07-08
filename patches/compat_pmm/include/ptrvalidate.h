#ifndef PTR_VALIDATE_H
#define PTR_VALIDATE_H

#define PTR_VAL_VOID_RET

#ifdef DEBUG

#define SETUP_PTR_VALIDATION(ptrSetVarName, structType)                               \
    static char sStructToValidateName__[] = #structType;                              \
    static U32HashsetHandle ptrSetVarName;                                            \
    RECOMP_CALLBACK(".", _internal_initHashObjects) void init_##ptrSetVarName(void) { \
        ptrSetVarName = recomputil_create_u32_hashset();                              \
    }                                                                                 \
    static bool isValidPtr__(const structType *ptr) {                                 \
        return recomputil_u32_hashset_contains(ptrSetVarName, (uintptr_t)ptr);        \
    }                                                                                 \
    static void addPtrToValidSet__(const structType *ptr) {                           \
        recomputil_u32_hashset_insert(ptrSetVarName, (uintptr_t)ptr);                 \
    }                                                                                 \
    static void removePtrFromValidSet__(const structType *ptr) {                      \
        recomputil_u32_hashset_erase(ptrSetVarName, (uintptr_t)ptr);                  \
    }

#define ADD_VALIDATED_PTR(ptr)                                           \
    if (isValidPtr__(ptr)) {                                             \
        Logger_printWarning("Pointer already exists in validated set!"); \
    }                                                                    \
    addPtrToValidSet__(ptr)

#define INVALIDATE_PTR(ptr)                                              \
    if (!isValidPtr__(ptr)) {                                            \
        Logger_printWarning("Pointer is already invalid!"); \
    }                                                                    \
    removePtrFromValidSet__(ptr);

#define RETURN_IF_INVALID_PTR(ptr, ...)                                             \
    if (!isValidPtr__(ptr)) {                                                       \
        Logger_printError("Received invalid %s pointer.", sStructToValidateName__); \
        return __VA_ARGS__;                                                         \
    }                                                                               \
    (void)0

#else

#define SETUP_PTR_VALIDATION(ptrSetVarName, structType)
#define ADD_VALIDATED_PTR(ptr)
#define RETURN_IF_INVALID_PTR(ptr, ...)
#define INVALIDATE_PTR(ptr)

#endif
#endif
