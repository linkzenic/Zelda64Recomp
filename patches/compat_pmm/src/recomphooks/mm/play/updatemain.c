#include "global.h"
#include "modding.h"

static PlayState *sPlay;

void updatePlayerProxyManager_on_UpdateMain(void);
void updateAdultProperties_on_Play_UpdateMain(PlayState *play);
void processPlayerProxyRefreshRequests_on_Play_UpdateMain(void);
void processFormProxyRefreshRequests_on_Play_UpdateMain(void);

RECOMP_HOOK("Play_UpdateMain") void on_Play_UpdateMain(PlayState *this) {
    sPlay = this;

    updatePlayerProxyManager_on_UpdateMain();
    updateAdultProperties_on_Play_UpdateMain(sPlay);
    processPlayerProxyRefreshRequests_on_Play_UpdateMain();
    processFormProxyRefreshRequests_on_Play_UpdateMain();
}

RECOMP_HOOK_RETURN("Play_UpdateMain") void on_return_Play_UpdateMain(void) {

}
