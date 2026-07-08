#ifndef EQUIPMENTOVERRIDES_H
#define EQUIPMENTOVERRIDES_H

#include "PR/ultratypes.h"
#include "modelmatrixids.h"
#include "playermodelmanager_api.h"

typedef struct EquipmentOverrideDLs {
    Link_DisplayList *overrides;
    size_t count;
} EquipmentOverrideDLs;

typedef struct EquipmentOverrideMtxs {
    Link_EquipmentMatrix *overrides;
    size_t count;
} EquipmentOverrideMtxs;

typedef struct EquipmentOverride {
    EquipmentOverrideDLs dl;
    EquipmentOverrideMtxs mtx;
} EquipmentOverride;

const EquipmentOverride *EquipmentOverrides_getEquipmentOverride(PlayerModelManagerModelType type);

#endif
