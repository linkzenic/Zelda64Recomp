#include "global.h"
#include "modding.h"

static PlayState *sPlay;

RECOMP_HOOK("Play_Draw") void on_Play_Draw(PlayState *this) {
    sPlay = this;
}

RECOMP_HOOK_RETURN("Play_Draw") void on_return_Play_Draw(void) {

}
