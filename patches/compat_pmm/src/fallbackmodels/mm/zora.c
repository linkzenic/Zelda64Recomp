#include "global.h"
#include "assets/objects/object_link_zora/object_link_zora.h"
#include "utils.h"
#include "globalobjects_api.h"
#include "playermodelmanager_api.h"
#include "defaultfacetex.h"
#include "apilocal.h"
#include "fallbackmodels.h"
#include "mm/vanillawrapperdls.h"

PlayerModelManagerHandle gZoraModelHandle;

Gfx *getZoraDL(Gfx *dl) {
    return GlobalObjects_getGlobalGfxPtr(OBJECT_LINK_ZORA, dl);
}

void registerZora(void) {
    void *zora = GlobalObjects_getGlobalObject(OBJECT_LINK_ZORA);

    FlexSkeletonHeader *skel = SEGMENTED_TO_GLOBAL_PTR(zora, &gLinkZoraSkel);

    gZoraModelHandle = PlayerModelManager_registerModel(PMM_API_VERSION, "__mm_object_link_zora__", PMM_MODEL_TYPE_ZORA);

    FallbackModelsCommon_doCommonAssignments(gZoraModelHandle, skel, zora, GlobalObjects_getGlobalObject(GAMEPLAY_KEEP));

#define SET_ENTRY_DL(id, dl) PlayerModelManager_setDisplayList(gZoraModelHandle, id, dl)

    // gLinkZoraSkel has fists on skeleton limbs, so we manually specify the open hands here
    PlayerModelManager_setDisplayList(gZoraModelHandle, LINK_DL_LHAND, getZoraDL(gLinkZoraLeftHandOpenDL));
    PlayerModelManager_setDisplayList(gZoraModelHandle, LINK_DL_OPT_LFIST, getZoraDL(gLinkZoraLeftHandClosedDL));
    PlayerModelManager_setDisplayList(gZoraModelHandle, LINK_DL_OPT_LHAND_BOTTLE, getZoraDL(gLinkZoraLeftHandHoldBottleDL));
    PlayerModelManager_setDisplayList(gZoraModelHandle, LINK_DL_OPT_LHAND_GUITAR, getZoraDL(object_link_zora_DL_00E088));
    PlayerModelManager_setDisplayList(gZoraModelHandle, LINK_DL_RHAND, getZoraDL(gLinkZoraRightHandOpenDL));
    PlayerModelManager_setDisplayList(gZoraModelHandle, LINK_DL_OPT_RFIST, getZoraDL(gLinkZoraRightHandClosedDL));;
    PlayerModelManager_setDisplayList(gZoraModelHandle, LINK_DL_ELEGY_OF_EMPTINESS_SHELL, gCallZoraElegyShellDL);

#undef SET_ENTRY_DL

    FallbackModelsCommon_addEquipmentAdultMM(gZoraModelHandle);
}
