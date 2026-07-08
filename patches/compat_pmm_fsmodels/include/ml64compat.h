#ifndef __ML64COMPAT_H__
#define __ML64COMPAT_H__

#include "global.h"
#include "modding.h"

void OotoFixHeaderSkelPtr(void *obj);
void OotoFixChildLeftShoulder(void *obj);
void setSegmentPtrRemap(u32 key, u32 value);
void applySegmentPtrRemaps(void *obj, Gfx *dl);

#endif
