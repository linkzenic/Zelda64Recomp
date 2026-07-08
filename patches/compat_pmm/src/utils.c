#include "modding.h"
#include "global.h"
#include "recomputils.h"
#include "recompconfig.h"
#include "modelmatrixids.h"
#include "utils.h"
#include "logger.h"
#include "globalobjects_api.h"
#include "string.h"
#include "yazmtcorelib_api.h"
#include "playermodelmanager_api.h"

Gfx *Utils_createShimDisplayList(Gfx *dls[], int n) {
    if (n < 1) {
        return NULL;
    }

    Gfx *shimDL = recomp_alloc(n * sizeof(*shimDL));

    for (int i = 0; i < n - 1; ++i) {
        gSPDisplayList(&shimDL[i], dls[i]);
    }

    gSPBranchList(&shimDL[n - 1], dls[n - 1]);

    return shimDL;
}

Gfx *Utils_createShimWithMatrix(Mtx *matrix, Gfx *dls[], int n) {
    if (n < 1) {
        return NULL;
    }

    int shimSize = n + 3;

    Gfx *shimDL = recomp_alloc((shimSize) * sizeof(*shimDL));

    gSPMatrix(shimDL, matrix, G_MTX_PUSH | G_MTX_MUL | G_MTX_MODELVIEW);

    Gfx *calledLists = shimDL + 1;
    for (int i = 0; i < n; ++i) {
        gSPDisplayList(&calledLists[i], dls[i]);
    }

    gSPPopMatrix(&shimDL[shimSize - 2], G_MTX_MODELVIEW);

    gSPEndDisplayList(&shimDL[shimSize - 1]);

    return shimDL;
}

u32 Utils_readU32(const u8 array[], u32 offset) {
    return (u32)(array[offset + 0]) << 24 |
           (u32)(array[offset + 1]) << 16 |
           (u32)(array[offset + 2]) << 8 |
           (u32)(array[offset + 3]);
}

void Utils_writeU32(u8 array[], u32 offset, u32 value) {
    array[offset + 0] = (value & 0xFF000000) >> 24;
    array[offset + 1] = (value & 0x00FF0000) >> 16;
    array[offset + 2] = (value & 0x0000FF00) >> 8;
    array[offset + 3] = (value & 0x000000FF);
}

bool Utils_isSegmentedPtr(void *p) {
    return p < (void *)K0BASE;
}

bool sIsCrashDisabled;

void Utils_setCrashEnabled(bool isEnabled) {
    sIsCrashDisabled = isEnabled;
}

void Utils_tryCrashGame(void) {
    if (!sIsCrashDisabled) {
        Logger_printError("Forcing game crash...");
        int *ptr = NULL;
        *ptr = 0xDEADBEEF;
    }
}

void *Utils_recompCalloc(size_t size) {
    void *alloc = recomp_alloc(size);

    if (alloc) {
        memset(alloc, 0, size);
    }

    return alloc;
}

void Utils_processIterableSetPtrs(YAZMTCore_IterableU32Set *set, void processorFunc(void *item)) {
    size_t numItems = YAZMTCore_IterableU32Set_size(set);
    void **items = (void **)YAZMTCore_IterableU32Set_values(set);

    for (size_t i = 0; i < numItems; ++i) {
        processorFunc(items[i]);
    }
}

bool Utils_isEquipmentModelType(PlayerModelManagerModelType modelType) {
    return (modelType > PMM_MODEL_TYPE_FIERCE_DEITY && modelType < PMM_MODEL_TYPE_MODEL_PACK) || modelType == PMM_MODEL_TYPE_BOMB || modelType == PMM_MODEL_TYPE_BOMBCHU;
}

bool Utils_isFormModelType(PlayerModelManagerModelType modelType) {
    return modelType <= PMM_MODEL_TYPE_FIERCE_DEITY && modelType >= PMM_MODEL_TYPE_CHILD;
}

bool Utils_isPackModelType(PlayerModelManagerModelType modelType) {
    return modelType == PMM_MODEL_TYPE_MODEL_PACK;
}

bool Utils_isValidModelType(PlayerModelManagerModelType modelType) {
    return modelType >= 0 && modelType < PMM_MODEL_TYPE_MAX;
}

bool Utils_isValidDisplayListId(Link_DisplayList dlId) {
    return dlId >= LINK_DL_WAIST && dlId < LINK_DL_MAX;
}

bool Utils_isValidMatrixId(Link_EquipmentMatrix mtxId) {
    return mtxId >= LINK_EQUIP_MATRIX_SWORD1_BACK && mtxId < LINK_EQUIP_MATRIX_MAX;
}
