#include "global.h"
#include "modding.h"

typedef struct EnRailgibud EnRailgibud;

static EnRailgibud *sEnRailgibud;
static PlayState *sPlay;

void fixEnemyHeight_on_EnRailgibud_MoveToIdealGrabPositionAndRotation(PlayState *play);

RECOMP_HOOK("EnRailgibud_MoveToIdealGrabPositionAndRotation") void on_EnRailgibud_MoveToIdealGrabPositionAndRotation(EnRailgibud *this, PlayState *play) {
    sEnRailgibud = this;
    sPlay = play;

    fixEnemyHeight_on_EnRailgibud_MoveToIdealGrabPositionAndRotation(sPlay);
}

void fixEnemyHeight_on_return_EnRailgibud_MoveToIdealGrabPositionAndRotation(void);

RECOMP_HOOK_RETURN("EnRailgibud_MoveToIdealGrabPositionAndRotation") void on_return_EnRailgibud_MoveToIdealGrabPositionAndRotation(void) {
    fixEnemyHeight_on_return_EnRailgibud_MoveToIdealGrabPositionAndRotation();
}
