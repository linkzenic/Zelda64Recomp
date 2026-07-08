#include "global.h"
#include "modding.h"

typedef struct ArmsHook ArmsHook;

static ArmsHook *sArmsHook;
static PlayState *sPlay;

void hookGfx_on_ArmsHook_Draw(ArmsHook *armsHook, PlayState *play);

RECOMP_HOOK("ArmsHook_Draw") void on_ArmsHook_Draw(Actor *thisx, PlayState *play) {
    sArmsHook = (ArmsHook *)thisx;
    sPlay = play;

    hookGfx_on_ArmsHook_Draw(sArmsHook, sPlay);
}

void hookGfx_on_return_ArmsHook_Draw(void);

RECOMP_HOOK_RETURN("ArmsHook_Draw") void on_return_ArmsHook_Draw(void) {
    hookGfx_on_return_ArmsHook_Draw();
}
