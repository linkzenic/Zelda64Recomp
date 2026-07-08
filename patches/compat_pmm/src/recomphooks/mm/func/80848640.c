#include "global.h"
#include "modding.h"

static PlayState *sPlay;
static Player *sPlayer;

RECOMP_HOOK("func_80848640") void on_func_80848640(PlayState *play, Player *player) {
    sPlay = play;
    sPlayer = player;
}

void setupElegyStatueProxy_on_return_func_80848640(PlayState *play, Player *player);

RECOMP_HOOK_RETURN("func_80848640") void on_return_func_80848640(void) {
    setupElegyStatueProxy_on_return_func_80848640(sPlay, sPlayer);
}
