#include "global.h"
#include "modding.h"

static Player *sPlayer;
static PlayState *sPlay;

RECOMP_HOOK("func_808306F8") void on_func_808306F8(Player *this, PlayState *play) {
    sPlayer = this;
    sPlay = play;
}

void setupArrowProxy_on_return_func_808306F8(Player *player);

RECOMP_HOOK_RETURN("func_808306F8") void on_return_func_808306F8(void) {
    setupArrowProxy_on_return_func_808306F8(sPlayer);
}
