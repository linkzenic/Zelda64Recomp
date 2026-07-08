#include "global.h"
#include "modding.h"
#include "playerproxy.h"
#include "proxyactorext.h"
#include "z64recomp_api.h"

void skipInterpolationBasedOnProxy_on_Actor_Draw(Actor *actor) {
    PlayerProxy *pp = ProxyActorExt_getPlayerProxy(actor);

    if (pp && PlayerProxy_shouldSkipInterpolation(pp)) {
        actor_set_interpolation_skipped(actor);
    }
}
