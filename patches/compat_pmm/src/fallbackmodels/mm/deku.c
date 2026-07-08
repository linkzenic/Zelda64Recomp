#include "modding.h"
#include "global.h"
#include "assets/objects/object_link_nuts/object_link_nuts.h"
#include "utils.h"
#include "globalobjects_api.h"
#include "playermodelmanager_api.h"
#include "defaultfacetex.h"
#include "apilocal.h"
#include "fallbackmodels.h"
#include "mm/vanillawrapperdls.h"

PlayerModelManagerHandle gDekuModelHandle;

Gfx *getDekuDL(Gfx *dl) {
    return GlobalObjects_getGlobalGfxPtr(OBJECT_LINK_NUTS, dl);
}

void registerDeku(void) {
    void *deku = GlobalObjects_getGlobalObject(OBJECT_LINK_NUTS);

    FlexSkeletonHeader *skel = SEGMENTED_TO_GLOBAL_PTR(deku, &gLinkDekuSkel);

    gDekuModelHandle = PlayerModelManager_registerModel(PMM_API_VERSION, "__mm_object_link_nuts__", PMM_MODEL_TYPE_DEKU);

    FallbackModelsCommon_doCommonAssignments(gDekuModelHandle, skel, deku, GlobalObjects_getGlobalObject(GAMEPLAY_KEEP));

    PlayerModelManager_setDisplayList(gDekuModelHandle, LINK_DL_ELEGY_OF_EMPTINESS_SHELL, gCallDekuElegyShellDL);

    FallbackModelsCommon_addEquipmentChildMM(gDekuModelHandle);
}
