#include "global.h"
#include "modding.h"

static Player *sPlayer;
static PlayState *sPlay;

void readTunicColor_on_Player_Draw(Player *player, PlayState *play);
void hookGfx_on_Player_Draw(Player *player, PlayState *play);
void updateAssets_on_Player_Draw(Player *player);
void fixAdultBremen_on_Player_Draw(Player *player, PlayState *play);
void fixCompatAdultHeight_on_Player_Draw(Player *player, PlayState *play);
void applyAdultBowProperties_on_Player_Draw(Player *player, PlayState *play);

RECOMP_HOOK("Player_Draw") void on_Player_Draw(Actor *thisx, PlayState *play) {
    sPlayer = (Player *)thisx;
    sPlay = play;

    hookGfx_on_Player_Draw(sPlayer, sPlay);
    readTunicColor_on_Player_Draw(sPlayer, sPlay);
    updateAssets_on_Player_Draw(sPlayer);
    fixCompatAdultHeight_on_Player_Draw(sPlayer, sPlay);
    fixAdultBremen_on_Player_Draw(sPlayer, sPlay);
    applyAdultBowProperties_on_Player_Draw(sPlayer, sPlay);
}

void hookGfx_on_return_Player_Draw(void);
void fixAdultBremen_on_return_Player_Draw(void);
void fixCompatAdultHeight_on_return_Player_Draw(void);
void applyAdultBowProperties_on_return_Player_Draw(void);

RECOMP_HOOK_RETURN("Player_Draw") void on_return_Player_Draw(void) {
    applyAdultBowProperties_on_return_Player_Draw();
    fixAdultBremen_on_return_Player_Draw();
    fixCompatAdultHeight_on_return_Player_Draw();
    hookGfx_on_return_Player_Draw();
}
