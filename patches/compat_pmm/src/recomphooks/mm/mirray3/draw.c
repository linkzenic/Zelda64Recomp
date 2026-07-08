#include "global.h"
#include "modding.h"

typedef struct MirRay3 MirRay3;

static MirRay3 *sMirRay3;
static PlayState *sPlay;

void overwriteShieldMtx_on_MirRay3_Draw(MirRay3 *mirRay3, PlayState *play);
void hookGfx_on_MirRay3_Draw(MirRay3 *mirRay3, PlayState *play);

RECOMP_HOOK("MirRay3_Draw") void on_MirRay3_Draw(Actor *thisx, PlayState *play) {
    sMirRay3 = (MirRay3 *)thisx;
    sPlay = play;

    overwriteShieldMtx_on_MirRay3_Draw(sMirRay3, sPlay);
    hookGfx_on_MirRay3_Draw(sMirRay3, sPlay);
}

void overwriteShieldMtx_on_return_MirRay3_Draw(void);
void hookGfx_on_return_MirRay3_Draw(void);

RECOMP_HOOK_RETURN("MirRay3_Draw") void on_return_MirRay3_Draw(void) {
    hookGfx_on_return_MirRay3_Draw();
    overwriteShieldMtx_on_return_MirRay3_Draw();
}
