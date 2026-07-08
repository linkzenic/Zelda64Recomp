#ifndef MODELINFO_H
#define MODELINFO_H

#include "modelmatrixids.h"
#include "recompdata.h"

typedef struct ModelEntryForm ModelEntryForm;

typedef struct ModelInfo {
    ModelEntryForm *modelEntryForm;
    U32ValueHashmapHandle gfxOverrides;
    U32ValueHashmapHandle mtxOverrides;
} ModelInfo;

void ModelInfo_init(ModelInfo *modelInfo);
void ModelInfo_destroy(ModelInfo *modelInfo);
bool ModelInfo_hasModelEntry(ModelInfo *modelInfo);
Gfx *ModelInfo_getDL(ModelInfo *modelInfo, Link_DisplayList id);
Mtx *ModelInfo_getMtx(ModelInfo *modelInfo, Link_EquipmentMatrix id);
void ModelInfo_setModelEntryForm(ModelInfo *modelInfo, ModelEntryForm *modelEntry);
ModelEntryForm *ModelInfo_getModelEntryForm(ModelInfo *modelInfo);
TexturePtr ModelInfo_getEyesTexture(ModelInfo *modelInfo, PlayerEyeIndex i);
TexturePtr ModelInfo_getMouthTexture(ModelInfo *modelInfo, PlayerMouthIndex i);
FlexSkeletonHeader *ModelInfo_getSkeleton(ModelInfo *modelInfo);
FlexSkeletonHeader *ModelInfo_getShieldingSkeleton(ModelInfo *modelInfo);
bool ModelInfo_setGfxOverride(ModelInfo *modelInfo, Link_DisplayList id, Gfx *dl);
bool ModelInfo_unsetGfxOverride(ModelInfo *modelInfo, Link_DisplayList id);
bool ModelInfo_setMtxOverride(ModelInfo *modelInfo, Link_EquipmentMatrix id, Mtx *mtx);
bool ModelInfo_unsetMtxOverride(ModelInfo *modelInfo, Link_EquipmentMatrix id);
void ModelInfo_clearAllGfxOverrides(ModelInfo *modelInfo);
void ModelInfo_clearAllMtxOverrides(ModelInfo *modelInfo);
bool ModelInfo_isAnyFlagEnabled(ModelInfo *modelInfo, u64 flags);

extern ModelInfo gHumanModelInfo;
extern ModelInfo gDekuModelInfo;
extern ModelInfo gGoronModelInfo;
extern ModelInfo gZoraModelInfo;
extern ModelInfo gFierceDeityModelInfo;

#endif
