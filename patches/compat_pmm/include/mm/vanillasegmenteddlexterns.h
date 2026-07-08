#ifndef MM_VANILLASEGMENTEDDLEXTERNS
#define MM_VANILLASEGMENTEDDLEXTERNS

#define DECLARE_MM_DL_CFG(objectId, segmentedDL, wrapperDLName) extern Gfx segmentedDL[];
#include "mm/dlconfig.h"
#undef DECLARE_MM_DL_CFG

#endif 
