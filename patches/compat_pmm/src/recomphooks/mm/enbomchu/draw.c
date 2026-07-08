#include "global.h"
#include "modding.h"

typedef struct EnBomChu EnBomChu;

static EnBomChu *sEnBomChu;
static PlayState *sPlay;

void hookGfx_on_EnBomChu_Draw(EnBomChu *enBomChu, PlayState *play);

RECOMP_HOOK("EnBomChu_Draw") void on_EnBomChu_Draw(Actor *thisx, PlayState *play) {
    sEnBomChu = (EnBomChu *)thisx;
    sPlay = play;

    hookGfx_on_EnBomChu_Draw(sEnBomChu, sPlay);
}

void hookGfx_on_return_EnBomChu_Draw(void);

RECOMP_HOOK_RETURN("EnBomChu_Draw") void on_return_EnBomChu_Draw(void) {
    hookGfx_on_return_EnBomChu_Draw();
}
