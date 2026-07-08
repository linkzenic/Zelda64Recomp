#include "global.h"
#include "modding.h"
#include "assets/objects/object_link_child/object_link_child.h"
#include "utils.h"
#include "globalobjects_api.h"
#include "modelmatrixids.h"
#include "playermodelmanager_api.h"
#include "customdls.h"
#include "apilocal.h"
#include "fallbackmodels.h"
#include "logger.h"
#include "mm/vanillawrapperdls.h"
#include "yazmtcorelib_api.h"

PlayerModelManagerHandle gHumanModelHandle;

LodLimb gColorableHumanSkelLimbs[21];

void *gColorableHumanSkelLimbPtrs[] = {
    &gColorableHumanSkelLimbs[0],  /* LINK_HUMAN_LIMB_ROOT */
    &gColorableHumanSkelLimbs[1],  /* LINK_HUMAN_LIMB_WAIST */
    &gColorableHumanSkelLimbs[2],  /* LINK_HUMAN_LIMB_LOWER_ROOT */
    &gColorableHumanSkelLimbs[3],  /* LINK_HUMAN_LIMB_RIGHT_THIGH */
    &gColorableHumanSkelLimbs[4],  /* LINK_HUMAN_LIMB_RIGHT_SHIN */
    &gColorableHumanSkelLimbs[5],  /* LINK_HUMAN_LIMB_RIGHT_FOOT */
    &gColorableHumanSkelLimbs[6],  /* LINK_HUMAN_LIMB_LEFT_THIGH */
    &gColorableHumanSkelLimbs[7],  /* LINK_HUMAN_LIMB_LEFT_SHIN */
    &gColorableHumanSkelLimbs[8],  /* LINK_HUMAN_LIMB_LEFT_FOOT */
    &gColorableHumanSkelLimbs[9],  /* LINK_HUMAN_LIMB_UPPER_ROOT */
    &gColorableHumanSkelLimbs[10], /* LINK_HUMAN_LIMB_HEAD */
    &gColorableHumanSkelLimbs[11], /* LINK_HUMAN_LIMB_HAT */
    &gColorableHumanSkelLimbs[12], /* LINK_HUMAN_LIMB_COLLAR */
    &gColorableHumanSkelLimbs[13], /* LINK_HUMAN_LIMB_LEFT_SHOULDER */
    &gColorableHumanSkelLimbs[14], /* LINK_HUMAN_LIMB_LEFT_FOREARM */
    &gColorableHumanSkelLimbs[15], /* LINK_HUMAN_LIMB_LEFT_HAND */
    &gColorableHumanSkelLimbs[16], /* LINK_HUMAN_LIMB_RIGHT_SHOULDER */
    &gColorableHumanSkelLimbs[17], /* LINK_HUMAN_LIMB_RIGHT_FOREARM */
    &gColorableHumanSkelLimbs[18], /* LINK_HUMAN_LIMB_RIGHT_HAND */
    &gColorableHumanSkelLimbs[19], /* LINK_HUMAN_LIMB_SHEATH */
    &gColorableHumanSkelLimbs[20], /* LINK_HUMAN_LIMB_TORSO */
};

FlexSkeletonHeader gColorableHumanSkel = {
    {gColorableHumanSkelLimbPtrs, ARRAY_COUNT(gColorableHumanSkelLimbPtrs)}, 18};

Gfx sPrimTunicCombinerDL[] = {
    gsDPSetCombineLERP(TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, TEXEL0, COMBINED, 0, SHADE, 0, 0, 0, 0, COMBINED),
    gsDPSetPrimColor(0, 0xFF, 30, 105, 27, 255),
};

Gfx sEnvTunicCombinerDL[] = {gsDPSetCombineLERP(TEXEL0, 0, ENVIRONMENT, 0, 0, 0, 0, TEXEL0, COMBINED, 0, SHADE, 0, 0, 0, 0, COMBINED)};

Gfx *getHumanDL(Gfx *dl) {
    return GlobalObjects_getGlobalGfxPtr(OBJECT_LINK_CHILD, dl);
}

static void makeColorable(Gfx *dl) {
    while (dl->words.w0 >> 24 != G_ENDDL) {
        if (dl->words.w0 >> 24 == G_DL) {
            if (((dl->words.w0 >> 16) & 0xFF) == G_DL_NOPUSH) {
                break;
            }
        }

        if (YAZMTCore_Utils_MemCmp(sPrimTunicCombinerDL, dl, sizeof(sPrimTunicCombinerDL)) == 0) {
            *dl = sEnvTunicCombinerDL[0];
        }

        dl++;
    }
}

static Gfx *getDLCopy(Gfx *dl) {
    Gfx *curr = dl;
    size_t dlLen = 1;

    while (curr->words.w0 >> 24 != G_ENDDL) {
        if (curr->words.w0 >> 24 == G_DL) {
            if (((curr->words.w0 >> 16) & 0xFF) == G_DL_NOPUSH) {
                break;
            }
        }

        curr++;
        dlLen++;
    }

    size_t allocSize = sizeof(Gfx) * dlLen;
    Gfx *copy = recomp_alloc(allocSize);

    Lib_MemCpy(copy, dl, allocSize);

    return copy;
}

void registerHuman(void) {
    void *human = GlobalObjects_getGlobalObject(OBJECT_LINK_CHILD);

    FlexSkeletonHeader *vanillaSkel = SEGMENTED_TO_GLOBAL_PTR(human, &gLinkHumanSkel);

    GlobalObjects_globalizeLodLimbSkeleton(human, vanillaSkel);

    for (u8 i = 0; i < vanillaSkel->sh.limbCount; ++i) {
        LodLimb *curr = vanillaSkel->sh.segment[i];
        LodLimb *currColorable = gColorableHumanSkel.sh.segment[i];

        *currColorable = *curr;

        if (curr->dLists[0]) {
            currColorable->dLists[0] = currColorable->dLists[1] = getDLCopy(curr->dLists[0]);
        }
    }

    gHumanModelHandle = PlayerModelManager_registerModel(PMM_API_VERSION, "__mm_object_link_child__", PMM_MODEL_TYPE_CHILD);

    FallbackModelsCommon_doCommonAssignments(gHumanModelHandle, &gColorableHumanSkel, human, GlobalObjects_getGlobalObject(GAMEPLAY_KEEP));

    for (u8 i = 0; i < gColorableHumanSkel.sh.limbCount; ++i) {
        LodLimb *curr = gColorableHumanSkel.sh.segment[i];

        if (curr->dLists[0]) {
            makeColorable(curr->dLists[0]);
        }
    }

    PlayerModelManager_setDisplayList(gHumanModelHandle, LINK_DL_OPT_LFIST, getHumanDL(gLinkHumanLeftHandClosedDL));
    PlayerModelManager_setDisplayList(gHumanModelHandle, LINK_DL_OPT_LHAND_BOTTLE, getHumanDL(gLinkHumanLeftHandHoldBottleDL));
    PlayerModelManager_setDisplayList(gHumanModelHandle, LINK_DL_OPT_LHAND_GUITAR, getHumanDL(gLinkHumanLeftHandOpenDL));
    PlayerModelManager_setDisplayList(gHumanModelHandle, LINK_DL_OPT_RFIST, getHumanDL(gLinkHumanRightHandClosedDL));
    PlayerModelManager_setDisplayList(gHumanModelHandle, LINK_DL_OPT_RHAND_OCARINA, gLinkHumanRightHandOcarinaDL);
    PlayerModelManager_setDisplayList(gHumanModelHandle, LINK_DL_OPT_FPS_RFOREARM, gEmptyDL);
    PlayerModelManager_setDisplayList(gHumanModelHandle, LINK_DL_OPT_FPS_RHAND, gLinkHumanFirstPersonArmDL);
    PlayerModelManager_setDisplayList(gHumanModelHandle, LINK_DL_ELEGY_OF_EMPTINESS_SHELL, gCallHumanElegyShellDL);

    FallbackModelsCommon_addEquipmentChildMM(gHumanModelHandle);
}
