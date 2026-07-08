#ifndef __OOT_OBJECTS_H__
#define __OOT_OBJECTS_H__

#include "playermodelmanager_api.h"

void *getGameplayKeepOOT();

void loadGameplayKeepOOT();

void registerChildLink();

void registerAdultLink();

extern u8 *gGameplayKeepOOT;
extern u8 *gLinkAdultOOT;
extern u8 *gLinkChildOOT;
extern u8 *gMirRayOOT;

PlayerModelManagerHandle getAdultLinkHumanHandle();

PlayerModelManagerHandle getChildLinkHumanHandle();

PlayerModelManagerHandle getAdultLinkFierceDeityHandle();

#endif
