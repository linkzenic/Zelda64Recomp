#include "recomputils.h"
#include "dynamicu32array.h"

#define DEFAULT_CAPACITY 1
#define NEXT_CAPACITY(current) ((current == 0) ? DEFAULT_CAPACITY : (current + (current + 1) / 2))

void DynU32Arr_resize(DynamicU32Array *dArr, size_t newCapacity) {
    if (newCapacity > 0) {
        u32 *newData = recomp_alloc(sizeof(*newData) * newCapacity);

        size_t min = dArr->count;
        if (min > newCapacity) {
            min = newCapacity;
        }

        for (size_t i = 0; i < min; ++i) {
            newData[i] = dArr->data[i];
        }

        recomp_free(dArr->data);

        dArr->count = min;
        dArr->capacity = newCapacity;
        dArr->data = newData;
    }
}

void DynU32Arr_reserve(DynamicU32Array *dArr, size_t minimumCapacity) {
    size_t capacity = dArr->capacity;
    if (minimumCapacity > capacity) {
        DynU32Arr_resize(dArr, capacity);
    }
}

void DynU32Arr_init(DynamicU32Array *dArr) {
    dArr->capacity = 0;
    dArr->count = 0;
    dArr->data = NULL;
}

void DynU32Arr_clear(DynamicU32Array *dArr) {
    dArr->count = 0;
}

void DynU32Arr_destroyMembers(DynamicU32Array *dArr) {
    recomp_free(dArr->data);
    dArr->capacity = 0;
    dArr->count = 0;
    dArr->data = NULL;
}

void DynU32Arr_push(DynamicU32Array *dArr, u32 value) {
    size_t newCount = dArr->count + 1;

    if (newCount > dArr->capacity) {
        DynU32Arr_resize(dArr, NEXT_CAPACITY(dArr->capacity));
    }

    dArr->data[dArr->count] = value;

    dArr->count = newCount;
}

bool DynU32Arr_pop(DynamicU32Array *dArr) {
    if (dArr->count > 0) {
        dArr->count--;
        return true;
    }

    return false;
}

bool DynU32Arr_removeByValue(DynamicU32Array *dArr, u32 value) {
    if (dArr->count > 0) {
        u32 *arr = dArr->data;
        for (size_t i = 0; i < dArr->count; ++i) {
            if (arr[i] == value) {
                DynU32Arr_removeByIndex(dArr, i);
                return true;
            }
        }
    }

    return false;
}

bool DynU32Arr_removeByIndex(DynamicU32Array *dArr, size_t index) {
    if (index >= dArr->count) {
        return false;
    }

    for (size_t i = index + 1; i < dArr->count; ++i) {
        dArr->data[i - 1] = dArr->data[i];
    }

    dArr->count--;

    return true;
}
