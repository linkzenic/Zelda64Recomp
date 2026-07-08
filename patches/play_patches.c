#include "play_patches.h"
#include "z64debug_display.h"
#include "input.h"
#include "prevent_bss_reordering.h"
#include "z64.h"
#include "regs.h"
#include "functions.h"
#include "z64vismono.h"
#include "z64visfbuf.h"
#include "buffers.h"

#include "variables.h"
#include "macros.h"
#include "idle.h"
#include "sys_cfb.h"
#include "z64bombers_notebook.h"
#include "z64quake.h"
#include "z64rumble.h"
#include "z64shrink_window.h"
#include "z64view.h"
#include "z64horse.h"
#include "z64malloc.h"
#include "loadfragment.h"
#include "save_editor_builtin.h"
#include "misc_funcs.h"

#include "overlays/gamestates/ovl_daytelop/z_daytelop.h"
#include "overlays/gamestates/ovl_opening/z_opening.h"
#include "overlays/gamestates/ovl_file_choose/z_file_select.h"
#include "overlays/kaleido_scope/ovl_kaleido_scope/z_kaleido_scope.h"
#include "debug.h"

extern Input D_801F6C18;
extern SceneEntranceTableEntry sSceneEntranceTable[];
extern ActorInit Player_InitVars;

RECOMP_DECLARE_EVENT(recomp_on_play_main(PlayState* play));
RECOMP_DECLARE_EVENT(recomp_on_play_update(PlayState* play));
RECOMP_DECLARE_EVENT(recomp_after_play_update(PlayState* play));

static void* AndroidDiag_ToRdramAlias(void* ptr);
static void compat_pmm_play_update(PlayState* play);
#if defined(ZELDA_ANDROID_BUILTIN_PMM)
void handleUIRequests_on_GameState_Update(GameState* state);
void updatePlayerProxyManager_on_UpdateMain(void);
void updateAdultProperties_on_Play_UpdateMain(PlayState* play);
void processPlayerProxyRefreshRequests_on_Play_UpdateMain(void);
void processFormProxyRefreshRequests_on_Play_UpdateMain(void);
void setAdultBootData_on_func_80123140(PlayState* play, Player* player);
void setAdultBootData_on_return_func_80123140(void);
bool PlayerModelManager_shouldUseAdultFixes(Player* player);
void adjustAdultLeg_on_Player_AdjustSingleLeg(Player* player);
void adjustAdultLeg_on_return_Player_AdjustSingleLeg(void);
#endif
#if defined(ZELDA_ANDROID_BUILTIN_CHEATS)
void CompatCheats_OnPlayInit(PlayState* play);
void CompatCheats_OnPlayMain(PlayState* play);
#endif

void controls_play_update(PlayState* play) {
    gSaveContext.options.zTargetSetting = recomp_get_targeting_mode();
}

RECOMP_PATCH ActorInit* Actor_LoadOverlay(ActorContext* actorCtx, s16 index) {
    size_t overlaySize;
    ActorOverlay* overlayEntry;
    ActorInit* actorInit;
    s32 samsungDiag = recomp_android_should_use_sync_boot_dma();

    if (samsungDiag && ((index < 0) || (index >= gMaxActorId) || (index >= ACTOR_ID_MAX))) {
        recomp_measure_latency(98, 0x80, (u32)(s32)index, (u32)gMaxActorId, 0);
        return NULL;
    }

    overlayEntry = &gActorOverlayTable[index];
    overlaySize = (uintptr_t)overlayEntry->vramEnd - (uintptr_t)overlayEntry->vramStart;

    if (samsungDiag && (overlayEntry->vramStart != NULL)) {
        if ((overlayEntry->vromStart >= overlayEntry->vromEnd) ||
            ((uintptr_t)overlayEntry->vramStart >= (uintptr_t)overlayEntry->vramEnd) ||
            (overlaySize == 0) || (overlaySize > 0x200000) ||
            (DmaMgr_FindDmaEntry(overlayEntry->vromStart) == NULL)) {
            recomp_measure_latency(98, 0x81, (u32)(s32)index, (u32)overlayEntry->vromStart,
                                   (u32)overlayEntry->vromEnd);
            recomp_measure_latency(98, 0x82, (u32)(uintptr_t)overlayEntry->vramStart,
                                   (u32)(uintptr_t)overlayEntry->vramEnd, (u32)overlaySize);
            return NULL;
        }
    }

    if (overlayEntry->vramStart == NULL) {
        actorInit = overlayEntry->initInfo;
    } else {
        if (overlayEntry->loadedRamAddr == NULL) {
            if (overlayEntry->allocType & ALLOCTYPE_ABSOLUTE) {
                if (actorCtx->absoluteSpace == NULL) {
                    actorCtx->absoluteSpace = ZeldaArena_MallocR(AM_FIELD_SIZE);
                }
                gActorOverlayTable[index].loadedRamAddr = actorCtx->absoluteSpace;
            } else if (overlayEntry->allocType & ALLOCTYPE_PERMANENT) {
                gActorOverlayTable[index].loadedRamAddr = ZeldaArena_MallocR(overlaySize);
            } else {
                gActorOverlayTable[index].loadedRamAddr = ZeldaArena_Malloc(overlaySize);
            }

            if (overlayEntry->loadedRamAddr == NULL) {
                return NULL;
            }

            Overlay_Load(overlayEntry->vromStart, overlayEntry->vromEnd, overlayEntry->vramStart, overlayEntry->vramEnd,
                         overlayEntry->loadedRamAddr);
            overlayEntry->numLoaded = 0;
        }

        actorInit = (void*)(uintptr_t)((overlayEntry->initInfo != NULL)
                                           ? (void*)((uintptr_t)overlayEntry->initInfo -
                                                     (intptr_t)((uintptr_t)overlayEntry->vramStart -
                                                                (uintptr_t)overlayEntry->loadedRamAddr))
                                           : NULL);
    }

    return actorInit;
}

// @recomp Patched to add hooks for various added functionality.
RECOMP_PATCH void Play_Main(GameState* thisx) {
    static Input* prevInput = NULL;
    PlayState* this = (PlayState*)thisx;

    // @recomp_event recomp_on_play_main(PlayState* play): Allow mods to execute code every frame.
    recomp_on_play_main(this);

#if defined(ZELDA_ANDROID_BUILTIN_PMM)
    handleUIRequests_on_GameState_Update(&this->state);
#endif
#if defined(ZELDA_ANDROID_BUILTIN_CHEATS)
    CompatCheats_OnPlayMain(this);
#endif

    // @recomp
    debug_play_update(this);
    controls_play_update(this);
    save_editor_builtin_play_update(this);
    compat_pmm_play_update(this);
    analog_cam_pre_play_update(this);
    matrix_play_update(this);
    
    // @recomp avoid unused variable warning
    (void)prevInput;

    prevInput = CONTROLLER1(&this->state);
    DebugDisplay_Init();

    {
        GraphicsContext* gfxCtx = this->state.gfxCtx;

        if (1) {
            this->state.gfxCtx = NULL;
        }
        camera_pre_play_update(this);

        // @recomp_event recomp_on_play_update(PlayState* play): Play_Update is about to be called.
        recomp_on_play_update(this);

        Play_Update(this);

        // @recomp_event recomp_after_play_update(PlayState* play): Play_Update was called.
        recomp_after_play_update(this);

        camera_post_play_update(this);
        analog_cam_post_play_update(this);
        autosave_post_play_update(this);
        this->state.gfxCtx = gfxCtx;
    }

    {
        Input input = *CONTROLLER1(&this->state);

        if (1) {
            *CONTROLLER1(&this->state) = D_801F6C18;
        }
        Play_Draw(this);
        *CONTROLLER1(&this->state) = input;
    }

    CutsceneManager_Update();
    CutsceneManager_ClearWaiting();
}

// @recomp Patched to add load a hook for loading rooms.
RECOMP_PATCH s32 Room_HandleLoadCallbacks(PlayState* play, RoomContext* roomCtx) {
    if (roomCtx->status == 1) {
        if (osRecvMesg(&roomCtx->loadQueue, NULL, OS_MESG_NOBLOCK) == 0) {
            s32 samsungDiag = recomp_android_should_use_sync_boot_dma();
            void* roomSegment = roomCtx->activeRoomVram;

            roomCtx->status = 0;
            if (samsungDiag) {
                roomSegment = AndroidDiag_ToRdramAlias(roomSegment);
                recomp_measure_latency(95, 0x20, (u32)(uintptr_t)roomCtx->activeRoomVram,
                                       (u32)(uintptr_t)roomSegment, (u32)roomCtx->curRoom.num);
            }
            roomCtx->curRoom.segment = roomSegment;
            gSegments[3] = OS_K0_TO_PHYSICAL(roomSegment);

            // @recomp Call the room load hook.
            room_load_hook(play, &roomCtx->curRoom);

            Scene_ExecuteCommands(play, roomCtx->curRoom.segment);
            func_80123140(play, GET_PLAYER(play));
            Actor_SpawnTransitionActors(play, &play->actorCtx);

            if (((play->sceneId != SCENE_IKANA) || (roomCtx->curRoom.num != 1)) && (play->sceneId != SCENE_IKNINSIDE)) {
                play->envCtx.lightSettingOverride = LIGHT_SETTING_OVERRIDE_NONE;
                play->envCtx.lightBlendOverride = LIGHT_BLEND_OVERRIDE_NONE;
            }
            func_800FEAB0();
            if (Environment_GetStormState(play) == STORM_STATE_OFF) {
                Environment_StopStormNatureAmbience(play);
            }
        } else {
            return 0;
        }
    }

    return 1;
}

void ZeldaArena_Init(void* start, size_t size);
void Target_SetFairyState(TargetContext* targetCtx, Actor* actor, ActorType type, PlayState* play);
void Target_InitLockOn(TargetContext* targetCtx, ActorType type, PlayState* play);
void Horse_SpawnMinigame(PlayState* play, Player* player);
void Horse_SpawnOverworld(PlayState* play, Player* player);

void Play_SpawnScene(PlayState* this, s32 sceneId, s32 spawn);
void Play_InitScene(PlayState* this, s32 spawn);
void Play_InitEnvironment(PlayState* this, s16 skyboxId);
void Play_InitMotionBlur(void);
extern TransitionInit TransitionFade_InitVars;
extern TransitionOverlay gTransitionOverlayTable[];
s32 Scene_ExecuteCommands(PlayState* play, SceneCmd* sceneSegment);
void func_80123140(PlayState* play, Player* player);
void Player_AdjustSingleLeg(PlayState* play, Player* player, SkelAnime* skelAnime, Vec3f* pos, Vec3s* rot,
                            s32 thighLimbIndex, s32 shinLimbIndex, s32 footLimbIndex);

RECOMP_PATCH void func_80125500(PlayState* play, Player* player, s32 limbIndex, Vec3f* pos, Vec3s* rot) {
    if (limbIndex == PLAYER_LIMB_LEFT_THIGH) {
        Player_AdjustSingleLeg(play, player, &player->skelAnime, pos, rot, PLAYER_LIMB_LEFT_THIGH,
                               PLAYER_LIMB_LEFT_SHIN, PLAYER_LIMB_LEFT_FOOT);
    } else if (limbIndex == PLAYER_LIMB_RIGHT_THIGH) {
        Player_AdjustSingleLeg(play, player, &player->skelAnime, pos, rot, PLAYER_LIMB_RIGHT_THIGH,
                               PLAYER_LIMB_RIGHT_SHIN, PLAYER_LIMB_RIGHT_FOOT);
    }
}

void Actor_SpawnTransitionActors(PlayState* play, ActorContext* actorCtx);
void func_800FEAB0(void);
u32 Environment_GetStormState(PlayState* play);
void Environment_StopStormNatureAmbience(PlayState* play);
void LightContext_Init(PlayState* play, LightContext* lightCtx);
void Door_InitContext(GameState* gameState, DoorContext* doorCtx);
void Room_Init(PlayState* play, RoomContext* roomCtx);
s32 Object_SpawnPersistent(ObjectContext* objectCtx, s16 id);
s32 Room_StartRoomTransition(PlayState* play, RoomContext* roomCtx, s32 index);
s32 DmaMgr_FindDmaIndex(uintptr_t vrom);
void Fault_AddHungupAndCrash(const char* file, s32 line);
void Message_SetTables(PlayState* play);
void CmpDma_GetFileInfo(uintptr_t segmentRom, s32 id, uintptr_t* outFileRom, size_t* size, s32* flag);
void CmpDma_Decompress(uintptr_t romStart, size_t size, void* dst);
void func_80178AC0(u16* src, void* dst, size_t size);
s32 Map_IsInDungeonOrBossArea(PlayState* play);
s32 Map_GetDungeonOrBossAreaIndex(PlayState* play);
s32 func_8010A2AC(PlayState* play);
s32 func_8010A238(PlayState* play);
void func_80105C40(s16 room);
void Scene_CommandSpawnList(PlayState* play, SceneCmd* cmd);
void Scene_CommandActorList(PlayState* play, SceneCmd* cmd);
void Scene_CommandActorCutsceneCamList(PlayState* play, SceneCmd* cmd);
void Scene_CommandCollisionHeader(PlayState* play, SceneCmd* cmd);
void Scene_CommandRoomList(PlayState* play, SceneCmd* cmd);
void Scene_CommandEntranceList(PlayState* play, SceneCmd* cmd);
void Scene_CommandSpecialFiles(PlayState* play, SceneCmd* cmd);
void Scene_CommandRoomBehavior(PlayState* play, SceneCmd* cmd);
void Scene_CommandMesh(PlayState* play, SceneCmd* cmd);
void Scene_CommandObjectList(PlayState* play, SceneCmd* cmd);
void Scene_CommandLightList(PlayState* play, SceneCmd* cmd);
void Scene_CommandPathList(PlayState* play, SceneCmd* cmd);
void Scene_CommandTransiActorList(PlayState* play, SceneCmd* cmd);
void Scene_CommandEnvLightSettings(PlayState* play, SceneCmd* cmd);
void Scene_CommandSkyboxSettings(PlayState* play, SceneCmd* cmd);
void Scene_CommandSkyboxDisables(PlayState* play, SceneCmd* cmd);
void Scene_CommandTimeSettings(PlayState* play, SceneCmd* cmd);
void Scene_CommandWindSettings(PlayState* play, SceneCmd* cmd);
void Scene_CommandExitList(PlayState* play, SceneCmd* cmd);
void Scene_Command09(PlayState* play, SceneCmd* cmd);
void Scene_CommandSoundSettings(PlayState* play, SceneCmd* cmd);
void Scene_CommandEchoSetting(PlayState* play, SceneCmd* cmd);
void Scene_CommandAltHeaderList(PlayState* play, SceneCmd* cmd);
void Scene_CommandCutsceneScriptList(PlayState* play, SceneCmd* cmd);
void Scene_CommandCutsceneList(PlayState* play, SceneCmd* cmd);
void Scene_CommandMiniMap(PlayState* play, SceneCmd* cmd);
void Scene_Command1D(PlayState* play, SceneCmd* cmd);
void Scene_CommandMiniMapCompassInfo(PlayState* play, SceneCmd* cmd);
void Scene_CommandSetRegionVisitedFlag(PlayState* play, SceneCmd* cmd);
void Scene_CommandAnimatedMaterials(PlayState* play, SceneCmd* cmd);

extern s16 sTransitionFillTimer;
extern Input D_801F6C18;
extern TransitionTile sTransitionTile;
extern s32 gTransitionTileState;
extern VisMono sPlayVisMono;
extern Color_RGBA8_u32 gVisMonoColor;
extern VisFbuf sPlayVisFbuf;
extern VisFbuf* sPlayVisFbufInstance;
extern BombersNotebook sBombersNotebook;
extern u8 sBombersNotebookOpen;
extern u8 sMotionBlurStatus;

extern s32 gDbgCamEnabled;
extern u8 D_801D0D54;

extern u8 gPictoPhotoI8[];
extern u8 D_80784600[];
extern s16 D_801BFE14[PLAYER_BOOTS_MAX][18];

// Non-relocatable references to the original addresses of these game state functions.
void DayTelop_Init_NORELOCATE(GameState*);
void TitleSetup_Init_NORELOCATE(GameState*);

RECOMP_DECLARE_EVENT(recomp_on_play_init(PlayState* this));
RECOMP_DECLARE_EVENT(recomp_after_play_init(PlayState* this));

bool allow_no_ocarina_tf = false;

#define ANDROID_NES_FONT_STATIC_SIZE 0x4E00
static u8 sAndroidNesFontStaticCache[ANDROID_NES_FONT_STATIC_SIZE] __attribute__((aligned(16)));
static u8 sAndroidNesFontStaticCacheReady = false;
static u8 sAndroidCmpDmaTempBuf[0x1000] __attribute__((aligned(16)));
static SceneTableEntry sAndroidSonchonoieSceneEntry = {
    { 0x02008000, 0x0200F500 }, 0x10B, 0, SCENE_DRAW_CFG_DEFAULT, 0, 0,
};

static void AndroidDiag_CopyBytes(u8* dst, const u8* src, size_t size) {
    size_t i;

    for (i = 0; i < size; i++) {
        dst[i] = src[i];
    }
}

static void AndroidDiag_ProcessDma(void* dst, uintptr_t vromStart, size_t size, const char* label) {
    DmaEntry* entry = DmaMgr_FindDmaEntry(vromStart);
    s32 index = DmaMgr_FindDmaIndex(vromStart);
    s32 yaz0Status;

    recomp_measure_latency(97, 0, (u32)(uintptr_t)dst, (u32)vromStart, (u32)size);

    if (entry == NULL) {
        recomp_measure_latency(97, 30, (u32)index, (u32)vromStart, (u32)size);
        recomp_printf("[AndroidLoadDiag] %s no dma entry index=%d dst=%08llX vrom=%08llX size=%08X; "
                      "falling back to raw ROM load\n",
                      label, index, (u64)(uintptr_t)dst, (u64)vromStart, size);
        recomp_load_overlays(vromStart, dst, size);
        recomp_measure_latency(97, 31, (u32)vromStart, (u32)(uintptr_t)dst, (u32)size);
        return;
    }

    recomp_measure_latency(97, 2, (u32)index, (u32)entry->vromStart, (u32)entry->vromEnd);
    recomp_measure_latency(97, 3, (u32)index, (u32)entry->romStart, (u32)entry->romEnd);

    recomp_printf("[AndroidLoadDiag] %s entry index=%d dst=%08llX vrom=%08llX size=%08X entryVrom=%08llX-%08llX "
                  "entryRom=%08llX-%08llX\n",
                  label, index, (u64)(uintptr_t)dst, (u64)vromStart, size, (u64)entry->vromStart,
                  (u64)entry->vromEnd, (u64)entry->romStart, (u64)entry->romEnd);

    if (entry->romEnd == 0) {
        if (entry->vromEnd < (vromStart + size)) {
            recomp_measure_latency(97, 4, (u32)entry->vromEnd, (u32)(vromStart + size), (u32)size);
            recomp_printf("[AndroidLoadDiag] %s uncompressed range overflow end=%08llX requestedEnd=%08llX\n", label,
                          (u64)entry->vromEnd, (u64)(vromStart + size));
            recomp_crash("Android DMA uncompressed range overflow");
        }

        recomp_measure_latency(97, 5, (u32)((entry->romStart + vromStart) - entry->vromStart), (u32)(uintptr_t)dst,
                               (u32)size);
        recomp_load_overlays((entry->romStart + vromStart) - entry->vromStart, dst, size);
        recomp_measure_latency(97, 6, (u32)((entry->romStart + vromStart) - entry->vromStart), (u32)(uintptr_t)dst,
                               (u32)size);
        return;
    }

    if (vromStart != entry->vromStart) {
        recomp_measure_latency(97, 7, (u32)vromStart, (u32)entry->vromStart, (u32)size);
        recomp_printf("[AndroidLoadDiag] %s compressed partial load vrom=%08llX entryVrom=%08llX\n", label,
                      (u64)vromStart, (u64)entry->vromStart);
        recomp_crash("Android DMA compressed partial load");
    }

    if (size != (entry->vromEnd - entry->vromStart)) {
        recomp_measure_latency(97, 8, (u32)size, (u32)(entry->vromEnd - entry->vromStart), (u32)vromStart);
        recomp_printf("[AndroidLoadDiag] %s compressed size mismatch size=%08X expected=%08llX\n", label, size,
                      (u64)(entry->vromEnd - entry->vromStart));
        recomp_crash("Android DMA compressed size mismatch");
    }

    recomp_measure_latency(97, 9, (u32)entry->romStart, (u32)(uintptr_t)dst, (u32)(entry->romEnd - entry->romStart));
    yaz0Status = recomp_android_load_yaz0(entry->romStart, entry->romEnd - entry->romStart, dst, size);
    recomp_measure_latency(97, 20, (u32)yaz0Status, (u32)entry->romStart, (u32)(uintptr_t)dst);

    if (yaz0Status != 0) {
        recomp_measure_latency(97, 21, (u32)yaz0Status, (u32)entry->romStart,
                               (u32)(entry->romEnd - entry->romStart));
        recomp_printf("[AndroidLoadDiag] %s host Yaz0 failed status=%d rom=%08llX compressed=%08llX\n", label,
                      yaz0Status, (u64)entry->romStart, (u64)(entry->romEnd - entry->romStart));
        recomp_crash("Android host Yaz0 failed");
    }
    recomp_measure_latency(97, 10, (u32)entry->romStart, (u32)(uintptr_t)dst, (u32)size);
}

static bool AndroidDiag_GetObjectFile(s16 id, RomFile* objectFile) {
    if (recomp_android_should_use_sync_boot_dma()) {
        switch (id) {
            case 1: // GAMEPLAY_KEEP
                objectFile->vromStart = 0x0108B000;
                objectFile->vromEnd = 0x0111D5E0;
                return true;
            case 2: // GAMEPLAY_FIELD_KEEP
                objectFile->vromStart = 0x0111E000;
                objectFile->vromEnd = 0x01127290;
                return true;
            case 3: // GAMEPLAY_DANGEON_KEEP
                objectFile->vromStart = 0x01128000;
                objectFile->vromEnd = 0x0114B280;
                return true;
            case 0x10: // OBJECT_LINK_BOY
                objectFile->vromStart = 0x0114D000;
                objectFile->vromEnd = 0x0115A890;
                return true;
            case 0x11: // OBJECT_LINK_CHILD
                objectFile->vromStart = 0x0115B000;
                objectFile->vromEnd = 0x01179250;
                return true;
            case 0x160: // OBJECT_LINK_GORON
                objectFile->vromStart = 0x0117A000;
                objectFile->vromEnd = 0x01191A90;
                return true;
            case 0x161: // OBJECT_LINK_ZORA
                objectFile->vromStart = 0x01192000;
                objectFile->vromEnd = 0x011A4C40;
                return true;
            case 0x162: // OBJECT_LINK_NUTS
                objectFile->vromStart = 0x011A5000;
                objectFile->vromEnd = 0x011B0A30;
                return true;
            case 0x192: // OBJECT_STK
                objectFile->vromStart = 0x01802000;
                objectFile->vromEnd = 0x0181F3E0;
                return true;
            case 0x1BE: // OBJECT_STK2
                objectFile->vromStart = 0x0199B000;
                objectFile->vromEnd = 0x019D5910;
                return true;
            case 0x277: // OBJECT_STK3
                objectFile->vromStart = 0x01E3D000;
                objectFile->vromEnd = 0x01E42F60;
                return true;
        }

        recomp_measure_latency(98, 2, (u32)(s32)id, (u32)(uintptr_t)gObjectTable,
                               0);
        recomp_printf("[AndroidLoadDiag] object id %d not in Android boot table; falling back to base gObjectTable\n",
                      id);
    }

    *objectFile = gObjectTable[id];
    return false;
}

static void AndroidDiag_LoadRomToRam(void* dst, uintptr_t vromStart, size_t size, const char* label) {
    if (size == 0) {
        return;
    }

    if (dst == NULL) {
        recomp_printf("[AndroidLoadDiag] %s skipped null dst vrom=%08llX size=%08X\n", label, (u64)vromStart, size);
        return;
    }

    if (recomp_android_should_use_sync_boot_dma()) {
        recomp_printf("[AndroidLoadDiag] %s sync dst=%08llX vrom=%08llX size=%08X\n",
                      label, (u64)(uintptr_t)dst, (u64)vromStart, size);
        AndroidDiag_ProcessDma(dst, vromStart, size, label);
    } else {
        DmaMgr_SendRequest0(dst, vromStart, size);
    }
}

static s32 CompatPmm_IsHumanLinkObject(s16 id) {
    return (id == OBJECT_LINK_CHILD) || (id == OBJECT_LINK_BOY);
}

static void CompatPmm_TryLoadSelectedObject(s16 id, void* dst, size_t maxSize, const char* label) {
    s32 status;

    if (!CompatPmm_IsHumanLinkObject(id) || dst == NULL || maxSize == 0) {
        return;
    }

    status = recomp_android_compat_pmm_load_selected_model(dst, (u32)maxSize);
    if (status > 0) {
        recomp_printf("[CompatPMM] loaded %s model bytes=%d max=%08X\n", label, status, (u32)maxSize);
    } else if (status != -1 && status != -2) {
        recomp_printf("[CompatPMM] skipped %s selected model status=%d max=%08X\n", label, status, (u32)maxSize);
    }
}

static void CompatPmm_ReloadLoadedHumanObjects(PlayState* play) {
    s32 i;
    RomFile objectFile;
    ObjectContext* objectCtx = &play->objectCtx;

    for (i = 0; i < objectCtx->numEntries; i++) {
        s16 id = objectCtx->slots[i].id;
        if (id < 0) {
            id = -id;
        }

        if (!CompatPmm_IsHumanLinkObject(id)) {
            continue;
        }

        AndroidDiag_GetObjectFile(id, &objectFile);
        CompatPmm_TryLoadSelectedObject(id, objectCtx->slots[i].segment, objectFile.vromEnd - objectFile.vromStart,
                                        "live");
    }
}

static void compat_pmm_play_update(PlayState* play) {
#if defined(ZELDA_ANDROID_BUILTIN_PMM)
    updatePlayerProxyManager_on_UpdateMain();
    updateAdultProperties_on_Play_UpdateMain(play);
    processPlayerProxyRefreshRequests_on_Play_UpdateMain();
    processFormProxyRefreshRequests_on_Play_UpdateMain();
#else
    Input* input = CONTROLLER1(&play->state);

    if ((recomp_android_compat_pmm_model_count() <= 0) ||
        !CHECK_BTN_ALL(input->cur.button, BTN_L) ||
        !CHECK_BTN_ALL(input->press.button, BTN_A)) {
        return;
    }

    if (recomp_android_compat_pmm_cycle_model(1) >= 0) {
        input->press.button &= (u16)~BTN_A;
        CompatPmm_ReloadLoadedHumanObjects(play);
    }
#endif
}

RECOMP_PATCH void CmpDma_LoadFileImpl(uintptr_t segmentRom, s32 id, void* dst, size_t size) {
    uintptr_t romStart;
    size_t compressedSize;
    s32 flag;
    s32 status;

    CmpDma_GetFileInfo(segmentRom, id, &romStart, &compressedSize, &flag);

    if (recomp_android_should_use_sync_boot_dma() && (size != 0)) {
        recomp_measure_latency(97, 0x60, (u32)segmentRom, (u32)id, (u32)size);
        if (flag & 1) {
            status = recomp_android_load_yaz0(romStart, compressedSize, sAndroidCmpDmaTempBuf, sizeof(sAndroidCmpDmaTempBuf));
            recomp_measure_latency(97, 0x61, (u32)status, (u32)romStart, (u32)compressedSize);
            if (status != 0) {
                recomp_printf("[AndroidLoadDiag] cmpdma rgba decode failed id=%d status=%d rom=%08llX compressed=%08X\n",
                              id, status, (u64)romStart, compressedSize);
                recomp_crash("Android CmpDma rgba decode failed");
            }
            func_80178AC0((u16*)sAndroidCmpDmaTempBuf, dst, size);
        } else {
            status = recomp_android_load_yaz0(romStart, compressedSize, dst, size);
            recomp_measure_latency(97, 0x62, (u32)status, (u32)romStart, (u32)compressedSize);
            if (status != 0) {
                recomp_printf("[AndroidLoadDiag] cmpdma decode failed id=%d status=%d rom=%08llX compressed=%08X "
                              "expected=%08X\n",
                              id, status, (u64)romStart, compressedSize, size);
                recomp_crash("Android CmpDma decode failed");
            }
        }
        return;
    }

    if (flag & 1) {
        CmpDma_Decompress(romStart, compressedSize, sAndroidCmpDmaTempBuf);
        func_80178AC0((u16*)sAndroidCmpDmaTempBuf, dst, size);
    } else {
        CmpDma_Decompress(romStart, compressedSize, dst);
    }
}

RECOMP_PATCH void Map_InitRoomData(PlayState* play, s16 room) {
    s32 mapIndex = gSaveContext.mapIndex;
    InterfaceContext* interfaceCtx = &play->interfaceCtx;

    if (recomp_android_should_use_sync_boot_dma()) {
        recomp_measure_latency(96, 0x40, (u32)play->sceneId, (u32)room, (u32)mapIndex);
    } else {
        func_80105C40(room);
    }

    if (room >= 0) {
        if (Map_IsInDungeonOrBossArea(play)) {
            gSaveContext.save.saveInfo.permanentSceneFlags[Play_GetOriginalSceneId(play->sceneId)].rooms |=
                gBitFlags[room];
            interfaceCtx->mapRoomNum = room;
            interfaceCtx->dungeonOrBossAreaMapIndex = mapIndex;
        }
    } else {
        interfaceCtx->mapRoomNum = 0;
    }

    if (gSaveContext.sunsSongState != SUNSSONG_SPEED_TIME) {
        gSaveContext.sunsSongState = SUNSSONG_INACTIVE;
    }
}

RECOMP_PATCH void Map_Init(PlayState* play) {
    InterfaceContext* interfaceCtx = &play->interfaceCtx;
    s32 dungeonIndex;

    if (recomp_android_should_use_sync_boot_dma()) {
        recomp_measure_latency(96, 0x41, (u32)play->sceneId, (u32)play->roomCtx.curRoom.num, 0);
    } else {
        func_80105C40(play->roomCtx.curRoom.num);
    }

    interfaceCtx->unk_278 = -1;
    interfaceCtx->dungeonOrBossAreaMapIndex = -1;
    interfaceCtx->mapSegment = THA_AllocTailAlign16(&play->state.tha, 0x1000);
    if (func_8010A2AC(play)) {
        gSaveContext.mapIndex = func_8010A238(play);
        return;
    }

    if (Map_IsInDungeonOrBossArea(play)) {
        dungeonIndex = Map_GetDungeonOrBossAreaIndex(play);
        gSaveContext.mapIndex = dungeonIndex;
        switch (play->sceneId) {
            case SCENE_MITURIN_BS:
                dungeonIndex = DUNGEON_INDEX_WOODFALL_TEMPLE;
                break;

            case SCENE_HAKUGIN_BS:
                dungeonIndex = DUNGEON_INDEX_SNOWHEAD_TEMPLE;
                break;

            case SCENE_SEA_BS:
                dungeonIndex = DUNGEON_INDEX_GREAT_BAY_TEMPLE;
                break;

            case SCENE_INISIE_BS:
                dungeonIndex = DUNGEON_INDEX_STONE_TOWER_TEMPLE;
                break;
        }

        gSaveContext.dungeonIndex = dungeonIndex;
        Map_InitRoomData(play, play->roomCtx.curRoom.num);
    }
}

static void* AndroidDiag_ToRdramAlias(void* ptr) {
    if (recomp_android_should_use_sync_boot_dma() && (((uintptr_t)ptr & 0x80000000U) == 0)) {
        return (void*)((uintptr_t)ptr | 0x80000000U);
    }

    return ptr;
}

static bool AndroidDiag_IsKseg0Ptr(void* ptr) {
    return ptr != NULL && (((uintptr_t)ptr & 0x80000000U) != 0);
}

static s16 AndroidDiag_GetPlayerObjectId(s32 playerForm) {
    if (!recomp_android_should_use_sync_boot_dma()) {
        return gPlayerFormObjectIds[playerForm];
    }

    switch (playerForm) {
        case PLAYER_FORM_FIERCE_DEITY:
            return OBJECT_LINK_BOY;
        case PLAYER_FORM_GORON:
            return OBJECT_LINK_GORON;
        case PLAYER_FORM_ZORA:
            return OBJECT_LINK_ZORA;
        case PLAYER_FORM_DEKU:
            return OBJECT_LINK_NUTS;
        case PLAYER_FORM_HUMAN:
        default:
            return OBJECT_LINK_CHILD;
    }
}

RECOMP_PATCH void Play_SetupTransition(PlayState* this, s32 transitionType) {
    TransitionContext* transitionCtx = &this->transitionCtx;
    s32 fbdemoType;
    s32 samsungDiag = recomp_android_should_use_sync_boot_dma();

    bzero(transitionCtx, sizeof(TransitionContext));

    fbdemoType = -1;
    if (transitionType & TRANS_TYPE_WIPE3) {
        fbdemoType = FBDEMO_WIPE3;
    } else if ((transitionType & 0x78) == TRANS_TYPE_WIPE4) {
        fbdemoType = FBDEMO_WIPE4;
    } else if (!(transitionType & (TRANS_TYPE_WIPE3 | TRANS_TYPE_WIPE4))) {
        switch (transitionType) {
            case TRANS_TYPE_TRIFORCE:
                fbdemoType = FBDEMO_TRIFORCE;
                break;

            case TRANS_TYPE_WIPE:
            case TRANS_TYPE_WIPE_FAST:
                fbdemoType = FBDEMO_WIPE1;
                break;

            case TRANS_TYPE_FADE_BLACK:
            case TRANS_TYPE_FADE_WHITE:
            case TRANS_TYPE_FADE_BLACK_FAST:
            case TRANS_TYPE_FADE_WHITE_FAST:
            case TRANS_TYPE_FADE_BLACK_SLOW:
            case TRANS_TYPE_FADE_WHITE_SLOW:
            case TRANS_TYPE_FADE_WHITE_CS_DELAYED:
            case TRANS_TYPE_FADE_WHITE_INSTANT:
            case TRANS_TYPE_FADE_GREEN:
            case TRANS_TYPE_FADE_BLUE:
                fbdemoType = FBDEMO_FADE;
                break;

            case TRANS_TYPE_FILL_WHITE_FAST:
            case TRANS_TYPE_FILL_WHITE:
                this->transitionMode = TRANS_MODE_FILL_WHITE_INIT;
                break;

            case TRANS_TYPE_INSTANT:
                this->transitionMode = TRANS_MODE_INSTANT;
                break;

            case TRANS_TYPE_FILL_BROWN:
                this->transitionMode = TRANS_MODE_FILL_BROWN_INIT;
                break;

            case TRANS_TYPE_SANDSTORM_PERSIST:
                this->transitionMode = TRANS_MODE_SANDSTORM_INIT;
                break;

            case TRANS_TYPE_SANDSTORM_END:
                this->transitionMode = TRANS_MODE_SANDSTORM_END_INIT;
                break;

            case TRANS_TYPE_CS_BLACK_FILL:
                this->transitionMode = TRANS_MODE_CS_BLACK_FILL_INIT;
                break;

            case TRANS_TYPE_CIRCLE:
                fbdemoType = FBDEMO_CIRCLE;
                break;

            case TRANS_TYPE_WIPE5:
                fbdemoType = FBDEMO_WIPE5;
                break;

            default:
                fbdemoType = -1;
                _dbg_hungup("../z_play.c", 1420);
        }
    } else {
        fbdemoType = -1;
        _dbg_hungup("../z_play.c", 1423);
    }

    if (samsungDiag && (fbdemoType != -1) && (fbdemoType != FBDEMO_FADE)) {
        recomp_measure_latency(98, 0xA0, transitionType, fbdemoType, TRANS_TYPE_FADE_BLACK);
        transitionType = TRANS_TYPE_FADE_BLACK;
        fbdemoType = FBDEMO_FADE;
    }

    transitionCtx->transitionType = transitionType;
    transitionCtx->fbdemoType = fbdemoType;
    if (fbdemoType != -1) {
        Transition_Init(transitionCtx);
    }
}

RECOMP_PATCH void Transition_Init(TransitionContext* transitionCtx) {
    TransitionOverlay* overlayEntry;
    ptrdiff_t relocOffset;
    TransitionInit* initInfo[1];
    s32 samsungDiag = recomp_android_should_use_sync_boot_dma();
    if (samsungDiag) {
        recomp_measure_latency(98, 0xA1, (u32)transitionCtx, transitionCtx->transitionType, transitionCtx->fbdemoType);
    }

    overlayEntry = &gTransitionOverlayTable[transitionCtx->fbdemoType];
    initInfo[0] = NULL;

    if (samsungDiag && (overlayEntry->vromStart == 0)) {
        initInfo[0] = overlayEntry->initInfo;
        recomp_measure_latency(98, 0xA3, (u32)transitionCtx, transitionCtx->transitionType, transitionCtx->fbdemoType);
    } else {
        TransitionOverlay_Load(overlayEntry);

        relocOffset = (uintptr_t)Lib_PhysicalToVirtual(overlayEntry->loadInfo.addr) - (uintptr_t)overlayEntry->vramStart;
        initInfo[0] = (overlayEntry->initInfo != NULL) ? (TransitionInit*)((uintptr_t)overlayEntry->initInfo + relocOffset)
                                                       : initInfo[0];
    }

    if (initInfo[0] == NULL) {
        recomp_measure_latency(98, 0xA4, (u32)transitionCtx, transitionCtx->transitionType, transitionCtx->fbdemoType);
        return;
    }

    transitionCtx->init = initInfo[0]->init;
    transitionCtx->destroy = initInfo[0]->destroy;
    transitionCtx->start = initInfo[0]->start;
    transitionCtx->isDone = initInfo[0]->isDone;
    transitionCtx->draw = initInfo[0]->draw;
    transitionCtx->update = initInfo[0]->update;
    transitionCtx->setType = initInfo[0]->setType;
    transitionCtx->setColor = initInfo[0]->setColor;
    transitionCtx->setEnvColor = initInfo[0]->setEnvColor;
}

RECOMP_PATCH void Transition_Destroy(TransitionContext* transitionCtx) {
    if (recomp_android_should_use_sync_boot_dma() && (transitionCtx->fbdemoType == FBDEMO_FADE)) {
        recomp_measure_latency(98, 0xA2, (u32)transitionCtx, transitionCtx->transitionType, transitionCtx->fbdemoType);
        return;
    }

    TransitionOverlay_Free(&gTransitionOverlayTable[transitionCtx->fbdemoType]);
}

RECOMP_PATCH void* THA_AllocTailAlign16(TwoHeadArena* tha, size_t size) {
    uintptr_t mask = ~((uintptr_t)0x10 - 1);

    tha->tail = (void*)((((uintptr_t)tha->tail & mask) - size) & mask);
    return tha->tail;
}

RECOMP_PATCH void* THA_AllocTailAlign(TwoHeadArena* tha, size_t size, uintptr_t mask) {
    if (recomp_android_should_use_sync_boot_dma() && ((mask & ~(uintptr_t)0xFFFFFFFFU) == 0)) {
        uintptr_t align = (~mask) + 1;

        if (align != 0) {
            mask = ~(align - 1);
        }
    }

    tha->tail = (void*)((((uintptr_t)tha->tail & mask) - size) & mask);
    return tha->tail;
}

RECOMP_PATCH void* THA_AllocTail(TwoHeadArena* tha, size_t size) {
    uintptr_t mask;

    if (size >= 0x10) {
        mask = ~((uintptr_t)0x10 - 1);
    } else if (size & 1) {
        mask = ~((uintptr_t)1 - 1);
    } else if (size & 2) {
        mask = ~((uintptr_t)2 - 1);
    } else if (size & 4) {
        mask = ~((uintptr_t)4 - 1);
    } else if (size & 8) {
        mask = ~((uintptr_t)8 - 1);
    } else {
        mask = ~((uintptr_t)1 - 1);
    }

    tha->tail = (void*)((((uintptr_t)tha->tail & mask) - size) & mask);
    return tha->tail;
}

RECOMP_PATCH void Target_Init(TargetContext* targetCtx, Actor* actor, PlayState* play) {
    if (recomp_android_should_use_sync_boot_dma()) {
        targetCtx->bgmEnemy = NULL;
        targetCtx->forcedTargetActor = NULL;
        targetCtx->lockOnActor = NULL;
        targetCtx->fairyActor = NULL;
        targetCtx->arrowPointedActor = NULL;
        targetCtx->rotZTick = 0;
        targetCtx->lockOnIndex = 0;
        targetCtx->fairyMoveProgressFactor = 0.0f;
        targetCtx->fairyPos = play->view.eye;
        targetCtx->lockOnPos = play->view.eye;
        targetCtx->fairyInnerColor.r = 255.0f;
        targetCtx->fairyInnerColor.g = 255.0f;
        targetCtx->fairyInnerColor.b = 230.0f;
        targetCtx->fairyInnerColor.a = 255.0f;
        targetCtx->fairyOuterColor.r = 220.0f;
        targetCtx->fairyOuterColor.g = 160.0f;
        targetCtx->fairyOuterColor.b = 80.0f;
        targetCtx->fairyOuterColor.a = 0.0f;
        targetCtx->fairyActorCategory = ACTORCAT_PLAYER;
        targetCtx->lockOnRadius = 500.0f;
        targetCtx->lockOnAlpha = 256;

        for (s32 i = 0; i < ARRAY_COUNT(targetCtx->lockOnTriangleSets); i++) {
            targetCtx->lockOnTriangleSets[i].pos.x = play->view.eye.x;
            targetCtx->lockOnTriangleSets[i].pos.y = play->view.eye.y;
            targetCtx->lockOnTriangleSets[i].pos.z = play->view.eye.z;
            targetCtx->lockOnTriangleSets[i].radius = targetCtx->lockOnRadius;
            targetCtx->lockOnTriangleSets[i].color.r = 255;
            targetCtx->lockOnTriangleSets[i].color.g = 255;
            targetCtx->lockOnTriangleSets[i].color.b = 230;
            targetCtx->lockOnTriangleSets[i].color.a = 255;
        }

        recomp_measure_latency(94, 0x20, (u32)(uintptr_t)actor, (u32)(uintptr_t)&play->actorCtx.targetCtx,
                               (u32)(uintptr_t)play);
        return;
    }

    targetCtx->bgmEnemy = NULL;
    targetCtx->forcedTargetActor = NULL;
    targetCtx->lockOnActor = NULL;
    targetCtx->fairyActor = NULL;
    targetCtx->rotZTick = 0;
    targetCtx->lockOnIndex = 0;
    targetCtx->fairyMoveProgressFactor = 0.0f;
    Target_SetFairyState(targetCtx, actor, actor->category, play);
    Target_InitLockOn(targetCtx, actor->category, play);
}

RECOMP_PATCH void Horse_Spawn(PlayState* play, Player* player) {
    if (recomp_android_should_use_sync_boot_dma()) {
        recomp_measure_latency(94, 0x30, (u32)(uintptr_t)play, (u32)(uintptr_t)player, (u32)play->sceneId);
        D_801BDAA0 = false;
        return;
    }

    if (((play->sceneId == SCENE_KOEPONARACE) &&
         (GET_WEEKEVENTREG_HORSE_RACE_STATE == WEEKEVENTREG_HORSE_RACE_STATE_START)) ||
        ((play->sceneId == SCENE_F01) && (((gSaveContext.sceneLayer == 1)) || (gSaveContext.sceneLayer == 5)) &&
         (player->transformation == PLAYER_FORM_HUMAN)) ||
        ((play->sceneId == SCENE_KOEPONARACE) &&
         (((GET_WEEKEVENTREG_HORSE_RACE_STATE == WEEKEVENTREG_HORSE_RACE_STATE_3)) ||
          (GET_WEEKEVENTREG_HORSE_RACE_STATE == WEEKEVENTREG_HORSE_RACE_STATE_2)))) {
        Horse_SpawnMinigame(play, player);
    } else {
        Horse_SpawnOverworld(play, player);
    }

    D_801BDAA0 = false;
}

RECOMP_PATCH void func_80123140(PlayState* play, Player* player) {
    s16* bootRegs;
    PlayerBoots currentBoots;
    f32 scale;
#if defined(ZELDA_ANDROID_BUILTIN_PMM)
    bool useAdultPmmScale = false;
#endif

    if (recomp_android_should_use_sync_boot_dma()) {
        player = AndroidDiag_ToRdramAlias(player);
        if (player == NULL) {
            recomp_measure_latency(95, 0x30, (u32)(uintptr_t)play, 0, 0);
            return;
        }
        recomp_measure_latency(95, 0x31, (u32)(uintptr_t)player, (u32)(s32)player->currentBoots,
                               (u32)(s32)player->transformation);
    }

#if defined(ZELDA_ANDROID_BUILTIN_PMM)
    useAdultPmmScale = false;
#endif

    if ((player->actor.id == ACTOR_PLAYER) &&
        ((player->transformation == PLAYER_FORM_FIERCE_DEITY)
#if defined(ZELDA_ANDROID_BUILTIN_PMM)
         || useAdultPmmScale
#endif
        )) {
        REG(27) = 1200;
    } else {
        REG(27) = 2000;
    }

    REG(48) = 370;

    currentBoots = player->currentBoots;
    if (recomp_android_should_use_sync_boot_dma() &&
        ((currentBoots < PLAYER_BOOTS_FIERCE_DEITY) || (currentBoots >= PLAYER_BOOTS_MAX))) {
        recomp_measure_latency(95, 0x32, (u32)(s32)currentBoots, (u32)(s32)player->transformation,
                               (u32)(uintptr_t)player);
        currentBoots = PLAYER_BOOTS_HYLIAN;
        player->currentBoots = PLAYER_BOOTS_HYLIAN;
    }

    if (currentBoots >= PLAYER_BOOTS_ZORA_UNDERWATER) {
        if (player->stateFlags1 & PLAYER_STATE1_8000000) {
            currentBoots++;
        }
        if (recomp_android_should_use_sync_boot_dma() && (currentBoots >= PLAYER_BOOTS_MAX)) {
            currentBoots = PLAYER_BOOTS_ZORA_UNDERWATER;
        }
        if (player->transformation == PLAYER_FORM_GORON) {
            REG(48) = 200;
        }
    } else if (currentBoots == PLAYER_BOOTS_GIANT) {
        REG(48) = 170;
    }

    bootRegs = D_801BFE14[currentBoots];
    REG(19) = bootRegs[0];
    REG(30) = bootRegs[1];
    REG(32) = bootRegs[2];
    REG(34) = bootRegs[3];
    REG(35) = bootRegs[4];
    REG(36) = bootRegs[5];
    REG(37) = bootRegs[6];
    REG(38) = bootRegs[7];
    REG(39) = bootRegs[8];
    REG(43) = bootRegs[9];
    R_RUN_SPEED_LIMIT = bootRegs[10];
    REG(68) = bootRegs[11];
    REG(69) = bootRegs[12];
    IREG(66) = bootRegs[13];
    IREG(67) = bootRegs[14];
    IREG(68) = bootRegs[15];
    IREG(69) = bootRegs[16];
    MREG(95) = bootRegs[17];

    if (play->roomCtx.curRoom.behaviorType1 == ROOM_BEHAVIOR_TYPE1_2) {
        R_RUN_SPEED_LIMIT = 500;
    }

    if ((player->actor.id == ACTOR_PLAYER) &&
        ((player->transformation == PLAYER_FORM_FIERCE_DEITY)
#if defined(ZELDA_ANDROID_BUILTIN_PMM)
         || useAdultPmmScale
#endif
        )) {
        scale = 0.015f;
    } else {
        scale = 0.01f;
    }

    Actor_SetScale(&player->actor, scale);

}

RECOMP_PATCH void SSNodeList_Alloc(PlayState* play, SSNodeList* this, s32 tblMax, s32 numPolys) {
    this->max = tblMax;
    this->count = 0;
    this->tbl = AndroidDiag_ToRdramAlias(THA_AllocTailAlign(&play->state.tha, tblMax * sizeof(SSNode), -2));
    this->polyCheckTbl = AndroidDiag_ToRdramAlias(THA_AllocTailAlign16(&play->state.tha, numPolys * sizeof(u8)));

    if (this->polyCheckTbl == NULL) {
        recomp_crash("Static collision node allocation failed");
    }
}

RECOMP_PATCH void DynaSSNodeList_Alloc(PlayState* play, DynaSSNodeList* list, u32 numNodes) {
    list->tbl = AndroidDiag_ToRdramAlias(THA_AllocTailAlign(&play->state.tha, numNodes * sizeof(SSNode), -2));
    list->maxNodes = numNodes;
    list->count = 0;
}

RECOMP_PATCH void DynaPoly_AllocPolyList(PlayState* play, CollisionPoly** polyList, s32 numPolys) {
    *polyList = AndroidDiag_ToRdramAlias(THA_AllocTailAlign(&play->state.tha, numPolys * sizeof(CollisionPoly), -2));
}

RECOMP_PATCH void DynaPoly_AllocVtxList(PlayState* play, Vec3s** vtxList, s32 numVtx) {
    *vtxList = AndroidDiag_ToRdramAlias(THA_AllocTailAlign(&play->state.tha, numVtx * sizeof(Vec3s), -2));
}

RECOMP_PATCH void DynaPoly_AllocWaterBoxList(PlayState* play, DynaWaterBoxList* waterBoxList, s32 numWaterBoxes) {
    waterBoxList->boxes = AndroidDiag_ToRdramAlias(THA_AllocTailAlign(&play->state.tha, numWaterBoxes * sizeof(WaterBox), -2));
}

RECOMP_PATCH void BgCheck_Allocate(CollisionContext* colCtx, PlayState* play, CollisionHeader* colHeader) {
    u32 tblMax;
    u32 memSize;
    u32 lookupTblMemSize;
    s32 customNodeListMax;

    customNodeListMax = -1;
    colCtx->colHeader = colHeader;
    colCtx->flags = 0;

    if (BgCheck_IsSmallMemScene(play)) {
        colCtx->memSize = 0xF000;
        colCtx->dyna.polyNodesMax = 1000;
        colCtx->dyna.polyListMax = 512;
        colCtx->dyna.vtxListMax = 512;
        colCtx->subdivAmount.x = 16;
        colCtx->subdivAmount.y = 4;
        colCtx->subdivAmount.z = 16;
    } else {
        u32 customMemSize;
        s32 useCustomSubdivisions;
        s32 i;

        if (BgCheck_TryGetCustomMemsize(play->sceneId, &customMemSize)) {
            colCtx->memSize = customMemSize;
        } else {
            colCtx->memSize = 0x23000;
        }
        colCtx->dyna.polyNodesMax = 1000;
        colCtx->dyna.polyListMax = 544;
        colCtx->dyna.vtxListMax = 512;
        BgCheck_GetSpecialSceneMaxObjects(play, &colCtx->dyna.polyNodesMax, &colCtx->dyna.polyListMax,
                                          &colCtx->dyna.vtxListMax);
        useCustomSubdivisions = false;

        for (i = 0; i < 3; i++) {
            if (play->sceneId == sSceneSubdivisionList[i].sceneId) {
                colCtx->subdivAmount.x = sSceneSubdivisionList[i].subdivAmount.x;
                colCtx->subdivAmount.y = sSceneSubdivisionList[i].subdivAmount.y;
                colCtx->subdivAmount.z = sSceneSubdivisionList[i].subdivAmount.z;
                useCustomSubdivisions = true;
                customNodeListMax = sSceneSubdivisionList[i].nodeListMax;
            }
        }
        if (useCustomSubdivisions == false) {
            colCtx->subdivAmount.x = 16;
            colCtx->subdivAmount.y = 4;
            colCtx->subdivAmount.z = 16;
        }
    }

    colCtx->lookupTbl = AndroidDiag_ToRdramAlias(THA_AllocTailAlign(
        &play->state.tha,
        colCtx->subdivAmount.x * sizeof(StaticLookup) * colCtx->subdivAmount.y * colCtx->subdivAmount.z, ~1));
    if (colCtx->lookupTbl == NULL) {
        recomp_crash("Static collision lookup allocation failed");
    }

    colCtx->minBounds.x = colCtx->colHeader->minBounds.x;
    colCtx->minBounds.y = colCtx->colHeader->minBounds.y;
    colCtx->minBounds.z = colCtx->colHeader->minBounds.z;
    colCtx->maxBounds.x = colCtx->colHeader->maxBounds.x;
    colCtx->maxBounds.y = colCtx->colHeader->maxBounds.y;
    colCtx->maxBounds.z = colCtx->colHeader->maxBounds.z;
    BgCheck_SetSubdivisionDimension(colCtx->minBounds.x, colCtx->subdivAmount.x, &colCtx->maxBounds.x,
                                    &colCtx->subdivLength.x, &colCtx->subdivLengthInv.x);
    BgCheck_SetSubdivisionDimension(colCtx->minBounds.y, colCtx->subdivAmount.y, &colCtx->maxBounds.y,
                                    &colCtx->subdivLength.y, &colCtx->subdivLengthInv.y);
    BgCheck_SetSubdivisionDimension(colCtx->minBounds.z, colCtx->subdivAmount.z, &colCtx->maxBounds.z,
                                    &colCtx->subdivLength.z, &colCtx->subdivLengthInv.z);

    memSize = colCtx->subdivAmount.x * sizeof(StaticLookup) * colCtx->subdivAmount.y * colCtx->subdivAmount.z +
              colCtx->colHeader->numPolygons * sizeof(u8) + colCtx->dyna.polyNodesMax * sizeof(SSNode) +
              colCtx->dyna.polyListMax * sizeof(CollisionPoly) + colCtx->dyna.vtxListMax * sizeof(Vec3s) +
              sizeof(CollisionContext);
    if (customNodeListMax > 0) {
        tblMax = customNodeListMax;
    } else {
        if (colCtx->memSize < memSize) {
            recomp_crash("Static collision memory is too small");
        }
        tblMax = (colCtx->memSize - memSize) / sizeof(SSNode);
    }

    SSNodeList_Init(&colCtx->polyNodes);
    SSNodeList_Alloc(play, &colCtx->polyNodes, tblMax, colCtx->colHeader->numPolygons);

    lookupTblMemSize = BgCheck_InitStaticLookup(colCtx, play, colCtx->lookupTbl);
    (void)lookupTblMemSize;

    DynaPoly_Init(play, &colCtx->dyna);
    DynaPoly_Alloc(play, &colCtx->dyna);
}

RECOMP_PATCH void* Play_LoadFile(PlayState* this, RomFile* entry) {
    size_t size = entry->vromEnd - entry->vromStart;
    void* allocp = THA_AllocTailAlign16(&this->state.tha, size);

    if (recomp_android_should_use_sync_boot_dma()) {
        recomp_measure_latency(97, 0x80, (u32)entry->vromStart, (u32)entry->vromEnd, (u32)size);
        recomp_measure_latency(97, 0x81, (u32)(uintptr_t)allocp, (u32)&this->state.tha, (u32)this->sceneId);
    }

    if (recomp_android_should_use_sync_boot_dma()) {
        AndroidDiag_LoadRomToRam(allocp, entry->vromStart, size, "play file");
    } else {
        DmaMgr_SendRequest0(allocp, entry->vromStart, size);
    }

    return allocp;
}

RECOMP_PATCH void Play_SpawnScene(PlayState* this, s32 sceneId, s32 spawn) {
    SceneTableEntry* scene = &gSceneTable[sceneId];

    if (recomp_android_should_use_sync_boot_dma() && (sceneId == SCENE_SONCHONOIE)) {
        sAndroidSonchonoieSceneEntry.segment.vromStart = 0x02008000;
        sAndroidSonchonoieSceneEntry.segment.vromEnd = 0x0200F500;
        sAndroidSonchonoieSceneEntry.titleTextId = 0x10B;
        sAndroidSonchonoieSceneEntry.drawConfig = SCENE_DRAW_CFG_DEFAULT;
        sAndroidSonchonoieSceneEntry.unk_D = 0;
        scene = &sAndroidSonchonoieSceneEntry;
    }

    scene->unk_D = 0;
    this->loadedScene = scene;
    this->sceneId = sceneId;
    this->sceneConfig = scene->drawConfig;
    this->sceneSegment = Play_LoadFile(this, &scene->segment);
    this->sceneSegment = AndroidDiag_ToRdramAlias(this->sceneSegment);
    scene->unk_D = 0;
    gSegments[2] = OS_K0_TO_PHYSICAL(this->sceneSegment);
    Play_InitScene(this, spawn);
    Room_AllocateAndLoad(this, &this->roomCtx);
}

static void AndroidDiag_EnsureNesFontCache(Font* font) {
    u32 rom;

    if (sAndroidNesFontStaticCacheReady) {
        return;
    }

    recomp_measure_latency(88, 0x80, (u32)font->fontBuf, SEGMENT_ROM_START(nes_font_static),
                           ANDROID_NES_FONT_STATIC_SIZE);
    rom = DmaMgr_TranslateVromToRom(SEGMENT_ROM_START(nes_font_static));
    if (rom != (u32)-1) {
        recomp_load_overlays(rom, font->fontBuf, ANDROID_NES_FONT_STATIC_SIZE);
    } else {
        recomp_printf("[AndroidLoadDiag] nes font cache translate failed vrom=%08llX\n",
                      (u64)SEGMENT_ROM_START(nes_font_static));
    }
    recomp_measure_latency(88, 0x81, (u32)font->fontBuf, SEGMENT_ROM_START(nes_font_static),
                           ANDROID_NES_FONT_STATIC_SIZE);
    AndroidDiag_CopyBytes(sAndroidNesFontStaticCache, font->fontBuf, ANDROID_NES_FONT_STATIC_SIZE);
    sAndroidNesFontStaticCacheReady = true;
    recomp_measure_latency(88, 0x82, (u32)sAndroidNesFontStaticCache, SEGMENT_ROM_START(nes_font_static),
                           ANDROID_NES_FONT_STATIC_SIZE);
}

RECOMP_PATCH void Font_LoadCharNES(PlayState* play, u8 codePointIndex, s32 offset) {
    MessageContext* msgCtx = &play->msgCtx;
    Font* font = &msgCtx->font;
    u8* writeLocation = &font->charBuf[font->unk_11D88][offset];
    uintptr_t vrom = SEGMENT_ROM_START_OFFSET(nes_font_static, (codePointIndex - ' ') * FONT_CHAR_TEX_SIZE);

    if (recomp_android_should_use_sync_boot_dma()) {
        u32 loadOffset = vrom - SEGMENT_ROM_START(nes_font_static);

        AndroidDiag_EnsureNesFontCache(font);
        if (loadOffset + FONT_CHAR_TEX_SIZE <= ANDROID_NES_FONT_STATIC_SIZE) {
            AndroidDiag_CopyBytes(writeLocation, &sAndroidNesFontStaticCache[loadOffset], FONT_CHAR_TEX_SIZE);
        }
    } else {
        DmaMgr_SendRequest0(writeLocation, vrom, FONT_CHAR_TEX_SIZE);
    }
}

RECOMP_PATCH void Font_LoadMessageBoxEndIcon(Font* font, u16 icon) {
    uintptr_t vrom = SEGMENT_ROM_START_OFFSET(message_static, 5 * 0x1000 + icon * FONT_CHAR_TEX_SIZE);

    if (recomp_android_should_use_sync_boot_dma()) {
        AndroidDiag_LoadRomToRam(&font->iconBuf, vrom, FONT_CHAR_TEX_SIZE, "font end icon");
    } else {
        DmaMgr_SendRequest0(&font->iconBuf, vrom, FONT_CHAR_TEX_SIZE);
    }
}

static u8 sAndroidFontOrdering[] = {
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26,
    0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36,
    0x37, 0x38, 0x39, 0x3A, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C,
    0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x00, 0x0D,
    0x0E, 0x1A, 0x61, 0x66, 0x6A, 0x6D, 0x6F, 0x73, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D,
    0x7E, 0x7F, 0x80, 0x81, 0x84, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C,
};

RECOMP_PATCH void Font_LoadOrderedFont(Font* font) {
    u32 loadOffset;
    s32 codePointIndex = 0;
    u8* writeLocation;

    if (recomp_android_should_use_sync_boot_dma()) {
        AndroidDiag_EnsureNesFontCache(font);
        recomp_measure_latency(88, 0x83, (u32)font, (u32)font->fontBuf, 0);
    }

    while (1) {
        writeLocation = &font->fontBuf[codePointIndex * FONT_CHAR_TEX_SIZE];
        loadOffset = sAndroidFontOrdering[codePointIndex] * FONT_CHAR_TEX_SIZE;
        if (sAndroidFontOrdering[codePointIndex] == 0) {
            loadOffset = 0;
        }

        if (recomp_android_should_use_sync_boot_dma()) {
            if (loadOffset + FONT_CHAR_TEX_SIZE <= ANDROID_NES_FONT_STATIC_SIZE) {
                AndroidDiag_CopyBytes(writeLocation, &sAndroidNesFontStaticCache[loadOffset], FONT_CHAR_TEX_SIZE);
            }
        } else {
            DmaMgr_SendRequest0(writeLocation, SEGMENT_ROM_START(nes_font_static) + loadOffset, FONT_CHAR_TEX_SIZE);
        }

        if (sAndroidFontOrdering[codePointIndex] == 0x8C) {
            break;
        }
        codePointIndex++;
    }

    if (recomp_android_should_use_sync_boot_dma()) {
        recomp_measure_latency(88, 0x84, (u32)font, (u32)font->fontBuf, codePointIndex);
    }
}

RECOMP_PATCH void Message_Init(PlayState* play) {
    Font* font;
    MessageContext* msgCtx = &play->msgCtx;
    s32 samsungDiag = recomp_android_should_use_sync_boot_dma();

    if (samsungDiag) {
        recomp_measure_latency(88, 1, (u32)play, (u32)msgCtx, 0);
    }

    Message_SetTables(play);

    if (samsungDiag) {
        recomp_measure_latency(88, 2, (u32)play->msgCtx.messageEntryTableNes, (u32)play->msgCtx.messageTableStaff, 0);
    }

    play->msgCtx.ocarinaMode = OCARINA_MODE_NONE;

    msgCtx->msgMode = MSGMODE_NONE;
    msgCtx->msgLength = 0;
    msgCtx->currentTextId = 0;
    msgCtx->textboxEndType = TEXTBOX_ENDTYPE_00;
    msgCtx->choiceIndex = 0;
    msgCtx->ocarinaAction = msgCtx->textUnskippable = 0;
    msgCtx->textColorAlpha = 0xFF;

    if (samsungDiag) {
        recomp_measure_latency(88, 3, (u32)play->state.gfxCtx, 0, 0);
    }

    View_Init(&msgCtx->view, play->state.gfxCtx);

    if (samsungDiag) {
        recomp_measure_latency(88, 4, (u32)&msgCtx->view, (u32)play->state.tha.start, (u32)play->state.tha.size);
    }

    msgCtx->textboxSegment = THA_AllocTailAlign16(&play->state.tha, 0x13C00);

    if (samsungDiag) {
        recomp_measure_latency(88, 5, (u32)msgCtx->textboxSegment, (u32)play->state.tha.start,
                               (u32)play->state.tha.size);
    }

    font = &play->msgCtx.font;
    Font_LoadOrderedFont(&play->msgCtx.font);

    if (samsungDiag) {
        recomp_measure_latency(88, 6, (u32)font, (u32)font->fontBuf, (u32)font->charBuf);
    }

    font->unk_11D88 = 0;

    msgCtx->textIsCredits = msgCtx->messageHasSetSfx = false;
    msgCtx->textboxSkipped = false;
    msgCtx->textFade = false;
    msgCtx->ocarinaAvailableSongs = 0;
    msgCtx->textboxX = 52;
    msgCtx->textboxY = 36;
    msgCtx->ocarinaSongEffectActive = false;
    msgCtx->unk120BE = 0;
    msgCtx->unk120C0 = 0;
    msgCtx->unk120C2 = 0;
    msgCtx->unk120C4 = 0;
    msgCtx->unk120C8 = 0;
    msgCtx->unk120CA = 0;
    msgCtx->unk120CC = 0;
    msgCtx->unk120CE = 0;
    msgCtx->unk120D0 = 0;
    msgCtx->unk120D2 = 0;
    msgCtx->unk120D4 = 0;
    msgCtx->unk120D6 = 0;

    if (samsungDiag) {
        recomp_measure_latency(88, 7, (u32)play, (u32)msgCtx->textboxSegment, 0);
    }
}

RECOMP_PATCH s32 Object_SpawnPersistent(ObjectContext* objectCtx, s16 id) {
    size_t size;
    bool samsungDiag = recomp_android_should_use_sync_boot_dma();
    RomFile objectFile;

    AndroidDiag_GetObjectFile(id, &objectFile);

    if (objectCtx->numEntries >= ARRAY_COUNT(objectCtx->slots)) {
        recomp_crash("Object context persistent slot overflow");
    }

    if (samsungDiag) {
        recomp_measure_latency(98, 0, (u32)(uintptr_t)objectCtx, (u32)(s32)id, (u32)objectCtx->numEntries);
        recomp_measure_latency(98, 1, (u32)(uintptr_t)objectCtx->slots[objectCtx->numEntries].segment, 0, 0);
    }

    if (samsungDiag) {
        recomp_measure_latency(98, 3, (u32)objectFile.vromStart, (u32)objectFile.vromEnd,
                               (u32)(objectFile.vromEnd - objectFile.vromStart));
        recomp_measure_latency(98, 4, 0x0108B000, 0x0111D5E0, 0x000925E0);
    }

    objectCtx->slots[objectCtx->numEntries].id = id;
    size = objectFile.vromEnd - objectFile.vromStart;

    if (size != 0) {
        AndroidDiag_LoadRomToRam(objectCtx->slots[objectCtx->numEntries].segment, objectFile.vromStart, size,
                                 "object persistent");
        CompatPmm_TryLoadSelectedObject(id, objectCtx->slots[objectCtx->numEntries].segment, size,
                                        "persistent");
    }

    if (objectCtx->numEntries < ARRAY_COUNT(objectCtx->slots) - 1) {
        objectCtx->slots[objectCtx->numEntries + 1].segment =
            (void*)ALIGN16((uintptr_t)objectCtx->slots[objectCtx->numEntries].segment + size);
    }

    objectCtx->numEntries++;
    objectCtx->numPersistentEntries = objectCtx->numEntries;

    return objectCtx->numEntries - 1;
}

RECOMP_PATCH void Object_InitContext(GameState* gameState, ObjectContext* objectCtx) {
    PlayState* play = (PlayState*)gameState;
    s32 pad;
    u32 spaceSize;
    s32 i;

    if (play->sceneId == SCENE_CLOCKTOWER || play->sceneId == SCENE_TOWN || play->sceneId == SCENE_BACKTOWN ||
        play->sceneId == SCENE_ICHIBA) {
        spaceSize = 1530 * 1024;
    } else if (play->sceneId == SCENE_MILK_BAR) {
        spaceSize = 1580 * 1024;
    } else if (play->sceneId == SCENE_00KEIKOKU) {
        spaceSize = 1470 * 1024;
    } else {
        spaceSize = 1380 * 1024;
    }

    objectCtx->numEntries = 0;
    objectCtx->numPersistentEntries = 0;
    objectCtx->mainKeepSlot = 0;
    objectCtx->subKeepSlot = 0;

    for (i = 0; i < ARRAY_COUNT(objectCtx->slots); i++) {
        objectCtx->slots[i].id = 0;
    }

    objectCtx->spaceStart = objectCtx->slots[0].segment = THA_AllocTailAlign16(&gameState->tha, spaceSize);
    if (objectCtx->spaceStart == NULL) {
        recomp_crash("Object context allocation failed");
    }

    objectCtx->spaceEnd = (void*)((uintptr_t)objectCtx->spaceStart + spaceSize);
    objectCtx->mainKeepSlot = Object_SpawnPersistent(objectCtx, GAMEPLAY_KEEP);

    gSegments[4] = OS_K0_TO_PHYSICAL(objectCtx->slots[objectCtx->mainKeepSlot].segment);
}

RECOMP_PATCH void Play_InitScene(PlayState* this, s32 spawn) {
    s32 samsungDiag = recomp_android_should_use_sync_boot_dma();

    this->curSpawn = spawn;
    this->linkActorEntry = NULL;
    this->actorCsCamList = NULL;
    this->setupEntranceList = NULL;
    this->setupExitList = NULL;
    this->naviQuestHints = NULL;
    this->setupPathList = NULL;
    this->sceneMaterialAnims = NULL;
    this->roomCtx.unk74 = NULL;
    this->numSetupActors = 0;

    if (samsungDiag) {
        recomp_measure_latency(95, 0x10, (u32)this, (u32)this->sceneSegment, (u32)this->sceneId);
    }

    Object_InitContext(&this->state, &this->objectCtx);
    LightContext_Init(this, &this->lightCtx);
    Door_InitContext(&this->state, &this->doorCtx);
    Room_Init(this, &this->roomCtx);
    gSaveContext.worldMapArea = 0;

    if (samsungDiag) {
        recomp_measure_latency(95, 0x11, (u32)this->sceneSegment, (u32)this->objectCtx.mainKeepSlot,
                               (u32)this->objectCtx.numEntries);
    }

    Scene_ExecuteCommands(this, this->sceneSegment);

    if (samsungDiag) {
        recomp_measure_latency(95, 0x12, (u32)this->sceneSegment, (u32)this->linkActorEntry,
                               (u32)this->setupEntranceList);
    }

    Play_InitEnvironment(this, this->skyboxId);

    if (samsungDiag) {
        recomp_measure_latency(95, 0x13, (u32)this->sceneSegment, (u32)this->skyboxId,
                               (u32)this->envCtx.skyboxConfig);
    }
}

RECOMP_PATCH void Skybox_Setup(GameState* gameState, SkyboxContext* skyboxCtx, s16 skyboxId) {
    PlayState* play = (PlayState*)gameState;
    size_t size;
    void* segment;
    s32 samsungDiag = recomp_android_should_use_sync_boot_dma();

    skyboxCtx->rot.z = 0.0f;

    switch (skyboxId) {
        case SKYBOX_NORMAL_SKY:
            skyboxCtx->staticSegments[0] = gLoBuffer.skyboxBuffer;
            size = SEGMENT_ROM_SIZE(d2_cloud_static);
            segment = (void*)ALIGN8((uintptr_t)skyboxCtx->staticSegments[0] + size);
            if (samsungDiag) {
                recomp_measure_latency(97, 0x50, SEGMENT_ROM_START(d2_cloud_static),
                                       (u32)(uintptr_t)skyboxCtx->staticSegments[0], (u32)size);
                AndroidDiag_LoadRomToRam(skyboxCtx->staticSegments[0], SEGMENT_ROM_START(d2_cloud_static), size,
                                         "skybox cloud");
            } else {
                DmaMgr_SendRequest0(skyboxCtx->staticSegments[0], SEGMENT_ROM_START(d2_cloud_static), size);
            }

            skyboxCtx->staticSegments[1] = segment;
            size = SEGMENT_ROM_SIZE(d2_fine_static);
            segment = (void*)ALIGN8((uintptr_t)segment + size);
            if (samsungDiag) {
                recomp_measure_latency(97, 0x51, SEGMENT_ROM_START(d2_fine_static),
                                       (u32)(uintptr_t)skyboxCtx->staticSegments[1], (u32)size);
                AndroidDiag_LoadRomToRam(skyboxCtx->staticSegments[1], SEGMENT_ROM_START(d2_fine_static), size,
                                         "skybox fine");
            } else {
                DmaMgr_SendRequest0(skyboxCtx->staticSegments[1], SEGMENT_ROM_START(d2_fine_static), size);
            }

            skyboxCtx->paletteStaticSegment = segment;
            size = SEGMENT_ROM_SIZE(d2_fine_pal_static);
            segment = (void*)ALIGN8((uintptr_t)segment + size);
            if (samsungDiag) {
                recomp_measure_latency(97, 0x52, SEGMENT_ROM_START(d2_fine_pal_static),
                                       (u32)(uintptr_t)skyboxCtx->paletteStaticSegment, (u32)size);
                AndroidDiag_LoadRomToRam(skyboxCtx->paletteStaticSegment, SEGMENT_ROM_START(d2_fine_pal_static), size,
                                         "skybox palette");
            } else {
                DmaMgr_SendRequest0(skyboxCtx->paletteStaticSegment, SEGMENT_ROM_START(d2_fine_pal_static), size);
            }

            skyboxCtx->prim.r = 145;
            skyboxCtx->prim.g = 120;
            skyboxCtx->prim.b = 155;

            skyboxCtx->env.r = 40;
            skyboxCtx->env.g = 0;
            skyboxCtx->env.b = 40;

            if ((play->sceneId == SCENE_F41) || (play->sceneId == SCENE_INISIE_R)) {
                skyboxCtx->rot.z = 3.15f;
            }
            break;

        case SKYBOX_2:
            break;

        default:
            break;
    }
}

RECOMP_PATCH void Skybox_Init(GameState* gameState, SkyboxContext* skyboxCtx, s16 skyboxId) {
    skyboxCtx->skyboxShouldDraw = false;
    skyboxCtx->rot.x = skyboxCtx->rot.y = skyboxCtx->rot.z = 0.0f;

    Skybox_Setup(gameState, skyboxCtx, skyboxId);

    if (skyboxId != SKYBOX_NONE) {
        skyboxCtx->dListBuf = AndroidDiag_ToRdramAlias(THA_AllocTailAlign16(&gameState->tha, 0x3840));

        if (skyboxId == SKYBOX_CUTSCENE_MAP) {
            skyboxCtx->roomVtx =
                AndroidDiag_ToRdramAlias(THA_AllocTailAlign16(&gameState->tha, sizeof(Vtx) * 32 * 6));
            func_80143148(skyboxCtx, 6);
        } else {
            skyboxCtx->roomVtx =
                AndroidDiag_ToRdramAlias(THA_AllocTailAlign16(&gameState->tha, sizeof(Vtx) * 32 * 5));
            func_80143148(skyboxCtx, 5);
        }
    }
}

RECOMP_PATCH void Object_UpdateEntries(ObjectContext* objectCtx) {
    s32 i;
    ObjectEntry* entry = &objectCtx->slots[0];
    RomFile objectFile;
    size_t size;

    for (i = 0; i < objectCtx->numEntries; i++) {
        if (entry->id < 0) {
            s32 id = -entry->id;

            if (entry->dmaReq.vromAddr == 0) {
                AndroidDiag_GetObjectFile(id, &objectFile);
                size = objectFile.vromEnd - objectFile.vromStart;

                if (size == 0) {
                    entry->id = 0;
                } else if (recomp_android_should_use_sync_boot_dma()) {
                    AndroidDiag_LoadRomToRam(entry->segment, objectFile.vromStart, size, "object async-sync");
                    CompatPmm_TryLoadSelectedObject(id, entry->segment, size, "async-sync");
                    entry->id = id;
                } else {
                    osCreateMesgQueue(&entry->loadQueue, &entry->loadMsg, 1);
                    DmaMgr_SendRequestImpl(&entry->dmaReq, entry->segment, objectFile.vromStart, size, 0,
                                           &entry->loadQueue, NULL);
                }
            } else if (!osRecvMesg(&entry->loadQueue, NULL, OS_MESG_NOBLOCK)) {
                AndroidDiag_GetObjectFile(id, &objectFile);
                CompatPmm_TryLoadSelectedObject(id, entry->segment, objectFile.vromEnd - objectFile.vromStart,
                                                "async");
                entry->id = id;
            }
        }

        entry++;
    }
}

RECOMP_PATCH void Object_LoadAll(ObjectContext* objectCtx) {
    s32 i;
    s32 id;
    uintptr_t vromSize;
    RomFile objectFile;

    for (i = 0; i < objectCtx->numEntries; i++) {
        id = objectCtx->slots[i].id;
        AndroidDiag_GetObjectFile(id, &objectFile);
        vromSize = objectFile.vromEnd - objectFile.vromStart;

        if (vromSize == 0) {
            continue;
        }

        AndroidDiag_LoadRomToRam(objectCtx->slots[i].segment, objectFile.vromStart, vromSize, "object all");
        CompatPmm_TryLoadSelectedObject(id, objectCtx->slots[i].segment, vromSize, "all");
    }
}

RECOMP_PATCH void* func_8012F73C(ObjectContext* objectCtx, s32 slot, s16 id) {
    uintptr_t addr;
    uintptr_t vromSize;
    RomFile fileTableEntry;

    if (slot < 0 || slot >= ARRAY_COUNT(objectCtx->slots)) {
        recomp_crash("Object slot out of bounds");
    }

    objectCtx->slots[slot].id = -id;
    objectCtx->slots[slot].dmaReq.vromAddr = 0;

    AndroidDiag_GetObjectFile(id, &fileTableEntry);
    vromSize = fileTableEntry.vromEnd - fileTableEntry.vromStart;

    addr = ((uintptr_t)objectCtx->slots[slot].segment) + vromSize;
    addr = ALIGN16(addr);

    return (void*)addr;
}

static void AndroidDiag_DispatchSceneCommand(PlayState* play, SceneCmd* sceneCmd, u32 cmdId) {
    switch (cmdId) {
        case SCENE_CMD_ID_SPAWN_LIST:
            Scene_CommandSpawnList(play, sceneCmd);
            break;
        case SCENE_CMD_ID_ACTOR_LIST:
            Scene_CommandActorList(play, sceneCmd);
            break;
        case SCENE_CMD_ID_ACTOR_CUTSCENE_CAM_LIST:
            Scene_CommandActorCutsceneCamList(play, sceneCmd);
            break;
        case SCENE_CMD_ID_COL_HEADER:
            Scene_CommandCollisionHeader(play, sceneCmd);
            break;
        case SCENE_CMD_ID_ROOM_LIST:
            Scene_CommandRoomList(play, sceneCmd);
            break;
        case SCENE_CMD_ID_WIND_SETTINGS:
            Scene_CommandWindSettings(play, sceneCmd);
            break;
        case SCENE_CMD_ID_ENTRANCE_LIST:
            Scene_CommandEntranceList(play, sceneCmd);
            break;
        case SCENE_CMD_ID_SPECIAL_FILES:
            Scene_CommandSpecialFiles(play, sceneCmd);
            break;
        case SCENE_CMD_ID_ROOM_BEHAVIOR:
            Scene_CommandRoomBehavior(play, sceneCmd);
            break;
        case SCENE_CMD_ID_UNK_09:
            Scene_Command09(play, sceneCmd);
            break;
        case SCENE_CMD_ID_ROOM_SHAPE:
            Scene_CommandMesh(play, sceneCmd);
            break;
        case SCENE_CMD_ID_OBJECT_LIST:
            Scene_CommandObjectList(play, sceneCmd);
            break;
        case SCENE_CMD_ID_LIGHT_LIST:
            Scene_CommandLightList(play, sceneCmd);
            break;
        case SCENE_CMD_ID_PATH_LIST:
            Scene_CommandPathList(play, sceneCmd);
            break;
        case SCENE_CMD_ID_TRANSI_ACTOR_LIST:
            Scene_CommandTransiActorList(play, sceneCmd);
            break;
        case SCENE_CMD_ID_ENV_LIGHT_SETTINGS:
            Scene_CommandEnvLightSettings(play, sceneCmd);
            break;
        case SCENE_CMD_ID_TIME_SETTINGS:
            Scene_CommandTimeSettings(play, sceneCmd);
            break;
        case SCENE_CMD_ID_SKYBOX_SETTINGS:
            Scene_CommandSkyboxSettings(play, sceneCmd);
            break;
        case SCENE_CMD_ID_SKYBOX_DISABLES:
            Scene_CommandSkyboxDisables(play, sceneCmd);
            break;
        case SCENE_CMD_ID_EXIT_LIST:
            Scene_CommandExitList(play, sceneCmd);
            break;
        case SCENE_CMD_ID_SOUND_SETTINGS:
            Scene_CommandSoundSettings(play, sceneCmd);
            break;
        case SCENE_CMD_ID_ECHO_SETTINGS:
            Scene_CommandEchoSetting(play, sceneCmd);
            break;
        case SCENE_CMD_ID_CUTSCENE_SCRIPT_LIST:
            Scene_CommandCutsceneScriptList(play, sceneCmd);
            break;
        case SCENE_CMD_ID_ALTERNATE_HEADER_LIST:
            Scene_CommandAltHeaderList(play, sceneCmd);
            break;
        case SCENE_CMD_ID_SET_REGION_VISITED:
            Scene_CommandSetRegionVisitedFlag(play, sceneCmd);
            break;
        case SCENE_CMD_ID_ANIMATED_MATERIAL_LIST:
            Scene_CommandAnimatedMaterials(play, sceneCmd);
            break;
        case SCENE_CMD_ID_ACTOR_CUTSCENE_LIST:
            Scene_CommandCutsceneList(play, sceneCmd);
            break;
        case SCENE_CMD_ID_MINIMAP_INFO:
            Scene_CommandMiniMap(play, sceneCmd);
            break;
        case SCENE_CMD_ID_UNUSED_1D:
            Scene_Command1D(play, sceneCmd);
            break;
        case SCENE_CMD_ID_MINIMAP_COMPASS_ICON_INFO:
            Scene_CommandMiniMapCompassInfo(play, sceneCmd);
            break;
        default:
            recomp_printf("[AndroidLoadDiag] invalid scene command id=%u ptr=%08llX data1=%u data2=%08X\n", cmdId,
                          (u64)(uintptr_t)sceneCmd, sceneCmd->base.data1, sceneCmd->base.data2);
            recomp_crash("Invalid Android scene command");
            break;
    }
}

RECOMP_PATCH void Scene_CommandCollisionHeader(PlayState* play, SceneCmd* cmd) {
    CollisionHeader* colHeaderTemp;
    CollisionHeader* colHeader;

    colHeaderTemp = AndroidDiag_ToRdramAlias(Lib_SegmentedToVirtual(cmd->colHeader.segment));
    colHeader = colHeaderTemp;

    colHeader->vtxList = AndroidDiag_ToRdramAlias(Lib_SegmentedToVirtual(colHeaderTemp->vtxList));
    colHeader->polyList = AndroidDiag_ToRdramAlias(Lib_SegmentedToVirtual(colHeader->polyList));

    if (colHeader->surfaceTypeList != NULL) {
        colHeader->surfaceTypeList = AndroidDiag_ToRdramAlias(Lib_SegmentedToVirtual(colHeader->surfaceTypeList));
    }

    if (colHeader->bgCamList != NULL) {
        colHeader->bgCamList = AndroidDiag_ToRdramAlias(Lib_SegmentedToVirtual(colHeader->bgCamList));
    }

    if (colHeader->waterBoxes != NULL) {
        colHeader->waterBoxes = AndroidDiag_ToRdramAlias(Lib_SegmentedToVirtual(colHeader->waterBoxes));
    }

    BgCheck_Allocate(&play->colCtx, play, colHeader);
}

RECOMP_PATCH void Scene_CommandSpawnList(PlayState* play, SceneCmd* cmd) {
    s32 loadedCount;
    s16 playerObjectId;
    s32 playerForm;
    void* objectPtr;
    ActorInit* playerInit;
    ActorEntry* spawnList = AndroidDiag_ToRdramAlias(Lib_SegmentedToVirtual(cmd->spawnList.segment));

    play->setupEntranceList = AndroidDiag_ToRdramAlias(play->setupEntranceList);
    play->linkActorEntry = AndroidDiag_ToRdramAlias(spawnList + play->setupEntranceList[play->curSpawn].spawn);
    recomp_measure_latency(99, 0, (u32)(uintptr_t)spawnList, (u32)(uintptr_t)play->setupEntranceList,
                           (u32)(uintptr_t)play->linkActorEntry);
    if ((PLAYER_GET_INITMODE(play->linkActorEntry) == PLAYER_INITMODE_TELESCOPE) ||
        ((gSaveContext.respawnFlag == 2) &&
         (gSaveContext.respawn[RESPAWN_MODE_RETURN].playerParams == PLAYER_PARAMS(0xFF, PLAYER_INITMODE_TELESCOPE)))) {
        Object_SpawnPersistent(&play->objectCtx, OBJECT_STK);
        return;
    }

    loadedCount = Object_SpawnPersistent(&play->objectCtx, OBJECT_LINK_CHILD);
    objectPtr = play->objectCtx.slots[play->objectCtx.numEntries].segment;
    play->objectCtx.numEntries = loadedCount;
    play->objectCtx.numPersistentEntries = loadedCount;
    playerForm = GET_PLAYER_FORM;
    if (recomp_android_should_use_sync_boot_dma() && ((playerForm < PLAYER_FORM_FIERCE_DEITY) || (playerForm >= PLAYER_FORM_MAX))) {
        recomp_measure_latency(99, 2, (u32)playerForm, (u32)gSaveContext.save.playerForm, 0);
        playerForm = PLAYER_FORM_HUMAN;
        gSaveContext.save.playerForm = PLAYER_FORM_HUMAN;
    }
    playerObjectId = AndroidDiag_GetPlayerObjectId(playerForm);
    playerInit = AndroidDiag_ToRdramAlias(&Player_InitVars);
    recomp_measure_latency(99, 1, (u32)loadedCount, (u32)(uintptr_t)objectPtr, (u32)(uintptr_t)playerInit);
    recomp_measure_latency(99, 3, (u32)playerForm, (u32)(s32)playerObjectId, (u32)(uintptr_t)&gPlayerFormObjectIds[0]);
    playerInit->objectId = playerObjectId;
    Object_SpawnPersistent(&play->objectCtx, playerObjectId);

    play->objectCtx.slots[play->objectCtx.numEntries].segment = objectPtr;
}

RECOMP_PATCH void Scene_CommandActorList(PlayState* play, SceneCmd* cmd) {
    play->numSetupActors = cmd->actorList.num;
    play->setupActorList = AndroidDiag_ToRdramAlias(Lib_SegmentedToVirtual(cmd->actorList.segment));
    play->actorCtx.halfDaysBit = 0;
}

RECOMP_PATCH void Scene_CommandActorCutsceneCamList(PlayState* play, SceneCmd* cmd) {
    play->actorCsCamList = AndroidDiag_ToRdramAlias(Lib_SegmentedToVirtual(cmd->actorCsCamList.segment));
}

RECOMP_PATCH void Scene_CommandRoomList(PlayState* play, SceneCmd* cmd) {
    play->numRooms = cmd->roomList.num;
    play->roomList = AndroidDiag_ToRdramAlias(Lib_SegmentedToVirtual(cmd->roomList.segment));
}

RECOMP_PATCH void Scene_CommandEntranceList(PlayState* play, SceneCmd* cmd) {
    play->setupEntranceList = AndroidDiag_ToRdramAlias(Lib_SegmentedToVirtual(cmd->entranceList.segment));
}

RECOMP_PATCH void Scene_CommandMesh(PlayState* play, SceneCmd* cmd) {
    play->roomCtx.curRoom.roomShape = AndroidDiag_ToRdramAlias(Lib_SegmentedToVirtual(cmd->mesh.segment));
}

RECOMP_PATCH void Scene_CommandObjectList(PlayState* play, SceneCmd* cmd) {
    s32 i;
    s32 j;
    s32 k;
    ObjectEntry* firstObject;
    ObjectEntry* entry;
    ObjectEntry* invalidatedEntry;
    s16* objectEntry;
    void* nextPtr;

    objectEntry = AndroidDiag_ToRdramAlias(Lib_SegmentedToVirtual(cmd->objectList.segment));
    k = 0;
    i = play->objectCtx.numPersistentEntries;
    entry = &play->objectCtx.slots[i];
    firstObject = &play->objectCtx.slots[0];

    while (i < play->objectCtx.numEntries) {
        if (entry->id != *objectEntry) {
            invalidatedEntry = &play->objectCtx.slots[i];

            for (j = i; j < play->objectCtx.numEntries; j++) {
                invalidatedEntry->id = 0;
                invalidatedEntry++;
            }

            play->objectCtx.numEntries = i;
            Actor_KillAllWithMissingObject(play, &play->actorCtx);

            continue;
        }

        i++;
        k++;
        objectEntry++;
        entry++;
    }

    while (k < cmd->objectList.num) {
        nextPtr = func_8012F73C(&play->objectCtx, i, *objectEntry);

        if (i < ARRAY_COUNT(play->objectCtx.slots) - 1) {
            firstObject[i + 1].segment = nextPtr;
        }

        i++;
        k++;
        objectEntry++;
    }

    play->objectCtx.numEntries = i;
}

RECOMP_PATCH void Scene_CommandLightList(PlayState* play, SceneCmd* cmd) {
    s32 i;
    LightInfo* lightInfo = AndroidDiag_ToRdramAlias(Lib_SegmentedToVirtual(cmd->lightList.segment));

    for (i = 0; i < cmd->lightList.num; i++) {
        LightContext_InsertLight(play, &play->lightCtx, lightInfo);
        lightInfo++;
    }
}

RECOMP_PATCH void Scene_CommandPathList(PlayState* play, SceneCmd* cmd) {
    play->setupPathList = AndroidDiag_ToRdramAlias(Lib_SegmentedToVirtual(cmd->pathList.segment));
}

RECOMP_PATCH void Scene_CommandTransiActorList(PlayState* play, SceneCmd* cmd) {
    play->doorCtx.numTransitionActors = cmd->transiActorList.num;
    play->doorCtx.transitionActorList = AndroidDiag_ToRdramAlias(Lib_SegmentedToVirtual(cmd->transiActorList.segment));
    func_80105818(play, play->doorCtx.numTransitionActors, play->doorCtx.transitionActorList);
}

RECOMP_PATCH void Scene_CommandEnvLightSettings(PlayState* play, SceneCmd* cmd) {
    play->envCtx.numLightSettings = cmd->lightSettingList.num;
    play->envCtx.lightSettingsList = AndroidDiag_ToRdramAlias(Lib_SegmentedToVirtual(cmd->lightSettingList.segment));
}

RECOMP_PATCH void Scene_CommandExitList(PlayState* play, SceneCmd* cmd) {
    play->setupExitList = AndroidDiag_ToRdramAlias(Lib_SegmentedToVirtual(cmd->exitList.segment));
}

RECOMP_PATCH void Scene_CommandAltHeaderList(PlayState* play, SceneCmd* cmd) {
    SceneCmd** altHeaderList;
    SceneCmd* altHeader;

    if (gSaveContext.sceneLayer != 0) {
        altHeaderList = AndroidDiag_ToRdramAlias(Lib_SegmentedToVirtual(cmd->altHeaders.segment));
        altHeader = altHeaderList[gSaveContext.sceneLayer - 1];

        if (altHeader != NULL) {
            Scene_ExecuteCommands(play, AndroidDiag_ToRdramAlias(Lib_SegmentedToVirtual(altHeader)));
            (cmd + 1)->base.code = SCENE_CMD_ID_END;
        }
    }
}

RECOMP_PATCH void Scene_CommandCutsceneScriptList(PlayState* play, SceneCmd* cmd) {
    play->csCtx.scriptListCount = cmd->scriptList.scriptListCount;
    play->csCtx.scriptList = AndroidDiag_ToRdramAlias(Lib_SegmentedToVirtual(cmd->scriptList.segment));
}

RECOMP_PATCH void Scene_CommandCutsceneList(PlayState* play, SceneCmd* cmd) {
    CutsceneManager_Init(play, AndroidDiag_ToRdramAlias(Lib_SegmentedToVirtual(cmd->cutsceneList.segment)),
                         cmd->cutsceneList.num);
}

RECOMP_PATCH void Scene_CommandAnimatedMaterials(PlayState* play, SceneCmd* cmd) {
    play->sceneMaterialAnims = AndroidDiag_ToRdramAlias(Lib_SegmentedToVirtual(cmd->textureAnimations.segment));
}

RECOMP_PATCH void Scene_CommandMiniMap(PlayState* play, SceneCmd* cmd) {
    if (recomp_android_should_use_sync_boot_dma()) {
        recomp_measure_latency(96, 0x1C, (u32)play->sceneId, (u32)cmd->minimapSettings.segment, 0);
        play->interfaceCtx.minimapAlpha = 0;
        return;
    }

    func_80104CF4(play);
    func_8010549C(play, cmd->minimapSettings.segment);
}

RECOMP_PATCH void Scene_CommandMiniMapCompassInfo(PlayState* play, SceneCmd* cmd) {
    if (recomp_android_should_use_sync_boot_dma()) {
        recomp_measure_latency(96, 0x1E, (u32)play->sceneId, (u32)cmd->minimapChests.segment,
                               cmd->minimapChests.num);
        return;
    }

    func_8010565C(play, cmd->minimapChests.num, cmd->minimapChests.segment);
}

RECOMP_PATCH s32 Scene_ExecuteCommands(PlayState* play, SceneCmd* sceneCmd) {
    u32 cmdId;
    u32 commandIndex = 0;

    while (true) {
        cmdId = sceneCmd->base.code;

        if (recomp_android_should_use_sync_boot_dma()) {
            recomp_measure_latency(96, 0, (u32)(uintptr_t)sceneCmd, cmdId, commandIndex);
            recomp_measure_latency(96, 1, sceneCmd->base.data1, sceneCmd->base.data2, play->sceneId);
            recomp_printf("[AndroidLoadDiag] scene cmd index=%u id=%u ptr=%08llX data1=%u data2=%08X scene=%d\n",
                          commandIndex, cmdId, (u64)(uintptr_t)sceneCmd, sceneCmd->base.data1, sceneCmd->base.data2,
                          play->sceneId);
        }

        if (cmdId == SCENE_CMD_ID_END) {
            break;
        }

        if (cmdId < SCENE_CMD_MAX) {
            AndroidDiag_DispatchSceneCommand(play, sceneCmd, cmdId);
            if (recomp_android_should_use_sync_boot_dma()) {
                recomp_measure_latency(96, 2, cmdId, commandIndex, play->sceneId);
                recomp_printf("[AndroidLoadDiag] scene cmd done index=%u id=%u\n", commandIndex, cmdId);
            }
        }

        sceneCmd++;
        commandIndex++;
    }

    return 0;
}

RECOMP_PATCH void Scene_LoadAreaTextures(PlayState* play, s32 fileIndex) {
    static RomFile sceneTextureFiles[9] = {
        { 0, 0 },
        { SEGMENT_ROM_START(scene_texture_01), SEGMENT_ROM_END(scene_texture_01) },
        { SEGMENT_ROM_START(scene_texture_02), SEGMENT_ROM_END(scene_texture_02) },
        { SEGMENT_ROM_START(scene_texture_03), SEGMENT_ROM_END(scene_texture_03) },
        { SEGMENT_ROM_START(scene_texture_04), SEGMENT_ROM_END(scene_texture_04) },
        { SEGMENT_ROM_START(scene_texture_05), SEGMENT_ROM_END(scene_texture_05) },
        { SEGMENT_ROM_START(scene_texture_06), SEGMENT_ROM_END(scene_texture_06) },
        { SEGMENT_ROM_START(scene_texture_07), SEGMENT_ROM_END(scene_texture_07) },
        { SEGMENT_ROM_START(scene_texture_08), SEGMENT_ROM_END(scene_texture_08) },
    };
    uintptr_t vromStart;
    size_t size;

    if (fileIndex < 0 || fileIndex >= ARRAY_COUNT(sceneTextureFiles)) {
        recomp_printf("[AndroidLoadDiag] invalid area texture index=%d\n", fileIndex);
        return;
    }

    vromStart = sceneTextureFiles[fileIndex].vromStart;
    size = sceneTextureFiles[fileIndex].vromEnd - vromStart;

    if (size != 0) {
        play->roomCtx.unk74 = THA_AllocTailAlign16(&play->state.tha, size);
        if (play->roomCtx.unk74 == NULL) {
            recomp_crash("Area texture allocation failed");
        }
        AndroidDiag_LoadRomToRam(play->roomCtx.unk74, vromStart, size, "area texture");
    }
}

RECOMP_PATCH s32 Room_StartRoomTransition(PlayState* play, RoomContext* roomCtx, s32 index) {
    if (roomCtx->status == 0) {
        size_t size;

        if (index < 0 || index >= play->numRooms) {
            recomp_crash("Room transition index out of bounds");
        }

        roomCtx->prevRoom = roomCtx->curRoom;
        roomCtx->curRoom.num = index;
        roomCtx->curRoom.segment = NULL;
        roomCtx->status = 1;

        size = play->roomList[index].vromEnd - play->roomList[index].vromStart;
        roomCtx->activeRoomVram = (void*)(ALIGN16((uintptr_t)roomCtx->roomMemPages[roomCtx->activeMemPage] -
                                                  (size + 8) * roomCtx->activeMemPage - 7));

        if (roomCtx->activeRoomVram == NULL) {
            recomp_crash("Room transition active page is null");
        }

        osCreateMesgQueue(&roomCtx->loadQueue, roomCtx->loadMsg, ARRAY_COUNT(roomCtx->loadMsg));
        if (recomp_android_should_use_sync_boot_dma()) {
            AndroidDiag_LoadRomToRam(roomCtx->activeRoomVram, play->roomList[index].vromStart, size, "room transition");
            osSendMesg(&roomCtx->loadQueue, NULL, OS_MESG_NOBLOCK);
        } else {
            DmaMgr_SendRequestImpl(&roomCtx->dmaRequest, roomCtx->activeRoomVram, play->roomList[index].vromStart, size,
                                   0, &roomCtx->loadQueue, NULL);
        }
        roomCtx->activeMemPage ^= 1;

        return 1;
    }

    return 0;
}

RECOMP_PATCH size_t Room_AllocateAndLoad(PlayState* play, RoomContext* roomCtx) {
    size_t maxRoomSize = 0;
    size_t roomSize;
    s32 i;
    s32 j;
    s32 frontRoom;
    s32 backRoom;
    size_t frontRoomSize;
    size_t backRoomSize;
    size_t cumulRoomSize;

    for (i = 0; i < play->numRooms; i++) {
        roomSize = play->roomList[i].vromEnd - play->roomList[i].vromStart;
        maxRoomSize = MAX(roomSize, maxRoomSize);
    }

    if ((u32)play->doorCtx.numTransitionActors != 0) {
        TransitionActorEntry* transitionActor = &play->doorCtx.transitionActorList[0];

        for (j = 0; j < play->doorCtx.numTransitionActors; j++) {
            frontRoom = transitionActor->sides[0].room;
            backRoom = transitionActor->sides[1].room;
            frontRoomSize = (frontRoom < 0) ? 0 : play->roomList[frontRoom].vromEnd - play->roomList[frontRoom].vromStart;
            backRoomSize = (backRoom < 0) ? 0 : play->roomList[backRoom].vromEnd - play->roomList[backRoom].vromStart;
            cumulRoomSize = (frontRoom != backRoom) ? frontRoomSize + backRoomSize : frontRoomSize;

            maxRoomSize = MAX(cumulRoomSize, maxRoomSize);
            transitionActor++;
        }
    }

    roomCtx->roomMemPages[0] = THA_AllocTailAlign16(&play->state.tha, maxRoomSize);
    if (roomCtx->roomMemPages[0] == NULL) {
        recomp_crash("Room memory allocation failed");
    }
    roomCtx->roomMemPages[1] = (void*)((uintptr_t)roomCtx->roomMemPages[0] + maxRoomSize);
    roomCtx->activeMemPage = 0;
    roomCtx->status = 0;

    if ((gSaveContext.respawnFlag != 0) && (gSaveContext.respawnFlag != -2) && (gSaveContext.respawnFlag != -7)) {
        s32 respawnMode;

        if ((gSaveContext.respawnFlag == -8) || (gSaveContext.respawnFlag == -5) || (gSaveContext.respawnFlag == -4) ||
            ((gSaveContext.respawnFlag < 0) && (gSaveContext.respawnFlag != -1) && (gSaveContext.respawnFlag != -6))) {
            respawnMode = RESPAWN_MODE_DOWN;
        } else if (gSaveContext.respawnFlag < 0) {
            respawnMode = RESPAWN_MODE_TOP;
        } else {
            respawnMode = gSaveContext.respawnFlag - 1;
        }
        frontRoom = gSaveContext.respawn[respawnMode].roomIndex;
    } else {
        frontRoom = play->setupEntranceList[play->curSpawn].room;
    }

    Room_StartRoomTransition(play, roomCtx, frontRoom);

    return maxRoomSize;
}

// @recomp_export void recomp_set_allow_no_ocarina_tf(bool new_val): Set whether to force Termina Field to load normally even if Link has no ocarina.
RECOMP_EXPORT void recomp_set_allow_no_ocarina_tf(bool new_val) {
    allow_no_ocarina_tf = new_val;
}

RECOMP_PATCH void Play_Init(GameState* thisx) {
    PlayState* this = (PlayState*)thisx;
    GraphicsContext* gfxCtx = this->state.gfxCtx;
    s32 samsungDiag = recomp_android_should_use_sync_boot_dma();
    u32 roomLoadSpin = 0;
    s32 pad;
    uintptr_t zAlloc;
    s32 zAllocSize;
    Player* player;
    s32 i;
    s32 spawn;
    u8 sceneLayer;
    s32 scene;

    if (samsungDiag) {
        recomp_measure_latency(80, (u32)this, (u32)gSaveContext.save.entrance, (u32)gSaveContext.gameMode,
                               (u32)gSaveContext.save.cutsceneIndex);
    }

    // @recomp_event recomp_on_play_init(PlayState* this): A new PlayState is being initialized.
    recomp_on_play_init(this);
#if defined(ZELDA_ANDROID_BUILTIN_CHEATS)
    CompatCheats_OnPlayInit(this);
#endif

    if (samsungDiag) {
        recomp_measure_latency(81, (u32)this, (u32)gSaveContext.respawnFlag, (u32)gSaveContext.nextCutsceneIndex,
                               (u32)gSaveContext.nextDayTime);
    }

    if ((gSaveContext.respawnFlag == -4) || (gSaveContext.respawnFlag == -0x63)) {
        if (CHECK_EVENTINF(EVENTINF_TRIGGER_DAYTELOP)) {
            CLEAR_EVENTINF(EVENTINF_TRIGGER_DAYTELOP);
            STOP_GAMESTATE(&this->state);
            // @recomp Use non-relocatable reference to DayTelop_Init instead.
            SET_NEXT_GAMESTATE(&this->state, DayTelop_Init_NORELOCATE, sizeof(DayTelopState));
            return;
        }

        gSaveContext.unk_3CA7 = 1;
        if (gSaveContext.respawnFlag == -0x63) {
            gSaveContext.respawnFlag = 2;
        }
    } else {
        gSaveContext.unk_3CA7 = 0;
    }

    if (gSaveContext.save.entrance == -1) {
        gSaveContext.save.entrance = 0;
        STOP_GAMESTATE(&this->state);
        // @recomp Use non-relocatable reference to TitleSetup_Init instead.
        SET_NEXT_GAMESTATE(&this->state, TitleSetup_Init_NORELOCATE, sizeof(TitleSetupState));
        return;
    }

    if ((gSaveContext.nextCutsceneIndex == 0xFFEF) || (gSaveContext.nextCutsceneIndex == 0xFFF0)) {
        scene = ((void)0, gSaveContext.save.entrance) >> 9;
        spawn = (((void)0, gSaveContext.save.entrance) >> 4) & 0x1F;

        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_SNOWHEAD_TEMPLE)) {
            if (scene == ENTR_SCENE_MOUNTAIN_VILLAGE_WINTER) {
                scene = ENTR_SCENE_MOUNTAIN_VILLAGE_SPRING;
            } else if (scene == ENTR_SCENE_GORON_VILLAGE_WINTER) {
                scene = ENTR_SCENE_GORON_VILLAGE_SPRING;
            } else if (scene == ENTR_SCENE_PATH_TO_GORON_VILLAGE_WINTER) {
                scene = ENTR_SCENE_PATH_TO_GORON_VILLAGE_SPRING;
            } else if ((scene == ENTR_SCENE_SNOWHEAD) || (scene == ENTR_SCENE_PATH_TO_SNOWHEAD) ||
                       (scene == ENTR_SCENE_PATH_TO_MOUNTAIN_VILLAGE) || (scene == ENTR_SCENE_GORON_SHRINE) ||
                       (scene == ENTR_SCENE_GORON_RACETRACK)) {
                gSaveContext.nextCutsceneIndex = 0xFFF0;
            }
        }

        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_WOODFALL_TEMPLE)) {
            if (scene == ENTR_SCENE_SOUTHERN_SWAMP_POISONED) {
                scene = ENTR_SCENE_SOUTHERN_SWAMP_CLEARED;
            } else if (scene == ENTR_SCENE_WOODFALL) {
                gSaveContext.nextCutsceneIndex = 0xFFF1;
            }
        }

        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_STONE_TOWER_TEMPLE) && (scene == ENTR_SCENE_IKANA_CANYON)) {
            gSaveContext.nextCutsceneIndex = 0xFFF2;
        }

        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_GREAT_BAY_TEMPLE) &&
            ((scene == ENTR_SCENE_GREAT_BAY_COAST) || (scene == ENTR_SCENE_ZORA_CAPE))) {
            gSaveContext.nextCutsceneIndex = 0xFFF0;
        }

        // "First cycle" Termina Field
        // @recomp_use_export_var allow_no_ocarina_tf: Skip loading into "First cycle" Termina Field if mods enable it.
        if (!allow_no_ocarina_tf && INV_CONTENT(ITEM_OCARINA_OF_TIME) != ITEM_OCARINA_OF_TIME) {
            if ((scene == ENTR_SCENE_TERMINA_FIELD) &&
                (((void)0, gSaveContext.save.entrance) != ENTRANCE(TERMINA_FIELD, 10))) {
                gSaveContext.nextCutsceneIndex = 0xFFF4;
            }
        }
        //! FAKE:
        gSaveContext.save.entrance =
            Entrance_Create(((void)0, scene), spawn, ((void)0, gSaveContext.save.entrance) & 0xF);
    }

    if (samsungDiag) {
        recomp_measure_latency(82, (u32)this, (u32)gSaveContext.save.entrance, (u32)gSaveContext.sceneLayer,
                               (u32)gSaveContext.save.cutsceneIndex);
    }

    GameState_Realloc(&this->state, 0);
    KaleidoManager_Init(this);
    ShrinkWindow_Init();
    View_Init(&this->view, gfxCtx);
    Audio_SetExtraFilter(0);
    Quake_Init();
    Distortion_Init(this);

    if (samsungDiag) {
        recomp_measure_latency(83, (u32)this, (u32)this->state.tha.start, (u32)this->state.tha.size,
                               (u32)this->state.gfxCtx);
    }

    for (i = 0; i < ARRAY_COUNT(this->cameraPtrs); i++) {
        this->cameraPtrs[i] = NULL;
    }

    Camera_Init(&this->mainCamera, &this->view, &this->colCtx, this);
    Camera_ChangeStatus(&this->mainCamera, CAM_STATUS_ACTIVE);

    for (i = 0; i < ARRAY_COUNT(this->subCameras); i++) {
        Camera_Init(&this->subCameras[i], &this->view, &this->colCtx, this);
        Camera_ChangeStatus(&this->subCameras[i], CAM_STATUS_INACTIVE);
    }

    this->cameraPtrs[CAM_ID_MAIN] = &this->mainCamera;
    this->cameraPtrs[CAM_ID_MAIN]->uid = CAM_ID_MAIN;
    this->activeCamId = CAM_ID_MAIN;

    Camera_OverwriteStateFlags(&this->mainCamera, CAM_STATE_0 | CAM_STATE_CHECK_WATER | CAM_STATE_2 | CAM_STATE_3 |
                                                      CAM_STATE_4 | CAM_STATE_DISABLE_MODE_CHANGE | CAM_STATE_6);
    if (samsungDiag) {
        recomp_measure_latency(84, (u32)this, (u32)&this->mainCamera, (u32)this->activeCamId,
                               (u32)this->cameraPtrs[CAM_ID_MAIN]);
    }

    if (samsungDiag) {
        recomp_measure_latency(85, (u32)this, (u32)&this->sramCtx, 0, 0);
    }
    Sram_Alloc(&this->state, &this->sramCtx);
    if (samsungDiag) {
        recomp_measure_latency(86, (u32)this, (u32)&this->sramCtx, 0, 0);
    }
    Regs_InitData(this);
    if (samsungDiag) {
        recomp_measure_latency(87, (u32)this, 0, 0, 0);
    }
    Message_Init(this);
    if (samsungDiag) {
        recomp_measure_latency(88, (u32)this, (u32)&this->msgCtx, 0, 0);
    }
    GameOver_Init(this);
    if (samsungDiag) {
        recomp_measure_latency(89, (u32)this, 0, 0, 0);
    }
    SoundSource_InitAll(this);
    if (samsungDiag) {
        recomp_measure_latency(90, (u32)this, 0, 0, 0);
    }
    EffFootmark_Init(this);
    if (samsungDiag) {
        recomp_measure_latency(91, (u32)this, 0, 0, 0);
    }
    Effect_Init(this);
    if (samsungDiag) {
        recomp_measure_latency(92, (u32)this, 0, 0, 0);
    }
    EffectSS_Init(this, 100);
    if (samsungDiag) {
        recomp_measure_latency(93, (u32)this, 100, 0, 0);
    }
    CollisionCheck_InitContext(this, &this->colChkCtx);
    if (samsungDiag) {
        recomp_measure_latency(94, (u32)this, (u32)&this->colChkCtx, 0, 0);
    }
    AnimationContext_Reset(&this->animationCtx);
    if (samsungDiag) {
        recomp_measure_latency(95, (u32)this, (u32)&this->animationCtx, 0, 0);
    }
    Cutscene_InitContext(this, &this->csCtx);

    if (samsungDiag) {
        recomp_measure_latency(96, (u32)this, (u32)&this->csCtx, (u32)&this->sramCtx, (u32)&this->msgCtx);
    }

    if (gSaveContext.nextCutsceneIndex != 0xFFEF) {
        gSaveContext.save.cutsceneIndex = gSaveContext.nextCutsceneIndex;
        gSaveContext.nextCutsceneIndex = 0xFFEF;
    }

    if (gSaveContext.save.cutsceneIndex == 0xFFFD) {
        gSaveContext.save.cutsceneIndex = 0;
    }

    if (gSaveContext.nextDayTime != NEXT_TIME_NONE) {
        gSaveContext.save.time = gSaveContext.nextDayTime;
        gSaveContext.skyboxTime = gSaveContext.nextDayTime;
    }

    if ((CURRENT_TIME >= CLOCK_TIME(18, 0)) || (CURRENT_TIME < CLOCK_TIME(6, 30))) {
        gSaveContext.save.isNight = true;
    } else {
        gSaveContext.save.isNight = false;
    }

    if (samsungDiag) {
        recomp_measure_latency(86, (u32)this, (u32)gSaveContext.save.time, (u32)gSaveContext.save.isNight,
                               (u32)gSaveContext.save.cutsceneIndex);
    }

    func_800EDDB0(this);

    if (((gSaveContext.gameMode != GAMEMODE_NORMAL) && (gSaveContext.gameMode != GAMEMODE_TITLE_SCREEN)) ||
        (gSaveContext.save.cutsceneIndex >= 0xFFF0)) {
        gSaveContext.nayrusLoveTimer = 0;
        Magic_Reset(this);
        gSaveContext.sceneLayer = (gSaveContext.save.cutsceneIndex & 0xF) + 1;

        // Set saved cutscene to 0 so it doesn't immediately play, but instead let the `CutsceneManager` handle it.
        gSaveContext.save.cutsceneIndex = 0;
    } else {
        gSaveContext.sceneLayer = 0;
    }

    sceneLayer = gSaveContext.sceneLayer;

    if (samsungDiag) {
        recomp_measure_latency(87, (u32)this, (u32)gSaveContext.save.entrance, (u32)gSaveContext.sceneLayer,
                               (u32)sceneLayer);
    }

    if (samsungDiag) {
        s32 sceneId;
        s32 spawnNum;

        recomp_android_get_entrance_scene_spawn(
            sSceneEntranceTable, ((void)0, gSaveContext.save.entrance) + ((void)0, gSaveContext.sceneLayer), &sceneId,
            &spawnNum);
        if ((((void)0, gSaveContext.save.entrance) + ((void)0, gSaveContext.sceneLayer) == 0) && (sceneId == 0) &&
            (spawnNum == 0)) {
            recomp_measure_latency(87, 2, 0, (u32)sceneId, SCENE_SONCHONOIE);
            sceneId = SCENE_SONCHONOIE;
            spawnNum = 0;
        }
        recomp_measure_latency(87, 1, (u32)(((void)0, gSaveContext.save.entrance) + ((void)0, gSaveContext.sceneLayer)),
                               (u32)sceneId, (u32)spawnNum);
        Play_SpawnScene(this, sceneId, spawnNum);
    } else {
        Play_SpawnScene(
            this,
            Entrance_GetSceneIdAbsolute(((void)0, gSaveContext.save.entrance) + ((void)0, gSaveContext.sceneLayer)),
            Entrance_GetSpawnNum(((void)0, gSaveContext.save.entrance) + ((void)0, gSaveContext.sceneLayer)));
    }

    if (samsungDiag) {
        recomp_measure_latency(88, (u32)this, (u32)this->sceneId, (u32)this->roomCtx.curRoom.num,
                               (u32)this->roomCtx.status);
    }

    KaleidoScopeCall_Init(this);
    Interface_Init(this);

    if (samsungDiag) {
        recomp_measure_latency(89, (u32)this, (u32)&this->pauseCtx, (u32)&this->interfaceCtx,
                               (u32)gSaveContext.nextDayTime);
    }

    if (gSaveContext.nextDayTime != NEXT_TIME_NONE) {
        if (gSaveContext.nextDayTime == NEXT_TIME_DAY) {
            gSaveContext.save.day++;
            gSaveContext.save.eventDayCount++;
            gSaveContext.dogIsLost = true;
            gSaveContext.nextDayTime = NEXT_TIME_DAY_SET;
        } else {
            gSaveContext.nextDayTime = NEXT_TIME_NIGHT_SET;
        }
    }

    Play_InitMotionBlur();

    R_PAUSE_BG_PRERENDER_STATE = PAUSE_BG_PRERENDER_OFF;
    R_PICTO_PHOTO_STATE = PICTO_PHOTO_STATE_OFF;

    PreRender_Init(&this->pauseBgPreRender);
    PreRender_SetValuesSave(&this->pauseBgPreRender, gCfbWidth, gCfbHeight, NULL, NULL, NULL);
    PreRender_SetValues(&this->pauseBgPreRender, gCfbWidth, gCfbHeight, NULL, NULL);

    if (samsungDiag) {
        recomp_measure_latency(90, (u32)this, (u32)gCfbWidth, (u32)gCfbHeight, (u32)&this->pauseBgPreRender);
    }

    this->unk_18E64 = gWorkBuffer;
    this->pictoPhotoI8 = gPictoPhotoI8;
    this->unk_18E68 = D_80784600;
    this->unk_18E58 = D_80784600;
    this->unk_18E60 = D_80784600;
    gTransitionTileState = TRANS_TILE_OFF;
    this->transitionMode = TRANS_MODE_OFF;
    D_801D0D54 = false;

    FrameAdvance_Init(&this->frameAdvCtx);
    Rand_Seed(osGetTime());
    Matrix_Init(&this->state);

    this->state.main = Play_Main;
    this->state.destroy = Play_Destroy;

    if (samsungDiag) {
        recomp_measure_latency(91, (u32)this, (u32)this->state.main, (u32)this->state.destroy,
                               (u32)this->transitionMode);
    }

    this->transitionTrigger = TRANS_TRIGGER_END;
    this->worldCoverAlpha = 0;
    this->bgCoverAlpha = 0;
    this->haltAllActors = false;
    this->unk_18844 = false;

    if (gSaveContext.gameMode != GAMEMODE_TITLE_SCREEN) {
        if (gSaveContext.nextTransitionType == TRANS_NEXT_TYPE_DEFAULT) {
            this->transitionType =
                (Entrance_GetTransitionFlags(((void)0, gSaveContext.save.entrance) + sceneLayer) >> 7) & 0x7F;
        } else {
            this->transitionType = gSaveContext.nextTransitionType;
            gSaveContext.nextTransitionType = TRANS_NEXT_TYPE_DEFAULT;
        }
    } else {
        this->transitionType = TRANS_TYPE_FADE_BLACK;
    }

    TransitionFade_Init(&this->unk_18E48);
    TransitionFade_SetType(&this->unk_18E48, 3);
    TransitionFade_SetColor(&this->unk_18E48, RGBA8(160, 160, 160, 255));
    TransitionFade_Start(&this->unk_18E48);
    VisMono_Init(&sPlayVisMono);

    gVisMonoColor.a = 0;
    sPlayVisFbufInstance = &sPlayVisFbuf;
    VisFbuf_Init(sPlayVisFbufInstance);
    sPlayVisFbufInstance->lodProportion = 0.0f;
    sPlayVisFbufInstance->mode = VIS_FBUF_MODE_GENERAL;
    sPlayVisFbufInstance->primColor.r = 0;
    sPlayVisFbufInstance->primColor.g = 0;
    sPlayVisFbufInstance->primColor.b = 0;
    sPlayVisFbufInstance->primColor.a = 0;
    sPlayVisFbufInstance->envColor.r = 0;
    sPlayVisFbufInstance->envColor.g = 0;
    sPlayVisFbufInstance->envColor.b = 0;
    sPlayVisFbufInstance->envColor.a = 0;
    CutsceneFlags_UnsetAll(this);

    if (samsungDiag) {
        recomp_measure_latency(92, (u32)this, (u32)sPlayVisFbufInstance, (u32)&this->unk_18E48,
                               (u32)&sPlayVisMono);
    }

    THA_GetRemaining(&this->state.tha);
    zAllocSize = THA_GetRemaining(&this->state.tha);
    if (samsungDiag) {
        recomp_measure_latency(93, (u32)this, (u32)zAllocSize, (u32)this->state.tha.head,
                               (u32)this->state.tha.tail);
        recomp_measure_latency(93, (u32)this->state.tha.start, (u32)this->state.tha.size,
                               (u32)(uintptr_t)this->sceneSegment, (u32)(uintptr_t)this->roomCtx.roomMemPages[0]);
    }
    if (samsungDiag) {
        uintptr_t zAllocStart = ALIGN16((uintptr_t)this->state.tha.head + 8);
        uintptr_t zAllocEnd = (uintptr_t)this->state.tha.start + this->state.tha.size;

        if (zAllocEnd <= zAllocStart) {
            recomp_crash("Samsung Play arena range is empty");
        }

        zAlloc = zAllocStart - 8;
        zAllocSize = zAllocEnd - zAlloc;
    } else if (zAllocSize <= 0) {
        recomp_crash("Play arena has no remaining THA memory");
    }

    if (samsungDiag) {
        recomp_measure_latency(93, (u32)this, (u32)zAllocSize, (u32)this->state.tha.start, (u32)this->state.tha.size);
    }

    if (!samsungDiag) {
        zAlloc = (uintptr_t)THA_AllocTailAlign16(&this->state.tha, zAllocSize);
        if (zAlloc == 0) {
            recomp_crash("Play arena allocation failed");
        }
    }

    {
        uintptr_t zAllocAligned = ALIGN16(zAlloc + 8);
        size_t zArenaSize = (zAlloc + zAllocSize > zAllocAligned) ? (zAlloc + zAllocSize - zAllocAligned) : 0;

        if (zArenaSize == 0) {
            recomp_crash("Play arena aligned size is zero");
        }

        ZeldaArena_Init((void*)zAllocAligned, zArenaSize);
    }

    if (samsungDiag) {
        recomp_measure_latency(94, (u32)this, (u32)zAlloc, (u32)zAllocSize, (u32)&this->actorCtx);
    }

    Actor_InitContext(this, &this->actorCtx, this->linkActorEntry);

    if (samsungDiag) {
        recomp_measure_latency(95, (u32)this, (u32)&this->actorCtx, (u32)this->linkActorEntry, (u32)&this->roomCtx);
    }

    while (!Room_HandleLoadCallbacks(this, &this->roomCtx)) {
        if (samsungDiag && ((roomLoadSpin++ & 0x3FF) == 0)) {
            recomp_measure_latency(96, roomLoadSpin, (u32)this->roomCtx.status, (u32)this->roomCtx.curRoom.num,
                                   (u32)this->roomCtx.activeRoomVram);
        }
    }

    if (samsungDiag) {
        recomp_measure_latency(97, (u32)this, (u32)this->roomCtx.status, (u32)this->roomCtx.curRoom.num,
                               (u32)this->roomCtx.curRoom.segment);
    }

    if ((CURRENT_DAY != 0) && ((this->roomCtx.curRoom.behaviorType1 == ROOM_BEHAVIOR_TYPE1_1) ||
                               (this->roomCtx.curRoom.behaviorType1 == ROOM_BEHAVIOR_TYPE1_5))) {
        Actor_Spawn(&this->actorCtx, this, ACTOR_EN_TEST4, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
    }

    player = GET_PLAYER(this);

    if (samsungDiag) {
        Actor* playerActor = this->actorCtx.actorLists[ACTORCAT_PLAYER].first;
        Actor* aliasedPlayerActor = AndroidDiag_IsKseg0Ptr(playerActor) ? playerActor : AndroidDiag_ToRdramAlias(playerActor);
        bool playerCameraReady = AndroidDiag_IsKseg0Ptr(aliasedPlayerActor) && (aliasedPlayerActor->id == ACTOR_PLAYER);

        recomp_measure_latency(98, 0x90, (u32)playerActor, (u32)aliasedPlayerActor,
                               (u32)this->actorCtx.actorLists[ACTORCAT_PLAYER].length);

        if (playerCameraReady) {
            player = (Player*)aliasedPlayerActor;
            recomp_measure_latency(98, 0x91, (u32)player, (u32)player->actor.id, (u32)player->actor.category);

            Camera_InitFocalActorSettings(&this->mainCamera, &player->actor);
            gDbgCamEnabled = false;

            if (PLAYER_GET_BG_CAM_INDEX(&player->actor) != 0xFF) {
                Camera_ChangeActorCsCamIndex(&this->mainCamera, PLAYER_GET_BG_CAM_INDEX(&player->actor));
            }
        } else {
            recomp_measure_latency(98, 0x92, (u32)playerActor, (u32)aliasedPlayerActor,
                                   AndroidDiag_IsKseg0Ptr(aliasedPlayerActor) ? (u32)aliasedPlayerActor->id : 0xFFFFFFFF);
            this->mainCamera.focalActor = NULL;
            gDbgCamEnabled = false;
        }
    } else {
        Camera_InitFocalActorSettings(&this->mainCamera, &player->actor);
        gDbgCamEnabled = false;

        if (PLAYER_GET_BG_CAM_INDEX(&player->actor) != 0xFF) {
            Camera_ChangeActorCsCamIndex(&this->mainCamera, PLAYER_GET_BG_CAM_INDEX(&player->actor));
        }
    }

    CutsceneManager_StoreCamera(&this->mainCamera);
    Interface_SetSceneRestrictions(this);
    Environment_PlaySceneSequence(this);

    if (samsungDiag) {
        recomp_measure_latency(98, (u32)this, (u32)player, (u32)this->sequenceCtx.seqId,
                               (u32)this->sequenceCtx.ambienceId);
    }

    gSaveContext.seqId = this->sequenceCtx.seqId;
    gSaveContext.ambienceId = this->sequenceCtx.ambienceId;
    AnimationContext_Update(this, &this->animationCtx);
    Cutscene_HandleEntranceTriggers(this);
    gSaveContext.respawnFlag = 0;
    sBombersNotebookOpen = false;
    BombersNotebook_Init(&sBombersNotebook);

    // @recomp_event recomp_after_play_init(PlayState* this): The new PlayState has finished initializing.
    recomp_after_play_init(this);

    if (samsungDiag) {
        recomp_measure_latency(99, (u32)this, (u32)this->state.main, (u32)this->state.destroy,
                               (u32)GameState_IsRunning(&this->state));
    }
}
