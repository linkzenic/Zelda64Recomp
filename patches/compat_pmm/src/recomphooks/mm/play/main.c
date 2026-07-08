#include "global.h"
#include "modding.h"

static PlayState *sPlay;

void updateLoggerLevel_on_Play_Main(void);

RECOMP_HOOK("Play_Main") void on_Play_Main(GameState *thisx) {
    sPlay = (PlayState *)thisx;

    updateLoggerLevel_on_Play_Main();
}

RECOMP_HOOK_RETURN("Play_Main") void on_return_Play_Main(void) {
    
}
