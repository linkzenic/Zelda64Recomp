#ifndef __RECOMP_FUNCS_H__
#define __RECOMP_FUNCS_H__

#include "patch_helpers.h"

DECLARE_FUNC(void, recomp_load_overlays, u32 rom, void* ram, u32 size);
DECLARE_FUNC(void, recomp_puts, const char* data, u32 size);
DECLARE_FUNC(void, recomp_measure_latency, s32 stage, u32 a, u32 b, u32 c, u32 d);
DECLARE_FUNC(s32, recomp_android_load_yaz0, u32 rom, u32 compressedSize, void* dst, u32 expectedSize);
DECLARE_FUNC(s32, recomp_android_should_use_sync_boot_dma);
DECLARE_FUNC(s32, recomp_android_is_n64_mode);
DECLARE_FUNC(void, recomp_android_reset_effect_ss_table, void* table, u32 count);
DECLARE_FUNC(void, recomp_android_get_entrance_scene_spawn, void* sceneEntranceTable, u32 entrance, s32* sceneIdOut,
             s32* spawnNumOut);
DECLARE_FUNC(void, recomp_exit);
DECLARE_FUNC(void, recomp_handle_quicksave_actions, OSMesgQueue* enter_mq, OSMesgQueue* exit_mq);
DECLARE_FUNC(void, recomp_handle_quicksave_actions_main, OSMesgQueue* enter_mq, OSMesgQueue* exit_mq);
DECLARE_FUNC(u16, recomp_get_pending_warp);
DECLARE_FUNC(u32, recomp_get_pending_set_time);
DECLARE_FUNC(s32, recomp_get_autosave_enabled);
DECLARE_FUNC(s32, recomp_get_save_anywhere_enabled);
DECLARE_FUNC(s32, recomp_android_compat_pmm_model_count);
DECLARE_FUNC(s32, recomp_android_compat_pmm_cycle_model, s32 delta);
DECLARE_FUNC(s32, recomp_android_compat_pmm_load_selected_model, void* dst, u32 maxSize);
DECLARE_FUNC(void, recomp_android_compat_pmm_apply_log, s32 eventType, s32 category, s32 entryType, s32 result,
             const char* entryName);
DECLARE_FUNC(s32, recomp_android_compat_pmm_begin_save);
DECLARE_FUNC(s32, recomp_android_compat_pmm_write_selection, const char* sectionName, const char* key,
             const char* internalName);
DECLARE_FUNC(s32, recomp_android_compat_pmm_load_selection, const char* sectionName, const char* key, void* dst,
             u32 maxSize);
DECLARE_FUNC(u32, recomp_android_compat_pmm_get_config_u32, const char* key);
DECLARE_FUNC(char*, recomp_android_compat_pmm_get_config_string, const char* key);
DECLARE_FUNC(void, recomp_android_compat_pmm_free_config_string, char* str);
DECLARE_FUNC(u32, recomp_android_compat_cheats_get_config_u32, const char* key);

#endif
