#ifndef FORMPROXY_H
#define FORMPROXY_H
#include "global.h"
#include "modelmatrixids.h"
#include "modelinfo.h"
#include "z64recomp_api.h"
#include "playermodelmanager_api.h"

typedef struct PlayerProxy PlayerProxy;

#define PLAYER_BODY_SHIELD_LIMB_COUNT 4
#define PLAYER_LIMB_COUNT (PLAYER_LIMB_MAX - 1)

typedef struct ShieldingSkeletonProxy {
    FlexSkeletonHeader flexSkeleton;
    StandardLimb *limbPtrs[PLAYER_BODY_SHIELD_LIMB_COUNT];
    StandardLimb limbs[PLAYER_BODY_SHIELD_LIMB_COUNT];
} ShieldingSkeletonProxy;

typedef struct SkeletonProxy {
    FlexSkeletonHeader flexSkeleton;
    LodLimb *limbPtrs[PLAYER_LIMB_COUNT];
    LodLimb limbs[PLAYER_LIMB_COUNT];
} SkeletonProxy;

typedef struct WrappedDisplayList {
    Gfx displayList[3];
} WrappedDisplayList;

#define WRAPPED_DL_PREDRAW 0
#define WRAPPED_DL_DRAW 1
#define WRAPPED_DL_POSTDRAW 2

typedef struct TunicColor {
    Color_RGBA8 current;
    Color_RGBA8 requested;
    bool isOverrideRequested;
} TunicColor;

typedef struct FormProxy {
    PlayerTransformation form;
    PlayerProxy *playerProxy;
    FormProxyId fpId;
    ModelInfo currentModelInfo;
    ModelInfo *fallbackOverrideModelInfo;
    ModelInfo *fallbackModelInfo;
    SkeletonProxy skeleton;
    ShieldingSkeletonProxy shieldingSkeleton;
    TunicColor tunicColor;
    Gfx *shimDisplayListPtrs[LINK_SHIMDL_MAX];
    Gfx *mtxDisplayLists[LINK_EQUIP_MATRIX_MAX];
    void *shimArrAlloc;
    void *mtxArrAlloc;
    size_t numDLs;
    Gfx *displayLists;
    WrappedDisplayList *wrappedDisplayLists;
} FormProxy;

void FormProxy_init(FormProxy *fp, PlayerProxy *pp, PlayerTransformation form, FormProxyId fpId, ModelInfo *fallback, ModelInfo *fallbackOverride);
void FormProxy_destroy(FormProxy *fp);
FormProxyId FormProxy_getFormProxyId(const FormProxy *fp);
ModelInfo *FormProxy_getCurrentModelInfo(FormProxy *fp);
ModelInfo *FormProxy_getFallbackOverrideModelInfo(FormProxy *fp);
ModelInfo *FormProxy_getFallbackModelInfo(FormProxy *fp);
bool FormProxy_setCurrentOverrideDL(FormProxy *fp, Link_DisplayList dlId, Gfx *dl);
bool FormProxy_unsetCurrentOverrideDL(FormProxy *fp, Link_DisplayList dlId);
bool FormProxy_setCurrentOverrideMtx(FormProxy *fp, Link_EquipmentMatrix mtxId, Mtx *mtx);
bool FormProxy_unsetCurrentOverrideMtx(FormProxy *fp, Link_EquipmentMatrix mtxId);
void FormProxy_requestTunicColorOverride(FormProxy *fp, Color_RGBA8 color);
bool FormProxy_isTunicColorOverrideRequested(FormProxy *fp);
Color_RGBA8 FormProxy_getRequestedTunicColor(FormProxy *fp);
Color_RGBA8 FormProxy_getCurrentTunicColor(FormProxy *fp);
void FormProxy_resetTunicColor(FormProxy *fp);
void FormProxy_pullCurrentTunicColorFromConfig(FormProxy *fp);
void FormProxy_pullCurrentTunicColorFromRequested(FormProxy *fp);
void FormProxy_clearCurrentModelState(FormProxy *fp);
void FormProxy_setCurrentModelFormEntry(FormProxy *fp, ModelEntryForm *modelEntry);
void FormProxy_repointPlayerFaceTexturePtrs(FormProxy *fp, TexturePtr eyesTextures[], TexturePtr mouthTextures[]);
Mtx *FormProxy_getMatrix(FormProxy *fp, Link_EquipmentMatrix id);
Gfx *FormProxy_getNextRefreshDL(FormProxy *fp, Link_DisplayList id);
Gfx *FormProxy_getDL(FormProxy *fp, Link_DisplayList id);
FlexSkeletonHeader *FormProxy_getSkeleton(FormProxy *fp);
FlexSkeletonHeader *FormProxy_getShieldingSkeleton(FormProxy *fp);
bool FormProxy_isAdultModelType(FormProxy *fp);
PlayerProxy *FormProxy_getPlayerProxy(FormProxy *fp);
PlayerTransformation FormProxy_getTargetForm(FormProxy *fp);
PlayerModelManagerModelType FormProxy_getModelType(FormProxy *fp);
ModelEntryForm *FormProxy_getModelEntryForm(FormProxy *fp);

#endif
