#include "global.h"
#include "modding.h"

static Player *sPlayer;
static PlayState *sPlay;

void fixAdultLinkSfx_on_Player_Action_65(Player *player);

RECOMP_HOOK("Player_Action_65") void on_Player_Action_65(Player *player, PlayState *play) {
    sPlayer = player;
    sPlay = play;

    fixAdultLinkSfx_on_Player_Action_65(sPlayer);
}

RECOMP_HOOK_RETURN("Player_Action_65") void on_return_Player_Action_65(void) {

}
