#include "global.h"
#include "formproxy.h"
#include "proxyactorext.h"
#include "playerproxy.h"
#include "modelinfo.h"
#include "overlays/actors/ovl_En_Test3/z_en_test3.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"
#include "logger.h"

void updatePlayerAssetsCommon(Player *player, FormProxy *fp, TexturePtr eyesTex[], TexturePtr mouthTex[]);

void updateAssets_on_EnTest3_Draw(EnTest3 *enTest3) {
    Player *player = &enTest3->player;

    FormProxy *fp = ProxyActorExt_getFormProxy(&player->actor);

    if (fp) {
        extern TexturePtr sEyeTextures_ovl_En_Test3[];
        extern TexturePtr sMouthTextures_ovl_En_Test3[];
        updatePlayerAssetsCommon(player, fp, sEyeTextures_ovl_En_Test3, sMouthTextures_ovl_En_Test3);
    }
}

void fixIdleAnimOddities_on_return_EnTest3_OverrideLimbDraw(EnTest3 *kafei, s32 limbIndex, Gfx **dList, Vec3s *rot) {
    FormProxy *fp = ProxyActorExt_getFormProxy(&kafei->player.actor);

    if (fp) {
        Player *player = &kafei->player;

        if (limbIndex == PLAYER_LIMB_HAT) {
            PlayerAnimationHeader *currAnim = player->skelAnime.animation;

            if ((player->transformation == PLAYER_FORM_HUMAN && currAnim == &gPlayerAnim_pz_wait) ||
                (currAnim >= &gPlayerAnim_kf_awase && currAnim <= &gPlayerAnim_kf_tetunagu_loop)) {
                static Vec3s hatRot = {0, 0, DEG_TO_BINANG(100.f)};
                *rot = hatRot;
            }
        } else if (limbIndex == PLAYER_LIMB_SHEATH) {
            *dList = NULL;
        }
    }
}
