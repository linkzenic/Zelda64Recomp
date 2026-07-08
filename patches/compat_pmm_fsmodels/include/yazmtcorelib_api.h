#ifndef __YAZMT_CORELIB_API_H__
#define __YAZMT_CORELIB_API_H__

#include "global.h"
#include "modding.h"

#define YAZMT_CORELIB_MOD_NAME "yazmt_mm_corelib"

typedef struct YAZMTCore_DynamicU32Array YAZMTCore_DynamicU32Array;
typedef struct YAZMTCore_DynamicDataArray YAZMTCore_DynamicDataArray;
typedef struct YAZMTCore_IterableU32Set YAZMTCore_IterableU32Set;
typedef struct YAZMTCore_StringU32Dictionary YAZMTCore_StringU32Dictionary;

RECOMP_IMPORT(YAZMT_CORELIB_MOD_NAME, void YAZMTCore_Utils_MemCpy(void *dest, const void *src, size_t size));

RECOMP_IMPORT(YAZMT_CORELIB_MOD_NAME, int YAZMTCore_Utils_MemCmp(void *dest, const void *src, size_t size));

RECOMP_IMPORT(YAZMT_CORELIB_MOD_NAME, YAZMTCore_DynamicU32Array *YAZMTCore_DynamicU32Array_new());

RECOMP_IMPORT(YAZMT_CORELIB_MOD_NAME, void YAZMTCore_DynamicU32Array_destroy(YAZMTCore_DynamicU32Array *dynArr));

RECOMP_IMPORT(YAZMT_CORELIB_MOD_NAME, void YAZMTCore_DynamicU32Array_clear(YAZMTCore_DynamicU32Array *dynArr));

RECOMP_IMPORT(YAZMT_CORELIB_MOD_NAME, void YAZMTCore_DynamicU32Array_push(YAZMTCore_DynamicU32Array *dynArr, u32 val));

RECOMP_IMPORT(YAZMT_CORELIB_MOD_NAME, bool YAZMTCore_DynamicU32Array_pop(YAZMTCore_DynamicU32Array *dynArr));

RECOMP_IMPORT(YAZMT_CORELIB_MOD_NAME, bool YAZMTCore_DynamicU32Array_removeByValue(YAZMTCore_DynamicU32Array *dynArr, u32 val));

RECOMP_IMPORT(YAZMT_CORELIB_MOD_NAME, bool YAZMTCore_DynamicU32Array_removeByIndex(YAZMTCore_DynamicU32Array *dynArr, size_t i));

RECOMP_IMPORT(YAZMT_CORELIB_MOD_NAME, void YAZMTCore_DynamicU32Array_reserve(YAZMTCore_DynamicU32Array *dynArr, size_t minimum));

RECOMP_IMPORT(YAZMT_CORELIB_MOD_NAME, u32 *YAZMTCore_DynamicU32Array_data(YAZMTCore_DynamicU32Array *dynArr));

RECOMP_IMPORT(YAZMT_CORELIB_MOD_NAME, size_t YAZMTCore_DynamicU32Array_size(YAZMTCore_DynamicU32Array *dynArr));

RECOMP_IMPORT(YAZMT_CORELIB_MOD_NAME, YAZMTCore_DynamicDataArray *YAZMTCore_DynamicDataArray_new(size_t elementSize));

RECOMP_IMPORT(YAZMT_CORELIB_MOD_NAME, void YAZMTCore_DynamicDataArray_destroy(YAZMTCore_DynamicDataArray *dynArr));

RECOMP_IMPORT(YAZMT_CORELIB_MOD_NAME, void YAZMTCore_DynamicDataArray_clear(YAZMTCore_DynamicDataArray *dynArr));

RECOMP_IMPORT(YAZMT_CORELIB_MOD_NAME, void *YAZMTCore_DynamicDataArray_createElement(YAZMTCore_DynamicDataArray *dynArr));

RECOMP_IMPORT(YAZMT_CORELIB_MOD_NAME, void *YAZMTCore_DynamicDataArray_getElement(YAZMTCore_DynamicDataArray *dynArr, size_t i));

RECOMP_IMPORT(YAZMT_CORELIB_MOD_NAME, bool YAZMTCore_DynamicDataArray_setElement(YAZMTCore_DynamicDataArray *dynArr, size_t i, const void *val));

RECOMP_IMPORT(YAZMT_CORELIB_MOD_NAME, void YAZMTCore_DynamicDataArray_push(YAZMTCore_DynamicDataArray *dynArr, void *val));

RECOMP_IMPORT(YAZMT_CORELIB_MOD_NAME, bool YAZMTCore_DynamicDataArray_pop(YAZMTCore_DynamicDataArray *dynArr));

RECOMP_IMPORT(YAZMT_CORELIB_MOD_NAME, void *YAZMTCore_DynamicDataArray_data(YAZMTCore_DynamicDataArray *dynArr));

RECOMP_IMPORT(YAZMT_CORELIB_MOD_NAME, size_t YAZMTCore_DynamicDataArray_size(YAZMTCore_DynamicDataArray *dynArr));

RECOMP_IMPORT(YAZMT_CORELIB_MOD_NAME, YAZMTCore_IterableU32Set *YAZMTCore_IterableU32Set_new());

RECOMP_IMPORT(YAZMT_CORELIB_MOD_NAME, void YAZMTCore_IterableU32Set_destroy(YAZMTCore_IterableU32Set *set));

RECOMP_IMPORT(YAZMT_CORELIB_MOD_NAME, bool YAZMTCore_IterableU32Set_insert(YAZMTCore_IterableU32Set *set, u32 val));

RECOMP_IMPORT(YAZMT_CORELIB_MOD_NAME, bool YAZMTCore_IterableU32Set_erase(YAZMTCore_IterableU32Set *set, u32 val));

RECOMP_IMPORT(YAZMT_CORELIB_MOD_NAME, bool YAZMTCore_IterableU32Set_contains(YAZMTCore_IterableU32Set *set, u32 val));

RECOMP_IMPORT(YAZMT_CORELIB_MOD_NAME, bool YAZMTCore_IterableU32Set_size(YAZMTCore_IterableU32Set *set));

RECOMP_IMPORT(YAZMT_CORELIB_MOD_NAME, void YAZMTCore_IterableU32Set_clear(YAZMTCore_IterableU32Set *set));

RECOMP_IMPORT(YAZMT_CORELIB_MOD_NAME, const u32 *YAZMTCore_IterableU32Set_values(YAZMTCore_IterableU32Set *set));

RECOMP_IMPORT(YAZMT_CORELIB_MOD_NAME, YAZMTCore_StringU32Dictionary *YAZMTCore_StringU32Dictionary_new());

RECOMP_IMPORT(YAZMT_CORELIB_MOD_NAME, void YAZMTCore_StringU32Dictionary_destroy(YAZMTCore_StringU32Dictionary *dict));

RECOMP_IMPORT(YAZMT_CORELIB_MOD_NAME, void YAZMTCore_StringU32Dictionary_clear(YAZMTCore_StringU32Dictionary *dict));

RECOMP_IMPORT(YAZMT_CORELIB_MOD_NAME, bool YAZMTCore_StringU32Dictionary_set(YAZMTCore_StringU32Dictionary *dict, const char *key, u32 val));

RECOMP_IMPORT(YAZMT_CORELIB_MOD_NAME, bool YAZMTCore_StringU32Dictionary_get(YAZMTCore_StringU32Dictionary *dict, const char *key, u32 *out));

RECOMP_IMPORT(YAZMT_CORELIB_MOD_NAME, bool YAZMTCore_StringU32Dictionary_unset(YAZMTCore_StringU32Dictionary *dict, const char *key));

RECOMP_IMPORT(YAZMT_CORELIB_MOD_NAME, bool YAZMTCore_StringU32Dictionary_contains(YAZMTCore_StringU32Dictionary *dict, const char *key));

#endif
