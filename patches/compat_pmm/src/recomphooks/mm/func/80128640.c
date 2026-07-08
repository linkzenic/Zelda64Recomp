#include "global.h"
#include "modding.h"

static PlayState *sPlay;
static Player *sPlayer;
static Gfx *sDList;

void fixBottleEnv_func_80128640(PlayState *play, Player *player);

RECOMP_HOOK("func_80128640") void on_func_80128640(PlayState *play, Player *player, Gfx *dList) {
    sPlay = play;
    sPlayer = player;
    sDList = dList;

    fixBottleEnv_func_80128640(sPlay, sPlayer);
}

RECOMP_HOOK_RETURN("func_80128640") void on_return_func_80128640(void) {

}
