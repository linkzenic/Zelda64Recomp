#include "global.h"
#include "modding.h"

static Player *sPlayer;

void adjustHeight_on_Player_GetHeight(Player *player);

RECOMP_HOOK("Player_GetHeight") void on_Player_GetHeight(Player *player) {
    sPlayer = player;

    adjustHeight_on_Player_GetHeight(sPlayer);
}

void adjustHeight_on_return_Player_GetHeight(Player *player);

RECOMP_HOOK_RETURN("Player_GetHeight") void on_return_Player_GetHeight(void) {
    adjustHeight_on_return_Player_GetHeight(sPlayer);
}
