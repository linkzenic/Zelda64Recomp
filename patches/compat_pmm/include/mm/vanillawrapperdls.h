#ifndef MM_VANILLAWRAPPERDLS_H
#define MM_VANILLAWRAPPERDLS_H

#define DECLARE_MM_DL_CFG(objectId, segmentedDL, wrapperDLName) extern Gfx wrapperDLName[];
#include "mm/dlconfig.h"
#undef DECLARE_MM_DL_CFG

#endif
