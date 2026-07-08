#include "global.h"
#include "modding.h"
#include "recompconfig.h"
#include "rt64_extended_gbi.h"
#include "formproxy.h"
#include "playerproxy.h"
#include "playermodelconfig.h"
#include "proxyactorext.h"
#include "../../misc_funcs.h"

static FormProxy *getTunicColorFormProxy(Player *player) {
    FormProxy *fp = ProxyActorExt_getFormProxy(&player->actor);

#if defined(ZELDA_ANDROID_BUILTIN_PMM)
    if (!fp && gPlayer1Proxy) {
        FormProxyId fpId;

        if (PlayerProxy_getProxyIdFromForm(player->transformation, &fpId)) {
            fp = PlayerProxy_getFormProxy(gPlayer1Proxy, fpId);
        }
    }
#endif

    return fp;
}

// Convert char to its numeric value
// Returns 0xFF if invalid character
u8 cToNum(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    } else if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    } else if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    }

    return 0xFF;
}

u8 sToU8(const char *s) {
    return (cToNum(s[0]) << 4) | cToNum(s[1]);
}

bool isValidHexString(const char *s) {
    bool isValid = true;

    size_t len = 0;

    while (*s != '\0' && isValid) {
        if (len > 6 || cToNum(*s) == 0xFF) {
            isValid = false;
        }
        s++;
        len++;
    }

    if (len < 6) {
        isValid = false;
    }

    return isValid;
}

typedef enum {
    TUNIC_COLOR_OFF,
    TUNIC_COLOR_AUTO,
    TUNIC_COLOR_FORCE,
} TunicColorConfigOption;

static void applyTunicColor(Player *player, PlayState *play) {
    static s32 sTunicDiagCounter = 0;
    FormProxy *fp = getTunicColorFormProxy(player);

    if (fp) {
        TunicColorConfigOption tunicColorConfig = recomp_get_config_u32("is_modify_tunic_color");

        if (tunicColorConfig == TUNIC_COLOR_OFF) {
            return;
        }

        if (tunicColorConfig == TUNIC_COLOR_FORCE) {
            FormProxy_pullCurrentTunicColorFromConfig(fp);
        } else if (FormProxy_isTunicColorOverrideRequested(fp)) {
            FormProxy_pullCurrentTunicColorFromRequested(fp);
        } else {
            FormProxy_pullCurrentTunicColorFromConfig(fp);
        }

        Color_RGBA8 tunicColor = FormProxy_getCurrentTunicColor(fp);

        if ((sTunicDiagCounter++ % 240) == 0) {
            s32 packedColor = (tunicColor.r << 16) | (tunicColor.g << 8) | tunicColor.b;
            recomp_android_compat_pmm_apply_log(60, tunicColorConfig, packedColor,
                                                FormProxy_isTunicColorOverrideRequested(fp) ? 1 : 0, "TunicColor");
        }

        OPEN_DISPS(play->state.gfxCtx);
        gDPSetEnvColor(POLY_OPA_DISP++, tunicColor.r, tunicColor.g, tunicColor.b, tunicColor.a);
        CLOSE_DISPS(play->state.gfxCtx);
    }
}

void readTunicColor_on_Player_Draw(Player *player, PlayState *play) {
    applyTunicColor(player, play);
}

void handleTunicColor_on_Player_PostLimbDrawGameplay(PlayState *play, Player *player) {
    applyTunicColor(player, play);

    OPEN_DISPS(play->state.gfxCtx);
    gEXPushEnvColor(POLY_OPA_DISP++);
    CLOSE_DISPS(play->state.gfxCtx);
}

void handleTunicColor_on_return_Player_PostLimbDrawGameplay(PlayState *play) {
    OPEN_DISPS(play->state.gfxCtx);
    gEXPopEnvColor(POLY_OPA_DISP++);
    CLOSE_DISPS(play->state.gfxCtx);
}

extern Color_RGB8 sPlayerBottleColors[];

void fixBottleEnv_func_80128640(PlayState *play, Player *player) {
    if (player->leftHandType == PLAYER_MODELTYPE_LH_BOTTLE) {
        PlayerBottle bottle = Player_BottleFromIA(player, player->itemAction);

        OPEN_DISPS(play->state.gfxCtx);

        Color_RGB8 *bottleColor = &sPlayerBottleColors[bottle];

        gDPSetEnvColor(POLY_XLU_DISP++, bottleColor->r, bottleColor->g, bottleColor->b, 0);

        CLOSE_DISPS(play->state.gfxCtx);
    }
}
