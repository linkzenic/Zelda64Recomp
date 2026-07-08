#include "global.h"
#include "mm/vanillawrapperdls.h"
#include "globalobjects_api.h"
#include "maskdls.h"
#include "mm/vanillasegmenteddlexterns.h"

#define DECLARE_MM_DL_CFG(objectId, segmentedDL, wrapperDLName) Gfx wrapperDLName[] = {gsSPEndDisplayList()};
#include "mm/dlconfig.h"
#undef DECLARE_MM_DL_CFG

typedef struct VanillaInitDLInfo {
    ObjectId objectId;
    Gfx *vanillaDL;
    Gfx *callDL;
} VanillaInitDLInfo;

#define DECLARE_MM_DL_CFG(objId, segmentedDL, wrapperDLName) {.callDL = wrapperDLName, .objectId = objId, .vanillaDL = segmentedDL},
static VanillaInitDLInfo sVanillaMMDLInits[] = {
#include "mm/dlconfig.h"
};
#undef DECLARE_MM_DL_CFG

static Gfx *getRepointedDL(ObjectId id, Gfx *segmentedDL) {
    return GlobalObjects_getGlobalGfxPtr(id, segmentedDL);
}

void initVanillaMMDLs(void) {
    for (int i = 0; i < ARRAY_COUNT(sVanillaMMDLInits); ++i) {
        VanillaInitDLInfo *curr = &sVanillaMMDLInits[i];

        if (curr->objectId && curr->vanillaDL) {
            Gfx *repointedDL = getRepointedDL(curr->objectId, curr->vanillaDL);

            if (repointedDL) {
                gSPBranchList(curr->callDL, getRepointedDL(curr->objectId, curr->vanillaDL));
            }
        }
    }
}
