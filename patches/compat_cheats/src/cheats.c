#include "recomp_api.h"
#include "z64extern.h"

static PlayState* sCompatCheatsPlayState;
static s32 sCompatCheatsActorCatExplosivesLength = -1;

static s32 compat_cheats_clamp_upgrade(s32 value) {
    if (value < 0) {
        return 0;
    }

    if (value > 3) {
        return 3;
    }

    return value;
}

static s32 compat_cheats_wallet_capacity(void) {
    switch (GET_CUR_UPG_VALUE(UPG_WALLET)) {
        case 0:
            return 99;
        case 1:
            return 200;
        case 2:
            return 500;
        default:
            return 999;
    }
}

static s32 compat_cheats_quiver_capacity(void) {
    static s32 capacities[] = { 0, 30, 40, 50 };
    return capacities[compat_cheats_clamp_upgrade(GET_CUR_UPG_VALUE(UPG_QUIVER))];
}

static s32 compat_cheats_bomb_bag_capacity(void) {
    static s32 capacities[] = { 0, 20, 30, 40 };
    return capacities[compat_cheats_clamp_upgrade(GET_CUR_UPG_VALUE(UPG_BOMB_BAG))];
}

static s32 compat_cheats_deku_stick_capacity(void) {
    static s32 capacities[] = { 0, 10, 20, 30 };
    return capacities[compat_cheats_clamp_upgrade(GET_CUR_UPG_VALUE(UPG_DEKU_STICKS))];
}

static s32 compat_cheats_deku_nut_capacity(void) {
    static s32 capacities[] = { 0, 20, 30, 40 };
    return capacities[compat_cheats_clamp_upgrade(GET_CUR_UPG_VALUE(UPG_DEKU_NUTS))];
}

static u32 compat_cheats_config(const char* key) {
    return recomp_get_config_u32(key);
}

RECOMP_CALLBACK("*", recomp_on_play_init)
void CompatCheats_OnPlayInit(PlayState* play) {
    sCompatCheatsPlayState = play;
}

RECOMP_CALLBACK("*", recomp_on_play_main)
void CompatCheats_OnPlayMain(PlayState* play) {
    if (compat_cheats_config("infinite_magic")) {
        gSaveContext.save.saveInfo.playerData.magic =
            MAGIC_NORMAL_METER * gSaveContext.save.saveInfo.playerData.magicLevel;
    }

    if (compat_cheats_config("infinite_health")) {
        gSaveContext.save.saveInfo.playerData.health = gSaveContext.save.saveInfo.playerData.healthCapacity;
    }

    if (compat_cheats_config("infinite_rupees")) {
        s32 capacity = compat_cheats_wallet_capacity();
        gSaveContext.save.saveInfo.playerData.rupees = capacity;
    }

    if (compat_cheats_config("infinite_consumables")) {
        if (INV_CONTENT(ITEM_BOW) == ITEM_BOW) {
            AMMO(ITEM_BOW) = compat_cheats_quiver_capacity();
        }

        if (INV_CONTENT(ITEM_BOMB) == ITEM_BOMB) {
            AMMO(ITEM_BOMB) = compat_cheats_bomb_bag_capacity();
        }

        if (INV_CONTENT(ITEM_BOMBCHU) == ITEM_BOMBCHU) {
            AMMO(ITEM_BOMBCHU) = compat_cheats_bomb_bag_capacity();
        }

        if (INV_CONTENT(ITEM_DEKU_STICK) == ITEM_DEKU_STICK) {
            AMMO(ITEM_DEKU_STICK) = compat_cheats_deku_stick_capacity();
        }

        if (INV_CONTENT(ITEM_DEKU_NUT) == ITEM_DEKU_NUT) {
            AMMO(ITEM_DEKU_NUT) = compat_cheats_deku_nut_capacity();
        }

        if (INV_CONTENT(ITEM_MAGIC_BEANS) == ITEM_MAGIC_BEANS) {
            AMMO(ITEM_MAGIC_BEANS) = 20;
        }

        if (INV_CONTENT(ITEM_POWDER_KEG) == ITEM_POWDER_KEG) {
            AMMO(ITEM_POWDER_KEG) = 1;
        }
    }
}

RECOMP_HOOK_RETURN("Interface_UpdateButtonsPart1")
void CompatCheats_Interface_UpdateButtonsPart1() {
    if (!compat_cheats_config("unrestricted_items")) {
        return;
    }

    gSaveContext.bButtonStatus = BTN_ENABLED;
    gSaveContext.buttonStatus[EQUIP_SLOT_C_RIGHT] = BTN_ENABLED;
    gSaveContext.buttonStatus[EQUIP_SLOT_C_DOWN] = BTN_ENABLED;
    gSaveContext.buttonStatus[EQUIP_SLOT_C_LEFT] = BTN_ENABLED;
    gSaveContext.buttonStatus[EQUIP_SLOT_B] = BTN_ENABLED;
}

RECOMP_HOOK_RETURN("Player_ProcessItemButtons")
void CompatCheats_Player_ProcessItemButtons() {
    if (sCompatCheatsPlayState == NULL) {
        return;
    }

    Player* player = GET_PLAYER(sCompatCheatsPlayState);
    u32 cooldownSeconds = compat_cheats_config("blast_mask_cooldown");
    if (cooldownSeconds == 15 || player->blastMaskTimer != 310) {
        return;
    }

    player->blastMaskTimer = cooldownSeconds * 20;
}

RECOMP_HOOK("Player_Action_94")
void CompatCheats_Player_Action_94(Player* this, PlayState* play) {
    if (compat_cheats_config("longer_deku_flower_glide")) {
        D_8085D958[0] = 99999.9f;
        D_8085D958[1] = 99999.9f;
    }
    else {
        D_8085D958[0] = 600.0f;
        D_8085D958[1] = 960.0f;
    }
}

RECOMP_HOOK("Player_UseItem")
void CompatCheats_Player_UseItem(PlayState* play, Player* this, ItemId item) {
    sCompatCheatsPlayState = play;

    if (!compat_cheats_config("no_explosive_limit")) {
        return;
    }

    if (item != ITEM_BOMB && item != ITEM_BOMBCHU && item != ITEM_POWDER_KEG) {
        return;
    }

    sCompatCheatsActorCatExplosivesLength = play->actorCtx.actorLists[ACTORCAT_EXPLOSIVES].length;
    play->actorCtx.actorLists[ACTORCAT_EXPLOSIVES].length = 0;
}

RECOMP_HOOK_RETURN("Player_UseItem")
void CompatCheats_After_Player_UseItem() {
    if (sCompatCheatsPlayState != NULL && sCompatCheatsActorCatExplosivesLength != -1) {
        sCompatCheatsPlayState->actorCtx.actorLists[ACTORCAT_EXPLOSIVES].length =
            sCompatCheatsActorCatExplosivesLength;
        sCompatCheatsActorCatExplosivesLength = -1;
    }
}

RECOMP_PATCH u32 SurfaceType_IsHookshotSurface(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId) {
    if (compat_cheats_config("hookshot_everything")) {
        return true;
    }

    return (SurfaceType_GetData(colCtx, poly, bgId, 1) >> 17) & 1;
}

RECOMP_PATCH void ArmsHook_Wait(ArmsHook* this, PlayState* play) {
    if (this->actor.parent == NULL) {
        u32 multiplier = compat_cheats_config("hookshot_length_multiplier");
        if (multiplier < 1) {
            multiplier = 1;
        }
        else if (multiplier > 5) {
            multiplier = 5;
        }

        ArmsHook_SetupAction(this, ArmsHook_Shoot);
        Actor_SetSpeeds(&this->actor, 20.0f);
        this->actor.parent = &GET_PLAYER(play)->actor;
        this->timer = 26 * multiplier;
    }
}
