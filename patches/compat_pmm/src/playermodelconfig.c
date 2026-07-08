#include "global.h"
#include "playermodelconfig.h"
#include "yazmtcorelib_api.h"
#include "modding.h"

static FormProxyId sNextId = 1;

static YAZMTCore_IterableU32Set *sFormIdsContainer;

static FormProxyIdsArray sFormIdsArr;

FormProxyIdsArray const *const gFormProxyIds = &sFormIdsArr;

static FormProxyId sFormProxyIdFierceDeity;
static FormProxyId sFormProxyIdGoron;
static FormProxyId sFormProxyIdZora;
static FormProxyId sFormProxyIdDeku;
static FormProxyId sFormProxyIdHuman;

const FormProxyId *const gFormProxyIdFierceDeity = &sFormProxyIdFierceDeity;
const FormProxyId *const gFormProxyIdGoron = &sFormProxyIdGoron;
const FormProxyId *const gFormProxyIdZora = &sFormProxyIdZora;
const FormProxyId *const gFormProxyIdDeku = &sFormProxyIdDeku;
const FormProxyId *const gFormProxyIdHuman = &sFormProxyIdHuman;

static FormProxyId createNewFormId(void) {
    FormProxyId newId = sNextId;

    do {
        sNextId++;
    } while (sNextId == FORM_PROXY_ID_NONE);

    YAZMTCore_IterableU32Set_insert(sFormIdsContainer, newId);
    sFormIdsArr.ids = YAZMTCore_IterableU32Set_values(sFormIdsContainer);
    sFormIdsArr.size = YAZMTCore_IterableU32Set_size(sFormIdsContainer);

    return newId;
}

RECOMP_CALLBACK(".", _internal_initHashObjects) void initFormIdArr(void) {
    sFormIdsContainer = YAZMTCore_IterableU32Set_new();

    FormProxyId *builtInIds[] = {
        &sFormProxyIdFierceDeity,
        &sFormProxyIdGoron,
        &sFormProxyIdZora,
        &sFormProxyIdDeku,
        &sFormProxyIdHuman,
    };

    for (int i = 0; i < ARRAY_COUNT(builtInIds); ++i) {
        *builtInIds[i] = createNewFormId();
    }
}
