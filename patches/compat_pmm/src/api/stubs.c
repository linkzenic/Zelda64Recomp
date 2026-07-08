#include "global.h"
#include "modding.h"
#include "playermodelmanager_api.h"
#include "modelmatrixids.h"

// Functions that have been removed from the API

RECOMP_EXPORT bool PlayerModelManager_overrideVanillaDisplayList(unsigned long apiVersion, PlayerTransformation form, Link_DisplayList dlId, Gfx *dl) {
    return false;
}

RECOMP_EXPORT bool PlayerModelManager_overrideVanillaMatrix(unsigned long apiVersion, PlayerTransformation form, Link_EquipmentMatrix mtxId, Mtx *mtx) {
    return false;
}

RECOMP_EXPORT Gfx *PlayerModelManager_getFormDisplayList(unsigned long apiVersion, PlayerTransformation form, Link_DisplayList dlId) {
    return NULL;
}

RECOMP_EXPORT bool PlayerModelManager_isApplied(PlayerModelManagerHandle h) {
    return false;
}

RECOMP_EXPORT bool PlayerModelManager_isCustomModelApplied(PlayerTransformation form) {
    return false;
}
