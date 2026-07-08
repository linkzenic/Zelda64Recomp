#ifndef UTILS_H
#define UTILS_H

#include "PR/ultratypes.h"
#include "PR/gbi.h"
#include "z64animation.h"
#include "stdbool.h"
#include "globalobjects_api.h"
#include "yazmtcorelib_api.h"
#include "playermodelmanager_api.h"
#include "modelmatrixids.h"

Gfx *Utils_createShimWithMatrix(Mtx *matrix, Gfx *dls[], int n);
Gfx *Utils_createShimDisplayList(Gfx *dls[], int n);

#define GFX_PTR_ARR(...) ((Gfx *[]){__VA_ARGS__})
#define CREATE_SHIM_DL_ARGS(...) GFX_PTR_ARR(__VA_ARGS__), ARRAY_COUNT(GFX_PTR_ARR(__VA_ARGS__))
#define Utils_createShimWithMatrixVa(matrix, ...) Utils_createShimWithMatrix(matrix, CREATE_SHIM_DL_ARGS(__VA_ARGS__))
#define Utils_createShimDisplayListVa(...) Utils_createShimDisplayList(CREATE_SHIM_DL_ARGS(__VA_ARGS__))

u32 Utils_readU32(const u8 array[], u32 offset);
void Utils_writeU32(u8 array[], u32 offset, u32 value);
bool Utils_isSegmentedPtr(void *p);

void Utils_setCrashEnabled(bool isEnabled);
void Utils_tryCrashGame();

void *Utils_recompCalloc(size_t size);

void Utils_processIterableSetPtrs(YAZMTCore_IterableU32Set *set, void processorFunc(void *item));

bool Utils_isEquipmentModelType(PlayerModelManagerModelType modelType);
bool Utils_isFormModelType(PlayerModelManagerModelType modelType);
bool Utils_isPackModelType(PlayerModelManagerModelType modelType);
bool Utils_isValidModelType(PlayerModelManagerModelType modelType);

bool Utils_isValidDisplayListId(Link_DisplayList dlId);
bool Utils_isValidMatrixId(Link_EquipmentMatrix mtxId);

#endif
