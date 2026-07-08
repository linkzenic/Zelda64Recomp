#include "global.h"
#include "modding.h"
#include "z64recomp_api.h"
#include "playerproxy.h"
#include "formproxy.h"
#include "proxyactorext.h"
#include "playermodelconfig.h"
#include "logger.h"
#include "playerproxymanager.h"
#include "../../misc_funcs.h"

typedef struct PlayerProxyInfo {
    PlayerProxyHandle proxyHandle;
    FormProxyId formId;
} PlayerProxyInfo;

static ActorExtensionId sActorExtIdPlayerProxyInfo;

static PlayerProxyInfo *getPlayerProxyInfo(Actor *actor) {
    if (!actor) {
        return NULL;
    }

    return z64recomp_get_extended_actor_data(actor, sActorExtIdPlayerProxyInfo);
}

PlayerProxy *ProxyActorExt_getPlayerProxy(Actor *actor) {
    PlayerProxyInfo *proxyInfo = getPlayerProxyInfo(actor);

    if (proxyInfo) {
        return PlayerProxyManager_getPlayerProxy(proxyInfo->proxyHandle);
    }

    return NULL;
}

bool ProxyActorExt_getFormProxyId(Actor *actor, FormProxyId *out) {
    PlayerProxyInfo *proxyInfo = getPlayerProxyInfo(actor);

    if (proxyInfo) {
        *out = proxyInfo->formId;
        return true;
    }

    return false;
}

FormProxy *ProxyActorExt_getFormProxy(Actor *actor) {
    PlayerProxy *actorPp = ProxyActorExt_getPlayerProxy(actor);

    if (actorPp) {
        FormProxyId fpId;

        if (ProxyActorExt_getFormProxyId(actor, &fpId)) {
            return PlayerProxy_getFormProxy(actorPp, fpId);
        }
    }

    return NULL;
}

bool ProxyActorExt_setFormProxyId(Actor *actor, FormProxyId id) {
    PlayerProxyInfo *proxyInfo = getPlayerProxyInfo(actor);

    if (proxyInfo) {
        proxyInfo->formId = id;
        return true;
    }

    return false;
}

bool ProxyActorExt_setPlayerProxyHandle(Actor *actor, PlayerProxyHandle h) {
    if (h) {
        PlayerProxyInfo *proxyInfo = getPlayerProxyInfo(actor);

        if (proxyInfo) {
            PlayerProxyHandle newRef = PlayerProxyManager_createNewReference(h);

            if (newRef) {
                if (proxyInfo->proxyHandle) {
                    PlayerProxyManager_releaseReference(proxyInfo->proxyHandle);
                }

                proxyInfo->proxyHandle = newRef;

                return true;
            }
        }
    }

    return false;
}

bool ProxyActorExt_copyProxyInformation(Actor *dest, Actor *src) {
    if (dest && src) {
        PlayerProxyInfo *srcProxyInfo = getPlayerProxyInfo(src);
        
        if (srcProxyInfo) {
            if (ProxyActorExt_setFormProxyId(dest, srcProxyInfo->formId)) {
                return ProxyActorExt_setPlayerProxyHandle(dest, srcProxyInfo->proxyHandle);
            }
        }
    }

    return false;
}

FormProxy *ProxyActorExt_getFormProxyOrFallback(Actor *actor, FormProxyId fallbackId, PlayState *play) {
    FormProxy *fp = ProxyActorExt_getFormProxy(actor);

    if (!fp) {
        fp = ProxyActorExt_getFormProxy(&(GET_PLAYER(play)->actor));
    }

    if (!fp) {
        fp = PlayerProxy_getFormProxy(gPlayer1Proxy, fallbackId);
    }

    return fp;
}

bool ProxyActorExt_isActorHasAppearanceData(Actor *actor) {
    return !!ProxyActorExt_getPlayerProxy(actor);
}

RECOMP_CALLBACK(".", _internal_preInitHashObjects) void handleFormProxyExtensionInits(void) {
    sActorExtIdPlayerProxyInfo = z64recomp_extend_actor_all(sizeof(PlayerProxyInfo));
}

static void resetProxyInfo(PlayerProxyInfo *proxyInfo) {
    proxyInfo->formId = FORM_PROXY_ID_NONE;
    proxyInfo->proxyHandle = 0;
}

void PlayerProxyInfo_init(PlayerProxyInfo *proxyInfo) {
    resetProxyInfo(proxyInfo);
}

void PlayerProxyInfo_destroy(PlayerProxyInfo *proxyInfo) {
    if (proxyInfo->proxyHandle) {
        PlayerProxyManager_releaseReference(proxyInfo->proxyHandle);
    }

    resetProxyInfo(proxyInfo);
}

void initFormProxyExtension_on_Actor_Init(Actor *actor) {
    PlayerProxyInfo *proxyInfo = getPlayerProxyInfo(actor);

    if (proxyInfo) {
        PlayerProxyInfo_init(proxyInfo);
    }
}

static bool attachPlayerFormProxy(Player *player, bool logResult) {
    if (player->transformation < PLAYER_FORM_MAX) {
        // TODO: CREATE AN EXPORT FOR OVERRIDING PLAYER PROXY
        FormProxyId id;

        if (PlayerProxy_getProxyIdFromForm(player->transformation, &id)) {
            PlayerProxyHandle proxyHandle = 0;
            bool setHandle = false;
            bool setForm = false;
            if (player->actor.id == ACTOR_PLAYER) {
                proxyHandle = gPlayer1ProxyHandle;
            } else if (player->actor.id == ACTOR_EN_TEST3) {
                proxyHandle = gPlayer2ProxyHandle;
            }

            if (proxyHandle) {
                setHandle = ProxyActorExt_setPlayerProxyHandle(&player->actor, proxyHandle);
                setForm = ProxyActorExt_setFormProxyId(&player->actor, id);
            }

#if defined(ZELDA_ANDROID_BUILTIN_PMM)
            if (logResult) {
                recomp_android_compat_pmm_apply_log(
                    63,
                    player->actor.id,
                    player->transformation,
                    (getPlayerProxyInfo(&player->actor) ? 1 : 0) |
                        (proxyHandle ? 2 : 0) |
                        (setHandle ? 4 : 0) |
                        (setForm ? 8 : 0) |
                        (ProxyActorExt_getFormProxy(&player->actor) ? 16 : 0),
                    "ProxyActorExt_setupPlayer");
            }
#endif

            return !!ProxyActorExt_getFormProxy(&player->actor);
        }
    }

    return false;
}

void setupPlayerFormProxy_on_Player_InitCommon(Player *player) {
    attachPlayerFormProxy(player, true);
}

bool ProxyActorExt_ensurePlayerFormProxy(Player *player) {
    FormProxyId expectedId;

    if (!player || !PlayerProxy_getProxyIdFromForm(player->transformation, &expectedId)) {
        return false;
    }

    FormProxyId currentId = FORM_PROXY_ID_NONE;
    if (ProxyActorExt_getFormProxy(&player->actor) && ProxyActorExt_getFormProxyId(&player->actor, &currentId) && currentId == expectedId) {
        return true;
    }

    return attachPlayerFormProxy(player, true);
}

void clearProxyExt_on_Actor_Destroy(Actor *actor) {
    PlayerProxyInfo *proxyInfo = getPlayerProxyInfo(actor);

    if (proxyInfo) {
        PlayerProxyInfo_destroy(proxyInfo);
    }
}
