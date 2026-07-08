#include "global.h"
#include "modding.h"

static PlayState *sPlay;
static Player *sPlayer;

void fixFPCamera_on_func_8083868C(Player *this);

RECOMP_HOOK("func_8083868C") void on_func_8083868C(PlayState *play, Player *this) {
    sPlay = play;
    sPlayer = this;

    fixFPCamera_on_func_8083868C(sPlayer);
}

void fixFPCamera_on_return_func_8083868C(Player *player);

RECOMP_HOOK_RETURN("func_8083868C") void on_return_func_8083868C(void) {
    fixFPCamera_on_return_func_8083868C(sPlayer);
}
