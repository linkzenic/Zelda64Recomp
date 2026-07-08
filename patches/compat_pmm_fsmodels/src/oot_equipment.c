#include "global.h"
#include "recompconfig.h"
#include "recomputils.h"
#include "oot_objects.h"
#include "globalobjects_api.h"
#include "playermodelmanager_api.h"
#include "oot_equipment.h"

Gfx *gOOTHookshotDL;
Gfx *gOOTHookshotChildDL;
Gfx *gOOTHookshotFirstPersonDL;
Gfx *gOOTHookshotFirstPersonChildDL;
Gfx *gOOTHookshotChainDL;
Gfx *gOOTHookshotHookDL;
Gfx *gOOTHookshotHookChildDL;
Gfx *gOOTBowDL;
Gfx *gOOTBowFirstPersonDL;
Gfx *gOOTBowChildDL;
Gfx *gOOTBowFirstPersonChildDL;
Gfx *gOOTBowStringChildDL;
Gfx *gOOTBowStringDL;
Gfx *gOOTMirrorShieldDL;
Gfx *gOOTMirrorShieldChildDL;
Gfx *gOOTMirrorShieldRayDL;
Gfx *gOOTMirrorShieldRayChildDL;
Gfx *gOOTMirrorShieldRayBeamDL;
Gfx *gOOTMirrorShieldRayBeamAdjustedDL;
Gfx *gOOTMirrorShieldRayBeamChildDL;
Gfx *gOOTHylianShieldDL;
Gfx *gOOTHylianShieldChildDL;
Gfx *gOOTDekuShieldDL;
Gfx *gOOTDekuShieldAdultDL;
Gfx *gOOTMasterSwordHiltDL;
Gfx *gOOTMasterSwordHiltChildDL;
Gfx *gOOTMasterSwordBladeDL;
Gfx *gOOTMasterSwordBladeChildDL;
Gfx *gOOTMasterSwordSheathDL;
Gfx *gOOTMasterSwordSheathChildDL;
Gfx *gOOTKokiriSwordHiltDL;
Gfx *gOOTKokiriSwordHiltAdultDL;
Gfx *gOOTKokiriSwordBladeDL;
Gfx *gOOTKokiriSwordBladeAdultDL;
Gfx *gOOTKokiriSwordSheathDL;
Gfx *gOOTKokiriSwordSheathAdultDL;
Gfx *gOOTBiggoronSwordHiltDL;
Gfx *gOOTBiggoronSwordHiltChildDL;
Gfx *gOOTBiggoronSwordBladeDL;
Gfx *gOOTBiggoronSwordBladeChildDL;
Gfx *gOOTBottleAdultDL;
Gfx *gOOTBottleChildDL;
Gfx *gOOTDekuStickDL;
Gfx *gOOTFairyOcarinaDL;
Gfx *gOOTFairyOcarinaAdultDL;

Mtx gOOTAdultShieldMtx;
Mtx gOOTHookshotHookAndChainMtx;
Mtx gOOTMasterSwordHiltMtx;

Mtx gOOTKokiriSwordHiltMtx;
Mtx gOOTHookshotHookAndChainChildMtx;
Mtx gOOTDekuShieldMtx;

Mtx gMasterSwordBackChildMtx;
Mtx gKokiriSwordBackAdultMtx;

static bool sIsAdultEquipmentInitialized;
static bool sIsChildEquipmentInitialized;
static bool sIsCrossEquipmentInitialized;

#define OOT_LINK_ADULT_FPS_RIGHT_HAND_AND_BOW 0x2A248
#define OOT_LINK_ADULT_RIGHT_HAND_AND_OCARINA 0x24698
#define OOT_LINK_ADULT_RIGHT_HAND_AND_MIRROR_SHIELD 0x241C0
#define OOT_LINK_ADULT_RIGHT_HAND_AND_HYLIAN_SHIELD 0x22970
#define OOT_LINK_ADULT_LEFT_HAND_AND_MASTER_SWORD 0x21F78
#define OOT_LINK_ADULT_SHEATH 0x249D8
#define OOT_LINK_ADULT_RIGHT_HAND_HOLDING_HOOKSHOT 0x24D70
#define OOT_LINK_ADULT_HOOKSHOT_HOOK 0x2B288
#define OOT_LINK_ADULT_HOOKSHOT_CHAIN 0x2AFF0
#define OOT_LINK_ADULT_FPS_RIGHT_HAND_HOLDING_HOOKSHOT 0x2A738
#define OOT_LINK_ADULT_RIGHT_HAND_HOLDING_BOW 0x22DA8
#define OOT_LINK_ADULT_FPS_RIGHT_HAND_AND_BOW 0x2A248
#define OOT_LINK_ADULT_BOW_STRING 0x2B108
#define OOT_LINK_ADULT_LEFT_HAND_HOLDING_BGS 0x238C8
#define OOT_LINK_ADULT_BOTTLE 0x2AD58

#define OOT_LINK_CHILD_LFIST_HOLDING_KOKIRI_SWORD 0x13F38
#define OOT_LINK_CHILD_LFIST_HOLDING_KOKIRI_TLUT 0x06A118
#define OOT_LINK_CHILD_SHEATH 0x15408
#define OOT_LINK_CHILD_BOTTLE_GLASS 0x18478
#define OOT_LINK_CHILD_DEKU_STICK 0x6CC0
#define OOT_LINK_CHILD_RFIST_HOLDING_DEKU_SHIELD 0x14440
#define OOT_LINK_CHILD_RFIST_HOLDING_FAIRY_OCARINA 0x15BA8

static Gfx sMasterSwordHiltDL[] = {
    gsDPPipeSync(),
    gsDPSetCombineLERP(TEXEL0, 0, SHADE, 0, 0, 0, 0, 1, COMBINED, 0, PRIMITIVE, 0, 0, 0, 0, COMBINED),
    gsDPSetRenderMode(G_RM_FOG_SHADE_A, G_RM_AA_ZB_OPA_SURF2),
    gsSPSetGeometryMode(G_FOG | G_LIGHTING),
    gsSPDisplayList(0x0C000000),
    gsDPPipeSync(),
    gsSPNoOp(), // branch command goes here
};

static Gfx sBiggoronSwordBladeDL[] = {
    gsDPPipeSync(),
    gsDPSetCombineLERP(TEXEL0, 0, SHADE, 0, 0, 0, 0, 1, COMBINED, 0, PRIMITIVE, 0, 0, 0, 0, COMBINED),
    gsDPSetRenderMode(G_RM_FOG_SHADE_A, G_RM_AA_ZB_OPA_SURF2),
    gsSPSetGeometryMode(G_FOG | G_LIGHTING | G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR),
    gsDPPipeSync(),
    gsSPNoOp(), // branch command goes here
};

static Gfx sKokiriSwordHiltDL[] = {
    gsDPPipeSync(),
    gsDPSetTextureLUT(G_TT_RGBA16),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPSetCombineLERP(TEXEL0, 0, SHADE, 0, 0, 0, 0, 1, COMBINED, 0, PRIMITIVE, 0, 0, 0, 0, COMBINED),
    gsDPSetRenderMode(G_RM_FOG_SHADE_A, G_RM_AA_ZB_OPA_SURF2),
    gsSPClearGeometryMode(G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR),
    gsSPSetGeometryMode(G_CULL_BACK | G_FOG | G_LIGHTING),
    gsDPSetPrimColor(0, 0, 255, 255, 255, 255),
    gsDPNoOp(), // branch goes here
};

static Gfx sKokiriSwordBladeDL[] = {
    gsDPPipeSync(),
    gsDPSetCombineLERP(TEXEL0, 0, SHADE, 0, 0, 0, 0, 1, COMBINED, 0, PRIMITIVE, 0, 0, 0, 0, COMBINED),
    gsDPSetRenderMode(G_RM_FOG_SHADE_A, G_RM_AA_ZB_OPA_SURF2),
    gsSPSetGeometryMode(G_CULL_BACK | G_FOG | G_LIGHTING | G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR),
    gsDPPipeSync(),
    gsDPNoOp(), // branch goes here
};

#define DECLARE_STATIC_MATRIX_WRAPPED_DL(dlName, mtxName, dl)           \
    static Mtx mtxName;                                                 \
    static Gfx dlName[] = {                                             \
        gsSPMatrix(&mtxName, G_MTX_PUSH | G_MTX_MUL | G_MTX_MODELVIEW), \
        gsSPDisplayList(dl),                                            \
        gsSPPopMatrix(G_MTX_MODELVIEW),                                 \
        gsSPEndDisplayList(),                                           \
    }

static Gfx sCallMasterSwordHiltDL[] = {
    gsSPEndDisplayList(),
};

DECLARE_STATIC_MATRIX_WRAPPED_DL(sMasterSwordHiltChildDL, sMasterSwordHiltChildResizerMtx, sCallMasterSwordHiltDL);

static Gfx sCallMasterSwordBladeDL[] = {
    gsSPEndDisplayList(),
};

DECLARE_STATIC_MATRIX_WRAPPED_DL(sMasterSwordBladeChildDL, sMasterSwordBladeChildResizerMtx, sCallMasterSwordBladeDL);

static Gfx sCallMasterSwordSheathDL[] = {
    gsSPEndDisplayList(),
};

DECLARE_STATIC_MATRIX_WRAPPED_DL(sMasterSwordSheathChildDL, sMasterSwordSheathChildResizerMtx, sCallMasterSwordSheathDL);

static Gfx sCallKokiriSwordHiltDL[] = {
    gsSPEndDisplayList(),
};

DECLARE_STATIC_MATRIX_WRAPPED_DL(sKokiriSwordHiltAdultDL, sKokiriSwordHiltAdultResizerMtx, sCallKokiriSwordHiltDL);

static Gfx sCallKokiriSwordBladeDL[] = {
    gsSPEndDisplayList(),
};

DECLARE_STATIC_MATRIX_WRAPPED_DL(sKokiriSwordBladeAdultDL, sKokiriSwordBladeAdultResizerMtx, sCallKokiriSwordBladeDL);

static Gfx sCallKokiriSwordSheathDL[] = {
    gsSPEndDisplayList(),
};

DECLARE_STATIC_MATRIX_WRAPPED_DL(sKokiriSwordSheathAdultDL, sKokiriSwordSheathAdultResizerMtx, sCallKokiriSwordSheathDL);

static Gfx sCallBiggoronSwordHiltDL[] = {
    gsSPEndDisplayList(),
};

DECLARE_STATIC_MATRIX_WRAPPED_DL(sBiggoronSwordHiltChildDL, sBiggoronSwordHiltChildResizerMtx, sCallBiggoronSwordHiltDL);

static Gfx sCallBiggoronSwordBladeDL[] = {
    gsSPEndDisplayList(),
};

DECLARE_STATIC_MATRIX_WRAPPED_DL(sBiggoronSwordBladeChildDL, sBiggoronSwordBladeChildResizerMtx, sCallBiggoronSwordBladeDL);

static Gfx sCallHylianShieldDL[] = {
    gsSPEndDisplayList(),
};

DECLARE_STATIC_MATRIX_WRAPPED_DL(sHylianShieldChildDL, sHylianShieldResizerMtx, sCallHylianShieldDL);

static Gfx sCallMirrorShieldDL[] = {
    gsSPEndDisplayList(),
};

DECLARE_STATIC_MATRIX_WRAPPED_DL(sMirrorShieldChildDL, sMirrorShieldResizerMtx, sCallMirrorShieldDL);

static Gfx sCallMirrorShieldRayDL[] = {
    gsSPEndDisplayList(),
};

DECLARE_STATIC_MATRIX_WRAPPED_DL(sMirrorShieldRayChildDL, sMirrorShieldRayResizerMtx, sCallMirrorShieldRayDL);

static Gfx sCallMirrorShieldRayBeamDL[] = {
    gsSPEndDisplayList(),
    gsSPEndDisplayList(),
    gsSPEndDisplayList(),
    gsSPEndDisplayList(),
};

DECLARE_STATIC_MATRIX_WRAPPED_DL(sMirrorShieldRayBeamChildDL, sMirrorShieldRayBeamResizerMtx, sCallMirrorShieldRayBeamDL);

static Gfx sCallHookshotDL[] = {
    gsSPEndDisplayList(),
};

DECLARE_STATIC_MATRIX_WRAPPED_DL(sHookshotChildDL, sHookshotResizerMtx, sCallHookshotDL);

static Gfx sCallHookshotHookDL[] = {
    gsSPEndDisplayList(),
};

DECLARE_STATIC_MATRIX_WRAPPED_DL(sHookshotHookChildDL, sHookshotHookResizerMtx, sCallHookshotHookDL);

static Gfx sCallHookshotFirstPersonDL[] = {
    gsSPEndDisplayList(),
};

DECLARE_STATIC_MATRIX_WRAPPED_DL(sHookshotFirstPersonChildDL, sHookshotFirstPersonResizerMtx, sCallHookshotFirstPersonDL);

static Gfx sCallBowDL[] = {
    gsSPEndDisplayList(),
};

DECLARE_STATIC_MATRIX_WRAPPED_DL(sBowChildDL, sBowChildResizerMtx, sCallBowDL);

DECLARE_STATIC_MATRIX_WRAPPED_DL(sBowChildTransRotDL, sBowChildTransRotMtx, sBowChildDL);

static Gfx sCallBowFirstPersonDL[] = {
    gsSPEndDisplayList(),
};

DECLARE_STATIC_MATRIX_WRAPPED_DL(sBowFirstPersonChildDL, sBowFirstPersonChildResizerMtx, sCallBowFirstPersonDL);

DECLARE_STATIC_MATRIX_WRAPPED_DL(sBowFirstPersonChildTransRotDL, sBowChildFirstPersonTransRotMtx, sBowFirstPersonChildDL);

static Gfx sCallBowStringDL[] = {
    gsSPEndDisplayList(),
};

DECLARE_STATIC_MATRIX_WRAPPED_DL(sBowStringChildDL, sBowStringChildResizerMtx, sCallBowStringDL);

static Gfx sCallDekuShieldDL[] = {
    gsSPEndDisplayList(),
};

DECLARE_STATIC_MATRIX_WRAPPED_DL(sDekuShieldAdult, sDekuShieldAdultResizerMtx, sCallDekuShieldDL);

static Gfx sCallFairyOcarinaDL[] = {
    gsSPEndDisplayList(),
};

DECLARE_STATIC_MATRIX_WRAPPED_DL(sFairyOcarinaAdultDL, sFairyOcarinaResizerMtx, sCallFairyOcarinaDL);

static void setupResizedSwordMatrixes(Mtx *hiltResizeMtx, Mtx *bladeResizeMtx, Mtx *sheathResizeMtx, Mtx *hiltBackMtx, f32 scale, Vec3f *translation, Vec3f *hiltBackBaseTranslation) {
    guPosition(hiltResizeMtx, 0.f, 0.f, 0.f, scale, translation->x, translation->y, translation->z);
    guPosition(bladeResizeMtx, 0.f, 0.f, 0.f, scale, translation->x, translation->y, translation->z);
    guPosition(sheathResizeMtx, 0.f, 0.f, 0.f, scale, 0.f, 0.f, 0.f);
    guPosition(hiltBackMtx, 0.f, 0.f, 0.f, 1.f,
               hiltBackBaseTranslation->x * scale - translation->x,
               hiltBackBaseTranslation->y * scale - translation->y,
               hiltBackBaseTranslation->z * scale - translation->z);
}

static void setDLForModelTypes(PlayerModelManagerHandle h, PlayerModelManagerDisplayListId id, Gfx *dl, PlayerModelManagerModelType types[], size_t n) {
    for (size_t i = 0; i < n; ++i) {
        PlayerModelManager_setDisplayListForModelType(h, types[i], id, dl);
    }
}

static void setMtxForModelTypes(PlayerModelManagerHandle h, PlayerModelManagerMatrixId id, Mtx *mtx, PlayerModelManagerModelType types[], size_t n) {
    for (size_t i = 0; i < n; ++i) {
        PlayerModelManager_setMatrixForModelType(h, types[i], id, mtx);
    }
}

static PlayerModelManagerModelType sModelTypesAdult[] = {PMM_MODEL_TYPE_ADULT, PMM_MODEL_TYPE_ZORA, PMM_MODEL_TYPE_GORON, PMM_MODEL_TYPE_FIERCE_DEITY};

static void setAdultModelTypeDL(PlayerModelManagerHandle h, PlayerModelManagerDisplayListId id, Gfx *dl) {
    setDLForModelTypes(h, id, dl, sModelTypesAdult, ARRAY_COUNT(sModelTypesAdult));
}

static void setAdultModelTypeMtx(PlayerModelManagerHandle h, PlayerModelManagerMatrixId id, Mtx *mtx) {
    setMtxForModelTypes(h, id, mtx, sModelTypesAdult, ARRAY_COUNT(sModelTypesAdult));
}

static PlayerModelManagerModelType sModelTypesChild[] = {PMM_MODEL_TYPE_CHILD, PMM_MODEL_TYPE_DEKU};

static void setChildModelTypeDL(PlayerModelManagerHandle h, PlayerModelManagerDisplayListId id, Gfx *dl) {
    setDLForModelTypes(h, id, dl, sModelTypesChild, ARRAY_COUNT(sModelTypesChild));
}

static void setChildModelTypeMtx(PlayerModelManagerHandle h, PlayerModelManagerMatrixId id, Mtx *mtx) {
    setMtxForModelTypes(h, id, mtx, sModelTypesChild, ARRAY_COUNT(sModelTypesChild));
}

#define ADULT_SWORD_TO_CHILD_SCALE 0.9f
#define ADULT_SWORD_TO_CHILD_X 30.f
#define ADULT_SWORD_TO_CHILD_Y -75.f
#define ADULT_SWORD_TO_CHILD_Z 75.f

#define CHILD_SWORD_TO_ADULT_SCALE 1.3f

void initCrossAgeEquipment() {
    if (sIsCrossEquipmentInitialized || !sIsAdultEquipmentInitialized || !sIsChildEquipmentInitialized) {
        return;
    }

    sIsCrossEquipmentInitialized = true;

    // Child Hylian Shield
    gSPBranchList(sCallHylianShieldDL, gOOTHylianShieldDL);
    guPosition(&sHylianShieldResizerMtx, 0.f, 0.f, 0.f, 0.8f, 0.f, 0.f, 0.f);
    gOOTHylianShieldChildDL = sHylianShieldChildDL;

    // Child Mirror Shield
    gSPBranchList(sCallMirrorShieldDL, gOOTMirrorShieldDL);
    guPosition(&sMirrorShieldResizerMtx, 0.f, 0.f, 0.f, 0.77f, 0.f, 0.f, 0.f);
    gOOTMirrorShieldChildDL = sMirrorShieldChildDL;

    if (gMirRayOOT) {
        gSPBranchList(sCallMirrorShieldRayDL, gOOTMirrorShieldRayDL);
        guPosition(&sMirrorShieldRayResizerMtx, 0.f, 0.f, 0.f, 0.77f, 0.f, 0.f, 0.f);
        gOOTMirrorShieldRayChildDL = sMirrorShieldRayChildDL;

        // Beam is always scaled by 5 in OoT
        static Mtx beamAdjuster;
        guMtxIdent(&beamAdjuster);
        guScale(&beamAdjuster, 1.f, 1.f, 5.f);
        gSPMatrix(&sCallMirrorShieldRayBeamDL[0], &beamAdjuster, G_MTX_PUSH | G_MTX_MUL | G_MTX_MODELVIEW);
        gSPDisplayList(&sCallMirrorShieldRayBeamDL[1], gOOTMirrorShieldRayBeamDL);
        gSPPopMatrix(&sCallMirrorShieldRayBeamDL[2], G_MTX_MODELVIEW);
        gOOTMirrorShieldRayBeamAdjustedDL = sCallMirrorShieldRayBeamDL;

        guPosition(&sMirrorShieldRayBeamResizerMtx, 0.f, 0.f, 0.f, 0.77f, 0.f, 0.f, 0.f);
        gOOTMirrorShieldRayBeamChildDL = sMirrorShieldRayBeamChildDL;
    }

    // Child Master Sword
    gSPBranchList(sCallMasterSwordHiltDL, gOOTMasterSwordHiltDL);
    gOOTMasterSwordHiltChildDL = sMasterSwordHiltChildDL;

    gSPBranchList(sCallMasterSwordBladeDL, gOOTMasterSwordBladeDL);
    gOOTMasterSwordBladeChildDL = sMasterSwordBladeChildDL;

    gSPBranchList(sCallMasterSwordSheathDL, gOOTMasterSwordSheathDL);
    gOOTMasterSwordSheathChildDL = sMasterSwordSheathChildDL;

    static Vec3f masterSwordChildTranslation = {ADULT_SWORD_TO_CHILD_X, ADULT_SWORD_TO_CHILD_Y, ADULT_SWORD_TO_CHILD_Z};
    static Vec3f masterSwordChildBackBaseTranslation = {-715.f, -310.f, 78.f};
    setupResizedSwordMatrixes(&sMasterSwordHiltChildResizerMtx, &sMasterSwordBladeChildResizerMtx, &sMasterSwordSheathChildResizerMtx,
                              &gMasterSwordBackChildMtx, ADULT_SWORD_TO_CHILD_SCALE, &masterSwordChildTranslation, &masterSwordChildBackBaseTranslation);

    // Adult Kokiri Sword
    gSPBranchList(sCallKokiriSwordHiltDL, gOOTKokiriSwordHiltDL);
    gOOTKokiriSwordHiltAdultDL = sKokiriSwordHiltAdultDL;

    gSPBranchList(sCallKokiriSwordBladeDL, gOOTKokiriSwordBladeDL);
    gOOTKokiriSwordBladeAdultDL = sKokiriSwordBladeAdultDL;

    gSPBranchList(sCallKokiriSwordSheathDL, gOOTKokiriSwordSheathDL);
    gOOTKokiriSwordSheathAdultDL = sKokiriSwordSheathAdultDL;

    static Vec3f kokiriSwordAdultTranslation = {100.f, 25.f, -90.f};
    static Vec3f kokiriSwordAdultBackBaseTranslation = {-440.f, -211.f, 0.f};
    setupResizedSwordMatrixes(&sKokiriSwordHiltAdultResizerMtx, &sKokiriSwordBladeAdultResizerMtx, &sKokiriSwordSheathAdultResizerMtx, 
    &gKokiriSwordBackAdultMtx, CHILD_SWORD_TO_ADULT_SCALE, &kokiriSwordAdultTranslation, &kokiriSwordAdultBackBaseTranslation);

    // Child Biggoron Sword
    gSPBranchList(sCallBiggoronSwordHiltDL, gOOTBiggoronSwordHiltDL);
    guPosition(&sBiggoronSwordHiltChildResizerMtx, 0.f, 0.f, 0.f, ADULT_SWORD_TO_CHILD_SCALE, ADULT_SWORD_TO_CHILD_X, ADULT_SWORD_TO_CHILD_Y, ADULT_SWORD_TO_CHILD_Z);
    gOOTBiggoronSwordHiltChildDL = sBiggoronSwordHiltChildDL;

    gSPBranchList(sCallBiggoronSwordBladeDL, gOOTBiggoronSwordBladeDL);
    guPosition(&sBiggoronSwordBladeChildResizerMtx, 0.f, 0.f, 0.f, ADULT_SWORD_TO_CHILD_SCALE, ADULT_SWORD_TO_CHILD_X, ADULT_SWORD_TO_CHILD_Y, ADULT_SWORD_TO_CHILD_Z);
    gOOTBiggoronSwordBladeChildDL = sBiggoronSwordBladeChildDL;

    // Child Hookshot
    gSPBranchList(sCallHookshotDL, gOOTHookshotDL);
    guPosition(&sHookshotResizerMtx, 0.f, 0.f, 0.f, 0.6f, 0.f, 0.f, 0.f);
    gOOTHookshotChildDL = sHookshotChildDL;

    gSPBranchList(sCallHookshotFirstPersonDL, gOOTHookshotFirstPersonDL);
    guPosition(&sHookshotFirstPersonResizerMtx, 0.f, 0.f, 0.f, 0.6f, 0.f, 0.f, 0.f);
    gOOTHookshotFirstPersonChildDL = sHookshotFirstPersonChildDL;

    gSPBranchList(sCallHookshotHookDL, gOOTHookshotHookDL);
    guPosition(&sHookshotHookResizerMtx, 0.f, 0.f, 0.f, 0.6f, 0.f, 0.f, 0.f);
    gOOTHookshotHookChildDL = sHookshotHookChildDL;

    // Child Bow
    const f32 BOW_X_SCALE = 0.7f;
    const f32 BOW_X_TRANS = -40.f;

    const f32 BOW_Y_TRANS = -65.f;

    gSPBranchList(sCallBowDL, gOOTBowDL);
    guScale(&sBowChildResizerMtx, BOW_X_SCALE, 1.f, 1.f);
    guPosition(&sBowChildTransRotMtx, 0.f, 0.f, 1.f, 1.f, BOW_X_TRANS, BOW_Y_TRANS, 0.f);
    gOOTBowChildDL = sBowChildTransRotDL;

    gSPBranchList(sCallBowFirstPersonDL, gOOTBowFirstPersonDL);
    guScale(&sBowFirstPersonChildResizerMtx, BOW_X_SCALE, 1.f, 1.f);
    guPosition(&sBowChildFirstPersonTransRotMtx, 0.f, 0.f, 1.f, 1.f, BOW_X_TRANS, BOW_Y_TRANS, 0.f);
    gOOTBowFirstPersonChildDL = sBowFirstPersonChildTransRotDL;

    extern Gfx object_link_child_DL_017818[];
    //gSPBranchList(sCallBowStringDL, gOOTBowStringDL);
    //guScale(&sBowStringChildResizerMtx, 0.7f, 0.7f, 1.f);
    gOOTBowStringChildDL = GlobalObjects_getGlobalGfxPtr(OBJECT_LINK_CHILD, object_link_child_DL_017818);

    // Adult Deku Shield
    gSPBranchList(sCallDekuShieldDL, gOOTDekuShieldDL);
    guPosition(&sDekuShieldAdultResizerMtx, 0.f, 0.f, 0.f, 1.35f, 0.f, 0.f, 0.f);
    gOOTDekuShieldAdultDL = sDekuShieldAdult;

    guPosition(&gOOTHookshotHookAndChainChildMtx, 0.f, 0.f, 0.f, 1.f, 30.f, 150.f, 0.f);

    // Adult Fairy Ocarina
    gSPBranchList(sCallFairyOcarinaDL, gOOTFairyOcarinaDL);
    guPosition(&sFairyOcarinaResizerMtx, 7.f, -8.f, -4.f, 1.f, 72.f, 238.f, -3.f);
    gOOTFairyOcarinaAdultDL = sFairyOcarinaAdultDL;

    addEquipmentToModelManager();
}

void initAdultEquipment() {
    if (sIsAdultEquipmentInitialized || !gLinkAdultOOT) {
        return;
    }

    sIsAdultEquipmentInitialized = true;

    GlobalObjectsSegmentMap aLinkSegs = {0};
    aLinkSegs[0x06] = gLinkAdultOOT;
    aLinkSegs[0x04] = gGameplayKeepOOT;

    // Unglue fist from Mirror Shield
    const uintptr_t MIRROR_SHIELD_HAND_START_DRAW_OFFSET = 0x24378;
    const uintptr_t MIRROR_SHIELD_BODY_START_DRAW_OFFSET = 0x245A8;
    gSPBranchList(gLinkAdultOOT + MIRROR_SHIELD_HAND_START_DRAW_OFFSET, SEGMENT_ADDR(6, MIRROR_SHIELD_BODY_START_DRAW_OFFSET));

    // Mirror shield beam DLs
    if (gMirRayOOT) {
        GlobalObjectsSegmentMap mirRaySegs = {0};
        mirRaySegs[0x06] = gMirRayOOT;
        mirRaySegs[0x04] = gGameplayKeepOOT;
        const uintptr_t MIRROR_SHIELD_IMAGE_START_DRAW_OFFSET = 0xB0;
        const uintptr_t MIRROR_SHIELD_BEAM_START_DRAW_OFFSET = 0xC50;

        gOOTMirrorShieldRayDL = (Gfx *)(gMirRayOOT + MIRROR_SHIELD_IMAGE_START_DRAW_OFFSET);
        gOOTMirrorShieldRayBeamDL = (Gfx *)(gMirRayOOT + MIRROR_SHIELD_BEAM_START_DRAW_OFFSET);

        GlobalObjects_rebaseDL(gOOTMirrorShieldRayDL, mirRaySegs);
        GlobalObjects_rebaseDL(gOOTMirrorShieldRayBeamDL, mirRaySegs);
    }

    // Unglue fist from Hylian Shield
    const uintptr_t HYLIAN_SHIELD_HAND_START_DRAW_OFFSET = 0x22AC8;
    const uintptr_t HYLIAN_SHIELD_BACK_START_DRAW_OFFSET = 0x22C28;
    const uintptr_t HYLIAN_SHIELD_HAND2_START_DRAW_OFFSET = 0x22CB8;
    gSPBranchList(gLinkAdultOOT + HYLIAN_SHIELD_HAND_START_DRAW_OFFSET, SEGMENT_ADDR(6, HYLIAN_SHIELD_BACK_START_DRAW_OFFSET));
    gSPEndDisplayList(gLinkAdultOOT + HYLIAN_SHIELD_HAND2_START_DRAW_OFFSET);

    // Master Sword Blade
    GlobalObjects_rebaseDL((Gfx *)(gLinkAdultOOT + OOT_LINK_ADULT_LEFT_HAND_AND_MASTER_SWORD), aLinkSegs);
    const uintptr_t MASTER_SWORD_BLADE_END = 0x22060;
    gSPEndDisplayList(gLinkAdultOOT + MASTER_SWORD_BLADE_END);

    // Master Sword Hilt
    const uintptr_t MASTER_SWORD_HILT_START = MASTER_SWORD_BLADE_END + sizeof(Gfx);
    const uintptr_t MASTER_SWORD_HILT_END = 0x22248;
    gSPBranchList(&sMasterSwordHiltDL[ARRAY_COUNT(sMasterSwordHiltDL) - 1], gLinkAdultOOT + MASTER_SWORD_HILT_START);
    gSPEndDisplayList(gLinkAdultOOT + MASTER_SWORD_HILT_END);

    // Hookshot (3rd Person)
    const uintptr_t HOOKSHOT_HAND_START = 0x24FC0;
    gSPEndDisplayList(gLinkAdultOOT + HOOKSHOT_HAND_START);

    // Hookshot (1st Person)
    const uintptr_t HOOKSHOT_FPS_HAND_START = 0x2AA28;
    gSPEndDisplayList(gLinkAdultOOT + HOOKSHOT_FPS_HAND_START);

    // Bow
    const uintptr_t BOW_HAND_START = 0x22F00;
    gSPEndDisplayList(gLinkAdultOOT + BOW_HAND_START);

    // Bow (First Person)
    const uintptr_t BOW_FPS_HAND_START = 0x2A3F8;
    gSPEndDisplayList(gLinkAdultOOT + BOW_FPS_HAND_START);

    // Biggoron Sword Hilt
    GlobalObjects_rebaseDL((Gfx *)(gLinkAdultOOT + OOT_LINK_ADULT_LEFT_HAND_HOLDING_BGS), aLinkSegs);
    const uintptr_t BIGGORON_SWORD_HILT_END = 0x23A28;
    gSPEndDisplayList(gLinkAdultOOT + BIGGORON_SWORD_HILT_END);

    // Biggoron Sword Blade
    const uintptr_t BIGGORON_SWORD_BLADE_START = BIGGORON_SWORD_HILT_END + sizeof(Gfx);
    const uintptr_t BIGGORON_SWORD_HAND_START = 0x23AE0;
    gSPBranchList(&sBiggoronSwordBladeDL[ARRAY_COUNT(sBiggoronSwordBladeDL) - 1], gLinkAdultOOT + BIGGORON_SWORD_BLADE_START);
    gSPEndDisplayList(gLinkAdultOOT + BIGGORON_SWORD_HAND_START);

    guPosition(&gOOTHookshotHookAndChainMtx, 0.f, 0.f, 0.f, 1.f, 50.f, 840.f, 0.f);

    guPosition(&gOOTAdultShieldMtx, 0.f, 0.f, 180.f, 1.f, 935.f, 94.f, 29.f);

    guPosition(&gOOTMasterSwordHiltMtx, 0.f, 0.f, 0.f, 1.f, -715.f, -310.f, 78.f);

    // clang-format off
    #define REPOINT_SET_ADULT(ptrToSet, dl) { ptrToSet = (Gfx *)(dl); GlobalObjects_rebaseDL(ptrToSet, aLinkSegs); } (void)0
    // clang-format on

    REPOINT_SET_ADULT(gOOTHylianShieldDL, gLinkAdultOOT + OOT_LINK_ADULT_RIGHT_HAND_AND_HYLIAN_SHIELD);
    REPOINT_SET_ADULT(gOOTMirrorShieldDL, gLinkAdultOOT + OOT_LINK_ADULT_RIGHT_HAND_AND_MIRROR_SHIELD);
    REPOINT_SET_ADULT(gOOTMasterSwordBladeDL, gLinkAdultOOT + OOT_LINK_ADULT_LEFT_HAND_AND_MASTER_SWORD);
    REPOINT_SET_ADULT(gOOTMasterSwordHiltDL, sMasterSwordHiltDL);
    REPOINT_SET_ADULT(gOOTMasterSwordSheathDL, gLinkAdultOOT + OOT_LINK_ADULT_SHEATH);
    REPOINT_SET_ADULT(gOOTHookshotFirstPersonDL, gLinkAdultOOT + OOT_LINK_ADULT_FPS_RIGHT_HAND_HOLDING_HOOKSHOT);
    REPOINT_SET_ADULT(gOOTHookshotDL, gLinkAdultOOT + OOT_LINK_ADULT_RIGHT_HAND_HOLDING_HOOKSHOT);
    REPOINT_SET_ADULT(gOOTHookshotChainDL, gLinkAdultOOT + OOT_LINK_ADULT_HOOKSHOT_CHAIN);
    REPOINT_SET_ADULT(gOOTHookshotHookDL, gLinkAdultOOT + OOT_LINK_ADULT_HOOKSHOT_HOOK);
    REPOINT_SET_ADULT(gOOTBowDL, gLinkAdultOOT + OOT_LINK_ADULT_RIGHT_HAND_HOLDING_BOW);
    REPOINT_SET_ADULT(gOOTBowFirstPersonDL, gLinkAdultOOT + OOT_LINK_ADULT_FPS_RIGHT_HAND_AND_BOW);
    REPOINT_SET_ADULT(gOOTBowStringDL, gLinkAdultOOT + OOT_LINK_ADULT_BOW_STRING);
    REPOINT_SET_ADULT(gOOTBiggoronSwordHiltDL, gLinkAdultOOT + OOT_LINK_ADULT_LEFT_HAND_HOLDING_BGS);
    REPOINT_SET_ADULT(gOOTBiggoronSwordBladeDL, sBiggoronSwordBladeDL);
    REPOINT_SET_ADULT(gOOTBottleAdultDL, gLinkAdultOOT + OOT_LINK_ADULT_BOTTLE);

    initCrossAgeEquipment();
}

void initChildEquipment() {
    if (sIsChildEquipmentInitialized || !gLinkChildOOT) {
        return;
    }

    sIsChildEquipmentInitialized = true;

    GlobalObjectsSegmentMap cLinkSegs = {0};
    cLinkSegs[0x06] = gLinkChildOOT;
    cLinkSegs[0x04] = gGameplayKeepOOT;

    // Kokiri Sword (Hilt)
    GlobalObjects_rebaseDL((Gfx *)(gLinkChildOOT + OOT_LINK_CHILD_LFIST_HOLDING_KOKIRI_SWORD), cLinkSegs);
    const uintptr_t KOKIRI_SWORD_HILT_START = 0x14048;
    const uintptr_t KOKIRI_SWORD_HILT_END = 0x14110;
    gSPEndDisplayList(gLinkChildOOT + KOKIRI_SWORD_HILT_END);
    gSPBranchList(&sKokiriSwordHiltDL[ARRAY_COUNT(sKokiriSwordHiltDL) - 1], gLinkChildOOT + KOKIRI_SWORD_HILT_START);

    // Kokiri Sword (Blade)
    const uintptr_t KOKIRI_SWORD_BLADE_START = KOKIRI_SWORD_HILT_END + sizeof(Gfx);
    gSPBranchList(&sKokiriSwordBladeDL[ARRAY_COUNT(sKokiriSwordBladeDL) - 1], gLinkChildOOT + KOKIRI_SWORD_BLADE_START);

    // Deku Shield
    const uintptr_t DEKU_SHIELD_HAND_START = 0x14580;
    gSPEndDisplayList(gLinkChildOOT + DEKU_SHIELD_HAND_START);

    guPosition(&gOOTKokiriSwordHiltMtx, 0.f, 0.f, 0.f, 1.f, -440.f, -211.f, 0.f);

    guPosition(&gOOTDekuShieldMtx, 0.f, 0.f, 180.f, 1.f, 545.f, 0.f, 80.f);

    // Fairy Ocarina
    const uintptr_t FAIRY_OCARINA_HAND_START = 0x15CB8;
    gSPEndDisplayList(gLinkChildOOT + FAIRY_OCARINA_HAND_START);

    // clang-format off
    #define REPOINT_SET_CHILD(ptrToSet, dl) { ptrToSet = (Gfx *)(dl); GlobalObjects_rebaseDL(ptrToSet, cLinkSegs); } (void)0
    // clang-format on

    REPOINT_SET_CHILD(gOOTBottleChildDL, gLinkChildOOT + OOT_LINK_CHILD_BOTTLE_GLASS);
    REPOINT_SET_CHILD(gOOTDekuStickDL, gLinkChildOOT + OOT_LINK_CHILD_DEKU_STICK);
    REPOINT_SET_CHILD(gOOTKokiriSwordSheathDL, gLinkChildOOT + OOT_LINK_CHILD_SHEATH);
    REPOINT_SET_CHILD(gOOTKokiriSwordBladeDL, sKokiriSwordBladeDL);
    REPOINT_SET_CHILD(gOOTKokiriSwordHiltDL, sKokiriSwordHiltDL);
    REPOINT_SET_CHILD(gOOTDekuShieldDL, gLinkChildOOT + OOT_LINK_CHILD_RFIST_HOLDING_DEKU_SHIELD);
    REPOINT_SET_CHILD(gOOTFairyOcarinaDL, gLinkChildOOT + OOT_LINK_CHILD_RFIST_HOLDING_FAIRY_OCARINA);

    initCrossAgeEquipment();
}

extern Mtx gLinkHumanMirrorShieldMtx;
extern Mtx gLinkHumanHerosShieldMtx;

void addEquipmentToModelManager() {
    PlayerModelManagerHandle ootPack = PlayerModelManager_registerModel(PMM_API_VERSION, "__oot_vanilla_equipment__", PMM_MODEL_TYPE_MODEL_PACK);
    PlayerModelManager_setAuthor(ootPack, "Nintendo");
    PlayerModelManager_setDisplayName(ootPack, "OoT Equipment");


    // Mirror Shield (OOT)
    {
        PlayerModelManagerHandle h = PLAYERMODELMANAGER_REGISTER_MODEL("__oot_mirror_shield3__", PMM_MODEL_TYPE_SHIELD_MIRROR);
        PlayerModelManager_setDisplayName(h, "Mirror Shield (OoT)");
        PlayerModelManager_setAuthor(h, "Nintendo");

        PlayerModelManagerHandle handles[] = {h};

        for (int i = 0; i < ARRAY_COUNT(handles); ++i) {
            PlayerModelManagerHandle h1 = handles[i];

            PlayerModelManager_setMatrix(h1, PMM_MTX_SHIELD_MIRROR_BACK, &gOOTAdultShieldMtx);
            PlayerModelManager_setDisplayList(h1, PMM_DL_SHIELD_MIRROR, gOOTMirrorShieldDL);
            PlayerModelManager_setDisplayList(h1, PMM_DL_SHIELD_MIRROR_RAY, gOOTMirrorShieldRayDL);
            PlayerModelManager_setDisplayList(h1, PMM_DL_SHIELD_MIRROR_RAY_BEAM, gOOTMirrorShieldRayBeamAdjustedDL);
        }

        setChildModelTypeMtx(h, PMM_MTX_SHIELD_MIRROR_BACK, SEGMENTED_TO_GLOBAL_PTR(GlobalObjects_getGlobalObject(OBJECT_LINK_CHILD), &gLinkHumanMirrorShieldMtx));
        setChildModelTypeDL(h, PMM_DL_SHIELD_MIRROR, gOOTMirrorShieldChildDL);
        setChildModelTypeDL(h, PMM_DL_SHIELD_MIRROR_RAY, gOOTMirrorShieldRayChildDL);
        setChildModelTypeDL(h, PMM_DL_SHIELD_MIRROR_RAY_BEAM, gOOTMirrorShieldRayBeamChildDL);

        PlayerModelManager_addHandleToPack(ootPack, h);
    }

    // Hylian Shield
    {
        PlayerModelManagerHandle h = PLAYERMODELMANAGER_REGISTER_MODEL("__oot_hylian_shield2__", PMM_MODEL_TYPE_SHIELD_HERO);
        PlayerModelManager_setDisplayName(h, "Hylian Shield (OoT)");
        PlayerModelManager_setAuthor(h, "Nintendo");

        PlayerModelManagerHandle handles[] = {h};

        for (int i = 0; i < ARRAY_COUNT(handles); ++i) {
            PlayerModelManagerHandle h1 = handles[i];

            PlayerModelManager_setMatrix(h1, PMM_MTX_SHIELD_HERO_BACK, &gOOTAdultShieldMtx);
            PlayerModelManager_setDisplayList(h1, PMM_DL_SHIELD_HERO, gOOTHylianShieldDL);
        }

        setChildModelTypeMtx(h, PMM_MTX_SHIELD_HERO_BACK, SEGMENTED_TO_GLOBAL_PTR(GlobalObjects_getGlobalObject(OBJECT_LINK_CHILD), &gLinkHumanHerosShieldMtx));
        setChildModelTypeDL(h, PMM_DL_SHIELD_HERO, gOOTHylianShieldChildDL);

        PlayerModelManager_addHandleToPack(ootPack, h);
    }

    // Deku Shield
    {
        PlayerModelManagerHandle h = PLAYERMODELMANAGER_REGISTER_MODEL("__oot_deku_shield1__", PMM_MODEL_TYPE_SHIELD_DEKU);
        PlayerModelManager_setDisplayName(h, "Deku Shield");
        PlayerModelManager_setAuthor(h, "Nintendo");
        PlayerModelManager_setMatrix(h, PMM_MTX_SHIELD_DEKU_BACK, &gOOTDekuShieldMtx);
        PlayerModelManager_setDisplayList(h, PMM_DL_SHIELD_DEKU, gOOTDekuShieldDL);
        setAdultModelTypeMtx(h, PMM_MTX_SHIELD_DEKU_BACK, &gOOTAdultShieldMtx);
        setAdultModelTypeDL(h, PMM_DL_SHIELD_DEKU, gOOTDekuShieldAdultDL);
    }

    {
        PlayerModelManagerHandle h = PLAYERMODELMANAGER_REGISTER_MODEL("__oot_deku_shield2__", PMM_MODEL_TYPE_SHIELD_HERO);
        PlayerModelManager_setDisplayName(h, "Deku Shield");
        PlayerModelManager_setAuthor(h, "Nintendo");
        PlayerModelManager_setMatrix(h, PMM_MTX_SHIELD_HERO_BACK, &gOOTDekuShieldMtx);
        PlayerModelManager_setDisplayList(h, PMM_DL_SHIELD_HERO, gOOTDekuShieldDL);
        setAdultModelTypeMtx(h, PMM_MTX_SHIELD_HERO_BACK, &gOOTAdultShieldMtx);
        setAdultModelTypeDL(h, PMM_DL_SHIELD_HERO, gOOTDekuShieldAdultDL);
    }

    // Master Sword
    {
        PlayerModelManagerHandle h = PLAYERMODELMANAGER_REGISTER_MODEL("__oot_master_sword1__", PMM_MODEL_TYPE_SWORD_KOKIRI);
        PlayerModelManager_setDisplayName(h, "Master Sword");
        PlayerModelManager_setAuthor(h, "Nintendo");

        PlayerModelManagerHandle handles[] = {h};

        for (int i = 0; i < ARRAY_COUNT(handles); ++i) {
            PlayerModelManagerHandle h1 = handles[i];

            PlayerModelManager_setMatrix(h1, PMM_MTX_SWORD_KOKIRI_BACK, &gOOTMasterSwordHiltMtx);
            PlayerModelManager_setDisplayList(h1, PMM_DL_SWORD_KOKIRI_BLADE, gOOTMasterSwordBladeDL);
            PlayerModelManager_setDisplayList(h1, PMM_DL_SWORD_KOKIRI_HILT, gOOTMasterSwordHiltDL);
            PlayerModelManager_setDisplayList(h1, PMM_DL_SWORD_KOKIRI_SHEATH, gOOTMasterSwordSheathDL);
        }

        setChildModelTypeMtx(h, PMM_MTX_SWORD_KOKIRI_BACK, &gMasterSwordBackChildMtx);
        setChildModelTypeDL(h, PMM_DL_SWORD_KOKIRI_BLADE, gOOTMasterSwordBladeChildDL);
        setChildModelTypeDL(h, PMM_DL_SWORD_KOKIRI_HILT, gOOTMasterSwordHiltChildDL);
        setChildModelTypeDL(h, PMM_DL_SWORD_KOKIRI_SHEATH, gOOTMasterSwordSheathChildDL);

        PlayerModelManager_addHandleToPack(ootPack, h);
    }

    {
        PlayerModelManagerHandle h = PLAYERMODELMANAGER_REGISTER_MODEL("__oot_master_sword2__", PMM_MODEL_TYPE_SWORD_RAZOR);
        PlayerModelManager_setDisplayName(h, "Master Sword");
        PlayerModelManager_setAuthor(h, "Nintendo");

        PlayerModelManagerHandle handles[] = {h};

        for (int i = 0; i < ARRAY_COUNT(handles); ++i) {
            PlayerModelManagerHandle h1 = handles[i];

            PlayerModelManager_setMatrix(h1, PMM_MTX_SWORD_RAZOR_BACK, &gOOTMasterSwordHiltMtx);
            PlayerModelManager_setDisplayList(h1, PMM_DL_SWORD_RAZOR_BLADE, gOOTMasterSwordBladeDL);
            PlayerModelManager_setDisplayList(h1, PMM_DL_SWORD_RAZOR_HILT, gOOTMasterSwordHiltDL);
            PlayerModelManager_setDisplayList(h1, PMM_DL_SWORD_RAZOR_SHEATH, gOOTMasterSwordSheathDL);
        }

        setChildModelTypeMtx(h, PMM_MTX_SWORD_RAZOR_BACK, &gMasterSwordBackChildMtx);
        setChildModelTypeDL(h, PMM_DL_SWORD_RAZOR_BLADE, gOOTMasterSwordBladeChildDL);
        setChildModelTypeDL(h, PMM_DL_SWORD_RAZOR_HILT, gOOTMasterSwordHiltChildDL);
        setChildModelTypeDL(h, PMM_DL_SWORD_RAZOR_SHEATH, gOOTMasterSwordSheathChildDL);

        PlayerModelManager_addHandleToPack(ootPack, h);
    }

    {
        PlayerModelManagerHandle h = PLAYERMODELMANAGER_REGISTER_MODEL("__oot_master_sword3__", PMM_MODEL_TYPE_SWORD_GILDED);
        PlayerModelManager_setDisplayName(h, "Master Sword");
        PlayerModelManager_setAuthor(h, "Nintendo");

        PlayerModelManagerHandle handles[] = {h};

        for (int i = 0; i < ARRAY_COUNT(handles); ++i) {
            PlayerModelManagerHandle h1 = handles[i];

            PlayerModelManager_setMatrix(h1, PMM_MTX_SWORD_GILDED_BACK, &gOOTMasterSwordHiltMtx);
            PlayerModelManager_setDisplayList(h1, PMM_DL_SWORD_GILDED_BLADE, gOOTMasterSwordBladeDL);
            PlayerModelManager_setDisplayList(h1, PMM_DL_SWORD_GILDED_HILT, gOOTMasterSwordHiltDL);
            PlayerModelManager_setDisplayList(h1, PMM_DL_SWORD_GILDED_SHEATH, gOOTMasterSwordSheathDL);
        }

        setChildModelTypeMtx(h, PMM_MTX_SWORD_GILDED_BACK, &gMasterSwordBackChildMtx);
        setChildModelTypeDL(h, PMM_DL_SWORD_GILDED_BLADE, gOOTMasterSwordBladeChildDL);
        setChildModelTypeDL(h, PMM_DL_SWORD_GILDED_HILT, gOOTMasterSwordHiltChildDL);
        setChildModelTypeDL(h, PMM_DL_SWORD_GILDED_SHEATH, gOOTMasterSwordSheathChildDL);

        PlayerModelManager_addHandleToPack(ootPack, h);
    }

    // Kokiri Sword (OOT)
    {
        PlayerModelManagerHandle h = PLAYERMODELMANAGER_REGISTER_MODEL("__oot_kokiri_sword1__", PMM_MODEL_TYPE_SWORD_KOKIRI);
        PlayerModelManager_setDisplayName(h, "Kokiri Sword (OoT)");
        PlayerModelManager_setAuthor(h, "Nintendo");

        PlayerModelManagerHandle handles[] = {h};

        for (int i = 0; i < ARRAY_COUNT(handles); ++i) {
            PlayerModelManagerHandle h1 = handles[i];

            PlayerModelManager_setMatrix(h1, PMM_MTX_SWORD_KOKIRI_BACK, &gOOTKokiriSwordHiltMtx);
            PlayerModelManager_setDisplayList(h1, PMM_DL_SWORD_KOKIRI_BLADE, gOOTKokiriSwordBladeDL);
            PlayerModelManager_setDisplayList(h1, PMM_DL_SWORD_KOKIRI_HILT, gOOTKokiriSwordHiltDL);
            PlayerModelManager_setDisplayList(h1, PMM_DL_SWORD_KOKIRI_SHEATH, gOOTKokiriSwordSheathDL);
        }

        setAdultModelTypeMtx(h, PMM_MTX_SWORD_KOKIRI_BACK, &gKokiriSwordBackAdultMtx);
        setAdultModelTypeDL(h, PMM_DL_SWORD_KOKIRI_BLADE, gOOTKokiriSwordBladeAdultDL);
        setAdultModelTypeDL(h, PMM_DL_SWORD_KOKIRI_HILT, gOOTKokiriSwordHiltAdultDL);
        setAdultModelTypeDL(h, PMM_DL_SWORD_KOKIRI_SHEATH, gOOTKokiriSwordSheathAdultDL);

        PlayerModelManager_addHandleToPack(ootPack, h);
    }

    // Biggoron Sword
    {
        PlayerModelManagerHandle h = PLAYERMODELMANAGER_REGISTER_MODEL("__oot_biggoron_sword5__", PMM_MODEL_TYPE_SWORD_GREAT_FAIRY);
        PlayerModelManager_setDisplayName(h, "Biggoron Sword");
        PlayerModelManager_setAuthor(h, "Nintendo");

        PlayerModelManagerHandle handles[] = {h};

        for (int i = 0; i < ARRAY_COUNT(handles); ++i) {
            PlayerModelManagerHandle h1 = handles[i];

            PlayerModelManager_setMatrix(h1, PMM_MTX_SWORD5_BACK, &gOOTMasterSwordHiltMtx);
            PlayerModelManager_setDisplayList(h1, PMM_DL_SWORD_GREAT_FAIRY_BLADE, gOOTBiggoronSwordBladeDL);
            PlayerModelManager_setDisplayList(h1, PMM_DL_SWORD_GREAT_FAIRY_HILT, gOOTBiggoronSwordHiltDL);
            PlayerModelManager_setDisplayList(h1, PMM_DL_SWORD5_SHEATH, gOOTMasterSwordSheathDL);
        }

        setChildModelTypeMtx(h, PMM_MTX_SWORD5_BACK, &gMasterSwordBackChildMtx);
        setChildModelTypeDL(h, PMM_DL_SWORD_GREAT_FAIRY_BLADE, gOOTBiggoronSwordBladeChildDL);
        setChildModelTypeDL(h, PMM_DL_SWORD_GREAT_FAIRY_HILT, gOOTBiggoronSwordHiltChildDL);
        setChildModelTypeDL(h, PMM_DL_SWORD5_SHEATH, gOOTMasterSwordSheathChildDL);

        PlayerModelManager_addHandleToPack(ootPack, h);
    }

    {
        PlayerModelManagerHandle h = PLAYERMODELMANAGER_REGISTER_MODEL("__oot_biggoron_sword4__", PMM_MODEL_TYPE_SWORD_FIERCE_DIETY);
        PlayerModelManager_setDisplayName(h, "Biggoron Sword");
        PlayerModelManager_setAuthor(h, "Nintendo");
        PlayerModelManager_setMatrix(h, PMM_MTX_SWORD4_BACK, &gOOTMasterSwordHiltMtx);

        PlayerModelManagerHandle handles[] = {h};

        for (int i = 0; i < ARRAY_COUNT(handles); ++i) {
            PlayerModelManagerHandle h1 = handles[i];

            PlayerModelManager_setMatrix(h1, PMM_MTX_SWORD4_BACK, &gOOTMasterSwordHiltMtx);
            PlayerModelManager_setDisplayList(h1, PMM_DL_SWORD_FIERCE_DEITY_BLADE, gOOTBiggoronSwordBladeDL);
            PlayerModelManager_setDisplayList(h1, PMM_DL_SWORD_FIERCE_DEITY_HILT, gOOTBiggoronSwordHiltDL);
            PlayerModelManager_setDisplayList(h1, PMM_DL_SWORD4_SHEATH, gOOTMasterSwordSheathDL);
        }

        setChildModelTypeMtx(h, PMM_MTX_SWORD4_BACK, &gMasterSwordBackChildMtx);
        setChildModelTypeDL(h, PMM_DL_SWORD_FIERCE_DEITY_BLADE, gOOTBiggoronSwordBladeChildDL);
        setChildModelTypeDL(h, PMM_DL_SWORD_FIERCE_DEITY_HILT, gOOTBiggoronSwordHiltChildDL);
        setChildModelTypeDL(h, PMM_DL_SWORD4_SHEATH, gOOTMasterSwordSheathChildDL);

        PlayerModelManager_addHandleToPack(ootPack, h);
    }

    // Hookshot (OOT)
    {
        PlayerModelManagerHandle h = PLAYERMODELMANAGER_REGISTER_MODEL("__oot_hookshot__", PMM_MODEL_TYPE_HOOKSHOT);
        PlayerModelManager_setDisplayName(h, "Hookshot (OoT)");
        PlayerModelManager_setAuthor(h, "Nintendo");

        PlayerModelManagerHandle handles[] = {h};

        for (int i = 0; i < ARRAY_COUNT(handles); ++i) {
            PlayerModelManagerHandle h1 = handles[i];

            PlayerModelManager_setMatrix(h1, PMM_MTX_HOOKSHOT_CHAIN_AND_HOOK, &gOOTHookshotHookAndChainMtx);
            PlayerModelManager_setDisplayList(h1, PMM_DL_HOOKSHOT, gOOTHookshotDL);
            PlayerModelManager_setDisplayList(h1, PMM_DL_FPS_HOOKSHOT, gOOTHookshotFirstPersonDL);
            PlayerModelManager_setDisplayList(h1, PMM_DL_HOOKSHOT_CHAIN, gOOTHookshotChainDL);
            PlayerModelManager_setDisplayList(h1, PMM_DL_HOOKSHOT_HOOK, gOOTHookshotHookDL);
        }

        setChildModelTypeMtx(h, PMM_MTX_HOOKSHOT_CHAIN_AND_HOOK, &gOOTHookshotHookAndChainChildMtx);
        setChildModelTypeDL(h, PMM_DL_HOOKSHOT, gOOTHookshotChildDL);
        setChildModelTypeDL(h, PMM_DL_FPS_HOOKSHOT, gOOTHookshotFirstPersonChildDL);
        setChildModelTypeDL(h, PMM_DL_HOOKSHOT_HOOK, gOOTHookshotHookChildDL);

        PlayerModelManager_addHandleToPack(ootPack, h);
    }

    // Bottle (OOT)
    {
        PlayerModelManagerHandle h = PLAYERMODELMANAGER_REGISTER_MODEL("__oot_bottle__", PMM_MODEL_TYPE_BOTTLE);
        PlayerModelManager_setDisplayName(h, "Bottle (OoT) (Adult)");
        PlayerModelManager_setAuthor(h, "Nintendo");

        PlayerModelManagerHandle handles[] = {h};

        for (int i = 0; i < ARRAY_COUNT(handles); ++i) {
            PlayerModelManagerHandle h1 = handles[i];

            PlayerModelManager_setDisplayList(h1, PMM_DL_BOTTLE_GLASS, gOOTBottleAdultDL);
            PlayerModelManager_setDisplayList(h1, PMM_DL_BOTTLE_CONTENTS, gEmptyDL);
        }

        setChildModelTypeDL(h, PMM_DL_BOTTLE_GLASS, gOOTBottleChildDL);

        PlayerModelManager_addHandleToPack(ootPack, h);
    }

    // Bow (OOT)
    {
        PlayerModelManagerHandle h = PLAYERMODELMANAGER_REGISTER_MODEL("__oot_bow__", PMM_MODEL_TYPE_BOW);
        PlayerModelManager_setDisplayName(h, "Bow (OoT)");
        PlayerModelManager_setAuthor(h, "Nintendo");

        PlayerModelManagerHandle handles[] = {h};

        for (int i = 0; i < ARRAY_COUNT(handles); ++i) {
            PlayerModelManagerHandle h1 = handles[i];

            PlayerModelManager_setDisplayList(h1, PMM_DL_BOW, gOOTBowDL);
            PlayerModelManager_setDisplayList(h1, PMM_DL_FPS_BOW, gOOTBowFirstPersonDL);
            PlayerModelManager_setDisplayList(h1, PMM_DL_BOW_STRING, gOOTBowStringDL);
        }

        setChildModelTypeDL(h, PMM_DL_BOW, gOOTBowChildDL);
        setChildModelTypeDL(h, PMM_DL_FPS_BOW, gOOTBowFirstPersonChildDL);
        setChildModelTypeDL(h, PMM_DL_BOW_STRING, gOOTBowStringChildDL);

        PlayerModelManager_addHandleToPack(ootPack, h);
    }

    // Fairy Ocarina
    {
        PlayerModelManagerHandle h = PLAYERMODELMANAGER_REGISTER_MODEL("__oot_ocarina1_fairy__", PMM_MODEL_TYPE_OCARINA_FAIRY);
        PlayerModelManager_setDisplayName(h, "Fairy Ocarina");
        PlayerModelManager_setAuthor(h, "Nintendo");
        PlayerModelManager_setDisplayList(h, PMM_DL_OCARINA_FAIRY, gOOTFairyOcarinaDL);
        setAdultModelTypeDL(h, PMM_DL_OCARINA_FAIRY, gOOTFairyOcarinaAdultDL);
    }

    {
        PlayerModelManagerHandle h = PLAYERMODELMANAGER_REGISTER_MODEL("__oot_ocarina2_fairy__", PMM_MODEL_TYPE_OCARINA_TIME);
        PlayerModelManager_setDisplayName(h, "Fairy Ocarina");
        PlayerModelManager_setAuthor(h, "Nintendo");
        PlayerModelManager_setDisplayList(h, PMM_DL_OCARINA_TIME, gOOTFairyOcarinaDL);
        setAdultModelTypeDL(h, PMM_DL_OCARINA_TIME, gOOTFairyOcarinaAdultDL);
    }

    // Deku Stick (OOT)
    {
        PlayerModelManagerHandle h = PLAYERMODELMANAGER_REGISTER_MODEL("__oot_deku_stick__", PMM_MODEL_TYPE_DEKU_STICK);
        PlayerModelManager_setDisplayName(h, "Deku Stick (OoT)");
        PlayerModelManager_setAuthor(h, "Nintendo");

        PlayerModelManagerHandle handles[] = {h};

        for (int i = 0; i < ARRAY_COUNT(handles); ++i) {
            PlayerModelManagerHandle h1 = handles[i];

            PlayerModelManager_setDisplayList(h1, PMM_DL_DEKU_STICK, gOOTDekuStickDL);
        }

        PlayerModelManager_addHandleToPack(ootPack, h);
    }
}
