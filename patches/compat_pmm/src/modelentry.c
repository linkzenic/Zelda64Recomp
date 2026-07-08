#include "global.h"
#include "modelentry.h"
#include "recomputils.h"
#include "utils.h"
#include "equipmentoverrides.h"
#include "playerproxy.h"
#include "formproxy.h"
#include "yazmtcorelib_api.h"
#include "logger.h"
#include "defaultfacetex.h"
#include "libc/string.h"
#include "playermodelconfig.h"
#include "ptrvalidate.h"
#include "../../misc_funcs.h"

SETUP_PTR_VALIDATION(sValidModelEntries, ModelEntry);

typedef struct ModelVisuals {
    U32ValueHashmapHandle displayListPtrs;
    U32ValueHashmapHandle matrixPtrs;
} ModelVisuals;

static void ModelVisuals_init(ModelVisuals *modelVisuals) {
    modelVisuals->displayListPtrs = recomputil_create_u32_value_hashmap();
    modelVisuals->matrixPtrs = recomputil_create_u32_value_hashmap();
}

static void ModelVisuals_setDL(ModelVisuals *modelVisuals, Link_DisplayList dlId, Gfx *dl) {
    if (dl) {
        recomputil_u32_value_hashmap_insert(modelVisuals->displayListPtrs, dlId, (uintptr_t)dl);
    } else {
        recomputil_u32_value_hashmap_erase(modelVisuals->displayListPtrs, dlId);
    }
}

static Gfx *ModelVisuals_getDL(const ModelVisuals *modelVisuals, Link_DisplayList id) {
    uintptr_t out = (uintptr_t)NULL;

    recomputil_u32_value_hashmap_get(modelVisuals->displayListPtrs, id, &out);

    return (Gfx *)out;
}

static void ModelVisuals_setMtx(ModelVisuals *modelVisuals, Link_EquipmentMatrix id, Mtx *mtx) {
    if (mtx) {
        recomputil_u32_value_hashmap_insert(modelVisuals->matrixPtrs, id, (uintptr_t)mtx);
    } else {
        recomputil_u32_value_hashmap_erase(modelVisuals->matrixPtrs, id);
    }
}

static Mtx *ModelVisuals_getMtx(const ModelVisuals *modelVisuals, Link_EquipmentMatrix id) {
    uintptr_t out = (uintptr_t)NULL;

    recomputil_u32_value_hashmap_get(modelVisuals->matrixPtrs, id, &out);

    return (Mtx *)out;
}

typedef struct ModelEntryFunctions {
    bool (*applyToFormProxy)(const struct ModelEntry *entry, FormProxy *fp);
    bool (*removeFromFormProxy)(const struct ModelEntry *entry, FormProxy *fp);
    bool (*setDisplayList)(struct ModelEntry *entry, Link_DisplayList id, Gfx *dl);
    Gfx *(*getDisplayList)(const struct ModelEntry *entry, Link_DisplayList id);
    bool (*setMatrix)(struct ModelEntry *entry, Link_EquipmentMatrix id, Mtx *mtx);
    Mtx *(*getMatrix)(const struct ModelEntry *entry, Link_EquipmentMatrix id);
} ModelEntryVirtualFunctions;

typedef struct ModelEntry {
    const ModelEntryVirtualFunctions *virtualFuncs;
    PlayerModelManagerModelType type;
    char *displayName;
    char *internalName;
    char *authorName;
    PlayerModelManagerEventHandler *callback;
    void *callbackData;
    PlayerModelManagerHandle handle;
    u64 flags;
} ModelEntry;

typedef struct ModelEntryForm {
    ModelEntry modelEntry;
    ModelVisuals modelVisuals;
    FlexSkeletonHeader *skel;
    FlexSkeletonHeader *shieldingSkel;
    TexturePtr eyesTex[PLAYER_EYES_MAX];
    TexturePtr mouthTex[PLAYER_MOUTH_MAX];
} ModelEntryForm;

typedef struct ModelEntryEquipment {
    ModelEntry modelEntry;
    ModelVisuals fallbackModelVisuals;
    U32ValueHashmapHandle modelTypeToModelVisuals;
} ModelEntryEquipment;

typedef struct ModelEntryPack {
    ModelEntry modelEntry;
    YAZMTCore_IterableU32Set *modelEntries;
} ModelEntryPack;

const char FORMATTED_DEFAULT_VIRTUAL_FUNCTION_OVERRIDE_ERROR[] = "Model Entry with internal name %s is using function that should have been overriden!\n";

static bool ModelEntry_applyToFormProxy_default(const ModelEntry *entry, FormProxy *fp) {
    Logger_printError(FORMATTED_DEFAULT_VIRTUAL_FUNCTION_OVERRIDE_ERROR, entry->internalName ? entry->internalName : "[UNKNOWN]");
    return false;
}

static bool ModelEntry_removeFromFormProxy_default(const ModelEntry *entry, FormProxy *fp) {
    Logger_printError(FORMATTED_DEFAULT_VIRTUAL_FUNCTION_OVERRIDE_ERROR, entry->internalName ? entry->internalName : "[UNKNOWN]");
    return false;
}

static bool ModelEntry_setDisplayList_default(ModelEntry *entry, Link_DisplayList id, Gfx *dl) {
    return false;
}

static Gfx *ModelEntry_getDisplayList_default(const ModelEntry *entry, Link_DisplayList id) {
    return NULL;
}

static bool ModelEntry_setMatrix_default(ModelEntry *entry, Link_EquipmentMatrix id, Mtx *mtx) {
    return false;
}

static Mtx *ModelEntry_getMatrix_default(const ModelEntry *entry, Link_EquipmentMatrix id) {
    return NULL;
}

static bool ModelEntry_init(ModelEntry *entry, PlayerModelManagerHandle handle, PlayerModelManagerModelType type, const char *internalName) {
    if (!Utils_isValidModelType(type)) {
        Logger_printError("Trying to initialize ModelEntry with invalid PlayerModelManagerModelType %d", type);
        Utils_tryCrashGame();
        return false;
    }

    if (!internalName) {
        Logger_printError("Trying to initialize ModelEntry with type %d with NULL internal name!\n", type);
        Utils_tryCrashGame();
        return false;
    }

    if (!handle) {
        Logger_printError("Trying to initialize ModelEntry with invalid PlayerModelManagerHandle %d", handle);
        Utils_tryCrashGame();
        return false;
    }

    static ModelEntryVirtualFunctions funcs = {
        .applyToFormProxy = ModelEntry_applyToFormProxy_default,
        .removeFromFormProxy = ModelEntry_removeFromFormProxy_default,
        .setDisplayList = ModelEntry_setDisplayList_default,
        .getDisplayList = ModelEntry_getDisplayList_default,
        .setMatrix = ModelEntry_setMatrix_default,
        .getMatrix = ModelEntry_getMatrix_default,
    };

    entry->virtualFuncs = &funcs;
    entry->type = type;
    entry->displayName = NULL;
    entry->internalName = YAZMTCore_Utils_StrDup(internalName);
    entry->authorName = NULL;
    entry->flags = 0;
    entry->callback = NULL;
    entry->callbackData = NULL;
    entry->handle = handle;

    ADD_VALIDATED_PTR(entry);

    return true;
}

bool ModelEntry_applyToFormProxy(const ModelEntry *entry, FormProxy *fp) {
    RETURN_IF_INVALID_PTR(entry, false);

    return entry->virtualFuncs->applyToFormProxy(entry, fp);
}

bool ModelEntry_removeFromFormProxy(const ModelEntry *entry, FormProxy *fp) {
    RETURN_IF_INVALID_PTR(entry, false);

    return entry->virtualFuncs->removeFromFormProxy(entry, fp);
}

Gfx *ModelEntry_getDisplayList(const ModelEntry *entry, Link_DisplayList id) {
    RETURN_IF_INVALID_PTR(entry, NULL);

    return entry->virtualFuncs->getDisplayList(entry, id);
}

bool ModelEntry_setDisplayList(ModelEntry *entry, Link_DisplayList id, Gfx *dl) {
    RETURN_IF_INVALID_PTR(entry, false);

    return entry->virtualFuncs->setDisplayList(entry, id, dl);
}

Mtx *ModelEntry_getMatrix(const ModelEntry *entry, Link_EquipmentMatrix id) {
    RETURN_IF_INVALID_PTR(entry, NULL);

    return entry->virtualFuncs->getMatrix(entry, id);
}

bool ModelEntry_setMatrix(ModelEntry *entry, Link_EquipmentMatrix id, Mtx *matrix) {
    RETURN_IF_INVALID_PTR(entry, false);

    return entry->virtualFuncs->setMatrix(entry, id, matrix);
}

static bool ModelEntryForm_applyToFormProxy(const ModelEntry *thisx, FormProxy *fp) {
    PlayerProxy *pp = FormProxy_getPlayerProxy(fp);

    if (pp) {
#if defined(ZELDA_ANDROID_BUILTIN_PMM)
        FormProxy_clearCurrentModelState(fp);
#endif
        FormProxy_setCurrentModelFormEntry(fp, (ModelEntryForm *)thisx);

        PlayerProxy_requestRefresh(pp);

#if defined(ZELDA_ANDROID_BUILTIN_PMM)
        ModelEntryForm *formEntry = (ModelEntryForm *)thisx;
        recomp_android_compat_pmm_apply_log(
            21,
            ModelEntry_getType(thisx),
            formEntry->skel ? 1 : 0,
            ModelVisuals_getDL(&formEntry->modelVisuals, LINK_DL_HEAD) ? 1 : 0,
            ModelEntry_getInternalName(thisx));
#endif

        return true;
    }

    return false;
}

static bool ModelEntryForm_removeFromFormProxy(const ModelEntry *thisx, FormProxy *fp) {
    PlayerProxy *pp = FormProxy_getPlayerProxy(fp);

    if (pp) {
        FormProxy_setCurrentModelFormEntry(fp, NULL);

        PlayerProxy_requestRefresh(pp);

        return true;
    }

    return false;
}

static bool ModelEntryForm_setDisplayList(ModelEntry *thisx, Link_DisplayList id, Gfx *dl) {
    ModelEntryForm *this = (ModelEntryForm *)thisx;
    ModelVisuals_setDL(&this->modelVisuals, id, dl);
    return true;
}

static Gfx *ModelEntryForm_getDisplayList(const ModelEntry *thisx, Link_DisplayList id) {
    ModelEntryForm *this = (ModelEntryForm *)thisx;
    return ModelVisuals_getDL(&this->modelVisuals, id);
}

static bool ModelEntryForm_setMatrix(ModelEntry *thisx, Link_EquipmentMatrix id, Mtx *mtx) {
    ModelEntryForm *this = (ModelEntryForm *)thisx;
    ModelVisuals_setMtx(&this->modelVisuals, id, mtx);
    return true;
}

static Mtx *ModelEntryForm_getMatrix(const ModelEntry *thisx, Link_EquipmentMatrix id) {
    ModelEntryForm *this = (ModelEntryForm *)thisx;
    return ModelVisuals_getMtx(&this->modelVisuals, id);
}

static bool ModelEntryForm_init(ModelEntryForm *entry, PlayerModelManagerHandle handle, PlayerModelManagerModelType type, const char *internalName) {
    if (!ModelEntry_init(&entry->modelEntry, handle, type, internalName)) {
        return false;
    }

    if (!Utils_isFormModelType(entry->modelEntry.type)) {
        Logger_printError("ModelEntryForm with internal name %s had non-form type %d passed in!\n", internalName, type);
        Utils_tryCrashGame();
        return false;
    }

    static ModelEntryVirtualFunctions funcs = {
        .applyToFormProxy = ModelEntryForm_applyToFormProxy,
        .removeFromFormProxy = ModelEntryForm_removeFromFormProxy,
        .setDisplayList = ModelEntryForm_setDisplayList,
        .getDisplayList = ModelEntryForm_getDisplayList,
        .setMatrix = ModelEntryForm_setMatrix,
        .getMatrix = ModelEntryForm_getMatrix,
    };

    entry->modelEntry.virtualFuncs = &funcs;

    ModelVisuals_init(&entry->modelVisuals);

    for (int i = 0; i < LINK_EQUIP_MATRIX_MAX; ++i) {
        ModelVisuals_setMtx(&entry->modelVisuals, i, NULL);
    }

    entry->skel = NULL;
    entry->shieldingSkel = NULL;
    Lib_MemSet(entry->mouthTex, 0, sizeof(entry->mouthTex));
    Lib_MemSet(entry->eyesTex, 0, sizeof(entry->eyesTex));

    return true;
}

const char *ModelEntry_getInternalName(const ModelEntry *entry) {
    RETURN_IF_INVALID_PTR(entry, NULL);

    return entry->internalName;
}

const char *ModelEntry_getDisplayName(const ModelEntry *entry) {
    RETURN_IF_INVALID_PTR(entry, NULL);

    return entry->displayName;
}

static char *duplicateStringAndTruncate(const char *s, size_t maxSize) {
    size_t len = strlen(s);

    if (len > maxSize) {
        len = maxSize;
    }

    char *newStr = recomp_alloc(len + 1);

    for (size_t i = 0; i < len; ++i) {
        newStr[i] = s[i];
    }

    newStr[len] = '\0';

    return newStr;
}

void ModelEntry_setDisplayName(ModelEntry *entry, const char *name) {
    RETURN_IF_INVALID_PTR(entry, PTR_VAL_VOID_RET);

    recomp_free(entry->displayName);

    if (name) {
        entry->displayName = duplicateStringAndTruncate(name, PMM_MAX_DISPLAY_NAME_LENGTH);
    } else if (entry->internalName) {
        ModelEntry_setDisplayName(entry, entry->internalName);
    }
}

const char *ModelEntry_getAuthorName(const ModelEntry *entry) {
    RETURN_IF_INVALID_PTR(entry, NULL);

    return entry->authorName;
}

void ModelEntry_setAuthorName(ModelEntry *entry, const char *name) {
    RETURN_IF_INVALID_PTR(entry, PTR_VAL_VOID_RET);

    recomp_free(entry->authorName);

    if (name) {
        entry->authorName = duplicateStringAndTruncate(name, PMM_MAX_AUTHOR_NAME_LENGTH);
    } else {
        entry->authorName = NULL;
    }
}

PlayerModelManagerModelType ModelEntry_getType(const ModelEntry *entry) {
    RETURN_IF_INVALID_PTR(entry, PMM_MODEL_TYPE_NONE);

    return entry->type;
}

void ModelEntry_doCallback(const ModelEntry *entry, PlayerModelManagerModelEvent eventId) {
    RETURN_IF_INVALID_PTR(entry, PTR_VAL_VOID_RET);

    if (entry->callback) {
        entry->callback(entry->handle, eventId, entry->callbackData);
    }
}

void ModelEntry_setFlags(ModelEntry *entry, u64 flags) {
    RETURN_IF_INVALID_PTR(entry, PTR_VAL_VOID_RET);

    entry->flags |= flags;
}

void ModelEntry_unsetFlags(ModelEntry *entry, u64 flags) {
    RETURN_IF_INVALID_PTR(entry, PTR_VAL_VOID_RET);

    entry->flags &= (~flags);
}

void ModelEntry_unsetAllFlags(ModelEntry *entry) {
    RETURN_IF_INVALID_PTR(entry, PTR_VAL_VOID_RET);

    entry->flags = 0;
}

bool ModelEntry_isAnyFlagEnabled(const ModelEntry *entry, ModelEntryFlag flags) {
    RETURN_IF_INVALID_PTR(entry, false);

    return entry->flags & flags;
}

bool ModelEntry_isAllFlagsEnabled(const ModelEntry *entry, ModelEntryFlag flags) {
    RETURN_IF_INVALID_PTR(entry, false);

    return (entry->flags & flags) == flags;
}

void ModelEntry_setCallback(ModelEntry *entry, PlayerModelManagerEventHandler *callback, void *data) {
    RETURN_IF_INVALID_PTR(entry, PTR_VAL_VOID_RET);

    entry->callback = callback;
    entry->callbackData = data;
}

ModelEntryForm *ModelEntryForm_new(PlayerModelManagerHandle handle, PlayerModelManagerModelType type, const char *internalName) {
    ModelEntryForm *entry = Utils_recompCalloc(sizeof(ModelEntryForm));

    if (!entry) {
        Logger_printError("Failed to allocate memory for ModelEntryForm!");
        Utils_tryCrashGame();
        return NULL;
    }

    if (!ModelEntryForm_init(entry, handle, type, internalName)) {
        Logger_printError("Failed to initialize ModelEntryForm!");
        Utils_tryCrashGame();
        recomp_free(entry);
        return NULL;
    }

    return entry;
}

ModelEntry *ModelEntryForm_getModelEntry(ModelEntryForm *entry) {
    RETURN_IF_INVALID_PTR((ModelEntry *)entry, NULL);

    return &entry->modelEntry;
}

TexturePtr ModelEntryForm_getEyesTexture(ModelEntryForm *entry, PlayerEyeIndex i) {
    RETURN_IF_INVALID_PTR((ModelEntry *)entry, NULL);

    return entry->eyesTex[i];
}

TexturePtr ModelEntryForm_getMouthTexture(ModelEntryForm *entry, PlayerMouthIndex i) {
    RETURN_IF_INVALID_PTR((ModelEntry *)entry, NULL);

    return entry->mouthTex[i];
}

void ModelEntryForm_setEyesTexture(ModelEntryForm *entry, TexturePtr tex, PlayerEyeIndex i) {
    RETURN_IF_INVALID_PTR((ModelEntry *)entry, PTR_VAL_VOID_RET);

    if (i < 0 || i >= PLAYER_EYES_MAX) {
        Logger_printWarning("Received invalid index %d", i);
        return;
    }

    entry->eyesTex[i] = tex;
}

void ModelEntryForm_setMouthTexture(ModelEntryForm *entry, TexturePtr tex, PlayerMouthIndex i) {
    RETURN_IF_INVALID_PTR((ModelEntry *)entry, PTR_VAL_VOID_RET);

    if (i < 0 || i >= PLAYER_MOUTH_MAX) {
        Logger_printWarning("Received invalid index %d", i);
        return;
    }

    entry->mouthTex[i] = tex;
}

void ModelEntryForm_fillDefaultFaceTextures(ModelEntryForm *entry) {
    RETURN_IF_INVALID_PTR((ModelEntry *)entry, PTR_VAL_VOID_RET);

    for (PlayerEyeIndex i = 0; i < PLAYER_EYES_MAX; ++i) {
        ModelEntryForm_setEyesTexture(entry, gDefaultEyesTextures[i], i);
    }

    for (PlayerMouthIndex i = 0; i < PLAYER_MOUTH_MAX; ++i) {
        ModelEntryForm_setMouthTexture(entry, gDefaultMouthTextures[i], i);
    }
}

void ModelEntryForm_setDLsFromSkeletons(ModelEntryForm *entry) {
    RETURN_IF_INVALID_PTR((ModelEntry *)entry, PTR_VAL_VOID_RET);

#define SET_LIMB_DL(pLimb, id)                                                                                                      \
    {                                                                                                                               \
        if (!ModelEntry_getDisplayList(&entry->modelEntry, id)) {                                                                   \
            ModelVisuals_setDL(&entry->modelVisuals, id, (limbs[pLimb - 1]->dLists[0]) ? (limbs[pLimb - 1]->dLists[0]) : gEmptyDL); \
        }                                                                                                                           \
    }                                                                                                                               \
    (void)0

    if (entry->skel) {
        LodLimb **limbs = (LodLimb **)entry->skel->sh.segment;
        SET_LIMB_DL(PLAYER_LIMB_WAIST, LINK_DL_WAIST);
        SET_LIMB_DL(PLAYER_LIMB_RIGHT_THIGH, LINK_DL_RTHIGH);
        SET_LIMB_DL(PLAYER_LIMB_RIGHT_SHIN, LINK_DL_RSHIN);
        SET_LIMB_DL(PLAYER_LIMB_RIGHT_FOOT, LINK_DL_RFOOT);
        SET_LIMB_DL(PLAYER_LIMB_LEFT_THIGH, LINK_DL_LTHIGH);
        SET_LIMB_DL(PLAYER_LIMB_LEFT_SHIN, LINK_DL_LSHIN);
        SET_LIMB_DL(PLAYER_LIMB_LEFT_FOOT, LINK_DL_LFOOT);
        SET_LIMB_DL(PLAYER_LIMB_HEAD, LINK_DL_HEAD);
        SET_LIMB_DL(PLAYER_LIMB_HAT, LINK_DL_HAT);
        SET_LIMB_DL(PLAYER_LIMB_COLLAR, LINK_DL_COLLAR);
        SET_LIMB_DL(PLAYER_LIMB_LEFT_SHOULDER, LINK_DL_LSHOULDER);
        SET_LIMB_DL(PLAYER_LIMB_LEFT_FOREARM, LINK_DL_LFOREARM);
        SET_LIMB_DL(PLAYER_LIMB_LEFT_HAND, LINK_DL_LHAND);
        SET_LIMB_DL(PLAYER_LIMB_RIGHT_SHOULDER, LINK_DL_RSHOULDER);
        SET_LIMB_DL(PLAYER_LIMB_RIGHT_FOREARM, LINK_DL_RFOREARM);
        SET_LIMB_DL(PLAYER_LIMB_RIGHT_HAND, LINK_DL_RHAND);
        SET_LIMB_DL(PLAYER_LIMB_SHEATH, LINK_DL_SHEATH_NONE);
        SET_LIMB_DL(PLAYER_LIMB_TORSO, LINK_DL_TORSO);

#if defined(ZELDA_ANDROID_BUILTIN_PMM)
        Gfx *rightFoot = ModelVisuals_getDL(&entry->modelVisuals, LINK_DL_RFOOT);
        Gfx *leftFoot = ModelVisuals_getDL(&entry->modelVisuals, LINK_DL_LFOOT);

        if (!rightFoot || rightFoot == gEmptyDL) {
            Gfx *rightBoot = ModelVisuals_getDL(&entry->modelVisuals, LINK_DL_BOOT_RIRON);
            if (!rightBoot || rightBoot == gEmptyDL) {
                rightBoot = ModelVisuals_getDL(&entry->modelVisuals, LINK_DL_BOOT_RHOVER);
            }
            if (rightBoot && rightBoot != gEmptyDL) {
                ModelVisuals_setDL(&entry->modelVisuals, LINK_DL_RFOOT, rightBoot);
            }
        }

        if (!leftFoot || leftFoot == gEmptyDL) {
            Gfx *leftBoot = ModelVisuals_getDL(&entry->modelVisuals, LINK_DL_BOOT_LIRON);
            if (!leftBoot || leftBoot == gEmptyDL) {
                leftBoot = ModelVisuals_getDL(&entry->modelVisuals, LINK_DL_BOOT_LHOVER);
            }
            if (leftBoot && leftBoot != gEmptyDL) {
                ModelVisuals_setDL(&entry->modelVisuals, LINK_DL_LFOOT, leftBoot);
            }
        }
#endif
    }

#undef SET_LIMB_DL

#define SET_SHIELDING_LIMB_DL(pLimb, id)                                                                                    \
    {                                                                                                                       \
        if (!ModelEntry_getDisplayList(&entry->modelEntry, id)) {                                                           \
            ModelVisuals_setDL(&entry->modelVisuals, id, (limbs[pLimb - 1]->dList) ? (limbs[pLimb - 1]->dList) : gEmptyDL); \
        }                                                                                                                   \
    }                                                                                                                       \
    (void)0

    if (entry->shieldingSkel) {
        StandardLimb **limbs = (StandardLimb **)entry->shieldingSkel->sh.segment;
        SET_SHIELDING_LIMB_DL(LINK_BODY_SHIELD_LIMB_BODY, LINK_DL_BODY_SHIELD_BODY);
        SET_SHIELDING_LIMB_DL(LINK_BODY_SHIELD_LIMB_HEAD, LINK_DL_BODY_SHIELD_HEAD);
        SET_SHIELDING_LIMB_DL(LINK_BODY_SHIELD_LIMB_ARMS_AND_LEGS, LINK_DL_BODY_SHIELD_ARMS_AND_LEGS);
    }

#undef SET_SHIELDING_LIMB_DL
}

void ModelEntryForm_setSkeleton(ModelEntryForm *entry, FlexSkeletonHeader *skel) {
    RETURN_IF_INVALID_PTR((ModelEntry *)entry, PTR_VAL_VOID_RET);

    entry->skel = skel;
}

FlexSkeletonHeader *ModelEntryForm_getSkeleton(ModelEntryForm *entry) {
    RETURN_IF_INVALID_PTR((ModelEntry *)entry, NULL);

    return entry->skel;
}

void ModelEntryForm_setShieldingSkeleton(ModelEntryForm *entry, FlexSkeletonHeader *skel) {
    RETURN_IF_INVALID_PTR((ModelEntry *)entry, PTR_VAL_VOID_RET);

    entry->shieldingSkel = skel;
}

FlexSkeletonHeader *ModelEntryForm_getShieldingSkeleton(ModelEntryForm *entry) {
    RETURN_IF_INVALID_PTR((ModelEntry *)entry, NULL);

    return entry->shieldingSkel;
}

static bool ModelEntryEquipment_setDisplayList(ModelEntry *thisx, Link_DisplayList id, Gfx *dl) {
    RETURN_IF_INVALID_PTR(thisx, false);

    ModelEntryEquipment *this = (ModelEntryEquipment *)thisx;

    const EquipmentOverride *override = EquipmentOverrides_getEquipmentOverride(this->modelEntry.type);

    if (override) {
        // Count generally is very small (count < 10), so a simple linear search will do
        for (size_t i = 0; i < override->dl.count; ++i) {
            if (id == override->dl.overrides[i]) {
                ModelVisuals_setDL(&this->fallbackModelVisuals, id, dl);
                return true;
            }
        }
    }

    return false;
}

static Gfx *ModelEntryEquipment_getDisplayList(const ModelEntry *thisx, Link_DisplayList id) {
    RETURN_IF_INVALID_PTR(thisx, NULL);

    const ModelEntryEquipment *this = (ModelEntryEquipment *)thisx;
    return ModelVisuals_getDL(&this->fallbackModelVisuals, id);
}

static ModelVisuals *ModelEntryEquipment_getModelTypeModelVisual(ModelEntryEquipment *entry, PlayerModelManagerModelType type) {
    return recomputil_u32_memory_hashmap_get(entry->modelTypeToModelVisuals, type);
}

static ModelVisuals *ModelEntryEquipment_getOrCreateModelTypeModelVisual(ModelEntryEquipment *entry, PlayerModelManagerModelType type) {
    RETURN_IF_INVALID_PTR((ModelEntry *)entry, NULL);

    ModelVisuals *modelVisuals = ModelEntryEquipment_getModelTypeModelVisual(entry, type);

    if (!modelVisuals) {
        if (recomputil_u32_memory_hashmap_create(entry->modelTypeToModelVisuals, type)) {
            modelVisuals = recomputil_u32_memory_hashmap_get(entry->modelTypeToModelVisuals, type);
            if (modelVisuals) {
                ModelVisuals_init(modelVisuals);
            }
        }
    }

    return modelVisuals;
}

bool ModelEntryEquipment_setDisplayListForModelType(ModelEntryEquipment *entry, PlayerModelManagerModelType type, Link_DisplayList id, Gfx *dl) {
    RETURN_IF_INVALID_PTR((ModelEntry *)entry, false);

    ModelVisuals *modelVisuals = ModelEntryEquipment_getOrCreateModelTypeModelVisual(entry, type);

    if (modelVisuals) {
        ModelVisuals_setDL(modelVisuals, id, dl);
        return true;
    }

    return false;
}

static Gfx *ModelEntryEquipment_getDisplayListForModelType(ModelEntryEquipment *entry, PlayerModelManagerModelType type, Link_DisplayList id) {
    RETURN_IF_INVALID_PTR((ModelEntry *)entry, NULL);

    Gfx *dl = NULL;

    ModelVisuals *overrideModelVisuals = ModelEntryEquipment_getModelTypeModelVisual(entry, type);

    if (overrideModelVisuals) {
        dl = ModelVisuals_getDL(overrideModelVisuals, id);
    }

    return dl;
}

static bool ModelEntryEquipment_setMatrix(ModelEntry *thisx, Link_EquipmentMatrix id, Mtx *mtx) {
    RETURN_IF_INVALID_PTR(thisx, false);

    ModelEntryEquipment *this = (ModelEntryEquipment *)thisx;

    const EquipmentOverride *override = EquipmentOverrides_getEquipmentOverride(this->modelEntry.type);

    if (override) {
        // Count generally is very small (count < 10), so a simple linear search will do
        for (size_t i = 0; i < override->mtx.count; ++i) {
            if (id == override->mtx.overrides[i]) {
                ModelVisuals_setMtx(&this->fallbackModelVisuals, id, mtx);
                return true;
            }
        }
    }

    return false;
}

static Mtx *ModelEntryEquipment_getMatrix(const ModelEntry *thisx, Link_EquipmentMatrix id) {
    RETURN_IF_INVALID_PTR(thisx, NULL);

    ModelEntryEquipment *this = (ModelEntryEquipment *)thisx;
    return ModelVisuals_getMtx(&this->fallbackModelVisuals, id);
}

bool ModelEntryEquipment_setMatrixForModelType(ModelEntryEquipment *entry, PlayerModelManagerModelType type, Link_EquipmentMatrix id, Mtx *mtx) {
    RETURN_IF_INVALID_PTR((ModelEntry *)entry, false);

    ModelVisuals *modelVisuals = ModelEntryEquipment_getOrCreateModelTypeModelVisual(entry, type);

    if (modelVisuals) {
        ModelVisuals_setMtx(modelVisuals, id, mtx);
        return true;
    }

    return false;
}

Mtx *ModelEntryEquipment_getMatrixForModelType(ModelEntryEquipment *entry, PlayerModelManagerModelType type, Link_EquipmentMatrix id) {
    RETURN_IF_INVALID_PTR((ModelEntry *)entry, NULL);

    Mtx *mtx = NULL;

    ModelVisuals *overrideModelVisuals = ModelEntryEquipment_getModelTypeModelVisual(entry, type);

    if (overrideModelVisuals) {
        mtx = ModelVisuals_getMtx(overrideModelVisuals, id);
    }

    return mtx;
}

static bool ModelEntryEquipment_applyToFormProxy(const ModelEntry *thisx, FormProxy *fp) {
    RETURN_IF_INVALID_PTR(thisx, false);

    PlayerProxy *pp = FormProxy_getPlayerProxy(fp);

    if (pp) {
        ModelEntryEquipment *this = (ModelEntryEquipment *)((void *)thisx);

        const EquipmentOverride *override = EquipmentOverrides_getEquipmentOverride(this->modelEntry.type);

        if (override) {
            for (size_t i = 0; i < gFormProxyIds->size; i++) {
                FormProxy *currFp = PlayerProxy_getFormProxy(pp, gFormProxyIds->ids[i]);
                if (currFp) {
                    PlayerModelManagerModelType type = FormProxy_getModelType(currFp);

                    for (size_t j = 0; j < override->dl.count; ++j) {
                        Link_DisplayList id = override->dl.overrides[j];

                        Gfx *singleFormDL = ModelEntryEquipment_getDisplayListForModelType(this, type, id);
                        if (singleFormDL) {
                            FormProxy_setCurrentOverrideDL(currFp, id, singleFormDL);
                        } else {
                            FormProxy_setCurrentOverrideDL(currFp, id, ModelVisuals_getDL(&this->fallbackModelVisuals, id));
                        }
                    }

                    for (size_t j = 0; j < override->mtx.count; ++j) {
                        Link_EquipmentMatrix id = override->mtx.overrides[j];

                        Mtx *singleFormMtx = ModelEntryEquipment_getMatrixForModelType(this, type, id);
                        if (singleFormMtx) {
                            FormProxy_setCurrentOverrideMtx(currFp, id, singleFormMtx);
                        } else {
                            FormProxy_setCurrentOverrideMtx(currFp, id, ModelVisuals_getMtx(&this->fallbackModelVisuals, id));
                        }
                    }
                }
            }

            PlayerProxy_requestRefresh(pp);

            return true;
        }
    }

    return false;
}

static bool ModelEntryEquipment_removeFromFormProxy(const ModelEntry *thisx, FormProxy *fp) {
    RETURN_IF_INVALID_PTR(thisx, false);

    PlayerProxy *pp = FormProxy_getPlayerProxy(fp);

    if (pp) {
        ModelEntryEquipment *this = (ModelEntryEquipment *)((void *)thisx);

        const EquipmentOverride *override = EquipmentOverrides_getEquipmentOverride(this->modelEntry.type);

        if (override) {
            for (size_t i = 0; i < override->dl.count; ++i) {
                Link_DisplayList id = override->dl.overrides[i];
                PlayerProxy_setOverrideDL(pp, id, NULL);
            }

            for (size_t i = 0; i < override->mtx.count; ++i) {
                Link_EquipmentMatrix id = override->mtx.overrides[i];
                PlayerProxy_setOverrideMtx(pp, id, NULL);
            }

            PlayerProxy_requestRefresh(pp);

            return true;
        }
    }

    return false;
}

static bool ModelEntryEquipment_init(ModelEntryEquipment *entry, PlayerModelManagerHandle handle, PlayerModelManagerModelType type, const char *internalName) {
    if (!ModelEntry_init(&entry->modelEntry, handle, type, internalName)) {
        return false;
    }

    if (!Utils_isEquipmentModelType(entry->modelEntry.type)) {
        Logger_printError("ModelEntryEquipment with internal name %s had non-equipment type %d passed in!\n", internalName, type);
        Utils_tryCrashGame();
        return false;
    }

    static ModelEntryVirtualFunctions funcs = {
        .applyToFormProxy = ModelEntryEquipment_applyToFormProxy,
        .removeFromFormProxy = ModelEntryEquipment_removeFromFormProxy,
        .setDisplayList = ModelEntryEquipment_setDisplayList,
        .getDisplayList = ModelEntryEquipment_getDisplayList,
        .setMatrix = ModelEntryEquipment_setMatrix,
        .getMatrix = ModelEntryEquipment_getMatrix,
    };

    entry->modelEntry.virtualFuncs = &funcs;
    ModelVisuals_init(&entry->fallbackModelVisuals);
    entry->modelTypeToModelVisuals = recomputil_create_u32_memory_hashmap(sizeof(ModelVisuals));

    return true;
}

ModelEntryEquipment *ModelEntryEquipment_new(PlayerModelManagerHandle handle, PlayerModelManagerModelType type, const char *internalName) {
    ModelEntryEquipment *entry = Utils_recompCalloc(sizeof(ModelEntryEquipment));

    if (!entry) {
        Logger_printError("Failed to allocate memory for ModelEntryEquipment!");
        Utils_tryCrashGame();
        return NULL;
    }

    if (!ModelEntryEquipment_init(entry, handle, type, internalName)) {
        Logger_printError("Failed to initialize ModelEntryEquipment!");
        Utils_tryCrashGame();
        recomp_free(entry);
        return NULL;
    }

    return entry;
}

ModelEntry *ModelEntryEquipment_getModelEntry(ModelEntryEquipment *entry) {
    RETURN_IF_INVALID_PTR((ModelEntry *)entry, NULL);

    return &entry->modelEntry;
}

bool ModelEntryPack_setDisplayList(ModelEntry *thisx, Link_DisplayList id, Gfx *dl) {
    return false;
}

bool ModelEntryPack_setMatrix(ModelEntry *thisx, Link_EquipmentMatrix id, Mtx *mtx) {
    return false;
}

bool ModelEntryPack_applyToFormProxy(const ModelEntry *thisx, FormProxy *fp) {
    RETURN_IF_INVALID_PTR(thisx, false);

    const ModelEntryPack *this = (const ModelEntryPack *)thisx;

    PlayerProxy *pp = FormProxy_getPlayerProxy(fp);

    size_t numEntries = ModelEntryPack_getModelEntriesCount(this);
    ModelEntry const *const *modelEntries = ModelEntryPack_getModelEntries(this);

    for (size_t i = 0; i < numEntries; ++i) {
        PlayerProxy_tryApplyEntry(pp, modelEntries[i]->type, modelEntries[i]);
    }

    return true;
}

bool ModelEntryPack_removeFromFormProxy(const ModelEntry *this, FormProxy *fp) {
    return true;
}

bool ModelEntryPack_init(ModelEntryPack *entry, PlayerModelManagerHandle handle, const char *internalName) {
    if (!ModelEntry_init(&entry->modelEntry, handle, PMM_MODEL_TYPE_MODEL_PACK, internalName)) {
        return false;
    }

    static ModelEntryVirtualFunctions funcs = {
        .applyToFormProxy = ModelEntryPack_applyToFormProxy,
        .removeFromFormProxy = ModelEntryPack_removeFromFormProxy,
        .setDisplayList = ModelEntryPack_setDisplayList,
        .setMatrix = ModelEntryPack_setMatrix,
    };
    entry->modelEntry.virtualFuncs = &funcs;
    entry->modelEntries = YAZMTCore_IterableU32Set_new();

    return true;
}

ModelEntryPack *ModelEntryPack_new(PlayerModelManagerHandle handle, const char *internalName) {
    ModelEntryPack *entry = Utils_recompCalloc(sizeof(ModelEntryPack));

    if (!entry) {
        Logger_printError("Failed to allocate memory for ModelEntryPack!");
        Utils_tryCrashGame();
        return NULL;
    }

    if (!ModelEntryPack_init(entry, handle, internalName)) {
        Logger_printError("Failed to initialize ModelEntryPack!");
        Utils_tryCrashGame();
        recomp_free(entry);
        return NULL;
    }

    return entry;
}

ModelEntry *ModelEntryPack_getModelEntry(ModelEntryPack *entry) {
    RETURN_IF_INVALID_PTR((ModelEntry *)entry, NULL);

    return &entry->modelEntry;
}

ModelEntry const *const *ModelEntryPack_getModelEntries(const ModelEntryPack *entry) {
    RETURN_IF_INVALID_PTR((ModelEntry *)entry, NULL);

    return (ModelEntry const *const *)YAZMTCore_IterableU32Set_values(entry->modelEntries);
}

size_t ModelEntryPack_getModelEntriesCount(const ModelEntryPack *entry) {
    RETURN_IF_INVALID_PTR((ModelEntry *)entry, 0);

    return YAZMTCore_IterableU32Set_size(entry->modelEntries);
}

static YAZMTCore_DynamicU32Array *sQueuedPacks;

static bool isEntryInPack(const ModelEntryPack *pack, const ModelEntry *entry) {
    YAZMTCore_DynamicU32Array_clear(sQueuedPacks);

    YAZMTCore_DynamicU32Array_push(sQueuedPacks, (uintptr_t)pack);

    do {
        size_t packsCount = YAZMTCore_DynamicU32Array_size(sQueuedPacks);

        ModelEntryPack *currPack = (ModelEntryPack *)YAZMTCore_DynamicU32Array_data(sQueuedPacks)[packsCount - 1];

        if (YAZMTCore_IterableU32Set_contains(pack->modelEntries, (uintptr_t)entry)) {
            return true;
        }

        YAZMTCore_DynamicU32Array_pop(sQueuedPacks);

        const ModelEntry *const *packEntries = ModelEntryPack_getModelEntries(currPack);
        size_t entryCount = ModelEntryPack_getModelEntriesCount(currPack);

        for (size_t i = 0; i < entryCount; ++i) {
            const ModelEntry *currEntry = packEntries[i];

            if (Utils_isPackModelType(ModelEntry_getType(currEntry))) {
                YAZMTCore_DynamicU32Array_push(sQueuedPacks, (uintptr_t)currEntry);
            }
        }

    } while (YAZMTCore_DynamicU32Array_size(sQueuedPacks) > 0);

    return false;
}

bool ModelEntryPack_addEntryToPack(ModelEntryPack *entry, ModelEntry *entryToAdd) {
    RETURN_IF_INVALID_PTR((ModelEntry *)entry, false);
    RETURN_IF_INVALID_PTR((ModelEntry *)entryToAdd, false);

    if (!Utils_isPackModelType(ModelEntry_getType(entryToAdd)) || !isEntryInPack((ModelEntryPack *)entryToAdd, &entry->modelEntry)) {
        YAZMTCore_IterableU32Set_insert(entry->modelEntries, (uintptr_t)entryToAdd);
        return true;
    }

    return false;
}

RECOMP_CALLBACK(".", _internal_initHashObjects) void initModelEntryObjects(void) {
    sQueuedPacks = YAZMTCore_DynamicU32Array_new();
}
