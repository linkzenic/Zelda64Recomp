#ifndef __REPOINT_H__
#define __REPOINT_H__
#include "global.h"

typedef void *SegmentMap[0x10];

void GlobalObjects_rebaseDL(Gfx *dl, SegmentMap segments);

void GlobalObjects_globalizeSegmentedDL(void *obj, Gfx *segmentedPtr);

void GlobalObjects_globalizeLodLimbSkeleton(void *obj, FlexSkeletonHeader *skel);

void GlobalObjects_globalizeStandardLimbSkeleton(void *obj, FlexSkeletonHeader *skel);

#endif
