#include "global.h"
#include "recompdata.h"
#include "recomputils.h"
#include "repoint.h"
#include "objectmanager.h"
#include "helpers.h"
#include "misc_funcs.h"

// Maps VROM addresses to their respective object IDs
U32ValueHashmapHandle gVromToObjId = 0;

// maps object ids to their location in memory
void *gObjIdToMemTable[OBJECT_ID_MAX] = {0};

// avoid going repointing each dl more than once
U32HashsetHandle gRepointTracker[OBJECT_ID_MAX];

bool isObjectManagerReady(const char *funcName) {
    if (!gVromToObjId) {
        recomp_printf("GlobalObjects: WARNING! FUNCTION %s CALLED BUT OBJECT MANAGER IS NOT INITIALIZED\n", funcName);
        return false;
    }

    return true;
}

void initObjectManager() {
    gVromToObjId = recomputil_create_u32_value_hashmap();

    for (size_t i = 0; i < OBJECT_ID_MAX; ++i) {
        if (gObjectTable[i].vromStart) {
                recomputil_u32_value_hashmap_insert(gVromToObjId, gObjectTable[i].vromStart, i);
        }
    }
}

void *loadObjectFromVrom(uintptr_t vromAddr, size_t size) {
    void *obj = recomp_alloc(size);

    if (recomp_android_should_use_sync_boot_dma()) {
        DmaEntry* entry = DmaMgr_FindDmaEntry(vromAddr);

        if (entry != NULL) {
            if (entry->romEnd == 0) {
                recomp_load_overlays((entry->romStart + vromAddr) - entry->vromStart, obj, size);
                return obj;
            }

            if ((vromAddr == entry->vromStart) && (size == (entry->vromEnd - entry->vromStart)) &&
                (recomp_android_load_yaz0(entry->romStart, entry->romEnd - entry->romStart, obj, size) == 0)) {
                return obj;
            }
        }

        recomp_printf("[GlobalObjects] Samsung direct object load fallback vrom=%08llX size=%08X entry=%08llX\n",
                      (u64)vromAddr, (u32)size, (u64)(uintptr_t)entry);
        recomp_load_overlays(vromAddr, obj, size);
        return obj;
    }

    DmaMgr_SendRequest0(obj, vromAddr, size);

    return obj;
}

RECOMP_EXPORT void *GlobalObjects_getGlobalObject(ObjectId id) {
    if (!isObjectManagerReady("GlobalObjects_getGlobalObject")) {
        return NULL;
    }

    if (id > OBJECT_ID_MAX) {
        return NULL;
    }

    if (!gObjIdToMemTable[id]) {
        uintptr_t vromStart = gObjectTable[id].vromStart;

        size_t size = gObjectTable[id].vromEnd - vromStart;

        gObjIdToMemTable[id] = loadObjectFromVrom(vromStart, size);

        gRepointTracker[id] = recomputil_create_u32_hashset();
    }

    return gObjIdToMemTable[id];
}

RECOMP_EXPORT bool GlobalObjects_getObjectIdFromVrom(uintptr_t vromStart, ObjectId *out) {
    if (!isObjectManagerReady("GlobalObjects_getObjectIdFromVrom")) {
        return false;
    }

    unsigned long objectId = 0;
    if (!recomputil_u32_value_hashmap_get(gVromToObjId, vromStart, &objectId)) {
        return false;
    }

    *out = (ObjectId)objectId;
    return true;
}

RECOMP_EXPORT void *GlobalObjects_getGlobalObjectFromVrom(uintptr_t vromStart) {
    if (!isObjectManagerReady("GlobalObjects_getGlobalObjectFromVrom")) {
        return NULL;
    }

    ObjectId id;

    if (GlobalObjects_getObjectIdFromVrom(vromStart, &id)) {
        return GlobalObjects_getGlobalObject(id);
    }

    return NULL;
}

bool hasFieldKeepDependency(ObjectId id) {
    return id == OBJECT_HANA || id == OBJECT_WOOD02;
}

bool hasDangeonKeepDependency(ObjectId id) {
    return id == OBJECT_BDOOR || id == OBJECT_SYOKUDAI;
}

static SegmentMap sGlobalGfxSegmentMap = {0};

RECOMP_EXPORT Gfx *GlobalObjects_getGlobalGfxPtr(ObjectId id, Gfx *segmentedPtr) {
    if (!isObjectManagerReady("GlobalObjects_getGlobalGfxPtr")) {
        return NULL;
    }

    if (!isSegmentedPtr(segmentedPtr)) {
        return NULL;
    }

    void *obj = GlobalObjects_getGlobalObject(id);

    if (!obj) {
        return NULL;
    }

    sGlobalGfxSegmentMap[0x04] = GlobalObjects_getGlobalObject(GAMEPLAY_KEEP);
    sGlobalGfxSegmentMap[0x05] = NULL;

    if (recomputil_u32_hashset_insert(gRepointTracker[id], (uintptr_t)segmentedPtr)) {
        // workaround for gameplay_dangeon_keep and gameplay_field_keep sharing a segment
        ObjectId fieldOrDungeonKeep = 0;
        if (hasFieldKeepDependency(id)) {
            fieldOrDungeonKeep = GAMEPLAY_FIELD_KEEP;
        } else if (hasDangeonKeepDependency(id)) {
            fieldOrDungeonKeep = GAMEPLAY_DANGEON_KEEP;
        }

        if (fieldOrDungeonKeep) {
            sGlobalGfxSegmentMap[0x05] = GlobalObjects_getGlobalObject(fieldOrDungeonKeep);
        }

        unsigned segment = (uintptr_t)segmentedPtr >> 24;

        sGlobalGfxSegmentMap[segment] = obj;

        GlobalObjects_rebaseDL(TO_GLOBAL_PTR(obj, segmentedPtr), sGlobalGfxSegmentMap);
        
        sGlobalGfxSegmentMap[segment] = NULL;
    }

    return TO_GLOBAL_PTR(obj, segmentedPtr);
}

// Can't start loading objects in until the dma manager is initialized
RECOMP_DECLARE_EVENT(onReady());

RECOMP_HOOK_RETURN("Main_Init")
void initializeObjectManagerOnce() {
    initObjectManager();
    onReady();
}
