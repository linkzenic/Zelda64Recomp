#include "global.h"
#include "modding.h"

typedef struct EnArrow EnArrow;

static EnArrow *sEnArrow;
static PlayState *sPlay;

void hookGfx_on_EnArrow_Draw(EnArrow *enArrow, PlayState *play);

RECOMP_HOOK("EnArrow_Draw") void on_EnArrow_Draw(Actor *thisx, PlayState *play) {
    sEnArrow = (EnArrow *)thisx;
    sPlay = play;

    hookGfx_on_EnArrow_Draw(sEnArrow, play);
}

void hookGfx_on_return_EnArrow_Draw(void);

RECOMP_HOOK_RETURN("EnArrow_Draw") void on_return_EnArrow_Draw(void) {
    hookGfx_on_return_EnArrow_Draw();
}
