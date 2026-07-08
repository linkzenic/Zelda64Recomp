#ifndef PLAYERPROXYMANAGER_H
#define PLAYERPROXYMANAGER_H

#include "modelmatrixids.h"
#include "PR/ultratypes.h"
#include "stdbool.h"

typedef struct PlayerProxy PlayerProxy;
typedef struct ModelEntry ModelEntry;
typedef unsigned long PlayerProxyHandle;

extern PlayerProxyHandle gPlayer1ProxyHandle;
extern PlayerProxyHandle gPlayer2ProxyHandle;

typedef enum PlayerProxyAllocType {
    PPALLOC_UNSET,
    PPALLOC_REF_COUNT,
    PPALLOC_PERMANENT,
} PlayerProxyAllocType;

PlayerProxyHandle PlayerProxyManager_createPlayerProxy(PlayerProxyAllocType allocType);
void PlayerProxyManager_refreshAll();
void PlayerProxyManager_refreshFullAllWithModelEntry(ModelEntry *modelEntry);
PlayerProxy *PlayerProxyManager_getPlayerProxy(PlayerProxyHandle h);
PlayerProxyHandle PlayerProxyManager_createNewReference(PlayerProxyHandle h);
bool PlayerProxyManager_releaseReference(PlayerProxyHandle h);

#endif
