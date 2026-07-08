#include "global.h"
#include "modelentrymanager.h"
#include "recomputils.h"
#include "modelmatrixids.h"
#include "playermodelmanager_api.h"
#include "yazmtcorelib_api.h"
#include "globalobjects_api.h"
#include "apilocal.h"
#include "logger.h"
#include "playerproxy.h"
#include "playerproxymanager.h"
#include "modelentry.h"
#include "formproxy.h"
#include "customdls.h"
#include "equipmentbuiltin.h"
#include "utils.h"
#include "proxyactorext.h"
#include "recompconfig.h"

typedef enum {
    TUNIC_COLOR_OFF,
    TUNIC_COLOR_AUTO,
    TUNIC_COLOR_FORCE,
} TunicColorConfigOption;

static bool shouldAcceptTunicColorOverride(void) {
    return recomp_get_config_u32("is_modify_tunic_color") != TUNIC_COLOR_FORCE;
}

static bool isShimDL(Link_DisplayList id) {
    static bool shimDLIds[] = {
        [LINK_DL_SWORD1] = true,
        [LINK_DL_SWORD2] = true,
        [LINK_DL_SWORD3] = true,
        [LINK_DL_SWORD4] = true,
        [LINK_DL_SWORD4_BROKEN] = true,
        [LINK_DL_SWORD5] = true,
        [LINK_DL_SWORD1_HILT_BACK] = true,
        [LINK_DL_SWORD2_HILT_BACK] = true,
        [LINK_DL_SWORD3_HILT_BACK] = true,
        [LINK_DL_SWORD4_HILT_BACK] = true,
        [LINK_DL_SWORD5_HILT_BACK] = true,
        [LINK_DL_SWORD1_SHEATHED] = true,
        [LINK_DL_SWORD2_SHEATHED] = true,
        [LINK_DL_SWORD3_SHEATHED] = true,
        [LINK_DL_SWORD4_SHEATHED] = true,
        [LINK_DL_SWORD5_SHEATHED] = true,
        [LINK_DL_SHIELD1_BACK] = true,
        [LINK_DL_SHIELD2_BACK] = true,
        [LINK_DL_SHIELD3_BACK] = true,
        [LINK_DL_SWORD1_SHIELD1_SHEATH] = true,
        [LINK_DL_SWORD1_SHIELD2_SHEATH] = true,
        [LINK_DL_SWORD1_SHIELD3_SHEATH] = true,
        [LINK_DL_SWORD2_SHIELD1_SHEATH] = true,
        [LINK_DL_SWORD2_SHIELD2_SHEATH] = true,
        [LINK_DL_SWORD2_SHIELD3_SHEATH] = true,
        [LINK_DL_SWORD3_SHIELD1_SHEATH] = true,
        [LINK_DL_SWORD3_SHIELD2_SHEATH] = true,
        [LINK_DL_SWORD3_SHIELD3_SHEATH] = true,
        [LINK_DL_SWORD4_SHIELD1_SHEATH] = true,
        [LINK_DL_SWORD4_SHIELD2_SHEATH] = true,
        [LINK_DL_SWORD4_SHIELD3_SHEATH] = true,
        [LINK_DL_SWORD5_SHIELD1_SHEATH] = true,
        [LINK_DL_SWORD5_SHIELD2_SHEATH] = true,
        [LINK_DL_SWORD5_SHIELD3_SHEATH] = true,
        [LINK_DL_SWORD1_SHIELD1_SHEATHED] = true,
        [LINK_DL_SWORD1_SHIELD2_SHEATHED] = true,
        [LINK_DL_SWORD1_SHIELD3_SHEATHED] = true,
        [LINK_DL_SWORD2_SHIELD1_SHEATHED] = true,
        [LINK_DL_SWORD2_SHIELD2_SHEATHED] = true,
        [LINK_DL_SWORD2_SHIELD3_SHEATHED] = true,
        [LINK_DL_SWORD3_SHIELD1_SHEATHED] = true,
        [LINK_DL_SWORD3_SHIELD2_SHEATHED] = true,
        [LINK_DL_SWORD3_SHIELD3_SHEATHED] = true,
        [LINK_DL_SWORD4_SHIELD1_SHEATHED] = true,
        [LINK_DL_SWORD4_SHIELD2_SHEATHED] = true,
        [LINK_DL_SWORD4_SHIELD3_SHEATHED] = true,
        [LINK_DL_SWORD5_SHIELD1_SHEATHED] = true,
        [LINK_DL_SWORD5_SHIELD2_SHEATHED] = true,
        [LINK_DL_SWORD5_SHIELD3_SHEATHED] = true,
        [LINK_DL_LFIST_SWORD1] = true,
        [LINK_DL_LFIST_SWORD2] = true,
        [LINK_DL_LFIST_SWORD3] = true,
        [LINK_DL_LFIST_SWORD3_PEDESTAL_GRABBED] = true,
        [LINK_DL_LFIST_SWORD4] = true,
        [LINK_DL_LFIST_SWORD4_BROKEN] = true,
        [LINK_DL_LFIST_SWORD5] = true,
        [LINK_DL_RFIST_SHIELD1] = true,
        [LINK_DL_RFIST_SHIELD2] = true,
        [LINK_DL_RFIST_SHIELD3] = true,
        [LINK_DL_LFIST_HAMMER] = true,
        [LINK_DL_LFIST_BOOMERANG] = true,
        [LINK_DL_RFIST_BOW] = true,
        [LINK_DL_RFIST_SLINGSHOT] = true,
        [LINK_DL_RFIST_HOOKSHOT] = true,
        [LINK_DL_RFIST_LONGSHOT] = true,
        [LINK_DL_RHAND_OCARINA_FAIRY] = true,
        [LINK_DL_RHAND_OCARINA_TIME] = true,
        [LINK_DL_FPS_RHAND_BOW] = true,
        [LINK_DL_FPS_RHAND_SLINGSHOT] = true,
        [LINK_DL_FPS_RHAND_HOOKSHOT] = true,
        [LINK_DL_FPS_RHAND_LONGSHOT] = true,
        [LINK_DL_SHIELD1_ITEM] = true,
        [LINK_DL_SWORD3_PEDESTAL] = true,
        [LINK_DL_SWORD3_PEDESTAL_GRABBED] = true,
        [LINK_DL_CENTER_FLOWER_PROPELLER_OPEN] = true,
        [LINK_DL_CENTER_FLOWER_PROPELLER_CLOSED] = true,
    };

    return id >= 0 && id < ARRAY_COUNT(shimDLIds) && shimDLIds[id]; 
}

static Gfx sBelowV3FirstPersonRightForearmDL[] = {
    gsSPEndDisplayList(),
};

static Gfx sBelowV3FirstPersonRightHandDL[] = {
    gsSPBranchList(gLinkHumanFirstPersonArmDL),
};

static bool isStrTooLong(const char *s, size_t maxLen) {
    size_t length = 0;

    while (*s != '\0') {
        length++;
        s++;

        if (length > maxLen) {
            return true;
        }
    }

    return false;
}

static bool isStrValid(const char *callerName, const char *strToVerify, size_t maxLen) {
    if (!strToVerify) {
        Logger_printError("%s: String passed in was NULL! Aborting!", callerName);
        return false;
    }

    bool isTooLong = isStrTooLong(strToVerify, maxLen);

    if (isTooLong) {
        Logger_printError("%s: String passed in was too long! Maximum length is %u!", callerName, maxLen);
        return false;
    }

    return true;
}

static bool sIsAPILocked = true;

static ModelEntry *getEntryOrPrintErr(PlayerModelManagerHandle h, const char *funcName) {
    ModelEntry *entry = ModelEntryManager_getEntry(h);

    if (!entry) {
        Logger_printError("Invalid handle passed in to %s.", funcName);
    }

    return entry;
}

static ModelEntry *getEntryOrPrintErrLocked(PlayerModelManagerHandle h, const char *funcName) {
    if (sIsAPILocked) {
        Logger_printWarning(
            "%s called while API locked. "
            "Please only call these functions during a onRegisterModels callback.",
            funcName);

        return NULL;
    }

    return getEntryOrPrintErr(h, funcName);
}

static ModelEntryForm *getFormEntryOrPrintErr(PlayerModelManagerHandle h, const char *funcName) {
    ModelEntry *entry = getEntryOrPrintErrLocked(h, funcName);

    if (!entry) {
        return NULL;
    }

    if (!Utils_isFormModelType(ModelEntry_getType(entry))) {
        Logger_printWarning("Handle with internal name '%s' does not support the function '%s'", ModelEntry_getInternalName(entry), funcName);
        return NULL;
    }

    return (ModelEntryForm *)entry;
}

RECOMP_EXPORT PlayerModelManagerHandle PlayerModelManager_registerModel(unsigned long apiVersion, const char *internalName, PlayerModelManagerModelType modelType) {
    if (apiVersion > PMM_API_VERSION) {
        Logger_printWarning("Model requesting unsupported API version %d! You may need to upgrade PlayerModelManager!");
        return 0;
    }

    if (sIsAPILocked) {
        Logger_printWarning("Models can only be registered during a onRegisterModels callback.");
        return 0;
    }

    if (!isStrValid(__func__, internalName, PMM_MAX_INTERNAL_NAME_LENGTH)) {
        return 0;
    }

    if (!Utils_isValidModelType(modelType)) {
        Logger_printError("Passed in unsupported PlayerModelManagerModelType %d.", modelType);
        return 0;
    }

    PlayerModelManagerHandle h = ModelEntryManager_createMemoryHandle(modelType, YAZMTCore_Utils_StrDup(internalName));

    ModelEntry *entry = getEntryOrPrintErrLocked(h, __func__);

    if (!entry) {
        return 0;
    }

    if (modelType == PMM_MODEL_TYPE_ADULT || modelType == PMM_MODEL_TYPE_ZORA || modelType == PMM_MODEL_TYPE_FIERCE_DEITY) {
        ModelEntry_setFlags(entry, MODELENTRY_FLAG_IS_ADULT);
    }

    ModelEntry_setDisplayName(entry, internalName);

    Logger_printVerbose("Registered new model '%s' with model type %d", ModelEntry_getInternalName(entry), modelType);

    if (apiVersion < 3UL && (modelType == PMM_MODEL_TYPE_CHILD || modelType == PMM_MODEL_TYPE_ADULT)) {
        ModelEntry_setDisplayList(entry, LINK_DL_OPT_FPS_RFOREARM, sBelowV3FirstPersonRightForearmDL);
        ModelEntry_setDisplayList(entry, LINK_DL_OPT_FPS_RHAND, sBelowV3FirstPersonRightHandDL);
        ModelEntry_setFlags(entry, MODELENTRY_FLAG_USE_OLD_EQUIP_BEHAVIOR);
    }

    return h;
}

RECOMP_EXPORT bool PlayerModelManager_setDisplayName(PlayerModelManagerHandle h, const char *displayName) {
    ModelEntry *entry = getEntryOrPrintErrLocked(h, __func__);

    if (!entry) {
        return false;
    }

    ModelEntry_setDisplayName(entry, displayName);

    Logger_printVerbose("Setting display name of handle with internal name '%s' to '%s'", ModelEntry_getInternalName(entry), displayName);

    return true;
}

RECOMP_EXPORT bool PlayerModelManager_setAuthor(PlayerModelManagerHandle h, const char *author) {
    ModelEntry *entry = getEntryOrPrintErrLocked(h, __func__);

    if (!entry) {
        return false;
    }

    ModelEntry_setAuthorName(entry, author);

    Logger_printVerbose("Setting author of handle with internal name '%s' to '%s'", ModelEntry_getInternalName(entry), author);

    return true;
}

static bool isOldElegyId(Link_DisplayList id) {
    switch (id) {
        case LINK_DL_ELEGY_OF_EMPTINESS_SHELL_HUMAN_UNUSED:
            FALLTHROUGH;
        case LINK_DL_ELEGY_OF_EMPTINESS_SHELL_DEKU_UNUSED:
            FALLTHROUGH;
        case LINK_DL_ELEGY_OF_EMPTINESS_SHELL_GORON_UNUSED:
            FALLTHROUGH;
        case LINK_DL_ELEGY_OF_EMPTINESS_SHELL_ZORA_UNUSED:
            FALLTHROUGH;
        case LINK_DL_ELEGY_OF_EMPTINESS_SHELL_FIERCE_DEITY_UNUSED:
            return true;
            break;

        default:
            break;
    }

    return false;
}

static bool isOldElegyIdMatchesModelType(Link_DisplayList id, PlayerModelManagerModelType type) {
    switch (id) {
        case LINK_DL_ELEGY_OF_EMPTINESS_SHELL_HUMAN_UNUSED:
            return type == PMM_MODEL_TYPE_CHILD || type == PMM_MODEL_TYPE_ADULT;
            break;
        case LINK_DL_ELEGY_OF_EMPTINESS_SHELL_DEKU_UNUSED:
            return type == PMM_MODEL_TYPE_DEKU;
            break;
        case LINK_DL_ELEGY_OF_EMPTINESS_SHELL_GORON_UNUSED:
            return type == PMM_MODEL_TYPE_GORON;
            break;
        case LINK_DL_ELEGY_OF_EMPTINESS_SHELL_ZORA_UNUSED:
            return type == PMM_MODEL_TYPE_ZORA;
            break;
        case LINK_DL_ELEGY_OF_EMPTINESS_SHELL_FIERCE_DEITY_UNUSED:
            return type == PMM_MODEL_TYPE_FIERCE_DEITY;
            break;

        default:
            break;
    }

    return false;
}

RECOMP_EXPORT bool PlayerModelManager_setDisplayList(PlayerModelManagerHandle h, Link_DisplayList dlId, Gfx *dl) {
    ModelEntry *entry = getEntryOrPrintErrLocked(h, __func__);

    if (!entry) {
        return false;
    }

    if (!Utils_isValidDisplayListId(dlId)) {
        Logger_printError("Tried to set invalid display list ID %d on model %s.", dlId, ModelEntry_getInternalName(entry));
        return false;
    }

    if (isShimDL(dlId)) {
        Logger_printError("Tried to set shim DL %d on model %s. This is not allowed.", dlId, ModelEntry_getInternalName(entry));
        return false;
    }

    // Backwards compatibility
    // LINK_DL_ELEGY_OF_EMPTINESS_SHELL_<FORM> were consolidated into just LINK_DL_ELEGY_OF_EMPTINESS_SHELL
    if (isOldElegyId(dlId)) {
        if (isOldElegyIdMatchesModelType(dlId, ModelEntry_getType(entry))) {
            dlId = LINK_DL_ELEGY_OF_EMPTINESS_SHELL;
        } else {
            return false;
        }
    }

    if (ModelEntry_setDisplayList(entry, dlId, dl)) {
#if defined(ZELDA_ANDROID_BUILTIN_PMM)
        if (ModelEntry_getType(entry) == PMM_MODEL_TYPE_ADULT) {
            if (dlId == LINK_DL_BOOT_RIRON || dlId == LINK_DL_BOOT_RHOVER) {
                Gfx *rightFoot = ModelEntry_getDisplayList(entry, LINK_DL_RFOOT);
                if (!rightFoot || rightFoot == gEmptyDL) {
                    ModelEntry_setDisplayList(entry, LINK_DL_RFOOT, dl);
                }
            } else if (dlId == LINK_DL_BOOT_LIRON || dlId == LINK_DL_BOOT_LHOVER) {
                Gfx *leftFoot = ModelEntry_getDisplayList(entry, LINK_DL_LFOOT);
                if (!leftFoot || leftFoot == gEmptyDL) {
                    ModelEntry_setDisplayList(entry, LINK_DL_LFOOT, dl);
                }
            }
        }
#endif
        Logger_printVerbose("Setting DL id %d on handle with internal name '%s' to 0x%X", dlId, ModelEntry_getInternalName(entry), dl);
        return true;
    }

    return false;
}

RECOMP_EXPORT bool PlayerModelManager_setDisplayListForModelType(PlayerModelManagerHandle h, PlayerModelManagerModelType type, Link_DisplayList dlId, Gfx *dl) {
    ModelEntry *entryx = getEntryOrPrintErrLocked(h, __func__);

    if (!entryx) {
        return false;
    }

    if (!Utils_isValidDisplayListId(dlId)) {
        Logger_printError("Tried to set invalid display list ID %d on model %s.", dlId, ModelEntry_getInternalName(entryx));
        return false;
    }

    if (isShimDL(dlId)) {
        Logger_printError("Tried to set shim DL %d on model %s. This is not allowed.", dlId, ModelEntry_getInternalName(entryx));
        return false;
    }

    if (!Utils_isEquipmentModelType(ModelEntry_getType(entryx))) {
        Logger_printError("Handle with internal name %s is not an equipment handle!", ModelEntry_getInternalName(entryx));
        return false;
    }

    ModelEntryEquipment *entry = (ModelEntryEquipment *)entryx;

    if (ModelEntryEquipment_setDisplayListForModelType(entry, type, dlId, dl)) {
        Logger_printVerbose("Setting DL id %d for type %d on handle with internal name '%s' to 0x%X", dlId, type, ModelEntry_getInternalName(entryx), dl);
        return true;
    }

    return false;
}

RECOMP_EXPORT bool PlayerModelManager_setMatrix(PlayerModelManagerHandle h, Link_EquipmentMatrix mtxId, Mtx *matrix) {
    ModelEntry *entry = getEntryOrPrintErrLocked(h, __func__);

    if (!entry) {
        return false;
    }

    if (!Utils_isValidMatrixId(mtxId)) {
        Logger_printError("Invalid matrix ID %d passed in to model %s.", mtxId, ModelEntry_getInternalName(entry));
        return false;
    }

    if (ModelEntry_setMatrix(entry, mtxId, matrix)) {
        Logger_printVerbose("Setting matrix with id %d at 0x%X into handle with internal name '%s'", mtxId, matrix, ModelEntry_getInternalName(entry));
        return true;
    }

    return false;
}

RECOMP_EXPORT bool PlayerModelManager_setMatrixForModelType(PlayerModelManagerHandle h, PlayerModelManagerModelType type, Link_EquipmentMatrix mtxId, Mtx *matrix) {
    if (!Utils_isValidMatrixId(mtxId)) {
        Logger_printError("Invalid matrix ID passed in.");
        return false;
    }

    if (!Utils_isValidModelType(type)) {
        Logger_printCurrentFuncAndLine("Invalid model type passed in.");
    }

    ModelEntry *entryx = getEntryOrPrintErrLocked(h, __func__);

    if (!entryx) {
        return false;
    }

    if (!Utils_isEquipmentModelType(ModelEntry_getType(entryx))) {
        Logger_printError("Handle with internal name %s is not an equipment handle!", ModelEntry_getInternalName(entryx));
        return false;
    }

    ModelEntryEquipment *entry = (ModelEntryEquipment *)entryx;

    if (ModelEntryEquipment_setMatrixForModelType(entry, type, mtxId, matrix)) {
        Logger_printVerbose("Setting matrix with id %d for type %d at 0x%X into handle with internal name '%s'", mtxId, type, matrix, ModelEntry_getInternalName(entryx));
        return true;
    }

    return false;
}

RECOMP_EXPORT bool PlayerModelManager_addHandleToPack(PlayerModelManagerHandle h, PlayerModelManagerHandle toAdd) {
    ModelEntry *entry = getEntryOrPrintErrLocked(h, __func__);

    if (!entry) {
        return false;
    }

    if (!Utils_isPackModelType(ModelEntry_getType(entry))) {
        Logger_printError("Non-model pack passed into first arg of %s (Category was %d)", __func__, ModelEntry_getType(entry));
        return false;
    }

    ModelEntry *entryToAdd = getEntryOrPrintErrLocked(toAdd, __func__);

    if (!entryToAdd) {
        return false;
    }

    // cast relies on first member of ModelEntryPack being a ModelEntry
    ModelEntryPack *packEntry = (ModelEntryPack *)entry;

    if (!ModelEntryPack_addEntryToPack(packEntry, entryToAdd)) {
        Logger_printError("Could not add handle! Handle with internal name '%s' is already a handle inside '%s' (Would cause circular reference)!", ModelEntry_getInternalName(entry), ModelEntry_getInternalName(entryToAdd));
        return false;
    }

    Logger_printVerbose("Adding handle with internal name '%s' to model pack with internal name '%s'", ModelEntry_getInternalName(entry), ModelEntry_getInternalName(entryToAdd), ModelEntry_getInternalName(entry));

    return true;
}

RECOMP_EXPORT bool PlayerModelManager_setCallback(PlayerModelManagerHandle h, PlayerModelManagerEventHandler *callback, void *userdata) {
    ModelEntry *entry = getEntryOrPrintErrLocked(h, __func__);

    if (!entry) {
        return false;
    }

    ModelEntry_setCallback(entry, callback, userdata);

    Logger_printVerbose("Setting handle with internal name '%s' to have callback function 0x%X with user data 0x%X", ModelEntry_getInternalName(entry), callback, userdata);

    return true;
}

RECOMP_EXPORT bool PlayerModelManager_setSkeleton(PlayerModelManagerHandle h, FlexSkeletonHeader *skel) {
    ModelEntryForm *entryForm = getFormEntryOrPrintErr(h, __func__);

    if (!entryForm) {
        return false;
    }

    ModelEntry *entry = ModelEntryForm_getModelEntry(entryForm);

    ModelEntryForm_setSkeleton(entryForm, skel);

    if (skel) {
#define LINK_SKEL_LIMB_COUNT 21
        if (skel->sh.limbCount != LINK_SKEL_LIMB_COUNT) {
            Logger_printWarning("For model %s, skeleton has %d limbs (expected %d)", ModelEntry_getInternalName(entry), skel->sh.limbCount, LINK_SKEL_LIMB_COUNT);
        }

#define LINK_SKEL_DLIST_COUNT 18
        if (skel->dListCount != LINK_SKEL_DLIST_COUNT) {
            Logger_printWarning("For model %s, skeleton has %d display lists (expected %d)", ModelEntry_getInternalName(entry), skel->dListCount, LINK_SKEL_DLIST_COUNT);
        }

        StandardLimb **limbs = (StandardLimb **)skel->sh.segment;

#define SET_LIMB_DL(pLimb, entryDL)                                                                                      \
    {                                                                                                                    \
        if (!ModelEntry_getDisplayList(entry, entryDL))                                                                  \
            ModelEntry_setDisplayList(entry, entryDL, (limbs[pLimb - 1]->dList) ? (limbs[pLimb - 1]->dList) : gEmptyDL); \
    }                                                                                                                    \
    (void)0

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
    }

#undef SET_LIMB_DL

    Logger_printVerbose("Setting skeleton of handle with internal name '%s' to 0x%X", ModelEntry_getInternalName(entry), skel);

    return true;
}

RECOMP_EXPORT bool PlayerModelManager_setShieldingSkeleton(PlayerModelManagerHandle h, FlexSkeletonHeader *skel) {
    ModelEntryForm *entryForm = getFormEntryOrPrintErr(h, __func__);
    ModelEntry *entry = ModelEntryForm_getModelEntry(entryForm);

    if (!entry) {
        return false;
    }

    ModelEntryForm_setShieldingSkeleton(entryForm, skel);

    if (skel) {
        if (skel->sh.limbCount != PLAYER_BODY_SHIELD_LIMB_COUNT) {
            Logger_printError("Skeleton with incorrect limb count passed in to %s", __func__);
            return false;
        }

        StandardLimb **limbs = (StandardLimb **)skel->sh.segment;

#define SET_SHIELDING_LIMB_DL(pLimb, entryDL)                                                                            \
    {                                                                                                                    \
        if (!ModelEntry_getDisplayList(entry, entryDL))                                                                  \
            ModelEntry_setDisplayList(entry, entryDL, (limbs[pLimb - 1]->dList) ? (limbs[pLimb - 1]->dList) : gEmptyDL); \
    }                                                                                                                    \
    (void)0

        SET_SHIELDING_LIMB_DL(LINK_BODY_SHIELD_LIMB_BODY, LINK_DL_BODY_SHIELD_BODY);
        SET_SHIELDING_LIMB_DL(LINK_BODY_SHIELD_LIMB_HEAD, LINK_DL_BODY_SHIELD_HEAD);
        SET_SHIELDING_LIMB_DL(LINK_BODY_SHIELD_LIMB_ARMS_AND_LEGS, LINK_DL_BODY_SHIELD_ARMS_AND_LEGS);
    }

#undef SET_SHIELDING_LIMB_DL

    Logger_printVerbose("Setting shielding skeleton of handle with internal name '%s' to 0x%X", ModelEntry_getInternalName(entry), skel);

    return true;
}

RECOMP_EXPORT bool PlayerModelManager_setEyesTextures(PlayerModelManagerHandle h, TexturePtr eyesTextures[]) {
    ModelEntryForm *entryForm = getFormEntryOrPrintErr(h, __func__);

    if (!entryForm) {
        return false;
    }

    ModelEntry *entry = ModelEntryForm_getModelEntry(entryForm);

    for (int i = 0; i < PLAYER_EYES_MAX; ++i) {
        ModelEntryForm_setEyesTexture(entryForm, eyesTextures[i], i);
    }

    Logger_printVerbose("Setting eyes flipbook textures of handle with internal name '%s'", ModelEntry_getInternalName(entry));

    return true;
}

RECOMP_EXPORT bool PlayerModelManager_setMouthTextures(PlayerModelManagerHandle h, TexturePtr mouthTextures[]) {
    ModelEntryForm *entryForm = getFormEntryOrPrintErr(h, __func__);

    if (!entryForm) {
        return false;
    }

    ModelEntry *entry = ModelEntryForm_getModelEntry(entryForm);

    for (int i = 0; i < PLAYER_MOUTH_MAX; ++i) {
        ModelEntryForm_setMouthTexture(entryForm, mouthTextures[i], i);
    }

    Logger_printVerbose("Setting mouth flipbook textures of handle with internal name '%s'", ModelEntry_getInternalName(entry));

    return true;
}

RECOMP_EXPORT void PlayerModelManager_requestOverrideTunicColor(u8 r, u8 g, u8 b, u8 a) {
    if (!shouldAcceptTunicColorOverride()) {
        return;
    }

    Color_RGBA8 color = {r, g, b, a};
    PlayerProxy_requestTunicColorOverride(gPlayer1Proxy, color);
    PlayerProxy_requestTunicColorOverride(gPlayer2Proxy, color);
}

RECOMP_EXPORT void PlayerModelManager_requestOverrideFormTunicColor(PlayerTransformation form, u8 r, u8 g, u8 b, u8 a) {
    if (!shouldAcceptTunicColorOverride()) {
        return;
    }

    FormProxyId id;

    switch (form) {
        case PLAYER_FORM_FIERCE_DEITY:
            id = FORM_PROXY_ID_FIERCE_DEITY;
            break;

        case PLAYER_FORM_GORON:
            id = FORM_PROXY_ID_GORON;
            break;

        case PLAYER_FORM_ZORA:
            id = FORM_PROXY_ID_ZORA;
            break;

        case PLAYER_FORM_DEKU:
            id = FORM_PROXY_ID_DEKU;
            break;

        case PLAYER_FORM_HUMAN:
            id = FORM_PROXY_ID_HUMAN;
            break;

        default:
            Logger_printError("Invalid form %d passed in!", form);
            id = FORM_PROXY_ID_NONE;
            break;
    }

    FormProxy *fp = PlayerProxy_getFormProxy(gPlayer1Proxy, id);

    if (fp) {
        FormProxy_requestTunicColorOverride(fp, (Color_RGBA8){r, g, b, a});
    }

    fp = PlayerProxy_getFormProxy(gPlayer2Proxy, id);

    if (fp) {
        FormProxy_requestTunicColorOverride(fp, (Color_RGBA8){r, g, b, a});
    }
}

void PlayerModelManager_lockAPI(void) {
    sIsAPILocked = true;
}

void PlayerModelManager_unlockAPI(void) {
    sIsAPILocked = false;
}
