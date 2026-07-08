#include "global.h"
#include "modding.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"
#include "modelmatrixids.h"
#include "formproxy.h"
#include "proxyactorext.h"
#include "yazmtcorelib_api.h"

static bool isMatrixesEqual(const Mtx *a, const Mtx *b) {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (a->intPart[i][j] != b->intPart[i][j] || a->fracPart[i][j] != b->fracPart[i][j]) {
                return false;
            }
        }
    }

    return true;
}

static bool isIdentityMtx(const Mtx *m) {
    return isMatrixesEqual(m, &gIdentityMtx);
}

static bool sIsMaskMatrixPushed = false;

void handleMaskMtx_on_Player_PostLimbDrawGameplay(PlayState *play, s32 limbIndex, Gfx **dList1, Player *player) {
    sIsMaskMatrixPushed = false;

    if (limbIndex == PLAYER_LIMB_HEAD) {
        FormProxy *fp = ProxyActorExt_getFormProxy(&player->actor);

        if (fp) {
            Mtx *maskMtx = FormProxy_getMatrix(fp, LINK_EQUIP_MATRIX_MASKS);
            if (maskMtx && !isIdentityMtx(maskMtx)) {
                if (((*dList1 != NULL) && ((u32)player->currentMask != PLAYER_MASK_NONE)) &&
                    (((player->transformation == PLAYER_FORM_HUMAN) &&
                      ((player->skelAnime.animation != &gPlayerAnim_cl_setmask) || (player->skelAnime.curFrame >= 12.0f))) ||
                     ((((player->transformation != PLAYER_FORM_HUMAN) && (player->currentMask >= PLAYER_MASK_FIERCE_DEITY)) &&
                       ((player->transformation + PLAYER_MASK_FIERCE_DEITY) != player->currentMask)) &&
                      (player->skelAnime.curFrame >= 10.0f)))) {
                    s32 maskMinusOne = player->currentMask - 1;
                    OPEN_DISPS(play->state.gfxCtx);
                    Matrix_Push();
                    sIsMaskMatrixPushed = true;
                    static MtxF sMaskMtxF;
                    Matrix_MtxToMtxF(maskMtx, &sMaskMtxF);
                    Matrix_Mult(&sMaskMtxF, MTXMODE_APPLY);
                    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
                    CLOSE_DISPS(play->state.gfxCtx);
                }
            }
        }
    }
}

void handleMaskMtx_on_return_Player_PostLimbDrawGameplay(void) {
    if (sIsMaskMatrixPushed) {
        Matrix_Pop();
    }
}

#define ADULT_LINK_BREMEN_HEIGHT_MODIFIER 1250.0f
#define COMPAT_ADULT_HEIGHT_MODIFIER 1150.0f
static bool sPushedMatrixBremen = false;
static bool sPushedMatrixCompatAdultHeight = false;

void fixCompatAdultHeight_on_Player_Draw(Player *player, PlayState *play) {
#if defined(ZELDA_ANDROID_BUILTIN_PMM)
    sPushedMatrixCompatAdultHeight = false;

    FormProxy *fp = ProxyActorExt_getFormProxy(&player->actor);

    if (fp && player->transformation == PLAYER_FORM_HUMAN && FormProxy_isAdultModelType(fp)) {
        OPEN_DISPS(play->state.gfxCtx);
        Matrix_Push();
        sPushedMatrixCompatAdultHeight = true;
        Matrix_Translate(0.0f, COMPAT_ADULT_HEIGHT_MODIFIER, 0.0f, MTXMODE_APPLY);
        CLOSE_DISPS(play->state.gfxCtx);
    }
#endif
}

void fixCompatAdultHeight_on_return_Player_Draw(void) {
#if defined(ZELDA_ANDROID_BUILTIN_PMM)
    if (sPushedMatrixCompatAdultHeight) {
        Matrix_Pop();
        sPushedMatrixCompatAdultHeight = false;
    }
#endif
}

void fixAdultBremen_on_Player_Draw(Player *player, PlayState *play) {
    sPushedMatrixBremen = false;

    FormProxy *fp = ProxyActorExt_getFormProxy(&player->actor);

    if (fp) {
        if (player->transformation == PLAYER_FORM_HUMAN && FormProxy_isAdultModelType(fp)) {
            if (player->stateFlags3 & PLAYER_STATE3_20000000) {
                OPEN_DISPS(play->state.gfxCtx);
                Matrix_Push();
                sPushedMatrixBremen = true;
                Matrix_Translate(0.f, ADULT_LINK_BREMEN_HEIGHT_MODIFIER, 0.f, MTXMODE_APPLY);
                MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
                CLOSE_DISPS(play->state.gfxCtx);
            }
        }
    }
}

void fixAdultBremen_on_return_Player_Draw(void) {
    if (sPushedMatrixBremen) {
        Matrix_Pop();
    }
}

void repositionHeldActors_on_Player_PostLimbDrawGameplay(PlayState *play, s32 limbIndex, Player *player) {
    if (player->actor.scale.y >= 0.f) {
        FormProxy *fp = ProxyActorExt_getFormProxy(&player->actor);

        if (fp) {
            Actor *heldActor = player->heldActor;

            if (fp && heldActor) {
                if (limbIndex == PLAYER_LIMB_LEFT_HAND && player->rightHandType == PLAYER_MODELTYPE_RH_BOW) {
                    if (!Player_IsHoldingHookshot(player)) {
                        if ((player->stateFlags3 & PLAYER_STATE3_40) && (player->transformation != PLAYER_FORM_DEKU)) {
                            Mtx *arrowMtx = FormProxy_getMatrix(fp, LINK_EQUIP_MATRIX_ARROW_DRAWN);

                            if (arrowMtx && !isIdentityMtx(arrowMtx)) {
                                OPEN_DISPS(play->state.gfxCtx);
                                MtxF arrowMtxF;
                                Matrix_MtxToMtxF(arrowMtx, &arrowMtxF);
                                Matrix_Mult(&arrowMtxF, MTXMODE_APPLY);
                                CLOSE_DISPS(play->state.gfxCtx);
                            }
                        }
                    }
                } else if (limbIndex == PLAYER_LIMB_RIGHT_HAND && Player_IsHoldingHookshot(player)) {
                    Mtx *hookMtx = FormProxy_getMatrix(fp, LINK_EQUIP_MATRIX_HOOKSHOT_CHAIN_AND_HOOK);

                    if (hookMtx && !isIdentityMtx(hookMtx)) {
                        OPEN_DISPS(play->state.gfxCtx);
                        MtxF hookMtxF;
                        Matrix_MtxToMtxF(hookMtx, &hookMtxF);
                        Matrix_Mult(&hookMtxF, MTXMODE_APPLY);
                        CLOSE_DISPS(play->state.gfxCtx);
                    }
                }
            }
        }
    }
}
