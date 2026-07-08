#include "interface_helper.h"

#define INCBIN(identifier, filename)          \
    asm(".pushsection .rodata\n"              \
        "\t.globl " #identifier "\n"          \
        "\t.type " #identifier ", @object\n"  \
        "\t.balign 8\n"                       \
        #identifier ":\n"                     \
        "\t.incbin \"" filename "\"\n\n"      \
                                              \
        "\t.balign 8\n"                       \
        "\t.popsection\n");                   \
    extern u8 identifier[]

RECOMP_IMPORT("*", u32 recomp_get_config_u32(const char* key));
RECOMP_IMPORT("*", u32 recomp_android_compat_mco_enabled(void));

// Function is used to determine what the correct glyph texture is.
extern TexturePtr gButtonBackgroundTex[];

INCBIN(tBGlyph, "compat_mco/textures/ia8/b_empty_icon.ia8.bin");
INCBIN(tAGlyph, "compat_mco/textures/ia8/a_empty_icon.ia8.bin");

const char BUTTON_CONFIGS[5][15] = {"attack_button", "1_button", "2_button", "3_button", "action_button"};

TexturePtr Mod_GlyphTexture(EquipSlot button) {
    switch (recomp_get_config_u32(BUTTON_CONFIGS[button])) {
        case 0: // Up
            return (u8*)gButtonBackgroundTex + ((32 * 32) * (EQUIP_SLOT_C_DOWN + 1));
            break;
        case 1: // Left
            return (u8*)gButtonBackgroundTex + ((32 * 32) * (EQUIP_SLOT_C_LEFT + 1));
            break;
        case 2: // Down
            return tBGlyph;
            break;
        case 3: // Right
            return tAGlyph;
            break;
        case 4: // Shoulder
            return (u8*)gButtonBackgroundTex + ((32 * 32) * (EQUIP_SLOT_C_RIGHT + 1));
            break;
    }

    return NULL;
}

extern s16 sMaskCButtonPosX[3];
extern s16 sMaskCButtonPosY[3];
extern s16 sCButtonPosX[3];
extern s16 sCButtonPosY[3];
AMMO_POSITIONS_TENS_X_DECLARE
AMMO_POSITIONS_TENS_Y_DECLARE
AMMO_POSITIONS_ONES_X_DECLARE
AMMO_POSITIONS_ONES_Y_DECLARE
BUTTON_POSITIONS_X_DECLARE
BUTTON_POSITIONS_Y_DECLARE
C_GLYPHS_ENABLED_DECLARE
C_GLYPH_TEXTURES_DECLARE
C_GLYPH_POSITIONS_X_DECLARE
C_GLYPH_POSITIONS_Y_DECLARE
START_BUTTON_ENABLED_DECLARE
START_LABEL_ENABLED_DECLARE
C_UP_BUTTON_POSITION_X_DECLARE
C_UP_LABEL_POSITION_X_DECLARE
B_BUTTON_DRAWN_DECLARE
ITEM_ICON_POSITIONS_X_DECLARE
ITEM_ICON_POSITIONS_Y_DECLARE
B_ITEM_EQUIP_POSITION_X_DECLARE
B_ITEM_EQUIP_POSITION_Y_DECLARE
A_ITEM_EQUIP_POSITION_X_DECLARE
A_ITEM_EQUIP_POSITION_Y_DECLARE

RECOMP_CALLBACK("*", recomp_after_play_init) void after_play_init() {
    AMMO_POSITIONS_TENS_X_REGISTER
    AMMO_POSITIONS_TENS_Y_REGISTER
    AMMO_POSITIONS_ONES_X_REGISTER
    AMMO_POSITIONS_ONES_Y_REGISTER
    BUTTON_POSITIONS_X_REGISTER
    BUTTON_POSITIONS_Y_REGISTER
    C_GLYPHS_ENABLED_REGISTER
    C_GLYPH_TEXTURES_REGISTER
    C_GLYPH_POSITIONS_X_REGISTER
    C_GLYPH_POSITIONS_Y_REGISTER
    START_BUTTON_ENABLED_REGISTER
    START_LABEL_ENABLED_REGISTER
    C_UP_BUTTON_POSITION_X_REGISTER
    C_UP_LABEL_POSITION_X_REGISTER
    B_BUTTON_DRAWN_REGISTER
    ITEM_ICON_POSITIONS_X_REGISTER
    ITEM_ICON_POSITIONS_Y_REGISTER
    B_ITEM_EQUIP_POSITION_X_REGISTER
    B_ITEM_EQUIP_POSITION_Y_REGISTER
    A_ITEM_EQUIP_POSITION_X_REGISTER
    A_ITEM_EQUIP_POSITION_Y_REGISTER

    if (!recomp_android_compat_mco_enabled()) {
        return;
    }
    
    (*pStartButtonEnabled) = false;
    (*pStartLabelEnabled) = false;

    (*pCUpButtonPositionX) = 251;
    (*pCUpLabelPositionX) = 965;
}

int vShoulderOffset;

// Set here so that config changes have immediate effect.
RECOMP_HOOK("Interface_DrawCButtonIcons") void Interface_DrawCButtonIcons_Init(PlayState* play) {
    if (!recomp_android_compat_mco_enabled()) {
        return;
    }

    // Calculate the offset that should be applied to the shoulder Y position based on user's config.
    vShoulderOffset = (recomp_get_config_u32("shoulder_position") - 1) * -3;
    
    // Move UI elements as per config
    for (int index = EQUIP_SLOT_B; index <= EQUIP_SLOT_C_RIGHT; index++ ) {
        if (index == EQUIP_SLOT_B) {
            // Attack button only shenanigans
            switch (recomp_get_config_u32(BUTTON_CONFIGS[index])) {
                case 0: // Up
                    (*pButtonPositionsX)[index] = 218;                         // Button X
                    (*pButtonPositionsY)[index] = 14;                          // Button Y
                    
                    (*pBItemEquipPositionX) = 550;                             // Item Equip X
                    (*pBItemEquipPositionY) = 1150;                            // Item Equip Y
                    break;
                case 1: // Left
                    (*pButtonPositionsX)[index] = 193;                         // Button X
                    (*pButtonPositionsY)[index] = 34;                          // Button Y
                    
                    (*pBItemEquipPositionX) = 350;                             // Item Equip X
                    (*pBItemEquipPositionY) = 950;                             // Item Equip Y
                    break;
                case 2: // Down
                    (*pButtonPositionsX)[index] = 218;                         // Button X
                    (*pButtonPositionsY)[index] = 54;                          // Button Y
                    
                    (*pBItemEquipPositionX) = 550;                             // Item Equip X
                    (*pBItemEquipPositionY) = 700;                             // Item Equip Y
                    break;
                case 3: // Right
                    (*pButtonPositionsX)[index] = 245;                         // Button X
                    (*pButtonPositionsY)[index] = 34;                          // Button Y
                    
                    (*pBItemEquipPositionX) = 815;                             // Item Equip X
                    (*pBItemEquipPositionY) = 950;                             // Item Equip Y
                    break;
                case 4: // Shoulder
                    (*pButtonPositionsX)[index] = 270;                         // Button X
                    (*pButtonPositionsY)[index] = 14 + vShoulderOffset;        // Button Y
                    
                    (*pBItemEquipPositionX) = 1140;                            // Item Equip X
                    (*pBItemEquipPositionY) = 1150 - (vShoulderOffset * 16);   // Item Equip Y
                    break;
            }
            
            // Set Attack ammo drawing positions to same place relative to button position
            (*pAmmoPositionsTensX)[index] = (*pButtonPositionsX)[index] + 2;
            (*pAmmoPositionsTensY)[index] = (*pButtonPositionsY)[index] + 19;
            
        } else {
            // C-button only shenanigans
            int cIndex = index - 1;
            
            // Move C-buttons as per config
            switch (recomp_get_config_u32(BUTTON_CONFIGS[index])) {
                case 0: // Up
                    (*pButtonPositionsX)[index] = 219;                      // Button X
                    (*pButtonPositionsY)[index] = 15;                       // Button Y
                    
                    sCButtonPosX[cIndex] = 550;                             // Item Equip X
                    sCButtonPosY[cIndex] = 1150;                            // Item Equip Y
                    break;
                case 1: // Left
                    (*pButtonPositionsX)[index] = 194;                      // Button X
                    (*pButtonPositionsY)[index] = 35;                       // Button Y
                    
                    sCButtonPosX[cIndex] = 350;                             // Item Equip X
                    sCButtonPosY[cIndex] = 950;                             // Item Equip Y
                    break;
                case 2: // Down
                    (*pButtonPositionsX)[index] = 219;                      // Button X
                    (*pButtonPositionsY)[index] = 55;                       // Button Y
                    
                    sCButtonPosX[cIndex] = 550;                             // Item Equip X
                    sCButtonPosY[cIndex] = 700;                             // Item Equip Y
                    break;
                case 3: // Right
                    (*pButtonPositionsX)[index] = 246;                      // Button X
                    (*pButtonPositionsY)[index] = 35;                       // Button Y
                    
                    sCButtonPosX[cIndex] = 815;                             // Item Equip X
                    sCButtonPosY[cIndex] = 950;                             // Item Equip Y
                    break;
                case 4: // Shoulder
                    (*pButtonPositionsX)[index] = 271;                      // Button X
                    (*pButtonPositionsY)[index] = 15 + vShoulderOffset;     // Button Y
                    
                    sCButtonPosX[cIndex] = 1140;                            // Item Equip X
                    sCButtonPosY[cIndex] = 1150 - (vShoulderOffset * 16);   // Item Equip Y
                    break;
            }
            
            // Disable C glyph code being run at all to avoid callbacks being erroneously ran
            if (recomp_get_config_u32("glyphs") == 0) {
                (*pCGlyphsEnabled)[cIndex] = false;
            } else {
                (*pCGlyphsEnabled)[cIndex] = true;
                // Only change glyph texture when glyph is visible, no point in doing it otherwise
                if (GET_CUR_FORM_BTN_ITEM(index) > 0xF0) {
                    (*pCGlyphTextures)[cIndex] = Mod_GlyphTexture(index);
                }
            }

            // Set glyph drawing positions to same place as button drawing positions
            (*pCGlyphPositionsX)[cIndex] = (*pButtonPositionsX)[index];
            (*pCGlyphPositionsY)[cIndex] = (*pButtonPositionsY)[index];
            
            // Set ammo positions to same place relative to button positions
            (*pAmmoPositionsTensX)[index] = (*pButtonPositionsX)[index] + 1;
            (*pAmmoPositionsTensY)[index] = (*pButtonPositionsY)[index] + 17;
            
            // Set mask equip destination to same place as item equip destination
            sMaskCButtonPosX[cIndex] = sCButtonPosX[cIndex];
            sMaskCButtonPosY[cIndex] = sCButtonPosY[cIndex];
        }
        
        // Set ammo one positions to be next to ammo tens
        (*pAmmoPositionsOnesX)[index] = (*pAmmoPositionsTensX)[index] + 6;
        (*pAmmoPositionsOnesY)[index] = (*pAmmoPositionsTensY)[index];
        
        // Set item icon drawing positions to the same positions as the buttons
        (*pItemIconPositionsX)[index] = (*pButtonPositionsX)[index] << 2;
        (*pItemIconPositionsY)[index] = (*pButtonPositionsY)[index] << 2;
    }
    
    // Makes sure the Action button's offset is always zero - it doesn't need to be changed with this mod.
    R_A_BTN_Y_OFFSET = 0;
    
    if (*pBButtonDrawn == false && recomp_get_config_u32("glyphs") == 2) {
        OPEN_DISPS(play->state.gfxCtx);
        
        // Draw Attack button glyph.
        gDPPipeSync(OVERLAY_DISP++);
        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 100, 255, 120, (&play->interfaceCtx)->bAlpha);
        OVERLAY_DISP = Gfx_DrawTexRectIA8(OVERLAY_DISP, Mod_GlyphTexture(EQUIP_SLOT_B),
            0x20, 0x20, (*pButtonPositionsX)[0], (*pButtonPositionsY)[0], 0x1D,
            0x1D, ((s32)(1.1230469f * (1 << 10)) >> 1) * 2, ((s32)(1.1230469f * (1 << 10)) >> 1) * 2);

        CLOSE_DISPS(play->state.gfxCtx);
    }
}
