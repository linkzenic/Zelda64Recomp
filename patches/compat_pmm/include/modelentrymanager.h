#ifndef CUSTOMMODELENTRYMANAGER_H
#define CUSTOMMODELENTRYMANAGER_H

typedef struct ModelEntry ModelEntry;

#include "playermodelmanager_api.h"
#include "modelmatrixids.h"

typedef unsigned long PlayerModelManagerHandle;
typedef struct PlayerProxy PlayerProxy;

void ModelEntryManager_setCurrentEntry(PlayerModelManagerModelType modelType, const ModelEntry *entry);
const ModelEntry **ModelEntryManager_getCategoryEntryData(PlayerModelManagerModelType modelType, size_t *count);
PlayerModelManagerHandle ModelEntryManager_createMemoryHandle(PlayerModelManagerModelType type, const char *internalName);
ModelEntry *ModelEntryManager_getEntry(PlayerModelManagerHandle h);
bool ModelEntryManager_isEntryHidden(const ModelEntry *ModelEntry);
void ModelEntryManager_setEntryHidden(const ModelEntry *modelEntry, bool isHidden);
bool ModelEntryManager_saveModelsToDisk(void);
void ModelEntryManager_registerProxyToSave(PlayerProxy *pp, const char *id);
void ModelEntryManager_applySavedEntriesToProxy(PlayerProxy *pp, const char *id);
bool ModelEntryManager_applyByInternalName(PlayerProxy *pp, PlayerModelManagerModelType modelType, const char *name);

#endif
