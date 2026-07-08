#include "modding.h"
#include "global.h"
#include "recomputils.h"
#include "recompconfig.h"
#include "recompdata.h"
#include "defines_mmo.h"
#include "defines_ooto.h"
#include "globalobjects_api.h"
#include "ml64compat.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"
#include "assets/objects/object_link_child/object_link_child.h"
#include "libc/string.h"
#include "playermodelmanager_api.h"
#include "oot_objects.h"
#include "../../misc_funcs.h"

static U32HashsetHandle sRepointedBuffers = 0;

#define IS_ALREADY_REPOINTED(buf) recomputil_u32_hashset_contains(sRepointedBuffers, (u32)buf)

void setupFaceTextures(PlayerModelManagerHandle h, u8 *zobj) {
    TexturePtr eyes[PLAYER_EYES_MAX];
    TexturePtr mouths[PLAYER_MOUTH_MAX];

    for (int i = 0; i < PLAYER_EYES_MAX; ++i) {
        eyes[i] = (TexturePtr)&zobj[Z64O_TEX_EYES_START + Z64O_TEX_EYES_SIZE * i];
    }

    for (int i = 0; i < PLAYER_MOUTH_MAX; ++i) {
        mouths[i] = (TexturePtr)&zobj[Z64O_TEX_MOUTH_START + Z64O_TEX_MOUTH_SIZE * i];
    }

    PlayerModelManager_setEyesTextures(h, eyes);
    PlayerModelManager_setMouthTextures(h, mouths);
}

typedef struct {
    PlayerLimb limb;
    u32 aliasTableIndex;
} LimbToAlias;

#define DECLARE_Z64O_LIMB_ALIAS(aliasName, modName, sheathDL)                                \
    static LimbToAlias aliasName[] = {                                                       \
        {.limb = PLAYER_LIMB_WAIST, .aliasTableIndex = modName##_LUT_DL_WAIST},              \
        {.limb = PLAYER_LIMB_RIGHT_THIGH, .aliasTableIndex = modName##_LUT_DL_RTHIGH},       \
        {.limb = PLAYER_LIMB_RIGHT_SHIN, .aliasTableIndex = modName##_LUT_DL_RSHIN},         \
        {.limb = PLAYER_LIMB_RIGHT_FOOT, .aliasTableIndex = modName##_LUT_DL_RFOOT},         \
        {.limb = PLAYER_LIMB_LEFT_THIGH, .aliasTableIndex = modName##_LUT_DL_LTHIGH},        \
        {.limb = PLAYER_LIMB_LEFT_SHIN, .aliasTableIndex = modName##_LUT_DL_LSHIN},          \
        {.limb = PLAYER_LIMB_LEFT_FOOT, .aliasTableIndex = modName##_LUT_DL_LFOOT},          \
        {.limb = PLAYER_LIMB_HEAD, .aliasTableIndex = modName##_LUT_DL_HEAD},                \
        {.limb = PLAYER_LIMB_HAT, .aliasTableIndex = modName##_LUT_DL_HAT},                  \
        {.limb = PLAYER_LIMB_COLLAR, .aliasTableIndex = modName##_LUT_DL_COLLAR},            \
        {.limb = PLAYER_LIMB_LEFT_SHOULDER, .aliasTableIndex = modName##_LUT_DL_LSHOULDER},  \
        {.limb = PLAYER_LIMB_LEFT_FOREARM, .aliasTableIndex = modName##_LUT_DL_LFOREARM},    \
        {.limb = PLAYER_LIMB_LEFT_HAND, .aliasTableIndex = modName##_LUT_DL_LHAND},          \
        {.limb = PLAYER_LIMB_RIGHT_SHOULDER, .aliasTableIndex = modName##_LUT_DL_RSHOULDER}, \
        {.limb = PLAYER_LIMB_RIGHT_FOREARM, .aliasTableIndex = modName##_LUT_DL_RFOREARM},   \
        {.limb = PLAYER_LIMB_RIGHT_HAND, .aliasTableIndex = modName##_LUT_DL_RHAND},         \
        {.limb = PLAYER_LIMB_SHEATH, .aliasTableIndex = sheathDL},                           \
        {.limb = PLAYER_LIMB_TORSO, .aliasTableIndex = modName##_LUT_DL_TORSO},              \
    };

DECLARE_Z64O_LIMB_ALIAS(sMMOLimbs, MMO, MMO_LUT_DL_SWORD_KOKIRI_SHEATH);
DECLARE_Z64O_LIMB_ALIAS(sOoTOChildLimbs, OOTO_CHILD, OOTO_CHILD_LUT_DL_SWORD_KOKIRI_SHEATH);
DECLARE_Z64O_LIMB_ALIAS(sOoTOAdultLimbs, OOTO_ADULT, OOTO_ADULT_LUT_DL_SWORD_MASTER_SHEATH);

static GlobalObjectsSegmentMap sML64SegmentMap = {NULL};

void handleZobjSkeleton(PlayerModelManagerHandle h, u8 *zobj, LimbToAlias limbsToAliases[]) {
    FlexSkeletonHeader *flexHeader = SEGMENTED_TO_GLOBAL_PTR(zobj, *(u32 *)(zobj + Z64O_SKELETON_HEADER_POINTER));

    if (!IS_ALREADY_REPOINTED(zobj)) {
        GlobalObjects_globalizeLodLimbSkeleton(zobj, flexHeader);

        for (size_t i = 0; i < ARRAY_COUNT(sMMOLimbs); ++i) {
            LimbToAlias *l2a = &limbsToAliases[i];

            int limbIdx = l2a->limb - 1;

            LodLimb *limb = flexHeader->sh.segment[limbIdx];

            if (limb->dLists[0]) {
                GlobalObjects_rebaseDL(limb->dLists[0], sML64SegmentMap);
            }
        }
    }

    PlayerModelManager_setSkeleton(h, flexHeader);
}

#define SET_MATRIX(dest, src) PlayerModelManager_setMatrix(h, dest, (Mtx *)&zobj[src])
#define SET_MODEL_DIRECT(dest, src)                              \
    {                                                            \
        Gfx *_g = (Gfx *)src;                                    \
        if (!IS_ALREADY_REPOINTED(zobj)) {                       \
            GlobalObjects_rebaseDL(_g, sML64SegmentMap);         \
        }                                                        \
        PlayerModelManager_setDisplayList(h, PMM_DL_##dest, _g); \
    }                                                            \
    (void)0
#define SET_MODEL(dest, src) SET_MODEL_DIRECT(dest, &zobj[src])
#define SET_Z64O_MODEL(dest, src, modName) SET_MODEL(dest, modName##_LUT_DL_##src)

#define SET_MMO_MODEL(dest, src) SET_Z64O_MODEL(dest, src, MMO)
#define QSET_MMO_MODEL(dlName) SET_MMO_MODEL(dlName, dlName)

void setupZobjMmoHuman(PlayerModelManagerHandle h, u8 *zobj) {

    handleZobjSkeleton(h, zobj, sMMOLimbs);

    setupFaceTextures(h, zobj);

    SET_MATRIX(PMM_MTX_SWORD_KOKIRI_BACK, MMO_MATRIX_SWORD_A);
    SET_MATRIX(PMM_MTX_SWORD_RAZOR_BACK, MMO_MATRIX_SWORD_B);
    SET_MATRIX(PMM_MTX_SWORD_GILDED_BACK, MMO_MATRIX_SWORD_A);
    SET_MATRIX(PMM_MTX_SHIELD_HERO_BACK, MMO_MATRIX_SHIELD_HERO);
    SET_MATRIX(PMM_MTX_SHIELD_MIRROR_BACK, MMO_MATRIX_SHIELD_MIRROR);

    QSET_MMO_MODEL(LFIST);
    QSET_MMO_MODEL(LHAND_BOTTLE);
    QSET_MMO_MODEL(RFIST);
    QSET_MMO_MODEL(SWORD_KOKIRI_SHEATH);
    QSET_MMO_MODEL(SWORD_RAZOR_SHEATH);
    QSET_MMO_MODEL(SWORD_GILDED_SHEATH);
    QSET_MMO_MODEL(SWORD_GILDED_HILT);
    QSET_MMO_MODEL(SWORD_GILDED_BLADE);
    QSET_MMO_MODEL(SWORD_GREAT_FAIRY_BLADE);
    QSET_MMO_MODEL(SHIELD_HERO);
    SET_MMO_MODEL(SHIELD_MIRROR, SHIELD_MIRROR_COMBINED);
    QSET_MMO_MODEL(OCARINA_TIME);
    QSET_MMO_MODEL(BOW);
    QSET_MMO_MODEL(BOW_STRING);
    QSET_MMO_MODEL(HOOKSHOT);
    QSET_MMO_MODEL(HOOKSHOT_HOOK);
    SET_MMO_MODEL(FPS_HOOKSHOT, HOOKSHOT);
    SET_MMO_MODEL(FPS_LFOREARM, LFOREARM);
    SET_MMO_MODEL(FPS_LHAND, LFIST);
    SET_MODEL_DIRECT(FPS_RFOREARM, gEmptyDL);
    QSET_MMO_MODEL(FPS_RHAND);
}

#define SET_OOTO_CHILD_MODEL(dest, src) SET_Z64O_MODEL(dest, src, OOTO_CHILD)
#define QSET_OOTO_CHILD_MODEL(dlName) SET_OOTO_CHILD_MODEL(dlName, dlName)

void setupZobjOotoChild(PlayerModelManagerHandle h, u8 *zobj) {
    // OotoFixHeaderSkelPtr MUST run before OotoFixChildLeftShoulder to ensure the latter reads the right offset for the skeleton in old zobjs
    // OotoFixChildLeftShoulder MUST run before repointZobjDls to ensure the left shoulder is repointed in old zobjs

    // old versions of manifest did not write header ptr
    OotoFixHeaderSkelPtr(zobj);
    // old versions of manifests had a typo that pointed the left shoulder entry to the left forearm
    OotoFixChildLeftShoulder(zobj);

    handleZobjSkeleton(h, zobj, sOoTOChildLimbs);

    setupFaceTextures(h, zobj);

    SET_MATRIX(PMM_MTX_SWORD_KOKIRI_BACK, OOTO_CHILD_MATRIX_SWORD_BACK);
    SET_MATRIX(PMM_MTX_SHIELD_HERO_BACK, OOTO_CHILD_MATRIX_SHIELD_BACK);

    QSET_OOTO_CHILD_MODEL(LFIST);
    QSET_OOTO_CHILD_MODEL(LHAND_BOTTLE);
    QSET_OOTO_CHILD_MODEL(RFIST);
    QSET_OOTO_CHILD_MODEL(DEKU_STICK);
    QSET_OOTO_CHILD_MODEL(BOTTLE_GLASS);
    SET_MODEL_DIRECT(BOTTLE_CONTENTS, gEmptyDL);
    QSET_OOTO_CHILD_MODEL(SWORD_KOKIRI_SHEATH);
    QSET_OOTO_CHILD_MODEL(SWORD_KOKIRI_HILT);
    QSET_OOTO_CHILD_MODEL(SWORD_KOKIRI_BLADE);
    SET_OOTO_CHILD_MODEL(SHIELD_HERO, SHIELD_DEKU);
    QSET_OOTO_CHILD_MODEL(OCARINA_TIME);
    SET_OOTO_CHILD_MODEL(FPS_LFOREARM, LFOREARM);
    SET_OOTO_CHILD_MODEL(FPS_LHAND, LFIST);
    SET_MODEL_DIRECT(FPS_RFOREARM, gEmptyDL);
    QSET_OOTO_CHILD_MODEL(FPS_RHAND);
}

static bool sIsOotHookshotMtxInitialized = false;
static Mtx sOotHookshotMtx;

#define SET_OOTO_ADULT_MODEL(dest, src) SET_Z64O_MODEL(dest, src, OOTO_ADULT)
#define QSET_OOTO_ADULT_MODEL(dlName) SET_OOTO_ADULT_MODEL(dlName, dlName)

extern Gfx *gOOTMirrorShieldRayBeamAdjustedDL;
extern Gfx *gOOTMirrorShieldRayDL;

void setupZobjOotoAdult(PlayerModelManagerHandle h, u8 *zobj) {
    if (!sIsOotHookshotMtxInitialized) {
        guPosition(&sOotHookshotMtx, 0, 0, 0, 1, 50, 840, 0);
        sIsOotHookshotMtxInitialized = true;
    }

    // OotoFixHeaderSkelPtr MUST run before repointZobjDls to ensure the latter reads the right offset for the skeleton in old zobjs

    // old versions of manifest did not write header ptr
    OotoFixHeaderSkelPtr(zobj);

    handleZobjSkeleton(h, zobj, sOoTOAdultLimbs);

    setupFaceTextures(h, zobj);

    SET_MATRIX(PMM_MTX_SWORD_KOKIRI_BACK, OOTO_ADULT_MATRIX_SWORD_BACK);
    SET_MATRIX(PMM_MTX_SWORD_RAZOR_BACK, OOTO_ADULT_MATRIX_SWORD_BACK);
    SET_MATRIX(PMM_MTX_SWORD_GILDED_BACK, OOTO_ADULT_MATRIX_SWORD_BACK);
    SET_MATRIX(PMM_MTX_SHIELD_HERO_BACK, OOTO_ADULT_MATRIX_SHIELD_BACK);
    SET_MATRIX(PMM_MTX_SHIELD_MIRROR_BACK, OOTO_ADULT_MATRIX_SHIELD_BACK);
    PlayerModelManager_setMatrix(h, PMM_MTX_HOOKSHOT_CHAIN_AND_HOOK, &sOotHookshotMtx);

    QSET_OOTO_ADULT_MODEL(LFIST);
    QSET_OOTO_ADULT_MODEL(LHAND_BOTTLE);
    QSET_OOTO_ADULT_MODEL(RFIST);
    SET_OOTO_ADULT_MODEL(SWORD_KOKIRI_SHEATH, SWORD_MASTER_SHEATH);
    SET_OOTO_ADULT_MODEL(SWORD_KOKIRI_HILT, SWORD_MASTER_HILT);
    SET_OOTO_ADULT_MODEL(SWORD_KOKIRI_BLADE, SWORD_MASTER_BLADE);
    SET_OOTO_ADULT_MODEL(SWORD_RAZOR_SHEATH, SWORD_MASTER_SHEATH);
    SET_OOTO_ADULT_MODEL(SWORD_RAZOR_HILT, SWORD_MASTER_HILT);
    SET_OOTO_ADULT_MODEL(SWORD_RAZOR_BLADE, SWORD_MASTER_BLADE);
    SET_OOTO_ADULT_MODEL(SWORD_GILDED_SHEATH, SWORD_MASTER_SHEATH);
    SET_OOTO_ADULT_MODEL(SWORD_GILDED_HILT, SWORD_MASTER_HILT);
    SET_OOTO_ADULT_MODEL(SWORD_GILDED_BLADE, SWORD_MASTER_BLADE);
    SET_OOTO_ADULT_MODEL(SWORD_GREAT_FAIRY_HILT, BIGSWORD_HILT);
    SET_OOTO_ADULT_MODEL(SWORD_GREAT_FAIRY_BLADE, BIGSWORD_BLADE);
    SET_OOTO_ADULT_MODEL(SWORD_FIERCE_DEITY_HILT, BIGSWORD_HILT);
    SET_OOTO_ADULT_MODEL(SWORD_FIERCE_DEITY_BLADE, BIGSWORD_BLADE);
    SET_OOTO_ADULT_MODEL(SHIELD_HERO, SHIELD_HYLIAN);
    QSET_OOTO_ADULT_MODEL(SHIELD_MIRROR);
    QSET_OOTO_ADULT_MODEL(OCARINA_TIME);
    QSET_OOTO_ADULT_MODEL(HOOKSHOT_HOOK);
    QSET_OOTO_ADULT_MODEL(HOOKSHOT_CHAIN);
    QSET_OOTO_ADULT_MODEL(HOOKSHOT_RETICLE);
    QSET_OOTO_ADULT_MODEL(HOOKSHOT);
    QSET_OOTO_ADULT_MODEL(BOW);
    QSET_OOTO_ADULT_MODEL(BOW_STRING);
    QSET_OOTO_ADULT_MODEL(BOTTLE_GLASS);
    SET_MODEL_DIRECT(BOTTLE_CONTENTS, gEmptyDL);
    QSET_OOTO_ADULT_MODEL(FPS_LFOREARM);
    QSET_OOTO_ADULT_MODEL(FPS_LHAND);
    QSET_OOTO_ADULT_MODEL(FPS_RFOREARM);
    QSET_OOTO_ADULT_MODEL(FPS_RHAND);
    QSET_OOTO_ADULT_MODEL(FPS_HOOKSHOT);
    PlayerModelManager_setDisplayList(h, PMM_DL_SHIELD_MIRROR_RAY, gOOTMirrorShieldRayDL);
    PlayerModelManager_setDisplayList(h, PMM_DL_SHIELD_MIRROR_RAY_BEAM, gOOTMirrorShieldRayBeamAdjustedDL);
}

void setupZobjZ64O(PlayerModelManagerHandle h, u8 *zobj) {
    if (!sRepointedBuffers) {
        sRepointedBuffers = recomputil_create_u32_hashset();
    }

    sML64SegmentMap[0x06] = zobj;
    sML64SegmentMap[0x04] = NULL;

    u8 formByte = zobj[Z64O_FORM_BYTE];

    switch (formByte) {
        case MMO_FORM_BYTE_CHILD:
        case MMO_FORM_BYTE_ADULT:
            sML64SegmentMap[0x04] = GlobalObjects_getGlobalObject(GAMEPLAY_KEEP);
            setupZobjMmoHuman(h, zobj);
            break;

        case OOTO_FORM_BYTE_CHILD:
            sML64SegmentMap[0x04] = getGameplayKeepOOT();
            setupZobjOotoChild(h, zobj);
            break;

        case OOTO_FORM_BYTE_ADULT:
            sML64SegmentMap[0x04] = getGameplayKeepOOT();
            setupZobjOotoAdult(h, zobj);
            break;

        default:
            break;
    }

    recomputil_u32_hashset_insert(sRepointedBuffers, (u32)zobj);

#if defined(ZELDA_ANDROID_BUILTIN_PMM)
    recomp_android_compat_pmm_apply_log(20, h, formByte, sML64SegmentMap[0x04] ? 1 : 0, "setupZobjZ64O");
#endif
}
