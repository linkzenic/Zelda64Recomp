#include "recomputils.h"
#include "dynamicdataarray.h"
#include "../include/utils.h"
#include "libc/string.h"

#define DEFAULT_CAPACITY 16
#define NEXT_CAPACITY(current) ((current == 0) ? DEFAULT_CAPACITY : ((current + 1) * 3 / 2))

void resizeDynDataArr(DynamicDataArray *dArr, size_t newCapacity) {
    if (newCapacity > 0) {
        size_t newByteSize = newCapacity * dArr->elementSize;
        u32 *newData = recomp_alloc(newByteSize);
        Lib_MemSet(newData, 0, newByteSize);

        size_t min = dArr->count;
        if (min > newCapacity) {
            min = newCapacity;
        }

        size_t newCount = dArr->count;
        if (newCapacity < dArr->count) {
            newCount = newCapacity;
        }

        if (dArr->data) {
            memcpy(newData, dArr->data, min * dArr->elementSize);
            recomp_free(dArr->data);
        }

        dArr->count = newCount;
        dArr->capacity = newCapacity;
        dArr->data = newData;
    }
}

void resetStruct(DynamicDataArray *dArr) {
    dArr->capacity = 0;
    dArr->count = 0;
    dArr->elementSize = 0;
    dArr->data = NULL;
}

void DynDataArr_init(DynamicDataArray *dArr, size_t elementSize, size_t initialCapacity) {
    resetStruct(dArr);

    dArr->elementSize = elementSize;

    if (initialCapacity) {
        resizeDynDataArr(dArr, initialCapacity);
    }
}

void DynDataArr_clear(DynamicDataArray *dArr) {
    dArr->count = 0;
}

void DynDataArr_destroyMembers(DynamicDataArray *dArr) {
    recomp_free(dArr->data);
    resetStruct(dArr);
}

void *DynDataArr_createElement(DynamicDataArray *dArr) {
    if (dArr->elementSize < 1) {
        return NULL;
    }

    size_t newCount = dArr->count + 1;

    if (newCount > dArr->capacity) {
        resizeDynDataArr(dArr, NEXT_CAPACITY(dArr->capacity));
    }

    u8 *data = dArr->data;

    void *element = DynDataArr_get(dArr, dArr->count);

    dArr->count = newCount;

    return element;
}

void *DynDataArr_get(DynamicDataArray *dArr, size_t index) {
    if (dArr->elementSize < 1) {
        return NULL;
    }

    u8 *data = dArr->data;

    return &data[dArr->elementSize * index];
}

bool DynDataArr_set(DynamicDataArray *dArr, size_t index, const void *value) {
    if (dArr->elementSize < 1 || index >= dArr->count) {
        return false;
    }

    memcpy(DynDataArr_get(dArr, index), value, dArr->elementSize);
    return true;
}

void DynDataArr_push(DynamicDataArray *dArr, void *value) {
    if (dArr->elementSize < 1) {
        return;
    }

    memcpy(DynDataArr_createElement(dArr), value, dArr->elementSize);
}

bool DynDataArr_pop(DynamicDataArray *dArr) {
    if (dArr->count < 1) {
        return false;
    }

    dArr->count--;

    return true;
}

bool DynDataArr_removeByIndex(DynamicDataArray *dArr, size_t index) {
    if (dArr->elementSize < 1 || index >= dArr->count) {
        return false;
    }

    u8 *data = dArr->data;

    for (size_t i = index + 1; i < dArr->count - 1; ++i) {
        memcpy(DynDataArr_get(dArr, i), DynDataArr_get(dArr, i + 1), dArr->elementSize);
    }

    dArr->count--;

    return true;
}

bool DynDataArr_removeByValue(DynamicDataArray *dArr, const void *value) {
    if (dArr->elementSize < 1) {
        return false;
    }

    for (size_t i = 0; i < dArr->count; ++i) {
        if (Utils_MemCmp(DynDataArr_get(dArr, i), value, dArr->elementSize) != 0) {
            DynDataArr_removeByIndex(dArr, i);
            dArr->count--;
            return true;
        }
    }

    return false;
}
