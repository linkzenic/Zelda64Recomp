#ifndef __HELPERS_H__
#define __HELPERS_H__
#include "global.h"

#define TO_GLOBAL_PTR(globalObj, segmentedPtr) ((void *)(!(segmentedPtr) || ((uintptr_t)(segmentedPtr) >= K0BASE) ? (uintptr_t)(segmentedPtr) : ((uintptr_t)(globalObj) + SEGMENT_OFFSET(segmentedPtr))))

bool isSegmentedPtr(void *p);

#endif
