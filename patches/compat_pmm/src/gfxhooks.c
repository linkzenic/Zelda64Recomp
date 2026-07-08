#include "global.h"
#include "modding.h"
#include "recompdata.h"
#include "recomputils.h"
#include "modelmatrixids.h"
#include "logger.h"
#include "maskdls.h"
#include "playerproxy.h"
#include "formproxy.h"
#include "proxyactorext.h"
#include "../../misc_funcs.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"
#include "assets/objects/object_link_child/object_link_child.h"
#include "assets/objects/object_link_boy/object_link_boy.h"
#include "assets/objects/object_link_nuts/object_link_nuts.h"
#include "assets/objects/object_link_goron/object_link_goron.h"
#include "assets/objects/object_link_zora/object_link_zora.h"
#include "assets/objects/object_mir_ray/object_mir_ray.h"
#include "overlays/actors/ovl_En_Arrow/z_en_arrow.h"
#include "overlays/actors/ovl_En_Bom/z_en_bom.h"
#include "overlays/actors/ovl_Arms_Hook/z_arms_hook.h"
#include "overlays/actors/ovl_Mir_Ray3/z_mir_ray3.h"
#include "overlays/actors/ovl_En_Bom_Chu/z_en_bom_chu.h"

typedef struct {
    Gfx *target;
    Link_DisplayList replacementId;
} GfxHookDisplayList;

#define DECLARE_GFX_HOOK_DL(targetDL, replacementDLId) {.target = targetDL, .replacementId = replacementDLId}

typedef struct {
    U32ValueHashmapHandle gfxPtrsToDLs;
    GfxHookDisplayList *rawHookDLEntries;
    size_t numHookDLEntries;
} GfxHookLookup;

#define DECLARE_GFX_HOOK_LUT(hookDLs)             \
    {                                             \
        .gfxPtrsToDLs = 0,                        \
        .rawHookDLEntries = hookDLs,              \
        .numHookDLEntries = ARRAY_COUNT(hookDLs), \
    }

typedef struct {
    PlayState *play;
    FormProxy *formProxy;
    const GfxHookLookup *segment04;
    const GfxHookLookup *segment06;
    const GfxHookLookup *segment0A;
    Gfx *startOpa;
    Gfx *startXlu;
} GfxHookData;

static GfxHookDisplayList sLinkHumanDLReplacements[] = {
    DECLARE_GFX_HOOK_DL(object_link_child_DL_017818, LINK_DL_BOW_STRING),
    DECLARE_GFX_HOOK_DL(object_link_child_DL_01D960, LINK_DL_HOOKSHOT_HOOK),
};
static GfxHookLookup sLinkHumanDLMap = DECLARE_GFX_HOOK_LUT(sLinkHumanDLReplacements);

static GfxHookDisplayList sLinkDekuDLReplacements[] = {
    DECLARE_GFX_HOOK_DL(object_link_nuts_DL_007390, LINK_DL_PIPE_MOUTH),
    DECLARE_GFX_HOOK_DL(object_link_nuts_DL_007548, LINK_DL_PIPE_RIGHT),
    DECLARE_GFX_HOOK_DL(object_link_nuts_DL_0076A0, LINK_DL_PIPE_UP),
    DECLARE_GFX_HOOK_DL(object_link_nuts_DL_0077D0, LINK_DL_PIPE_DOWN),
    DECLARE_GFX_HOOK_DL(object_link_nuts_DL_007900, LINK_DL_PIPE_LEFT),
    DECLARE_GFX_HOOK_DL(object_link_nuts_DL_007A28, LINK_DL_PIPE_A),
    DECLARE_GFX_HOOK_DL(object_link_nuts_DL_009AB8, LINK_DL_PAD_WOOD),
    DECLARE_GFX_HOOK_DL(object_link_nuts_DL_009C48, LINK_DL_PAD_GRASS),
    DECLARE_GFX_HOOK_DL(object_link_nuts_DL_009DB8, LINK_DL_PAD_OPENING),
    DECLARE_GFX_HOOK_DL(object_link_nuts_DL_008860, LINK_DL_PETAL_PARTICLE),
    DECLARE_GFX_HOOK_DL(object_link_nuts_DL_00A348, LINK_DL_DEKU_GUARD),
    DECLARE_GFX_HOOK_DL(gLinkDekuOpenFlowerDL, LINK_DL_CENTER_FLOWER_PROPELLER_OPEN),
    DECLARE_GFX_HOOK_DL(gLinkDekuClosedFlowerDL, LINK_DL_CENTER_FLOWER_PROPELLER_CLOSED),
};
static GfxHookLookup sLinkDekuDLMap = DECLARE_GFX_HOOK_LUT(sLinkDekuDLReplacements);

static GfxHookDisplayList sLinkGoronDLReplacements[] = {
    DECLARE_GFX_HOOK_DL(object_link_goron_DL_00FC18, LINK_DL_DRUM_STRAP),
    DECLARE_GFX_HOOK_DL(object_link_goron_DL_00FCF0, LINK_DL_DRUM_UP),
    DECLARE_GFX_HOOK_DL(object_link_goron_DL_00FF18, LINK_DL_DRUM_LEFT),
    DECLARE_GFX_HOOK_DL(object_link_goron_DL_010140, LINK_DL_DRUM_RIGHT),
    DECLARE_GFX_HOOK_DL(object_link_goron_DL_010368, LINK_DL_DRUM_DOWN),
    DECLARE_GFX_HOOK_DL(object_link_goron_DL_010590, LINK_DL_DRUM_A),
    DECLARE_GFX_HOOK_DL(gLinkGoronCurledDL, LINK_DL_CURLED),
    DECLARE_GFX_HOOK_DL(object_link_goron_DL_00C540, LINK_DL_SPIKES),
    DECLARE_GFX_HOOK_DL(object_link_goron_DL_0127B0, LINK_DL_FIRE_INIT),
    DECLARE_GFX_HOOK_DL(object_link_goron_DL_0134D0, LINK_DL_FIRE_ROLL),
    DECLARE_GFX_HOOK_DL(gLinkGoronGoronPunchEffectDL, LINK_DL_FIRE_PUNCH),
};
static GfxHookLookup sLinkGoronDLMap = DECLARE_GFX_HOOK_LUT(sLinkGoronDLReplacements);

static GfxHookDisplayList sLinkZoraDLReplacements[] = {
    DECLARE_GFX_HOOK_DL(gLinkZoraLeftHandOpenDL, LINK_DL_LHAND),
    DECLARE_GFX_HOOK_DL(gLinkZoraRightHandOpenDL, LINK_DL_RHAND),
    DECLARE_GFX_HOOK_DL(object_link_zora_DL_00E088, LINK_DL_OPT_LHAND_GUITAR),
    DECLARE_GFX_HOOK_DL(object_link_zora_DL_00E2A0, LINK_DL_GUITAR),
    DECLARE_GFX_HOOK_DL(object_link_zora_DL_0110A8, LINK_DL_FIN_SHIELD),
    DECLARE_GFX_HOOK_DL(object_link_zora_DL_011760, LINK_DL_MAGIC_BARRIER),
};
static GfxHookLookup sLinkZoraDLMap = DECLARE_GFX_HOOK_LUT(sLinkZoraDLReplacements);

static GfxHookDisplayList sShieldMirrorDLReplacements[] = {
    DECLARE_GFX_HOOK_DL(object_mir_ray_DL_0004B0, LINK_DL_SHIELD3_RAY),
    DECLARE_GFX_HOOK_DL(object_mir_ray_DL_000168, LINK_DL_SHIELD3_RAY_BEAM),
};
static GfxHookLookup sShieldMirrorDLMap = DECLARE_GFX_HOOK_LUT(sShieldMirrorDLReplacements);

static GfxHookDisplayList sGameplayKeepDLReplacements[] = {
    DECLARE_GFX_HOOK_DL(gDekuStickDL, LINK_DL_DEKU_STICK),
    DECLARE_GFX_HOOK_DL(gBottleContentsDL, LINK_DL_BOTTLE_CONTENTS),
    DECLARE_GFX_HOOK_DL(gBottleGlassDL, LINK_DL_BOTTLE_GLASS),
    DECLARE_GFX_HOOK_DL(gameplay_keep_DL_013FF0, LINK_DL_BOW_ARROW),
    DECLARE_GFX_HOOK_DL(gameplay_keep_DL_014370, LINK_DL_BOW_ARROW),
    DECLARE_GFX_HOOK_DL(gHookshotChainDL, LINK_DL_HOOKSHOT_CHAIN),
    DECLARE_GFX_HOOK_DL(gHookshotReticleDL, LINK_DL_HOOKSHOT_RETICLE),
    DECLARE_GFX_HOOK_DL(gBombBodyDL, LINK_DL_BOMB_BODY_2D),
    DECLARE_GFX_HOOK_DL(gBombCapDL, LINK_DL_BOMB_CAP),
    DECLARE_GFX_HOOK_DL(gBombchuDL, LINK_DL_BOMBCHU),
};
static GfxHookLookup sGameplayKeepDLMap = DECLARE_GFX_HOOK_LUT(sGameplayKeepDLReplacements);

static GfxHookDisplayList sBlastMaskDLReplacements[] = {
    DECLARE_GFX_HOOK_DL(object_mask_bakuretu_DL_000440, LINK_DL_MASK_BLAST_COOLING_DOWN),
};
static GfxHookLookup sBlastMaskDLMap = DECLARE_GFX_HOOK_LUT(sBlastMaskDLReplacements);

void GfxHookLookup_init(GfxHookLookup *ghl) {
    ghl->gfxPtrsToDLs = recomputil_create_u32_value_hashmap();
    for (size_t i = 0; i < ghl->numHookDLEntries; ++i) {
        GfxHookDisplayList *currHookDL = &ghl->rawHookDLEntries[i];
        if (!recomputil_u32_value_hashmap_insert(ghl->gfxPtrsToDLs, (uintptr_t)(currHookDL->target), currHookDL->replacementId)) {
            Logger_printWarning("GfxHookLookup_init: Passed in GfxHookLookup contains duplicate key 0x%X!\n", currHookDL->target);
        }
    }
}

RECOMP_CALLBACK(".", _internal_initHashObjects) void initGfxHookReplacmentMaps(void) {
    GfxHookLookup_init(&sGameplayKeepDLMap);
    GfxHookLookup_init(&sLinkHumanDLMap);
    GfxHookLookup_init(&sLinkDekuDLMap);
    GfxHookLookup_init(&sLinkZoraDLMap);
    GfxHookLookup_init(&sLinkGoronDLMap);
    GfxHookLookup_init(&sShieldMirrorDLMap);
    GfxHookLookup_init(&sBlastMaskDLMap);
}

// assumed hookdat has valid playstate ptr
static void fillGfxHookData(GfxHookData *hookDat, PlayState *play, FormProxy *formProxy, const GfxHookLookup *seg04, const GfxHookLookup *seg06, const GfxHookLookup *seg0A) {
    hookDat->play = play;
    hookDat->formProxy = formProxy;

    OPEN_DISPS(play->state.gfxCtx);

    hookDat->startOpa = POLY_OPA_DISP;
    hookDat->startXlu = POLY_XLU_DISP;
    hookDat->segment04 = seg04;
    hookDat->segment06 = seg06;
    hookDat->segment0A = seg0A;

    CLOSE_DISPS(play->state.gfxCtx);
}

static int replaceHookedGfxCommands(GfxHookData *hookDat, Gfx *startDL, Gfx *endDL) {
    if (!hookDat->formProxy) {
        return 0;
    }

    Gfx *curr = startDL;
    int replacements = 0;

    U32HashsetHandle seg04Handle = hookDat->segment04 ? hookDat->segment04->gfxPtrsToDLs : 0;
    U32HashsetHandle savedSeg04Handle = seg04Handle;

    U32HashsetHandle seg06Handle = hookDat->segment06 ? hookDat->segment06->gfxPtrsToDLs : 0;
    U32HashsetHandle savedSeg06Handle = seg06Handle;

    U32HashsetHandle seg0AHandle = hookDat->segment0A ? hookDat->segment0A->gfxPtrsToDLs : 0;
    U32HashsetHandle savedSeg0AHandle = seg0AHandle;

    uintptr_t origSeg04 = gSegments[0x04];
    uintptr_t origSeg06 = gSegments[0x06];
    uintptr_t origSeg0A = gSegments[0x0A];

    while (curr <= endDL) {
        // encountered a gSPSegment command
        // Disable replacing commands unless the object being pointed to is restored
        if ((curr->words.w0 >> 16) == ((G_MOVEWORD << 8) | G_MW_SEGMENT)) {
            int segment = (curr->words.w0 & 0xFFFF) / 4;

            if (segment == 0x04) {
                seg04Handle = curr->words.w1 == origSeg04 ? savedSeg04Handle : 0;
            } else if (segment == 0x06) {
                seg06Handle = curr->words.w1 == origSeg06 ? savedSeg06Handle : 0;
            } else if (segment == 0x0A) {
                seg0AHandle = curr->words.w1 == origSeg0A ? savedSeg0AHandle : 0;
            }
        }

        // Check if this command is segmented gSPDisplayList command
        if ((curr->words.w0 >> 16) == (G_DL << 8 | G_DL_PUSH) && curr->words.w1 < K0BASE) {
            U32ValueHashmapHandle hashmap = 0;

            switch (SEGMENT_NUMBER(curr->words.w1)) {
                case 0x04:
                    hashmap = seg04Handle;
                    break;

                case 0x06:
                    hashmap = seg06Handle;
                    break;

                case 0x0A:
                    hashmap = seg0AHandle;
                    break;

                default:
                    break;
            }

            if (hashmap) {
                unsigned long tmp;
                if (recomputil_u32_value_hashmap_get(hashmap, curr->words.w1, &tmp)) {
                    Gfx *dl = FormProxy_getDL(hookDat->formProxy, tmp);

                    if (dl) {
                        gSPDisplayList(curr, dl);
                        replacements++;
                    }
                }
            }
        }

        curr++;
    }

    return replacements;
}

static int replaceHookedOpaGfxCommands(GfxHookData *hookDat) {
    int replacements;

    OPEN_DISPS(hookDat->play->state.gfxCtx);
    replacements = replaceHookedGfxCommands(hookDat, hookDat->startOpa, POLY_OPA_DISP);
    CLOSE_DISPS(hookDat->play.state.gfxCtx);

    return replacements;
}

static int replaceHookedXluGfxCommands(GfxHookData *hookDat) {
    int replacements;

    OPEN_DISPS(hookDat->play->state.gfxCtx);
    replacements = replaceHookedGfxCommands(hookDat, hookDat->startXlu, POLY_XLU_DISP);
    CLOSE_DISPS(hookDat->play.state.gfxCtx);

    return replacements;
}

static GfxHookLookup *getFormGfxLookup(PlayerTransformation form) {
    GfxHookLookup *seg06;

    switch (form) {
        case PLAYER_FORM_GORON:
            seg06 = &sLinkGoronDLMap;
            break;

        case PLAYER_FORM_ZORA:
            seg06 = &sLinkZoraDLMap;
            break;

        case PLAYER_FORM_DEKU:
            seg06 = &sLinkDekuDLMap;
            break;

        case PLAYER_FORM_HUMAN:
            seg06 = &sLinkHumanDLMap;
            break;

        default:
            seg06 = NULL;
            break;
    }

    return seg06;
}

static GfxHookData sPlayerDrawGfxHook;

#if defined(ZELDA_ANDROID_BUILTIN_PMM)
static bool getCompatModelTypeForForm(PlayerTransformation form, PlayerModelManagerModelType *out) {
    if (!out) {
        return false;
    }

    switch (form) {
        case PLAYER_FORM_HUMAN:
            *out = PMM_MODEL_TYPE_CHILD;
            return true;

        case PLAYER_FORM_DEKU:
            *out = PMM_MODEL_TYPE_DEKU;
            return true;

        case PLAYER_FORM_GORON:
            *out = PMM_MODEL_TYPE_GORON;
            return true;

        case PLAYER_FORM_ZORA:
            *out = PMM_MODEL_TYPE_ZORA;
            return true;

        case PLAYER_FORM_FIERCE_DEITY:
            *out = PMM_MODEL_TYPE_FIERCE_DEITY;
            return true;

        default:
            return false;
    }
}

static FormProxy *getCompatSelectedPlayerFormProxy(Player *player) {
    PlayerModelManagerModelType modelType;
    FormProxyId fpId;

    if (!gPlayer1Proxy || !getCompatModelTypeForForm(player->transformation, &modelType)) {
        return NULL;
    }

    if (!PlayerProxy_getCurrentEntry(gPlayer1Proxy, modelType)) {
        return NULL;
    }

    if (!PlayerProxy_getProxyIdFromForm(player->transformation, &fpId)) {
        return NULL;
    }

    return PlayerProxy_getFormProxy(gPlayer1Proxy, fpId);
}
#endif

static FormProxy *getPlayerDrawFormProxy(Player *player) {
    FormProxy *fp = ProxyActorExt_getFormProxy(&player->actor);

#if defined(ZELDA_ANDROID_BUILTIN_PMM)
    if (!fp) {
        fp = getCompatSelectedPlayerFormProxy(player);
    }
#endif

    return fp;
}

void hookGfx_on_Player_Draw(Player *player, PlayState *play) {
    FormProxy *fp = getPlayerDrawFormProxy(player);

#if defined(ZELDA_ANDROID_BUILTIN_PMM)
    static int sPlayerDrawLogCounter = 0;
    if ((sPlayerDrawLogCounter++ % 120) == 0) {
        recomp_android_compat_pmm_apply_log(40, player->transformation, fp ? 1 : 0, gPlayer1Proxy ? 1 : 0, "Player_Draw");
    }
#endif

    fillGfxHookData(&sPlayerDrawGfxHook, play, fp, &sGameplayKeepDLMap, getFormGfxLookup(player->transformation), NULL);
}

void hookGfx_on_return_Player_Draw(void) {
    int opaReplacements = replaceHookedOpaGfxCommands(&sPlayerDrawGfxHook);
    int xluReplacements = replaceHookedXluGfxCommands(&sPlayerDrawGfxHook);

#if defined(ZELDA_ANDROID_BUILTIN_PMM)
    static int sPlayerDrawReturnLogCounter = 0;
    if ((sPlayerDrawReturnLogCounter++ % 120) == 0) {
        recomp_android_compat_pmm_apply_log(41, opaReplacements, xluReplacements, sPlayerDrawGfxHook.formProxy ? 1 : 0, "Player_Draw_return");
    }
#endif
}

static GfxHookData sPlayerDrawBlastMaskGfxHook;

void hookGfx_on_Player_DrawBlastMask(PlayState *play, Player *player) {
    fillGfxHookData(&sPlayerDrawBlastMaskGfxHook, play, ProxyActorExt_getFormProxy(&player->actor), &sGameplayKeepDLMap, NULL, &sBlastMaskDLMap);
}

void hookGfx_on_return_Player_DrawBlastMask(void) {
    replaceHookedOpaGfxCommands(&sPlayerDrawBlastMaskGfxHook);
    replaceHookedXluGfxCommands(&sPlayerDrawBlastMaskGfxHook);
}

void assignHookshotProxy_on_return_Player_InitHookshotIA(Player *player) {
    ProxyActorExt_copyProxyInformation(player->heldActor, &player->actor);
}

static GfxHookData sHookshotGfxHook;

void hookGfx_on_ArmsHook_Draw(ArmsHook *armsHook, PlayState *play) {
    fillGfxHookData(&sHookshotGfxHook, play, ProxyActorExt_getFormProxyOrFallback(&armsHook->actor, FORM_PROXY_ID_HUMAN, play), &sGameplayKeepDLMap, &sLinkHumanDLMap, NULL);
}

void hookGfx_on_return_ArmsHook_Draw(void) {
    replaceHookedOpaGfxCommands(&sHookshotGfxHook);
    replaceHookedXluGfxCommands(&sHookshotGfxHook);
}

void setupArrowProxy_on_return_func_808306F8(Player *player) {
    Actor *heldActor = player->heldActor;

    if (heldActor && heldActor->id == ACTOR_EN_ARROW) {
        ProxyActorExt_copyProxyInformation(heldActor, &player->actor);
    }
}

static GfxHookData sArrowGfxHook;

void hookGfx_on_EnArrow_Draw(EnArrow *enArrow, PlayState *play) {
    fillGfxHookData(&sArrowGfxHook, play, ProxyActorExt_getFormProxyOrFallback(&enArrow->actor, FORM_PROXY_ID_HUMAN, play), &sGameplayKeepDLMap, NULL, NULL);
}

void hookGfx_on_return_EnArrow_Draw(void) {
    replaceHookedOpaGfxCommands(&sArrowGfxHook);
    replaceHookedXluGfxCommands(&sArrowGfxHook);
}

static GfxHookData sMirrorRayGfxHook;

void hookGfx_on_MirRay3_Draw(MirRay3 *mirRay3, PlayState *play) {
    Player *player = GET_PLAYER(play);

    FormProxyId formId;

    if (!PlayerProxy_getProxyIdFromForm(player->transformation, &formId)) {
        formId = FORM_PROXY_ID_HUMAN;
    }

    fillGfxHookData(&sMirrorRayGfxHook, play, ProxyActorExt_getFormProxyOrFallback(&mirRay3->actor, formId, play), &sGameplayKeepDLMap, &sShieldMirrorDLMap, NULL);
}

void hookGfx_on_return_MirRay3_Draw(void) {
    replaceHookedOpaGfxCommands(&sMirrorRayGfxHook);
    replaceHookedXluGfxCommands(&sMirrorRayGfxHook);
}

void setupBombProxy_on_return_Player_InitExplosiveIA(Player *player) {
    Actor *heldActor = player->heldActor;

    if (heldActor && (heldActor->id == ACTOR_EN_BOM || heldActor->id == ACTOR_EN_BOM_CHU)) {
        ProxyActorExt_copyProxyInformation(heldActor, &player->actor);
    }
}

void draw3DBombBody_on_EnBom_Draw(EnBom *enBom, PlayState *play) {
    if (enBom->actor.params == BOMB_TYPE_BODY && !enBom->isPowderKeg) {
        FormProxy *fp = ProxyActorExt_getFormProxyOrFallback(&enBom->actor, FORM_PROXY_ID_HUMAN, play);

        if (fp) {
            Gfx *bombBody3DDL = FormProxy_getDL(fp, LINK_DL_BOMB_BODY_3D);

            if (bombBody3DDL) {
                Gfx_SetupDL25_Opa(play->state.gfxCtx);

                OPEN_DISPS(play->state.gfxCtx);
                MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
                gDPPipeSync(POLY_OPA_DISP++);
                gDPSetEnvColor(POLY_OPA_DISP++, (s8)enBom->unk_1F4, 0, 40, 255);
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, (s8)enBom->unk_1F4, 0, 40, 255);
                gSPDisplayList(POLY_OPA_DISP++, bombBody3DDL);

                CLOSE_DISPS();
            }
        }
    }
}

static GfxHookData sBombGfxHook;

void hookGfx_on_EnBom_Draw(EnBom *enBom, PlayState *play) {
    fillGfxHookData(&sBombGfxHook, play, ProxyActorExt_getFormProxyOrFallback(&enBom->actor, FORM_PROXY_ID_HUMAN, play), &sGameplayKeepDLMap, NULL, NULL);
}

void hookGfx_on_return_EnBom_Draw(void) {
    replaceHookedOpaGfxCommands(&sBombGfxHook);
}

static GfxHookData sBombchuGfxHook;

void hookGfx_on_EnBomChu_Draw(EnBomChu *enBomChu, PlayState *play) {
    fillGfxHookData(&sBombchuGfxHook, play, ProxyActorExt_getFormProxyOrFallback(&enBomChu->actor, FORM_PROXY_ID_HUMAN, play), &sGameplayKeepDLMap, NULL, NULL);
}

void hookGfx_on_return_EnBomChu_Draw(void) {
    replaceHookedOpaGfxCommands(&sBombchuGfxHook);
}

static GfxHookData sZoraMagicBarrierGfxHook;

void hookGfx_on_Player_DrawZoraShield(PlayState *play, Player *player) {
    fillGfxHookData(&sZoraMagicBarrierGfxHook, play, ProxyActorExt_getFormProxy(&player->actor), &sGameplayKeepDLMap, &sLinkZoraDLMap, NULL);
}

void hookGfx_on_return_Player_DrawZoraShield(void) {
    replaceHookedXluGfxCommands(&sZoraMagicBarrierGfxHook);
}
