#include "dynamicu32array.h"
#include "dynamicdataarray.h"
#include "stringu32dictionary.h"
#include "iterableu32set.h"
#include "../include/utils.h"
#include "modding.h"
#include "recomputils.h"

// ------------------------
// Utility Function Exports
// ------------------------

RECOMP_EXPORT int YAZMTCore_Utils_MemCmp(void *dest, const void *src, size_t size) {
    return Utils_MemCmp(dest, src, size);
}

RECOMP_EXPORT char *YAZMTCore_Utils_StrDup(const char* s) {
    return Utils_StrDup(s);
}

// -----------------------
// DynimicU32Array Exports
// -----------------------

RECOMP_EXPORT DynamicU32Array *YAZMTCore_DynamicU32Array_new() {
    DynamicU32Array *d = recomp_alloc(sizeof(DynamicU32Array));
    DynU32Arr_init(d);
    return d;
}

RECOMP_EXPORT void YAZMTCore_DynamicU32Array_destroy(DynamicU32Array *d) {
    if (d) {
        DynU32Arr_destroyMembers(d);
        recomp_free(d);
    }
}

RECOMP_EXPORT void YAZMTCore_DynamicU32Array_clear(DynamicU32Array *d) {
    DynU32Arr_clear(d);
}

RECOMP_EXPORT void YAZMTCore_DynamicU32Array_push(DynamicU32Array *d, u32 v) {
    DynU32Arr_push(d, v);
}

RECOMP_EXPORT bool YAZMTCore_DynamicU32Array_pop(DynamicU32Array *d) {
    return DynU32Arr_pop(d);
}

RECOMP_EXPORT bool YAZMTCore_DynamicU32Array_removeByValue(DynamicU32Array *d, u32 v) {
    return DynU32Arr_removeByValue(d, v);
}

RECOMP_EXPORT bool YAZMTCore_DynamicU32Array_removeByIndex(DynamicU32Array *d, size_t i) {
    return DynU32Arr_removeByIndex(d, i);
}

RECOMP_EXPORT void YAZMTCore_DynamicU32Array_reserve(DynamicU32Array *d, size_t minimum) {
    DynU32Arr_reserve(d, minimum);
}

RECOMP_EXPORT u32 *YAZMTCore_DynamicU32Array_data(DynamicU32Array *d) {
    return d->data;
}

RECOMP_EXPORT size_t YAZMTCore_DynamicU32Array_size(DynamicU32Array *d) {
    return d->count;
}

// ------------------------
// DynamicDataArray exports
// ------------------------

RECOMP_EXPORT DynamicDataArray *YAZMTCore_DynamicDataArray_new(size_t elementSize) {
    DynamicDataArray *d = recomp_alloc(sizeof(DynamicDataArray));
    DynDataArr_init(d, elementSize, 0);
    return d;
}

RECOMP_EXPORT void YAZMTCore_DynamicDataArray_destroy(DynamicDataArray *d) {
    if (d) {
        DynDataArr_destroyMembers(d);
        recomp_free(d);
    }
}

RECOMP_EXPORT void YAZMTCore_DynamicDataArray_clear(DynamicDataArray *d) {
    DynDataArr_clear(d);
}

RECOMP_EXPORT void *YAZMTCore_DynamicDataArray_createElement(DynamicDataArray *d) {
    return DynDataArr_createElement(d);
}

RECOMP_EXPORT void *YAZMTCore_DynamicDataArray_getElement(DynamicDataArray *d, size_t i) {
    return DynDataArr_get(d, i);
}

RECOMP_EXPORT bool YAZMTCore_DynamicDataArray_setElement(DynamicDataArray *d, size_t i, const void *value) {
    return DynDataArr_set(d, i, value);
}

RECOMP_EXPORT void YAZMTCore_DynamicDataArray_push(DynamicDataArray *d, void *v) {
    DynDataArr_push(d, v);
}

RECOMP_EXPORT bool YAZMTCore_DynamicDataArray_pop(DynamicDataArray *d) {
    return DynDataArr_pop(d);
}

RECOMP_EXPORT void *YAZMTCore_DynamicDataArray_data(DynamicDataArray *d) {
    return d->data;
}

RECOMP_EXPORT size_t YAZMTCore_DynamicDataArray_size(DynamicDataArray *d) {
    return d->count;
}

// ----------------------
// IterableU32Set exports
// ----------------------

RECOMP_EXPORT IterableU32Set *YAZMTCore_IterableU32Set_new() {
    IterableU32Set *set = recomp_alloc(sizeof(IterableU32Set));
    IterU32Set_init(set);
    return set;
}

RECOMP_EXPORT void YAZMTCore_IterableU32Set_destroy(IterableU32Set *set) {
    if (set) {
        IterU32Set_destroyMembers(set);
        recomp_free(set);
    }
}

RECOMP_EXPORT bool YAZMTCore_IterableU32Set_insert(IterableU32Set *set, u32 value) {
    return IterU32Set_insert(set, value);
}

RECOMP_EXPORT bool YAZMTCore_IterableU32Set_erase(IterableU32Set *set, u32 value) {
    return IterU32Set_erase(set, value);
}

RECOMP_EXPORT bool YAZMTCore_IterableU32Set_contains(IterableU32Set *set, u32 value) {
    return IterU32Set_contains(set, value);
}

RECOMP_EXPORT size_t YAZMTCore_IterableU32Set_size(IterableU32Set *set) {
    return IterU32Set_count(set);
}

RECOMP_EXPORT void YAZMTCore_IterableU32Set_clear(IterableU32Set *set) {
    IterU32Set_clear(set);
}

RECOMP_EXPORT const u32 *YAZMTCore_IterableU32Set_values(IterableU32Set *set) {
    return set->values.data;
}

// ---------------------------
// StringU32Dictionary exports
// ---------------------------

RECOMP_EXPORT StringU32Dictionary *YAZMTCore_StringU32Dictionary_new() {
    StringU32Dictionary *dict = recomp_alloc(sizeof(StringU32Dictionary));

    StringU32Dictionary_init(dict);

    return dict;
}

RECOMP_EXPORT void YAZMTCore_StringU32Dictionary_destroy(StringU32Dictionary *dict) {
    if (!dict) {
        return;
    }

    StringU32Dictionary_destroyMembers(dict);

    recomp_free(dict);
}

RECOMP_EXPORT void YAZMTCore_StringU32Dictionary_clear(StringU32Dictionary *dict) {
    StringU32Dictionary_clear(dict);
}

RECOMP_EXPORT bool YAZMTCore_StringU32Dictionary_set(StringU32Dictionary *dict, const char *key, u32 value) {
    return StringU32Dictionary_set(dict, key, value);
}

RECOMP_EXPORT bool YAZMTCore_StringU32Dictionary_get(StringU32Dictionary *dict, const char *key, u32 *out) {
    return StringU32Dictionary_get(dict, key, out);
}

RECOMP_EXPORT bool YAZMTCore_StringU32Dictionary_unset(StringU32Dictionary *dict, const char *key) {
    return StringU32Dictionary_unset(dict, key);
}

RECOMP_EXPORT bool YAZMTCore_StringU32Dictionary_contains(StringU32Dictionary *dict, const char *key) {
    return StringU32Dictionary_contains(dict, key);
}
