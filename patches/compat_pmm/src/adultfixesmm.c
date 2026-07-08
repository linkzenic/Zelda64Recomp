#include "modding.h"
#include "global.h"
#include "recomputils.h"
#include "recompconfig.h"
#include "formproxy.h"
#include "modelentry.h"
#include "playerproxy.h"
#include "proxyactorext.h"
#include "overlays/actors/ovl_En_Rd/z_en_rd.h"
#include "overlays/actors/ovl_En_Railgibud/z_en_railgibud.h"
#include "overlays/actors/ovl_En_Talk_Gibud/z_en_talk_gibud.h"
#include "overlays/actors/ovl_Arms_Hook/z_arms_hook.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"
#include "../../misc_funcs.h"

static FormProxy *getAdultFixFormProxy(Player *player) {
    return ProxyActorExt_getFormProxy(&player->actor);
}

#if defined(ZELDA_ANDROID_BUILTIN_PMM)
static FormProxy *getCompatAdultAgePropsFormProxy(Player *player) {
    if (player->actor.id != ACTOR_PLAYER || player->transformation != PLAYER_FORM_HUMAN || !gPlayer1Proxy) {
        return NULL;
    }

    if (!PlayerProxy_getCurrentEntry(gPlayer1Proxy, PMM_MODEL_TYPE_CHILD)) {
        return NULL;
    }

    return PlayerProxy_getFormProxy(gPlayer1Proxy, FORM_PROXY_ID_HUMAN);
}
#endif

static bool shouldUseAdultFixes(Player *player) {
    if (player->transformation != PLAYER_FORM_HUMAN) {
        return false;
    }

    FormProxy *fp = getAdultFixFormProxy(player);

    return fp && FormProxy_isAdultModelType(fp);
}

static bool shouldUseCompatAdultAgeProps(Player *player);

bool PlayerModelManager_shouldUseAdultFixes(Player *player) {
#if defined(ZELDA_ANDROID_BUILTIN_PMM)
    return shouldUseCompatAdultAgeProps(player);
#else
    return shouldUseAdultFixes(player);
#endif
}

static bool shouldUseCompatAdultAgeProps(Player *player) {
    if (shouldUseAdultFixes(player)) {
        return true;
    }

#if defined(ZELDA_ANDROID_BUILTIN_PMM)
    FormProxy *fp = getCompatAdultAgePropsFormProxy(player);
    return fp && FormProxy_isAdultModelType(fp);
#else
    return false;
#endif
}

bool PlayerModelManager_shouldUseCompatAdultModel(Player *player) {
    return shouldUseCompatAdultAgeProps(player);
}

static Gfx *sFirstPersonDLToOverrideWith;

static void setFirstPersonPlayerDLOverride(Player *player, Link_DisplayList hookshotDLId, Link_DisplayList bowDLId) {
    FormProxy *fp = getAdultFixFormProxy(player);

    if (fp) {
        if (Player_IsHoldingHookshot(player)) {
            sFirstPersonDLToOverrideWith = FormProxy_getDL(fp, hookshotDLId);
        } else {
            sFirstPersonDLToOverrideWith = FormProxy_getDL(fp, bowDLId);
        }
    }
}

void addCustomFirstPersonDLs_on_Player_OverrideLimbDrawFirstPerson(Player *player, s32 limbIndex) {
    sFirstPersonDLToOverrideWith = NULL;

    if (player->unk_AA5 == PLAYER_UNKAA5_3) { // vanilla first person limb override doesn't draw unless this condition is met
        if (limbIndex == PLAYER_LIMB_LEFT_SHOULDER) {
            setFirstPersonPlayerDLOverride(player, LINK_DL_OPT_FPS_LSHOULDER_HOOKSHOT, LINK_DL_OPT_FPS_LSHOULDER_BOW);
        } else if (limbIndex == PLAYER_LIMB_RIGHT_FOREARM) {
            setFirstPersonPlayerDLOverride(player, LINK_DL_OPT_FPS_RFOREARM_HOOKSHOT, LINK_DL_OPT_FPS_RFOREARM_BOW);
        }
    }
}

void addCustomFirstPersonDLs_on_return_Player_OverrideLimbDrawFirstPerson(Gfx **dList) {
    if (sFirstPersonDLToOverrideWith) {
        *dList = sFirstPersonDLToOverrideWith;
    }
}

PlayerAgeProperties gAdultLinkAgeProps;

extern PlayerAgeProperties sPlayerAgeProperties[];

void initAdultLinkAgeProperties(void) {
    PlayerAgeProperties *fdProps = &sPlayerAgeProperties[PLAYER_FORM_FIERCE_DEITY];
    gAdultLinkAgeProps = sPlayerAgeProperties[PLAYER_FORM_ZORA];

    gAdultLinkAgeProps.unk_28 = 44.6f;
    gAdultLinkAgeProps.unk_3C = 15.0f;
    gAdultLinkAgeProps.unk_44 = fdProps->unk_44;

    for (int i = 0; i < ARRAY_COUNT(gAdultLinkAgeProps.unk_4A); ++i) {
        gAdultLinkAgeProps.unk_4A[i] = fdProps->unk_4A[i];
        gAdultLinkAgeProps.unk_62[i] = fdProps->unk_62[i];
        gAdultLinkAgeProps.unk_7A[i] = fdProps->unk_7A[i];
    }
    gAdultLinkAgeProps.voiceSfxIdOffset = SFX_VOICE_BANK_SIZE * 0;
    gAdultLinkAgeProps.surfaceSfxIdOffset = 0x80;
}

extern u8 sPlayerMass[PLAYER_FORM_MAX];
static u8 sHumanMass;

void changePlayerMass_on_Player_UpdateCommon(Player *player) {
    sHumanMass = sPlayerMass[PLAYER_FORM_HUMAN];

    if (shouldUseAdultFixes(player)) {
        sPlayerMass[PLAYER_FORM_HUMAN] = sPlayerMass[PLAYER_FORM_ZORA];
    }
}

void changePlayerMass_on_return_Player_UpdateCommon(void) {
    sPlayerMass[PLAYER_FORM_HUMAN] = sHumanMass;
}

#define NUM_BOOT_PROPERTIES 18
extern s16 D_801BFE14[PLAYER_BOOTS_MAX][NUM_BOOT_PROPERTIES];
#define PLAYER_BOOTS_ARR D_801BFE14

static void copyBootProperties(s16 *dest, s16 *src) {
    for (int i = 0; i < NUM_BOOT_PROPERTIES; ++i) {
        dest[i] = src[i];
    }
}

static s16 sBootsPropsTemp[NUM_BOOT_PROPERTIES];
static bool sIsBootsOverwritten;

void setAdultBootData_on_func_80123140(PlayState *play, Player *player) {
    sIsBootsOverwritten = shouldUseAdultFixes(player) || shouldUseCompatAdultAgeProps(player);

    if (sIsBootsOverwritten) {
        copyBootProperties(sBootsPropsTemp, PLAYER_BOOTS_ARR[PLAYER_BOOTS_HYLIAN]);
        copyBootProperties(PLAYER_BOOTS_ARR[PLAYER_BOOTS_HYLIAN], PLAYER_BOOTS_ARR[PLAYER_BOOTS_ZORA_LAND]);
    }
}

void setAdultBootData_on_return_func_80123140(void) {
    if (sIsBootsOverwritten) {
        copyBootProperties(PLAYER_BOOTS_ARR[PLAYER_BOOTS_HYLIAN], sBootsPropsTemp);
    }
}

static PlayerTransformation sRealPlayerFormPlayerDoorKnob;

void replaceDoorAnim_on_Player_Door_Knob(Player *player) {
    sRealPlayerFormPlayerDoorKnob = player->transformation;

    if (shouldUseAdultFixes(player)) {
        player->transformation = PLAYER_FORM_FIERCE_DEITY;
    }
}

void replaceDoorAnim_on_return_Player_Door_Knob(Player *player) {
    player->transformation = sRealPlayerFormPlayerDoorKnob;
}

void initVanillaProps_on_Player_Init(void) {
    static bool isFirstTimeInitDone;
    if (!isFirstTimeInitDone) {
        isFirstTimeInitDone = true;
        initAdultLinkAgeProperties();
    }
}

static void updateAgeProps(Player *player) {
    static s32 sAdultDiagCounter = 0;
    bool useAdultFixes = shouldUseAdultFixes(player);
    bool useAdultAgeProps = shouldUseCompatAdultAgeProps(player);

    if ((sAdultDiagCounter++ % 240) == 0) {
        FormProxy *fp = getAdultFixFormProxy(player);
        FormProxy *compatFp = NULL;
#if defined(ZELDA_ANDROID_BUILTIN_PMM)
        compatFp = getCompatAdultAgePropsFormProxy(player);
#endif
        const ModelEntry *adultEntry = gPlayer1Proxy ? PlayerProxy_getCurrentEntry(gPlayer1Proxy, PMM_MODEL_TYPE_ADULT) : NULL;
        const ModelEntry *childEntry = gPlayer1Proxy ? PlayerProxy_getCurrentEntry(gPlayer1Proxy, PMM_MODEL_TYPE_CHILD) : NULL;
        const char *entryName = adultEntry ? ModelEntry_getInternalName(adultEntry) : "AdultProps";
        s32 adultDiag =
            (useAdultFixes ? 1 : 0) |
            (fp ? 2 : 0) |
            ((fp && FormProxy_isAdultModelType(fp)) ? 4 : 0) |
            (adultEntry ? 8 : 0) |
            (childEntry ? 16 : 0) |
            (useAdultAgeProps ? 32 : 0) |
            (compatFp ? 64 : 0) |
            ((compatFp && FormProxy_isAdultModelType(compatFp)) ? 128 : 0);
        s32 adultEntryType = adultEntry ? ModelEntry_getType(adultEntry) : -1;

        recomp_android_compat_pmm_apply_log(61, adultDiag, adultEntryType,
                                            player->actor.shape.yOffset, entryName);
    }

    if (player->transformation == PLAYER_FORM_HUMAN) {
#if defined(ZELDA_ANDROID_BUILTIN_PMM)
        if (useAdultFixes && !getCompatAdultAgePropsFormProxy(player)) {
#else
        if (useAdultFixes) {
#endif
            player->ageProperties = &gAdultLinkAgeProps;
        } else {
            player->ageProperties = &sPlayerAgeProperties[PLAYER_FORM_HUMAN];
        }
    }
}

// Need to do this every frame or adult Link clips into the floor on unpause
void updateAdultProperties_on_Play_UpdateMain(PlayState *play) {
    Player *player = GET_PLAYER(play);

    while (player) {
#if defined(ZELDA_ANDROID_BUILTIN_PMM)
        ProxyActorExt_ensurePlayerFormProxy(player);
#endif
        updateAgeProps(player);
        player = (Player *)player->actor.next;
    }

    if (gActorOverlayTable[ACTOR_EN_TEST3].loadedRamAddr != NULL) {
        Actor *actor = play->actorCtx.actorLists[ACTORCAT_NPC].first;
        while (actor) {
            if (actor->id == ACTOR_EN_TEST3) {
                Player *kafei = (Player *)actor;

                if (shouldUseAdultFixes(kafei)) {
                    kafei->ageProperties = &gAdultLinkAgeProps;
                } else if (kafei->transformation == PLAYER_FORM_HUMAN) {
                    kafei->ageProperties = &sPlayerAgeProperties[PLAYER_FORM_HUMAN];
                }
            }

            actor = actor->next;
        }
    }
}

static PlayerTransformation sRealPlayerFormHeight;

void adjustHeight_on_Player_GetHeight(Player *player) {
    sRealPlayerFormHeight = player->transformation;

    if (shouldUseAdultFixes(player) || shouldUseCompatAdultAgeProps(player)) {
        player->transformation = PLAYER_FORM_ZORA;
    }
}

void adjustHeight_on_return_Player_GetHeight(Player *player) {
    player->transformation = sRealPlayerFormHeight;
}

// Use adult camera for first person bow/hookshot
static PlayerTransformation sRealPlayerFormFPCamera = PLAYER_FORM_HUMAN;

void fixFPCamera_on_func_8083868C(Player *this) {
    sRealPlayerFormFPCamera = this->transformation;

    if (shouldUseAdultFixes(this)) {
        this->transformation = PLAYER_FORM_ZORA;
    }
}

void fixFPCamera_on_return_func_8083868C(Player *player) {
    player->transformation = sRealPlayerFormFPCamera;
}

static PlayerTransformation sRealPlayerFormEnRdGrab;
static Player *sPlayerEnRdGrab;

void fixEnemyHeight_on_EnRd_Grab(PlayState *play) {
    Player *player = sPlayerEnRdGrab = GET_PLAYER(play);

    sRealPlayerFormEnRdGrab = player->transformation;

    if (shouldUseAdultFixes(player)) {
        player->transformation = PLAYER_FORM_ZORA;
    }
}

void fixEnemyHeight_on_return_EnRd_Grab(void) {
    sPlayerEnRdGrab->transformation = sRealPlayerFormEnRdGrab;
}

// Music Box Gibdo height fix
static PlayerTransformation sRealPlayerFormIdealPosMusicBox;

void fixEnemyHeight_on_EnRailgibud_MoveToIdealGrabPositionAndRotation(PlayState *play) {
    sRealPlayerFormIdealPosMusicBox = GET_PLAYER_FORM;

    if (shouldUseAdultFixes(GET_PLAYER(play))) {
        gSaveContext.save.playerForm = PLAYER_FORM_ZORA;
    }
}

void fixEnemyHeight_on_return_EnRailgibud_MoveToIdealGrabPositionAndRotation(void) {
    gSaveContext.save.playerForm = sRealPlayerFormIdealPosMusicBox;
}

// Talking Redead/Gibdo height fix
static PlayerTransformation sRealPlayerFormIdealPosTalk;

void fixEnemyHeight_on_EnTalkgibud_MoveToIdealGrabPositionAndRotation(PlayState *play) {
    sRealPlayerFormIdealPosTalk = GET_PLAYER_FORM;

    if (shouldUseAdultFixes(GET_PLAYER(play))) {
        gSaveContext.save.playerForm = PLAYER_FORM_ZORA;
    }
}

void fixEnemyHeight_on_return_EnTalkgibud_MoveToIdealGrabPositionAndRotation(void) {
    gSaveContext.save.playerForm = sRealPlayerFormIdealPosTalk;
}

// Move adult models higher on Epona
#define EPONA_HEIGHT_OFFSET 1100.f

void doEponaHeightOffset_on_return_Player_UpdateCommon(Player *player) {
    if (shouldUseAdultFixes(player) && player->stateFlags1 & PLAYER_STATE1_800000) {
        player->actor.shape.yOffset -= EPONA_HEIGHT_OFFSET;
    }
}

extern Vec3f D_801C0D60; // Adult arrow hand position
extern Vec3f D_801C0D6C; // Child arrow hand position
static Vec3f sChildArrowOffset;

extern Vec3f D_801C0D98; // Bow string data
static Vec3f sChildBowStringData;
static Vec3f sAdultBowStringData = {0.0f, -360.4f, 0.0f}; // From OoT

static bool sIsAdultBowData = false;

void applyAdultBowProperties_on_Player_Draw(Player *player, PlayState *play) {
    sIsAdultBowData = shouldUseAdultFixes(player);

    if (sIsAdultBowData) {
        sChildBowStringData = D_801C0D98;
        D_801C0D98 = sAdultBowStringData;

        sChildArrowOffset = D_801C0D6C;
        D_801C0D6C = D_801C0D60;
    }
}

void applyAdultBowProperties_on_return_Player_Draw(void) {
    if (sIsAdultBowData) {
        D_801C0D6C = sChildArrowOffset;
        D_801C0D98 = sChildBowStringData;
    }
}

extern Vec3f D_801C08C0[PLAYER_FORM_MAX];

static Vec3f sTmpAdjustSingleLegVec;

extern f32 D_801C08FC[PLAYER_FORM_MAX];
extern f32 D_801C0910[PLAYER_FORM_MAX];
extern f32 D_801C0924[PLAYER_FORM_MAX];
extern f32 D_801C0938[PLAYER_FORM_MAX];

typedef struct SingleLegFloat {
    f32 *vanillaArr;
    f32 tmp;
} SingleLegFloat;

static SingleLegFloat sSingleLegFloatsTmp[] = {
    {.vanillaArr = D_801C08FC},
    {.vanillaArr = D_801C0910},
    {.vanillaArr = D_801C0924},
    {.vanillaArr = D_801C0938},
};

static bool sIsSingleLegAdjusted;

void adjustAdultLeg_on_Player_AdjustSingleLeg(Player *player) {
    sIsSingleLegAdjusted = (shouldUseAdultFixes(player) || shouldUseCompatAdultAgeProps(player)) &&
                            player->skelAnime.animation != &gPlayerAnim_cl_maskoff;

    if (sIsSingleLegAdjusted) {
        sTmpAdjustSingleLegVec = D_801C08C0[PLAYER_FORM_HUMAN];
        D_801C08C0[PLAYER_FORM_HUMAN] = D_801C08C0[PLAYER_FORM_FIERCE_DEITY];

        for (int i = 0; i < ARRAY_COUNT(sSingleLegFloatsTmp); ++i) {
            SingleLegFloat *curr = &sSingleLegFloatsTmp[i];

            curr->tmp = curr->vanillaArr[PLAYER_FORM_HUMAN];
        }

    }
}

void adjustAdultLeg_on_return_Player_AdjustSingleLeg(void) {
    if (sIsSingleLegAdjusted) {
        D_801C08C0[PLAYER_FORM_HUMAN] = sTmpAdjustSingleLegVec;

        for (int i = 0; i < ARRAY_COUNT(sSingleLegFloatsTmp); ++i) {
            SingleLegFloat *curr = &sSingleLegFloatsTmp[i];

            curr->vanillaArr[PLAYER_FORM_HUMAN] = curr->tmp;
        }
    }
}

void fixAdultAgeProps_on_Player_InitCommon(Player *player) {
    // Earliest point age props can be updated without a RECOMP_PATCH
    // Makes adult Link use right voice during certain transitions
    // (e.g. getting thrown out of the Pirate's Fortress)
    if (player->actor.id == ACTOR_PLAYER) { // obligatory Kafei check
        updateAgeProps(player);
    }
}

typedef struct struct_8085D224 {
    /* 0x0 */ PlayerAnimationHeader *anim;
    /* 0x4 */ f32 unk_4;
    /* 0x8 */ f32 unk_8;
} struct_8085D224; // size = 0xC

extern struct_8085D224 D_8085D224[][2];

struct_8085D224 sTempHorseAnimInfo[2];

void fixAdultHorseMount_on_Player_ActionHandler_3(Player *player) {
    sTempHorseAnimInfo[0] = D_8085D224[0][0];
    sTempHorseAnimInfo[1] = D_8085D224[0][1];

    if (shouldUseAdultFixes(player)) {
        D_8085D224[0][0] = D_8085D224[1][0];
        D_8085D224[0][1] = D_8085D224[1][1];
    }
}

void fixAdultHorseMount_on_return_Player_ActionHandler_3(void) {
    D_8085D224[0][0] = sTempHorseAnimInfo[0];
    D_8085D224[0][1] = sTempHorseAnimInfo[1];
}

typedef struct AnimSfxEntry {
    /* 0x0 */ u16 sfxId;
    /* 0x2 */ s16 flags; // negative marks the end
} AnimSfxEntry;          // size = 0x4

typedef enum AnimSfxType {
    /*  1 */ ANIMSFX_TYPE_GENERAL = 1,
    /*  2 */ ANIMSFX_TYPE_FLOOR,
    /*  3 */ ANIMSFX_TYPE_FLOOR_BY_AGE,
    /*  4 */ ANIMSFX_TYPE_VOICE,
    /*  5 */ ANIMSFX_TYPE_FLOOR_LAND, // does not use sfxId
    /*  6 */ ANIMSFX_TYPE_6,          // FLOOR_WALK_Something // does not use sfxId
    /*  7 */ ANIMSFX_TYPE_FLOOR_JUMP, // does not use sfxId
    /*  8 */ ANIMSFX_TYPE_8,          // FLOOR_WALK_Something2 // does not use sfxId
    /*  9 */ ANIMSFX_TYPE_9,          // Uses NA_SE_PL_WALK_LADDER // does not use sfxId, unused
    /* 10 */ ANIMSFX_TYPE_SURFACE
} AnimSfxType;

#define ANIMSFX_SHIFT_TYPE(type) ((type) << 11)

#define ANIMSFX_CONTINUE (1)
#define ANIMSFX_STOP (0)

#define ANIMSFX_FLAGS(type, frame, cont) \
    (((ANIMSFX_##cont) == ANIMSFX_STOP ? -1 : 1) * (ANIMSFX_SHIFT_TYPE(type) | ((frame) & 0x7FF)))

#define ANIMSFX(type, frame, sfxId, cont) \
    {(sfxId), ANIMSFX_FLAGS(type, frame, cont)}

void adjustTransformationVoice_on_func_80855218(Player *player) {
    extern AnimSfxEntry D_8085D8F0[];

    D_8085D8F0[3].sfxId = shouldUseAdultFixes(player) ? NA_SE_NONE : NA_SE_PL_TRANSFORM_VOICE;
}

void adjustTransformationVoice_on_return_func_80855218(Player *player) {
    if ((R_PLAY_FILL_SCREEN_ON == 0) && (player->skelAnime.animation == &gPlayerAnim_cl_setmask) && shouldUseAdultFixes(player)) {
        static f32 adultTransformationMaskFreq = 0.94167805f;
        static s8 adultTransformationMaskReverbAdd = 0x40;

        if (PlayerAnimation_OnFrame(&player->skelAnime, 30)) {
            AudioSfx_PlaySfx(NA_SE_VO_LI_DEMO_DAMAGE, &player->actor.projectedPos, 4, &adultTransformationMaskFreq, &gSfxDefaultFreqAndVolScale,
                             &adultTransformationMaskReverbAdd);
        } else if (PlayerAnimation_OnFrame(&player->skelAnime, 50)) {
            AudioSfx_PlaySfx(NA_SE_VO_LI_TAKEN_AWAY, &player->actor.projectedPos, 4, &adultTransformationMaskFreq, &gSfxDefaultFreqAndVolScale,
                             &adultTransformationMaskReverbAdd);
        }
    }
}

void Player_PlaySfx(Player *player, u16 sfxId);

void playAdultLinkVoice_on_return_Player_AnimSfx_PlayVoice(Player *player, u16 sfxId) {
    if (player->actor.id == ACTOR_PLAYER && shouldUseAdultFixes(player)) {
        if (player->currentMask == PLAYER_MASK_GIANT) {
            extern f32 sGiantsMaskFreq;
            extern s8 sGiantsMaskReverbAdd;
            AudioSfx_PlaySfx((sfxId & 0x681F) + player->ageProperties->voiceSfxIdOffset, &player->actor.projectedPos, 4, &sGiantsMaskFreq, &gSfxDefaultFreqAndVolScale, &sGiantsMaskReverbAdd);
        }
        else if (player->currentMask != PLAYER_MASK_SCENTS) {
            if (sfxId == NA_SE_VO_LI_LASH) {
                Player_PlaySfx(player, NA_SE_VO_LI_SWORD_N + player->ageProperties->voiceSfxIdOffset);
            }
        }
    }
}

void fixAdultLinkSfx_on_Player_Action_65(Player *player) {
    extern AnimSfxEntry D_8085D73C[];

    if (shouldUseAdultFixes(player)) {
        D_8085D73C[0].flags = -1;
    } else {
        D_8085D73C[0].flags = ANIMSFX_FLAGS(ANIMSFX_TYPE_FLOOR_JUMP, 87, CONTINUE);
    }
}
