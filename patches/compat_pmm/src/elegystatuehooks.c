#include "global.h"
#include "proxyactorext.h"
#include "playerproxy.h"
#include "formproxy.h"
#include "overlays/actors/ovl_En_Torch2/z_en_torch2.h"

void setupElegyStatueProxy_on_return_func_80848640(PlayState *play, Player *player) {
    EnTorch2 *elegyStatue = play->actorCtx.elegyShells[player->transformation];

    if (elegyStatue) {
        ProxyActorExt_copyProxyInformation(&elegyStatue->actor, &player->actor);
    }
}

void replaceElegyStatue_on_EnTorch2_Draw(Actor *actor, PlayState *play) {
    PlayerTransformation form = actor->params;

    FormProxyId formId;

    if (!PlayerProxy_getProxyIdFromForm(form, &formId)) {
        formId = FORM_PROXY_ID_HUMAN;
    }

    FormProxy *fp = ProxyActorExt_getFormProxyOrFallback(actor, formId, play);

    if (fp) {
        extern Gfx *sShellDLists[];

        Gfx *dl = FormProxy_getDL(fp, LINK_DL_ELEGY_OF_EMPTINESS_SHELL);

        if (dl) {
            sShellDLists[form] = dl;
        }
    }
}
