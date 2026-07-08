#ifndef __OBJECT_MANAGER_H__
#define __OBJECT_MANAGER_H__

#include "global.h"

// A workaround for object dependency issues
// This manager keeps objects loaded and repoints as needed

// Returns a pointer to the object with the given id
// if it was not already loaded, it is loaded here
void *GlobalObjects_getGlobalObject(ObjectId id);

Gfx *GlobalObjects_getGlobalGfxPtr(ObjectId id, Gfx *segmentedPtr);

bool GlobalObjects_getObjectIdFromVrom(uintptr_t vromStart, ObjectId *out);

void *GlobalObjects_getGlobalObjectFromVrom(uintptr_t vromStart);

// Func name is used for error printing
bool isObjectManagerReady(const char *funcName);

#endif
