#ifndef ZELDA_COMPAT_CHEATS_Z64EXTERN_H
#define ZELDA_COMPAT_CHEATS_Z64EXTERN_H

#include "patches.h"
#include "overlays/actors/ovl_Arms_Hook/z_arms_hook.h"

extern f32 D_8085D958[2];

u32 SurfaceType_GetData(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId, s32 dataIdx);
void ArmsHook_SetupAction(ArmsHook* this, ArmsHookActionFunc actionFunc);
void ArmsHook_Shoot(ArmsHook* this, PlayState* play);

#endif
