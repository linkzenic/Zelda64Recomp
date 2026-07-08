#include "global.h"
#include "modding.h"
#include "recomputils.h"
#include "globalobjects_api.h"
#include "playermodelmanager_api.h"
#include "oot_objects.h"
#include "oot_equipment.h"

RECOMP_IMPORT(".", bool PMMZobj_extractGameplayKeep(void *buffer, int bufferSize));
RECOMP_IMPORT(".", bool PMMZobj_extractChildLink(void *buffer, int bufferSize));
RECOMP_IMPORT(".", bool PMMZobj_extractAdultLink(void *buffer, int bufferSize));
RECOMP_IMPORT(".", bool PMMZobj_extractMirrorShieldRay(void *buffer, int bufferSize));

u8 *gGameplayKeepOOT = NULL;

u8 *gLinkChildOOT = NULL;

u8 *gLinkAdultOOT = NULL;

u8 *gMirRayOOT = NULL;

static bool sIsGameplayKeepHandled = false;
static bool sIsRealGameplayKeepLoaded = false;

void *getGameplayKeepOOT() {
    return gGameplayKeepOOT;
}

#define OOT_NTSC_GAMEPLAY_KEEP_SIZE 0x05BCE0

void loadGameplayKeepOOT() {
    if (!sIsGameplayKeepHandled) {
        gGameplayKeepOOT = recomp_alloc(OOT_NTSC_GAMEPLAY_KEEP_SIZE);

        if (!PMMZobj_extractGameplayKeep(gGameplayKeepOOT, OOT_NTSC_GAMEPLAY_KEEP_SIZE)) {
            for (int i = 0; i < OOT_NTSC_GAMEPLAY_KEEP_SIZE; i += sizeof(Gfx)) {
                gSPEndDisplayList(gGameplayKeepOOT + i);
            }
        } else {
            sIsRealGameplayKeepLoaded = true;
        }

        sIsGameplayKeepHandled = true;
    }
}

static bool sIsMirRayHandled = false;

#define OOT_NTSC_OBJ_MIR_RAY_SIZE 0x001D00

void loadMirRayOOT() {
    if (!sIsMirRayHandled) {

        gMirRayOOT = recomp_alloc(OOT_NTSC_OBJ_MIR_RAY_SIZE);

        if (!PMMZobj_extractMirrorShieldRay(gMirRayOOT, OOT_NTSC_OBJ_MIR_RAY_SIZE)) {
            recomp_free(gMirRayOOT);
            gMirRayOOT = NULL;
        }

        sIsMirRayHandled = true;
    }
}

void setupFaceTextures(PlayerModelManagerHandle h, u8 *zobj);

#define OOT_LINK_CHILD_SKELETON 0x0602CF6C
#define OOT_LINK_CHILD_SIZE 0x2CF80
#define OOT_LINK_CHILD_LFIST 0x13E18
#define OOT_LINK_CHILD_RFIST 0x14320
#define OOT_LINK_CHILD_BOTTLE_HAND 0x15FD0

void repointSkeletonAndDisableLOD(FlexSkeletonHeader *skel, GlobalObjectsSegmentMap segments) {

    LodLimb **limbs = (LodLimb **)skel->sh.segment;

    for (int i = 0; i < skel->sh.limbCount; ++i) {
        LodLimb *limb = limbs[i];

        if (limb->dLists[0]) {
            GlobalObjects_rebaseDL(limb->dLists[0], segments);
            limb->dLists[1] = limb->dLists[0];
        }
    }
}

#define REPOINT_AND_SET(handle, dlid, base, dlOffset, segments)     \
    {                                                               \
        Gfx *_g = (Gfx *)(SEGMENTED_TO_GLOBAL_PTR(base, dlOffset)); \
        GlobalObjects_rebaseDL(_g, segments);                       \
        PlayerModelManager_setDisplayList(handle, dlid, _g);        \
    }                                                               \
    (void)0

static PlayerModelManagerHandle sOoTChildHumanHandle = 0;
static bool sWasChildRegisterAttempted = false;

#define OOT_LINK_CHILD_RFIST_HOLDING_FAIRY_OCARINA 0x15BA8

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

static void setupMMFirstPersonRightHand() {
    extern Gfx object_link_child_DL_017B40[];

    Gfx *humanRightHandHoldingHookshotFirstPersonDL = GlobalObjects_getGlobalGfxPtr(OBJECT_LINK_CHILD, object_link_child_DL_017B40);

    const size_t FPS_RIGHT_HAND_START_DRAW_INDEX = 113;

    gSPBranchList(&gLinkHumanFirstPersonArmDL[ARRAY_COUNT(gLinkHumanFirstPersonArmDL) - 1], &humanRightHandHoldingHookshotFirstPersonDL[FPS_RIGHT_HAND_START_DRAW_INDEX]);
}

void registerChildLink() {
    if (sWasChildRegisterAttempted) {
        return;
    }

    sWasChildRegisterAttempted = true;

    gLinkChildOOT = recomp_alloc(OOT_LINK_CHILD_SIZE);

    loadGameplayKeepOOT();

    if (sIsRealGameplayKeepLoaded && PMMZobj_extractChildLink(gLinkChildOOT, OOT_LINK_CHILD_SIZE)) {
        GlobalObjectsSegmentMap cLinkSegs = {0};
        cLinkSegs[0x06] = gLinkChildOOT;
        cLinkSegs[0x04] = gGameplayKeepOOT;

        GlobalObjects_globalizeLodLimbSkeleton(gLinkChildOOT, (FlexSkeletonHeader *)OOT_LINK_CHILD_SKELETON);

        FlexSkeletonHeader *skel = SEGMENTED_TO_GLOBAL_PTR(gLinkChildOOT, OOT_LINK_CHILD_SKELETON);

        repointSkeletonAndDisableLOD(skel, cLinkSegs);

        PlayerModelManagerHandle h = PLAYERMODELMANAGER_REGISTER_MODEL("__oot_object_link_child__", PMM_MODEL_TYPE_CHILD);
        sOoTChildHumanHandle = h;
        PlayerModelManager_setDisplayName(h, "Young Link (OoT)");
        PlayerModelManager_setAuthor(h, "Nintendo");
        PlayerModelManager_setSkeleton(h, skel);
        setupFaceTextures(h, gLinkChildOOT);

        // Fairy Ocarina
        static Gfx rightHandOcarina[] = {
            gsDPPipeSync(),
            gsSPEndDisplayList(),
        };
        const uintptr_t FAIRY_OCARINA_HAND_START_PLUS_ONE = 0x15CB8 + sizeof(Gfx);
        gSPBranchList(&rightHandOcarina[1], gLinkChildOOT + FAIRY_OCARINA_HAND_START_PLUS_ONE);

        // right hand holding ocarina copy
        GlobalObjects_rebaseDL((Gfx *)(gLinkChildOOT + FAIRY_OCARINA_HAND_START_PLUS_ONE), cLinkSegs);
        PlayerModelManager_setDisplayList(h, PMM_DL_OPT_RHAND_OCARINA, rightHandOcarina);

#define REPOINT_SET_CHILD(dloffset, pmmdl) REPOINT_AND_SET(h, pmmdl, gLinkChildOOT, dloffset, cLinkSegs)
        REPOINT_SET_CHILD(OOT_LINK_CHILD_LFIST, PMM_DL_LFIST);
        REPOINT_SET_CHILD(OOT_LINK_CHILD_RFIST, PMM_DL_RFIST);
        REPOINT_SET_CHILD(OOT_LINK_CHILD_BOTTLE_HAND, PMM_DL_LHAND_BOTTLE);
#undef REPOINT_SET_CHILD

        setupMMFirstPersonRightHand();
        PlayerModelManager_setDisplayList(h, PMM_DL_FPS_RHAND, gLinkHumanFirstPersonArmDL);
        PlayerModelManager_setDisplayList(h, PMM_DL_FPS_RFOREARM, gEmptyDL);

        initChildEquipment();
    } else {
        recomp_printf("[INFO] PMM_FS: Could not load OOT child Link from file system!\n");
        recomp_free(gLinkChildOOT);
        gLinkChildOOT = NULL;
    }
}

#define OOT_LINK_ADULT_SKELETON 0x060377F4
#define OOT_LINK_ADULT_SIZE 0x37800
#define OOT_LINK_ADULT_LFIST 0x21CE8
#define OOT_LINK_ADULT_RFIST 0x226E0
#define OOT_LINK_ADULT_BOTTLE_HAND 0x24B58
#define OOT_LINK_ADULT_FPS_RIGHT_FOREARM 0x29FA0
#define OOT_LINK_ADULT_FPS_LEFT_FOREARM 0x29918
#define OOT_LINK_ADULT_FPS_LEFT_HAND 0x29C20
#define OOT_LINK_ADULT_FPS_RIGHT_HAND_AND_BOW 0x2A248
#define OOT_LINK_ADULT_RIGHT_HAND_AND_OCARINA 0x24698

static bool sWasAdultRegisterAttempted = false;
static PlayerModelManagerHandle sOoTAdultHumanHandle = 0;
static PlayerModelManagerHandle sOoTAdultFierceDeityHandle = 0;

static Gfx sLinkAdultFirstPersonHand[] = {
    gsDPPipeSync(),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPSetCombineLERP(TEXEL0, 0, SHADE, 0, 0, 0, 0, 1, COMBINED, 0, PRIMITIVE, 0, 0, 0, 0, COMBINED),
    gsDPSetRenderMode(G_RM_FOG_SHADE_A, G_RM_AA_ZB_OPA_SURF2),
    gsSPClearGeometryMode(G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR),
    gsSPSetGeometryMode(G_FOG | G_LIGHTING),
    gsSPDisplayList(0x0C000000),
    gsDPPipeSync(),
    gsSPEndDisplayList(),
};

void registerAdultLink() {
    if (sWasAdultRegisterAttempted) {
        return;
    }

    sWasAdultRegisterAttempted = true;

    gLinkAdultOOT = recomp_alloc(OOT_LINK_ADULT_SIZE);

    loadGameplayKeepOOT();

    if (sIsRealGameplayKeepLoaded && PMMZobj_extractAdultLink(gLinkAdultOOT, OOT_LINK_ADULT_SIZE)) {
        loadMirRayOOT();

        GlobalObjectsSegmentMap aLinkSegs = {0};
        aLinkSegs[0x06] = gLinkAdultOOT;
        aLinkSegs[0x04] = gGameplayKeepOOT;

        GlobalObjects_globalizeLodLimbSkeleton(gLinkAdultOOT, (FlexSkeletonHeader *)OOT_LINK_ADULT_SKELETON);

        FlexSkeletonHeader *skel = SEGMENTED_TO_GLOBAL_PTR(gLinkAdultOOT, OOT_LINK_ADULT_SKELETON);

        repointSkeletonAndDisableLOD(skel, aLinkSegs);

        // Human
        PlayerModelManagerHandle h = PLAYERMODELMANAGER_REGISTER_MODEL("__oot_object_link_boy_adult__", PMM_MODEL_TYPE_ADULT);
        sOoTAdultHumanHandle = h;
        PlayerModelManager_setDisplayName(h, "Adult Link (OoT)");
        PlayerModelManager_setAuthor(h, "Nintendo");
        PlayerModelManager_setSkeleton(h, skel);
        setupFaceTextures(h, gLinkAdultOOT);

        // Unglue FPS right hand from bow
        const uintptr_t BOW_START_DRAW_OFFSET = 0x2A2C8;
        const uintptr_t BOW_HAND_START_DRAW_OFFSET = 0x2A3F8 + sizeof(Gfx);
        gSPBranchList(sLinkAdultFirstPersonHand + (ARRAY_COUNT(sLinkAdultFirstPersonHand) - 1), gLinkAdultOOT + BOW_HAND_START_DRAW_OFFSET);
        GlobalObjects_rebaseDL((Gfx *)(gLinkAdultOOT + OOT_LINK_ADULT_FPS_RIGHT_HAND_AND_BOW), aLinkSegs);

        // Unglue Ocarina of Time from hand
        const uintptr_t OCARINA_HAND_START_DRAW_OFFSET = 0x24748;
        const uintptr_t OCARINA_START_DRAW_OFFSET = 0x248D8;

        // right hand holding ocarina copy
        size_t ocarinaHandDLSize = OCARINA_START_DRAW_OFFSET - OOT_LINK_ADULT_RIGHT_HAND_AND_OCARINA + sizeof(Gfx);
        Gfx *rightHandOcarina = recomp_alloc(ocarinaHandDLSize);
        Lib_MemCpy(rightHandOcarina, gLinkAdultOOT + OOT_LINK_ADULT_RIGHT_HAND_AND_OCARINA, ocarinaHandDLSize);
        gSPEndDisplayList(&rightHandOcarina[(ocarinaHandDLSize / sizeof(Gfx)) - 1]);
        GlobalObjects_rebaseDL(rightHandOcarina, aLinkSegs);

        gSPBranchList(gLinkAdultOOT + OCARINA_HAND_START_DRAW_OFFSET, SEGMENT_ADDR(6, OCARINA_START_DRAW_OFFSET));

#define REPOINT_SET_ADULT(dloffset, pmmdl) REPOINT_AND_SET(h, pmmdl, gLinkAdultOOT, dloffset, aLinkSegs)
        REPOINT_SET_ADULT(OOT_LINK_ADULT_LFIST, PMM_DL_LFIST);
        REPOINT_SET_ADULT(OOT_LINK_ADULT_RFIST, PMM_DL_RFIST);
        REPOINT_SET_ADULT(OOT_LINK_ADULT_BOTTLE_HAND, PMM_DL_LHAND_BOTTLE);
        PlayerModelManager_setDisplayList(h, PMM_DL_OPT_RHAND_OCARINA, rightHandOcarina);
        REPOINT_SET_ADULT(OOT_LINK_ADULT_FPS_LEFT_HAND, PMM_DL_FPS_LHAND);
        PlayerModelManager_setDisplayList(h, PMM_DL_OPT_FPS_RHAND, sLinkAdultFirstPersonHand);
        REPOINT_SET_ADULT(OOT_LINK_ADULT_FPS_RIGHT_FOREARM, PMM_DL_FPS_RFOREARM);
        REPOINT_SET_ADULT(OOT_LINK_ADULT_FPS_LEFT_FOREARM, PMM_DL_FPS_LFOREARM);
        REPOINT_SET_ADULT(OOT_LINK_ADULT_RIGHT_HAND_AND_OCARINA, PMM_DL_OCARINA_TIME);

        // Fierce Deity
        h = PLAYERMODELMANAGER_REGISTER_MODEL("__oot_object_link_boy_fd__", PMM_MODEL_TYPE_FIERCE_DEITY);
        sOoTAdultFierceDeityHandle = h;
        PlayerModelManager_setDisplayName(h, "Adult Link (OoT)");
        PlayerModelManager_setAuthor(h, "Nintendo");
        PlayerModelManager_setSkeleton(h, skel);
        setupFaceTextures(h, gLinkAdultOOT);

        REPOINT_SET_ADULT(OOT_LINK_ADULT_LFIST, PMM_DL_LFIST);
        REPOINT_SET_ADULT(OOT_LINK_ADULT_RFIST, PMM_DL_RFIST);
        REPOINT_SET_ADULT(OOT_LINK_ADULT_BOTTLE_HAND, PMM_DL_LHAND_BOTTLE);
        REPOINT_SET_ADULT(OOT_LINK_ADULT_FPS_LEFT_HAND, PMM_DL_FPS_LHAND);
        PlayerModelManager_setDisplayList(h, PMM_DL_FPS_RHAND, sLinkAdultFirstPersonHand);
        REPOINT_SET_ADULT(OOT_LINK_ADULT_FPS_RIGHT_FOREARM, PMM_DL_FPS_RFOREARM);
        REPOINT_SET_ADULT(OOT_LINK_ADULT_FPS_LEFT_FOREARM, PMM_DL_FPS_LFOREARM);
        REPOINT_SET_ADULT(OOT_LINK_ADULT_RIGHT_HAND_AND_OCARINA, PMM_DL_OCARINA_TIME);
#undef REPOINT_SET_ADULT

        initAdultEquipment();
    } else {
        recomp_printf("[INFO] PMM_FS: Could not load OOT adult Link from file system!\n");
        recomp_free(gLinkAdultOOT);
        gLinkAdultOOT = NULL;
    }
}

PlayerModelManagerHandle getAdultLinkHumanHandle() {
    return sOoTAdultHumanHandle;
}

PlayerModelManagerHandle getChildLinkHumanHandle() {
    return sOoTChildHumanHandle;
}

PlayerModelManagerHandle getAdultLinkFierceDeityHandle() {
    return sOoTAdultFierceDeityHandle;
}
