#ifndef API_LOCAL_H
#define API_LOCAL_H

#include "PR/ultratypes.h"
#include "PR/gbi.h"
#include "z64player.h"
#include "playermodelmanager_api.h"
#include "modelmatrixids.h"

PlayerModelManagerHandle PlayerModelManager_registerModel(unsigned long apiVersion, const char *internalName, PlayerModelManagerModelType modelType);
bool PlayerModelManager_setDisplayName(PlayerModelManagerHandle h, const char *displayName);
bool PlayerModelManager_setAuthor(PlayerModelManagerHandle h, const char *author);
bool PlayerModelManager_setFlags(PlayerModelManagerHandle h, u64 flags);
bool PlayerModelManager_clearFlags(PlayerModelManagerHandle h, u64 flags);
bool PlayerModelManager_clearAllFlags(PlayerModelManagerHandle h, u64 flags);
bool PlayerModelManager_setDisplayList(PlayerModelManagerHandle h, Link_DisplayList dlId, Gfx *dl);
bool PlayerModelManager_setDisplayListForModelType(PlayerModelManagerHandle h, PlayerModelManagerModelType type, Link_DisplayList dlId, Gfx *dl);
bool PlayerModelManager_setMatrix(PlayerModelManagerHandle h, Link_EquipmentMatrix mtxId, Mtx *matrix);
bool PlayerModelManager_setMatrixForModelType(PlayerModelManagerHandle h, PlayerModelManagerModelType type, Link_EquipmentMatrix mtxId, Mtx *matrix);
bool PlayerModelManager_addHandleToPack(PlayerModelManagerHandle h, PlayerModelManagerHandle toAdd);
bool PlayerModelManager_setCallback(PlayerModelManagerHandle h, PlayerModelManagerEventHandler *callback, void *userdata);
bool PlayerModelManager_setSkeleton(PlayerModelManagerHandle h, FlexSkeletonHeader *skel);
bool PlayerModelManager_setShieldingSkeleton(PlayerModelManagerHandle h, FlexSkeletonHeader *skel);
bool PlayerModelManager_setEyesTextures(PlayerModelManagerHandle h, TexturePtr eyesTextures[]);
bool PlayerModelManager_setMouthTextures(PlayerModelManagerHandle h, TexturePtr mouthTextures[]);
Gfx *PlayerModelManager_getFormDisplayList(unsigned long apiVersion, PlayerTransformation form, Link_DisplayList dlId);
bool PlayerModelManager_overrideVanillaDisplayList(unsigned long apiVersion, PlayerTransformation form, Link_DisplayList dlId, Gfx *dl);
bool PlayerModelManager_overrideVanillaMatrix(unsigned long apiVersion, PlayerTransformation form, Link_EquipmentMatrix mtxId, Mtx *mtx);
bool PlayerModelManager_isApplied(PlayerModelManagerHandle h);
void PlayerModelManager_requestOverrideTunicColor(u8 r, u8 g, u8 b, u8 a);
void PlayerModelManager_requestOverrideFormTunicColor(PlayerTransformation form, u8 r, u8 g, u8 b, u8 a);
bool PlayerModelManager_isCustomModelApplied(PlayerTransformation form);
void PlayerModelManager_lockAPI(void);
void PlayerModelManager_unlockAPI(void);

#endif
