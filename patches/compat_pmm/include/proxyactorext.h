#ifndef PROXYACTOREXT_H
#define PROXYACTOREXT_H

#include "stdbool.h"
#include "modelmatrixids.h"
#include "playerproxymanager.h"

typedef struct Actor Actor;
typedef struct Player Player;
typedef struct PlayerProxy PlayerProxy;
typedef struct FormProxy FormProxy;
typedef struct PlayState PlayState;

FormProxy *ProxyActorExt_getFormProxy(Actor *actor);
PlayerProxy *ProxyActorExt_getPlayerProxy(Actor *actor);
bool ProxyActorExt_getFormProxyId(Actor *actor, FormProxyId *out);
bool ProxyActorExt_setFormProxyId(Actor *actor, FormProxyId id);
bool ProxyActorExt_setPlayerProxyHandle(Actor *actor, PlayerProxyHandle h);
bool ProxyActorExt_copyProxyInformation(Actor *dest, Actor *src);
FormProxy *ProxyActorExt_getFormProxyOrFallback(Actor *actor, FormProxyId fallbackId, PlayState *play);
bool ProxyActorExt_isActorHasAppearanceData(Actor *actor);
bool ProxyActorExt_ensurePlayerFormProxy(Player *player);

#endif
