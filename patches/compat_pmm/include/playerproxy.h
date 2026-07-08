#ifndef PLAYERPROXY_H
#define PLAYERPROXY_H

#include "PR/ultratypes.h"
#include "PR/gbi.h"
#include "z64player.h"
#include "recompdata.h"
#include "modelmatrixids.h"
#include "playermodelmanager_api.h"
#include "stdbool.h"
#include "modelentry.h"
#include "yazmtcorelib_api.h"

typedef struct FormProxy FormProxy;

typedef enum PlayerProxyInterpolationStatus {
    PP_INTERP_STAT_NO_SKIP,
    PP_INTERP_STAT_SKIPPING,
    PP_INTERP_STAT_RESUME_NEXT_FRAME,
} PlayerProxyInterpolationStatus;

typedef struct PlayerProxy {
    U32MemoryHashmapHandle formProxies;
    U32ValueHashmapHandle modelEntries;
    YAZMTCore_IterableU32Set *currentlyEquippedEntries;
    PlayerProxyInterpolationStatus interpStatus;
} PlayerProxy;

void PlayerProxy_init(PlayerProxy *pp);
void PlayerProxy_destroy(PlayerProxy *pp);
FormProxy *PlayerProxy_getFormProxy(PlayerProxy *pp, FormProxyId formId);
void PlayerProxy_requestRefresh(PlayerProxy *pp);
void PlayerProxy_setOverrideDL(PlayerProxy *pp, Link_DisplayList dlId, Gfx *dl);
void PlayerProxy_setOverrideMtx(PlayerProxy *pp, Link_EquipmentMatrix mtxId, Mtx *mtx);
bool PlayerProxy_getProxyIdFromForm(PlayerTransformation form, FormProxyId *out);
void PlayerProxy_requestTunicColorOverride(PlayerProxy *pp, Color_RGBA8 color);
const ModelEntry *PlayerProxy_getCurrentEntry(PlayerProxy *pp, PlayerModelManagerModelType modelType);
bool PlayerProxy_tryApplyEntry(PlayerProxy *pp, PlayerModelManagerModelType modelType, const ModelEntry *newEntry);
bool PlayerProxy_forceApplyEntry(PlayerProxy *pp, PlayerModelManagerModelType modelType, const ModelEntry *newEntry);
void PlayerProxy_removeEntry(PlayerProxy *pp, PlayerModelManagerModelType modelType);
bool PlayerProxy_isModelEntryApplied(PlayerProxy *pp, const ModelEntry *entry);
bool PlayerProxy_shouldSkipInterpolation(const PlayerProxy *pp);
void PlayerProxy_updateInterpolationStatus(PlayerProxy *pp);
void PlayerProxy_requestInterpolationSkip(PlayerProxy *pp);

extern PlayerProxy *gPlayer1Proxy;
extern PlayerProxy *gPlayer2Proxy;

#endif
