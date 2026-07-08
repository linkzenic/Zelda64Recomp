#ifndef __RECOMPCONFIG_H__
#define __RECOMPCONFIG_H__

#include "modding.h"

// Reads a config value of the specified type and name for the mod that called these functions.
// These correspond to the config schema provided in the mod's manifest.
RECOMP_IMPORT("*", unsigned long recomp_get_config_u32(const char* key));
RECOMP_IMPORT("*", double recomp_get_config_double(const char* key));
RECOMP_IMPORT("*", char* recomp_get_config_string(const char* key));

// Frees a value returned by `recomp_get_config_string`. MUST be called to prevent a memory leak.
RECOMP_IMPORT("*", void recomp_free_config_string(char* str));

// Gets the version of the mod that called this function. Writes the mod's version numbers into the provided pointers.
RECOMP_IMPORT("*", void recomp_get_mod_version(unsigned long* major, unsigned long* minor, unsigned long* patch));

// Swaps to using a different file. The new save file will be located at `<mod id>/<filename>.bin` in the normal saves folder.
// Don't include `.bin` in the provided filename.
// Be careful calling this function during normal gameplay as the game won't be aware that any currently loaded save data is outdated.
RECOMP_IMPORT("*", void recomp_change_save_file(const char* filename));

// Returns a UTF-8 encoded zero-terminated string containing the absolute path to the current save file.
// The return type is an unsigned char pointer to indicate the UTF-8 encoding.
// `recomp_free` (found in `recomputils.h`) MUST be called on the return value of this when the value is no longer in use to prevent a memory leak.
RECOMP_IMPORT("*", unsigned char* recomp_get_save_file_path());

#endif
