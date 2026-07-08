#ifndef __INTERFACE_HELPER__
#define __INTERFACE_HELPER__

// Interface Helper header v1.1.0

#include "modding.h"
#include "global.h"

#define AMMO_FORCE_MAX_COLOUR_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void AmmoForceMaxColour_Register(bool (**pointer))); \
bool (*pAmmoForceMaxColour);

#define AMMO_FORCE_MAX_COLOUR_REGISTER AmmoForceMaxColour_Register(&pAmmoForceMaxColour);

#define AMMO_FULL_COLOUR_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void AmmoFullColour_Register(int (**pointer)[3])); \
int (*pAmmoFullColour)[3];

#define AMMO_FULL_COLOUR_REGISTER AmmoFullColour_Register(&pAmmoFullColour);

#define AMMO_EMPTY_COLOUR_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void AmmoEmptyColour_Register(int (**pointer)[3])); \
int (*pAmmoEmptyColour)[3];

#define AMMO_EMPTY_COLOUR_REGISTER AmmoEmptyColour_Register(&pAmmoEmptyColour);

#define AMMO_TEXTURE_TENS_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void AmmoTextureTens_Register(TexturePtr (**pointer))); \
TexturePtr (*pAmmoTextureTens);

#define AMMO_TEXTURE_TENS_REGISTER AmmoTextureTens_Register(&pAmmoTextureTens);

#define AMMO_TEXTURE_WIDTH_TENS_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void AmmoTextureWidthTens_Register(s16 (**pointer))); \
s16 (*pAmmoTextureWidthTens);

#define AMMO_TEXTURE_WIDTH_TENS_REGISTER AmmoTextureWidthTens_Register(&pAmmoTextureWidthTens);

#define AMMO_TEXTURE_HEIGHT_TENS_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void AmmoTextureHeightTens_Register(s16 (**pointer))); \
s16 (*pAmmoTextureHeightTens);

#define AMMO_TEXTURE_HEIGHT_TENS_REGISTER AmmoTextureHeightTens_Register(&pAmmoTextureHeightTens);

#define AMMO_POSITIONS_TENS_X_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void AmmoPositionsTensX_Register(s16 (**pointer)[4])); \
s16 (*pAmmoPositionsTensX)[4];

#define AMMO_POSITIONS_TENS_X_REGISTER AmmoPositionsTensX_Register(&pAmmoPositionsTensX);

#define AMMO_POSITIONS_TENS_Y_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void AmmoPositionsTensY_Register(s16 (**pointer)[4])); \
s16 (*pAmmoPositionsTensY)[4];

#define AMMO_POSITIONS_TENS_Y_REGISTER AmmoPositionsTensY_Register(&pAmmoPositionsTensY);

#define AMMO_RECT_SIZE_TENS_X_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void AmmoRectSizeTensX_Register(s16 (**pointer)[4])); \
s16 (*pAmmoRectSizeTensX)[4];

#define AMMO_RECT_SIZE_TENS_X_REGISTER AmmoRectSizeTensX_Register(&pAmmoRectSizeTensX);

#define AMMO_RECT_SIZE_TENS_Y_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void AmmoRectSizeTensY_Register(s16 (**pointer)[4])); \
s16 (*pAmmoRectSizeTensY)[4];

#define AMMO_RECT_SIZE_TENS_Y_REGISTER AmmoRectSizeTensY_Register(&pAmmoRectSizeTensY);

#define AMMO_UPPER_SCALE_TENS_X_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void AmmoUpperScaleTensX_Register(u16 (**pointer))); \
u16 (*pAmmoUpperScaleTensX);

#define AMMO_UPPER_SCALE_TENS_X_REGISTER AmmoUpperScaleTensX_Register(&pAmmoUpperScaleTensX);

#define AMMO_UPPER_SCALE_TENS_Y_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void AmmoUpperScaleTensY_Register(u16 (**pointer))); \
u16 (*pAmmoUpperScaleTensY);

#define AMMO_UPPER_SCALE_TENS_Y_REGISTER AmmoUpperScaleTensY_Register(&pAmmoUpperScaleTensY);

#define AMMO_LOWER_SCALE_TENS_X_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void AmmoLowerScaleTensX_Register(u16 (**pointer))); \
u16 (*pAmmoLowerScaleTensX);

#define AMMO_LOWER_SCALE_TENS_X_REGISTER AmmoLowerScaleTensX_Register(&pAmmoLowerScaleTensX);

#define AMMO_LOWER_SCALE_TENS_Y_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void AmmoLowerScaleTensY_Register(u16 (**pointer))); \
u16 (*pAmmoLowerScaleTensY);

#define AMMO_LOWER_SCALE_TENS_Y_REGISTER AmmoLowerScaleTensY_Register(&pAmmoLowerScaleTensY);

#define AMMO_TEXTURE_ONES_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void AmmoTextureOnes_Register(TexturePtr (**pointer))); \
TexturePtr (*pAmmoTextureOnes);

#define AMMO_TEXTURE_ONES_REGISTER AmmoTextureOnes_Register(&pAmmoTextureOnes);

#define AMMO_TEXTURE_WIDTH_ONES_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void AmmoTextureWidthOnes_Register(s16 (**pointer))); \
s16 (*pAmmoTextureWidthOnes);

#define AMMO_TEXTURE_WIDTH_ONES_REGISTER AmmoTextureWidthOnes_Register(&pAmmoTextureWidthOnes);

#define AMMO_TEXTURE_HEIGHT_ONES_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void AmmoTextureHeightOnes_Register(s16 (**pointer))); \
s16 (*pAmmoTextureHeightOnes);

#define AMMO_TEXTURE_HEIGHT_ONES_REGISTER AmmoTextureHeightOnes_Register(&pAmmoTextureHeightOnes);

#define AMMO_POSITIONS_ONES_X_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void AmmoPositionsOnesX_Register(s16 (**pointer)[4])); \
s16 (*pAmmoPositionsOnesX)[4];

#define AMMO_POSITIONS_ONES_X_REGISTER AmmoPositionsOnesX_Register(&pAmmoPositionsOnesX);

#define AMMO_POSITIONS_ONES_Y_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void AmmoPositionsOnesY_Register(s16 (**pointer)[4])); \
s16 (*pAmmoPositionsOnesY)[4];

#define AMMO_POSITIONS_ONES_Y_REGISTER AmmoPositionsOnesY_Register(&pAmmoPositionsOnesY);

#define AMMO_RECT_SIZE_ONES_X_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void AmmoRectSizeOnesX_Register(s16 (**pointer)[4])); \
s16 (*pAmmoRectSizeOnesX)[4];

#define AMMO_RECT_SIZE_ONES_X_REGISTER AmmoRectSizeOnesX_Register(&pAmmoRectSizeOnesX);

#define AMMO_RECT_SIZE_ONES_Y_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void AmmoRectSizeOnesY_Register(s16 (**pointer)[4])); \
s16 (*pAmmoRectSizeOnesY)[4];

#define AMMO_RECT_SIZE_ONES_Y_REGISTER AmmoRectSizeOnesY_Register(&pAmmoRectSizeOnesY);

#define AMMO_UPPER_SCALE_ONES_X_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void AmmoUpperScaleOnesX_Register(u16 (**pointer))); \
u16 (*pAmmoUpperScaleOnesX);

#define AMMO_UPPER_SCALE_ONES_X_REGISTER AmmoUpperScaleOnesX_Register(&pAmmoUpperScaleOnesX);

#define AMMO_UPPER_SCALE_ONES_Y_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void AmmoUpperScaleOnesY_Register(u16 (**pointer))); \
u16 (*pAmmoUpperScaleOnesY);

#define AMMO_UPPER_SCALE_ONES_Y_REGISTER AmmoUpperScaleOnesY_Register(&pAmmoUpperScaleOnesY);

#define AMMO_LOWER_SCALE_ONES_X_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void AmmoLowerScaleOnesX_Register(u16 (**pointer))); \
u16 (*pAmmoLowerScaleOnesX);

#define AMMO_LOWER_SCALE_ONES_X_REGISTER AmmoLowerScaleOnesX_Register(&pAmmoLowerScaleOnesX);

#define AMMO_LOWER_SCALE_ONES_Y_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void AmmoLowerScaleOnesY_Register(u16 (**pointer))); \
u16 (*pAmmoLowerScaleOnesY);

#define AMMO_LOWER_SCALE_ONES_Y_REGISTER AmmoLowerScaleOnesY_Register(&pAmmoLowerScaleOnesY);

#define B_BUTTON_DRAWN_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void BButtonDrawn_Register(bool (**pointer))); \
bool (*pBButtonDrawn);

#define B_BUTTON_DRAWN_REGISTER BButtonDrawn_Register(&pBButtonDrawn);

#define B_BUTTON_ENABLED_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void BButtonEnabled_Register(bool (**pointer)[8])); \
bool (*pBButtonEnabled)[8];

#define B_BUTTON_ENABLED_REGISTER BButtonEnabled_Register(&pBButtonEnabled);

#define BUTTONS_ENABLED_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void ButtonsEnabled_Register(bool (**pointer)[4])); \
bool (*pButtonsEnabled)[4];

#define BUTTONS_ENABLED_REGISTER ButtonsEnabled_Register(&pButtonsEnabled);

#define BUTTON_TEXTURES_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void ButtonTextures_Register(TexturePtr (**pointer)[4])); \
TexturePtr (*pButtonTextures)[4];

#define BUTTON_TEXTURES_REGISTER ButtonTextures_Register(&pButtonTextures);

#define BUTTON_TEXTURES_WIDTH_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void ButtonTexturesWidth_Register(s16 (**pointer)[4])); \
s16 (*pButtonTexturesWidth)[4];

#define BUTTON_TEXTURES_WIDTH_REGISTER ButtonTexturesWidth_Register(&pButtonTexturesWidth);

#define BUTTON_TEXTURES_HEIGHT_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void ButtonTexturesHeight_Register(s16 (**pointer)[4])); \
s16 (*pButtonTexturesHeight)[4];

#define BUTTON_TEXTURES_HEIGHT_REGISTER ButtonTexturesHeight_Register(&pButtonTexturesHeight);

#define BUTTON_POSITIONS_X_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void ButtonPositionsX_Register(s16 (**pointer)[4])); \
s16 (*pButtonPositionsX)[4];

#define BUTTON_POSITIONS_X_REGISTER ButtonPositionsX_Register(&pButtonPositionsX);

#define BUTTON_POSITIONS_Y_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void ButtonPositionsY_Register(s16 (**pointer)[4])); \
s16 (*pButtonPositionsY)[4];

#define BUTTON_POSITIONS_Y_REGISTER ButtonPositionsY_Register(&pButtonPositionsY);

#define BUTTON_RECT_SIZES_X_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void ButtonRectSizesX_Register(s16 (**pointer)[4])); \
s16 (*pButtonRectSizesX)[4];

#define BUTTON_RECT_SIZES_X_REGISTER ButtonRectSizesX_Register(&pButtonRectSizesX);

#define BUTTON_RECT_SIZES_Y_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void ButtonRectSizesY_Register(s16 (**pointer)[4])); \
s16 (*pButtonRectSizesY)[4];

#define BUTTON_RECT_SIZES_Y_REGISTER ButtonRectSizesY_Register(&pButtonRectSizesY);

#define BUTTON_SCALES_X_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void ButtonScalesX_Register(s16 (**pointer)[4])); \
s16 (*pButtonScalesX)[4];

#define BUTTON_SCALES_X_REGISTER ButtonScalesX_Register(&pButtonScalesX);

#define BUTTON_SCALES_Y_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void ButtonScalesY_Register(s16 (**pointer)[4])); \
s16 (*pButtonScalesY)[4];

#define BUTTON_SCALES_Y_REGISTER ButtonScalesY_Register(&pButtonScalesY);

#define BUTTON_COLOURS_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void ButtonColours_Register(s16 (**pointer)[4][3])); \
s16 (*pButtonColours)[4][3];

#define BUTTON_COLOURS_REGISTER ButtonColours_Register(&pButtonColours);

#define BUTTON_ALPHAS_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void ButtonAlphas_Register(s16 (**pointer)[4])); \
s16 (*pButtonAlphas)[4];

#define BUTTON_ALPHAS_REGISTER ButtonAlphas_Register(&pButtonAlphas);

#define START_FORCE_ENABLED_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void StartForceEnabled_Register(bool (**pointer))); \
bool (*pStartForceEnabled);

#define START_FORCE_ENABLED_REGISTER StartForceEnabled_Register(&pStartForceEnabled);

#define START_BUTTON_ENABLED_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void StartButtonEnabled_Register(bool (**pointer))); \
bool (*pStartButtonEnabled);

#define START_BUTTON_ENABLED_REGISTER StartButtonEnabled_Register(&pStartButtonEnabled);

#define START_BUTTON_TEXTURE_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void StartButtonTexture_Register(TexturePtr (**pointer))); \
TexturePtr (*pStartButtonTexture);

#define START_BUTTON_TEXTURE_REGISTER StartButtonTexture_Register(&pStartButtonTexture);

#define START_BUTTON_TEXTURE_WIDTH_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void StartButtonTextureWidth_Register(s16 (**pointer))); \
s16 (*pStartButtonTextureWidth);

#define START_BUTTON_TEXTURE_WIDTH_REGISTER StartButtonTextureWidth_Register(&pStartButtonTextureWidth);

#define START_BUTTON_TEXTURE_HEIGHT_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void StartButtonTextureHeight_Register(s16 (**pointer))); \
s16 (*pStartButtonTextureHeight);

#define START_BUTTON_TEXTURE_HEIGHT_REGISTER StartButtonTextureHeight_Register(&pStartButtonTextureHeight);

#define START_BUTTON_POSITION_X_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void StartButtonPositionX_Register(s16 (**pointer))); \
s16 (*pStartButtonPositionX);

#define START_BUTTON_POSITION_X_REGISTER StartButtonPositionX_Register(&pStartButtonPositionX);

#define START_BUTTON_POSITION_Y_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void StartButtonPositionY_Register(s16 (**pointer))); \
s16 (*pStartButtonPositionY);

#define START_BUTTON_POSITION_Y_REGISTER StartButtonPositionY_Register(&pStartButtonPositionY);

#define START_BUTTON_RECT_SIZE_X_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void StartButtonRectSizeX_Register(s16 (**pointer))); \
s16 (*pStartButtonRectSizeX);

#define START_BUTTON_RECT_SIZE_X_REGISTER StartButtonRectSizeX_Register(&pStartButtonRectSizeX);

#define START_BUTTON_RECT_SIZE_Y_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void StartButtonRectSizeY_Register(s16 (**pointer))); \
s16 (*pStartButtonRectSizeY);

#define START_BUTTON_RECT_SIZE_Y_REGISTER StartButtonRectSizeY_Register(&pStartButtonRectSizeY);

#define START_BUTTON_SCALE_X_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void StartButtonScaleX_Register(u16 (**pointer))); \
u16 (*pStartButtonScaleX);

#define START_BUTTON_SCALE_X_REGISTER StartButtonScaleX_Register(&pStartButtonScaleX);

#define START_BUTTON_SCALE_Y_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void StartButtonScaleY_Register(u16 (**pointer))); \
u16 (*pStartButtonScaleY);

#define START_BUTTON_SCALE_Y_REGISTER StartButtonScaleY_Register(&pStartButtonScaleY);

#define START_BUTTON_COLOUR_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void StartButtonColour_Register(s16 (**pointer)[3])); \
s16 (*pStartButtonColour)[3];

#define START_BUTTON_COLOUR_REGISTER StartButtonColour_Register(&pStartButtonColour);

#define START_LABEL_ENABLED_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void StartLabelEnabled_Register(bool (**pointer))); \
bool (*pStartLabelEnabled);

#define START_LABEL_ENABLED_REGISTER StartLabelEnabled_Register(&pStartLabelEnabled);

#define START_LABEL_PRIM_COLOUR_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void StartLabelPrimColour_Register(int (**pointer)[3])); \
int (*pStartLabelPrimColour)[3];

#define START_LABEL_PRIM_COLOUR_REGISTER StartLabelPrimColour_Register(&pStartLabelPrimColour);

#define START_LABEL_ENV_COLOUR_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void StartLabelEnvColour_Register(int (**pointer)[4])); \
int (*pStartLabelEnvColour)[4];

#define START_LABEL_ENV_COLOUR_REGISTER StartLabelEnvColour_Register(&pStartLabelEnvColour);

#define START_LABEL_TEXTURE_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void StartLabelTexture_Register(TexturePtr (**pointer))); \
TexturePtr (*pStartLabelTexture);

#define START_LABEL_TEXTURE_REGISTER StartLabelTexture_Register(&pStartLabelTexture);

#define START_LABEL_TEXTURE_WIDTH_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void StartLabelTextureWidth_Register(int (**pointer))); \
int (*pStartLabelTextureWidth);

#define START_LABEL_TEXTURE_WIDTH_REGISTER StartLabelTextureWidth_Register(&pStartLabelTextureWidth);

#define START_LABEL_TEXTURE_HEIGHT_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void StartLabelTextureHeight_Register(int (**pointer))); \
int (*pStartLabelTextureHeight);

#define START_LABEL_TEXTURE_HEIGHT_REGISTER StartLabelTextureHeight_Register(&pStartLabelTextureHeight);

#define START_LABEL_POSITION_X_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void StartLabelPositionX_Register(int (**pointer))); \
int (*pStartLabelPositionX);

#define START_LABEL_POSITION_X_REGISTER StartLabelPositionX_Register(&pStartLabelPositionX);

#define START_LABEL_POSITION_Y_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void StartLabelPositionY_Register(int (**pointer))); \
int (*pStartLabelPositionY);

#define START_LABEL_POSITION_Y_REGISTER StartLabelPositionY_Register(&pStartLabelPositionY);

#define START_LABEL_RECT_SIZE_X_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void StartLabelRectSizeX_Register(int (**pointer))); \
int (*pStartLabelRectSizeX);

#define START_LABEL_RECT_SIZE_X_REGISTER StartLabelRectSizeX_Register(&pStartLabelRectSizeX);

#define START_LABEL_RECT_SIZE_Y_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void StartLabelRectSizeY_Register(int (**pointer))); \
int (*pStartLabelRectSizeY);

#define START_LABEL_RECT_SIZE_Y_REGISTER StartLabelRectSizeY_Register(&pStartLabelRectSizeY);

#define START_LABEL_SCALE_X_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void StartLabelScaleX_Register(int (**pointer))); \
int (*pStartLabelScaleX);

#define START_LABEL_SCALE_X_REGISTER StartLabelScaleX_Register(&pStartLabelScaleX);

#define START_LABEL_SCALE_Y_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void StartLabelScaleY_Register(int (**pointer))); \
int (*pStartLabelScaleY);

#define START_LABEL_SCALE_Y_REGISTER StartLabelScaleY_Register(&pStartLabelScaleY);

#define C_UP_BUTTON_FORCE_ENABLED_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void CUpButtonForceEnabled_Register(bool (**pointer))); \
bool (*pCUpButtonForceEnabled);

#define C_UP_BUTTON_FORCE_ENABLED_REGISTER CUpButtonForceEnabled_Register(&pCUpButtonForceEnabled);

#define C_UP_BUTTON_ENABLED_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void CUpButtonEnabled_Register(bool (**pointer))); \
bool (*pCUpButtonEnabled);

#define C_UP_BUTTON_ENABLED_REGISTER CUpButtonEnabled_Register(&pCUpButtonEnabled);

#define C_UP_BUTTON_TEXTURE_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void CUpButtonTexture_Register(TexturePtr (**pointer))); \
TexturePtr (*pCUpButtonTexture);

#define C_UP_BUTTON_TEXTURE_REGISTER CUpButtonTexture_Register(&pCUpButtonTexture);

#define C_UP_BUTTON_TEXTURE_WIDTH_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void CUpButtonTextureWidth_Register(int (**pointer))); \
int (*pCUpButtonTextureWidth);

#define C_UP_BUTTON_TEXTURE_WIDTH_REGISTER CUpButtonTextureWidth_Register(&pCUpButtonTextureWidth);

#define C_UP_BUTTON_TEXTURE_HEIGHT_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void CUpButtonTextureHeight_Register(int (**pointer))); \
int (*pCUpButtonTextureHeight);

#define C_UP_BUTTON_TEXTURE_HEIGHT_REGISTER CUpButtonTextureHeight_Register(&pCUpButtonTextureHeight);

#define C_UP_BUTTON_POSITION_X_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void CUpButtonPositionX_Register(s16 (**pointer))); \
s16 (*pCUpButtonPositionX);

#define C_UP_BUTTON_POSITION_X_REGISTER CUpButtonPositionX_Register(&pCUpButtonPositionX);

#define C_UP_BUTTON_POSITION_Y_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void CUpButtonPositionY_Register(s16 (**pointer))); \
s16 (*pCUpButtonPositionY);

#define C_UP_BUTTON_POSITION_Y_REGISTER CUpButtonPositionY_Register(&pCUpButtonPositionY);

#define C_UP_BUTTON_RECT_SIZE_X_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void CUpButtonRectSizeX_Register(s16 (**pointer))); \
s16 (*pCUpButtonRectSizeX);

#define C_UP_BUTTON_RECT_SIZE_X_REGISTER CUpButtonRectSizeX_Register(&pCUpButtonRectSizeX);

#define C_UP_BUTTON_RECT_SIZE_Y_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void CUpButtonRectSizeY_Register(s16 (**pointer))); \
s16 (*pCUpButtonRectSizeY);

#define C_UP_BUTTON_RECT_SIZE_Y_REGISTER CUpButtonRectSizeY_Register(&pCUpButtonRectSizeY);

#define C_UP_BUTTON_SCALE_X_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void CUpButtonScaleX_Register(u16 (**pointer))); \
u16 (*pCUpButtonScaleX);

#define C_UP_BUTTON_SCALE_X_REGISTER CUpButtonScaleX_Register(&pCUpButtonScaleX);

#define C_UP_BUTTON_SCALE_Y_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void CUpButtonScaleY_Register(u16 (**pointer))); \
u16 (*pCUpButtonScaleY);

#define C_UP_BUTTON_SCALE_Y_REGISTER CUpButtonScaleY_Register(&pCUpButtonScaleY);

#define C_UP_BUTTON_COLOURS_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void CUpButtonColours_Register(s16 (**pointer)[3])); \
s16 (*pCUpButtonColours)[3];

#define C_UP_BUTTON_COLOURS_REGISTER CUpButtonColours_Register(&pCUpButtonColours);

#define C_UP_LABEL_ENABLED_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void CUpLabelEnabled_Register(bool (**pointer))); \
bool (*pCUpLabelEnabled);

#define C_UP_LABEL_ENABLED_REGISTER CUpLabelEnabled_Register(&pCUpLabelEnabled);

#define C_UP_LABEL_PRIM_COLOUR_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void CUpLabelPrimColour_Register(int (**pointer)[3])); \
int (*pCUpLabelPrimColour)[3];

#define C_UP_LABEL_PRIM_COLOUR_REGISTER CUpLabelPrimColour_Register(&pCUpLabelPrimColour);

#define C_UP_LABEL_ENV_COLOUR_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void CUpLabelEnvColour_Register(int (**pointer)[4])); \
int (*pCUpLabelEnvColour)[4];

#define C_UP_LABEL_ENV_COLOUR_REGISTER CUpLabelEnvColour_Register(&pCUpLabelEnvColour);

#define C_UP_LABEL_TEXTURES_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void CUpLabelTextures_Register(TexturePtr (**pointer)[LANGUAGE_MAX])); \
TexturePtr (*pCUpLabelTextures)[LANGUAGE_MAX];

#define C_UP_LABEL_TEXTURES_REGISTER CUpLabelTextures_Register(&pCUpLabelTextures);

#define C_UP_LABEL_TEXTURE_WIDTHS_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void CUpLabelTextureWidths_Register(int (**pointer)[LANGUAGE_MAX])); \
int (*pCUpLabelTextureWidths)[LANGUAGE_MAX];

#define C_UP_LABEL_TEXTURE_WIDTHS_REGISTER CUpLabelTextureWidths_Register(&pCUpLabelTextureWidths);

#define C_UP_LABEL_TEXTURE_HEIGHTS_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void CUpLabelTextureHeights_Register(int (**pointer)[LANGUAGE_MAX])); \
int (*pCUpLabelTextureHeights)[LANGUAGE_MAX];

#define C_UP_LABEL_TEXTURE_HEIGHTS_REGISTER CUpLabelTextureHeights_Register(&pCUpLabelTextureHeights);

#define C_UP_LABEL_POSITION_X_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void CUpLabelPositionX_Register(int (**pointer))); \
int (*pCUpLabelPositionX);

#define C_UP_LABEL_POSITION_X_REGISTER CUpLabelPositionX_Register(&pCUpLabelPositionX);

#define C_UP_LABEL_POSITION_Y_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void CUpLabelPositionY_Register(int (**pointer))); \
int (*pCUpLabelPositionY);

#define C_UP_LABEL_POSITION_Y_REGISTER CUpLabelPositionY_Register(&pCUpLabelPositionY);

#define C_UP_LABEL_RECT_SIZE_X_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void CUpLabelRectSizeX_Register(int (**pointer))); \
int (*pCUpLabelRectSizeX);

#define C_UP_LABEL_RECT_SIZE_X_REGISTER CUpLabelRectSizeX_Register(&pCUpLabelRectSizeX);

#define C_UP_LABEL_RECT_SIZE_Y_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void CUpLabelRectSizeY_Register(int (**pointer))); \
int (*pCUpLabelRectSizeY);

#define C_UP_LABEL_RECT_SIZE_Y_REGISTER CUpLabelRectSizeY_Register(&pCUpLabelRectSizeY);

#define C_UP_LABEL_SCALE_X_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void CUpLabelScaleX_Register(int (**pointer))); \
int (*pCUpLabelScaleX);

#define C_UP_LABEL_SCALE_X_REGISTER CUpLabelScaleX_Register(&pCUpLabelScaleX);

#define C_UP_LABEL_SCALE_Y_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void CUpLabelScaleY_Register(int (**pointer))); \
int (*pCUpLabelScaleY);

#define C_UP_LABEL_SCALE_Y_REGISTER CUpLabelScaleY_Register(&pCUpLabelScaleY);

#define C_GLYPHS_ENABLED_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void CGlyphsEnabled_Register(bool (**pointer)[3])); \
bool (*pCGlyphsEnabled)[3];

#define C_GLYPHS_ENABLED_REGISTER CGlyphsEnabled_Register(&pCGlyphsEnabled);

#define C_GLYPH_TEXTURES_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void CGlyphTextures_Register(TexturePtr (**pointer)[3])); \
TexturePtr (*pCGlyphTextures)[3];

#define C_GLYPH_TEXTURES_REGISTER CGlyphTextures_Register(&pCGlyphTextures);

#define C_GLYPH_TEXTURE_WIDTHS_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void CGlyphTextureWidths_Register(s16 (**pointer)[3])); \
s16 (*pCGlyphTextureWidths)[3];

#define C_GLYPH_TEXTURE_WIDTHS_REGISTER CGlyphTextureWidths_Register(&pCGlyphTextureWidths);

#define C_GLYPH_TEXTURE_HEIGHTS_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void CGlyphTextureHeights_Register(s16 (**pointer)[3])); \
s16 (*pCGlyphTextureHeights)[3];

#define C_GLYPH_TEXTURE_HEIGHTS_REGISTER CGlyphTextureHeights_Register(&pCGlyphTextureHeights);

#define C_GLYPH_POSITIONS_X_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void CGlyphPositionsX_Register(s16 (**pointer)[3])); \
s16 (*pCGlyphPositionsX)[3];

#define C_GLYPH_POSITIONS_X_REGISTER CGlyphPositionsX_Register(&pCGlyphPositionsX);

#define C_GLYPH_POSITIONS_Y_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void CGlyphPositionsY_Register(s16 (**pointer)[3])); \
s16 (*pCGlyphPositionsY)[3];

#define C_GLYPH_POSITIONS_Y_REGISTER CGlyphPositionsY_Register(&pCGlyphPositionsY);

#define C_GLYPH_RECT_SIZES_X_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void CGlyphRectSizesX_Register(s16 (**pointer)[3])); \
s16 (*pCGlyphRectSizesX)[3];

#define C_GLYPH_RECT_SIZES_X_REGISTER CGlyphRectSizesX_Register(&pCGlyphRectSizesX);

#define C_GLYPH_RECT_SIZES_Y_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void CGlyphRectSizesY_Register(s16 (**pointer)[3])); \
s16 (*pCGlyphRectSizesY)[3];

#define C_GLYPH_RECT_SIZES_Y_REGISTER CGlyphRectSizesY_Register(&pCGlyphRectSizesY);

#define C_GLYPH_SCALES_X_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void CGlyphScalesX_Register(s16 (**pointer)[3])); \
s16 (*pCGlyphScalesX)[3];

#define C_GLYPH_SCALES_X_REGISTER CGlyphScalesX_Register(&pCGlyphScalesX);

#define C_GLYPH_SCALES_Y_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void CGlyphScalesY_Register(s16 (**pointer)[3])); \
s16 (*pCGlyphScalesY)[3];

#define C_GLYPH_SCALES_Y_REGISTER CGlyphScalesY_Register(&pCGlyphScalesY);

#define C_GLYPH_COLOURS_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void CGlyphColours_Register(s16 (**pointer)[3][3])); \
s16 (*pCGlyphColours)[3][3];

#define C_GLYPH_COLOURS_REGISTER CGlyphColours_Register(&pCGlyphColours);

#define ITEM_ICON_TEXTURE_OVERRIDE_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void ItemIconTextureOverride_Register(TexturePtr (**pointer))); \
TexturePtr (*pItemIconTextureOverride);

#define ITEM_ICON_TEXTURE_OVERRIDE_REGISTER ItemIconTextureOverride_Register(&pItemIconTextureOverride);

#define ITEM_ICON_TEXTURE_WIDTH_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void ItemIconTextureWidth_Register(int (**pointer))); \
int (*pItemIconTextureWidth);

#define ITEM_ICON_TEXTURE_WIDTH_REGISTER ItemIconTextureWidth_Register(&pItemIconTextureWidth);

#define ITEM_ICON_TEXTURE_HEIGHT_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void ItemIconTextureHeight_Register(int (**pointer))); \
int (*pItemIconTextureHeight);

#define ITEM_ICON_TEXTURE_HEIGHT_REGISTER ItemIconTextureHeight_Register(&pItemIconTextureHeight);

#define ITEM_ICON_POSITIONS_X_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void ItemIconPositionsX_Register(s16 (**pointer)[])); \
s16 (*pItemIconPositionsX)[];

#define ITEM_ICON_POSITIONS_X_REGISTER ItemIconPositionsX_Register(&pItemIconPositionsX);

#define ITEM_ICON_POSITIONS_Y_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void ItemIconPositionsY_Register(s16 (**pointer)[])); \
s16 (*pItemIconPositionsY)[];

#define ITEM_ICON_POSITIONS_Y_REGISTER ItemIconPositionsY_Register(&pItemIconPositionsY);

#define ITEM_ICON_RECT_SIZES_X_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void ItemIconRectSizesX_Register(s16 (**pointer)[])); \
s16 (*pItemIconRectSizesX)[];

#define ITEM_ICON_RECT_SIZES_X_REGISTER ItemIconRectSizesX_Register(&pItemIconRectSizesX);

#define ITEM_ICON_RECT_SIZES_Y_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void ItemIconRectSizesY_Register(s16 (**pointer)[])); \
s16 (*pItemIconRectSizesY)[];

#define ITEM_ICON_RECT_SIZES_Y_REGISTER ItemIconRectSizesY_Register(&pItemIconRectSizesY);

#define ITEM_ICON_TEXTURE_DIMENSIONS_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void ItemIconTextureDimensions_Register(s16 (**pointer)[])); \
s16 (*pItemIconTextureDimensions)[];

#define ITEM_ICON_TEXTURE_DIMENSIONS_REGISTER ItemIconTextureDimensions_Register(&pItemIconTextureDimensions);

#define ITEM_ICON_TEXTURE_SCALES_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void ItemIconTextureScales_Register(s16 (**pointer)[])); \
s16 (*pItemIconTextureScales)[];

#define ITEM_ICON_TEXTURE_SCALES_REGISTER ItemIconTextureScales_Register(&pItemIconTextureScales);

#define B_ITEM_EQUIP_POSITION_X_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void BItemEquipPositionX_Register(s16 (**pointer))); \
s16 (*pBItemEquipPositionX);

#define B_ITEM_EQUIP_POSITION_X_REGISTER BItemEquipPositionX_Register(&pBItemEquipPositionX);

#define B_ITEM_EQUIP_POSITION_Y_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void BItemEquipPositionY_Register(s16 (**pointer))); \
s16 (*pBItemEquipPositionY);

#define B_ITEM_EQUIP_POSITION_Y_REGISTER BItemEquipPositionY_Register(&pBItemEquipPositionY);

#define A_ITEM_EQUIP_POSITION_X_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void AItemEquipPositionX_Register(s16 (**pointer))); \
s16 (*pAItemEquipPositionX);

#define A_ITEM_EQUIP_POSITION_X_REGISTER AItemEquipPositionX_Register(&pAItemEquipPositionX);

#define A_ITEM_EQUIP_POSITION_Y_DECLARE RECOMP_IMPORT("mm_recomp_interface_helper", void AItemEquipPositionY_Register(s16 (**pointer))); \
s16 (*pAItemEquipPositionY);

#define A_ITEM_EQUIP_POSITION_Y_REGISTER AItemEquipPositionY_Register(&pAItemEquipPositionY);

#endif // __INTERFACE_HELPER__