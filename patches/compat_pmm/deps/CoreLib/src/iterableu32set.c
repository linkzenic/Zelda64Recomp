#include "iterableu32set.h"

void IterU32Set_init(IterableU32Set *s) {
    DynU32Arr_init(&s->values);
    s->hashset = recomputil_create_u32_hashset();
}

void IterU32Set_destroyMembers(IterableU32Set *s) {
    DynU32Arr_destroyMembers(&s->values);
    recomputil_destroy_u32_hashset(s->hashset);
}

bool IterU32Set_insert(IterableU32Set *s, u32 v) {
    if (recomputil_u32_hashset_insert(s->hashset, v)) {
        DynU32Arr_push(&s->values, v);
        return true;
    }

    return false;
}

bool IterU32Set_erase(IterableU32Set *s, u32 v) {
    if (recomputil_u32_hashset_erase(s->hashset, v)) {
        DynU32Arr_removeByValue(&s->values, v);
        return true;
    }

    return false;
}

void IterU32Set_clear(IterableU32Set *s) {
    for (size_t i = 0; i < s->values.count; ++i) {
        recomputil_u32_hashset_erase(s->hashset, s->values.data[i]);
    }

    DynU32Arr_clear(&s->values);
}

bool IterU32Set_contains(IterableU32Set *s, u32 v) {
    return recomputil_u32_hashset_contains(s->hashset, v);
}

size_t IterU32Set_count(IterableU32Set *s) {
    return s->values.count;
}
