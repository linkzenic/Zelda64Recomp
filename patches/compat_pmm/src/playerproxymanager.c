#include "global.h"
#include "playerproxymanager.h"
#include "playerproxy.h"
#include "modelentry.h"
#include "modelinfo.h"
#include "formproxy.h"
#include "playermodelconfig.h"
#include "yazmtcorelib_api.h"
#include "logger.h"
#include "utils.h"
#include "recompdata.h"

PlayerProxyHandle gPlayer1ProxyHandle;
PlayerProxyHandle gPlayer2ProxyHandle;

typedef struct PlayerProxyEntry PlayerProxyEntry;

typedef struct PlayerProxyEntry {
    PlayerProxy *pp;
    PlayerProxyAllocType allocType;
    s32 refCount;
    PlayerProxyEntry *next;
    PlayerProxyEntry *prev;
} PlayerProxyEntry;

static PlayerProxyEntry *sPlayerProxyEntryListStart;

static U32SlotmapHandle sPlayerProxyEntryReferences;

RECOMP_CALLBACK(".", _internal_initHashObjects) void initPPMHash(void) {
    sPlayerProxyEntryReferences = recomputil_create_u32_slotmap();
}

static PlayerProxyHandle createReferenceFromEntry(PlayerProxyEntry *proxyEntry) {
    PlayerProxyHandle h = recomputil_u32_slotmap_create(sPlayerProxyEntryReferences);
    recomputil_u32_slotmap_set(sPlayerProxyEntryReferences, h, (uintptr_t)proxyEntry);
    proxyEntry->refCount++;
    return h;
}

PlayerProxyHandle PlayerProxyManager_createPlayerProxy(PlayerProxyAllocType allocType) {
    PlayerProxyEntry *proxyEntry = Utils_recompCalloc(sizeof(*proxyEntry));
    proxyEntry->allocType = allocType;
    proxyEntry->refCount = 0;
    proxyEntry->next = NULL;
    proxyEntry->prev = NULL;

    PlayerProxy *pp = proxyEntry->pp = Utils_recompCalloc(sizeof(*pp));

    PlayerProxy_init(pp);

    proxyEntry->next = sPlayerProxyEntryListStart;

    if (proxyEntry->next) {
        proxyEntry->next->prev = proxyEntry;
    }

    sPlayerProxyEntryListStart = proxyEntry;

    return createReferenceFromEntry(proxyEntry);
}

static PlayerProxyEntry *getProxyEntryFromRef(PlayerProxyHandle h) {
    uintptr_t out = 0;
    if (h) {
        recomputil_u32_slotmap_get(sPlayerProxyEntryReferences, h, &out);
    }
    return (PlayerProxyEntry *)out;
}

PlayerProxy *PlayerProxyManager_getPlayerProxy(PlayerProxyHandle h) {
    PlayerProxyEntry *proxyEntry = getProxyEntryFromRef(h);

    return proxyEntry ? proxyEntry->pp : NULL;
}

PlayerProxyHandle PlayerProxyManager_createNewReference(PlayerProxyHandle h) {
    PlayerProxyHandle newHandle = 0;

    PlayerProxyEntry *proxyEntry = getProxyEntryFromRef(h);

    if (proxyEntry) {
        newHandle = createReferenceFromEntry(proxyEntry);
    } else {
        Logger_printWarning("Tried to duplicate invalid reference 0x%lX", h);
    }

    return newHandle;
}

bool PlayerProxyManager_releaseReference(PlayerProxyHandle h) {
    PlayerProxyEntry *proxyEntry = getProxyEntryFromRef(h);

    if (proxyEntry) {
        recomputil_u32_slotmap_erase(sPlayerProxyEntryReferences, h);
        proxyEntry->refCount--;
        return true;
    } else {
        Logger_printWarning("Tried to release invalid reference 0x%lX", h);
    }

    return false;
}

static bool isModelEntryInModelInfo(ModelInfo *mi, ModelEntry *entry) {
    if (!mi || !entry) {
        return false;
    }

    return (void *)ModelInfo_getModelEntryForm(mi) == entry;
}

void PlayerProxyManager_refreshAll(void) {
    PlayerProxyEntry *curr = sPlayerProxyEntryListStart;

    while (curr) {
        if (curr->pp) {
            PlayerProxy_requestRefresh(curr->pp);
        }

        curr = curr->next;
    }
}

void PlayerProxyManager_refreshFullAllWithModelEntry(ModelEntry *modelEntry) {
    PlayerProxyEntry *curr = sPlayerProxyEntryListStart;

    while (curr) {
        for (size_t i = 0; i < gFormProxyIds->size; ++i) {
            FormProxy *currFp = PlayerProxy_getFormProxy(curr->pp, gFormProxyIds->ids[i]);

            if (currFp) {
                if (isModelEntryInModelInfo(FormProxy_getCurrentModelInfo(currFp), modelEntry) ||
                    isModelEntryInModelInfo(FormProxy_getFallbackModelInfo(currFp), modelEntry) ||
                    isModelEntryInModelInfo(FormProxy_getFallbackOverrideModelInfo(currFp), modelEntry)) {
                    PlayerProxy_requestRefresh(curr->pp);
                }
            }
        }

        curr = curr->next;
    }
}

void updatePlayerProxyManager_on_UpdateMain(void) {
    PlayerProxyEntry *curr = sPlayerProxyEntryListStart;

    while (curr) {
        PlayerProxyEntry *const next = curr->next;

        PlayerProxy_updateInterpolationStatus(curr->pp);

        if (curr->allocType == PPALLOC_REF_COUNT && curr->refCount <= 0) {
            PlayerProxy_destroy(curr->pp);
            if (curr == sPlayerProxyEntryListStart) {
                sPlayerProxyEntryListStart = curr->next;
            } else {
                if (curr->prev) {
                    curr->prev->next = curr->next;
                }
            }

            if (curr->next) {
                curr->next->prev = curr->prev;
            }

            recomp_free(curr);
        }

        curr = next;
    }
}
