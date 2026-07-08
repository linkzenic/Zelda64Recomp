#include "global.h"
#include "playermodelmanager_api.h"
#include "globalobjects_api.h"
#include "apilocal.h"
#include "modelentrymanager.h"
#include "defaultfacetex.h"
#include "logger.h"
#include "customdls.h"
#include "mm/vanillawrapperdls.h"
#include "equipmentbuiltin.h"

typedef struct DLIdToBuiltInDL {
    Link_DisplayList dlId;
    Gfx *builtInDL;
} DLIdToBuiltInDL;
#define DECLARE_BUILT_IN_DL(tgt, src) {.dlId = tgt, .builtInDL = src}

typedef struct MtxIdToBuiltInMtx {
    Link_EquipmentMatrix mtxId;
    Mtx *builtInMtx;
} MtxIdToBuiltInMtx;
#define DECLARE_BUILT_IN_MTX(tgt, src) {.mtxId = tgt, .builtInMtx = src}

void FallbackModelsCommon_doCommonAssignments(PlayerModelManagerHandle h, FlexSkeletonHeader *skel, void *seg06, void *seg04) {
    ModelEntry *entry = ModelEntryManager_getEntry(h);

    if (!entry) {
        Logger_printWarning("Invalid PlayerModelManagerHandle passed in!");
        return;
    }

    GlobalObjectsSegmentMap segments = {0};
    segments[0x04] = seg04;
    segments[0x06] = seg06;

    GlobalObjects_globalizeLodLimbSkeleton(seg06, skel);

    for (int i = 0; i < skel->sh.limbCount; ++i) {
        LodLimb *curr = skel->sh.segment[i];

        if (curr->dLists[0]) {
            GlobalObjects_rebaseDL(curr->dLists[0], segments);
        }

        if (curr->dLists[1]) {
            GlobalObjects_rebaseDL(curr->dLists[1], segments);
        }
    }

    PlayerModelManager_setSkeleton(h, skel);

    TexturePtr eyesTex[PLAYER_EYES_MAX];
    for (PlayerEyeIndex i = 0; i < PLAYER_EYES_MAX; ++i) {
        eyesTex[i] = SEGMENTED_TO_GLOBAL_PTR(seg06, gDefaultEyesTextures[i]);
    }
    PlayerModelManager_setEyesTextures(h, eyesTex);

    TexturePtr mouthTex[PLAYER_MOUTH_MAX];
    for (PlayerMouthIndex i = 0; i < PLAYER_MOUTH_MAX; ++i) {
        mouthTex[i] = SEGMENTED_TO_GLOBAL_PTR(seg06, gDefaultMouthTextures[i]);
    }
    PlayerModelManager_setMouthTextures(h, mouthTex);

    ModelEntryManager_setEntryHidden(entry, true);
}

static void addAllBuiltInDLs(PlayerModelManagerHandle h, DLIdToBuiltInDL builtIns[], size_t n) {
    for (size_t i = 0; i < n; ++i) {
        DLIdToBuiltInDL *curr = &builtIns[i];
        PlayerModelManager_setDisplayList(h, curr->dlId, curr->builtInDL);
    }
}

static void addAllBuiltInMtxs(PlayerModelManagerHandle h, MtxIdToBuiltInMtx builtIns[], size_t n) {
    for (size_t i = 0; i < n; ++i) {
        MtxIdToBuiltInMtx *curr = &builtIns[i];
        PlayerModelManager_setMatrix(h, curr->mtxId, curr->builtInMtx);
    }
}

static DLIdToBuiltInDL sBuiltInDLCommonZ64[] = {
    DECLARE_BUILT_IN_DL(LINK_DL_OPT_FPS_LSHOULDER_SLINGSHOT, gEmptyDL),
    DECLARE_BUILT_IN_DL(LINK_DL_OPT_FPS_LFOREARM_SLINGSHOT, gEmptyDL),
    DECLARE_BUILT_IN_DL(LINK_DL_OPT_FPS_LHAND_SLINGSHOT, gEmptyDL),
    DECLARE_BUILT_IN_DL(LINK_DL_PIPE_MOUTH, gCallDekuPipeMouthDL),
    DECLARE_BUILT_IN_DL(LINK_DL_PIPE_RIGHT, gCallDekuPipeRightDL),
    DECLARE_BUILT_IN_DL(LINK_DL_PIPE_UP, gCallDekuPipeUpDL),
    DECLARE_BUILT_IN_DL(LINK_DL_PIPE_DOWN, gCallDekuPipeDownDL),
    DECLARE_BUILT_IN_DL(LINK_DL_PIPE_LEFT, gCallDekuPipeLeftDL),
    DECLARE_BUILT_IN_DL(LINK_DL_PIPE_A, gCallDekuPipeADL),
    DECLARE_BUILT_IN_DL(LINK_DL_DRUM_STRAP, gCallGoronDrumStrapDL),
    DECLARE_BUILT_IN_DL(LINK_DL_DRUM_RIGHT, gCallGoronDrumRightDL),
    DECLARE_BUILT_IN_DL(LINK_DL_DRUM_UP, gCallGoronDrumUpDL),
    DECLARE_BUILT_IN_DL(LINK_DL_DRUM_DOWN, gCallGoronDrumDownDL),
    DECLARE_BUILT_IN_DL(LINK_DL_DRUM_LEFT, gCallGoronDrumLeftDL),
    DECLARE_BUILT_IN_DL(LINK_DL_DRUM_A, gCallGoronDrumADL),
    DECLARE_BUILT_IN_DL(LINK_DL_GUITAR, gCallZoraGuitarDL),
    DECLARE_BUILT_IN_DL(LINK_DL_MASK_KAFEIS_MASK, gCallHumanMaskKafeiDL),
    DECLARE_BUILT_IN_DL(LINK_DL_MASK_ALL_NIGHT, gCallHumanMaskAllNightDL),
    DECLARE_BUILT_IN_DL(LINK_DL_MASK_GARO, gCallHumanMaskGaroDL),
    DECLARE_BUILT_IN_DL(LINK_DL_MASK_ROMANI, gCallHumanMaskRomaniDL),
    DECLARE_BUILT_IN_DL(LINK_DL_MASK_CIRCUS_LEADER, gCallHumanMaskCircusLeaderDL),
    DECLARE_BUILT_IN_DL(LINK_DL_MASK_COUPLE, gCallHumanMaskCoupleDL),
    DECLARE_BUILT_IN_DL(LINK_DL_MASK_POSTMAN, gCallHumanMaskPostmanDL),
    DECLARE_BUILT_IN_DL(LINK_DL_MASK_GREAT_FAIRY, gCallHumanMaskGreatFairyDL),
    DECLARE_BUILT_IN_DL(LINK_DL_MASK_GIBDO, gCallHumanMaskGibdoDL),
    DECLARE_BUILT_IN_DL(LINK_DL_MASK_DON_GERO, gCallHumanMaskDonGeroDL),
    DECLARE_BUILT_IN_DL(LINK_DL_MASK_KAMARO, gCallHumanMaskKamaroDL),
    DECLARE_BUILT_IN_DL(LINK_DL_MASK_CAPTAIN, gCallHumanMaskCaptainDL),
    DECLARE_BUILT_IN_DL(LINK_DL_MASK_STONE, gCallHumanMaskStoneDL),
    DECLARE_BUILT_IN_DL(LINK_DL_MASK_BREMEN, gCallHumanMaskBremenDL),
    DECLARE_BUILT_IN_DL(LINK_DL_MASK_BLAST, gCallHumanMaskBlastDL),
    DECLARE_BUILT_IN_DL(LINK_DL_MASK_BLAST_COOLING_DOWN, gCallHumanMaskBlastCooldownDL),
    DECLARE_BUILT_IN_DL(LINK_DL_MASK_SCENTS, gCallHumanMaskScentsDL),
    DECLARE_BUILT_IN_DL(LINK_DL_MASK_GIANT, gCallHumanMaskGiantDL),
    DECLARE_BUILT_IN_DL(LINK_DL_MASK_DEKU, gCallHumanMaskDekuDL),
    DECLARE_BUILT_IN_DL(LINK_DL_MASK_FIERCE_DEITY, gCallHumanMaskFierceDeityDL),
    DECLARE_BUILT_IN_DL(LINK_DL_MASK_DEKU_SCREAM, gCallHumanMaskDekuScreamDL),
    DECLARE_BUILT_IN_DL(LINK_DL_MASK_GORON_SCREAM, gCallHumanMaskGoronScreamDL),
    DECLARE_BUILT_IN_DL(LINK_DL_MASK_ZORA_SCREAM, gCallHumanMaskZoraScreamDL),
    DECLARE_BUILT_IN_DL(LINK_DL_MASK_FIERCE_DEITY_SCREAM, gCallHumanMaskFierceDeityScreamDL),
    DECLARE_BUILT_IN_DL(LINK_DL_LFIN, gCallZoraFinLeftDL),
    DECLARE_BUILT_IN_DL(LINK_DL_RFIN, gCallZoraFinRightDL),
    DECLARE_BUILT_IN_DL(LINK_DL_LFIN_BOOMERANG, gCallZoraBoomerangLeftDL),
    DECLARE_BUILT_IN_DL(LINK_DL_RFIN_BOOMERANG, gCallZoraBoomerangRightDL),
    DECLARE_BUILT_IN_DL(LINK_DL_LFIN_SWIM, gCallZoraFinSwimLeftDL),
    DECLARE_BUILT_IN_DL(LINK_DL_RFIN_SWIM, gCallZoraFinSwimRightDL),
    DECLARE_BUILT_IN_DL(LINK_DL_FIN_SHIELD, gCallZoraFinShieldDL),
    DECLARE_BUILT_IN_DL(LINK_DL_MAGIC_BARRIER, gZoraMagicBarrierDL),
    DECLARE_BUILT_IN_DL(LINK_DL_STEM_RIGHT, gCallDekuStemRightDL),
    DECLARE_BUILT_IN_DL(LINK_DL_STEM_LEFT, gCallDekuStemLeftDL),
    DECLARE_BUILT_IN_DL(LINK_DL_FLOWER_CENTER_CLOSED, gCallDekuFlowerCenterClosedDL),
    DECLARE_BUILT_IN_DL(LINK_DL_FLOWER_CENTER_OPEN, gCallDekuFlowerCenterOpenDL),
    DECLARE_BUILT_IN_DL(LINK_DL_FLOWER_PROPELLER_CLOSED, gCallDekuFlowerPropellerClosedDL),
    DECLARE_BUILT_IN_DL(LINK_DL_FLOWER_PROPELLER_OPEN, gCallDekuFlowerPropellerOpenDL),
    DECLARE_BUILT_IN_DL(LINK_DL_PETAL_PARTICLE, gCallDekuFlowerParticleDL),
    DECLARE_BUILT_IN_DL(LINK_DL_PAD_WOOD, gCallDekuPadWoodDL),
    DECLARE_BUILT_IN_DL(LINK_DL_PAD_GRASS, gCallDekuPadGrassDL),
    DECLARE_BUILT_IN_DL(LINK_DL_PAD_OPENING, gCallDekuPadOpeningDL),
    DECLARE_BUILT_IN_DL(LINK_DL_DEKU_GUARD, gCallDekuGuardDL),
    DECLARE_BUILT_IN_DL(LINK_DL_CURLED, gCallGoronCurledDL),
    DECLARE_BUILT_IN_DL(LINK_DL_SPIKES, gCallGoronSpikesDL),
    DECLARE_BUILT_IN_DL(LINK_DL_FIRE_INIT, gCallGoronFireInitDL),
    DECLARE_BUILT_IN_DL(LINK_DL_FIRE_ROLL, gCallGoronFireRollDL),
    DECLARE_BUILT_IN_DL(LINK_DL_FIRE_PUNCH, gCallGoronFirePunchDL),
    DECLARE_BUILT_IN_DL(LINK_DL_BODY_SHIELD_BODY, gCallGoronShieldBodyDL),
    DECLARE_BUILT_IN_DL(LINK_DL_BODY_SHIELD_HEAD, gCallGoronShieldHeadDL),
    DECLARE_BUILT_IN_DL(LINK_DL_BODY_SHIELD_ARMS_AND_LEGS, gCallGoronShieldArmsAndLegsDL),
    DECLARE_BUILT_IN_DL(LINK_DL_BOMB_BODY_3D, gEmptyDL),
};

void FallbackModelsCommon_addEquipmentCommonZ64(PlayerModelManagerHandle h) {
    for (Link_EquipmentMatrix i = 0; i < LINK_EQUIP_MATRIX_MAX; ++i) {
        PlayerModelManager_setMatrix(h, i, &gIdentityMtx);
    }

    addAllBuiltInDLs(h, sBuiltInDLCommonZ64, ARRAY_COUNT(sBuiltInDLCommonZ64));
}

static DLIdToBuiltInDL sBuiltInDLCommonMM[] = {
    DECLARE_BUILT_IN_DL(LINK_DL_HOOKSHOT_RETICLE, gCallHumanHookshotReticleDL),
    DECLARE_BUILT_IN_DL(LINK_DL_HOOKSHOT_CHAIN, gCallHumanHookshotChainDL),
    DECLARE_BUILT_IN_DL(LINK_DL_BOW_ARROW, gCallHumanArrowDL),
    DECLARE_BUILT_IN_DL(LINK_DL_BOTTLE_GLASS, gCallHumanBottleGlassDL),
    DECLARE_BUILT_IN_DL(LINK_DL_BOTTLE_CONTENTS, gCallHumanBottleContentsDL),
    DECLARE_BUILT_IN_DL(LINK_DL_DEKU_STICK, gCallHumanDekuStickDL),
    DECLARE_BUILT_IN_DL(LINK_DL_MASK_TRUTH, gCallHumanMaskTruthDL),
    DECLARE_BUILT_IN_DL(LINK_DL_MASK_BUNNY, gCallHumanMaskBunnyDL),
    DECLARE_BUILT_IN_DL(LINK_DL_MASK_KEATON, gCallHumanMaskKeatonDL),
    DECLARE_BUILT_IN_DL(LINK_DL_MASK_GORON, gCallHumanMaskGoronDL),
    DECLARE_BUILT_IN_DL(LINK_DL_MASK_ZORA, gCallHumanMaskZoraDL),
    DECLARE_BUILT_IN_DL(LINK_DL_BOMB_CAP, gCallHumanBombCapDL),
    DECLARE_BUILT_IN_DL(LINK_DL_BOMB_BODY_2D, gCallHumanBombBodyDL),
    DECLARE_BUILT_IN_DL(LINK_DL_BOMBCHU, gCallHumanBombchuDL),
};

void FallbackModelsCommon_addEquipmentCommonMM(PlayerModelManagerHandle h) {
    FallbackModelsCommon_addEquipmentCommonZ64(h);

    addAllBuiltInDLs(h, sBuiltInDLCommonMM, ARRAY_COUNT(sBuiltInDLCommonMM));
}

static MtxIdToBuiltInMtx sBuiltInMtxChildMM[] = {
    DECLARE_BUILT_IN_MTX(LINK_EQUIP_MATRIX_SWORD_KOKIRI_BACK, &gHumanKokiriSwordHiltBackMtx),
    DECLARE_BUILT_IN_MTX(LINK_EQUIP_MATRIX_SWORD_RAZOR_BACK, &gHumanRazorSwordHiltBackMtx),
    DECLARE_BUILT_IN_MTX(LINK_EQUIP_MATRIX_SWORD_GILDED_BACK, &gHumanGildedSwordHiltBackMtx),
    DECLARE_BUILT_IN_MTX(LINK_EQUIP_MATRIX_SHIELD1_BACK, &gChildDekuShieldBackMtx),
    DECLARE_BUILT_IN_MTX(LINK_EQUIP_MATRIX_SHIELD_HERO_BACK, &gHumanHeroShieldBackMtx),
    DECLARE_BUILT_IN_MTX(LINK_EQUIP_MATRIX_SHIELD_MIRROR_BACK, &gHumanMirrorShieldBackMtx),
};

static DLIdToBuiltInDL sBuiltInDLChildMM[] = {
    DECLARE_BUILT_IN_DL(LINK_DL_SWORD_KOKIRI_HILT, gCallHumanKokiriSwordHiltDL),
    DECLARE_BUILT_IN_DL(LINK_DL_SWORD_KOKIRI_BLADE, gCallHumanKokiriSwordBladeDL),
    DECLARE_BUILT_IN_DL(LINK_DL_SWORD_KOKIRI_SHEATH, gCallHumanKokiriSwordSheathDL),
    DECLARE_BUILT_IN_DL(LINK_DL_SWORD_RAZOR_HILT, gCallHumanRazorSwordHiltDL),
    DECLARE_BUILT_IN_DL(LINK_DL_SWORD_RAZOR_BLADE, gCallHumanRazorSwordBladeDL),
    DECLARE_BUILT_IN_DL(LINK_DL_SWORD_RAZOR_SHEATH, gCallHumanRazorSwordSheathDL),
    DECLARE_BUILT_IN_DL(LINK_DL_SWORD_GILDED_HILT, gCallHumanGildedSwordHiltDL),
    DECLARE_BUILT_IN_DL(LINK_DL_SWORD_GILDED_BLADE, gCallHumanGildedSwordBladeDL),
    DECLARE_BUILT_IN_DL(LINK_DL_SWORD_GILDED_SHEATH, gCallHumanGildedSwordSheathDL),
    DECLARE_BUILT_IN_DL(LINK_DL_SWORD_FIERCE_DEITY_HILT, gEmptyDL),
    DECLARE_BUILT_IN_DL(LINK_DL_SWORD_FIERCE_DEITY_BLADE, gFierceDeityChildSwordDL),
    DECLARE_BUILT_IN_DL(LINK_DL_SWORD_GREAT_FAIRY_HILT, gEmptyDL),
    DECLARE_BUILT_IN_DL(LINK_DL_SWORD_GREAT_FAIRY_BLADE, gCallHumanGreatFairySwordDL),
    DECLARE_BUILT_IN_DL(LINK_DL_SHIELD_HERO, gCallHumanHeroShieldDL),
    DECLARE_BUILT_IN_DL(LINK_DL_SHIELD_MIRROR, gCallHumanMirrorShieldDL),
    DECLARE_BUILT_IN_DL(LINK_DL_SHIELD_MIRROR_RAY, gCallHumanMirrorShieldRayDL),
    DECLARE_BUILT_IN_DL(LINK_DL_SHIELD_MIRROR_RAY_BEAM, gCallHumanMirrorShieldRayBeamDL),
    DECLARE_BUILT_IN_DL(LINK_DL_HOOKSHOT, gCallHumanHookshotDL),
    DECLARE_BUILT_IN_DL(LINK_DL_FPS_HOOKSHOT, gLinkHumanFirstPersonHookshotDL),
    DECLARE_BUILT_IN_DL(LINK_DL_HOOKSHOT_HOOK, gCallHumanHookshotHookDL),
    DECLARE_BUILT_IN_DL(LINK_DL_BOW, gCallHumanBowDL),
    DECLARE_BUILT_IN_DL(LINK_DL_FPS_BOW, gLinkHumanFirstPersonBowDL),
    DECLARE_BUILT_IN_DL(LINK_DL_BOW_STRING, gCallHumanBowStringDL),
    DECLARE_BUILT_IN_DL(LINK_DL_OCARINA_TIME, gLinkHumanOcarinaDL),
};

void FallbackModelsCommon_addEquipmentChildMM(PlayerModelManagerHandle h) {
    FallbackModelsCommon_addEquipmentCommonZ64(h);
    FallbackModelsCommon_addEquipmentCommonMM(h);

    addAllBuiltInDLs(h, sBuiltInDLChildMM, ARRAY_COUNT(sBuiltInDLChildMM));
    addAllBuiltInMtxs(h, sBuiltInMtxChildMM, ARRAY_COUNT(sBuiltInMtxChildMM));
}

static MtxIdToBuiltInMtx sBuiltInMtxAdultMM[] = {
    DECLARE_BUILT_IN_MTX(LINK_EQUIP_MATRIX_SWORD_KOKIRI_BACK, &gHumanAdultKokiriSwordHiltBackMtx),
    DECLARE_BUILT_IN_MTX(LINK_EQUIP_MATRIX_SWORD_RAZOR_BACK, &gHumanAdultRazorSwordHiltBackMtx),
    DECLARE_BUILT_IN_MTX(LINK_EQUIP_MATRIX_SWORD_GILDED_BACK, &gHumanAdultGildedSwordHiltBackMtx),
    DECLARE_BUILT_IN_MTX(LINK_EQUIP_MATRIX_SHIELD1_BACK, &gHumanAdultHeroShieldBackMtx),
    DECLARE_BUILT_IN_MTX(LINK_EQUIP_MATRIX_SHIELD_HERO_BACK, &gHumanAdultHeroShieldBackMtx),
    DECLARE_BUILT_IN_MTX(LINK_EQUIP_MATRIX_SHIELD_MIRROR_BACK, &gHumanAdultMirrorShieldBackMtx),
    DECLARE_BUILT_IN_MTX(LINK_EQUIP_MATRIX_HOOKSHOT_CHAIN_AND_HOOK, &gHumanAdultHookshotHookAndChainMtx),
};

static DLIdToBuiltInDL sBuiltInDLAdultMM[] = {
    DECLARE_BUILT_IN_DL(LINK_DL_SWORD_KOKIRI_HILT, gHumanAdultKokiriSwordHiltDL),
    DECLARE_BUILT_IN_DL(LINK_DL_SWORD_KOKIRI_BLADE, gHumanAdultKokiriSwordBladeDL),
    DECLARE_BUILT_IN_DL(LINK_DL_SWORD_KOKIRI_SHEATH, gHumanAdultKokiriSwordSheathDL),
    DECLARE_BUILT_IN_DL(LINK_DL_SWORD_RAZOR_HILT, gHumanAdultRazorSwordHiltDL),
    DECLARE_BUILT_IN_DL(LINK_DL_SWORD_RAZOR_BLADE, gHumanAdultRazorSwordBladeDL),
    DECLARE_BUILT_IN_DL(LINK_DL_SWORD_RAZOR_SHEATH, gHumanAdultRazorSwordSheathDL),
    DECLARE_BUILT_IN_DL(LINK_DL_SWORD_GILDED_HILT, gHumanAdultGildedSwordHiltDL),
    DECLARE_BUILT_IN_DL(LINK_DL_SWORD_GILDED_BLADE, gHumanAdultGildedSwordBladeDL),
    DECLARE_BUILT_IN_DL(LINK_DL_SWORD_GILDED_SHEATH, gHumanAdultGildedSwordSheathDL),
    DECLARE_BUILT_IN_DL(LINK_DL_SWORD_FIERCE_DEITY_HILT, gEmptyDL),
    DECLARE_BUILT_IN_DL(LINK_DL_SWORD_FIERCE_DEITY_BLADE, gCallFierceDeitySwordDL),
    DECLARE_BUILT_IN_DL(LINK_DL_SWORD_GREAT_FAIRY_HILT, gEmptyDL),
    DECLARE_BUILT_IN_DL(LINK_DL_SWORD_GREAT_FAIRY_BLADE, gHumanAdultGreatFairySwordDL),
    DECLARE_BUILT_IN_DL(LINK_DL_SHIELD_HERO, gHumanAdultHeroShieldDL),
    DECLARE_BUILT_IN_DL(LINK_DL_SHIELD_MIRROR, gHumanAdultMirrorShieldDL),
    DECLARE_BUILT_IN_DL(LINK_DL_SHIELD_MIRROR_RAY, gHumanAdultMirrorShieldRayDL),
    DECLARE_BUILT_IN_DL(LINK_DL_SHIELD_MIRROR_RAY_BEAM, gHumanAdultMirrorShieldRayBeamDL),
    DECLARE_BUILT_IN_DL(LINK_DL_HOOKSHOT, gHumanAdultHookshotDL),
    DECLARE_BUILT_IN_DL(LINK_DL_FPS_HOOKSHOT, gHumanAdultHookshotFirstPersonDL),
    DECLARE_BUILT_IN_DL(LINK_DL_HOOKSHOT_HOOK, gHumanAdultHookshotHookDL),
    DECLARE_BUILT_IN_DL(LINK_DL_BOW, gHumanAdultBowDL),
    DECLARE_BUILT_IN_DL(LINK_DL_FPS_BOW, gHumanAdultBowFirstPersonDL),
    DECLARE_BUILT_IN_DL(LINK_DL_BOW_STRING, gHumanAdultBowStringDL),
    DECLARE_BUILT_IN_DL(LINK_DL_OCARINA_TIME, gHumanAdultOcarinaTimeDL),
};

void FallbackModelsCommon_addEquipmentAdultMM(PlayerModelManagerHandle h) {
    FallbackModelsCommon_addEquipmentCommonZ64(h);
    FallbackModelsCommon_addEquipmentCommonMM(h);

    addAllBuiltInDLs(h, sBuiltInDLAdultMM, ARRAY_COUNT(sBuiltInDLAdultMM));
    addAllBuiltInMtxs(h, sBuiltInMtxAdultMM, ARRAY_COUNT(sBuiltInMtxAdultMM));
}
