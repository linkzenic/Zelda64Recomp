#include "global.h"
#include "modding.h"
#include "overlays/actors/ovl_En_Test3/z_en_test3.h"

static PlayState *sPlay;
static s32 sLimbIndex;
static Gfx **sDList;
static Vec3f *sPos;
static Vec3s *sRot;
static EnTest3 *sEnTest3;

RECOMP_HOOK("EnTest3_OverrideLimbDraw") void on_EnTest3_OverrideLimbDraw(PlayState *play, s32 limbIndex, Gfx **dList, Vec3f *pos, Vec3s *rot, Actor *thisx) {
    sPlay = play;
    sLimbIndex = limbIndex;
    sDList = dList;
    sPos = pos;
    sRot = rot;
    sEnTest3 = (EnTest3 *)thisx;
}

void fixIdleAnimOddities_on_return_EnTest3_OverrideLimbDraw(EnTest3 *kafei, s32 limbIndex, Gfx **dList, Vec3s *rot);

RECOMP_HOOK_RETURN("EnTest3_OverrideLimbDraw") void on_return_EnTest3_OverrideLimbDraw(void) {
    fixIdleAnimOddities_on_return_EnTest3_OverrideLimbDraw(sEnTest3, sLimbIndex, sDList, sRot);
}
