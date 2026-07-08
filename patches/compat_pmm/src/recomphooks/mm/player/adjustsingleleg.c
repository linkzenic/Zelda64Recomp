#include "global.h"
#include "modding.h"

static PlayState *sPlay;
static Player *sPlayer;
static SkelAnime *sSkelAnime;
Vec3f *sPos;
Vec3s *sRot;
s32 sThighLimbIndex;
s32 sShinLimbIndex;
s32 sFootLimbIndex;

void adjustAdultLeg_on_Player_AdjustSingleLeg(Player *player);

RECOMP_HOOK("Player_AdjustSingleLeg") void on_Player_AdjustSingleLeg(PlayState *play, Player *player, SkelAnime *skelAnime, Vec3f *pos, Vec3s *rot, s32 thighLimbIndex, s32 shinLimbIndex, s32 footLimbIndex) {
    sPlay = play;
    sPlayer = player;
    sSkelAnime = skelAnime;
    sPos = pos;
    sRot = rot;
    sThighLimbIndex = thighLimbIndex;
    sShinLimbIndex = shinLimbIndex;
    sFootLimbIndex = footLimbIndex;

    adjustAdultLeg_on_Player_AdjustSingleLeg(sPlayer);
}

void adjustAdultLeg_on_return_Player_AdjustSingleLeg(void);

RECOMP_HOOK_RETURN("Player_AdjustSingleLeg") void on_return_Player_AdjustSingleLeg(void) {
    adjustAdultLeg_on_return_Player_AdjustSingleLeg();
}
