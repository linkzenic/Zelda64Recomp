#include "global.h"
#include "globalobjects_api.h"
#include "playermodelmanager_api.h"
#include "assets/objects/object_link_child/object_link_child.h"
#include "assets/objects/object_link_boy/object_link_boy.h"
#include "assets/objects/object_link_nuts/object_link_nuts.h"
#include "assets/objects/object_link_goron/object_link_goron.h"
#include "assets/objects/object_link_zora/object_link_zora.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"
#include "assets/objects/object_mir_ray/object_mir_ray.h"
#include "maskdls.h"
#include "apilocal.h"
#include "recomputils.h"
#include "equipmentbuiltin.h"
#include "mm/vanillawrapperdls.h"

extern Gfx gLinkHumanFirstPersonHookshotDL[];
extern Gfx gLinkHumanFirstPersonBowDL[];
extern Gfx gLinkHumanOcarinaDL[];

#define DECLARE_MATRIX_WRAPPED_DL(dlName, mtxName, dl)                  \
    Mtx mtxName;                                                        \
    Gfx dlName[] = {                                                    \
        gsSPMatrix(&mtxName, G_MTX_PUSH | G_MTX_MUL | G_MTX_MODELVIEW), \
        gsSPDisplayList(dl),                                            \
        gsSPPopMatrix(G_MTX_MODELVIEW),                                 \
        gsSPEndDisplayList(),                                           \
    }

DECLARE_MATRIX_WRAPPED_DL(gHumanAdultHeroShieldDL, gHumanAdultHeroShieldResizerMtx, gCallHumanHeroShieldDL);

DECLARE_MATRIX_WRAPPED_DL(gHumanAdultMirrorShieldDL, gHumanAdultMirrorShieldResizerMtx, gCallHumanMirrorShieldDL);

DECLARE_MATRIX_WRAPPED_DL(gHumanAdultMirrorShieldRayDL, gHumanAdultMirrorShieldRayResizerMtx, gCallHumanMirrorShieldRayDL);

DECLARE_MATRIX_WRAPPED_DL(gHumanAdultMirrorShieldRayBeamDL, gHumanAdultMirrorShieldRayBeamResizerMtx, gCallHumanMirrorShieldRayBeamDL);

DECLARE_MATRIX_WRAPPED_DL(gHumanAdultKokiriSwordHiltDL, gHumanAdultKokiriSwordHiltResizerMtx, gCallHumanKokiriSwordHiltDL);

DECLARE_MATRIX_WRAPPED_DL(gHumanAdultKokiriSwordBladeDL, gHumanAdultKokiriSwordBladeResizerMtx, gCallHumanKokiriSwordBladeDL);

DECLARE_MATRIX_WRAPPED_DL(gHumanAdultKokiriSwordSheathDL, gHumanAdultKokiriSwordSheathResizerMtx, gCallHumanKokiriSwordSheathDL);

DECLARE_MATRIX_WRAPPED_DL(gHumanAdultRazorSwordHiltDL, gHumanAdultRazorSwordHiltResizerMtx, gCallHumanRazorSwordHiltDL);

DECLARE_MATRIX_WRAPPED_DL(gHumanAdultRazorSwordBladeDL, gHumanAdultRazorSwordBladeResizerMtx, gCallHumanRazorSwordBladeDL);

DECLARE_MATRIX_WRAPPED_DL(gHumanAdultRazorSwordSheathDL, gHumanAdultRazorSwordSheathResizerMtx, gCallHumanRazorSwordSheathDL);

DECLARE_MATRIX_WRAPPED_DL(gHumanAdultGildedSwordHiltDL, gHumanAdultGildedSwordHiltResizerMtx, gCallHumanGildedSwordHiltDL);

DECLARE_MATRIX_WRAPPED_DL(gHumanAdultGildedSwordBladeDL, gHumanAdultGildedSwordBladeResizerMtx, gCallHumanGildedSwordBladeDL);

DECLARE_MATRIX_WRAPPED_DL(gHumanAdultGildedSwordSheathDL, gHumanAdultGildedSwordSheathResizerMtx, gCallHumanGildedSwordSheathDL);

DECLARE_MATRIX_WRAPPED_DL(gHumanAdultGreatFairySwordDL, gHumanAdultGreatFairySwordResizerMtx, gCallHumanGreatFairySwordDL);

DECLARE_MATRIX_WRAPPED_DL(gFierceDeityChildSwordDL, gFierceDeityChildSwordResizerMtx, gCallFierceDeitySwordDL);

DECLARE_MATRIX_WRAPPED_DL(gHumanAdultHookshotDL, gHumanAdultHookshotResizerMtx, gCallHumanHookshotDL);

DECLARE_MATRIX_WRAPPED_DL(gHumanAdultHookshotFirstPersonDL, gHumanAdultHookshotFirstPersonResizerMtx, gLinkHumanFirstPersonHookshotDL);

DECLARE_MATRIX_WRAPPED_DL(gHumanAdultHookshotHookDL, gHumanAdultHookshotHookResizerMtx, gCallHumanHookshotHookDL);

DECLARE_MATRIX_WRAPPED_DL(gHumanAdultHookshotChainDL, gHumanAdultHookshotChainResizerMtx, gCallHumanHookshotChainDL);

DECLARE_MATRIX_WRAPPED_DL(gHumanAdultBowDL, gHumanAdultBowResizerMtx, gCallHumanBowDL);

DECLARE_MATRIX_WRAPPED_DL(gHumanAdultBowFirstPersonDL, gHumanAdultBowFirstPersonResizerMtx, gLinkHumanFirstPersonBowDL);

DECLARE_MATRIX_WRAPPED_DL(gHumanAdultBowStringDL, gHumanAdultBowStringResizerMtx, gCallHumanBowStringDL);

DECLARE_MATRIX_WRAPPED_DL(gHumanAdultOcarinaTimeDL, gHumanAdultOcarinaTimeResizerMtx, gLinkHumanOcarinaDL);

#undef DECLARE_STATIC_MATRIX_WRAPPED_DL

Mtx gHumanAdultHookshotHookAndChainMtx;

Mtx gChildDekuShieldBackMtx;

Mtx gHumanHeroShieldBackMtx;
Mtx gHumanAdultHeroShieldBackMtx;

Mtx gHumanMirrorShieldBackMtx;
Mtx gHumanAdultMirrorShieldBackMtx;

Mtx gHumanKokiriSwordHiltBackMtx;
Mtx gHumanAdultKokiriSwordHiltBackMtx;

Mtx gHumanRazorSwordHiltBackMtx;
Mtx gHumanAdultRazorSwordHiltBackMtx;

Mtx gHumanGildedSwordHiltBackMtx;
Mtx gHumanAdultGildedSwordHiltBackMtx;

static void *getMMObject(ObjectId id) {
    return GlobalObjects_getGlobalObject(id);
}

static void setupResizedSwordMatrixes(Mtx *hiltResizeMtx, Mtx *bladeResizeMtx, Mtx *sheathResizeMtx, Mtx *hiltBackMtx, f32 scale, Vec3f *translation, Vec3f *hiltBackBaseTranslation) {
    guPosition(hiltResizeMtx, 0.f, 0.f, 0.f, scale, translation->x, translation->y, translation->z);
    guPosition(bladeResizeMtx, 0.f, 0.f, 0.f, scale, translation->x, translation->y, translation->z);
    guPosition(sheathResizeMtx, 0.f, 0.f, 0.f, scale, 0.f, 0.f, 0.f);
    guPosition(hiltBackMtx, 0.f, 0.f, 0.f, 1.f,
               hiltBackBaseTranslation->x * scale - translation->x,
               hiltBackBaseTranslation->y * scale - translation->y,
               hiltBackBaseTranslation->z * scale - translation->z);
}

static Mtx *getGlobalObjectMtx(void *globalObject, Mtx *segmentedPtr) {
    return SEGMENTED_TO_GLOBAL_PTR(globalObject, segmentedPtr);
}

static void initCustomDLs(void) {
    static bool isCustomDLsInit;

    if (isCustomDLsInit) {
        return;
    }

    isCustomDLsInit = true;

    const f32 ADULT_SWORD_SCALE_FACTOR = 1.3f;

    void *human = getMMObject(OBJECT_LINK_CHILD);

    guPosition(&gChildDekuShieldBackMtx, 0.f, 0.f, 180.f, 1.f, 545.f, 0.f, 80.f);

    gHumanHeroShieldBackMtx = *getGlobalObjectMtx(human, &gLinkHumanHerosShieldMtx);
    guPosition(&gHumanAdultHeroShieldBackMtx, 0.f, 0.f, 180.f, 1.f, 935.f, 94.f, 29.f);

    gHumanMirrorShieldBackMtx = *getGlobalObjectMtx(human, &gLinkHumanMirrorShieldMtx);
    gHumanAdultMirrorShieldBackMtx = gHumanAdultHeroShieldBackMtx;

    guPosition(&gHumanAdultHeroShieldResizerMtx, 0.f, 0.f, 0.f, 1.3f, 0.f, 0.f, 0.f);

    guPosition(&gHumanAdultMirrorShieldResizerMtx, 0.f, 0.f, 0.f, 1.3f, 0.f, 0.f, 0.f);

    guPosition(&gHumanAdultMirrorShieldRayResizerMtx, 0.f, 0.f, 0.f, 1.3f, 0.f, 0.f, 0.f);

    guPosition(&gHumanAdultMirrorShieldRayBeamResizerMtx, 0.f, 0.f, 0.f, 1.3f, 0.f, 0.f, 0.f);

    gHumanKokiriSwordHiltBackMtx = *getGlobalObjectMtx(human, &gLinkHumanSheathedKokiriSwordMtx);
    static Vec3f kokiriSwordTranslation = {75.f, 25.f, -115.f};
    static Vec3f kokiriSwordBackBaseTranslation = {-578.f, -221.f, -32.f};
    setupResizedSwordMatrixes(&gHumanAdultKokiriSwordHiltResizerMtx, &gHumanAdultKokiriSwordBladeResizerMtx, &gHumanAdultKokiriSwordSheathResizerMtx,
                              &gHumanAdultKokiriSwordHiltBackMtx, ADULT_SWORD_SCALE_FACTOR,
                              &kokiriSwordTranslation, &kokiriSwordBackBaseTranslation);

    gHumanRazorSwordHiltBackMtx = *getGlobalObjectMtx(human, &gLinkHumanSheathedRazorSwordMtx);
    static Vec3f razorSwordTranslation = {75.f, 25.f, -115.f};
    static Vec3f razorSwordBackBaseTranslation = {-480.f, -240.f, -14.f};
    setupResizedSwordMatrixes(&gHumanAdultRazorSwordHiltResizerMtx, &gHumanAdultRazorSwordBladeResizerMtx, &gHumanAdultRazorSwordSheathResizerMtx,
                              &gHumanAdultRazorSwordHiltBackMtx, ADULT_SWORD_SCALE_FACTOR, &razorSwordTranslation,
                              &razorSwordBackBaseTranslation);

    gHumanGildedSwordHiltBackMtx = *getGlobalObjectMtx(human, &gLinkHumanSheathedKokiriSwordMtx);
    static Vec3f gildedSwordTranslation = {-25.f, 25.f, -100.f};
    static Vec3f gildedSwordBackBaseTranslation = {-578.f, -221.f, -32.f};
    setupResizedSwordMatrixes(&gHumanAdultGildedSwordHiltResizerMtx, &gHumanAdultGildedSwordBladeResizerMtx, &gHumanAdultGildedSwordSheathResizerMtx,
                              &gHumanAdultGildedSwordHiltBackMtx, ADULT_SWORD_SCALE_FACTOR, &gildedSwordTranslation,
                              &gildedSwordBackBaseTranslation);

    guPosition(&gHumanAdultGreatFairySwordResizerMtx, 0.f, 0.f, 0.f, 1.1f, -30.f, 125.f, -100.f);

    guPosition(&gFierceDeityChildSwordResizerMtx, 0.f, 0.f, 0.f, 0.9f, 0.f, -200.f, 25.f);

    guPosition(&gHumanAdultHookshotResizerMtx, 0.f, 0.f, 0.f, 1.4f, 50.f, 105.f, 0.f);
    gHumanAdultHookshotFirstPersonResizerMtx = gHumanAdultHookshotResizerMtx;

    guPosition(&gHumanAdultHookshotChainResizerMtx, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f);

    guPosition(&gHumanAdultHookshotHookResizerMtx, 0.f, 0.f, 0.f, 1.4f, 0.f, 0.f, 0.f);

    guPosition(&gHumanAdultHookshotHookAndChainMtx, 0.f, 0.f, 0.f, 1.f, 50.f, 420.f, 0.f);

    const f32 BOW_SCALE_FACTOR = 1.3f;
    const f32 BOW_Y_OFFSET = 40.f;
    guPosition(&gHumanAdultBowResizerMtx, 0.f, 0.f, 0.f, BOW_SCALE_FACTOR, 0.f, BOW_Y_OFFSET, 0.f);
    guPosition(&gHumanAdultBowFirstPersonResizerMtx, 0.f, 0.f, 0.f, BOW_SCALE_FACTOR, 0.f, BOW_Y_OFFSET, 0.f);
    guScale(&gHumanAdultBowStringResizerMtx, BOW_SCALE_FACTOR, BOW_SCALE_FACTOR, 0.7f);

    guPosition(&gHumanAdultOcarinaTimeResizerMtx, 7.f, -8.f, -4.f, 1.f, 72.f, 238.f, -3.f);
}

static void registerMaskDL(PlayerModelManagerHandle pack, const char *internalName, const char *displayName, PlayerModelManagerModelType modelType, Link_DisplayList dlId, Gfx *dl) {
    PlayerModelManagerHandle h = PlayerModelManager_registerModel(PMM_API_VERSION, internalName, modelType);
    PlayerModelManager_setDisplayName(h, displayName);
    PlayerModelManager_setAuthor(h, "Nintendo");
    PlayerModelManager_setDisplayList(h, dlId, dl);
    PlayerModelManager_addHandleToPack(pack, h);
}

static void registerMaskDL2(PlayerModelManagerHandle pack, const char *internalName, const char *displayName, PlayerModelManagerModelType modelType, Link_DisplayList dlId1, Gfx *dl1, Link_DisplayList dlId2, Gfx *dl2) {
    PlayerModelManagerHandle h = PlayerModelManager_registerModel(PMM_API_VERSION, internalName, modelType);
    PlayerModelManager_setDisplayName(h, displayName);
    PlayerModelManager_setAuthor(h, "Nintendo");
    PlayerModelManager_setDisplayList(h, dlId1, dl1);
    PlayerModelManager_setDisplayList(h, dlId2, dl2);
    PlayerModelManager_addHandleToPack(pack, h);
}

static void setDLForModelTypes(PlayerModelManagerHandle h, Link_DisplayList id, Gfx *dl, PlayerModelManagerModelType types[], size_t n) {
    for (size_t i = 0; i < n; ++i) {
        PlayerModelManager_setDisplayListForModelType(h, types[i], id, dl);
    }
}

static void setMtxForModelTypes(PlayerModelManagerHandle h, Link_EquipmentMatrix id, Mtx *mtx, PlayerModelManagerModelType types[], size_t n) {
    for (size_t i = 0; i < n; ++i) {
        PlayerModelManager_setMatrixForModelType(h, types[i], id, mtx);
    }
}

static PlayerModelManagerModelType sModelTypesAdult[] = {PMM_MODEL_TYPE_ADULT, PMM_MODEL_TYPE_ZORA, PMM_MODEL_TYPE_GORON, PMM_MODEL_TYPE_FIERCE_DEITY};

static void setAdultModelTypeDL(PlayerModelManagerHandle h, Link_DisplayList id, Gfx *dl) {
    setDLForModelTypes(h, id, dl, sModelTypesAdult, ARRAY_COUNT(sModelTypesAdult));
}

static void setAdultModelTypeMtx(PlayerModelManagerHandle h, Link_EquipmentMatrix id, Mtx *mtx) {
    setMtxForModelTypes(h, id, mtx, sModelTypesAdult, ARRAY_COUNT(sModelTypesAdult));
}

static PlayerModelManagerModelType sModelTypesChild[] = {PMM_MODEL_TYPE_CHILD, PMM_MODEL_TYPE_DEKU};

static void setChildModelTypeDL(PlayerModelManagerHandle h, Link_DisplayList id, Gfx *dl) {
    setDLForModelTypes(h, id, dl, sModelTypesChild, ARRAY_COUNT(sModelTypesChild));
}

static void setChildModelTypeMtx(PlayerModelManagerHandle h, Link_EquipmentMatrix id, Mtx *mtx) {
    setMtxForModelTypes(h, id, mtx, sModelTypesChild, ARRAY_COUNT(sModelTypesChild));
}

RECOMP_CALLBACK(".", onRegisterModels) void registerMMEquipment(void) {
    initCustomDLs();

    PlayerModelManagerHandle vanillaPack = PlayerModelManager_registerModel(PMM_API_VERSION, "__mm_vanilla_equipment__", PMM_MODEL_TYPE_MODEL_PACK);
    PlayerModelManager_setAuthor(vanillaPack, "Nintendo");
    PlayerModelManager_setDisplayName(vanillaPack, "MM Equipment");

    // Hero's Shield
    {
        PlayerModelManagerHandle h = PlayerModelManager_registerModel(PMM_API_VERSION, "__mm_hero_shield2__", PMM_MODEL_TYPE_SHIELD_HERO);
        PlayerModelManager_setDisplayName(h, "Hero's Shield");
        PlayerModelManager_setAuthor(h, "Nintendo");

        PlayerModelManager_setMatrix(h, LINK_EQUIP_MATRIX_SHIELD_HERO_BACK, &gHumanHeroShieldBackMtx);
        PlayerModelManager_setDisplayList(h, LINK_DL_SHIELD_HERO, gCallHumanHeroShieldDL);

        setAdultModelTypeDL(h, LINK_DL_SHIELD_HERO, gHumanAdultHeroShieldDL);
        setAdultModelTypeMtx(h, LINK_EQUIP_MATRIX_SHIELD_HERO_BACK, &gHumanAdultHeroShieldBackMtx);

        PlayerModelManager_addHandleToPack(vanillaPack, h);
    }

    // Mirror Shield (MM)
    {
        PlayerModelManagerHandle h = PlayerModelManager_registerModel(PMM_API_VERSION, "__mm_mirror_shield3__", PMM_MODEL_TYPE_SHIELD_MIRROR);
        PlayerModelManager_setDisplayName(h, "Mirror Shield (MM)");
        PlayerModelManager_setAuthor(h, "Nintendo");

        PlayerModelManager_setMatrix(h, LINK_EQUIP_MATRIX_SHIELD_MIRROR_BACK, &gHumanMirrorShieldBackMtx);
        PlayerModelManager_setDisplayList(h, LINK_DL_SHIELD_MIRROR, gCallHumanMirrorShieldDL);
        PlayerModelManager_setDisplayList(h, LINK_DL_SHIELD_MIRROR_RAY, gCallHumanMirrorShieldRayDL);
        PlayerModelManager_setDisplayList(h, LINK_DL_SHIELD_MIRROR_RAY_BEAM, gCallHumanMirrorShieldRayBeamDL);

        setAdultModelTypeMtx(h, LINK_EQUIP_MATRIX_SHIELD_MIRROR_BACK, &gHumanAdultMirrorShieldBackMtx);
        setAdultModelTypeDL(h, LINK_DL_SHIELD_MIRROR, gHumanAdultMirrorShieldDL);
        setAdultModelTypeDL(h, LINK_DL_SHIELD_MIRROR_RAY, gHumanAdultMirrorShieldRayDL);
        setAdultModelTypeDL(h, LINK_DL_SHIELD_MIRROR_RAY_BEAM, gHumanAdultMirrorShieldRayBeamDL);

        PlayerModelManager_addHandleToPack(vanillaPack, h);
    }

    // Kokiri Sword (MM)
    {
        PlayerModelManagerHandle h = PlayerModelManager_registerModel(PMM_API_VERSION, "__mm_kokiri_sword1__", PMM_MODEL_TYPE_SWORD_KOKIRI);
        PlayerModelManager_setDisplayName(h, "Kokiri Sword (MM)");
        PlayerModelManager_setAuthor(h, "Nintendo");

        PlayerModelManager_setMatrix(h, LINK_EQUIP_MATRIX_SWORD_KOKIRI_BACK, &gHumanKokiriSwordHiltBackMtx);
        PlayerModelManager_setDisplayList(h, LINK_DL_SWORD_KOKIRI_HILT, gCallHumanKokiriSwordHiltDL);
        PlayerModelManager_setDisplayList(h, LINK_DL_SWORD_KOKIRI_BLADE, gCallHumanKokiriSwordBladeDL);
        PlayerModelManager_setDisplayList(h, LINK_DL_SWORD_KOKIRI_SHEATH, gCallHumanKokiriSwordSheathDL);

        setAdultModelTypeMtx(h, LINK_EQUIP_MATRIX_SWORD_KOKIRI_BACK, &gHumanAdultKokiriSwordHiltBackMtx);
        setAdultModelTypeDL(h, LINK_DL_SWORD_KOKIRI_HILT, gHumanAdultKokiriSwordHiltDL);
        setAdultModelTypeDL(h, LINK_DL_SWORD_KOKIRI_BLADE, gHumanAdultKokiriSwordBladeDL);
        setAdultModelTypeDL(h, LINK_DL_SWORD_KOKIRI_SHEATH, gHumanAdultKokiriSwordSheathDL);

        PlayerModelManager_addHandleToPack(vanillaPack, h);
    }

    // Razor Sword (MM)
    {
        PlayerModelManagerHandle h = PlayerModelManager_registerModel(PMM_API_VERSION, "__mm_razor_sword2__", PMM_MODEL_TYPE_SWORD_RAZOR);
        PlayerModelManager_setDisplayName(h, "Razor Sword");
        PlayerModelManager_setAuthor(h, "Nintendo");

        PlayerModelManager_setMatrix(h, LINK_EQUIP_MATRIX_SWORD_RAZOR_BACK, &gHumanRazorSwordHiltBackMtx);
        PlayerModelManager_setDisplayList(h, LINK_DL_SWORD_RAZOR_HILT, gCallHumanRazorSwordHiltDL);
        PlayerModelManager_setDisplayList(h, LINK_DL_SWORD_RAZOR_BLADE, gCallHumanRazorSwordBladeDL);
        PlayerModelManager_setDisplayList(h, LINK_DL_SWORD_RAZOR_SHEATH, gCallHumanRazorSwordSheathDL);

        setAdultModelTypeMtx(h, LINK_EQUIP_MATRIX_SWORD_RAZOR_BACK, &gHumanAdultRazorSwordHiltBackMtx);
        setAdultModelTypeDL(h, LINK_DL_SWORD_RAZOR_HILT, gHumanAdultRazorSwordHiltDL);
        setAdultModelTypeDL(h, LINK_DL_SWORD_RAZOR_BLADE, gHumanAdultRazorSwordBladeDL);
        setAdultModelTypeDL(h, LINK_DL_SWORD_RAZOR_SHEATH, gHumanAdultRazorSwordSheathDL);

        PlayerModelManager_addHandleToPack(vanillaPack, h);
    }

    // Gilded Sword (MM)
    {
        PlayerModelManagerHandle h = PlayerModelManager_registerModel(PMM_API_VERSION, "__mm_gilded_sword3__", PMM_MODEL_TYPE_SWORD_GILDED);
        PlayerModelManager_setDisplayName(h, "Gilded Sword");
        PlayerModelManager_setAuthor(h, "Nintendo");

        PlayerModelManager_setMatrix(h, LINK_EQUIP_MATRIX_SWORD_GILDED_BACK, &gHumanGildedSwordHiltBackMtx);
        PlayerModelManager_setDisplayList(h, LINK_DL_SWORD_GILDED_HILT, gCallHumanGildedSwordHiltDL);
        PlayerModelManager_setDisplayList(h, LINK_DL_SWORD_GILDED_BLADE, gCallHumanGildedSwordBladeDL);
        PlayerModelManager_setDisplayList(h, LINK_DL_SWORD_GILDED_SHEATH, gCallHumanGildedSwordSheathDL);

        setAdultModelTypeMtx(h, LINK_EQUIP_MATRIX_SWORD_GILDED_BACK, &gHumanAdultGildedSwordHiltBackMtx);
        setAdultModelTypeDL(h, LINK_DL_SWORD_GILDED_HILT, gHumanAdultGildedSwordHiltDL);
        setAdultModelTypeDL(h, LINK_DL_SWORD_GILDED_BLADE, gHumanAdultGildedSwordBladeDL);
        setAdultModelTypeDL(h, LINK_DL_SWORD_GILDED_SHEATH, gHumanAdultGildedSwordSheathDL);

        PlayerModelManager_addHandleToPack(vanillaPack, h);
    }

    // Fierce Deity Sword
    {
        PlayerModelManagerHandle h = PlayerModelManager_registerModel(PMM_API_VERSION, "__mm_fierce_deity_sword4__", PMM_MODEL_TYPE_SWORD_FIERCE_DIETY);
        PlayerModelManager_setDisplayName(h, "Fierce Deity's Sword");
        PlayerModelManager_setAuthor(h, "Nintendo");

        PlayerModelManager_setDisplayList(h, LINK_DL_SWORD_FIERCE_DEITY_BLADE, gCallFierceDeitySwordDL);
        PlayerModelManager_setDisplayList(h, LINK_DL_SWORD_FIERCE_DEITY_HILT, gEmptyDL);

        setChildModelTypeDL(h, LINK_DL_SWORD_FIERCE_DEITY_BLADE, gFierceDeityChildSwordDL);

        PlayerModelManager_addHandleToPack(vanillaPack, h);
    }

    {
        PlayerModelManagerHandle h = PlayerModelManager_registerModel(PMM_API_VERSION, "__mm_fierce_deity_sword5__", PMM_MODEL_TYPE_SWORD_GREAT_FAIRY);
        PlayerModelManager_setDisplayName(h, "Fierce Deity's Sword");
        PlayerModelManager_setAuthor(h, "Nintendo");

        PlayerModelManager_setDisplayList(h, LINK_DL_SWORD_GREAT_FAIRY_BLADE, gCallFierceDeitySwordDL);
        PlayerModelManager_setDisplayList(h, LINK_DL_SWORD_GREAT_FAIRY_HILT, gEmptyDL);

        setChildModelTypeDL(h, LINK_DL_SWORD_GREAT_FAIRY_BLADE, gFierceDeityChildSwordDL);
    }

    // Great Fairy's Sword
    {
        PlayerModelManagerHandle h = PlayerModelManager_registerModel(PMM_API_VERSION, "__mm_great_fairy_sword5__", PMM_MODEL_TYPE_SWORD_GREAT_FAIRY);
        PlayerModelManager_setDisplayName(h, "Great Fairy's Sword");
        PlayerModelManager_setAuthor(h, "Nintendo");

        PlayerModelManager_setDisplayList(h, LINK_DL_SWORD_GREAT_FAIRY_BLADE, gCallHumanGreatFairySwordDL);
        PlayerModelManager_setDisplayList(h, LINK_DL_SWORD_GREAT_FAIRY_HILT, gEmptyDL);

        setAdultModelTypeDL(h, LINK_DL_SWORD_GREAT_FAIRY_BLADE, gHumanAdultGreatFairySwordDL);

        PlayerModelManager_addHandleToPack(vanillaPack, h);
    }

    {
        PlayerModelManagerHandle h = PlayerModelManager_registerModel(PMM_API_VERSION, "__mm_great_fairy_sword4__", PMM_MODEL_TYPE_SWORD_FIERCE_DIETY);
        PlayerModelManager_setDisplayName(h, "Great Fairy's Sword");
        PlayerModelManager_setAuthor(h, "Nintendo");

        PlayerModelManager_setDisplayList(h, LINK_DL_SWORD_FIERCE_DEITY_BLADE, gCallHumanGreatFairySwordDL);
        PlayerModelManager_setDisplayList(h, LINK_DL_SWORD_FIERCE_DEITY_HILT, gEmptyDL);

        setAdultModelTypeDL(h, LINK_DL_SWORD_FIERCE_DEITY_BLADE, gHumanAdultGreatFairySwordDL);
    }

    // Hookshot (MM)
    {
        PlayerModelManagerHandle h = PlayerModelManager_registerModel(PMM_API_VERSION, "__mm_hookshot__", PMM_MODEL_TYPE_HOOKSHOT);
        PlayerModelManager_setDisplayName(h, "Hookshot (MM)");
        PlayerModelManager_setAuthor(h, "Nintendo");

        PlayerModelManager_setMatrix(h, LINK_EQUIP_MATRIX_HOOKSHOT_CHAIN_AND_HOOK, &gIdentityMtx);
        PlayerModelManager_setDisplayList(h, LINK_DL_HOOKSHOT, gCallHumanHookshotDL);
        PlayerModelManager_setDisplayList(h, LINK_DL_FPS_HOOKSHOT, gLinkHumanFirstPersonHookshotDL);
        PlayerModelManager_setDisplayList(h, LINK_DL_HOOKSHOT_CHAIN, gCallHumanHookshotChainDL);
        PlayerModelManager_setDisplayList(h, LINK_DL_HOOKSHOT_HOOK, gCallHumanHookshotHookDL);
        PlayerModelManager_setDisplayList(h, LINK_DL_HOOKSHOT_RETICLE, gCallHumanHookshotReticleDL);

        setAdultModelTypeMtx(h, LINK_EQUIP_MATRIX_HOOKSHOT_CHAIN_AND_HOOK, &gHumanAdultHookshotHookAndChainMtx);
        setAdultModelTypeDL(h, LINK_DL_HOOKSHOT, gHumanAdultHookshotDL);
        setAdultModelTypeDL(h, LINK_DL_HOOKSHOT_HOOK, gHumanAdultHookshotHookDL);
        setAdultModelTypeDL(h, LINK_DL_HOOKSHOT_CHAIN, gHumanAdultHookshotChainDL);
        setAdultModelTypeDL(h, LINK_DL_FPS_HOOKSHOT, gHumanAdultHookshotFirstPersonDL);

        PlayerModelManager_addHandleToPack(vanillaPack, h);
    }

    // Bow (MM)
    {
        PlayerModelManagerHandle h = PlayerModelManager_registerModel(PMM_API_VERSION, "__mm_bow__", PMM_MODEL_TYPE_BOW);
        PlayerModelManager_setDisplayName(h, "Bow (MM)");
        PlayerModelManager_setAuthor(h, "Nintendo");

        PlayerModelManager_setDisplayList(h, LINK_DL_BOW, gCallHumanBowDL);
        PlayerModelManager_setDisplayList(h, LINK_DL_FPS_BOW, gLinkHumanFirstPersonBowDL);
        PlayerModelManager_setDisplayList(h, LINK_DL_BOW_STRING, gCallHumanBowStringDL);
        PlayerModelManager_setDisplayList(h, LINK_DL_BOW_ARROW, gCallHumanArrowDL);

        setAdultModelTypeDL(h, LINK_DL_BOW, gHumanAdultBowDL);
        setAdultModelTypeDL(h, LINK_DL_FPS_BOW, gHumanAdultBowFirstPersonDL);
        setAdultModelTypeDL(h, LINK_DL_BOW_STRING, gHumanAdultBowStringDL);

        PlayerModelManager_addHandleToPack(vanillaPack, h);
    }

    // Bottle (MM)
    {
        PlayerModelManagerHandle h = PlayerModelManager_registerModel(PMM_API_VERSION, "__mm_bottle__", PMM_MODEL_TYPE_BOTTLE);
        PlayerModelManager_setDisplayName(h, "Bottle (MM)");
        PlayerModelManager_setAuthor(h, "Nintendo");

        PlayerModelManager_setDisplayList(h, LINK_DL_BOTTLE_GLASS, gCallHumanBottleGlassDL);
        PlayerModelManager_setDisplayList(h, LINK_DL_BOTTLE_CONTENTS, gCallHumanBottleContentsDL);

        PlayerModelManager_addHandleToPack(vanillaPack, h);
    }

    // Ocarina of Time
    {
        PlayerModelManagerHandle h = PlayerModelManager_registerModel(PMM_API_VERSION, "__mm_ocarina2_time__", PMM_MODEL_TYPE_OCARINA_TIME);
        PlayerModelManager_setDisplayName(h, "Ocarina of Time");
        PlayerModelManager_setAuthor(h, "Nintendo");
        PlayerModelManager_setDisplayList(h, LINK_DL_OCARINA_TIME, gLinkHumanOcarinaDL);

        setAdultModelTypeDL(h, LINK_DL_OCARINA_TIME, gHumanAdultOcarinaTimeDL);

        PlayerModelManager_addHandleToPack(vanillaPack, h);
    }

    // Deku Stick (MM)
    {
        PlayerModelManagerHandle h = PlayerModelManager_registerModel(PMM_API_VERSION, "__mm_deku_stick__", PMM_MODEL_TYPE_DEKU_STICK);
        PlayerModelManager_setDisplayName(h, "Deku Stick (MM)");
        PlayerModelManager_setAuthor(h, "Nintendo");

        PlayerModelManager_setDisplayList(h, LINK_DL_DEKU_STICK, gCallHumanDekuStickDL);

        PlayerModelManager_addHandleToPack(vanillaPack, h);
    }

    // Deku Pipes
    {
        PlayerModelManagerHandle h = PlayerModelManager_registerModel(PMM_API_VERSION, "__mm_pipes__", PMM_MODEL_TYPE_PIPES);
        PlayerModelManager_setDisplayName(h, "Deku Pipes");
        PlayerModelManager_setAuthor(h, "Nintendo");

        PlayerModelManager_setDisplayList(h, LINK_DL_PIPE_MOUTH, gCallDekuPipeMouthDL);
        PlayerModelManager_setDisplayList(h, LINK_DL_PIPE_UP, gCallDekuPipeUpDL);
        PlayerModelManager_setDisplayList(h, LINK_DL_PIPE_DOWN, gCallDekuPipeDownDL);
        PlayerModelManager_setDisplayList(h, LINK_DL_PIPE_LEFT, gCallDekuPipeLeftDL);
        PlayerModelManager_setDisplayList(h, LINK_DL_PIPE_RIGHT, gCallDekuPipeRightDL);
        PlayerModelManager_setDisplayList(h, LINK_DL_PIPE_A, gCallDekuPipeADL);

        PlayerModelManager_addHandleToPack(vanillaPack, h);
    }

    // Goron Drums
    {
        PlayerModelManagerHandle h = PlayerModelManager_registerModel(PMM_API_VERSION, "__mm_drums__", PMM_MODEL_TYPE_DRUMS);
        PlayerModelManager_setDisplayName(h, "Goron Drums");
        PlayerModelManager_setAuthor(h, "Nintendo");

        PlayerModelManager_setDisplayList(h, LINK_DL_DRUM_STRAP, gCallGoronDrumStrapDL);
        PlayerModelManager_setDisplayList(h, LINK_DL_DRUM_UP, gCallGoronDrumUpDL);
        PlayerModelManager_setDisplayList(h, LINK_DL_DRUM_DOWN, gCallGoronDrumDownDL);
        PlayerModelManager_setDisplayList(h, LINK_DL_DRUM_LEFT, gCallGoronDrumLeftDL);
        PlayerModelManager_setDisplayList(h, LINK_DL_DRUM_RIGHT, gCallGoronDrumRightDL);
        PlayerModelManager_setDisplayList(h, LINK_DL_DRUM_A, gCallGoronDrumADL);

        PlayerModelManager_addHandleToPack(vanillaPack, h);
    }

    // Zora Guitar
    {
        PlayerModelManagerHandle h = PlayerModelManager_registerModel(PMM_API_VERSION, "__mm_guitar__", PMM_MODEL_TYPE_GUITAR);
        PlayerModelManager_setDisplayName(h, "Zora Guitar");
        PlayerModelManager_setAuthor(h, "Nintendo");

        PlayerModelManager_setDisplayList(h, LINK_DL_GUITAR, gCallZoraGuitarDL);

        PlayerModelManager_addHandleToPack(vanillaPack, h);
    }

    // Masks
    registerMaskDL(vanillaPack, "__mm_mask_truth__", "Mask of Truth", PMM_MODEL_TYPE_MASK_TRUTH, LINK_DL_MASK_TRUTH, gCallHumanMaskTruthDL);
    registerMaskDL(vanillaPack, "__mm_mask_kafei__", "Kafei's Mask", PMM_MODEL_TYPE_MASK_KAFEIS_MASK, LINK_DL_MASK_KAFEIS_MASK, gCallHumanMaskKafeiDL);
    registerMaskDL(vanillaPack, "__mm_mask_all_night__", "All Night Mask", PMM_MODEL_TYPE_MASK_ALL_NIGHT, LINK_DL_MASK_ALL_NIGHT, gCallHumanMaskAllNightDL);
    registerMaskDL(vanillaPack, "__mm_mask_bunny_hood__", "Bunny Hood", PMM_MODEL_TYPE_MASK_BUNNY, LINK_DL_MASK_BUNNY, gCallHumanMaskBunnyDL);
    registerMaskDL(vanillaPack, "__mm_mask_keaton__", "Keaton Mask", PMM_MODEL_TYPE_MASK_KEATON, LINK_DL_MASK_KEATON, gCallHumanMaskKeatonDL);
    registerMaskDL(vanillaPack, "__mm_mask_garo__", "Garo's Mask", PMM_MODEL_TYPE_MASK_GARO, LINK_DL_MASK_GARO, gCallHumanMaskGaroDL);
    registerMaskDL(vanillaPack, "__mm_mask_romani__", "Romani's Mask", PMM_MODEL_TYPE_MASK_ROMANI, LINK_DL_MASK_ROMANI, gCallHumanMaskRomaniDL);
    registerMaskDL(vanillaPack, "__mm_mask_circus_leader__", "Circus Leader's Mask", PMM_MODEL_TYPE_MASK_CIRCUS_LEADER, LINK_DL_MASK_CIRCUS_LEADER, gCallHumanMaskCircusLeaderDL);
    registerMaskDL(vanillaPack, "__mm_mask_postman__", "Postman's Hat", PMM_MODEL_TYPE_MASK_POSTMAN, LINK_DL_MASK_POSTMAN, gCallHumanMaskPostmanDL);
    registerMaskDL(vanillaPack, "__mm_mask_couple__", "Couple's Mask", PMM_MODEL_TYPE_MASK_COUPLE, LINK_DL_MASK_COUPLE, gCallHumanMaskCoupleDL);
    registerMaskDL(vanillaPack, "__mm_mask_great_fairy__", "Great Fairy Mask", PMM_MODEL_TYPE_MASK_GREAT_FAIRY, LINK_DL_MASK_GREAT_FAIRY, gCallHumanMaskGreatFairyDL);
    registerMaskDL(vanillaPack, "__mm_mask_gibdo__", "Gibdo Mask", PMM_MODEL_TYPE_MASK_GIBDO, LINK_DL_MASK_GIBDO, gCallHumanMaskGibdoDL);
    registerMaskDL(vanillaPack, "__mm_mask_don_gero__", "Don Gero's Mask", PMM_MODEL_TYPE_MASK_DON_GERO, LINK_DL_MASK_DON_GERO, gCallHumanMaskDonGeroDL);
    registerMaskDL(vanillaPack, "__mm_mask_kamaro__", "Kamaro's Mask", PMM_MODEL_TYPE_MASK_KAMARO, LINK_DL_MASK_KAMARO, gCallHumanMaskKamaroDL);
    registerMaskDL(vanillaPack, "__mm_mask_captain__", "Captain's Hat", PMM_MODEL_TYPE_MASK_CAPTAIN, LINK_DL_MASK_CAPTAIN, gCallHumanMaskCaptainDL);
    registerMaskDL(vanillaPack, "__mm_mask_stone__", "Stone Mask", PMM_MODEL_TYPE_MASK_STONE, LINK_DL_MASK_STONE, gCallHumanMaskStoneDL);
    registerMaskDL(vanillaPack, "__mm_mask_bremen__", "Bremen Mask", PMM_MODEL_TYPE_MASK_BREMEN, LINK_DL_MASK_BREMEN, gCallHumanMaskBremenDL);
    registerMaskDL(vanillaPack, "__mm_mask_scents__", "Mask of Scents", PMM_MODEL_TYPE_MASK_SCENTS, LINK_DL_MASK_SCENTS, gCallHumanMaskScentsDL);
    registerMaskDL2(vanillaPack, "__mm_mask_blast__", "Blast Mask", PMM_MODEL_TYPE_MASK_BLAST, LINK_DL_MASK_BLAST, gCallHumanMaskBlastDL, LINK_DL_MASK_BLAST_COOLING_DOWN, gCallHumanMaskBlastCooldownDL);
    registerMaskDL(vanillaPack, "__mm__mask_giant__", "Giant's Mask", PMM_MODEL_TYPE_MASK_GIANT, LINK_DL_MASK_GIANT, gCallHumanMaskGiantDL);
    registerMaskDL2(vanillaPack, "__mm_mask_fierce_deity__", "Fierce Deity's Mask", PMM_MODEL_TYPE_MASK_FIERCE_DEITY, LINK_DL_MASK_FIERCE_DEITY, gCallHumanMaskFierceDeityDL, LINK_DL_MASK_FIERCE_DEITY_SCREAM, gCallHumanMaskFierceDeityScreamDL);
    registerMaskDL2(vanillaPack, "__mm_mask_goron__", "Goron Mask", PMM_MODEL_TYPE_MASK_GORON, LINK_DL_MASK_GORON, gCallHumanMaskGoronDL, LINK_DL_MASK_GORON_SCREAM, gCallHumanMaskGoronScreamDL);
    registerMaskDL2(vanillaPack, "__mm_mask_zora__", "Zora Mask", PMM_MODEL_TYPE_MASK_ZORA, LINK_DL_MASK_ZORA, gCallHumanMaskZoraDL, LINK_DL_MASK_ZORA_SCREAM, gCallHumanMaskZoraScreamDL);
    registerMaskDL2(vanillaPack, "__mm_mask_deku__", "Deku Mask", PMM_MODEL_TYPE_MASK_DEKU, LINK_DL_MASK_DEKU, gCallHumanMaskDekuDL, LINK_DL_MASK_DEKU_SCREAM, gCallHumanMaskDekuScreamDL);

    // Bomb
    {
        PlayerModelManagerHandle h = PlayerModelManager_registerModel(PMM_API_VERSION, "__mm_bomb__", PMM_MODEL_TYPE_BOMB);
        PlayerModelManager_setDisplayName(h, "Bomb");
        PlayerModelManager_setAuthor(h, "Nintendo");

        PlayerModelManager_setDisplayList(h, LINK_DL_BOMB_BODY_2D, gCallHumanBombBodyDL);
        PlayerModelManager_setDisplayList(h, LINK_DL_BOMB_BODY_3D, gEmptyDL);
        PlayerModelManager_setDisplayList(h, LINK_DL_BOMB_CAP, gCallHumanBombCapDL);

        PlayerModelManager_addHandleToPack(vanillaPack, h);
    }

    // Bombchu
    {
        PlayerModelManagerHandle h = PlayerModelManager_registerModel(PMM_API_VERSION, "__mm_bombchu__", PMM_MODEL_TYPE_BOMBCHU);
        PlayerModelManager_setDisplayName(h, "Bombchu");
        PlayerModelManager_setAuthor(h, "Nintendo");

        PlayerModelManager_setDisplayList(h, LINK_DL_BOMBCHU, gCallHumanBombchuDL);

        PlayerModelManager_addHandleToPack(vanillaPack, h);
    }
}
