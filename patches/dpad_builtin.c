#include "patches.h"
#include "input.h"
#include "overlays/kaleido_scope/ovl_kaleido_scope/z_kaleido_scope.h"

// Keep the built-in D-Pad textures in .text for this standalone NRM. The mod
// packer cannot resolve a .text relocation to .rodata here, even though the
// runtime only reads the bytes as textures.
asm(".pushsection .text\n"
    "\t.local dpad_icon_default\n"
    "\t.type dpad_icon_default, @object\n"
    "\t.balign 8\n"
    "dpad_icon_default:\n"
    "\t.incbin \"dpad_default.rgba32.bin\"\n\n"
    "\t.local dpad_icon_hd\n"
    "\t.type dpad_icon_hd, @object\n"
    "\t.balign 8\n"
    "dpad_icon_hd:\n"
    "\t.incbin \"dpad_hd.rgba32.bin\"\n\n"
    "\t.balign 8\n"
    "\t.popsection\n");
extern u8 dpad_icon_default[];
extern u8 dpad_icon_hd[];

void CmpDma_LoadFile(uintptr_t segmentVrom, s32 id, void* dst, size_t size);

typedef enum {
    DPAD_ITEMS_MODE_OFF = 0,
    DPAD_ITEMS_MODE_DEFAULT = 1,
    DPAD_ITEMS_MODE_HD_STYLE = 2,
} DpadItemsMode;

s32 dpad_items_mode = DPAD_ITEMS_MODE_DEFAULT;

static bool dpad_items_enabled(void) {
    return dpad_items_mode != DPAD_ITEMS_MODE_OFF;
}

static bool dpad_hd_style_enabled(void) {
    return dpad_items_mode == DPAD_ITEMS_MODE_HD_STYLE;
}

static u8* dpad_icon_texture(void) {
    return dpad_hd_style_enabled() ? dpad_icon_hd : dpad_icon_default;
}

#define DPAD_W 18
#define DPAD_H 18

#define DPAD_IMG_W 32
#define DPAD_IMG_H 32

#define DPAD_DSDX (s32)(1024.0f * (float)(DPAD_IMG_W) / (DPAD_W))
#define DPAD_DTDY (s32)(1024.0f * (float)(DPAD_IMG_H) / (DPAD_H))

#define DPAD_CENTER_X 32
#define DPAD_CENTER_Y 76

#define ICON_IMG_SIZE 32
#define ICON_SIZE 16
#define ICON_DIST 14

#define ICON_DSDX (s32)(1024.0f * (float)(ICON_IMG_SIZE) / (ICON_SIZE))
#define ICON_DTDY (s32)(1024.0f * (float)(ICON_IMG_SIZE) / (ICON_SIZE))

#define EXTRA_ITEM_SLOT_COUNT 4
#define TOTAL_SLOT_COUNT (3 + EXTRA_ITEM_SLOT_COUNT)

u8 extra_item_slot_statuses[EXTRA_ITEM_SLOT_COUNT];
s16 extra_item_slot_alphas[EXTRA_ITEM_SLOT_COUNT];
u8 extra_button_items[4][EXTRA_ITEM_SLOT_COUNT] = {
    { ITEM_MASK_GORON, ITEM_MASK_ZORA, ITEM_OCARINA_OF_TIME, ITEM_MASK_DEKU },
    { ITEM_MASK_GORON, ITEM_MASK_ZORA, ITEM_OCARINA_OF_TIME, ITEM_MASK_DEKU },
    { ITEM_MASK_GORON, ITEM_MASK_ZORA, ITEM_OCARINA_OF_TIME, ITEM_MASK_DEKU },
    { ITEM_MASK_GORON, ITEM_MASK_ZORA, ITEM_OCARINA_OF_TIME, ITEM_MASK_DEKU },
};
bool dpad_item_icons_loaded = false;

static const u8 dpad_3ds_items[EXTRA_ITEM_SLOT_COUNT] = {
    ITEM_MASK_GORON,
    ITEM_MASK_ZORA,
    ITEM_OCARINA_OF_TIME,
    ITEM_MASK_DEKU,
};

static void dpad_apply_3ds_layout(void) {
    bool changed = false;

    for (int form = 0; form < ARRAY_COUNT(extra_button_items); form++) {
        for (int slot = 0; slot < EXTRA_ITEM_SLOT_COUNT; slot++) {
            if (extra_button_items[form][slot] != dpad_3ds_items[slot]) {
                extra_button_items[form][slot] = dpad_3ds_items[slot];
                changed = true;
            }
        }
    }

    if (changed) {
        dpad_item_icons_loaded = false;
    }
}

static void dpad_refresh_mode(void) {
    dpad_items_mode = recomp_get_dpad_items_mode_for_dpad_builtin();

    if (dpad_items_mode < DPAD_ITEMS_MODE_OFF || dpad_items_mode > DPAD_ITEMS_MODE_HD_STYLE) {
        dpad_items_mode = DPAD_ITEMS_MODE_DEFAULT;
    }

    dpad_apply_3ds_layout();
}

static void dpad_load_cross_texture(Gfx** gfx) {
    gDPLoadTextureBlock((*gfx)++, dpad_icon_texture(), G_IM_FMT_RGBA, G_IM_SIZ_32b, DPAD_IMG_W, DPAD_IMG_H, 0,
        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
}

#define EQUIP_SLOT_EX_START ARRAY_COUNT(gSaveContext.buttonStatus)

typedef enum {
    EQUIP_SLOT_EX_DUP = EQUIP_SLOT_EX_START,
    EQUIP_SLOT_EX_DLEFT,
    EQUIP_SLOT_EX_DRIGHT,
    EQUIP_SLOT_EX_DDOWN,
} EquipSlotEx;

struct ExButtonMapping {
    u32 button;
    EquipSlotEx slot;
};

// These are negated to avoid a check where the game clamps the button to B if it's greater than 
struct ExButtonMapping buttons_to_extra_slot[] = {
    {BTN_DLEFT,  -EQUIP_SLOT_EX_DLEFT},
    {BTN_DRIGHT, -EQUIP_SLOT_EX_DRIGHT},
    {BTN_DUP,    -EQUIP_SLOT_EX_DUP},
    {BTN_DDOWN,  -EQUIP_SLOT_EX_DDOWN},
};

s32 dpad_item_icon_positions[4][2] = {
    {           0, -ICON_DIST},
    { -ICON_DIST, 0          },
    {  ICON_DIST, 0          },
    {           0, ICON_DIST - 2 }
};

Gfx* Gfx_DrawRect_DropShadowEx(Gfx* gfx, u16 lorigin, u16 rorigin, s16 rectLeft, s16 rectTop, s16 rectWidth, s16 rectHeight, u16 dsdx, u16 dtdy,
                              s16 r, s16 g, s16 b, s16 a) {
    s16 dropShadowAlpha = a;

    if (a > 100) {
        dropShadowAlpha = 100;
    }

    gDPPipeSync(gfx++);
    gDPSetPrimColor(gfx++, 0, 0, 0, 0, 0, dropShadowAlpha);
    gEXTextureRectangle(gfx++, lorigin, rorigin, (rectLeft + 2) * 4, (rectTop + 2) * 4, (rectLeft + rectWidth + 2) * 4,
                        (rectTop + rectHeight + 2) * 4, G_TX_RENDERTILE, 0, 0, dsdx, dtdy);

    gDPPipeSync(gfx++);
    gDPSetPrimColor(gfx++, 0, 0, r, g, b, a);

    gEXTextureRectangle(gfx++, lorigin, rorigin, rectLeft * 4, rectTop * 4, (rectLeft + rectWidth) * 4, (rectTop + rectHeight) * 4,
                        G_TX_RENDERTILE, 0, 0, dsdx, dtdy);

    return gfx;
}

RECOMP_HOOK("Interface_DrawItemButtons") void draw_dpad(PlayState* play) {
    PauseContext* pauseCtx = &play->pauseCtx;
    dpad_refresh_mode();

    if (pauseCtx->state != PAUSE_STATE_MAIN && dpad_items_enabled()) {
        OPEN_DISPS(play->state.gfxCtx);

        gEXForceUpscale2D(OVERLAY_DISP++, 1);
        dpad_load_cross_texture(&OVERLAY_DISP);
        // Set a fullscreen scissor.
        gEXPushScissor(OVERLAY_DISP++);
        gEXSetScissor(OVERLAY_DISP++, G_SC_NON_INTERLACE, G_EX_ORIGIN_LEFT, G_EX_ORIGIN_RIGHT, 0, 0, 0, SCREEN_HEIGHT);

        // Determine the maximum alpha of all the D-Pad items and use that as the alpha of the D-Pad itself.
        int alpha = 0;
        for (int i = 0; i < 4; i++) {
            int cur_alpha = extra_item_slot_alphas[i];
            alpha = MAX(alpha, cur_alpha);
        }

        // Check if none of the D-Pad items have been obtained and clamp the alpha to 70 if so.
        bool item_obtained = false;
        for (int i = 0; i < 4; i++) {
            s32 item = extra_button_items[0][i];
            if ((item != ITEM_NONE) && (INV_CONTENT(item) == item)) {
                item_obtained = true;
                break;
            }
        }

        if (!item_obtained) {
            alpha = MIN(alpha, 70);
        }

        gDPSetCombineMode(OVERLAY_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        gDPPipeSync(OVERLAY_DISP++);
        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, alpha);
        gEXTextureRectangle(OVERLAY_DISP++, G_EX_ORIGIN_LEFT, G_EX_ORIGIN_LEFT,
            (DPAD_CENTER_X - (DPAD_W / 2)) * 4, (DPAD_CENTER_Y - (DPAD_W / 2)) * 4,
            (DPAD_CENTER_X + (DPAD_W / 2)) * 4, (DPAD_CENTER_Y + (DPAD_H / 2)) * 4,
            G_TX_RENDERTILE, 0, 0, DPAD_DSDX, DPAD_DTDY);
        gEXForceUpscale2D(OVERLAY_DISP++, 0);
        // Restore the previous scissor.
        gEXPopScissor(OVERLAY_DISP++);

        CLOSE_DISPS(play->state.gfxCtx);
    }
}

u8 dpad_item_textures[4][ICON_IMG_SIZE * ICON_IMG_SIZE * 4] __attribute__((aligned(8)));

RECOMP_HOOK("Interface_DrawCButtonIcons") void draw_dpad_icons(PlayState* play) {
    PauseContext* pauseCtx = &play->pauseCtx;
    dpad_refresh_mode();

    if (pauseCtx->state != PAUSE_STATE_MAIN && dpad_items_enabled()) {
        if (!dpad_item_icons_loaded) {
            for (int i = 0; i < 4; i++) {
                CmpDma_LoadFile(SEGMENT_ROM_START(icon_item_static_yar), extra_button_items[0][i], dpad_item_textures[i], sizeof(dpad_item_textures[i]));
            }

            dpad_item_icons_loaded = true;
        }

        OPEN_DISPS(play->state.gfxCtx);

        gEXForceUpscale2D(OVERLAY_DISP++, 1);
        // Set a fullscreen scissor.
        gEXPushScissor(OVERLAY_DISP++);
        gEXSetScissor(OVERLAY_DISP++, G_SC_NON_INTERLACE, G_EX_ORIGIN_LEFT, G_EX_ORIGIN_RIGHT, 0, 0, 0, SCREEN_HEIGHT);

        dpad_load_cross_texture(&OVERLAY_DISP);
            
        gDPSetCombineMode(OVERLAY_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

        for (int i = 0; i < 4; i++) {
            s32 item = extra_button_items[0][i];
            if ((item != ITEM_NONE) && (INV_CONTENT(item) == item)) {
                gDPLoadTextureBlock(OVERLAY_DISP++, dpad_item_textures[i], G_IM_FMT_RGBA, G_IM_SIZ_32b, 32, 32, 0, G_TX_NOMIRROR | G_TX_WRAP,
                                    G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
                gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, extra_item_slot_alphas[i]);
                gEXTextureRectangle(OVERLAY_DISP++, G_EX_ORIGIN_LEFT, G_EX_ORIGIN_LEFT,
                    (dpad_item_icon_positions[i][0] + DPAD_CENTER_X - (ICON_SIZE/2)) * 4, (dpad_item_icon_positions[i][1] + DPAD_CENTER_Y - (ICON_SIZE/2)) * 4,
                    (dpad_item_icon_positions[i][0] + DPAD_CENTER_X + (ICON_SIZE/2)) * 4, (dpad_item_icon_positions[i][1] + DPAD_CENTER_Y + (ICON_SIZE/2)) * 4,
                    0,
                    0, 0,
                    ICON_DSDX, ICON_DTDY);
            }
        }

        gEXForceUpscale2D(OVERLAY_DISP++, 0);
        // Restore the previous scissor.
        gEXPopScissor(OVERLAY_DISP++);

        CLOSE_DISPS(play->state.gfxCtx);
    }
}

// Equivalent to C_BTN_ITEM for extra item slots
#define EXTRA_BTN_ITEM(btn)                                 \
    ((extra_item_slot_statuses[(btn) - EQUIP_SLOT_EX_START] != BTN_DISABLED) \
         ? extra_button_items[CUR_FORM][(btn) - EQUIP_SLOT_EX_START]                  \
         : ((gSaveContext.hudVisibility == HUD_VISIBILITY_A_B_C) ? extra_button_items[CUR_FORM][(btn) - EQUIP_SLOT_EX_START] : ITEM_NONE))

extern Input* sPlayerControlInput;
extern u16 sPlayerItemButtons[4];

typedef struct struct_8085D910 {
    /* 0x0 */ u8 unk_0;
    /* 0x1 */ u8 unk_1;
    /* 0x2 */ u8 unk_2;
    /* 0x3 */ u8 unk_3;
} struct_8085D910; // size = 0x4

extern u16 D_8085D908[4];
extern struct_8085D910 D_8085D910[2];

bool func_808323C0(Player *this, s16 csId);
void func_80855218(PlayState *play, Player *this, struct_8085D910 **arg2);
void func_808550D0(PlayState *play, Player *this, f32 arg2, f32 arg3, s32 arg4);

extern s16 sPictoState;
extern s16 sPictoPhotoBeingTaken;
extern void* gWorkBuffer;
u16 func_801A5100(void);

// @mod Patched to check the extra item slots. Return currently-pressed button, in order of priority D-Pad, B, CLEFT, CDOWN, CRIGHT.
RECOMP_PATCH EquipSlot func_8082FDC4(void) {
    EquipSlot i;
    dpad_refresh_mode();

    // @mod Check the extra item slots.
    if (dpad_items_enabled()) {
        for (int extra_slot_index = 0; extra_slot_index < ARRAY_COUNT(buttons_to_extra_slot); extra_slot_index++) {
            if (CHECK_BTN_ALL(sPlayerControlInput->press.button, buttons_to_extra_slot[extra_slot_index].button)) {
                return (EquipSlot)buttons_to_extra_slot[extra_slot_index].slot;
            }
        }
    }

    for (i = 0; i < ARRAY_COUNT(sPlayerItemButtons); i++) {
        if (CHECK_BTN_ALL(sPlayerControlInput->press.button, sPlayerItemButtons[i])) {
            break;
        }
    }

    return i;
}

// @mod Patched to check the extra item slots.
RECOMP_PATCH ItemId Player_GetItemOnButton(PlayState* play, Player* player, EquipSlot slot) {
    if (slot >= EQUIP_SLOT_A) {
        return ITEM_NONE;
    }

    // @mod Check for extra item slots.
    if (slot <= -EQUIP_SLOT_EX_START) {
        if (!dpad_items_enabled()) {
            return ITEM_NONE;
        }

        ItemId item = EXTRA_BTN_ITEM(-slot);

        // Ensure the item was valid and has been obtained.
        if ((item != ITEM_NONE) && (INV_CONTENT(item) == item)) {
            return item;
        }
        else {
            return ITEM_NONE;
        }
    }

    if (slot == EQUIP_SLOT_B) {
        ItemId item = Inventory_GetBtnBItem(play);

        if (item >= ITEM_FD) {
            return item;
        }

        if ((player->currentMask == PLAYER_MASK_BLAST) && (play->interfaceCtx.bButtonDoAction == DO_ACTION_EXPLODE)) {
            return ITEM_F0;
        }

        if ((player->currentMask == PLAYER_MASK_BREMEN) && (play->interfaceCtx.bButtonDoAction == DO_ACTION_MARCH)) {
            return ITEM_F1;
        }

        if ((player->currentMask == PLAYER_MASK_KAMARO) && (play->interfaceCtx.bButtonDoAction == DO_ACTION_DANCE)) {
            return ITEM_F2;
        }

        return item;
    }

    if (slot == EQUIP_SLOT_C_LEFT) {
        return C_BTN_ITEM(EQUIP_SLOT_C_LEFT);
    }

    if (slot == EQUIP_SLOT_C_DOWN) {
        return C_BTN_ITEM(EQUIP_SLOT_C_DOWN);
    }

    // EQUIP_SLOT_C_RIGHT

    return C_BTN_ITEM(EQUIP_SLOT_C_RIGHT);
}

// @mod Patched to also check for d-pad buttons for skipping the transformation cutscene.
RECOMP_PATCH void Player_Action_86(Player *this, PlayState *play) {
    struct_8085D910 *sp4C = D_8085D910;
    s32 sp48 = false;

    func_808323C0(this, play->playerCsIds[PLAYER_CS_ID_MASK_TRANSFORMATION]);
    sPlayerControlInput = play->state.input;

    Camera_ChangeMode(GET_ACTIVE_CAM(play),
        (this->transformation == PLAYER_FORM_HUMAN) ? CAM_MODE_NORMAL : CAM_MODE_JUMP);
    this->stateFlags2 |= PLAYER_STATE2_40;
    this->actor.shape.rot.y = Camera_GetCamDirYaw(GET_ACTIVE_CAM(play)) + 0x8000;

    func_80855218(play, this, &sp4C);

    if (this->av1.actionVar1 == 0x14) {
        Play_EnableMotionBlurPriority(100);
    }

    if (R_PLAY_FILL_SCREEN_ON != 0) {
        R_PLAY_FILL_SCREEN_ALPHA += R_PLAY_FILL_SCREEN_ON;
        if (R_PLAY_FILL_SCREEN_ALPHA > 255) {
            R_PLAY_FILL_SCREEN_ALPHA = 255;
            this->actor.update = func_8012301C;
            this->actor.draw = NULL;
            this->av1.actionVar1 = 0;
            Play_DisableMotionBlurPriority();
            SET_WEEKEVENTREG(D_8085D908[GET_PLAYER_FORM]);
        }
    }
    else if ((this->av1.actionVar1++ > ((this->transformation == PLAYER_FORM_HUMAN) ? 0x53 : 0x37)) ||
        ((this->av1.actionVar1 >= 5) &&
            (sp48 =
                ((this->transformation != PLAYER_FORM_HUMAN) || CHECK_WEEKEVENTREG(D_8085D908[GET_PLAYER_FORM])) &&
                // @mod Patched to also check for d-pad buttons for skipping the transformation cutscene.
                CHECK_BTN_ANY(play->state.input[0].press.button,
                    BTN_CRIGHT | BTN_CLEFT | BTN_CDOWN | BTN_CUP | BTN_B | BTN_A |
                        (dpad_items_enabled() ? (BTN_DRIGHT | BTN_DLEFT | BTN_DDOWN | BTN_DUP) : 0))))) {
        R_PLAY_FILL_SCREEN_ON = 45;
        R_PLAY_FILL_SCREEN_R = 220;
        R_PLAY_FILL_SCREEN_G = 220;
        R_PLAY_FILL_SCREEN_B = 220;
        R_PLAY_FILL_SCREEN_ALPHA = 0;

        if (sp48) {
            if (CutsceneManager_GetCurrentCsId() == this->csId) {
                func_800E0348(Play_GetCamera(play, CutsceneManager_GetCurrentSubCamId(this->csId)));
            }

            if (this->transformation == PLAYER_FORM_HUMAN) {
                AudioSfx_StopById(NA_SE_PL_TRANSFORM_VOICE);
                AudioSfx_StopById(NA_SE_IT_TRANSFORM_MASK_BROKEN);
            }
            else {
                AudioSfx_StopById(NA_SE_PL_FACE_CHANGE);
            }
        }

        Player_PlaySfx(this, NA_SE_SY_TRANSFORM_MASK_FLASH);
    }

    if (this->av1.actionVar1 >= sp4C->unk_0) {
        if (this->av1.actionVar1 < sp4C->unk_2) {
            Math_StepToF(&this->unk_B10[4], 1.0f, sp4C->unk_1 / 100.0f);
        }
        else if (this->av1.actionVar1 < sp4C->unk_3) {
            if (this->av1.actionVar1 == sp4C->unk_2) {
                Lib_PlaySfx_2(NA_SE_EV_LIGHTNING_HARD);
            }

            Math_StepToF(&this->unk_B10[4], 2.0f, 0.5f);
        }
        else {
            Math_StepToF(&this->unk_B10[4], 3.0f, 0.2f);
        }
    }

    if (this->av1.actionVar1 >= 0x10) {
        if (this->av1.actionVar1 < 0x40) {
            Math_StepToF(&this->unk_B10[5], 1.0f, 0.2f);
        }
        else if (this->av1.actionVar1 < 0x37) {
            Math_StepToF(&this->unk_B10[5], 2.0f, 1.0f);
        }
        else {
            Math_StepToF(&this->unk_B10[5], 3.0f, 0.55f);
        }
    }

    func_808550D0(play, this, this->unk_B10[4], this->unk_B10[5], (this->transformation == PLAYER_FORM_HUMAN) ? 0 : 1);
}

/**
 * Sets the button alphas to be dimmed for disabled buttons, or to the requested alpha for non-disabled buttons
 */
// @mod Patched to also set extra slot alpha values.
RECOMP_PATCH void Interface_UpdateButtonAlphasByStatus(PlayState* play, s16 risingAlpha) {
    InterfaceContext* interfaceCtx = &play->interfaceCtx;

    if ((gSaveContext.buttonStatus[EQUIP_SLOT_B] == BTN_DISABLED) || (gSaveContext.bButtonStatus == BTN_DISABLED)) {
        if (interfaceCtx->bAlpha != 70) {
            interfaceCtx->bAlpha = 70;
        }
    } else {
        if (interfaceCtx->bAlpha != 255) {
            interfaceCtx->bAlpha = risingAlpha;
        }
    }

    if (gSaveContext.buttonStatus[EQUIP_SLOT_C_LEFT] == BTN_DISABLED) {
        if (interfaceCtx->cLeftAlpha != 70) {
            interfaceCtx->cLeftAlpha = 70;
        }
    } else {
        if (interfaceCtx->cLeftAlpha != 255) {
            interfaceCtx->cLeftAlpha = risingAlpha;
        }
    }

    if (gSaveContext.buttonStatus[EQUIP_SLOT_C_DOWN] == BTN_DISABLED) {
        if (interfaceCtx->cDownAlpha != 70) {
            interfaceCtx->cDownAlpha = 70;
        }
    } else {
        if (interfaceCtx->cDownAlpha != 255) {
            interfaceCtx->cDownAlpha = risingAlpha;
        }
    }

    if (gSaveContext.buttonStatus[EQUIP_SLOT_C_RIGHT] == BTN_DISABLED) {
        if (interfaceCtx->cRightAlpha != 70) {
            interfaceCtx->cRightAlpha = 70;
        }
    } else {
        if (interfaceCtx->cRightAlpha != 255) {
            interfaceCtx->cRightAlpha = risingAlpha;
        }
    }

    if (gSaveContext.buttonStatus[EQUIP_SLOT_A] == BTN_DISABLED) {
        if (interfaceCtx->aAlpha != 70) {
            interfaceCtx->aAlpha = 70;
        }
    } else {
        if (interfaceCtx->aAlpha != 255) {
            interfaceCtx->aAlpha = risingAlpha;
        }
    }

    for (int i = 0; i < EXTRA_ITEM_SLOT_COUNT; i++) {
        if (extra_item_slot_statuses[i] == BTN_DISABLED) {
            if (extra_item_slot_alphas[i] != 70) {
                extra_item_slot_alphas[i] = 70;
            }
        }
        else {
            if (extra_item_slot_alphas[i] != 255) {
                extra_item_slot_alphas[i] = risingAlpha;
            }
        }
    }
}

/**
 * Lower button alphas on the HUD to the requested value
 * If (gSaveContext.hudVisibilityForceButtonAlphasByStatus), then instead update button alphas
 * depending on button status
 */
// @mod Patched to also set extra slot alpha values.
RECOMP_PATCH void Interface_UpdateButtonAlphas(PlayState* play, s16 dimmingAlpha, s16 risingAlpha) {
    InterfaceContext* interfaceCtx = &play->interfaceCtx;

    if (gSaveContext.hudVisibilityForceButtonAlphasByStatus) {
        Interface_UpdateButtonAlphasByStatus(play, risingAlpha);
        return;
    }

    if ((interfaceCtx->bAlpha != 0) && (interfaceCtx->bAlpha > dimmingAlpha)) {
        interfaceCtx->bAlpha = dimmingAlpha;
    }

    if ((interfaceCtx->aAlpha != 0) && (interfaceCtx->aAlpha > dimmingAlpha)) {
        interfaceCtx->aAlpha = dimmingAlpha;
    }

    if ((interfaceCtx->cLeftAlpha != 0) && (interfaceCtx->cLeftAlpha > dimmingAlpha)) {
        interfaceCtx->cLeftAlpha = dimmingAlpha;
    }

    if ((interfaceCtx->cDownAlpha != 0) && (interfaceCtx->cDownAlpha > dimmingAlpha)) {
        interfaceCtx->cDownAlpha = dimmingAlpha;
    }

    if ((interfaceCtx->cRightAlpha != 0) && (interfaceCtx->cRightAlpha > dimmingAlpha)) {
        interfaceCtx->cRightAlpha = dimmingAlpha;
    }
            
    // @mod
    for (int i = 0; i < EXTRA_ITEM_SLOT_COUNT; i++) {
        if ((extra_item_slot_alphas[i] != 0) && (extra_item_slot_alphas[i] > dimmingAlpha)) {
            extra_item_slot_alphas[i] = dimmingAlpha;
        }
    }
}

// @mod Patched to also set extra slot alpha values.
RECOMP_PATCH void Interface_UpdateHudAlphas(PlayState* play, s16 dimmingAlpha) {
    InterfaceContext* interfaceCtx = &play->interfaceCtx;
    s16 risingAlpha = 255 - dimmingAlpha;

    switch (gSaveContext.nextHudVisibility) {
        case HUD_VISIBILITY_NONE:
        case HUD_VISIBILITY_NONE_ALT:
        case HUD_VISIBILITY_B:
            if (gSaveContext.nextHudVisibility == HUD_VISIBILITY_B) {
                if (interfaceCtx->bAlpha != 255) {
                    interfaceCtx->bAlpha = risingAlpha;
                }
            } else {
                if ((interfaceCtx->bAlpha != 0) && (interfaceCtx->bAlpha > dimmingAlpha)) {
                    interfaceCtx->bAlpha = dimmingAlpha;
                }
            }

            if ((interfaceCtx->aAlpha != 0) && (interfaceCtx->aAlpha > dimmingAlpha)) {
                interfaceCtx->aAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->cLeftAlpha != 0) && (interfaceCtx->cLeftAlpha > dimmingAlpha)) {
                interfaceCtx->cLeftAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->cDownAlpha != 0) && (interfaceCtx->cDownAlpha > dimmingAlpha)) {
                interfaceCtx->cDownAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->cRightAlpha != 0) && (interfaceCtx->cRightAlpha > dimmingAlpha)) {
                interfaceCtx->cRightAlpha = dimmingAlpha;
            }
            
            // @mod
            for (int i = 0; i < EXTRA_ITEM_SLOT_COUNT; i++) {
                if ((extra_item_slot_alphas[i] != 0) && (extra_item_slot_alphas[i] > dimmingAlpha)) {
                    extra_item_slot_alphas[i] = dimmingAlpha;
                }
            }

            if ((interfaceCtx->healthAlpha != 0) && (interfaceCtx->healthAlpha > dimmingAlpha)) {
                interfaceCtx->healthAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->magicAlpha != 0) && (interfaceCtx->magicAlpha > dimmingAlpha)) {
                interfaceCtx->magicAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->minimapAlpha != 0) && (interfaceCtx->minimapAlpha > dimmingAlpha)) {
                interfaceCtx->minimapAlpha = dimmingAlpha;
            }

            break;

        case HUD_VISIBILITY_HEARTS_WITH_OVERWRITE:
            // aAlpha is immediately overwritten in Interface_UpdateButtonAlphas
            if ((interfaceCtx->aAlpha != 0) && (interfaceCtx->aAlpha > dimmingAlpha)) {
                interfaceCtx->aAlpha = dimmingAlpha;
            }

            Interface_UpdateButtonAlphas(play, dimmingAlpha, risingAlpha + 0);

            if ((interfaceCtx->magicAlpha != 0) && (interfaceCtx->magicAlpha > dimmingAlpha)) {
                interfaceCtx->magicAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->minimapAlpha != 0) && (interfaceCtx->minimapAlpha > dimmingAlpha)) {
                interfaceCtx->minimapAlpha = dimmingAlpha;
            }

            if (interfaceCtx->healthAlpha != 255) {
                interfaceCtx->healthAlpha = risingAlpha;
            }

            break;

        case HUD_VISIBILITY_A:
            if ((interfaceCtx->bAlpha != 0) && (interfaceCtx->bAlpha > dimmingAlpha)) {
                interfaceCtx->bAlpha = dimmingAlpha;
            }

            // aAlpha is immediately overwritten below
            if ((interfaceCtx->aAlpha != 0) && (interfaceCtx->aAlpha > dimmingAlpha)) {
                interfaceCtx->aAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->cLeftAlpha != 0) && (interfaceCtx->cLeftAlpha > dimmingAlpha)) {
                interfaceCtx->cLeftAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->cDownAlpha != 0) && (interfaceCtx->cDownAlpha > dimmingAlpha)) {
                interfaceCtx->cDownAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->cRightAlpha != 0) && (interfaceCtx->cRightAlpha > dimmingAlpha)) {
                interfaceCtx->cRightAlpha = dimmingAlpha;
            }
            
            // @mod
            for (int i = 0; i < EXTRA_ITEM_SLOT_COUNT; i++) {
                if ((extra_item_slot_alphas[i] != 0) && (extra_item_slot_alphas[i] > dimmingAlpha)) {
                    extra_item_slot_alphas[i] = dimmingAlpha;
                }
            }

            if ((interfaceCtx->healthAlpha != 0) && (interfaceCtx->healthAlpha > dimmingAlpha)) {
                interfaceCtx->healthAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->magicAlpha != 0) && (interfaceCtx->magicAlpha > dimmingAlpha)) {
                interfaceCtx->magicAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->minimapAlpha != 0) && (interfaceCtx->minimapAlpha > dimmingAlpha)) {
                interfaceCtx->minimapAlpha = dimmingAlpha;
            }

            if (interfaceCtx->aAlpha != 255) {
                interfaceCtx->aAlpha = risingAlpha;
            }

            break;

        case HUD_VISIBILITY_A_HEARTS_MAGIC_WITH_OVERWRITE:
            Interface_UpdateButtonAlphas(play, dimmingAlpha, risingAlpha);

            if ((interfaceCtx->minimapAlpha != 0) && (interfaceCtx->minimapAlpha > dimmingAlpha)) {
                interfaceCtx->minimapAlpha = dimmingAlpha;
            }

            // aAlpha overwrites the value set in Interface_UpdateButtonAlphas
            if (interfaceCtx->aAlpha != 255) {
                interfaceCtx->aAlpha = risingAlpha;
            }

            if (interfaceCtx->healthAlpha != 255) {
                interfaceCtx->healthAlpha = risingAlpha;
            }

            if (interfaceCtx->magicAlpha != 255) {
                interfaceCtx->magicAlpha = risingAlpha;
            }

            break;

        case HUD_VISIBILITY_A_HEARTS_MAGIC_MINIMAP_WITH_OVERWRITE:
            Interface_UpdateButtonAlphas(play, dimmingAlpha, risingAlpha);

            // aAlpha overwrites the value set in Interface_UpdateButtonAlphas
            if (interfaceCtx->aAlpha != 255) {
                interfaceCtx->aAlpha = risingAlpha;
            }

            if (interfaceCtx->healthAlpha != 255) {
                interfaceCtx->healthAlpha = risingAlpha;
            }

            if (interfaceCtx->magicAlpha != 255) {
                interfaceCtx->magicAlpha = risingAlpha;
            }

            if (play->sceneId == SCENE_SPOT00) {
                if (interfaceCtx->minimapAlpha < 170) {
                    interfaceCtx->minimapAlpha = risingAlpha;
                } else {
                    interfaceCtx->minimapAlpha = 170;
                }
            } else if (interfaceCtx->minimapAlpha != 255) {
                interfaceCtx->minimapAlpha = risingAlpha;
            }

            break;

        case HUD_VISIBILITY_ALL_NO_MINIMAP_W_DISABLED:
            if ((interfaceCtx->minimapAlpha != 0) && (interfaceCtx->minimapAlpha > dimmingAlpha)) {
                interfaceCtx->minimapAlpha = dimmingAlpha;
            }

            Interface_UpdateButtonAlphasByStatus(play, risingAlpha);

            if (interfaceCtx->healthAlpha != 255) {
                interfaceCtx->healthAlpha = risingAlpha;
            }

            if (interfaceCtx->magicAlpha != 255) {
                interfaceCtx->magicAlpha = risingAlpha;
            }

            break;

        case HUD_VISIBILITY_HEARTS_MAGIC:
            if ((interfaceCtx->bAlpha != 0) && (interfaceCtx->bAlpha > dimmingAlpha)) {
                interfaceCtx->bAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->aAlpha != 0) && (interfaceCtx->aAlpha > dimmingAlpha)) {
                interfaceCtx->aAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->cLeftAlpha != 0) && (interfaceCtx->cLeftAlpha > dimmingAlpha)) {
                interfaceCtx->cLeftAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->cDownAlpha != 0) && (interfaceCtx->cDownAlpha > dimmingAlpha)) {
                interfaceCtx->cDownAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->cRightAlpha != 0) && (interfaceCtx->cRightAlpha > dimmingAlpha)) {
                interfaceCtx->cRightAlpha = dimmingAlpha;
            }
            
            // @mod
            for (int i = 0; i < EXTRA_ITEM_SLOT_COUNT; i++) {
                if ((extra_item_slot_alphas[i] != 0) && (extra_item_slot_alphas[i] > dimmingAlpha)) {
                    extra_item_slot_alphas[i] = dimmingAlpha;
                }
            }

            if ((interfaceCtx->minimapAlpha != 0) && (interfaceCtx->minimapAlpha > dimmingAlpha)) {
                interfaceCtx->minimapAlpha = dimmingAlpha;
            }

            if (interfaceCtx->magicAlpha != 255) {
                interfaceCtx->magicAlpha = risingAlpha;
            }

            if (interfaceCtx->healthAlpha != 255) {
                interfaceCtx->healthAlpha = risingAlpha;
            }

            break;

        case HUD_VISIBILITY_B_ALT:
            if ((interfaceCtx->aAlpha != 0) && (interfaceCtx->aAlpha > dimmingAlpha)) {
                interfaceCtx->aAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->cLeftAlpha != 0) && (interfaceCtx->cLeftAlpha > dimmingAlpha)) {
                interfaceCtx->cLeftAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->cDownAlpha != 0) && (interfaceCtx->cDownAlpha > dimmingAlpha)) {
                interfaceCtx->cDownAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->cRightAlpha != 0) && (interfaceCtx->cRightAlpha > dimmingAlpha)) {
                interfaceCtx->cRightAlpha = dimmingAlpha;
            }
            
            // @mod
            for (int i = 0; i < EXTRA_ITEM_SLOT_COUNT; i++) {
                if ((extra_item_slot_alphas[i] != 0) && (extra_item_slot_alphas[i] > dimmingAlpha)) {
                    extra_item_slot_alphas[i] = dimmingAlpha;
                }
            }

            if ((interfaceCtx->healthAlpha != 0) && (interfaceCtx->healthAlpha > dimmingAlpha)) {
                interfaceCtx->healthAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->magicAlpha != 0) && (interfaceCtx->magicAlpha > dimmingAlpha)) {
                interfaceCtx->magicAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->minimapAlpha != 0) && (interfaceCtx->minimapAlpha > dimmingAlpha)) {
                interfaceCtx->minimapAlpha = dimmingAlpha;
            }

            if (interfaceCtx->bAlpha != 255) {
                interfaceCtx->bAlpha = risingAlpha;
            }

            break;

        case HUD_VISIBILITY_HEARTS:
            if ((interfaceCtx->bAlpha != 0) && (interfaceCtx->bAlpha > dimmingAlpha)) {
                interfaceCtx->bAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->aAlpha != 0) && (interfaceCtx->aAlpha > dimmingAlpha)) {
                interfaceCtx->aAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->cLeftAlpha != 0) && (interfaceCtx->cLeftAlpha > dimmingAlpha)) {
                interfaceCtx->cLeftAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->cDownAlpha != 0) && (interfaceCtx->cDownAlpha > dimmingAlpha)) {
                interfaceCtx->cDownAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->cRightAlpha != 0) && (interfaceCtx->cRightAlpha > dimmingAlpha)) {
                interfaceCtx->cRightAlpha = dimmingAlpha;
            }
            
            // @mod
            for (int i = 0; i < EXTRA_ITEM_SLOT_COUNT; i++) {
                if ((extra_item_slot_alphas[i] != 0) && (extra_item_slot_alphas[i] > dimmingAlpha)) {
                    extra_item_slot_alphas[i] = dimmingAlpha;
                }
            }

            if ((interfaceCtx->minimapAlpha != 0) && (interfaceCtx->minimapAlpha > dimmingAlpha)) {
                interfaceCtx->minimapAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->magicAlpha != 0) && (interfaceCtx->magicAlpha > dimmingAlpha)) {
                interfaceCtx->magicAlpha = dimmingAlpha;
            }

            if (interfaceCtx->healthAlpha != 255) {
                interfaceCtx->healthAlpha = risingAlpha;
            }

            break;

        case HUD_VISIBILITY_A_B_MINIMAP:
            if (interfaceCtx->aAlpha != 255) {
                interfaceCtx->aAlpha = risingAlpha;
            }

            if ((gSaveContext.buttonStatus[EQUIP_SLOT_B] == BTN_DISABLED) ||
                (gSaveContext.bButtonStatus == ITEM_NONE)) {
                if (interfaceCtx->bAlpha != 70) {
                    interfaceCtx->bAlpha = 70;
                }
            } else {
                if (interfaceCtx->bAlpha != 255) {
                    interfaceCtx->bAlpha = risingAlpha;
                }
            }

            if (interfaceCtx->minimapAlpha != 255) {
                interfaceCtx->minimapAlpha = risingAlpha;
            }

            if ((interfaceCtx->cLeftAlpha != 0) && (interfaceCtx->cLeftAlpha > dimmingAlpha)) {
                interfaceCtx->cLeftAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->cDownAlpha != 0) && (interfaceCtx->cDownAlpha > dimmingAlpha)) {
                interfaceCtx->cDownAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->cRightAlpha != 0) && (interfaceCtx->cRightAlpha > dimmingAlpha)) {
                interfaceCtx->cRightAlpha = dimmingAlpha;
            }
            
            // @mod
            for (int i = 0; i < EXTRA_ITEM_SLOT_COUNT; i++) {
                if ((extra_item_slot_alphas[i] != 0) && (extra_item_slot_alphas[i] > dimmingAlpha)) {
                    extra_item_slot_alphas[i] = dimmingAlpha;
                }
            }

            if ((interfaceCtx->magicAlpha != 0) && (interfaceCtx->magicAlpha > dimmingAlpha)) {
                interfaceCtx->magicAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->healthAlpha != 0) && (interfaceCtx->healthAlpha > dimmingAlpha)) {
                interfaceCtx->healthAlpha = dimmingAlpha;
            }

            break;

        case HUD_VISIBILITY_HEARTS_MAGIC_WITH_OVERWRITE:
            Interface_UpdateButtonAlphas(play, dimmingAlpha, risingAlpha);

            if ((interfaceCtx->minimapAlpha != 0) && (interfaceCtx->minimapAlpha > dimmingAlpha)) {
                interfaceCtx->minimapAlpha = dimmingAlpha;
            }

            // aAlpha overwrites the value set in Interface_UpdateButtonAlphas
            if ((interfaceCtx->aAlpha != 0) && (interfaceCtx->aAlpha > dimmingAlpha)) {
                interfaceCtx->aAlpha = dimmingAlpha;
            }

            if (interfaceCtx->magicAlpha != 255) {
                interfaceCtx->magicAlpha = risingAlpha;
            }

            if (interfaceCtx->healthAlpha != 255) {
                interfaceCtx->healthAlpha = risingAlpha;
            }

            break;

        case HUD_VISIBILITY_HEARTS_MAGIC_C:
            if ((interfaceCtx->bAlpha != 0) && (interfaceCtx->bAlpha > dimmingAlpha)) {
                interfaceCtx->bAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->aAlpha != 0) && (interfaceCtx->aAlpha > dimmingAlpha)) {
                interfaceCtx->aAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->minimapAlpha != 0) && (interfaceCtx->minimapAlpha > dimmingAlpha)) {
                interfaceCtx->minimapAlpha = dimmingAlpha;
            }

            if (interfaceCtx->cLeftAlpha != 255) {
                interfaceCtx->cLeftAlpha = risingAlpha;
            }

            if (interfaceCtx->cDownAlpha != 255) {
                interfaceCtx->cDownAlpha = risingAlpha;
            }

            if (interfaceCtx->cRightAlpha != 255) {
                interfaceCtx->cRightAlpha = risingAlpha;
            }
            
            // @mod
            for (int i = 0; i < EXTRA_ITEM_SLOT_COUNT; i++) {
                if (extra_item_slot_alphas[i] != 255) {
                    extra_item_slot_alphas[i] = risingAlpha;
                }
            }

            if (interfaceCtx->magicAlpha != 255) {
                interfaceCtx->magicAlpha = risingAlpha;
            }

            if (interfaceCtx->healthAlpha != 255) {
                interfaceCtx->healthAlpha = risingAlpha;
            }

            break;

        case HUD_VISIBILITY_ALL_NO_MINIMAP:
            if ((interfaceCtx->minimapAlpha != 0) && (interfaceCtx->minimapAlpha > dimmingAlpha)) {
                interfaceCtx->minimapAlpha = dimmingAlpha;
            }

            if (interfaceCtx->bAlpha != 255) {
                interfaceCtx->bAlpha = risingAlpha;
            }

            if (interfaceCtx->aAlpha != 255) {
                interfaceCtx->aAlpha = risingAlpha;
            }

            if (interfaceCtx->cLeftAlpha != 255) {
                interfaceCtx->cLeftAlpha = risingAlpha;
            }

            if (interfaceCtx->cDownAlpha != 255) {
                interfaceCtx->cDownAlpha = risingAlpha;
            }

            if (interfaceCtx->cRightAlpha != 255) {
                interfaceCtx->cRightAlpha = risingAlpha;
            }
            
            // @mod
            for (int i = 0; i < EXTRA_ITEM_SLOT_COUNT; i++) {
                if (extra_item_slot_alphas[i] != 255) {
                    extra_item_slot_alphas[i] = risingAlpha;
                }
            }

            if (interfaceCtx->magicAlpha != 255) {
                interfaceCtx->magicAlpha = risingAlpha;
            }

            if (interfaceCtx->healthAlpha != 255) {
                interfaceCtx->healthAlpha = risingAlpha;
            }

            break;

        case HUD_VISIBILITY_A_B_C:
            if ((interfaceCtx->minimapAlpha != 0) && (interfaceCtx->minimapAlpha > dimmingAlpha)) {
                interfaceCtx->minimapAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->magicAlpha != 0) && (interfaceCtx->magicAlpha > dimmingAlpha)) {
                interfaceCtx->magicAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->healthAlpha != 0) && (interfaceCtx->healthAlpha > dimmingAlpha)) {
                interfaceCtx->healthAlpha = dimmingAlpha;
            }

            if (interfaceCtx->bAlpha != 255) {
                interfaceCtx->bAlpha = risingAlpha;
            }

            if (interfaceCtx->aAlpha != 255) {
                interfaceCtx->aAlpha = risingAlpha;
            }

            if (interfaceCtx->cLeftAlpha != 255) {
                interfaceCtx->cLeftAlpha = risingAlpha;
            }

            if (interfaceCtx->cDownAlpha != 255) {
                interfaceCtx->cDownAlpha = risingAlpha;
            }

            if (interfaceCtx->cRightAlpha != 255) {
                interfaceCtx->cRightAlpha = risingAlpha;
            }
            
            // @mod
            for (int i = 0; i < EXTRA_ITEM_SLOT_COUNT; i++) {
                if (extra_item_slot_alphas[i] != 255) {
                    extra_item_slot_alphas[i] = risingAlpha;
                }
            }

            break;

        case HUD_VISIBILITY_B_MINIMAP:
            if ((interfaceCtx->aAlpha != 0) && (interfaceCtx->aAlpha > dimmingAlpha)) {
                interfaceCtx->aAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->cLeftAlpha != 0) && (interfaceCtx->cLeftAlpha > dimmingAlpha)) {
                interfaceCtx->cLeftAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->cDownAlpha != 0) && (interfaceCtx->cDownAlpha > dimmingAlpha)) {
                interfaceCtx->cDownAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->cRightAlpha != 0) && (interfaceCtx->cRightAlpha > dimmingAlpha)) {
                interfaceCtx->cRightAlpha = dimmingAlpha;
            }
            
            // @mod
            for (int i = 0; i < EXTRA_ITEM_SLOT_COUNT; i++) {
                if ((extra_item_slot_alphas[i] != 0) && (extra_item_slot_alphas[i] > dimmingAlpha)) {
                    extra_item_slot_alphas[i] = dimmingAlpha;
                }
            }

            if ((interfaceCtx->magicAlpha != 0) && (interfaceCtx->magicAlpha > dimmingAlpha)) {
                interfaceCtx->magicAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->healthAlpha != 0) && (interfaceCtx->healthAlpha > dimmingAlpha)) {
                interfaceCtx->healthAlpha = dimmingAlpha;
            }

            if (interfaceCtx->bAlpha != 255) {
                interfaceCtx->bAlpha = risingAlpha;
            }

            if (interfaceCtx->minimapAlpha != 255) {
                interfaceCtx->minimapAlpha = risingAlpha;
            }

            break;

        case HUD_VISIBILITY_HEARTS_MAGIC_MINIMAP:
            if ((interfaceCtx->bAlpha != 0) && (interfaceCtx->bAlpha > dimmingAlpha)) {
                interfaceCtx->bAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->aAlpha != 0) && (interfaceCtx->aAlpha > dimmingAlpha)) {
                interfaceCtx->aAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->cLeftAlpha != 0) && (interfaceCtx->cLeftAlpha > dimmingAlpha)) {
                interfaceCtx->cLeftAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->cDownAlpha != 0) && (interfaceCtx->cDownAlpha > dimmingAlpha)) {
                interfaceCtx->cDownAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->cRightAlpha != 0) && (interfaceCtx->cRightAlpha > dimmingAlpha)) {
                interfaceCtx->cRightAlpha = dimmingAlpha;
            }
            
            // @mod
            for (int i = 0; i < EXTRA_ITEM_SLOT_COUNT; i++) {
                if ((extra_item_slot_alphas[i] != 0) && (extra_item_slot_alphas[i] > dimmingAlpha)) {
                    extra_item_slot_alphas[i] = dimmingAlpha;
                }
            }

            if (interfaceCtx->healthAlpha != 255) {
                interfaceCtx->healthAlpha = risingAlpha;
            }

            if (interfaceCtx->magicAlpha != 255) {
                interfaceCtx->magicAlpha = risingAlpha;
            }

            if (interfaceCtx->minimapAlpha != 255) {
                interfaceCtx->minimapAlpha = risingAlpha;
            }

            break;

        case HUD_VISIBILITY_A_HEARTS_MAGIC_MINIMAP:
            if ((interfaceCtx->bAlpha != 0) && (interfaceCtx->bAlpha > dimmingAlpha)) {
                interfaceCtx->bAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->cLeftAlpha != 0) && (interfaceCtx->cLeftAlpha > dimmingAlpha)) {
                interfaceCtx->cLeftAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->cDownAlpha != 0) && (interfaceCtx->cDownAlpha > dimmingAlpha)) {
                interfaceCtx->cDownAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->cRightAlpha != 0) && (interfaceCtx->cRightAlpha > dimmingAlpha)) {
                interfaceCtx->cRightAlpha = dimmingAlpha;
            }
            
            // @mod
            for (int i = 0; i < EXTRA_ITEM_SLOT_COUNT; i++) {
                if ((extra_item_slot_alphas[i] != 0) && (extra_item_slot_alphas[i] > dimmingAlpha)) {
                    extra_item_slot_alphas[i] = dimmingAlpha;
                }
            }

            if (interfaceCtx->aAlpha != 255) {
                interfaceCtx->aAlpha = risingAlpha;
            }

            if (interfaceCtx->minimapAlpha != 255) {
                interfaceCtx->minimapAlpha = risingAlpha;
            }

            if (interfaceCtx->magicAlpha != 255) {
                interfaceCtx->magicAlpha = risingAlpha;
            }

            if (interfaceCtx->healthAlpha != 255) {
                interfaceCtx->healthAlpha = risingAlpha;
            }

            break;

        case HUD_VISIBILITY_B_MAGIC:
            if ((interfaceCtx->aAlpha != 0) && (interfaceCtx->aAlpha > dimmingAlpha)) {
                interfaceCtx->aAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->cLeftAlpha != 0) && (interfaceCtx->cLeftAlpha > dimmingAlpha)) {
                interfaceCtx->cLeftAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->cDownAlpha != 0) && (interfaceCtx->cDownAlpha > dimmingAlpha)) {
                interfaceCtx->cDownAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->cRightAlpha != 0) && (interfaceCtx->cRightAlpha > dimmingAlpha)) {
                interfaceCtx->cRightAlpha = dimmingAlpha;
            }
            
            // @mod
            for (int i = 0; i < EXTRA_ITEM_SLOT_COUNT; i++) {
                if ((extra_item_slot_alphas[i] != 0) && (extra_item_slot_alphas[i] > dimmingAlpha)) {
                    extra_item_slot_alphas[i] = dimmingAlpha;
                }
            }

            if ((interfaceCtx->minimapAlpha != 0) && (interfaceCtx->minimapAlpha > dimmingAlpha)) {
                interfaceCtx->minimapAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->healthAlpha != 0) && (interfaceCtx->healthAlpha > dimmingAlpha)) {
                interfaceCtx->healthAlpha = dimmingAlpha;
            }

            if (interfaceCtx->bAlpha != 255) {
                interfaceCtx->bAlpha = risingAlpha;
            }

            if (interfaceCtx->magicAlpha != 255) {
                interfaceCtx->magicAlpha = risingAlpha;
            }

            break;

        case HUD_VISIBILITY_A_B:
            if (interfaceCtx->aAlpha != 255) {
                interfaceCtx->aAlpha = risingAlpha;
            }

            if (interfaceCtx->bAlpha != 255) {
                interfaceCtx->bAlpha = risingAlpha;
            }

            if ((interfaceCtx->cLeftAlpha != 0) && (interfaceCtx->cLeftAlpha > dimmingAlpha)) {
                interfaceCtx->cLeftAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->cDownAlpha != 0) && (interfaceCtx->cDownAlpha > dimmingAlpha)) {
                interfaceCtx->cDownAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->cRightAlpha != 0) && (interfaceCtx->cRightAlpha > dimmingAlpha)) {
                interfaceCtx->cRightAlpha = dimmingAlpha;
            }
            
            // @mod
            for (int i = 0; i < EXTRA_ITEM_SLOT_COUNT; i++) {
                if ((extra_item_slot_alphas[i] != 0) && (extra_item_slot_alphas[i] > dimmingAlpha)) {
                    extra_item_slot_alphas[i] = dimmingAlpha;
                }
            }

            if ((interfaceCtx->minimapAlpha != 0) && (interfaceCtx->minimapAlpha > dimmingAlpha)) {
                interfaceCtx->minimapAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->magicAlpha != 0) && (interfaceCtx->magicAlpha > dimmingAlpha)) {
                interfaceCtx->magicAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->healthAlpha != 0) && (interfaceCtx->healthAlpha > dimmingAlpha)) {
                interfaceCtx->healthAlpha = dimmingAlpha;
            }

            break;

        case HUD_VISIBILITY_A_B_HEARTS_MAGIC_MINIMAP:
            if ((interfaceCtx->cLeftAlpha != 0) && (interfaceCtx->cLeftAlpha > dimmingAlpha)) {
                interfaceCtx->cLeftAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->cDownAlpha != 0) && (interfaceCtx->cDownAlpha > dimmingAlpha)) {
                interfaceCtx->cDownAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->cRightAlpha != 0) && (interfaceCtx->cRightAlpha > dimmingAlpha)) {
                interfaceCtx->cRightAlpha = dimmingAlpha;
            }
            
            // @mod
            for (int i = 0; i < EXTRA_ITEM_SLOT_COUNT; i++) {
                if ((extra_item_slot_alphas[i] != 0) && (extra_item_slot_alphas[i] > dimmingAlpha)) {
                    extra_item_slot_alphas[i] = dimmingAlpha;
                }
            }

            if (interfaceCtx->bAlpha != 255) {
                interfaceCtx->bAlpha = risingAlpha;
            }

            if (interfaceCtx->aAlpha != 255) {
                interfaceCtx->aAlpha = risingAlpha;
            }

            if (interfaceCtx->minimapAlpha != 255) {
                interfaceCtx->minimapAlpha = risingAlpha;
            }

            if (interfaceCtx->magicAlpha != 255) {
                interfaceCtx->magicAlpha = risingAlpha;
            }

            if (interfaceCtx->healthAlpha != 255) {
                interfaceCtx->healthAlpha = risingAlpha;
            }

            break;
    }

    if ((play->roomCtx.curRoom.behaviorType1 == ROOM_BEHAVIOR_TYPE1_1) && (interfaceCtx->minimapAlpha >= 255)) {
        interfaceCtx->minimapAlpha = 255;
    }
}

// Set the enabled status when the set item slot statuses event is fired.
RECOMP_CALLBACK("*", recomp_set_extra_item_slot_statuses) void on_set_slot_statuses(PlayState* play, s32 enabled) {
    dpad_refresh_mode();

    if (!dpad_items_enabled()) {
        enabled = BTN_DISABLED;
    }

    for (int i = 0; i < EXTRA_ITEM_SLOT_COUNT; i++) {
        extra_item_slot_statuses[i] = enabled;
    }
}

#define GET_CUR_FORM_BTN_ITEM_EX(btn) (extra_button_items[CUR_FORM][btn])

// Update the D-Pad item slots in the same way that the base game updates the C-Button item slots.
// Logic copied from Interface_UpdateButtonsPart2.
RECOMP_HOOK("Interface_UpdateButtonsPart2") void on_update_buttons_part2(PlayState* play) {
    MessageContext* msgCtx = &play->msgCtx;
    InterfaceContext* interfaceCtx = &play->interfaceCtx;
    Player* player = GET_PLAYER(play);
    s16 i;
    s16 restoreHudVisibility = false;
    dpad_refresh_mode();

    if (!dpad_items_enabled()) {
        for (i = 0; i < EXTRA_ITEM_SLOT_COUNT; i++) {
            extra_item_slot_statuses[i] = BTN_DISABLED;
            extra_item_slot_alphas[i] = 0;
        }
        return;
    }

    if (CHECK_EVENTINF(EVENTINF_41)) {
        // Related to swamp boat (non-minigame)?
        for (i = 0; i < EXTRA_ITEM_SLOT_COUNT; i++) {
            if ((GET_CUR_FORM_BTN_ITEM_EX(i) != ITEM_PICTOGRAPH_BOX) || (msgCtx->msgMode != MSGMODE_NONE)) {
                if (extra_item_slot_statuses[i] == BTN_ENABLED) {
                    restoreHudVisibility = true;
                }
                extra_item_slot_statuses[i] = BTN_DISABLED;
            } else {
                if (extra_item_slot_statuses[i] == BTN_DISABLED) {
                    restoreHudVisibility = true;
                }
                extra_item_slot_statuses[i] = BTN_ENABLED;
            }
        }

        // @mod B-Button Handled by the base recomp.
    } else if (CHECK_WEEKEVENTREG(WEEKEVENTREG_90_20)) {
        // Fishermans's jumping minigame
        for (i = 0; i < EXTRA_ITEM_SLOT_COUNT; i++) {
            if (extra_item_slot_statuses[i] == BTN_ENABLED) {
                extra_item_slot_statuses[i] = BTN_DISABLED;
            }
        }

        // @mod HUD visibility handled by the base recomp.
    } else if (CHECK_WEEKEVENTREG(WEEKEVENTREG_82_08)) {
        // Swordsman's log minigame
        for (i = 0; i < EXTRA_ITEM_SLOT_COUNT; i++) {
            if (extra_item_slot_statuses[i] == BTN_ENABLED) {
                extra_item_slot_statuses[i] = BTN_DISABLED;
            }
        }

        // @mod HUD visibility handled by the base recomp.
    } else if (CHECK_WEEKEVENTREG(WEEKEVENTREG_84_20)) {
        // Related to moon child
        if (player->currentMask == PLAYER_MASK_FIERCE_DEITY) {
            for (i = 0; i < EXTRA_ITEM_SLOT_COUNT; i++) {
                if ((GET_CUR_FORM_BTN_ITEM_EX(i) == ITEM_MASK_FIERCE_DEITY) ||
                    ((GET_CUR_FORM_BTN_ITEM_EX(i) >= ITEM_BOTTLE) && (GET_CUR_FORM_BTN_ITEM_EX(i) <= ITEM_OBABA_DRINK))) {
                    if (extra_item_slot_statuses[i] == BTN_DISABLED) {
                        restoreHudVisibility = true;
                        extra_item_slot_statuses[i] = BTN_ENABLED;
                    }
                } else {
                    if (extra_item_slot_statuses[i] != BTN_DISABLED) {
                        extra_item_slot_statuses[i] = BTN_DISABLED;
                        restoreHudVisibility = true;
                    }
                }
            }
        } else {
            for (i = 0; i < EXTRA_ITEM_SLOT_COUNT; i++) {
                if ((GET_CUR_FORM_BTN_ITEM_EX(i) >= ITEM_MASK_DEKU) && (GET_CUR_FORM_BTN_ITEM_EX(i) <= ITEM_MASK_ZORA)) {
                    if (extra_item_slot_statuses[i] != BTN_DISABLED) {
                        restoreHudVisibility = true;
                    }
                    extra_item_slot_statuses[i] = BTN_DISABLED;
                } else {
                    if (extra_item_slot_statuses[i] == BTN_DISABLED) {
                        restoreHudVisibility = true;
                    }
                    extra_item_slot_statuses[i] = BTN_ENABLED;
                }
            }
        }
    } else if ((play->sceneId == SCENE_SPOT00) && (gSaveContext.sceneLayer == 6)) {
        // Unknown cutscene
        for (i = 0; i < EXTRA_ITEM_SLOT_COUNT; i++) {
            if (extra_item_slot_statuses[i] == BTN_ENABLED) {
                restoreHudVisibility = true;
            }
            extra_item_slot_statuses[i] = BTN_DISABLED;
        }
    } else if (CHECK_EVENTINF(EVENTINF_34)) {
        // Deku playground minigame
        if (player->stateFlags3 & PLAYER_STATE3_1000000) {
            // @mod Handled by the base recomp.
        } else {
            for (i = 0; i < EXTRA_ITEM_SLOT_COUNT; i++) {
                if (extra_item_slot_statuses[i] == BTN_ENABLED) {
                    restoreHudVisibility = true;
                }
                extra_item_slot_statuses[i] = BTN_DISABLED;
            }
        }

        // @mod This HUD visibility case needs to be handled by this mod as well to account for the extra item slots.
        if (restoreHudVisibility || (gSaveContext.hudVisibility != HUD_VISIBILITY_A_B_MINIMAP)) {
            gSaveContext.hudVisibility = HUD_VISIBILITY_IDLE;
            Interface_SetHudVisibility(HUD_VISIBILITY_A_B_MINIMAP);
            restoreHudVisibility = false;
        }
    } else if (player->stateFlags3 & PLAYER_STATE3_1000000) {
        // Nuts on B (from flying as Deku Link)
        // @mod B-Button Handled by the base recomp.
    } else if (!gSaveContext.save.saveInfo.playerData.isMagicAcquired && (CUR_FORM == PLAYER_FORM_DEKU) &&
               (BUTTON_ITEM_EQUIP(CUR_FORM, EQUIP_SLOT_B) == ITEM_DEKU_NUT)) {
        // Nuts on B (as Deku Link)
        // @mod B-Button Handled by the base recomp.
    } else if ((Player_GetEnvironmentalHazard(play) >= PLAYER_ENV_HAZARD_UNDERWATER_FLOOR) &&
               (Player_GetEnvironmentalHazard(play) <= PLAYER_ENV_HAZARD_UNDERWATER_FREE)) {
        // Swimming underwater
        // @mod B-Button Handled by the base recomp.

        for (i = 0; i < EXTRA_ITEM_SLOT_COUNT; i++) {
            if (GET_CUR_FORM_BTN_ITEM_EX(i) != ITEM_MASK_ZORA) {
                if (Player_GetEnvironmentalHazard(play) == PLAYER_ENV_HAZARD_UNDERWATER_FLOOR) {
                    if (!((GET_CUR_FORM_BTN_ITEM_EX(i) >= ITEM_BOTTLE) &&
                          (GET_CUR_FORM_BTN_ITEM_EX(i) <= ITEM_OBABA_DRINK))) {
                        if (extra_item_slot_statuses[i] == BTN_ENABLED) {
                            restoreHudVisibility = true;
                        }
                        extra_item_slot_statuses[i] = BTN_DISABLED;
                    } else {
                        if (extra_item_slot_statuses[i] == BTN_DISABLED) {
                            restoreHudVisibility = true;
                        }
                        extra_item_slot_statuses[i] = BTN_ENABLED;
                    }
                } else {
                    if (extra_item_slot_statuses[i] == BTN_ENABLED) {
                        restoreHudVisibility = true;
                    }
                    extra_item_slot_statuses[i] = BTN_DISABLED;
                }
            } else if (extra_item_slot_statuses[i] == BTN_DISABLED) {
                extra_item_slot_statuses[i] = BTN_ENABLED;
                restoreHudVisibility = true;
            }
        }

        // @mod This HUD visibility case needs to be handled by this mod as well to account for the extra item slots.
        if (restoreHudVisibility) {
            gSaveContext.hudVisibility = HUD_VISIBILITY_IDLE;
        }

        if ((play->transitionTrigger == TRANS_TRIGGER_OFF) && (play->transitionMode == TRANS_MODE_OFF)) {
            if (CutsceneManager_GetCurrentCsId() == CS_ID_NONE) {
                Interface_SetHudVisibility(HUD_VISIBILITY_ALL);
            }
        }
    } else if (player->stateFlags1 & PLAYER_STATE1_200000) {
        // First person view
        for (i = 0; i < EXTRA_ITEM_SLOT_COUNT; i++) {
            if (GET_CUR_FORM_BTN_ITEM_EX(i) != ITEM_LENS_OF_TRUTH) {
                if (extra_item_slot_statuses[i] == BTN_ENABLED) {
                    restoreHudVisibility = true;
                }
                extra_item_slot_statuses[i] = BTN_DISABLED;
            } else {
                if (extra_item_slot_statuses[i] == BTN_DISABLED) {
                    restoreHudVisibility = true;
                }
                extra_item_slot_statuses[i] = BTN_ENABLED;
            }
        }

        // @mod B-Button Handled by the base recomp.
    } else if (player->stateFlags1 & PLAYER_STATE1_2000) {
        // Hanging from a ledge
        // @mod Handled by the base recomp.
    } else {
        // End of special event cases

        // B button
        // @mod B-Button Handled by the base recomp.

        // C buttons
        if (GET_PLAYER_FORM == player->transformation) {
            for (i = 0; i < EXTRA_ITEM_SLOT_COUNT; i++) {
                // Individual C button
                if (!gPlayerFormItemRestrictions[GET_PLAYER_FORM][GET_CUR_FORM_BTN_ITEM_EX(i)]) {
                    // Item not usable in current playerForm
                    if (extra_item_slot_statuses[i] != BTN_DISABLED) {
                        extra_item_slot_statuses[i] = BTN_DISABLED;
                        restoreHudVisibility = true;
                    }
                } else if (player->actor.id != ACTOR_PLAYER) {
                    // Currently not playing as the main player
                    if (extra_item_slot_statuses[i] != BTN_DISABLED) {
                        extra_item_slot_statuses[i] = BTN_DISABLED;
                        restoreHudVisibility = true;
                    }
                } else if (player->currentMask == PLAYER_MASK_GIANT) {
                    // Currently wearing Giant's Mask
                    if (GET_CUR_FORM_BTN_ITEM_EX(i) != ITEM_MASK_GIANT) {
                        if (extra_item_slot_statuses[i] != BTN_DISABLED) {
                            extra_item_slot_statuses[i] = BTN_DISABLED;
                            restoreHudVisibility = true;
                        }
                    } else if (extra_item_slot_statuses[i] == BTN_DISABLED) {
                        restoreHudVisibility = true;
                        extra_item_slot_statuses[i] = BTN_ENABLED;
                    }
                } else if (GET_CUR_FORM_BTN_ITEM_EX(i) == ITEM_MASK_GIANT) {
                    // Giant's Mask is equipped
                    if (play->sceneId != SCENE_INISIE_BS) {
                        if (extra_item_slot_statuses[i] != BTN_DISABLED) {
                            extra_item_slot_statuses[i] = BTN_DISABLED;
                            restoreHudVisibility = true;
                        }
                    } else if (extra_item_slot_statuses[i] == BTN_DISABLED) {
                        restoreHudVisibility = true;
                        extra_item_slot_statuses[i] = BTN_ENABLED;
                    }
                } else if (GET_CUR_FORM_BTN_ITEM_EX(i) == ITEM_MASK_FIERCE_DEITY) {
                    // Fierce Deity's Mask is equipped
                    if ((play->sceneId != SCENE_MITURIN_BS) && (play->sceneId != SCENE_HAKUGIN_BS) &&
                        (play->sceneId != SCENE_SEA_BS) && (play->sceneId != SCENE_INISIE_BS) &&
                        (play->sceneId != SCENE_LAST_BS)) {
                        if (extra_item_slot_statuses[i] != BTN_DISABLED) {
                            extra_item_slot_statuses[i] = BTN_DISABLED;
                            restoreHudVisibility = true;
                        }
                    } else if (extra_item_slot_statuses[i] == BTN_DISABLED) {
                        restoreHudVisibility = true;
                        extra_item_slot_statuses[i] = BTN_ENABLED;
                    }
                } else {
                    // End of special item cases. Apply restrictions to buttons
                    if (interfaceCtx->restrictions.tradeItems != 0) {
                        if (((GET_CUR_FORM_BTN_ITEM_EX(i) >= ITEM_MOONS_TEAR) &&
                             (GET_CUR_FORM_BTN_ITEM_EX(i) <= ITEM_PENDANT_OF_MEMORIES)) ||
                            ((GET_CUR_FORM_BTN_ITEM_EX(i) >= ITEM_BOTTLE) &&
                             (GET_CUR_FORM_BTN_ITEM_EX(i) <= ITEM_OBABA_DRINK)) ||
                            (GET_CUR_FORM_BTN_ITEM_EX(i) == ITEM_OCARINA_OF_TIME)) {
                            if (extra_item_slot_statuses[i] == BTN_ENABLED) {
                                restoreHudVisibility = true;
                            }
                            extra_item_slot_statuses[i] = BTN_DISABLED;
                        }
                    } else if (interfaceCtx->restrictions.tradeItems == 0) {
                        if (((GET_CUR_FORM_BTN_ITEM_EX(i) >= ITEM_MOONS_TEAR) &&
                             (GET_CUR_FORM_BTN_ITEM_EX(i) <= ITEM_PENDANT_OF_MEMORIES)) ||
                            ((GET_CUR_FORM_BTN_ITEM_EX(i) >= ITEM_BOTTLE) &&
                             (GET_CUR_FORM_BTN_ITEM_EX(i) <= ITEM_OBABA_DRINK)) ||
                            (GET_CUR_FORM_BTN_ITEM_EX(i) == ITEM_OCARINA_OF_TIME)) {
                            if (extra_item_slot_statuses[i] == BTN_DISABLED) {
                                restoreHudVisibility = true;
                            }
                            extra_item_slot_statuses[i] = BTN_ENABLED;
                        }
                    }

                    if (interfaceCtx->restrictions.masks != 0) {
                        if ((GET_CUR_FORM_BTN_ITEM_EX(i) >= ITEM_MASK_DEKU) &&
                            (GET_CUR_FORM_BTN_ITEM_EX(i) <= ITEM_MASK_GIANT)) {
                            if (!extra_item_slot_statuses[i]) { // == BTN_ENABLED
                                restoreHudVisibility = true;
                            }
                            extra_item_slot_statuses[i] = BTN_DISABLED;
                        }
                    } else if (interfaceCtx->restrictions.masks == 0) {
                        if ((GET_CUR_FORM_BTN_ITEM_EX(i) >= ITEM_MASK_DEKU) &&
                            (GET_CUR_FORM_BTN_ITEM_EX(i) <= ITEM_MASK_GIANT)) {
                            if (extra_item_slot_statuses[i] == BTN_DISABLED) {
                                restoreHudVisibility = true;
                            }
                            extra_item_slot_statuses[i] = BTN_ENABLED;
                        }
                    }

                    if (interfaceCtx->restrictions.pictoBox != 0) {
                        if (GET_CUR_FORM_BTN_ITEM_EX(i) == ITEM_PICTOGRAPH_BOX) {
                            if (!extra_item_slot_statuses[i]) { // == BTN_ENABLED
                                restoreHudVisibility = true;
                            }
                            extra_item_slot_statuses[i] = BTN_DISABLED;
                        }
                    } else if (interfaceCtx->restrictions.pictoBox == 0) {
                        if (GET_CUR_FORM_BTN_ITEM_EX(i) == ITEM_PICTOGRAPH_BOX) {
                            if (extra_item_slot_statuses[i] == BTN_DISABLED) {
                                restoreHudVisibility = true;
                            }
                            extra_item_slot_statuses[i] = BTN_ENABLED;
                        }
                    }

                    if (interfaceCtx->restrictions.all != 0) {
                        if (!((GET_CUR_FORM_BTN_ITEM_EX(i) >= ITEM_MOONS_TEAR) &&
                              (GET_CUR_FORM_BTN_ITEM_EX(i) <= ITEM_PENDANT_OF_MEMORIES)) &&
                            !((GET_CUR_FORM_BTN_ITEM_EX(i) >= ITEM_BOTTLE) &&
                              (GET_CUR_FORM_BTN_ITEM_EX(i) <= ITEM_OBABA_DRINK)) &&
                            (GET_CUR_FORM_BTN_ITEM_EX(i) != ITEM_OCARINA_OF_TIME) &&
                            !((GET_CUR_FORM_BTN_ITEM_EX(i) >= ITEM_MASK_DEKU) &&
                              (GET_CUR_FORM_BTN_ITEM_EX(i) <= ITEM_MASK_GIANT)) &&
                            (GET_CUR_FORM_BTN_ITEM_EX(i) != ITEM_PICTOGRAPH_BOX)) {

                            if (extra_item_slot_statuses[i] == BTN_ENABLED) {
                                restoreHudVisibility = true;
                                extra_item_slot_statuses[i] = BTN_DISABLED;
                            }
                        }
                    } else if (interfaceCtx->restrictions.all == 0) {
                        if (!((GET_CUR_FORM_BTN_ITEM_EX(i) >= ITEM_MOONS_TEAR) &&
                              (GET_CUR_FORM_BTN_ITEM_EX(i) <= ITEM_PENDANT_OF_MEMORIES)) &&
                            !((GET_CUR_FORM_BTN_ITEM_EX(i) >= ITEM_BOTTLE) &&
                              (GET_CUR_FORM_BTN_ITEM_EX(i) <= ITEM_OBABA_DRINK)) &&
                            (GET_CUR_FORM_BTN_ITEM_EX(i) != ITEM_OCARINA_OF_TIME) &&
                            !((GET_CUR_FORM_BTN_ITEM_EX(i) >= ITEM_MASK_DEKU) &&
                              (GET_CUR_FORM_BTN_ITEM_EX(i) <= ITEM_MASK_GIANT)) &&
                            (GET_CUR_FORM_BTN_ITEM_EX(i) != ITEM_PICTOGRAPH_BOX)) {

                            if (extra_item_slot_statuses[i] == BTN_DISABLED) {
                                restoreHudVisibility = true;
                                extra_item_slot_statuses[i] = BTN_ENABLED;
                            }
                        }
                    }
                }
            }
        }
    }

    // @mod This HUD visibility case needs to be handled by this mod as well to account for the extra item slots.
    if (restoreHudVisibility && (play->activeCamId == CAM_ID_MAIN) && (play->transitionTrigger == TRANS_TRIGGER_OFF) &&
        (play->transitionMode == TRANS_MODE_OFF)) {
        gSaveContext.hudVisibility = HUD_VISIBILITY_IDLE;
        Interface_SetHudVisibility(HUD_VISIBILITY_ALL);
    }
}
