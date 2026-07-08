#include "global.h"
#include "modding.h"

static PlayState *sPlay;
static Player *sPlayer;

void setAdultBootData_on_func_80123140(PlayState *play, Player *player);

RECOMP_HOOK("func_80123140") void on_func_80123140(PlayState *play, Player *player) {
    sPlay = play;
    sPlayer = player;

    setAdultBootData_on_func_80123140(sPlay, sPlayer);
}

void setAdultBootData_on_return_func_80123140(void);

RECOMP_HOOK_RETURN("func_80123140") void on_return_func_80123140(void) {
    setAdultBootData_on_return_func_80123140();
}
