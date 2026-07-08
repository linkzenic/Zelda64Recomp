#include "global.h"
#include "modelinfo.h"
#include "modelentry.h"
#include "recompdata.h"
#include "logger.h"
#include "utils.h"
#include "ptrvalidate.h"

SETUP_PTR_VALIDATION(sValidModelInfoPtrSet, ModelInfo);

void ModelInfo_init(ModelInfo *modelInfo) {
    if (!modelInfo) {
        Logger_printError("Passed in modelInfo was NULL!");
    }

    modelInfo->modelEntryForm = NULL;
    modelInfo->gfxOverrides = recomputil_create_u32_value_hashmap();
    modelInfo->mtxOverrides = recomputil_create_u32_value_hashmap();

    ADD_VALIDATED_PTR(modelInfo);
}

void ModelInfo_destroy(ModelInfo *modelInfo) {
    RETURN_IF_INVALID_PTR(modelInfo, PTR_VAL_VOID_RET);

    modelInfo->modelEntryForm = NULL;
    recomputil_destroy_u32_value_hashmap(modelInfo->gfxOverrides);
    recomputil_destroy_u32_value_hashmap(modelInfo->mtxOverrides);

    INVALIDATE_PTR(modelInfo);
}

bool ModelInfo_hasModelEntry(ModelInfo *modelInfo) {
    RETURN_IF_INVALID_PTR(modelInfo, false);

    return modelInfo->modelEntryForm;
}

Gfx *ModelInfo_getDL(ModelInfo *modelInfo, Link_DisplayList id) {
    RETURN_IF_INVALID_PTR(modelInfo, NULL);

    if (!Utils_isValidDisplayListId(id)) {
        Logger_printWarning("Invalid display list ID %d.", id);
    }

    uintptr_t dl = 0;
    recomputil_u32_value_hashmap_get(modelInfo->gfxOverrides, id, &dl);

    if (!dl) {
        if (modelInfo->modelEntryForm) {
            dl = (uintptr_t)ModelEntry_getDisplayList(ModelEntryForm_getModelEntry(modelInfo->modelEntryForm), id);
        }
    }

    return (Gfx *)dl;
}

Mtx *ModelInfo_getMtx(ModelInfo *modelInfo, Link_EquipmentMatrix id) {
    RETURN_IF_INVALID_PTR(modelInfo, NULL);

    if (!Utils_isValidMatrixId(id)) {
        Logger_printWarning("Invalid matrix ID %d.", id);
        return NULL;
    }

    uintptr_t mtx = 0;
    recomputil_u32_value_hashmap_get(modelInfo->mtxOverrides, id, &mtx);

    if (!mtx) {
        if (modelInfo->modelEntryForm) {
            mtx = (uintptr_t)ModelEntry_getMatrix(ModelEntryForm_getModelEntry(modelInfo->modelEntryForm), id);
        }
    }

    return (Mtx *)mtx;
}

void ModelInfo_setModelEntryForm(ModelInfo *modelInfo, ModelEntryForm *modelEntry) {
    RETURN_IF_INVALID_PTR(modelInfo, PTR_VAL_VOID_RET);

    modelInfo->modelEntryForm = modelEntry;
}

ModelEntryForm *ModelInfo_getModelEntryForm(ModelInfo *modelInfo) {
    RETURN_IF_INVALID_PTR(modelInfo, NULL);

    return modelInfo->modelEntryForm;
}

TexturePtr ModelInfo_getEyesTexture(ModelInfo *modelInfo, PlayerEyeIndex i) {
    RETURN_IF_INVALID_PTR(modelInfo, NULL);

    if (i >= PLAYER_EYES_MAX || i < 0) {
        Logger_printWarning("ModelInfo_getEyesTexture called with invalid PlayerEyeIndex %d.", i);
        return NULL;
    }

    if (!modelInfo->modelEntryForm) {
        return NULL;
    }

    return ModelEntryForm_getEyesTexture(modelInfo->modelEntryForm, i);
}

TexturePtr ModelInfo_getMouthTexture(ModelInfo *modelInfo, PlayerMouthIndex i) {
    RETURN_IF_INVALID_PTR(modelInfo, NULL);

    if (i >= PLAYER_MOUTH_MAX || i < 0) {
        Logger_printWarning("ModelInfo_getMouthTexture called with invalid PlayerMouthIndex %d.", i);
        return NULL;
    }

    if (!modelInfo->modelEntryForm) {
        return NULL;
    }

    return ModelEntryForm_getMouthTexture(modelInfo->modelEntryForm, i);
}

FlexSkeletonHeader *ModelInfo_getSkeleton(ModelInfo *modelInfo) {
    RETURN_IF_INVALID_PTR(modelInfo, NULL);

    return modelInfo->modelEntryForm ? ModelEntryForm_getSkeleton(modelInfo->modelEntryForm) : NULL;
}

FlexSkeletonHeader *ModelInfo_getShieldingSkeleton(ModelInfo *modelInfo) {
    RETURN_IF_INVALID_PTR(modelInfo, NULL);

    return modelInfo->modelEntryForm ? ModelEntryForm_getShieldingSkeleton(modelInfo->modelEntryForm) : NULL;
}

bool ModelInfo_setGfxOverride(ModelInfo *modelInfo, Link_DisplayList id, Gfx *dl) {
    RETURN_IF_INVALID_PTR(modelInfo, false);

    if (!dl) {
        return ModelInfo_unsetGfxOverride(modelInfo, id);
    }

    return recomputil_u32_value_hashmap_insert(modelInfo->gfxOverrides, id, (uintptr_t)dl);
}

bool ModelInfo_unsetGfxOverride(ModelInfo *modelInfo, Link_DisplayList id) {
    RETURN_IF_INVALID_PTR(modelInfo, false);

    return recomputil_u32_value_hashmap_erase(modelInfo->gfxOverrides, id);
}

bool ModelInfo_setMtxOverride(ModelInfo *modelInfo, Link_EquipmentMatrix id, Mtx *mtx) {
    RETURN_IF_INVALID_PTR(modelInfo, false);

    return recomputil_u32_value_hashmap_insert(modelInfo->mtxOverrides, id, (uintptr_t)mtx);
}

bool ModelInfo_unsetMtxOverride(ModelInfo *modelInfo, Link_EquipmentMatrix id) {
    RETURN_IF_INVALID_PTR(modelInfo, false);

    return recomputil_u32_value_hashmap_erase(modelInfo->mtxOverrides, id);
}

void ModelInfo_clearAllGfxOverrides(ModelInfo *modelInfo) {
    RETURN_IF_INVALID_PTR(modelInfo, PTR_VAL_VOID_RET);

    for (Link_DisplayList i = 0; i < LINK_DL_MAX; ++i) {
        ModelInfo_unsetGfxOverride(modelInfo, i);
    }
}

void ModelInfo_clearAllMtxOverrides(ModelInfo *modelInfo) {
    RETURN_IF_INVALID_PTR(modelInfo, PTR_VAL_VOID_RET);

    for (Link_EquipmentMatrix i = 0; i < LINK_EQUIP_MATRIX_MAX; ++i) {
        ModelInfo_unsetMtxOverride(modelInfo, i);
    }
}

bool ModelInfo_isAnyFlagEnabled(ModelInfo *modelInfo, u64 flags) {
    RETURN_IF_INVALID_PTR(modelInfo, false);

    return modelInfo->modelEntryForm ? (ModelEntry_isAnyFlagEnabled(ModelEntryForm_getModelEntry(modelInfo->modelEntryForm), flags)) : false;
}
