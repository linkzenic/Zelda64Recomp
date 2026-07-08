#include "global.h"
#include "modding.h"
#include "overlays/actors/ovl_En_Boom/z_en_boom.h"

static EnBoom *sEnBoom;
static PlayState *sPlay;

void updateFinBoomerangDL_on_EnBoom_Draw(Actor *actor, PlayState *play);

RECOMP_HOOK("EnBoom_Draw") void on_EnBoom_Draw(Actor *actor, PlayState *play) {
    sEnBoom = (EnBoom *)actor;
    sPlay = play;

    updateFinBoomerangDL_on_EnBoom_Draw(&sEnBoom->actor, sPlay);
}

RECOMP_HOOK_RETURN("EnBoom_Draw") void on_return_EnBoom_Draw(void) {

}
