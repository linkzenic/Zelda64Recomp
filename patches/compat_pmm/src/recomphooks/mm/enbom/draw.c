#include "global.h"
#include "modding.h"

typedef struct EnBom EnBom;

static EnBom *sEnBom;
static PlayState *sPlay;

void hookGfx_on_EnBom_Draw(EnBom *enBom, PlayState *play);
void draw3DBombBody_on_EnBom_Draw(EnBom *enBom, PlayState *play);

RECOMP_HOOK("EnBom_Draw") void on_EnBom_Draw(Actor *thisx, PlayState *play) {
    sEnBom = (EnBom *)thisx;
    sPlay = play;

    hookGfx_on_EnBom_Draw(sEnBom, sPlay);
    draw3DBombBody_on_EnBom_Draw(sEnBom, sPlay);
}

void hookGfx_on_return_EnBom_Draw(void);

RECOMP_HOOK_RETURN("EnBom_Draw") void on_return_EnBom_Draw(void) {
    hookGfx_on_return_EnBom_Draw();
}
