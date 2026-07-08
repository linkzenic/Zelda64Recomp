#include "global.h"
#include "modding.h"

static Actor *sActor;
static PlayState *sPlay;

void initFormProxyExtension_on_Actor_Init(Actor *actor);

RECOMP_HOOK("Actor_Init") void on_Actor_Init(Actor *actor, PlayState *play) {
    sActor = actor;
    sPlay = play;

    initFormProxyExtension_on_Actor_Init(sActor);
}

RECOMP_HOOK_RETURN("Actor_Init") void on_return_Actor_Init(void) {
    
}
