#include "modding.h"
#include "global.h"

RECOMP_IMPORT("*", u32 recomp_get_config_u32(const char* key));
RECOMP_IMPORT("*", u32 recomp_android_compat_mco_enabled(void));

// Only the Action button uses Interface_SetPerspectiveView. Hook into this, save the PlayState.
PlayState* bPlayState;

RECOMP_HOOK("Interface_SetPerspectiveView") void Interface_SetPerspectiveView_Init(PlayState* play, s32 topY, s32 bottomY, s32 leftX, s32 rightX) {
    if (!recomp_android_compat_mco_enabled()) {
        return;
    }

    bPlayState = play;
}

extern s16 (*pAItemEquipPositionX);
extern s16 (*pAItemEquipPositionY);
extern int vShoulderOffset;

RECOMP_HOOK("View_SetViewport") void View_SetViewport_Init(View* view, Viewport* viewport) {
    if (!recomp_android_compat_mco_enabled() || bPlayState == NULL) {
        return;
    }

    InterfaceContext* interfaceCtx = &bPlayState->interfaceCtx;

    // If this function is being called for the Action button, move the viewport by the correct values.
    if (&interfaceCtx->view == view && &interfaceCtx->viewport == viewport) {
        switch (recomp_get_config_u32("action_button")) {
            case 0: // Up
                viewport->topY -= 17;
                viewport->bottomY -= 17;
                viewport->leftX += 18;
                viewport->rightX += 18;

                (*pAItemEquipPositionX) = 550;                             // Item Equip X
                (*pAItemEquipPositionY) = 1150;                            // Item Equip Y
                break;
            case 1: // Left
                viewport->topY += 3;
                viewport->bottomY += 3;
                viewport->leftX -= 7;
                viewport->rightX -= 7;

                (*pAItemEquipPositionX) = 350;                             // Item Equip X
                (*pAItemEquipPositionY) = 950;                             // Item Equip Y
                break;
            case 2: // Down
                viewport->topY += 23;
                viewport->bottomY += 23;
                viewport->leftX += 18;
                viewport->rightX += 18;

                (*pAItemEquipPositionX) = 550;                             // Item Equip X
                (*pAItemEquipPositionY) = 700;                             // Item Equip Y
                break;
            case 3: // Right
                viewport->topY += 3;
                viewport->bottomY += 3;
                viewport->leftX += 45;
                viewport->rightX += 45;

                (*pAItemEquipPositionX) = 815;                             // Item Equip X
                (*pAItemEquipPositionY) = 950;                             // Item Equip Y
                break;
            case 4: // Shoulder
                viewport->topY -= 17 - vShoulderOffset;
                viewport->bottomY -= 17 - vShoulderOffset;
                viewport->leftX += 70;
                viewport->rightX += 70;

                (*pAItemEquipPositionX) = 1140;                            // Item Equip X
                (*pAItemEquipPositionY) = 1150 - (vShoulderOffset * 16);   // Item Equip Y
                break;
        }
    }
}

extern f32 sAButtonDoActionTexScales[2];

// Draw Action button glyph.
extern TexturePtr Mod_GlyphTexture(EquipSlot button);

u8 inAButton = 0;

RECOMP_HOOK("Interface_DrawAButton") void Interface_DrawAButton_Init(PlayState* play) {
    if (!recomp_android_compat_mco_enabled()) {
        return;
    }

    inAButton = 1;
}

RECOMP_HOOK("Matrix_Translate") void Matrix_Translate_Init(f32 x, f32 y, f32 z, MatrixMode mode) {
    if (!recomp_android_compat_mco_enabled() || bPlayState == NULL) {
        return;
    }

    InterfaceContext* interfaceCtx = &bPlayState->interfaceCtx;

    if (inAButton) {
        inAButton++;
    }

    if (inAButton == 3) {
        if (interfaceCtx->aButtonDoAction == DO_ACTION_NONE && recomp_get_config_u32("glyphs") == 2) {
            OPEN_DISPS(bPlayState->state.gfxCtx);

            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 100, 200, 255, interfaceCtx->aAlpha);
            OVERLAY_DISP = Gfx_DrawTexQuadIA8(OVERLAY_DISP, Mod_GlyphTexture(EQUIP_SLOT_A), 32, 32, 0);

            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, interfaceCtx->aAlpha);

            CLOSE_DISPS(bPlayState->state.gfxCtx);
        }
        inAButton = 0;
    }
}
