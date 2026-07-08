#include "global.h"
#include "modding.h"

static GameState *sGameState;

void handleUIRequests_on_GameState_Update(GameState *state);

RECOMP_HOOK("GameState_Update") void on_GameState_Update(GameState *gameState) {
    sGameState = gameState;

    handleUIRequests_on_GameState_Update(sGameState);
}

RECOMP_HOOK_RETURN("GameState_Update") void on_return_GameState_Update(void) {

}
