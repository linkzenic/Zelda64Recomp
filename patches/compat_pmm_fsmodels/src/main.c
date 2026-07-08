#include "modding.h"
#include "global.h"
#include "recomputils.h"
#include "recompconfig.h"
#include "recompdata.h"
#include "yazmtcorelib_api.h"
#include "playermodelmanager_api.h"
#include "libc/string.h"
#include "defines_z64o.h"
#include "defines_ooto.h"
#include "defines_mmo.h"
#include "ml64compat_mm.h"
#include "oot_objects.h"

RECOMP_IMPORT(".", bool PMMZobj_setPMMDir(const unsigned char *str));
RECOMP_IMPORT(".", int PMMZobj_scanForDiskEntries());
RECOMP_IMPORT(".", int PMMZobj_getNumDiskEntries());
RECOMP_IMPORT(".", int PMMZobj_entryInternalNameLength(int i));
RECOMP_IMPORT(".", int PMMZobj_entryDisplayNameLength(int i));
RECOMP_IMPORT(".", int PMMZobj_entryAuthorNameLength(int i));
RECOMP_IMPORT(".", bool PMMZobj_writeInternalNameToBuffer(int i, char *buffer, int bufferSize));
RECOMP_IMPORT(".", bool PMMZobj_writeDisplayNameToBuffer(int i, char *buffer, int bufferSize));
RECOMP_IMPORT(".", bool PMMZobj_writeAuthorNameToBuffer(int i, char *buffer, int bufferSize));
RECOMP_IMPORT(".", int PMMZobj_getEntryFileSize(int i));
RECOMP_IMPORT(".", bool PMMZobj_getEntryFileData(int i, char *buffer, int bufferSize));
RECOMP_IMPORT(".", bool PMMZobj_createDirectory(char *path));
RECOMP_IMPORT(".", bool PMMZobj_isDirectoryExist(char *path));
RECOMP_IMPORT(".", bool PMMZobj_readEntryU8(int i, int offset, u8 *out));
RECOMP_IMPORT(".", bool PMMZobj_readEntryU16(int i, int offset, u16 *out));
RECOMP_IMPORT(".", bool PMMZobj_readEntryU32(int i, int offset, u32 *out));
RECOMP_IMPORT(".", bool PMMZobj_isModelType(int i, PlayerModelManagerModelType t));
RECOMP_IMPORT(".", void PMMZobj_clearDiskEntries());
RECOMP_IMPORT(".", bool PMMZobj_tryLoadOOTROM());
RECOMP_IMPORT(".", bool PMMZobj_unloadOOTROM());
RECOMP_IMPORT(".", bool PMMZobj_isOOTRomLoaded());

static const PlayerTransformation FORM_MODEL_TO_FORM[PMM_MODEL_TYPE_MAX] = {
    [PMM_MODEL_TYPE_NONE] = PLAYER_FORM_MAX,
    [PMM_MODEL_TYPE_CHILD] = PLAYER_FORM_HUMAN,
    [PMM_MODEL_TYPE_ADULT] = PLAYER_FORM_HUMAN,
    [PMM_MODEL_TYPE_DEKU] = PLAYER_FORM_DEKU,
    [PMM_MODEL_TYPE_GORON] = PLAYER_FORM_GORON,
    [PMM_MODEL_TYPE_ZORA] = PLAYER_FORM_ZORA,
    [PMM_MODEL_TYPE_FIERCE_DEITY] = PLAYER_FORM_FIERCE_DEITY,
};

static const char FORM_MODEL_TO_SUFFIX[PMM_MODEL_TYPE_MAX] = {
    [PMM_MODEL_TYPE_NONE] = 'N',
    [PMM_MODEL_TYPE_CHILD] = 'C',
    [PMM_MODEL_TYPE_ADULT] = 'A',
    [PMM_MODEL_TYPE_DEKU] = 'D',
    [PMM_MODEL_TYPE_GORON] = 'G',
    [PMM_MODEL_TYPE_ZORA] = 'Z',
    [PMM_MODEL_TYPE_FIERCE_DEITY] = 'F',
};

static U32ValueHashmapHandle sModelBuffers = 0;

// Combine, at most, 32 path strings
char *getCombinedPath(int count, ...) {
    typedef struct {
        char *str;
        int length;
    } StringInfo;

    const int MAX_COUNT = 32;

    if (count > MAX_COUNT) {
        return NULL;
    }

    char *combinedPath = NULL;

    size_t finalPathLen = 0;

    StringInfo pathStrings[count];

    va_list args;
    va_start(args, count);

    for (int i = 0; i < count; ++i) {
        StringInfo *strInfo = &pathStrings[i];
        strInfo->str = NULL;
        strInfo->length = 0;

        strInfo->str = va_arg(args, char *);

        strInfo->length = strlen(strInfo->str);

        finalPathLen += strInfo->length;
    }

    va_end(args);

    // make room for dir seperators
    finalPathLen += count - 1;

    // room for null byte
    ++finalPathLen;

    combinedPath = recomp_alloc(finalPathLen * sizeof(char));
    char *pos = combinedPath;

    for (int i = 0; i < count; ++i) {
        StringInfo *strInfo = &pathStrings[i];

        for (int j = 0; j < strInfo->length; ++j) {
            *pos = strInfo->str[j];
            ++pos;
        }

        // add directory seperators between path names
        if (i < count - 1) {
            // Windows/Mac/Linux all support forward slash dir seperator
            // so there's no need to handle back slash
            pos[0] = '/';
            ++pos;
        }
    }

    combinedPath[finalPathLen - 1] = '\0';

    return combinedPath;
}

char *getStringFromEntry(int i, bool nameWriter(int i, char *buffer, int bufferSize), int lengthGetter(int i)) {
    char *result = NULL;

    int stringLen = lengthGetter(i) + 1;

    if (stringLen > 0) {
        int len = stringLen + 1;

        result = recomp_alloc(len);

        if (!nameWriter(i, result, len)) {
            recomp_free(result);
            result = NULL;
        }
    }

    return result;
}

PLAYERMODELMANAGER_CALLBACK_REGISTER_MODELS void registerDiskModels() {
    if (!sModelBuffers) {
        sModelBuffers = recomputil_create_u32_value_hashmap();
    }

    {
#if defined(ZELDA_ANDROID_BUILTIN_PMM)
        PMMZobj_setPMMDir(NULL);
#else
        unsigned char *modDir = recomp_get_mod_folder_path();

        PMMZobj_setPMMDir(modDir);

        recomp_free(modDir);
#endif
    }

    PMMZobj_tryLoadOOTROM();
    loadGameplayKeepOOT();
    registerChildLink();
    registerAdultLink();

    int numDiskEntries = PMMZobj_scanForDiskEntries();

    for (int i = 0; i < numDiskEntries; ++i) {
        char *internalName = getStringFromEntry(i, PMMZobj_writeInternalNameToBuffer, PMMZobj_entryInternalNameLength);
        char *displayName = getStringFromEntry(i, PMMZobj_writeDisplayNameToBuffer, PMMZobj_entryDisplayNameLength);
        char *authorName = getStringFromEntry(i, PMMZobj_writeAuthorNameToBuffer, PMMZobj_entryAuthorNameLength);
        int entrySize = PMMZobj_getEntryFileSize(i);

        if (entrySize > 0 && internalName && internalName[0] != '\0') {
            char *formChar = internalName;

            while (*formChar != '\0') {
                formChar++;
            }
            formChar--;

            // recomp_printf("Allocating 0x%X bytes for model with internal name '%s'...\n", entrySize, internalName);
            void *modelBuf = recomp_alloc(entrySize);

            if (PMMZobj_getEntryFileData(i, modelBuf, entrySize)) {
                for (int j = 0; j < PMM_MODEL_TYPE_MAX; ++j) {
                    if (PMMZobj_isModelType(i, j)) {
                        // If a model is registered for multiple model types, we differentiate them by the last character internally
                        *formChar = FORM_MODEL_TO_SUFFIX[j];

                        PlayerModelManagerHandle h = PLAYERMODELMANAGER_REGISTER_MODEL(internalName, j);

                        if (displayName) {
                            PlayerModelManager_setDisplayName(h, displayName);
                        }

                        if (authorName) {
                            PlayerModelManager_setAuthor(h, authorName);
                        }

                        recomputil_u32_value_hashmap_insert(sModelBuffers, h, (uintptr_t)modelBuf);

                        // char *debugDispName = displayName ? displayName : "";
                        // char *debugAuthorName = authorName ? authorName : "";
                        // recomp_printf("Processing zobj...\n"
                        //               "Display Name: %s\n"
                        //               "Internal Name: %s\n"
                        //               "Author: %s\n"
                        //               "-----------------\n",
                        //               debugDispName,
                        //               internalName,
                        //               authorName);

                        setupZobjZ64O(h, modelBuf);
                    }
                }
            } else {
                recomp_free(modelBuf);
            }
        }

        recomp_free(internalName);
        recomp_free(displayName);
        recomp_free(authorName);
    }

    PMMZobj_unloadOOTROM();

    PMMZobj_clearDiskEntries();

}
