#include "global.h"
#include "modding.h"

static PlayState *sPlay;
static s32 sLimbIndex;
static Gfx **sDList;
static Vec3f *sPos;
static Vec3s *sRot;
static Player *sPlayer;

void addCustomFirstPersonDLs_on_Player_OverrideLimbDrawFirstPerson(Player *player, s32 limbIndex);

RECOMP_HOOK("Player_OverrideLimbDrawGameplayFirstPerson") void on_Player_OverrideLimbDrawGameplayFirstPerson(PlayState *play, s32 limbIndex, Gfx **dList, Vec3f *pos, Vec3s *rot, Actor *actor) {
    sPlay = play;
    sLimbIndex = limbIndex;
    sDList = dList;
    sPos = pos;
    sRot = rot;
    sPlayer = (Player *)actor;

    addCustomFirstPersonDLs_on_Player_OverrideLimbDrawFirstPerson(sPlayer, sLimbIndex);
}

void addCustomFirstPersonDLs_on_return_Player_OverrideLimbDrawFirstPerson(Gfx **dList);

RECOMP_HOOK_RETURN("Player_OverrideLimbDrawGameplayFirstPerson") void on_return_Player_OverrideLimbDrawGameplayFirstPerson(void) {
    addCustomFirstPersonDLs_on_return_Player_OverrideLimbDrawFirstPerson(sDList);
}
