#include "global.h"
#include "formproxy.h"
#include "playerproxy.h"
#include "proxyactorext.h"
#include "overlays/actors/ovl_En_Boom/z_en_boom.h"

void copyProxyInfo_on_return_Player_UpperAction_14(Player *player, PlayState *play) {
    if (player->zoraBoomerangActor) {
        ProxyActorExt_copyProxyInformation(player->zoraBoomerangActor, &player->actor);
        
        if (player->zoraBoomerangActor->child) {
            ProxyActorExt_copyProxyInformation(player->zoraBoomerangActor->child, &player->actor);
        }
    }
}

typedef struct {
    /* 0x00 */ Gfx *unk_00;
    /* 0x04 */ Vec3f unk_04;
    /* 0x10 */ Vec3f unk_10;
} EnBoomStruct; // size = 0x1C

extern EnBoomStruct D_808A3078[];

void updateFinBoomerangDL_on_EnBoom_Draw(Actor *actor, PlayState *play) {
    Player *player = GET_PLAYER(play);

    FormProxyId formId;

    if (!PlayerProxy_getProxyIdFromForm(player->transformation, &formId)) {
        formId = FORM_PROXY_ID_ZORA;
    }

    FormProxy *fp = ProxyActorExt_getFormProxyOrFallback(actor, formId, play);

    if (fp) {
        Link_DisplayList target = actor->params == ZORA_BOOMERANG_LEFT ? LINK_DL_LFIN_BOOMERANG : LINK_DL_RFIN_BOOMERANG;
        Gfx *boomerangDL = FormProxy_getDL(fp, target);

        if (boomerangDL) {
            D_808A3078[actor->params].unk_00 = boomerangDL;
        }
    }
}
