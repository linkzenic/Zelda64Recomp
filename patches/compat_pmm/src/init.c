#include "global.h"
#include "modding.h"
#include "globalobjects_api.h"
#include "playerproxy.h"
#include "formproxy.h"
#include "playerproxymanager.h"
#include "apilocal.h"
#include "logger.h"
#include "modelinfo.h"
#include "modelentrymanager.h"
#include "fallbackmodels.h"
#include "repy_api.h"

static bool sIsGlobalObjectsReady;
static bool sIsAllHashObjectsInitialized;
static bool sIsREPYReady;

static void doRegisterModels(void);

void setupLoggerFirstTime(void);
void initUIOnRecompInit(void);
void preInitUiObjects(void);
void handleFormProxyExtensionInits(void);
void initMirrorShieldActorExtensions(void);
void initFormProxyExDLs(void);
void initPPMHash(void);
void initPlayerProxyHash(void);
void initFormProxyObjects(void);
void initUiObjects(void);
void initModelEntryObjects(void);
void initCMEMHash(void);
void initGfxHookReplacmentMaps(void);
void initFormIdArr(void);
void initPlayerProxyVars(void);
void registerMMEquipment(void);
void allowUIOnReady(void);

void registerHuman(void);
void registerDeku(void);
void registerGoron(void);
void registerZora(void);
void registerFierceDeity(void);
void registerKafei(void);
void initObjectManager(void);
#if defined(ZELDA_ANDROID_BUILTIN_PMM)
void registerDiskModels(void);
void initVanillaProps_on_Player_Init(void);
#endif

#if defined(ZELDA_ANDROID_BUILTIN_PMM)
static void init_sValidModelEntries(void) {}
static void init_sPlayerProxyPtrSet(void) {}
static void init_sValidFormProxySet(void) {}
static void init_sValidModelInfoPtrSet(void) {}
#else
void init_sValidModelEntries(void);
void init_sPlayerProxyPtrSet(void);
void init_sValidFormProxySet(void);
void init_sValidModelInfoPtrSet(void);
#endif

// initialize player models as blank display lists
static void forceInitFormProxies(void) {
    registerHuman();
    ModelInfo_setModelEntryForm(&gHumanModelInfo, (ModelEntryForm *)ModelEntryManager_getEntry(gHumanModelHandle));
    registerDeku();
    ModelInfo_setModelEntryForm(&gDekuModelInfo, (ModelEntryForm *)ModelEntryManager_getEntry(gDekuModelHandle));
    registerGoron();
    ModelInfo_setModelEntryForm(&gGoronModelInfo, (ModelEntryForm *)ModelEntryManager_getEntry(gGoronModelHandle));
    registerZora();
    ModelInfo_setModelEntryForm(&gZoraModelInfo, (ModelEntryForm *)ModelEntryManager_getEntry(gZoraModelHandle));
    registerFierceDeity();
    ModelInfo_setModelEntryForm(&gFierceDeityModelInfo, (ModelEntryForm *)ModelEntryManager_getEntry(gFierceDeityModelHandle));

    registerKafei();

    gPlayer1ProxyHandle = PlayerProxyManager_createPlayerProxy(PPALLOC_PERMANENT);
    gPlayer1Proxy = PlayerProxyManager_getPlayerProxy(gPlayer1ProxyHandle);

    gPlayer2ProxyHandle = PlayerProxyManager_createPlayerProxy(PPALLOC_PERMANENT);
    gPlayer2Proxy = PlayerProxyManager_getPlayerProxy(gPlayer2ProxyHandle);
}

static void initProxies(void) {
    static bool isFormProxiesInitialized;

    if (!isFormProxiesInitialized) {
        isFormProxiesInitialized = true;
        forceInitFormProxies();
    }
}

void initVanillaMMDLs(void);
void initCustomDLs(void);

GLOBAL_OBJECTS_CALLBACK_ON_READY void initCustomDLsOnGlobalObjects(void) {
    initCustomDLs();
    initVanillaMMDLs();
    sIsGlobalObjectsReady = true;
    doRegisterModels();
}

RECOMP_DECLARE_EVENT(_internal_preInitHashObjects(void));
RECOMP_DECLARE_EVENT(_internal_initHashObjects(void));
RECOMP_DECLARE_EVENT(_internal_postInitHashObjects(void));

static void handleObjectInits(void) {
    setupLoggerFirstTime();
    initUIOnRecompInit();
    preInitUiObjects();
    handleFormProxyExtensionInits();
    initMirrorShieldActorExtensions();
    initFormProxyExDLs();

    // @recomp_event _internal_preInitHashObjects(): Player Model Manager internal setup before hash object creation.
    _internal_preInitHashObjects();

    init_sValidModelEntries();
    init_sPlayerProxyPtrSet();
    init_sValidFormProxySet();
    init_sValidModelInfoPtrSet();
    initPPMHash();
    initFormProxyObjects();
    initPlayerProxyHash();
    initUiObjects();
    initModelEntryObjects();
    initCMEMHash();
    initGfxHookReplacmentMaps();
    initFormIdArr();

    // @recomp_event _internal_initHashObjects(): Player Model Manager internal hash object creation.
    _internal_initHashObjects();

    initPlayerProxyVars();

    // @recomp_event _internal_postInitHashObjects(): Player Model Manager internal setup after hash object creation.
    _internal_postInitHashObjects();
    sIsAllHashObjectsInitialized = true;
    doRegisterModels();
}

REPY_ON_POST_INIT void onREPYReady(void) {
    sIsREPYReady = true;
    handleObjectInits();
    doRegisterModels();
}

#if defined(ZELDA_ANDROID_BUILTIN_PMM)
void zelda_builtin_pmm_after_main_init(void) {
    static bool initialized;

    if (initialized) {
        return;
    }

    initialized = true;
    sIsREPYReady = true;
    initObjectManager();
    initVanillaProps_on_Player_Init();
    initCustomDLsOnGlobalObjects();
    handleObjectInits();
}
#endif

RECOMP_DECLARE_EVENT(onRegisterModels(void));
RECOMP_DECLARE_EVENT(onReady(void));

void initUIFileList(void);

#define ID_PREFIX "mm."
#define PLAYER1_CFG_ID ID_PREFIX "player1"
#define PLAYER2_CFG_ID ID_PREFIX "kafei"

static void applyDiskModelsNow(void) {
    ModelEntryManager_applySavedEntriesToProxy(gPlayer1Proxy, PLAYER1_CFG_ID);

    ModelEntryManager_applySavedEntriesToProxy(gPlayer2Proxy, PLAYER2_CFG_ID);
    if (!PlayerProxy_getCurrentEntry(gPlayer2Proxy, PMM_MODEL_TYPE_CHILD)) {
        PlayerProxy_tryApplyEntry(gPlayer2Proxy, PMM_MODEL_TYPE_CHILD, ModelEntryManager_getEntry(gKafeiModelHandle));
    }
}

static void applyDiskModels(void) {
#if defined(ZELDA_ANDROID_BUILTIN_PMM)
    // The baked Android path needs a live player draw before applying saved models.
    // Applying here can leave the form proxy mapped but invisible until the PMM UI opens.
#else
    applyDiskModelsNow();
#endif
}

#if defined(ZELDA_ANDROID_BUILTIN_PMM)
void PlayerModelManager_applyPendingDiskModels(void) {
    static bool applied;

    if (applied) {
        return;
    }

    applied = true;
    applyDiskModelsNow();
}
#endif

static void doRegisterModels(void) {
    static bool isModelsRegistered;

    if (isModelsRegistered || !sIsAllHashObjectsInitialized || !sIsGlobalObjectsReady || !sIsREPYReady) {
        return;
    }

    isModelsRegistered = true;

    PlayerModelManager_unlockAPI();

    Logger_printInfo("Setting up vanilla models...");
    initProxies();
    ModelEntryManager_registerProxyToSave(gPlayer1Proxy, PLAYER1_CFG_ID);
    ModelEntryManager_registerProxyToSave(gPlayer2Proxy, PLAYER2_CFG_ID);

    Logger_printInfo("Registering custom models...");
    registerMMEquipment();

#if defined(ZELDA_ANDROID_BUILTIN_PMM)
    registerDiskModels();
#endif

    // @recomp_event onRegisterModels(): Allow built-in Player Model Manager model packs to register models.
    onRegisterModels();

    PlayerModelManager_lockAPI();

    initUIFileList();
    Logger_printInfo("Finished registering models.");

    PlayerProxyManager_refreshAll();

    applyDiskModels();

    Logger_printInfo("Ready!");

    allowUIOnReady();

    // @recomp_event onReady(): Player Model Manager has finished registering and applying models.
    onReady();
}
