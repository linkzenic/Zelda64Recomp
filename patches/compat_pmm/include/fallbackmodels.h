#ifndef FALLBACKMODELS_H
#define FALLBACKMODELS_H

#include "playermodelmanager_api.h"
#include "z64animation.h"

typedef struct ModelEntryForm ModelEntryForm;

extern PlayerModelManagerHandle gHumanModelHandle;
extern PlayerModelManagerHandle gKafeiModelHandle;
extern PlayerModelManagerHandle gDekuModelHandle;
extern PlayerModelManagerHandle gGoronModelHandle;
extern PlayerModelManagerHandle gZoraModelHandle;
extern PlayerModelManagerHandle gFierceDeityModelHandle;

// FlexSkeletonHeader should contain a pointer to an array of LodLimb limbs
void FallbackModelsCommon_doCommonAssignments(PlayerModelManagerHandle h, FlexSkeletonHeader *skel, void *seg06, void *seg04);
void FallbackModelsCommon_addEquipmentAdultMM(PlayerModelManagerHandle h);
void FallbackModelsCommon_addEquipmentChildMM(PlayerModelManagerHandle h);

#endif
