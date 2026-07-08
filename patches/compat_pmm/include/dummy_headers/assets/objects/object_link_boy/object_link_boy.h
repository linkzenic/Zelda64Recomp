#ifndef OBJECT_LINK_BOY_H
#define OBJECT_LINK_BOY_H 1

typedef enum LinkFierceDeityLimb {
    /* 0x00 */ LINK_FIERCE_DEITY_LIMB_NONE,
    /* 0x01 */ LINK_FIERCE_DEITY_LIMB_ROOT,
    /* 0x02 */ LINK_FIERCE_DEITY_LIMB_WAIST,
    /* 0x03 */ LINK_FIERCE_DEITY_LIMB_LOWER_ROOT,
    /* 0x04 */ LINK_FIERCE_DEITY_LIMB_RIGHT_THIGH,
    /* 0x05 */ LINK_FIERCE_DEITY_LIMB_RIGHT_SHIN,
    /* 0x06 */ LINK_FIERCE_DEITY_LIMB_RIGHT_FOOT,
    /* 0x07 */ LINK_FIERCE_DEITY_LIMB_LEFT_THIGH,
    /* 0x08 */ LINK_FIERCE_DEITY_LIMB_LEFT_SHIN,
    /* 0x09 */ LINK_FIERCE_DEITY_LIMB_LEFT_FOOT,
    /* 0x0A */ LINK_FIERCE_DEITY_LIMB_UPPER_ROOT,
    /* 0x0B */ LINK_FIERCE_DEITY_LIMB_HEAD,
    /* 0x0C */ LINK_FIERCE_DEITY_LIMB_HAT,
    /* 0x0D */ LINK_FIERCE_DEITY_LIMB_COLLAR,
    /* 0x0E */ LINK_FIERCE_DEITY_LIMB_LEFT_SHOULDER,
    /* 0x0F */ LINK_FIERCE_DEITY_LIMB_LEFT_FOREARM,
    /* 0x10 */ LINK_FIERCE_DEITY_LIMB_LEFT_HAND,
    /* 0x11 */ LINK_FIERCE_DEITY_LIMB_RIGHT_SHOULDER,
    /* 0x12 */ LINK_FIERCE_DEITY_LIMB_RIGHT_FOREARM,
    /* 0x13 */ LINK_FIERCE_DEITY_LIMB_RIGHT_HAND,
    /* 0x14 */ LINK_FIERCE_DEITY_LIMB_SHEATH,
    /* 0x15 */ LINK_FIERCE_DEITY_LIMB_TORSO,
    /* 0x16 */ LINK_FIERCE_DEITY_LIMB_MAX
} LinkFierceDeityLimb;

extern u64 object_link_boy_TLUT_000000[];
extern u64 object_link_boy_TLUT_000200[];
extern u64 gLinkFierceDeityHandHoldingTLUT[];
extern u64 object_link_boy_Tex_000600[];
extern u64 object_link_boy_Tex_000800[];
extern u64 gLinkFierceDeityHandHoldingTex[];
extern u64 object_link_boy_Tex_000B00[];
extern Vtx object_link_boyVtx_000C00[];
extern Gfx gLinkFierceDeityWaistDL[];
extern Gfx gLinkFierceDeityRightThighDL[];
extern Gfx gLinkFierceDeityRightShinDL[];
extern Gfx gLinkFierceDeityRightFootDL[];
extern Gfx gLinkFierceDeityLeftThighDL[];
extern Gfx gLinkFierceDeityLeftShinDL[];
extern Gfx gLinkFierceDeityLeftFootDL[];
extern Gfx gLinkFierceDeityHatDL[];
extern Gfx gLinkFierceDeityHeadDL[];
extern Gfx object_link_boy_DL_0060E8[];
extern Gfx gLinkFierceDeityTorsoDL[];
extern Gfx object_link_boy_DL_006370[];
extern Gfx gLinkFierceDeityRightShoulderDL[];
extern Gfx gLinkFierceDeityRightForearmDL[];
extern Gfx gLinkFierceDeityRightHandDL[];
extern Gfx gLinkFierceDeityLeftShoulderDL[];
extern Gfx gLinkFierceDeityLeftForearmDL[];
extern Gfx gLinkFierceDeityLeftHandDL[];
extern Vtx object_link_boyVtx_007110[];
extern Gfx gLinkFierceDeityLeftHandHoldBottleDL[];
extern Gfx object_link_boy_DL_007A50[];
extern Gfx gLinkFierceDeityBottleDL[];
extern Vtx object_link_boyVtx_007BE8[];
extern Gfx gLinkFierceDeitySwordDL[];
extern Gfx gLinkFierceDeityLeftHandHoldingSwordDL[];
extern Gfx gLinkFierceDeityEmptyDL[];
extern u64 object_link_boy_TLUT_008108[];
extern u64 object_link_boy_TLUT_008128[];
extern u64 object_link_boy_TLUT_008148[];
extern u64 object_link_boy_TLUT_008168[];
extern u64 object_link_boy_TLUT_008188[];
extern u64 object_link_boy_TLUT_0081A8[];
extern u64 gLinkFierceDeityHandTLUT[];
extern u64 object_link_boy_TLUT_0081E8[];
extern u64 object_link_boy_Tex_008208[];
extern u64 object_link_boy_Tex_008408[];
extern u64 object_link_boy_Tex_008488[];
extern u64 object_link_boy_Tex_008888[];
extern u64 object_link_boy_Tex_008C88[];
extern u64 object_link_boy_Tex_008D08[];
extern u64 object_link_boy_Tex_008F08[];
extern u64 gLinkFierceDeityEyesTex[];
extern u64 object_link_boy_Tex_00A708[];
extern u64 gLinkFierceDeityMouthTex[];
extern u64 gLinkFierceDeityEarTex[];
extern u64 object_link_boy_Tex_00AF08[];
extern u64 gLinkFierceDeityHandTex[];
extern u64 object_link_boy_Tex_00B188[];
extern u64 object_link_boy_Tex_00B988[];
extern LodLimb gLinkFierceDeityRootLimb;
extern LodLimb gLinkFierceDeityWaistLimb;
extern LodLimb gLinkFierceDeityLowerRootLimb;
extern LodLimb gLinkFierceDeityRightThighLimb;
extern LodLimb gLinkFierceDeityRightShinLimb;
extern LodLimb gLinkFierceDeityRightFootLimb;
extern LodLimb gLinkFierceDeityLeftThighLimb;
extern LodLimb gLinkFierceDeityLeftShinLimb;
extern LodLimb gLinkFierceDeityLeftFootLimb;
extern LodLimb gLinkFierceDeityUpperRootLimb;
extern LodLimb gLinkFierceDeityHeadLimb;
extern LodLimb gLinkFierceDeityHatLimb;
extern LodLimb gLinkFierceDeityCollarLimb;
extern LodLimb gLinkFierceDeityLeftShoulderLimb;
extern LodLimb gLinkFierceDeityLeftForearmLimb;
extern LodLimb gLinkFierceDeityLeftHandLimb;
extern LodLimb gLinkFierceDeityRightShoulderLimb;
extern LodLimb gLinkFierceDeityRightForearmLimb;
extern LodLimb gLinkFierceDeityRightHandLimb;
extern LodLimb gLinkFierceDeitySheathLimb;
extern LodLimb gLinkFierceDeityTorsoLimb;
extern void* gLinkFierceDeitySkelLimbs[];
extern FlexSkeletonHeader gLinkFierceDeitySkel;
#endif
