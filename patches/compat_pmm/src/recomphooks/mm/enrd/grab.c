#include "global.h"
#include "modding.h"

typedef struct EnRd EnRd;

static EnRd *sEnRd;
static PlayState *sPlay;

void fixEnemyHeight_on_EnRd_Grab(PlayState *play);

RECOMP_HOOK("EnRd_Grab") void on_EnRd_Grab(EnRd *this, PlayState *play) {
    sEnRd = this;
    sPlay = play;

    fixEnemyHeight_on_EnRd_Grab(sPlay);
}

void fixEnemyHeight_on_return_EnRd_Grab(void);

RECOMP_HOOK_RETURN("EnRd_Grab") void on_return_EnRd_Grab(void) {
    fixEnemyHeight_on_return_EnRd_Grab();
}
