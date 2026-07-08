#include "global.h"
#include "modding.h"

static Actor *sActor;
static PlayState *sPlay;

void clearProxyExt_on_Actor_Destroy(Actor *actor);

RECOMP_HOOK("Actor_Destroy") void on_Actor_Destroy(Actor *actor, PlayState *play) {
    sActor = actor;
    sPlay = play;

    clearProxyExt_on_Actor_Destroy(sActor);
}

RECOMP_HOOK_RETURN("Actor_Destroy") void on_return_Actor_Destroy(void) {

}
