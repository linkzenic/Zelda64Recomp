#include "modding.h"
#include "global.h"
#include "assets/objects/object_link_goron/object_link_goron.h"
#include "utils.h"
#include "globalobjects_api.h"
#include "playermodelmanager_api.h"
#include "defaultfacetex.h"
#include "apilocal.h"
#include "fallbackmodels.h"
#include "mm/vanillawrapperdls.h"

PlayerModelManagerHandle gGoronModelHandle;

Gfx *getGoronDL(Gfx *dl) {
    return GlobalObjects_getGlobalGfxPtr(OBJECT_LINK_GORON, dl);
}

void registerGoron(void) {
    void *goron = GlobalObjects_getGlobalObject(OBJECT_LINK_GORON);
    void *gk = GlobalObjects_getGlobalObject(GAMEPLAY_KEEP);

    FlexSkeletonHeader *skel = SEGMENTED_TO_GLOBAL_PTR(goron, &gLinkGoronSkel);

    FlexSkeletonHeader *shieldingSkel = SEGMENTED_TO_GLOBAL_PTR(goron, &gLinkGoronShieldingSkel);
    GlobalObjects_globalizeStandardLimbSkeleton(goron, &gLinkGoronShieldingSkel);

    GlobalObjectsSegmentMap segments = {0};
    segments[0x06] = goron;
    segments[0x04] = gk;

    for (int i = 0; i < shieldingSkel->sh.limbCount; ++i) {
        StandardLimb *currLimb = shieldingSkel->sh.segment[i];

        if (currLimb->dList) {
            GlobalObjects_rebaseDL(currLimb->dList, segments);
        }
    }

    gGoronModelHandle = PlayerModelManager_registerModel(PMM_API_VERSION, "__mm_object_link_goron__", PMM_MODEL_TYPE_GORON);

    FallbackModelsCommon_doCommonAssignments(gGoronModelHandle, skel, goron, gk);

    PlayerModelManager_setShieldingSkeleton(gGoronModelHandle, shieldingSkel);

    PlayerModelManager_setDisplayList(gGoronModelHandle, LINK_DL_OPT_LFIST, getGoronDL(gLinkGoronLeftHandClosedDL));
    PlayerModelManager_setDisplayList(gGoronModelHandle, LINK_DL_OPT_LHAND_BOTTLE, getGoronDL(gLinkGoronLeftHandHoldBottleDL));
    PlayerModelManager_setDisplayList(gGoronModelHandle, LINK_DL_OPT_RFIST, getGoronDL(gLinkGoronRightHandClosedDL));
    PlayerModelManager_setDisplayList(gGoronModelHandle, LINK_DL_ELEGY_OF_EMPTINESS_SHELL, gCallGoronElegyShellDL);

    FallbackModelsCommon_addEquipmentAdultMM(gGoronModelHandle);
}
