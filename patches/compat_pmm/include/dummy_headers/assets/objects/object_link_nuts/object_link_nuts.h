#ifndef OBJECT_LINK_NUTS_H
#define OBJECT_LINK_NUTS_H 1

typedef enum LinkDekuLimb {
    /* 0x00 */ LINK_DEKU_LIMB_NONE,
    /* 0x01 */ LINK_DEKU_LIMB_ROOT,
    /* 0x02 */ LINK_DEKU_LIMB_WAIST,
    /* 0x03 */ LINK_DEKU_LIMB_LOWER_ROOT,
    /* 0x04 */ LINK_DEKU_LIMB_RIGHT_THIGH,
    /* 0x05 */ LINK_DEKU_LIMB_RIGHT_SHIN,
    /* 0x06 */ LINK_DEKU_LIMB_RIGHT_FOOT,
    /* 0x07 */ LINK_DEKU_LIMB_LEFT_THIGH,
    /* 0x08 */ LINK_DEKU_LIMB_LEFT_SHIN,
    /* 0x09 */ LINK_DEKU_LIMB_LEFT_FOOT,
    /* 0x0A */ LINK_DEKU_LIMB_UPPER_ROOT,
    /* 0x0B */ LINK_DEKU_LIMB_HEAD,
    /* 0x0C */ LINK_DEKU_LIMB_HAT,
    /* 0x0D */ LINK_DEKU_LIMB_COLLAR,
    /* 0x0E */ LINK_DEKU_LIMB_LEFT_SHOULDER,
    /* 0x0F */ LINK_DEKU_LIMB_LEFT_FOREARM,
    /* 0x10 */ LINK_DEKU_LIMB_LEFT_HAND,
    /* 0x11 */ LINK_DEKU_LIMB_RIGHT_SHOULDER,
    /* 0x12 */ LINK_DEKU_LIMB_RIGHT_FOREARM,
    /* 0x13 */ LINK_DEKU_LIMB_RIGHT_HAND,
    /* 0x14 */ LINK_DEKU_LIMB_SHEATH,
    /* 0x15 */ LINK_DEKU_LIMB_TORSO,
    /* 0x16 */ LINK_DEKU_LIMB_MAX
} LinkDekuLimb;

extern Vtx object_link_nutsVtx_000000[];
extern Gfx gLinkDekuRightThighDL[];
extern Gfx gLinkDekuRightShinDL[];
extern Gfx gLinkDekuRightFootDL[];
extern Gfx gLinkDekuLeftThighDL[];
extern Gfx gLinkDekuLeftShinDL[];
extern Gfx gLinkDekuLeftFootDL[];
extern Gfx gLinkDekuWaistDL[];
extern Gfx gLinkDekuCollarDL[];
extern Gfx gLinkDekuTorsoDL[];
extern Gfx gLinkDekuHeadDL[];
extern Gfx gLinkDekuHatDL[];
extern Gfx gLinkDekuSheathDL[];
extern Gfx gLinkDekuRightShoulderDL[];
extern Gfx gLinkDekuRightForearmDL[];
extern Gfx gLinkDekuRightHandDL[];
extern Gfx gLinkDekuLeftShoulderDL[];
extern Gfx gLinkDekuLeftForearmDL[];
extern Gfx gLinkDekuLeftHandDL[];
extern u64 object_link_nuts_TLUT_003CB0[];
extern u64 object_link_nuts_TLUT_003EB0[];
extern u64 object_link_nuts_Tex_0040B0[];
extern u64 object_link_nuts_Tex_0041B0[];
extern u64 object_link_nuts_Tex_0042B0[];
extern u64 object_link_nuts_Tex_0043B0[];
extern u64 object_link_nuts_Tex_0044B0[];
extern u64 object_link_nuts_Tex_0048B0[];
extern u64 object_link_nuts_Tex_0058B0[];
extern Vtx object_link_nutsVtx_0059B0[];
extern Gfx object_link_nuts_DL_007390[];
extern Gfx object_link_nuts_DL_007548[];
extern Gfx object_link_nuts_DL_0076A0[];
extern Gfx object_link_nuts_DL_0077D0[];
extern Gfx object_link_nuts_DL_007900[];
extern Gfx object_link_nuts_DL_007A28[];
extern u64 object_link_nuts_Tex_007B80[];
extern Vtx object_link_nutsVtx_007D80[];
extern Gfx object_link_nuts_DL_008660[];
extern Gfx object_link_nuts_DL_008760[];
extern Gfx object_link_nuts_DL_008860[];
extern Gfx object_link_nuts_DL_008908[];
extern Gfx object_link_nuts_DL_0089F0[];
extern Gfx gLinkDekuClosedFlowerDL[];
extern Gfx object_link_nuts_DL_008AB8[];
extern Gfx object_link_nuts_DL_008BA0[];
extern Gfx gLinkDekuOpenFlowerDL[];
extern u64 object_link_nuts_Tex_008C68[];
extern u64 object_link_nuts_Tex_008E68[];
extern u64 object_link_nuts_Tex_009068[];
extern Vtx object_link_nutsVtx_0090E8[];
extern Gfx object_link_nuts_DL_009AB8[];
extern Gfx object_link_nuts_DL_009C48[];
extern Gfx object_link_nuts_DL_009DB8[];
extern Vtx object_link_nutsVtx_009ED8[];
extern Gfx object_link_nuts_DL_00A348[];
extern u64 object_link_nuts_Tex_00A4E0[];
extern Vtx object_link_nutsVtx_00B4E0[];
extern Gfx object_link_nuts_DL_00B690[];
extern u64 object_link_nuts_Tex_00B780[];
extern LodLimb gLinkDekuRootLimb;
extern LodLimb gLinkDekuWaistLimb;
extern LodLimb gLinkDekuLowerRootLimb;
extern LodLimb gLinkDekuRightThighLimb;
extern LodLimb gLinkDekuRightShinLimb;
extern LodLimb gLinkDekuRightFootLimb;
extern LodLimb gLinkDekuLeftThighLimb;
extern LodLimb gLinkDekuLeftShinLimb;
extern LodLimb gLinkDekuLeftFootLimb;
extern LodLimb gLinkDekuUpperRootLimb;
extern LodLimb gLinkDekuHeadLimb;
extern LodLimb gLinkDekuHatLimb;
extern LodLimb gLinkDekuCollarLimb;
extern LodLimb gLinkDekuLeftShoulderLimb;
extern LodLimb gLinkDekuLeftForearmLimb;
extern LodLimb gLinkDekuLeftHandLimb;
extern LodLimb gLinkDekuRightShoulderLimb;
extern LodLimb gLinkDekuRightForearmLimb;
extern LodLimb gLinkDekuRightHandLimb;
extern LodLimb gLinkDekuSheathLimb;
extern LodLimb gLinkDekuTorsoLimb;
extern void* gLinkDekuSkelLimbs[];
extern FlexSkeletonHeader gLinkDekuSkel;
#endif
