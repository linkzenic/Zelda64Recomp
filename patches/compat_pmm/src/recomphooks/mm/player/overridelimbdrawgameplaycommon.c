#include "global.h"
#include "modding.h"

static PlayState *sPlay;
static s32 sLimbIndex;
static Gfx **sDList;
static Vec3f *sPos;
static Vec3s *sRot;
static Player *sPlayer;

RECOMP_HOOK("Player_OverrideLimbDrawGameplayCommon") void on_Player_OverrideLimbDrawGameplayCommon(PlayState *play, s32 limbIndex, Gfx **dList, Vec3f *pos, Vec3s *rot, Actor *thisx) {
    sPlay = play;
    sLimbIndex = limbIndex;
    sDList = dList;
    sPos = pos;
    sRot = rot;
    sPlayer = (Player *)thisx;
}

RECOMP_HOOK_RETURN("Player_OverrideLimbDrawGameplayCommon") void on_return_Player_OverrideLimbDrawGameplayCommon(void) {

}
