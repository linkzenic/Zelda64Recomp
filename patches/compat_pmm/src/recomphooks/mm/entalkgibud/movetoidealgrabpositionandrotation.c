#include "global.h"
#include "modding.h"

typedef struct EnTalkGibud EnTalkGibud;

static EnTalkGibud *sEnTalkGibud;
static PlayState *sPlay;

void fixEnemyHeight_on_EnTalkgibud_MoveToIdealGrabPositionAndRotation(PlayState *play);

RECOMP_HOOK("EnTalkGibud_MoveToIdealGrabPositionAndRotation") void on_EnTalkGibud_MoveToIdealGrabPositionAndRotation(EnTalkGibud *this, PlayState *play) {
    sEnTalkGibud = this;
    sPlay = play;

    fixEnemyHeight_on_EnTalkgibud_MoveToIdealGrabPositionAndRotation(sPlay);
}

void fixEnemyHeight_on_return_EnTalkgibud_MoveToIdealGrabPositionAndRotation(void);

RECOMP_HOOK_RETURN("EnTalkGibud_MoveToIdealGrabPositionAndRotation") void on_return_EnTalkGibud_MoveToIdealGrabPositionAndRotation(void) {
    fixEnemyHeight_on_return_EnTalkgibud_MoveToIdealGrabPositionAndRotation();
}
