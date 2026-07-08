#include "global.h"
#include "modding.h"

static Player *sPlayer;
static PlayState *sPlay;

RECOMP_HOOK("Player_UpperAction_14") void on_Player_UpperAction_14(Player *player, PlayState *play) {
    sPlayer = player;
    sPlay = play;
}

void copyProxyInfo_on_return_Player_UpperAction_14(Player *player, PlayState *play);

RECOMP_HOOK_RETURN("Player_UpperAction_14") void on_return_Player_UpperAction_14(void) {
    copyProxyInfo_on_return_Player_UpperAction_14(sPlayer, sPlay);
}
