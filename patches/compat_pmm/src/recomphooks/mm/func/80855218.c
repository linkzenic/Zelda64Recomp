#include "global.h"
#include "modding.h"

typedef struct struct_8085D910 struct_8085D910;

static Player *sPlayer;
static PlayState *sPlay;
static struct_8085D910 **sArg2;

void adjustTransformationVoice_on_func_80855218(Player *player);

RECOMP_HOOK("func_80855218") void on_func_80855218(PlayState *play, Player *this, struct_8085D910 **arg2) {
    sPlayer = this;
    sPlay = play;
    sArg2 = arg2;

    adjustTransformationVoice_on_func_80855218(sPlayer);
}

void adjustTransformationVoice_on_return_func_80855218(Player *player);

RECOMP_HOOK_RETURN("func_80855218") void on_return_func_80855218(void) {
    adjustTransformationVoice_on_return_func_80855218(sPlayer);
}
