#include "global.h"
#include "modding.h"

static PlayState *sPlay;
static Player *sPlayer;

void hookGfx_on_Player_DrawBlastMask(PlayState *play, Player *player);

RECOMP_HOOK("Player_DrawBlastMask") void on_Player_DrawBlastMask(PlayState *play, Player *player) {
    sPlay = play;
    sPlayer = player;

    hookGfx_on_Player_DrawBlastMask(sPlay, sPlayer);
}

void hookGfx_on_return_Player_DrawBlastMask(void);

RECOMP_HOOK_RETURN("Player_DrawBlastMask") void on_return_Player_DrawBlastMask(void) {
    hookGfx_on_return_Player_DrawBlastMask();
}
