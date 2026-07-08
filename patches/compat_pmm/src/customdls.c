#include "global.h"
#include "modding.h"
#include "customdls.h"
#include "globalobjects_api.h"
#include "logger.h"
#include "assets/objects/object_link_child/object_link_child.h"
#include "assets/objects/object_link_zora/object_link_zora.h"

#define ARRAY_LAST_ELEMENT_PTR(arr) (arr + (ARRAY_COUNT(arr) - 1))

// handless ocarina DL
Gfx gLinkHumanOcarinaDL[] = {
    gsDPPipeSync(),
    gsDPSetTextureLUT(G_TT_NONE),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPSetCombineLERP(TEXEL0, 0, SHADE, 0, 0, 0, 0, 1, COMBINED, 0, PRIMITIVE, 0, 0, 0, 0, COMBINED),
    gsDPSetRenderMode(G_RM_FOG_SHADE_A, G_RM_AA_ZB_OPA_SURF2),
    gsSPClearGeometryMode(G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR),
    gsSPSetGeometryMode(G_CULL_BACK | G_FOG | G_LIGHTING),
    gsDPSetPrimColor(0, 0, 255, 255, 255, 255),
    gsSPEndDisplayList(), // branch to gLinkHumanRightHandHoldingOcarinaDL
};

// ocarina-less hand DL
Gfx gLinkHumanRightHandOcarinaDL[] = {
    gsSPEndDisplayList(), // branch to gLinkHumanRightHandHoldingOcarinaDL
};

// weaponless first person arm DL
Gfx gLinkHumanFirstPersonArmDL[] = {
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPPipeSync(),
    gsDPSetCombineLERP(TEXEL0, 0, SHADE, 0, 0, 0, 0, TEXEL0, PRIMITIVE, 0, COMBINED, 0, 0, 0, 0, COMBINED),
    gsDPSetRenderMode(G_RM_FOG_SHADE_A, G_RM_AA_ZB_OPA_SURF2),
    gsDPPipeSync(),
    gsDPSetTextureLUT(G_TT_RGBA16),
    gsDPSetPrimColor(0, 0x80, 255, 255, 255, 255),
    gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BACK | G_FOG | G_LIGHTING | G_SHADING_SMOOTH),
    gsDPPipeSync(),
    gsSPEndDisplayList(), // branch to first person arm here
};

// handless first person hookshot
Gfx gLinkHumanFirstPersonHookshotDL[] = {
    gsSPEndDisplayList(), // branch to first person hookshot DL
};

// handless first person bow
Gfx gLinkHumanFirstPersonBowDL[] = {
    gsSPEndDisplayList(), // branch to bow DL
};

static Gfx *getDLCopy(Gfx dl[], size_t n) {
    size_t copySize = sizeof(Gfx) * n;
    Gfx *copy = recomp_alloc(copySize);

    Lib_MemCpy(copy, dl, copySize);

    return copy;
}

static void unglueOcarinaFromHand(void) {
    const size_t RIGHT_HAND_HOLDING_OCARINA_DL_LEN = 74;

    Gfx *humanRightHandHoldingOcarinaDL = getDLCopy(GlobalObjects_getGlobalGfxPtr(OBJECT_LINK_CHILD, gLinkHumanRightHandHoldingOcarinaDL), RIGHT_HAND_HOLDING_OCARINA_DL_LEN);

    const size_t OCARINA_START_DRAW_INDEX = 44;

    gSPEndDisplayList(&humanRightHandHoldingOcarinaDL[OCARINA_START_DRAW_INDEX]);

    gSPBranchList(gLinkHumanRightHandOcarinaDL, humanRightHandHoldingOcarinaDL);

    gSPBranchList(ARRAY_LAST_ELEMENT_PTR(gLinkHumanOcarinaDL), &humanRightHandHoldingOcarinaDL[OCARINA_START_DRAW_INDEX + 1]);
}

static void unglueFirstPersonHandFromHookshot(void) {
    const size_t RIGHT_HAND_HOLDING_HOOKSHOT_FIRST_PERSON_DL_LEN = 209;

    Gfx *humanRightHandHoldingHookshotFirstPersonDL = getDLCopy(GlobalObjects_getGlobalGfxPtr(OBJECT_LINK_CHILD, object_link_child_DL_017B40), RIGHT_HAND_HOLDING_HOOKSHOT_FIRST_PERSON_DL_LEN);

    const size_t FPS_RIGHT_HAND_START_DRAW_INDEX = 112;

    gSPEndDisplayList(&humanRightHandHoldingHookshotFirstPersonDL[FPS_RIGHT_HAND_START_DRAW_INDEX]);

    gSPBranchList(gLinkHumanFirstPersonHookshotDL, humanRightHandHoldingHookshotFirstPersonDL);

    gSPBranchList(ARRAY_LAST_ELEMENT_PTR(gLinkHumanFirstPersonArmDL), &humanRightHandHoldingHookshotFirstPersonDL[FPS_RIGHT_HAND_START_DRAW_INDEX + 1]);
}

static void unglueFirstPersonBowFromHand(void) {
    const size_t FPS_BOW_DL_LEN = 86;

    Gfx *firstPersonBow = getDLCopy(GlobalObjects_getGlobalGfxPtr(OBJECT_LINK_CHILD, object_link_child_DL_018490), FPS_BOW_DL_LEN);

    gSPEndDisplayList(&firstPersonBow[FPS_BOW_DL_LEN - 1]);

    gSPBranchList(gLinkHumanFirstPersonBowDL, firstPersonBow);
}

#define ZORA_SHIELD_DL_LEN 95
#define NUM_ZORA_SHIELD_VTX 80

static Vtx sZoraShieldVtxs[NUM_ZORA_SHIELD_VTX];

static u8 *sZoraShieldPhi;

Gfx gZoraMagicBarrierDL[ZORA_SHIELD_DL_LEN];

void initZoraMagicBarrierDL(void) {
    void *zora = GlobalObjects_getGlobalObject(OBJECT_LINK_ZORA);

    Lib_MemCpy(gZoraMagicBarrierDL, GlobalObjects_getGlobalGfxPtr(OBJECT_LINK_ZORA, object_link_zora_DL_011760), sizeof(gZoraMagicBarrierDL));
    Lib_MemCpy(sZoraShieldVtxs, SEGMENTED_TO_GLOBAL_PTR(zora, object_link_zora_Vtx_011210), sizeof(sZoraShieldVtxs));

    gSPVertex(&gZoraMagicBarrierDL[90], &sZoraShieldVtxs[72], 8, 0);
    gSPVertex(&gZoraMagicBarrierDL[68], &sZoraShieldVtxs[40], 32, 0);
    gSPVertex(&gZoraMagicBarrierDL[45], &sZoraShieldVtxs[32], 8, 0);
    gSPVertex(&gZoraMagicBarrierDL[23], &sZoraShieldVtxs[0], 32, 0);

    sZoraShieldPhi = SEGMENTED_TO_GLOBAL_PTR(GlobalObjects_getGlobalObject(OBJECT_LINK_ZORA), object_link_zora_U8_011710);
}

// Mostly copied from Player_DrawZoraShield
void updateZoraMagicBarrier_on_Player_DrawZoraShield(Player *player) {
    Vtx *vtx = sZoraShieldVtxs;
    u8 *phi_a0 = sZoraShieldPhi;

    // ARRAY_COUNT(object_link_zora_Vtx_011210)
    for (int i = 0; i < NUM_ZORA_SHIELD_VTX; i++) {
        // Editing the Vtxs in object itself
        vtx->v.cn[3] = (*phi_a0 * player->unk_B62) >> 8;
        vtx++;
        phi_a0++;
    }
}

void initCustomDLs(void) {
    unglueOcarinaFromHand();
    unglueFirstPersonHandFromHookshot();
    unglueFirstPersonBowFromHand();
    initZoraMagicBarrierDL();
}
