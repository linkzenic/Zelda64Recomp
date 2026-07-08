#include "global.h"
#include "modding.h"
#include "overlays/actors/ovl_En_Torch2/z_en_torch2.h"

static EnTorch2 *sEnTorch2;
static PlayState *sPlay;

void replaceElegyStatue_on_EnTorch2_Draw(Actor *actor, PlayState *play);

RECOMP_HOOK("EnTorch2_Draw") void on_EnTorch2_Draw(Actor *thisx, PlayState *play) {
    sEnTorch2 = (EnTorch2 *)thisx;
    sPlay = play;

    replaceElegyStatue_on_EnTorch2_Draw(&sEnTorch2->actor, sPlay);
}

RECOMP_HOOK_RETURN("EnTorch2_Draw") void on_return_EnTorch2_Draw(void) {

}
