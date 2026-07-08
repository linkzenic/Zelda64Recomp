#ifndef __DYNAMICU32ARRAY_H__
#define __DYNAMICU32ARRAY_H__

// Array of 32-bit integers that automatically expands as it fills.

#include "global.h"

typedef struct {
    u32 *data;
    size_t capacity;
    size_t count;
} DynamicU32Array;

void DynU32Arr_init(DynamicU32Array *dArr);

void DynU32Arr_clear(DynamicU32Array *dArr);

void DynU32Arr_destroyMembers(DynamicU32Array *dArr);

void DynU32Arr_push(DynamicU32Array *dArr, u32 value);

bool DynU32Arr_pop(DynamicU32Array *dArr);

bool DynU32Arr_removeByValue(DynamicU32Array *dArr, u32 value);

bool DynU32Arr_removeByIndex(DynamicU32Array *dArr, size_t index);

void DynU32Arr_resize(DynamicU32Array *dArr, size_t newCapacity);

void DynU32Arr_reserve(DynamicU32Array *dArr, size_t minimumCapacity);

#endif
