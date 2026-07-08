#include "global.h"
#include "modding.h"
#include "modelmatrixids.h"
#include "modelentry.h"
#include "proxyactorext.h"
#include "formproxy.h"
#include "playerproxy.h"
#include "utils.h"
#include "logger.h"
#include "modelentrymanager.h"

RECOMP_EXPORT bool PlayerModelManager_Actor_isModelApplied(Actor *actor, PlayerModelManagerHandle h) {
    const ModelEntry *entryToCheck = ModelEntryManager_getEntry(h);

    if (entryToCheck) {
        PlayerModelManagerModelType type = ModelEntry_getType(entryToCheck);

        if (Utils_isFormModelType(type)) {
            FormProxy *fp = ProxyActorExt_getFormProxy(actor);

            if (fp) {
                ModelEntryForm *entryForm = FormProxy_getModelEntryForm(fp);

                return entryForm && ModelEntryForm_getModelEntry(entryForm) == entryToCheck;
            }
        } else if (Utils_isEquipmentModelType(type)) {
            PlayerProxy *pp = ProxyActorExt_getPlayerProxy(actor);

            if (pp) {
                return PlayerProxy_isModelEntryApplied(pp, entryToCheck);
            }
        }
    }

    return false;
}

RECOMP_EXPORT Gfx *PlayerModelManager_Actor_getDisplayList(Actor *actor, Link_DisplayList dlId) {
    if (!Utils_isValidDisplayListId(dlId)) {
        Logger_printError("Mod requesting invalid display list ID %d!", dlId);
        return NULL;
    }

    FormProxy *fp = ProxyActorExt_getFormProxy(actor);

    if (fp) {
        return FormProxy_getDL(fp, dlId);
    }

    return NULL;
}

RECOMP_EXPORT Mtx *PlayerModelManager_Actor_getMatrix(Actor *actor, Link_EquipmentMatrix mtxId) {
    if (!Utils_isValidMatrixId(mtxId)) {
        Logger_printError("Mod requesting invalid matrix ID %d!", mtxId);
        return NULL;
    }

    FormProxy *fp = ProxyActorExt_getFormProxy(actor);

    if (fp) {
        return FormProxy_getMatrix(fp, mtxId);
    }

    return NULL;
}

RECOMP_EXPORT bool PlayerModelManager_Actor_hasAppearanceData(Actor *actor) {
    return !!ProxyActorExt_getPlayerProxy(actor);
}
