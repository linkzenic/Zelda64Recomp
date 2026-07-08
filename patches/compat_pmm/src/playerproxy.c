#include "global.h"
#include "playerproxy.h"
#include "modelinfo.h"
#include "formproxy.h"
#include "logger.h"
#include "playermodelconfig.h"
#include "modelentry.h"
#include "utils.h"
#include "ptrvalidate.h"
#include "../../misc_funcs.h"

// this function should only be called by PlayerProxy
void FormProxy_requestRefresh(FormProxy *fp);

PlayerProxy *gPlayer1Proxy;
PlayerProxy *gPlayer2Proxy;

ModelInfo gHumanModelInfoFallbackOverride;
ModelInfo gDekuModelInfoFallbackOverride;
ModelInfo gGoronModelInfoFallbackOverride;
ModelInfo gZoraModelInfoFallbackOverride;
ModelInfo gFierceDeityModelInfoFallbackOverride;
ModelInfo gHumanModelInfo;
ModelInfo gDekuModelInfo;
ModelInfo gGoronModelInfo;
ModelInfo gZoraModelInfo;
ModelInfo gFierceDeityModelInfo;

static YAZMTCore_IterableU32Set *sQueuedRefreshes;

SETUP_PTR_VALIDATION(sPlayerProxyPtrSet, PlayerProxy);

FormProxy *PlayerProxy_getFormProxy(PlayerProxy *pp, FormProxyId formId) {
    RETURN_IF_INVALID_PTR(pp, NULL);

    return recomputil_u32_memory_hashmap_get(pp->formProxies, formId);
}

void PlayerProxy_createFormProxy(PlayerProxy *pp, FormProxyId proxyId, PlayerTransformation form, ModelInfo *fallback, ModelInfo *fallbackOverride) {
    RETURN_IF_INVALID_PTR(pp, PTR_VAL_VOID_RET);

    if (recomputil_u32_memory_hashmap_create(pp->formProxies, proxyId)) {
        FormProxy *fp = PlayerProxy_getFormProxy(pp, proxyId);
        FormProxy_init(fp, pp, form, proxyId, fallback, fallbackOverride);
    } else {
        Logger_printWarning("Tried to create a FormProxy, but key %d already existed!", proxyId);
    }
}

void PlayerProxy_init(PlayerProxy *pp) {
    if (!pp) {
        Logger_printError("Passed in a NULL pointer!");
        return;
    }

    ADD_VALIDATED_PTR(pp);

    pp->formProxies = recomputil_create_u32_memory_hashmap(sizeof(FormProxy));
    pp->modelEntries = recomputil_create_u32_value_hashmap();
    pp->currentlyEquippedEntries = YAZMTCore_IterableU32Set_new();
    pp->interpStatus = PP_INTERP_STAT_NO_SKIP;

    PlayerProxy_createFormProxy(pp, FORM_PROXY_ID_HUMAN, PLAYER_FORM_HUMAN, &gHumanModelInfo, &gHumanModelInfoFallbackOverride);
    PlayerProxy_createFormProxy(pp, FORM_PROXY_ID_DEKU, PLAYER_FORM_DEKU, &gDekuModelInfo, &gDekuModelInfoFallbackOverride);
    PlayerProxy_createFormProxy(pp, FORM_PROXY_ID_GORON, PLAYER_FORM_GORON, &gGoronModelInfo, &gGoronModelInfoFallbackOverride);
    PlayerProxy_createFormProxy(pp, FORM_PROXY_ID_ZORA, PLAYER_FORM_ZORA, &gZoraModelInfo, &gZoraModelInfoFallbackOverride);
    PlayerProxy_createFormProxy(pp, FORM_PROXY_ID_FIERCE_DEITY, PLAYER_FORM_FIERCE_DEITY, &gFierceDeityModelInfo, &gFierceDeityModelInfoFallbackOverride);
}

void PlayerProxy_destroy(PlayerProxy *pp) {
    RETURN_IF_INVALID_PTR(pp, PTR_VAL_VOID_RET);

    YAZMTCore_IterableU32Set_erase(sQueuedRefreshes, (uintptr_t)pp);

    for (size_t i = 0; i < gFormProxyIds->size; ++i) {
        FormProxy *fp = PlayerProxy_getFormProxy(pp, i);

        if (fp) {
            FormProxy_destroy(fp);
            recomputil_u32_memory_hashmap_erase(pp->formProxies, i);
        }
    }

    recomputil_destroy_u32_memory_hashmap(pp->formProxies);
    pp->formProxies = 0;
    recomputil_destroy_u32_value_hashmap(pp->modelEntries);
    pp->modelEntries = 0;
    YAZMTCore_IterableU32Set_destroy(pp->currentlyEquippedEntries);
    pp->currentlyEquippedEntries = NULL;

    INVALIDATE_PTR(pp);
}

static void PlayerProxy_refresh(PlayerProxy *pp) {
    RETURN_IF_INVALID_PTR(pp, PTR_VAL_VOID_RET);

    for (size_t i = 0; i < gFormProxyIds->size; ++i) {
        FormProxy *currFp = PlayerProxy_getFormProxy(pp, gFormProxyIds->ids[i]);

        if (currFp) {
            FormProxy_requestRefresh(currFp);
        }
    }

    YAZMTCore_IterableU32Set_clear(pp->currentlyEquippedEntries);

    for (PlayerModelManagerModelType type = 0; type < PMM_MODEL_TYPE_MAX; ++type) {
        uintptr_t entry = 0;
        recomputil_u32_value_hashmap_get(pp->modelEntries, type, &entry);
        if (entry) {
            YAZMTCore_IterableU32Set_insert(pp->currentlyEquippedEntries, entry);
        }
    }

    PlayerProxy_requestInterpolationSkip(pp);
}

void PlayerProxy_requestRefresh(PlayerProxy *pp) {
    RETURN_IF_INVALID_PTR(pp, PTR_VAL_VOID_RET);

    if (pp && sQueuedRefreshes) {
        YAZMTCore_IterableU32Set_insert(sQueuedRefreshes, (uintptr_t)pp);
#if defined(ZELDA_ANDROID_BUILTIN_PMM)
        recomp_android_compat_pmm_apply_log(30, (uintptr_t)pp, 0, 0, "PlayerProxy_requestRefresh");
#endif
    }
}

void PlayerProxy_setOverrideDL(PlayerProxy *pp, Link_DisplayList dlId, Gfx *dl) {
    RETURN_IF_INVALID_PTR(pp, PTR_VAL_VOID_RET);

    for (size_t i = 0; i < gFormProxyIds->size; ++i) {
        FormProxy *currFp = PlayerProxy_getFormProxy(pp, gFormProxyIds->ids[i]);

        if (currFp) {
            FormProxy_setCurrentOverrideDL(currFp, dlId, dl);
        }
    }
}

void PlayerProxy_setOverrideMtx(PlayerProxy *pp, Link_EquipmentMatrix mtxId, Mtx *mtx) {
    RETURN_IF_INVALID_PTR(pp, PTR_VAL_VOID_RET);

    for (size_t i = 0; i < gFormProxyIds->size; ++i) {
        FormProxy *currFp = PlayerProxy_getFormProxy(pp, gFormProxyIds->ids[i]);

        if (currFp) {
            FormProxy_setCurrentOverrideMtx(currFp, mtxId, mtx);
        }
    }
}

bool PlayerProxy_getProxyIdFromForm(PlayerTransformation form, FormProxyId *out) {
    if (!out) {
        return false;
    }

    switch (form) {
        case PLAYER_FORM_FIERCE_DEITY:
            *out = FORM_PROXY_ID_FIERCE_DEITY;
            break;

        case PLAYER_FORM_GORON:
            *out = FORM_PROXY_ID_GORON;
            break;

        case PLAYER_FORM_ZORA:
            *out = FORM_PROXY_ID_ZORA;
            break;

        case PLAYER_FORM_DEKU:
            *out = FORM_PROXY_ID_DEKU;
            break;

        case PLAYER_FORM_HUMAN:
            *out = FORM_PROXY_ID_HUMAN;
            break;

        default:
            return false;
            break;
    }

    return true;
}

void PlayerProxy_requestTunicColorOverride(PlayerProxy *pp, Color_RGBA8 color) {
    RETURN_IF_INVALID_PTR(pp, PTR_VAL_VOID_RET);

    for (size_t i = 0; i < gFormProxyIds->size; ++i) {
        FormProxy *currFp = PlayerProxy_getFormProxy(pp, gFormProxyIds->ids[i]);

        if (currFp) {
            FormProxy_requestTunicColorOverride(currFp, color);
        }
    }
}

bool isCompatibleModelTypes(PlayerModelManagerModelType a, PlayerModelManagerModelType b) {
    if (a == PMM_MODEL_TYPE_NONE || b == PMM_MODEL_TYPE_NONE) {
        return false;
    }

    return a == b ||
           (a == PMM_MODEL_TYPE_CHILD && b == PMM_MODEL_TYPE_ADULT) ||
           (a == PMM_MODEL_TYPE_ADULT && b == PMM_MODEL_TYPE_CHILD);
}

static void PlayerProxy_setModelEntry(PlayerProxy *pp, PlayerModelManagerModelType modelType, const ModelEntry *modelEntry) {
    RETURN_IF_INVALID_PTR(pp, PTR_VAL_VOID_RET);

    if (!Utils_isValidModelType(modelType)) {
        Logger_printError("Passed in invalid model type %d", modelType);
        return;
    }

    if (!modelEntry) {
        recomputil_u32_value_hashmap_erase(pp->modelEntries, modelType);
        return;
    }

    PlayerModelManagerModelType entryType = ModelEntry_getType(modelEntry);

    if (!isCompatibleModelTypes(modelType, entryType)) {
        Logger_printError("Mismatched modelType and modelEntry type! modelType: %d, entryType: %d", modelType, entryType);
        return;
    }

    if (Utils_isPackModelType(modelType)) {
        return;
    }

    recomputil_u32_value_hashmap_insert(pp->modelEntries, modelType, (uintptr_t)modelEntry);
}

const ModelEntry *PlayerProxy_getCurrentEntry(PlayerProxy *pp, PlayerModelManagerModelType modelType) {
    RETURN_IF_INVALID_PTR(pp, NULL);

    uintptr_t entry = 0;
    recomputil_u32_value_hashmap_get(pp->modelEntries, modelType, &entry);
    return (const ModelEntry *)entry;
}

static FormProxy *getFormProxyFromCategory(PlayerProxy *pp, PlayerModelManagerModelType modelType) {
    RETURN_IF_INVALID_PTR(pp, NULL);

    FormProxyId fpId;

    switch (modelType) {
        case PMM_MODEL_TYPE_DEKU:
            fpId = FORM_PROXY_ID_DEKU;
            break;

        case PMM_MODEL_TYPE_GORON:
            fpId = FORM_PROXY_ID_GORON;
            break;

        case PMM_MODEL_TYPE_ZORA:
            fpId = FORM_PROXY_ID_ZORA;
            break;

        case PMM_MODEL_TYPE_FIERCE_DEITY:
            fpId = FORM_PROXY_ID_FIERCE_DEITY;
            break;

        default:
            fpId = FORM_PROXY_ID_HUMAN;
            break;
    }

    return PlayerProxy_getFormProxy(pp, fpId);
}

static void reapplyAllEquipmentEntries(PlayerProxy *pp) {
    RETURN_IF_INVALID_PTR(pp, PTR_VAL_VOID_RET);

    for (PlayerModelManagerModelType i = 0; i < PMM_MODEL_TYPE_MAX; ++i) {
        if (Utils_isEquipmentModelType(i)) {
            const ModelEntry *tmp = PlayerProxy_getCurrentEntry(pp, i);
            PlayerProxy_removeEntry(pp, i);
            PlayerProxy_tryApplyEntry(pp, i, tmp);
        }
    }
}

bool PlayerProxy_forceApplyEntry(PlayerProxy *pp, PlayerModelManagerModelType modelType, const ModelEntry *newEntry) {
    RETURN_IF_INVALID_PTR(pp, false);

    const ModelEntry *currEntry = PlayerProxy_getCurrentEntry(pp, modelType);

    if (newEntry == NULL) {
        PlayerProxy_removeEntry(pp, modelType);
        return true;
    }

    FormProxy *targetFp = getFormProxyFromCategory(pp, modelType);

    if (ModelEntry_applyToFormProxy(newEntry, targetFp)) {
        PlayerModelManagerModelEvent removedEvt;
        PlayerModelManagerModelEvent appliedEvt;

        if (gPlayer1Proxy == pp) {
            appliedEvt = PMM_EVENT_MODEL_APPLIED_TO_MAIN_PLAYER;
            removedEvt = PMM_EVENT_MODEL_REMOVED_FROM_MAIN_PLAYER;
        } else {
            appliedEvt = PMM_EVENT_MODEL_APPLIED_TO_OTHER;
            removedEvt = PMM_EVENT_MODEL_REMOVED_FROM_OTHER;
        }

        if (currEntry) {
            ModelEntry_doCallback(currEntry, removedEvt);
        }

        PlayerProxy_setModelEntry(pp, modelType, newEntry);

        if (newEntry) {
            ModelEntry_doCallback(newEntry, appliedEvt);
        }

        if (modelType == PMM_MODEL_TYPE_CHILD || modelType == PMM_MODEL_TYPE_ADULT) {
            reapplyAllEquipmentEntries(pp);
        }

#if defined(ZELDA_ANDROID_BUILTIN_PMM)
        {
            const char *entryName = ModelEntry_getInternalName(newEntry);
            s32 fpState = (targetFp ? 1 : 0) | ((targetFp && FormProxy_isAdultModelType(targetFp)) ? 2 : 0);
            recomp_android_compat_pmm_apply_log(
                64,
                modelType,
                ModelEntry_getType(newEntry),
                fpState,
                entryName ? entryName : "PlayerProxy_forceApplyEntry");
        }
#endif

        return true;
    }

    return false;
}

bool PlayerProxy_tryApplyEntry(PlayerProxy *pp, PlayerModelManagerModelType modelType, const ModelEntry *newEntry) {
    RETURN_IF_INVALID_PTR(pp, false);

    const ModelEntry *currEntry = PlayerProxy_getCurrentEntry(pp, modelType);
    if (newEntry != currEntry) {
        if (newEntry) {
            PlayerModelManagerModelType t = ModelEntry_getType(newEntry);
            if (!isCompatibleModelTypes(t, modelType)) {
                Logger_printError("ModelEntry passed in with model type %d not compatible with model type %d. Not applying!", modelType, t);
                return false;
            }
        }

        return PlayerProxy_forceApplyEntry(pp, modelType, newEntry);
    }

    return false;
}

void PlayerProxy_removeEntry(PlayerProxy *pp, PlayerModelManagerModelType modelType) {
    RETURN_IF_INVALID_PTR(pp, PTR_VAL_VOID_RET);

    if (!Utils_isValidModelType(modelType)) {
        Logger_printError("Called with invalid category %d\n", modelType);
        Utils_tryCrashGame();
        return;
    }

    const ModelEntry *entry = PlayerProxy_getCurrentEntry(pp, modelType);

    if (entry) {
        ModelEntry_doCallback(entry, pp == gPlayer1Proxy ? PMM_EVENT_MODEL_REMOVED_FROM_MAIN_PLAYER : PMM_EVENT_MODEL_REMOVED_FROM_OTHER);

        PlayerProxy_setModelEntry(pp, modelType, NULL);

        ModelEntry_removeFromFormProxy(entry, getFormProxyFromCategory(pp, modelType));

        if (modelType == PMM_MODEL_TYPE_CHILD || modelType == PMM_MODEL_TYPE_ADULT) {
            reapplyAllEquipmentEntries(pp);
        }
    }
}

bool PlayerProxy_isModelEntryApplied(PlayerProxy *pp, const ModelEntry *entry) {
    RETURN_IF_INVALID_PTR(pp, false);

    return YAZMTCore_IterableU32Set_contains(pp->currentlyEquippedEntries, (uintptr_t)entry);
}

bool PlayerProxy_shouldSkipInterpolation(const PlayerProxy *pp) {
    RETURN_IF_INVALID_PTR(pp, false);

    return pp->interpStatus != PP_INTERP_STAT_NO_SKIP;
}

void PlayerProxy_updateInterpolationStatus(PlayerProxy *pp) {
    if (pp->interpStatus == PP_INTERP_STAT_SKIPPING) {
        pp->interpStatus = PP_INTERP_STAT_RESUME_NEXT_FRAME;
    } else if (pp->interpStatus == PP_INTERP_STAT_RESUME_NEXT_FRAME) {
        pp->interpStatus = PP_INTERP_STAT_NO_SKIP;
    }
}

void PlayerProxy_requestInterpolationSkip(PlayerProxy *pp) {
    RETURN_IF_INVALID_PTR(pp, PTR_VAL_VOID_RET);

    pp->interpStatus = PP_INTERP_STAT_SKIPPING;
}

RECOMP_CALLBACK(".", _internal_initHashObjects) void initPlayerProxyHash(void) {
    sQueuedRefreshes = YAZMTCore_IterableU32Set_new();
}

RECOMP_CALLBACK(".", _internal_postInitHashObjects) void initPlayerProxyVars(void) {
    ModelInfo_init(&gHumanModelInfoFallbackOverride);
    ModelInfo_init(&gDekuModelInfoFallbackOverride);
    ModelInfo_init(&gGoronModelInfoFallbackOverride);
    ModelInfo_init(&gZoraModelInfoFallbackOverride);
    ModelInfo_init(&gFierceDeityModelInfoFallbackOverride);
    ModelInfo_init(&gHumanModelInfo);
    ModelInfo_init(&gDekuModelInfo);
    ModelInfo_init(&gGoronModelInfo);
    ModelInfo_init(&gZoraModelInfo);
    ModelInfo_init(&gFierceDeityModelInfo);
}

static void requestProcessor(void *pp) {
#if defined(ZELDA_ANDROID_BUILTIN_PMM)
    recomp_android_compat_pmm_apply_log(31, (uintptr_t)pp, 0, 0, "PlayerProxy_refresh");
#endif
    PlayerProxy_refresh(pp);
}

void processPlayerProxyRefreshRequests_on_Play_UpdateMain(void) {
    if (!sQueuedRefreshes) {
        return;
    }

    Utils_processIterableSetPtrs(sQueuedRefreshes, requestProcessor);
    YAZMTCore_IterableU32Set_clear(sQueuedRefreshes);
}
