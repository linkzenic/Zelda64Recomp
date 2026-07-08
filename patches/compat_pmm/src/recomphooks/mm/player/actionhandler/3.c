#include "global.h"
#include "modding.h"

static Player *sPlayer;
static PlayState *sPlay;

void fixAdultHorseMount_on_Player_ActionHandler_3(Player *player);

RECOMP_HOOK("Player_ActionHandler_3") void on_Player_ActionHandler_3(Player *player, PlayState *play) {
    sPlayer = player;
    sPlay = play;

    fixAdultHorseMount_on_Player_ActionHandler_3(sPlayer);
}

void fixAdultHorseMount_on_return_Player_ActionHandler_3(void);

RECOMP_HOOK_RETURN("Player_ActionHandler_3") void on_return_Player_ActionHandler_3(void) {
    fixAdultHorseMount_on_return_Player_ActionHandler_3();
}
