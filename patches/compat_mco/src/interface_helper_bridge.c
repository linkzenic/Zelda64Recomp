#include "interface_helper.h"

extern s16 D_801BF9D4[4];
extern s16 D_801BF9DC[4];
extern s16 D_801BFB04[4];
extern s16 D_801BFB0C[4];

static bool sBButtonDrawn;
static bool sStartButtonEnabled = true;
static bool sStartLabelEnabled = true;
static s16 sCUpButtonPositionX = 254;
static int sCUpLabelPositionX = 1026;
static bool sCGlyphsEnabled[3] = { true, true, true };
static TexturePtr sCGlyphTextures[3];
static s16 sCGlyphPositionsX[3] = { 227, 249, 271 };
static s16 sCGlyphPositionsY[3] = { 18, 34, 18 };
static s16 sAmmoPositionsOnesX[4] = { 0xA8, 0xEA, 0x100, 0x116 };
static s16 sAmmoPositionsOnesY[4] = { 0x23, 0x23, 0x33, 0x23 };
static s16 sItemIconPositionsX[4];
static s16 sItemIconPositionsY[4];
static s16 sBItemEquipPositionX = 550;
static s16 sBItemEquipPositionY = 950;
static s16 sAItemEquipPositionX = 815;
static s16 sAItemEquipPositionY = 950;

void AmmoPositionsTensX_Register(s16 (**pointer)[4]) {
    *pointer = &D_801BFB04;
}

void AmmoPositionsTensY_Register(s16 (**pointer)[4]) {
    *pointer = &D_801BFB0C;
}

void AmmoPositionsOnesX_Register(s16 (**pointer)[4]) {
    *pointer = &sAmmoPositionsOnesX;
}

void AmmoPositionsOnesY_Register(s16 (**pointer)[4]) {
    *pointer = &sAmmoPositionsOnesY;
}

void ButtonPositionsX_Register(s16 (**pointer)[4]) {
    *pointer = &D_801BF9D4;
}

void ButtonPositionsY_Register(s16 (**pointer)[4]) {
    *pointer = &D_801BF9DC;
}

void CGlyphsEnabled_Register(bool (**pointer)[3]) {
    *pointer = &sCGlyphsEnabled;
}

void CGlyphTextures_Register(TexturePtr (**pointer)[3]) {
    *pointer = &sCGlyphTextures;
}

void CGlyphPositionsX_Register(s16 (**pointer)[3]) {
    *pointer = &sCGlyphPositionsX;
}

void CGlyphPositionsY_Register(s16 (**pointer)[3]) {
    *pointer = &sCGlyphPositionsY;
}

void StartButtonEnabled_Register(bool (**pointer)) {
    *pointer = &sStartButtonEnabled;
}

void StartLabelEnabled_Register(bool (**pointer)) {
    *pointer = &sStartLabelEnabled;
}

void CUpButtonPositionX_Register(s16 (**pointer)) {
    *pointer = &sCUpButtonPositionX;
}

void CUpLabelPositionX_Register(int (**pointer)) {
    *pointer = &sCUpLabelPositionX;
}

void BButtonDrawn_Register(bool (**pointer)) {
    *pointer = &sBButtonDrawn;
}

void ItemIconPositionsX_Register(s16 (**pointer)[]) {
    *pointer = &sItemIconPositionsX;
}

void ItemIconPositionsY_Register(s16 (**pointer)[]) {
    *pointer = &sItemIconPositionsY;
}

void BItemEquipPositionX_Register(s16 (**pointer)) {
    *pointer = &sBItemEquipPositionX;
}

void BItemEquipPositionY_Register(s16 (**pointer)) {
    *pointer = &sBItemEquipPositionY;
}

void AItemEquipPositionX_Register(s16 (**pointer)) {
    *pointer = &sAItemEquipPositionX;
}

void AItemEquipPositionY_Register(s16 (**pointer)) {
    *pointer = &sAItemEquipPositionY;
}
