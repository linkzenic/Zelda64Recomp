#include "global.h"
#include "modding.h"
#include "modelmatrixids.h"
#include "z64recomp_api.h"
#include "formproxy.h"
#include "proxyactorext.h"
#include "playerproxy.h"
#include "overlays/actors/ovl_Mir_Ray3/z_mir_ray3.h"

ActorExtensionId gPlayerExtIdMirrorShieldBackMf;

RECOMP_CALLBACK(".", _internal_preInitHashObjects) void initMirrorShieldActorExtensions(void) {
    gPlayerExtIdMirrorShieldBackMf = z64recomp_extend_actor(ACTOR_PLAYER, sizeof(MtxF));
}

extern Vec3f sSheathLimbModelShieldOnBackPos;
extern Vec3s sSheathLimbModelShieldOnBackZyxRot;

void setMirrorShieldMtxF_on_Player_PostLimbDrawGameplay(Player *player, s32 limbIndex) {
    if (limbIndex == PLAYER_LIMB_SHEATH) {
        if (player->actor.scale.y >= 0.0f) {
            FormProxy *fp = ProxyActorExt_getFormProxy(&player->actor);

            if (fp) {
                MtxF *currMf = Matrix_GetCurrent();

                Mtx *formProxyM = FormProxy_getMatrix(fp, LINK_EQUIP_MATRIX_SHIELD_MIRROR_BACK);
                MtxF formProxyMf;
                Matrix_MtxToMtxF(formProxyM, &formProxyMf);

                MtxF *dest = z64recomp_get_extended_actor_data(&player->actor, gPlayerExtIdMirrorShieldBackMf);

                if (dest) {
                    SkinMatrix_MtxFMtxFMult(currMf, &formProxyMf, dest);
                }
            }
        }
    }
}

static MtxF sRealShieldMf;
static Player *sMirRay3Player;

void overwriteShieldMtx_on_MirRay3_Draw(MirRay3 *mirRay3, PlayState *play) {
    Player *player = GET_PLAYER(play);

    if (!ProxyActorExt_getPlayerProxy(&mirRay3->actor) && PlayerProxy_shouldSkipInterpolation(gPlayer1Proxy)) {
        actor_set_interpolation_skipped(&mirRay3->actor);
    }

    sRealShieldMf = player->shieldMf;
    sMirRay3Player = player;

    if (!(mirRay3->unk_210 & 1) && Player_HasMirrorShieldEquipped(play) && (player->rightHandType != PLAYER_MODELTYPE_RH_SHIELD) &&
        (player->rightHandType != PLAYER_MODELTYPE_RH_FF)) {
        MtxF *shieldMtx = z64recomp_get_extended_actor_data(&player->actor, gPlayerExtIdMirrorShieldBackMf);

        if (shieldMtx) {
            player->shieldMf = *shieldMtx;
        }
    }
}

void overwriteShieldMtx_on_return_MirRay3_Draw(void) {
    sMirRay3Player->shieldMf = sRealShieldMf;
}
