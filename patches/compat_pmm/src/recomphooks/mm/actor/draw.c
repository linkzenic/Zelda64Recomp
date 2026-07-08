#include "global.h"
#include "modding.h"

static PlayState *sPlay;
static Actor *sActor;

void skipInterpolationBasedOnProxy_on_Actor_Draw(Actor *actor);

RECOMP_HOOK("Actor_Draw") void on_Actor_Draw(PlayState *play, Actor *actor) {
    sActor = actor;
    sPlay = play;

    skipInterpolationBasedOnProxy_on_Actor_Draw(sActor);
}

RECOMP_HOOK_RETURN("Actor_Draw") void on_return_Actor_Draw(void) {

}
