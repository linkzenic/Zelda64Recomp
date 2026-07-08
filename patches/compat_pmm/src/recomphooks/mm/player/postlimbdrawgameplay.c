#include "global.h"
#include "modding.h"

static PlayState *sPlay;
static s32 sLimbIndex;
static Gfx **sDList1;
static Gfx **sDList2;
static Vec3s *sRot;
static Player *sPlayer;

void handleTunicColor_on_Player_PostLimbDrawGameplay(PlayState *play, Player *player);
void handleMaskMtx_on_Player_PostLimbDrawGameplay(PlayState *play, s32 limbIndex, Gfx **dList1, Player *player);
void repositionHeldActors_on_Player_PostLimbDrawGameplay(PlayState *play, s32 limbIndex, Player *player);
void setMirrorShieldMtxF_on_Player_PostLimbDrawGameplay(Player *player, s32 limbIndex);

RECOMP_HOOK("Player_PostLimbDrawGameplay") void on_Player_PostLimbDrawGameplay(PlayState *play, s32 limbIndex, Gfx **dList1, Gfx **dList2, Vec3s *rot, Actor *actor) {
    sPlay = play;
    sLimbIndex = limbIndex;
    sDList1 = dList1;
    sDList2 = dList2;
    sRot = rot;
    sPlayer = (Player *)actor;

    handleTunicColor_on_Player_PostLimbDrawGameplay(sPlay, sPlayer);
    handleMaskMtx_on_Player_PostLimbDrawGameplay(sPlay, sLimbIndex, sDList1, sPlayer);
    repositionHeldActors_on_Player_PostLimbDrawGameplay(sPlay, sLimbIndex, sPlayer);
    setMirrorShieldMtxF_on_Player_PostLimbDrawGameplay(sPlayer, sLimbIndex);
}

void handleTunicColor_on_return_Player_PostLimbDrawGameplay(PlayState *play);
void handleMaskMtx_on_return_Player_PostLimbDrawGameplay(void);

RECOMP_HOOK_RETURN("Player_PostLimbDrawGameplay") void on_return_Player_PostLimbDrawGameplay(void) {
    handleMaskMtx_on_return_Player_PostLimbDrawGameplay();
    handleTunicColor_on_return_Player_PostLimbDrawGameplay(sPlay);
}
