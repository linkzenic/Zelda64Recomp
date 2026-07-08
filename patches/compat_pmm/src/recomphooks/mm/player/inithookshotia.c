#include "global.h"
#include "modding.h"

static PlayState *sPlay;
static Player *sPlayer;

RECOMP_HOOK("Player_InitHookshotIA") void on_Player_InitHookshotIA(PlayState *play, Player *this) {
    sPlay = play;
    sPlayer = this;
}

void assignHookshotProxy_on_return_Player_InitHookshotIA(Player *player);

RECOMP_HOOK_RETURN("Player_InitHookshotIA") void on_return_Player_InitHookshotIA(void) {
    assignHookshotProxy_on_return_Player_InitHookshotIA(sPlayer);
}
