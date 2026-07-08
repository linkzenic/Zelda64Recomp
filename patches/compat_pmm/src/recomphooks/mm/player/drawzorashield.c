#include "global.h"
#include "modding.h"

static PlayState *sPlay;
static Player *sPlayer;

void updateZoraMagicBarrier_on_Player_DrawZoraShield(Player *player);
void hookGfx_on_Player_DrawZoraShield(PlayState *play, Player *player);

RECOMP_HOOK("Player_DrawZoraShield") void on_Player_DrawZoraShield(PlayState *play, Player *player) {
    sPlay = play;
    sPlayer = player;

    updateZoraMagicBarrier_on_Player_DrawZoraShield(sPlayer);
    hookGfx_on_Player_DrawZoraShield(sPlay, sPlayer);
}

void hookGfx_on_return_Player_DrawZoraShield(void);

RECOMP_HOOK_RETURN("Player_DrawZoraShield") void on_return_Player_DrawZoraShield(void) {
    hookGfx_on_return_Player_DrawZoraShield();
}
