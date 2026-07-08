#include "global.h"
#include "modding.h"
#include "recomputils.h"
#include "recompconfig.h"
#include "recompui.h"
#include "modelentrymanager.h"
#include "yazmtcorelib_api.h"
#include "logger.h"
#include "recompdata.h"
#include "modelentry.h"
#include "playerproxy.h"
#include "utils.h"
#include "fallbackmodels.h"
#include "../../misc_funcs.h"

static bool sIsFileListRefreshRequested;

static void requestRefreshFileList(void);
static void onUpOneLevelButtonPressed(RecompuiResource resource, const RecompuiEventData *data, void *userdata);
static void clearPressedInputButtons(Input *input, u16 buttons);
static void applyRealEntries(void);

typedef enum PlayerProxyIndex {
    PP_IDX_LOCAL,
    PP_IDX_KAFEI
} PlayerProxyIndex;

static PlayerProxy *getProxyFromIndex(PlayerProxyIndex i) {
    if (i == PP_IDX_LOCAL) {
        return gPlayer1Proxy;
    } else if (i == PP_IDX_KAFEI) {
        return gPlayer2Proxy;
    }

    return NULL;
}

typedef struct CategoryInfo {
    int index;
    const char *displayName;
    bool isVisible;
    bool isUsedByCurrentGame;
    PlayerModelManagerModelType category;
    bool isNeedsDiskSave;
    PlayerProxyIndex ppIndex;
} CategoryInfo;

static U32ValueHashmapHandle sPlayerProxiesToRealEntries;

static U32ValueHashmapHandle getOrCreateRealEntriesMap(const PlayerProxy *pp) {
    if (!sPlayerProxiesToRealEntries) {
        sPlayerProxiesToRealEntries = recomputil_create_u32_value_hashmap();
    }

    uintptr_t key = (uintptr_t)pp;

    if (!recomputil_u32_value_hashmap_contains(sPlayerProxiesToRealEntries, key)) {
        recomputil_u32_value_hashmap_insert(sPlayerProxiesToRealEntries, key, recomputil_create_u32_value_hashmap());
    }

    U32ValueHashmapHandle result = 0;

    recomputil_u32_value_hashmap_get(sPlayerProxiesToRealEntries, key, &result);

    return result;
}

static const ModelEntry *getRealEntry(CategoryInfo *ci) {
    uintptr_t result = 0;
    recomputil_u32_value_hashmap_get(getOrCreateRealEntriesMap(getProxyFromIndex(ci->ppIndex)), ci->category, &result);
    return (const ModelEntry *)result;
}

static void setRealEntry(CategoryInfo *ci, const ModelEntry *entry) {
    PlayerProxy *pp = getProxyFromIndex(ci->ppIndex);
    uintptr_t key = ci->category;
    uintptr_t value = (uintptr_t)entry;
    U32ValueHashmapHandle realEntries = getOrCreateRealEntriesMap(pp);

    if (value == 0) {
        recomputil_u32_value_hashmap_erase(realEntries, key);
    } else {
        recomputil_u32_value_hashmap_insert(realEntries, key, value);
    }
}

static bool sIsForceAllCategoriesVisible = false;

#define DECLARE_DEFAULT_CAT_INFO(name, isUsed, modelType, isVisibleByDefault, ppIdx) {.displayName = name, .isVisible = isVisibleByDefault, .isUsedByCurrentGame = isUsed, .category = modelType, .isNeedsDiskSave = false, .ppIndex = ppIdx}
#define DECLARE_CAT_INFO(name, isUsed, modelType) DECLARE_DEFAULT_CAT_INFO(name, isUsed, modelType, false, PP_IDX_LOCAL)
#define DECLARE_CAT_INFO_KAFEI(name, isUsed, modelType) DECLARE_DEFAULT_CAT_INFO(name, isUsed, modelType, false, PP_IDX_KAFEI)

#define CAT_USED_MM true

// Always opposite of MM
#define CAT_USED_OOT (!CAT_USED_MM)

// used by both games
#define CAT_USED_Z64 true

static CategoryInfo sCategoryInfos[] = {
    // At least one category must be visible or the category selector goes into an infinite loop
    DECLARE_DEFAULT_CAT_INFO("Model Packs", CAT_USED_Z64, PMM_MODEL_TYPE_MODEL_PACK, CAT_USED_Z64, PP_IDX_LOCAL),
    DECLARE_DEFAULT_CAT_INFO("Young Link", CAT_USED_OOT, PMM_MODEL_TYPE_CHILD, CAT_USED_OOT, PP_IDX_LOCAL),
    DECLARE_DEFAULT_CAT_INFO("Adult Link", CAT_USED_OOT, PMM_MODEL_TYPE_ADULT, CAT_USED_OOT, PP_IDX_LOCAL),
    DECLARE_DEFAULT_CAT_INFO("Human", CAT_USED_MM, PMM_MODEL_TYPE_CHILD, CAT_USED_MM, PP_IDX_LOCAL),
    DECLARE_DEFAULT_CAT_INFO("Deku", CAT_USED_MM, PMM_MODEL_TYPE_DEKU, CAT_USED_MM, PP_IDX_LOCAL),
    DECLARE_DEFAULT_CAT_INFO("Goron", CAT_USED_MM, PMM_MODEL_TYPE_GORON, CAT_USED_MM, PP_IDX_LOCAL),
    DECLARE_DEFAULT_CAT_INFO("Zora", CAT_USED_MM, PMM_MODEL_TYPE_ZORA, CAT_USED_MM, PP_IDX_LOCAL),
    DECLARE_DEFAULT_CAT_INFO("Fierce Deity", CAT_USED_MM, PMM_MODEL_TYPE_FIERCE_DEITY, CAT_USED_MM, PP_IDX_LOCAL),
    DECLARE_CAT_INFO("Kokiri Sword", CAT_USED_Z64, PMM_MODEL_TYPE_SWORD1),
    DECLARE_CAT_INFO("Razor Sword", CAT_USED_MM, PMM_MODEL_TYPE_SWORD2),
    DECLARE_CAT_INFO("Gilded Sword", CAT_USED_Z64, PMM_MODEL_TYPE_SWORD3),
    DECLARE_CAT_INFO("Fierce Deity's Sword", CAT_USED_Z64, PMM_MODEL_TYPE_SWORD4),
    DECLARE_CAT_INFO("Great Fairy's Sword", CAT_USED_MM, PMM_MODEL_TYPE_SWORD5),
    DECLARE_CAT_INFO("Deku Shield", CAT_USED_OOT, PMM_MODEL_TYPE_SHIELD1),
    DECLARE_CAT_INFO("Hero's Shield", CAT_USED_Z64, PMM_MODEL_TYPE_SHIELD2),
    DECLARE_CAT_INFO("Mirror Shield", CAT_USED_Z64, PMM_MODEL_TYPE_SHIELD3),
    DECLARE_CAT_INFO("Hookshot", CAT_USED_Z64, PMM_MODEL_TYPE_HOOKSHOT),
    DECLARE_CAT_INFO("Bow", CAT_USED_Z64, PMM_MODEL_TYPE_BOW),
    DECLARE_CAT_INFO("Slingshot", CAT_USED_OOT, PMM_MODEL_TYPE_SLINGSHOT),
    DECLARE_CAT_INFO("Bottle", CAT_USED_Z64, PMM_MODEL_TYPE_BOTTLE),
    DECLARE_CAT_INFO("Boomerang", CAT_USED_OOT, PMM_MODEL_TYPE_BOOMERANG),
    DECLARE_CAT_INFO("Hammer", CAT_USED_OOT, PMM_MODEL_TYPE_HAMMER),
    DECLARE_CAT_INFO("Deku Stick", CAT_USED_Z64, PMM_MODEL_TYPE_DEKU_STICK),
    DECLARE_CAT_INFO("Bomb", CAT_USED_Z64, PMM_MODEL_TYPE_BOMB),
    DECLARE_CAT_INFO("Bombchu", CAT_USED_Z64, PMM_MODEL_TYPE_BOMBCHU),
    DECLARE_CAT_INFO("Fairy Ocarina", CAT_USED_OOT, PMM_MODEL_TYPE_OCARINA_FAIRY),
    DECLARE_CAT_INFO("Ocarina of Time", CAT_USED_Z64, PMM_MODEL_TYPE_OCARINA_TIME),
    DECLARE_CAT_INFO("Deku Pipes", CAT_USED_MM, PMM_MODEL_TYPE_PIPES),
    DECLARE_CAT_INFO("Goron Drums", CAT_USED_MM, PMM_MODEL_TYPE_DRUMS),
    DECLARE_CAT_INFO("Zora Guitar", CAT_USED_MM, PMM_MODEL_TYPE_GUITAR),
    DECLARE_CAT_INFO("Skull Mask", CAT_USED_OOT, PMM_MODEL_TYPE_MASK_SKULL),
    DECLARE_CAT_INFO("Spooky Mask", CAT_USED_OOT, PMM_MODEL_TYPE_MASK_SPOOKY),
    DECLARE_CAT_INFO("Gerudo Mask", CAT_USED_OOT, PMM_MODEL_TYPE_MASK_GERUDO),
    DECLARE_CAT_INFO("Mask of Truth", CAT_USED_Z64, PMM_MODEL_TYPE_MASK_TRUTH),
    DECLARE_CAT_INFO("Kafei's Mask", CAT_USED_MM, PMM_MODEL_TYPE_MASK_KAFEIS_MASK),
    DECLARE_CAT_INFO("All Night Mask", CAT_USED_MM, PMM_MODEL_TYPE_MASK_ALL_NIGHT),
    DECLARE_CAT_INFO("Bunny Hood", CAT_USED_Z64, PMM_MODEL_TYPE_MASK_BUNNY),
    DECLARE_CAT_INFO("Keaton Mask", CAT_USED_Z64, PMM_MODEL_TYPE_MASK_KEATON),
    DECLARE_CAT_INFO("Garo Mask", CAT_USED_MM, PMM_MODEL_TYPE_MASK_GARO),
    DECLARE_CAT_INFO("Romani Mask", CAT_USED_MM, PMM_MODEL_TYPE_MASK_ROMANI),
    DECLARE_CAT_INFO("Circus Leader's Mask", CAT_USED_MM, PMM_MODEL_TYPE_MASK_CIRCUS_LEADER),
    DECLARE_CAT_INFO("Couple's Mask", CAT_USED_MM, PMM_MODEL_TYPE_MASK_COUPLE),
    DECLARE_CAT_INFO("Postman's Hat", CAT_USED_MM, PMM_MODEL_TYPE_MASK_POSTMAN),
    DECLARE_CAT_INFO("Great Fairy Mask", CAT_USED_MM, PMM_MODEL_TYPE_MASK_GREAT_FAIRY),
    DECLARE_CAT_INFO("Gibdo Mask", CAT_USED_MM, PMM_MODEL_TYPE_MASK_GIBDO),
    DECLARE_CAT_INFO("Don Gero's Mask", CAT_USED_MM, PMM_MODEL_TYPE_MASK_DON_GERO),
    DECLARE_CAT_INFO("Kamaro Mask", CAT_USED_MM, PMM_MODEL_TYPE_MASK_KAMARO),
    DECLARE_CAT_INFO("Captain's Hat", CAT_USED_MM, PMM_MODEL_TYPE_MASK_CAPTAIN),
    DECLARE_CAT_INFO("Stone Mask", CAT_USED_MM, PMM_MODEL_TYPE_MASK_STONE),
    DECLARE_CAT_INFO("Bremen Mask", CAT_USED_MM, PMM_MODEL_TYPE_MASK_BREMEN),
    DECLARE_CAT_INFO("Blast Mask", CAT_USED_MM, PMM_MODEL_TYPE_MASK_BLAST),
    DECLARE_CAT_INFO("Mask of Scents", CAT_USED_MM, PMM_MODEL_TYPE_MASK_SCENTS),
    DECLARE_CAT_INFO("Giant's Mask", CAT_USED_MM, PMM_MODEL_TYPE_MASK_GIANT),
    DECLARE_CAT_INFO("Deku Mask", CAT_USED_MM, PMM_MODEL_TYPE_MASK_DEKU),
    DECLARE_CAT_INFO("Goron Mask", CAT_USED_Z64, PMM_MODEL_TYPE_MASK_GORON),
    DECLARE_CAT_INFO("Zora Mask", CAT_USED_Z64, PMM_MODEL_TYPE_MASK_ZORA),
    DECLARE_CAT_INFO("Fierce Deity Mask", CAT_USED_MM, PMM_MODEL_TYPE_MASK_FIERCE_DEITY),
    DECLARE_CAT_INFO_KAFEI("Kafei", CAT_USED_MM, PMM_MODEL_TYPE_CHILD),
    DECLARE_CAT_INFO_KAFEI("Kafei's Keaton Mask", CAT_USED_MM, PMM_MODEL_TYPE_MASK_KEATON),
};

#define SELECTING_CATEGORY -99

static int sCurrentCategoryInfo = 0;

static bool isValidCategoryInfoIndex(int i) {
    return i >= 0 && i < ARRAY_COUNT(sCategoryInfos);
}

static bool isSelectingCategory(void) {
    return false;
}

static bool isSelectingModel(void) {
    return isValidCategoryInfoIndex(sCurrentCategoryInfo);
}

static CategoryInfo *getCurrentCategoryInfo(void) {
    if (isSelectingModel()) {
        return &sCategoryInfos[sCurrentCategoryInfo];
    } else {
        Logger_printWarning("getCurrentCategoryInfo found invalid sCurrentCategoryInfo value %d\n", sCurrentCategoryInfo);
    }

    return NULL;
}

typedef struct {
    RecompuiColor borderColor;
    RecompuiColor bgColor;
} ButtonColor;

static const ButtonColor sPrimaryButtonColor = {
    .borderColor = {
        .r = 185,
        .g = 125,
        .b = 242,
        .a = 204,
    },
    .bgColor = {
        .r = 185,
        .g = 125,
        .b = 242,
        .a = 13,
    },
};

static const ButtonColor sSecondaryButtonColor = {
    .borderColor = {
        .r = 23,
        .g = 214,
        .b = 232,
        .a = 204,
    },
    .bgColor = {
        .r = 23,
        .g = 214,
        .b = 232,
        .a = 13,
    },
};

static const ButtonColor sModelSelectedButtonColor = {
    .borderColor = {
        .r = 227,
        .g = 151,
        .b = 75,
        .a = 204,
    },
    .bgColor = {
        .r = 227,
        .g = 151,
        .b = 75,
        .a = 13,
    },
};

static const ButtonColor sModelRemovedButtonColor = {
    .borderColor = {
        .r = 224,
        .g = 74,
        .b = 89,
        .a = 204,
    },
    .bgColor = {
        .r = 224,
        .g = 74,
        .b = 89,
        .a = 13,
    },
};

static void setAllNavDirsToAuto(RecompuiResource id) {
    recompui_set_nav_auto(id, NAVDIRECTION_DOWN);
    recompui_set_nav_auto(id, NAVDIRECTION_UP);
    recompui_set_nav_auto(id, NAVDIRECTION_LEFT);
    recompui_set_nav_auto(id, NAVDIRECTION_RIGHT);
}

static RecompuiResource createListBoxButton(RecompuiContext context, RecompuiResource parent, const char *text, RecompuiButtonStyle style) {
    RecompuiResource button = recompui_create_button(context, parent, text, style);
    recompui_set_flex_shrink(button, 0);
    recompui_set_width(button, 100.0f, UNIT_PERCENT);
    recompui_set_text_align(button, TEXT_ALIGN_CENTER);

    return button;
}

// Connect a list of buttons vertically with the first entry at the top
//
// Left and right inputs will navigate to the top and bottom of the list,
// respectively
static void connectListBoxButtons(const RecompuiResource buttons[], size_t n) {
    if (n < 2) {
        return;
    }

    RecompuiResource first = buttons[0];
    RecompuiResource last = buttons[n - 1];

    for (size_t i = 1; i < n - 1; ++i) {
        RecompuiResource curr = buttons[i];
        RecompuiResource next = buttons[i + 1];

        recompui_set_nav(buttons[i], NAVDIRECTION_DOWN, next);
        recompui_set_nav(buttons[i], NAVDIRECTION_LEFT, first);
        recompui_set_nav(buttons[i], NAVDIRECTION_RIGHT, last);

        recompui_set_nav(next, NAVDIRECTION_UP, curr);
    }

    recompui_set_nav(first, NAVDIRECTION_DOWN, buttons[1]);
    recompui_set_nav(first, NAVDIRECTION_RIGHT, last);

    recompui_set_nav(last, NAVDIRECTION_UP, buttons[n - 2]);
    recompui_set_nav(last, NAVDIRECTION_LEFT, first);
}

YAZMTCore_IterableU32Set *sModelListButtons;

static const RecompuiResource *getCurrentModelButtonArray(void) {
    return YAZMTCore_IterableU32Set_values(sModelListButtons);
}

static size_t getCurrentModelButtonArraySize(void) {
    return YAZMTCore_IterableU32Set_size(sModelListButtons);
}

YAZMTCore_IterableU32Set *sCategoryListButtons;

static const RecompuiResource *getCategoryButtonArray(void) {
    return YAZMTCore_IterableU32Set_values(sCategoryListButtons);
}

static size_t getCategoryButtonArraySize(void) {
    return YAZMTCore_IterableU32Set_size(sCategoryListButtons);
}

static void pushButtonToList(YAZMTCore_IterableU32Set *buttonList, RecompuiResource button) {
    YAZMTCore_IterableU32Set_insert(buttonList, button);
}

RecompuiResource createAndPushButtonToList(RecompuiContext context, RecompuiResource parent, const char *text, RecompuiButtonStyle style, YAZMTCore_IterableU32Set *buttonList) {
    RecompuiResource button = createListBoxButton(context, parent, text, style);
    pushButtonToList(buttonList, button);
    return button;
}

U32ValueHashmapHandle sModelButtonsToData;
U32ValueHashmapHandle sCategoryButtonsToData;

static const void *getListButtonData(U32ValueHashmapHandle dataMap, RecompuiResource button) {
    u32 out = (uintptr_t)NULL;
    recomputil_u32_value_hashmap_get(dataMap, button, &out);
    return (void *)out;
}

static void setListButtonData(U32ValueHashmapHandle dataMap, RecompuiResource button, const void *data) {
    recomputil_u32_value_hashmap_insert(dataMap, button, (uintptr_t)data);
}

static bool getListButtonValue(U32ValueHashmapHandle dataMap, RecompuiResource button, u32 *out) {
    return recomputil_u32_value_hashmap_get(dataMap, button, out);
}

static void setListButtonValue(U32ValueHashmapHandle dataMap, RecompuiResource button, u32 val) {
    recomputil_u32_value_hashmap_insert(dataMap, button, val);
}

static void setButtonColor(RecompuiResource button, const ButtonColor *color) {
    recompui_set_background_color(button, &color->bgColor);
    recompui_set_border_color(button, &color->borderColor);
}

static void refreshModelButtonEntryColors(void) {
    CategoryInfo *catInf = getCurrentCategoryInfo();

    if (catInf) {
        const void *entry = PlayerProxy_getCurrentEntry(getProxyFromIndex(catInf->ppIndex), catInf->category);
        size_t count = getCurrentModelButtonArraySize();
        const RecompuiResource *buttons = getCurrentModelButtonArray();

        for (size_t i = 0; i < count; ++i) {
            RecompuiResource button = buttons[i];
            const void *buttonData = getListButtonData(sModelButtonsToData, button);

            if (entry && entry == buttonData) {
                setButtonColor(button, &sModelSelectedButtonColor);
            } else if (buttonData) {
                setButtonColor(button, &sPrimaryButtonColor);
            }
        }
    }
}

static void refreshCategoryButtonEntryColors(void) {
    CategoryInfo *catInf = getCurrentCategoryInfo();

    if (catInf) {
        size_t count = getCategoryButtonArraySize();
        const RecompuiResource *buttons = getCategoryButtonArray();

        for (size_t i = 0; i < count; ++i) {
            RecompuiResource button = buttons[i];
            const void *buttonData = getListButtonData(sCategoryButtonsToData, button);

            if (buttonData && buttonData == catInf) {
                setButtonColor(button, &sModelSelectedButtonColor);
            } else if (buttonData) {
                setButtonColor(button, &sPrimaryButtonColor);
            }
        }
    }
}

static RecompuiContext sUIContext;
static RecompuiResource sUIRoot;
static RecompuiResource sContainerMain;
static RecompuiResource sRowTop;
static RecompuiResource sRowAuthor;
static RecompuiResource sRowCategory;
static RecompuiResource sButtonCategoryNext;
static RecompuiResource sButtonCategoryPrev;
static RecompuiResource sLabelCategory;
static RecompuiResource sButtonClose;
static RecompuiResource sButtonCategoriesShow;

static RecompuiResource sContainerCategories;
static bool sIsContainerCategoriesVisible;
static RecompuiResource sCategoriesListElement;
static RecompuiResource sCategoriesTopRow;
static RecompuiResource sButtonCategoriesHide;
static RecompuiResource sButtonRemoveAllModels;

static bool sIsUIContextShown = false;

RecompuiResource sModelListElement;
RecompuiResource sLabelAuthorPrefix;
RecompuiResource sLabelAuthor;

static bool sIsLivePreviewEnabled = false;
#if defined(ZELDA_ANDROID_BUILTIN_PMM)
static int sAndroidCompatModelBrowseIndex = 0;
static bool sAndroidCompatBrowseStickHeld = false;
static int sAndroidCompatInputLogFrame = 0;
void processPlayerProxyRefreshRequests_on_Play_UpdateMain(void);
void processFormProxyRefreshRequests_on_Play_UpdateMain(void);
#endif

static bool shouldLivePreview(void) {
#if defined(ZELDA_ANDROID_BUILTIN_PMM)
    return true;
#else
    return sIsLivePreviewEnabled;
#endif
}

static void destroyAuthor(void) {
    if (sLabelAuthor) {
        recompui_destroy_element(sRowAuthor, sLabelAuthor);
        recompui_destroy_element(sRowAuthor, sLabelAuthorPrefix);
    }
    sLabelAuthor = 0;
    sLabelAuthorPrefix = 0;
}

static void setAuthor(const char *author) {
    // TODO: Restore author preview
    return;

    if (!author) {
        author = "N/A";
    }

    destroyAuthor();
    sLabelAuthorPrefix = recompui_create_label(sUIContext, sRowAuthor, "Author(s): ", LABELSTYLE_NORMAL);
    sLabelAuthor = recompui_create_label(sUIContext, sRowAuthor, author, LABELSTYLE_NORMAL);
}

static void applyRealEntry(int entryIndex) {
    if (entryIndex >= 0 && entryIndex < ARRAY_COUNT(sCategoryInfos)) {
        PlayerProxy *pp = getProxyFromIndex(sCategoryInfos[entryIndex].ppIndex);
        PlayerProxy_tryApplyEntry(pp, sCategoryInfos[entryIndex].category, getRealEntry(&sCategoryInfos[entryIndex]));
    } else {
        Logger_printWarning("applyRealEntry received invalid entryIndex %d", entryIndex);
    }
}

#if defined(ZELDA_ANDROID_BUILTIN_PMM)
static int getVisibleModelEntryCount(CategoryInfo *catInf) {
    size_t count = 0;
    const ModelEntry **modelEntries = ModelEntryManager_getCategoryEntryData(catInf->category, &count);
    int visibleCount = 1; // [None]

    for (size_t i = 0; i < count; ++i) {
        if (!ModelEntryManager_isEntryHidden(modelEntries[i])) {
            visibleCount++;
        }
    }

    return visibleCount;
}

static const ModelEntry *getVisibleModelEntryAt(CategoryInfo *catInf, int visibleIndex) {
    if (visibleIndex <= 0) {
        return NULL;
    }

    size_t count = 0;
    const ModelEntry **modelEntries = ModelEntryManager_getCategoryEntryData(catInf->category, &count);
    int cursor = 1;

    for (size_t i = 0; i < count; ++i) {
        if (ModelEntryManager_isEntryHidden(modelEntries[i])) {
            continue;
        }

        if (cursor == visibleIndex) {
            return modelEntries[i];
        }

        cursor++;
    }

    return NULL;
}

static void syncAndroidCompatBrowseIndexToRealEntry(CategoryInfo *catInf) {
    const ModelEntry *realEntry = getRealEntry(catInf);

    sAndroidCompatModelBrowseIndex = 0;
    if (!realEntry) {
        return;
    }

    int visibleCount = getVisibleModelEntryCount(catInf);
    for (int i = 1; i < visibleCount; ++i) {
        if (getVisibleModelEntryAt(catInf, i) == realEntry) {
            sAndroidCompatModelBrowseIndex = i;
            return;
        }
    }
}

static void applyAndroidCompatBrowsePreview(CategoryInfo *catInf) {
    const ModelEntry *entryOrNull = getVisibleModelEntryAt(catInf, sAndroidCompatModelBrowseIndex);
    PlayerModelManagerModelType modelType = catInf->category;
    PlayerModelManagerModelType entryType = entryOrNull ? ModelEntry_getType(entryOrNull) : PMM_MODEL_TYPE_NONE;
    const char *name = entryOrNull ? ModelEntry_getInternalName(entryOrNull) : "[None]";

    bool result = PlayerProxy_tryApplyEntry(getProxyFromIndex(catInf->ppIndex), modelType, entryOrNull);
    recomp_android_compat_pmm_apply_log(3, modelType, entryType, result ? 1 : 0, name);

    if (result || entryOrNull == NULL) {
        setRealEntry(catInf, entryOrNull);
        catInf->isNeedsDiskSave = true;
        refreshModelButtonEntryColors();
    }
}

static void handleAndroidCompatModelBrowse(GameState *state) {
    if (!sIsUIContextShown || !isSelectingModel()) {
        return;
    }

    Input *input = CONTROLLER1(state);
    CategoryInfo *catInf = getCurrentCategoryInfo();
    if (!catInf) {
        return;
    }

    if ((sAndroidCompatInputLogFrame++ % 60) == 0 ||
        input->press.button != 0 || input->rel.stick_y <= -30 || input->rel.stick_y >= 30) {
        recomp_android_compat_pmm_apply_log(4, input->press.button, input->rel.stick_x, input->rel.stick_y, "Input");
    }

    int delta = 0;
    if (CHECK_BTN_ALL(input->press.button, BTN_DDOWN)) {
        delta = 1;
        clearPressedInputButtons(input, BTN_DDOWN);
    } else if (CHECK_BTN_ALL(input->press.button, BTN_DUP)) {
        delta = -1;
        clearPressedInputButtons(input, BTN_DUP);
    } else if (!sAndroidCompatBrowseStickHeld && input->rel.stick_y <= -30) {
        delta = 1;
        sAndroidCompatBrowseStickHeld = true;
    } else if (!sAndroidCompatBrowseStickHeld && input->rel.stick_y >= 30) {
        delta = -1;
        sAndroidCompatBrowseStickHeld = true;
    }

    if (input->rel.stick_y > -20 && input->rel.stick_y < 20) {
        sAndroidCompatBrowseStickHeld = false;
    }

    if (delta == 0) {
        return;
    }

    int visibleCount = getVisibleModelEntryCount(catInf);
    if (visibleCount <= 0) {
        return;
    }

    sAndroidCompatModelBrowseIndex = (sAndroidCompatModelBrowseIndex + delta + visibleCount) % visibleCount;
    applyAndroidCompatBrowsePreview(catInf);
}
#endif

static void applyRealEntries(void) {
    for (int i = 0; i < ARRAY_COUNT(sCategoryInfos); ++i) {
        applyRealEntry(i);
    }
}

static void clearRealEntries(void) {
    for (int i = 0; i < ARRAY_COUNT(sCategoryInfos); ++i) {
        setRealEntry(&sCategoryInfos[i], NULL);
    }
}

static void fillRealEntries(void) {
    for (int i = 0; i < ARRAY_COUNT(sCategoryInfos); ++i) {
        CategoryInfo *catInf = &sCategoryInfos[i];
        setRealEntry(catInf, PlayerProxy_getCurrentEntry(getProxyFromIndex(catInf->ppIndex), catInf->category));
    }
}

static void removeAllModels(void) {
    for (int i = 0; i < ARRAY_COUNT(sCategoryInfos); ++i) {
        CategoryInfo *catInf = &sCategoryInfos[i];
        PlayerProxy_tryApplyEntry(getProxyFromIndex(catInf->ppIndex), catInf->category, NULL);
    }

    PlayerProxy_tryApplyEntry(gPlayer2Proxy, PMM_MODEL_TYPE_CHILD, ModelEntryManager_getEntry(gKafeiModelHandle));
}

static void removeEquipmentModels(void) {
    for (int i = 0; i < ARRAY_COUNT(sCategoryInfos); ++i) {
        CategoryInfo *catInf = &sCategoryInfos[i];
        if (Utils_isEquipmentModelType(catInf->category)) {
            PlayerProxy_tryApplyEntry(getProxyFromIndex(catInf->ppIndex), catInf->category, NULL);
        }
    }
}

static void saveAllModels(void) {
    for (int i = 0; i < ARRAY_COUNT(sCategoryInfos); ++i) {
        CategoryInfo *catInf = &sCategoryInfos[i];
        catInf->isNeedsDiskSave = true;
        setRealEntry(catInf, PlayerProxy_getCurrentEntry(getProxyFromIndex(catInf->ppIndex), catInf->category));
    }
}

static void removeAllModelsButtonPressed(RecompuiResource resource, const RecompuiEventData *data, void *userdata) {
    if (sIsUIContextShown) {
        if (data->type == UI_EVENT_CLICK) {
            Audio_PlaySfx(NA_SE_SY_DECIDE);
            removeAllModels();
            saveAllModels();
        }
    }
}

static void removeEquipmentModelsButtonPressed(RecompuiResource resource, const RecompuiEventData *data, void *userdata) {
    if (sIsUIContextShown) {
        if (data->type == UI_EVENT_CLICK) {
            if (isSelectingModel()) {
                CategoryInfo *catInf = getCurrentCategoryInfo();
                if (catInf) {
                    Audio_PlaySfx(NA_SE_SY_DECIDE);
                    applyRealEntries();
                    removeEquipmentModels();
                    saveAllModels();
                } else {
                    Audio_PlaySfx(NA_SE_SY_ERROR);
                }
            }
        } else if (data->type == UI_EVENT_FOCUS || data->type == UI_EVENT_HOVER) {
            destroyAuthor();

            if (shouldLivePreview()) {
                applyRealEntries();
                removeEquipmentModels();
            }
        }
    }
}

static void removePackButtonPressed(RecompuiResource resource, const RecompuiEventData *data, void *userdata) {
    if (sIsUIContextShown) {
        if (data->type == UI_EVENT_CLICK) {
            if (isSelectingModel()) {
                CategoryInfo *catInf = getCurrentCategoryInfo();
                if (catInf) {
                    Audio_PlaySfx(NA_SE_SY_DECIDE);
                    removeAllModels();
                    saveAllModels();
                } else {
                    Audio_PlaySfx(NA_SE_SY_ERROR);
                }
            }
        } else if (data->type == UI_EVENT_FOCUS || data->type == UI_EVENT_HOVER) {
            destroyAuthor();

            if (shouldLivePreview()) {
                removeAllModels();
            }
        }
    }
}

static bool sShouldClearAllButtonsNextFrame;

static void closeButtonPressed(RecompuiResource resource, const RecompuiEventData *data, void *userdata) {
    if (sIsUIContextShown) {
        if (data->type == UI_EVENT_CLICK) {
            recomp_android_compat_pmm_apply_log(9, 0, 0, 0, "Apply");
            applyRealEntries();
            recompui_hide_context(sUIContext);
            sIsUIContextShown = false;
            bool wasModelChanged = false;

            for (int i = 0; i < ARRAY_COUNT(sCategoryInfos); ++i) {
                CategoryInfo *catInf = &sCategoryInfos[i];

                if (catInf->isNeedsDiskSave) {
                    catInf->isNeedsDiskSave = false;
                    wasModelChanged = true;
                }
            }

            if (wasModelChanged) {
                Audio_PlaySfx(ModelEntryManager_saveModelsToDisk() ? NA_SE_SY_PIECE_OF_HEART : NA_SE_SY_ERROR);
            } else {
                Audio_PlaySfx(NA_SE_SY_DECIDE);
            }

            clearRealEntries();

            sShouldClearAllButtonsNextFrame = true;
        } else if (data->type == UI_EVENT_FOCUS || data->type == UI_EVENT_HOVER) {
            destroyAuthor();

            if (shouldLivePreview()) {
                applyRealEntries();
            }
        }
    }
}

static void destroyModelButtons();

void refreshCategoryName(void) {
    if (sLabelCategory) {
        recompui_destroy_element(sRowCategory, sLabelCategory);
        sLabelCategory = 0;
    }

    if (isSelectingCategory()) {
        sLabelCategory = recompui_create_label(sUIContext, sRowCategory, "Categories", LABELSTYLE_LARGE);

    } else if (isSelectingModel()) {
        CategoryInfo *catInf = getCurrentCategoryInfo();

        if (catInf) {
            sLabelCategory = recompui_create_label(sUIContext, sRowCategory, catInf->displayName, LABELSTYLE_LARGE);
        }
    }
}

static void incrementCurrentCategory(void) {
    CategoryInfo *currInf;

    do {
        sCurrentCategoryInfo = (sCurrentCategoryInfo + 1) % ARRAY_COUNT(sCategoryInfos);

        currInf = &sCategoryInfos[sCurrentCategoryInfo];
    } while (!(currInf->isVisible || sIsForceAllCategoriesVisible));
}

static void decrementCurrentCategory(void) {
    CategoryInfo *currInf;

    do {
        sCurrentCategoryInfo = (sCurrentCategoryInfo - 1 + ARRAY_COUNT(sCategoryInfos)) % ARRAY_COUNT(sCategoryInfos);

        currInf = &sCategoryInfos[sCurrentCategoryInfo];
    } while (!(currInf->isVisible || sIsForceAllCategoriesVisible));
}

static void changeCategoryButtonPressed(RecompuiResource resource, const RecompuiEventData *data, void *userdata) {
    if (sIsUIContextShown) {
        if (data->type == UI_EVENT_CLICK) {
            Audio_PlaySfx(NA_SE_SY_DECIDE);

            applyRealEntries();

            bool isNextButton = !!userdata;
            recomp_android_compat_pmm_apply_log(7, sCurrentCategoryInfo, 0, isNextButton ? 1 : 0, "Category");

            if (isNextButton) {
                incrementCurrentCategory();
            } else {
                decrementCurrentCategory();
            }

            requestRefreshFileList();
        } else if (data->type == UI_EVENT_FOCUS || data->type == UI_EVENT_HOVER) {
            destroyAuthor();

            if (shouldLivePreview()) {
                applyRealEntries();
            }
        }
    }
}

static void createNextPrevCategoryButtons(void) {
    sButtonCategoryPrev = recompui_create_button(sUIContext, sRowCategory, "◀", BUTTONSTYLE_SECONDARY);
    sButtonCategoryNext = recompui_create_button(sUIContext, sRowCategory, "▶", BUTTONSTYLE_SECONDARY);

    recompui_register_callback(sButtonCategoryPrev, changeCategoryButtonPressed, NULL);
    recompui_register_callback(sButtonCategoryNext, changeCategoryButtonPressed, (void *)1);
}

static void setupContainer(RecompuiResource container, float borderWidth, float borderRadius, RecompuiColor *borderColor, RecompuiColor *modalColor) {
    recompui_set_height(container, 100.f, UNIT_PERCENT);
    recompui_set_flex_grow(container, 1.0f);
    recompui_set_max_width(container, 33.f, UNIT_PERCENT);

    recompui_set_display(container, DISPLAY_FLEX);
    recompui_set_justify_content(container, JUSTIFY_CONTENT_FLEX_START);
    recompui_set_flex_direction(container, FLEX_DIRECTION_COLUMN);
    recompui_set_padding(container, 16.0f, UNIT_DP);
    recompui_set_gap(container, 16.0f, UNIT_DP);
    recompui_set_align_items(container, ALIGN_ITEMS_FLEX_START);

    recompui_set_border_width(container, borderWidth, UNIT_DP);
    recompui_set_border_radius(container, borderRadius, UNIT_DP);
    recompui_set_border_color(container, borderColor);
    recompui_set_background_color(container, modalColor);
}

static void setupScrollingList(RecompuiResource list) {
    recompui_set_flex_basis(list, 100.0f, UNIT_DP);
    recompui_set_flex_grow(list, 1.0f);
    recompui_set_flex_shrink(list, 0.0f);
    recompui_set_display(list, DISPLAY_FLEX);
    recompui_set_flex_direction(list, FLEX_DIRECTION_COLUMN);
    recompui_set_justify_content(list, JUSTIFY_CONTENT_FLEX_START);
    recompui_set_align_items(list, ALIGN_ITEMS_FLEX_START);
    recompui_set_gap(list, 16.0f, UNIT_DP);
    recompui_set_overflow_y(list, OVERFLOW_SCROLL);
    recompui_set_overflow_x(list, OVERFLOW_HIDDEN);
}

static void setupRow(RecompuiResource row) {
    recompui_set_flex_basis(row, 100.0f, UNIT_DP);
    recompui_set_flex_grow(row, 0);
    recompui_set_flex_shrink(row, 0);
    recompui_set_display(row, DISPLAY_FLEX);
    recompui_set_flex_direction(row, FLEX_DIRECTION_ROW);
    recompui_set_justify_content(row, JUSTIFY_CONTENT_FLEX_START);
    recompui_set_align_items(row, ALIGN_ITEMS_FLEX_END);
    recompui_set_gap(row, 16.0f, UNIT_DP);
}

static void setupAuthorRow(void) {
    sRowAuthor = recompui_create_element(sUIContext, sContainerMain);
    recompui_set_flex_basis(sRowAuthor, 100.0f, UNIT_DP);
    recompui_set_flex_grow(sRowAuthor, 0.0f);
    recompui_set_flex_shrink(sRowAuthor, 0.0f);
    recompui_set_display(sRowAuthor, DISPLAY_FLEX);
    recompui_set_flex_direction(sRowAuthor, FLEX_DIRECTION_ROW);
    recompui_set_justify_content(sRowAuthor, JUSTIFY_CONTENT_FLEX_START);
    recompui_set_align_items(sRowAuthor, ALIGN_ITEMS_FLEX_END);
    recompui_set_gap(sRowAuthor, 0.0f, UNIT_DP);
}

RECOMP_CALLBACK(".", _internal_preInitHashObjects) void initUIOnRecompInit(void) {
    RecompuiColor backgroundColor;
    backgroundColor.r = 255;
    backgroundColor.g = 255;
    backgroundColor.b = 255;
    backgroundColor.a = 0.1f * 255;

    RecompuiColor borderColor;
    borderColor.r = 255;
    borderColor.g = 255;
    borderColor.b = 255;
    borderColor.a = 0.2f * 255;

    RecompuiColor modalColor;
    modalColor.r = 8;
    modalColor.g = 7;
    modalColor.b = 13;
    modalColor.a = 0.9f * 255;

    const float bodyPadding = 64.0f;
    const float modalHeight = RECOMPUI_TOTAL_HEIGHT - (2 * bodyPadding);
    const float modalMaxWidth = modalHeight * (16.0f / 9.0f);
    const float modalBorderWidth = 1.1f;
    const float modalBorderRadius = 16.0f;

    sUIContext = recompui_create_context();
    recompui_open_context(sUIContext);

    sUIRoot = recompui_context_root(sUIContext);
    // Set up the sUIRoot element so it takes up the full screen.
    recompui_set_position(sUIRoot, POSITION_ABSOLUTE);
    recompui_set_top(sUIRoot, 0, UNIT_DP);
    recompui_set_right(sUIRoot, 0, UNIT_DP);
    recompui_set_bottom(sUIRoot, 0, UNIT_DP);
    recompui_set_left(sUIRoot, 0, UNIT_DP);
    recompui_set_width_auto(sUIRoot);
    recompui_set_height_auto(sUIRoot);

    // Set up the sUIRoot element's padding so the modal contents don't touch the screen edges.
    recompui_set_padding(sUIRoot, bodyPadding, UNIT_DP);

    // Set up the flexbox properties of the sUIRoot element.
    recompui_set_flex_direction(sUIRoot, FLEX_DIRECTION_ROW);
    recompui_set_justify_content(sUIRoot, JUSTIFY_CONTENT_FLEX_START);
    recompui_set_align_items(sUIRoot, ALIGN_ITEMS_FLEX_START);

    sContainerMain = recompui_create_element(sUIContext, sUIRoot);

    setupContainer(sContainerMain, modalBorderWidth, modalBorderRadius, &borderColor, &modalColor);

    sRowTop = recompui_create_element(sUIContext, sContainerMain);
    setupRow(sRowTop);

    sRowCategory = recompui_create_element(sUIContext, sContainerMain);
    setupRow(sRowCategory);
    createNextPrevCategoryButtons();

    sButtonClose = recompui_create_button(sUIContext, sRowTop, "Apply", BUTTONSTYLE_SECONDARY);
    recompui_set_text_align(sButtonClose, TEXT_ALIGN_CENTER);

    sButtonCategoriesShow = recompui_create_button(sUIContext, sRowTop, "Show Categories", BUTTONSTYLE_SECONDARY);
    recompui_register_callback(sButtonCategoriesShow, onUpOneLevelButtonPressed, NULL);

    recompui_register_callback(sButtonClose, closeButtonPressed, NULL);

    sModelListElement = recompui_create_element(sUIContext, sContainerMain);
    setupScrollingList(sModelListElement);

    // TODO: Restore author preview after UI crashes are mitigated in Recomp
    // setupAuthorRow();

    // Setup category container
    sContainerCategories = recompui_create_element(sUIContext, sUIRoot);

    setupContainer(sContainerCategories, modalBorderWidth, modalBorderRadius, &borderColor, &modalColor);

    sCategoriesTopRow = recompui_create_element(sUIContext, sContainerCategories);
    setupRow(sCategoriesTopRow);

    sButtonCategoriesHide = recompui_create_button(sUIContext, sCategoriesTopRow, "Hide Cat.", BUTTONSTYLE_SECONDARY);
    recompui_register_callback(sButtonCategoriesHide, onUpOneLevelButtonPressed, NULL);

    sButtonRemoveAllModels = recompui_create_button(sUIContext, sCategoriesTopRow, "[Remove All]", BUTTONSTYLE_SECONDARY);
    recompui_register_callback(sButtonRemoveAllModels, removeAllModelsButtonPressed, NULL);
    setButtonColor(sButtonRemoveAllModels, &sModelRemovedButtonColor);

    sCategoriesListElement = recompui_create_element(sUIContext, sContainerCategories);
    setupScrollingList(sCategoriesListElement);

    recompui_set_visibility(sContainerCategories, VISIBILITY_HIDDEN);
    sIsContainerCategoriesVisible = false;

    recompui_set_nav(sButtonCategoriesShow, NAVDIRECTION_RIGHT, sButtonCategoryPrev);
    recompui_set_nav(sButtonCategoriesShow, NAVDIRECTION_DOWN, sButtonCategoryNext);

    recompui_set_nav(sButtonCategoryNext, NAVDIRECTION_UP, sButtonCategoriesShow);

    recompui_set_nav(sButtonCategoryPrev, NAVDIRECTION_LEFT, sButtonCategoriesShow);
    recompui_set_nav(sButtonCategoryPrev, NAVDIRECTION_UP, sButtonClose);

    recompui_set_nav(sButtonClose, NAVDIRECTION_DOWN, sButtonCategoryPrev);
    recompui_set_nav(sButtonClose, NAVDIRECTION_RIGHT, sButtonCategoriesShow);
    recompui_set_nav(sButtonClose, NAVDIRECTION_DOWN, sButtonCategoryPrev);

    recompui_close_context(sUIContext);

    sIsUIContextShown = false;
}

static void onModelButtonPressed(RecompuiResource resource, const RecompuiEventData *data, void *userdata) {
    if (sIsUIContextShown) {
        ModelEntry *entryOrNull = userdata;

        CategoryInfo *catInf = getCurrentCategoryInfo();

        if (catInf) {
            PlayerModelManagerModelType modelType = catInf->category;

            if (data->type == UI_EVENT_CLICK) {
                const char* name = entryOrNull ? ModelEntry_getInternalName(entryOrNull) : "[None]";
                PlayerModelManagerModelType entryType = entryOrNull ? ModelEntry_getType(entryOrNull) : PMM_MODEL_TYPE_NONE;
                bool result;

                Audio_PlaySfx(NA_SE_SY_DECIDE);

                result = PlayerProxy_tryApplyEntry(getProxyFromIndex(catInf->ppIndex), modelType, entryOrNull);
                recomp_android_compat_pmm_apply_log(1, modelType, entryType, result ? 1 : 0, name);

                setRealEntry(catInf, entryOrNull);

                refreshModelButtonEntryColors();

                catInf->isNeedsDiskSave = true;
            } else if (data->type == UI_EVENT_FOCUS || data->type == UI_EVENT_HOVER) {
                const char* name = entryOrNull ? ModelEntry_getInternalName(entryOrNull) : "[None]";
                PlayerModelManagerModelType entryType = entryOrNull ? ModelEntry_getType(entryOrNull) : PMM_MODEL_TYPE_NONE;

                if (entryOrNull) {
                    setAuthor(ModelEntry_getAuthorName(entryOrNull));
                } else {
                    destroyAuthor();
                }

                if (shouldLivePreview()) {
                    bool result;

                    applyRealEntries();
                    result = PlayerProxy_tryApplyEntry(getProxyFromIndex(catInf->ppIndex), modelType, entryOrNull);
                    recomp_android_compat_pmm_apply_log(2, modelType, entryType, result ? 1 : 0, name);
#if defined(ZELDA_ANDROID_BUILTIN_PMM)
                    if (result || entryOrNull == NULL) {
                        setRealEntry(catInf, entryOrNull);
                        catInf->isNeedsDiskSave = true;
                        refreshModelButtonEntryColors();
                    }
#endif
                }
            }
        }
    }
}

static void onKafeiRemoveButtonPressed(RecompuiResource resource, const RecompuiEventData *data, void *userdata) {
    onModelButtonPressed(resource, data, ModelEntryManager_getEntry(gKafeiModelHandle));
}

static void onPackButtonPressed(RecompuiResource resource, const RecompuiEventData *data, void *userdata) {
    if (sIsUIContextShown) {
        ModelEntry *entryOrNull = userdata;

        CategoryInfo *catInf = getCurrentCategoryInfo();

        if (catInf) {
            PlayerModelManagerModelType modelType = catInf->category;
            PlayerProxy *pp = getProxyFromIndex(catInf->ppIndex);

            if (data->type == UI_EVENT_CLICK) {
                Audio_PlaySfx(NA_SE_SY_DECIDE);
                PlayerProxy_tryApplyEntry(pp, modelType, entryOrNull);
                saveAllModels();
            } else if (data->type == UI_EVENT_FOCUS || data->type == UI_EVENT_HOVER) {
                if (entryOrNull) {
                    setAuthor(ModelEntry_getAuthorName(entryOrNull));
                } else {
                    destroyAuthor();
                }

                if (shouldLivePreview()) {
                    applyRealEntries();
                    PlayerProxy_tryApplyEntry(pp, modelType, entryOrNull);
                }
            }
        }
    }
}

static void onCategoryButtonPressed(RecompuiResource resource, const RecompuiEventData *data, void *userdata) {
    if (sIsUIContextShown) {
        if (data->type == UI_EVENT_CLICK) {
            CategoryInfo *catInf = userdata;

            if (catInf && isValidCategoryInfoIndex(catInf->index)) {
                Audio_PlaySfx(NA_SE_SY_DECIDE);
                sCurrentCategoryInfo = catInf->index;
                requestRefreshFileList();
            } else {
                Audio_PlaySfx(NA_SE_SY_ERROR);
            }
        }
    }
}

static void onUpOneLevelButtonPressed(RecompuiResource resource, const RecompuiEventData *data, void *userdata) {
    if (sIsUIContextShown) {
        if (data->type == UI_EVENT_CLICK) {
            Audio_PlaySfx(NA_SE_SY_DECIDE);
            recomp_android_compat_pmm_apply_log(8, sCurrentCategoryInfo, 0, sIsContainerCategoriesVisible ? 0 : 1, "Categories");

            sIsContainerCategoriesVisible = !sIsContainerCategoriesVisible;

            if (sIsContainerCategoriesVisible) {
                recompui_set_visibility(sContainerCategories, VISIBILITY_VISIBLE);
                recompui_set_visibility(sButtonCategoriesShow, VISIBILITY_HIDDEN);
                recompui_set_nav(sButtonClose, NAVDIRECTION_RIGHT, sButtonCategoriesHide);
                recompui_set_nav(sButtonCategoryNext, NAVDIRECTION_UP, sButtonClose);
            } else {
                recompui_set_visibility(sContainerCategories, VISIBILITY_HIDDEN);
                recompui_set_visibility(sButtonCategoriesShow, VISIBILITY_VISIBLE);
                recompui_set_nav(sButtonClose, NAVDIRECTION_RIGHT, sButtonCategoriesShow);
                recompui_set_nav(sButtonCategoryNext, NAVDIRECTION_UP, sButtonCategoriesShow);
            }
        } else if (data->type == UI_EVENT_FOCUS || data->type == UI_EVENT_HOVER) {
            destroyAuthor();

            if (shouldLivePreview()) {
                applyRealEntries();
            }
        }
    }
}

static void destroyModelButtons(void) {
    size_t count = getCurrentModelButtonArraySize();
    const RecompuiResource *buttons = getCurrentModelButtonArray();

    for (size_t i = 0; i < count; ++i) {
        recomputil_u32_value_hashmap_erase(sModelButtonsToData, buttons[i]);
        recompui_destroy_element(sModelListElement, buttons[i]);
    }

    YAZMTCore_IterableU32Set_clear(sModelListButtons);
}

static void createModelListButtons(void) {
    CategoryInfo *catInf = getCurrentCategoryInfo();
    RecompuiEventHandler *pressedCallback = onModelButtonPressed;
    RecompuiEventHandler *removedCallback = onModelButtonPressed;
    const char *removeText = "[None]";

    if (catInf->ppIndex == PP_IDX_KAFEI) {
        removedCallback = onKafeiRemoveButtonPressed;
    }

    bool isPack = Utils_isPackModelType(catInf->category);
    if (isPack) {
        pressedCallback = onPackButtonPressed;
        removedCallback = removeEquipmentModelsButtonPressed;
        removeText = "[Remove Equipment Models]";
    }

    RecompuiResource removeModelButton = createAndPushButtonToList(sUIContext, sModelListElement, removeText, BUTTONSTYLE_PRIMARY, sModelListButtons);
    setButtonColor(removeModelButton, &sModelRemovedButtonColor);
    recompui_register_callback(removeModelButton, removedCallback, NULL);

    size_t count = 0;
    const ModelEntry **modelEntries = ModelEntryManager_getCategoryEntryData(catInf->category, &count);

    for (size_t i = 0; i < count; ++i) {
        if (!ModelEntryManager_isEntryHidden(modelEntries[i])) {
            const char *name = NULL;

            name = ModelEntry_getDisplayName(modelEntries[i]);
            if (!name) {
                name = ModelEntry_getInternalName(modelEntries[i]);

                if (!name) {
                    name = "ERROR READING MODEL ENTRY NAME";
                }
            }

            RecompuiResource modelButton = createAndPushButtonToList(sUIContext, sModelListElement, name, BUTTONSTYLE_PRIMARY, sModelListButtons);

            // Need to cast away constness due to API signature
            // function in pressedCallback should not modify the ModelEntry, though
            recompui_register_callback(modelButton, pressedCallback, (ModelEntry *)modelEntries[i]);

            setListButtonData(sModelButtonsToData, modelButton, modelEntries[i]);
        }
    }

    connectListBoxButtons(getCurrentModelButtonArray(), getCurrentModelButtonArraySize());
}

static void requestRefreshFileList(void) {
    sIsFileListRefreshRequested = true;
}

static void refreshFileList(void) {
    // MUST CALL INSIDE UI CONTEXT

    destroyModelButtons();

    if (isSelectingModel()) {
        createModelListButtons();

#if defined(ZELDA_ANDROID_BUILTIN_PMM)
        syncAndroidCompatBrowseIndexToRealEntry(getCurrentCategoryInfo());
#endif
        refreshModelButtonEntryColors();
        refreshCategoryButtonEntryColors();

        size_t listCount = getCurrentModelButtonArraySize();

        if (listCount > 0) {
            const RecompuiResource *buttons = getCurrentModelButtonArray();
            RecompuiResource first = buttons[0];
            RecompuiResource last = buttons[listCount - 1];

            recompui_set_nav(first, NAVDIRECTION_UP, sButtonCategoryPrev);
            recompui_set_nav(first, NAVDIRECTION_LEFT, sButtonCategoryNext);

            recompui_set_nav(sButtonCategoryNext, NAVDIRECTION_DOWN, first);
            recompui_set_nav(sButtonCategoryNext, NAVDIRECTION_RIGHT, first);

            recompui_set_nav(sButtonCategoryPrev, NAVDIRECTION_DOWN, first);

            recompui_set_nav(sButtonClose, NAVDIRECTION_UP, last);

            recompui_set_nav(last, NAVDIRECTION_DOWN, sButtonClose);

            recompui_set_nav(sButtonCategoriesShow, NAVDIRECTION_UP, last);
        } else {
            recompui_set_nav(sButtonCategoryNext, NAVDIRECTION_DOWN, sButtonCategoriesShow);

            recompui_set_nav(sButtonCategoryPrev, NAVDIRECTION_DOWN, sButtonClose);

            recompui_set_nav(sButtonClose, NAVDIRECTION_UP, sButtonCategoryPrev);

            recompui_set_nav(sButtonCategoriesShow, NAVDIRECTION_UP, sButtonCategoryNext);
        }
    }
    refreshCategoryName();
    applyRealEntries();
}

typedef enum {
    MODCFG_BUTTON_COMBO_NONE,
    MODCFG_BUTTON_COMBO_LR,
    MODCFG_BUTTON_COMBO_LA,
} ModConfig_ButtonCombo;

static void clearPressedInputButtons(Input *input, u16 buttons) {
    input->press.button &= (~buttons);
}

static bool isOpenMenuComboPressed(GameState *state) {
    Input *input = CONTROLLER1(state);

    switch (recomp_get_config_u32("open_menu_buttons")) {
        case MODCFG_BUTTON_COMBO_LR:
            if ((CHECK_BTN_ALL(input->press.button, BTN_L) && CHECK_BTN_ALL(input->cur.button, BTN_R)) ||
                (CHECK_BTN_ALL(input->cur.button, BTN_L) && CHECK_BTN_ALL(input->press.button, BTN_R))) {
                clearPressedInputButtons(input, BTN_L | BTN_R);
                return true;
            }
            break;

        case MODCFG_BUTTON_COMBO_LA:

            if ((CHECK_BTN_ALL(input->press.button, BTN_L) && CHECK_BTN_ALL(input->cur.button, BTN_A)) ||
                (CHECK_BTN_ALL(input->cur.button, BTN_L) && CHECK_BTN_ALL(input->press.button, BTN_A))) {
                clearPressedInputButtons(input, BTN_L | BTN_A);
                return true;
            }
            break;

        default:
            break;
    }

    return false;
}

static void openModelMenu(void) {
    if (!sIsUIContextShown) {
        sIsLivePreviewEnabled = recomp_get_config_u32("is_live_preview_enabled");
        fillRealEntries();
        recompui_show_context(sUIContext);
        sIsUIContextShown = true;
    }
}

static bool sIsModelManagerReady;

RECOMP_CALLBACK(".", onReady) void allowUIOnReady(void) {
    sIsModelManagerReady = true;
}

// Hook Play_UpdateMain to check if the L button is pressed and show this mod's UI if so.
void handleUIRequests_on_GameState_Update(GameState *state) {
    if (!sIsModelManagerReady) {
        return;
    }

    if (isOpenMenuComboPressed(state)) {
        openModelMenu();
    }

#if defined(ZELDA_ANDROID_BUILTIN_PMM)
    handleAndroidCompatModelBrowse(state);
    if (sIsUIContextShown) {
        processPlayerProxyRefreshRequests_on_Play_UpdateMain();
        processFormProxyRefreshRequests_on_Play_UpdateMain();
    }
#endif

    if (sIsFileListRefreshRequested && sIsUIContextShown) {
        sIsFileListRefreshRequested = false;
        recompui_open_context(sUIContext);
        refreshFileList();
        recompui_close_context(sUIContext);
    }

    if (sShouldClearAllButtonsNextFrame) {
        clearPressedInputButtons(CONTROLLER1(state), 0xFFFFU);
    }

    sShouldClearAllButtonsNextFrame = false;
}

void initUIFileList(void) {
    recompui_open_context(sUIContext);

    for (int i = 0; i < ARRAY_COUNT(sCategoryInfos); ++i) {
        CategoryInfo *catInf = &sCategoryInfos[i];
        size_t count = 0;
        ModelEntryManager_getCategoryEntryData(catInf->category, &count);
        catInf->isVisible = catInf->isVisible || (catInf->isUsedByCurrentGame && count > 0);

        if (catInf->isVisible) {
            RecompuiResource button = createAndPushButtonToList(sUIContext, sCategoriesListElement, catInf->displayName, BUTTONSTYLE_PRIMARY, sCategoryListButtons);
            recompui_register_callback(button, onCategoryButtonPressed, catInf);
            setListButtonData(sCategoryButtonsToData, button, catInf);
        }
    }

    const RecompuiResource *catButtons = getCategoryButtonArray();
    size_t catButtonsNum = getCategoryButtonArraySize();
    connectListBoxButtons(catButtons, catButtonsNum);
    recompui_set_nav(sButtonCategoriesHide, NAVDIRECTION_DOWN, catButtons[0]);
    recompui_set_nav(sButtonCategoriesHide, NAVDIRECTION_RIGHT, sButtonRemoveAllModels);
    recompui_set_nav(sButtonCategoriesHide, NAVDIRECTION_UP, catButtons[catButtonsNum - 1]);
    recompui_set_nav(sButtonCategoriesHide, NAVDIRECTION_LEFT, sButtonClose);

    recompui_set_nav(sButtonRemoveAllModels, NAVDIRECTION_DOWN, catButtons[0]);
    recompui_set_nav(sButtonRemoveAllModels, NAVDIRECTION_RIGHT, catButtons[0]);
    recompui_set_nav(sButtonRemoveAllModels, NAVDIRECTION_UP, catButtons[catButtonsNum - 1]);
    recompui_set_nav(sButtonRemoveAllModels, NAVDIRECTION_LEFT, sButtonCategoriesHide);

    recompui_set_nav(catButtons[0], NAVDIRECTION_LEFT, sButtonRemoveAllModels);
    recompui_set_nav(catButtons[0], NAVDIRECTION_UP, sButtonCategoriesHide);

    recompui_set_nav(catButtons[catButtonsNum - 1], NAVDIRECTION_DOWN, sButtonCategoriesHide);

    requestRefreshFileList();
    recompui_close_context(sUIContext);
}

RECOMP_CALLBACK(".", _internal_initHashObjects) void initUiObjects(void) {
    sModelButtonsToData = recomputil_create_u32_value_hashmap();
    sCategoryButtonsToData = recomputil_create_u32_value_hashmap();
    sModelListButtons = YAZMTCore_IterableU32Set_new();
    sCategoryListButtons = YAZMTCore_IterableU32Set_new();
}

RECOMP_CALLBACK(".", _internal_preInitHashObjects) void preInitUiObjects(void) {
    for (int i = 0; i < ARRAY_COUNT(sCategoryInfos); ++i) {
        sCategoryInfos[i].index = i;
    }
}
