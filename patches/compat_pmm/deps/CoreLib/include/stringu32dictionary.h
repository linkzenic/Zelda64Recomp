#ifndef __STRINGU32DICTIONARY_H__
#define __STRINGU32DICTIONARY_H__

#include "ultra64.h"
#include "recompdata.h"
#include "dynamicu32array.h"

typedef struct {
    char *key;
    u32 value;
} StringU32DictionaryEntrySlot;

typedef struct {
    U32MemoryHashmapHandle hashmap;
    DynamicU32Array keyHashes;
} StringU32Dictionary;

// Creates a StringU32Dictionary and returns a handle
void StringU32Dictionary_init(StringU32Dictionary *dict);

// Destroys a StringU32Dictionary's members
void StringU32Dictionary_destroyMembers(StringU32Dictionary *dict);

// Remove all keys from a StringU32Dictionary
void StringU32Dictionary_clear(StringU32Dictionary *dict);

// Returns true if key was created, false otherwise.
bool StringU32Dictionary_set(StringU32Dictionary *dict, const char *key, u32 value);

// Returns true and writes to out if key exists, false and leaves out unmodified otherwise
bool StringU32Dictionary_get(StringU32Dictionary *dict, const char *key, u32 *out);

// Returns true if key existed and was removed, false otherwise
bool StringU32Dictionary_unset(StringU32Dictionary *dict, const char *key);

// Returns true if key exists in dictionary, false otherwise
bool StringU32Dictionary_contains(StringU32Dictionary *dict, const char *key);

#endif
