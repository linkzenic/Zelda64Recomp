#include "global.h"
#include "modding.h"

static PlayState *sPlay;
static Player *sPlayer;

RECOMP_HOOK("Player_InitExplosiveIA") void on_Player_InitExplosiveIA(PlayState *play, Player *this) {
    sPlay = play;
    sPlayer = this;
}

void setupBombProxy_on_return_Player_InitExplosiveIA(Player *player);

RECOMP_HOOK_RETURN("Player_InitExplosiveIA") void on_return_Player_InitExplosiveIA(void) {
    setupBombProxy_on_return_Player_InitExplosiveIA(sPlayer);
}
