#ifndef __DYNAMICDATAARRAY_H__
#define __DYNAMICDATAARRAY_H__

// Array of arbitrarily sized values that automatically expands as it fills.

#include "global.h"

typedef struct {
    void *data;
    size_t capacity;
    size_t count;
    size_t elementSize;
} DynamicDataArray;

void DynDataArr_init(DynamicDataArray *dArr, size_t elementSize, size_t initialCapacity);

void DynDataArr_clear(DynamicDataArray *dArr);

void DynDataArr_destroyMembers(DynamicDataArray *dArr);

void *DynDataArr_createElement(DynamicDataArray *dArr);

void *DynDataArr_get(DynamicDataArray *dArr, size_t index);

bool DynDataArr_set(DynamicDataArray *dArr, size_t index, const void *value);

void DynDataArr_push(DynamicDataArray *dArr, void *value);

bool DynDataArr_pop(DynamicDataArray *dArr);

bool DynDataArr_removeByIndex(DynamicDataArray *dArr, size_t index);

bool DynDataArr_removeByValue(DynamicDataArray *dArr, const void *value);

#endif