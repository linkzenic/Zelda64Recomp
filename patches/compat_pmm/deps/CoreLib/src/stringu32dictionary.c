#include "global.h"
#include "stringu32dictionary.h"
#include "libc/string.h"
#include "recompdata.h"
#include "recomputils.h"
#include "dynamicdataarray.h"
#include "dynamicu32array.h"
#include "fnv.h"

u32 hasher(const void *data, size_t length) {
    return fnv_32a_buf(data, length, FNV1_32A_INIT);
}

void destroySlotMembers(StringU32DictionaryEntrySlot *slot) {
    recomp_free(slot->key);
    slot->key = NULL;
}

void StringU32Dictionary_init(StringU32Dictionary *dict) {
    dict->hashmap = recomputil_create_u32_memory_hashmap(sizeof(DynamicDataArray));
    DynU32Arr_init(&dict->keyHashes);
}

void StringU32Dictionary_destroyMembers(StringU32Dictionary *dict) {
    StringU32Dictionary_clear(dict);

    recomputil_destroy_u32_memory_hashmap(dict->hashmap);
}

void StringU32Dictionary_clear(StringU32Dictionary *dict) {
    for (size_t i = 0; i < dict->keyHashes.count; ++i) {
        u32 hash = dict->keyHashes.data[i];

        DynamicDataArray *slots = recomputil_u32_memory_hashmap_get(dict->hashmap, hash);

        if (slots) {
            StringU32DictionaryEntrySlot *slotData = slots->data;
            for (size_t j = 0; j < slots->count; ++j) {
                destroySlotMembers(&slotData[j]);
            }

            DynDataArr_destroyMembers(slots);
        }
    }
}

bool StringU32Dictionary_set(StringU32Dictionary *dict, const char *key, u32 value) {
    if (!key) {
        return false;
    }

    size_t keyLen = strlen(key);

    u32 hash = hasher(key, keyLen);

    DynamicDataArray *dynMemArr = NULL;

    if (recomputil_u32_memory_hashmap_create(dict->hashmap, hash)) {
        dynMemArr = recomputil_u32_memory_hashmap_get(dict->hashmap, hash);

        // collisions should be rare, so start with 1 slot
        DynDataArr_init(dynMemArr, sizeof(StringU32DictionaryEntrySlot), 1);
    }

    if (!dynMemArr) {
        dynMemArr = recomputil_u32_memory_hashmap_get(dict->hashmap, hash);
    }

    StringU32DictionaryEntrySlot *slot = NULL;

    for (size_t i = 0; i < dynMemArr->count && !slot; ++i) {
        StringU32DictionaryEntrySlot *candidate = DynDataArr_get(dynMemArr, i);

        if (candidate && strcmp(key, candidate->key) == 0) {
            slot = candidate;
        }
    }

    bool wasElementCreated = !!slot;

    if (!wasElementCreated) {
        slot = DynDataArr_createElement(dynMemArr);
        slot->key = recomp_alloc((keyLen + 1) * sizeof(*key));
        strcpy(slot->key, key);
    }

    if (slot) {
        slot->value = value;

        DynU32Arr_push(&dict->keyHashes, hash);
    }

    return wasElementCreated;
}

bool StringU32Dictionary_get(StringU32Dictionary *dict, const char *key, u32 *out) {
    if (!key) {
        return false;
    }

    size_t keyLen = strlen(key);

    u32 hash = hasher(key, keyLen);

    DynamicDataArray *slots = recomputil_u32_memory_hashmap_get(dict->hashmap, hash);

    if (!slots) {
        return false;
    }

    for (size_t i = 0; i < slots->count; ++i) {
        StringU32DictionaryEntrySlot *candidate = DynDataArr_get(slots, i);

        if (candidate && strcmp(key, candidate->key) == 0) {
            *out = candidate->value;
            return true;
        }
    }

    return false;
}

bool StringU32Dictionary_contains(StringU32Dictionary *dict, const char *key) {
    if (!key) {
        return false;
    }

    size_t keyLen = strlen(key);

    u32 hash = hasher(key, keyLen);

    DynamicDataArray *slots = recomputil_u32_memory_hashmap_get(dict->hashmap, hash);

    if (!slots) {
        return false;
    }

    for (size_t i = 0; i < slots->count; ++i) {
        StringU32DictionaryEntrySlot *candidate = DynDataArr_get(slots, i);

        if (candidate && strcmp(key, candidate->key) == 0) {
            return true;
        }
    }

    return false;
}

bool StringU32Dictionary_unset(StringU32Dictionary *dict, const char *key) {
    if (!key) {
        return false;
    }

    size_t keyLen = strlen(key);

    u32 hash = hasher(key, keyLen);

    DynamicDataArray *slots = recomputil_u32_memory_hashmap_get(dict->hashmap, hash);

    if (!slots) {
        return false;
    }

    for (size_t i = 0; i < slots->count; ++i) {
        StringU32DictionaryEntrySlot *candidate = DynDataArr_get(slots, i);

        if (candidate && strcmp(key, candidate->key) == 0) {
            destroySlotMembers(candidate);
            DynDataArr_removeByIndex(slots, i);
            return true;
        }
    }

    return false;
}
