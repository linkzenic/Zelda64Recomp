#include "global.h"
#include "modding.h"

static Player *sPlayer;
static PlayState *sPlay;
static Input *sInput;

void changePlayerMass_on_Player_UpdateCommon(Player *player);

RECOMP_HOOK("Player_UpdateCommon") void on_Player_UpdateCommon(Player *this, PlayState *play, Input *input) {
    sPlayer = this;
    sPlay = play;
    sInput = input;

    changePlayerMass_on_Player_UpdateCommon(sPlayer);
}

void changePlayerMass_on_return_Player_UpdateCommon(void);
void doEponaHeightOffset_on_return_Player_UpdateCommon(Player *player);

RECOMP_HOOK_RETURN("Player_UpdateCommon") void on_return_Player_UpdateCommon(void) {
    changePlayerMass_on_return_Player_UpdateCommon();
    doEponaHeightOffset_on_return_Player_UpdateCommon(sPlayer);
}
