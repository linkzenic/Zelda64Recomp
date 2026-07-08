#ifndef ZELDA_COMPAT_BOMB_ARROWS_RECOMPCONFIG_H
#define ZELDA_COMPAT_BOMB_ARROWS_RECOMPCONFIG_H

#include "modding.h"

RECOMP_IMPORT(".", unsigned long recomp_android_compat_bomb_arrows_get_config_u32(const char* key));

static inline unsigned long recomp_get_config_u32(const char* key) {
    return recomp_android_compat_bomb_arrows_get_config_u32(key);
}

#endif
