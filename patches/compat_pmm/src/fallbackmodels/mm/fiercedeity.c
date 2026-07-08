#include "modding.h"
#include "global.h"
#include "assets/objects/object_link_boy/object_link_boy.h"
#include "utils.h"
#include "globalobjects_api.h"
#include "playermodelmanager_api.h"
#include "defaultfacetex.h"
#include "apilocal.h"
#include "fallbackmodels.h"
#include "mm/vanillawrapperdls.h"

PlayerModelManagerHandle gFierceDeityModelHandle;

Gfx *getFierceDeityDL(Gfx *dl) {
    return GlobalObjects_getGlobalGfxPtr(OBJECT_LINK_BOY, dl);
}

void registerFierceDeity(void) {
    void *fd = GlobalObjects_getGlobalObject(OBJECT_LINK_BOY);

    FlexSkeletonHeader *skel = SEGMENTED_TO_GLOBAL_PTR(fd, &gLinkFierceDeitySkel);

    gFierceDeityModelHandle = PlayerModelManager_registerModel(PMM_API_VERSION, "__mm_object_link_boy__", PMM_MODEL_TYPE_FIERCE_DEITY);

    FallbackModelsCommon_doCommonAssignments(gFierceDeityModelHandle, skel, fd, GlobalObjects_getGlobalObject(GAMEPLAY_KEEP));

    PlayerModelManager_setDisplayList(gFierceDeityModelHandle, LINK_DL_ELEGY_OF_EMPTINESS_SHELL, gCallHumanElegyShellDL);

    FallbackModelsCommon_addEquipmentAdultMM(gFierceDeityModelHandle);
}
