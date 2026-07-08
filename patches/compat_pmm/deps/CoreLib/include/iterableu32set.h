#ifndef __ITERABLEU32SET_H__
#define __ITERABLEU32SET_H__

#include "global.h"
#include "recompdata.h"
#include "dynamicu32array.h"

typedef struct {
    DynamicU32Array values;
    U32HashsetHandle hashset;
} IterableU32Set;

void IterU32Set_init(IterableU32Set *s);

void IterU32Set_destroyMembers(IterableU32Set *s);

bool IterU32Set_insert(IterableU32Set *s, u32 v);

bool IterU32Set_erase(IterableU32Set *s, u32 v);

void IterU32Set_clear(IterableU32Set *s);

bool IterU32Set_contains(IterableU32Set *s, u32 v);

size_t IterU32Set_count(IterableU32Set *s);

#endif
