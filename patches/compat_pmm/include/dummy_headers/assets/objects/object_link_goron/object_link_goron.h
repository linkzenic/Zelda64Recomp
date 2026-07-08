#ifndef OBJECT_LINK_GORON_H
#define OBJECT_LINK_GORON_H 1

typedef enum LinkGoronShieldingLimb {
    /* 0x00 */ LINK_GORON_SHIELDING_LIMB_NONE,
    /* 0x01 */ LINK_GORON_SHIELDING_LIMB_ROOT,
    /* 0x02 */ LINK_GORON_SHIELDING_LIMB_BODY,
    /* 0x03 */ LINK_GORON_SHIELDING_LIMB_HEAD,
    /* 0x04 */ LINK_GORON_SHIELDING_LIMB_ARMS_AND_LEGS,
    /* 0x05 */ LINK_GORON_SHIELDING_LIMB_MAX
} LinkGoronShieldingLimb;

typedef enum LinkGoronLimb {
    /* 0x00 */ LINK_GORON_LIMB_NONE,
    /* 0x01 */ LINK_GORON_LIMB_ROOT,
    /* 0x02 */ LINK_GORON_LIMB_WAIST,
    /* 0x03 */ LINK_GORON_LIMB_LOWER_ROOT,
    /* 0x04 */ LINK_GORON_LIMB_RIGHT_THIGH,
    /* 0x05 */ LINK_GORON_LIMB_RIGHT_SHIN,
    /* 0x06 */ LINK_GORON_LIMB_RIGHT_FOOT,
    /* 0x07 */ LINK_GORON_LIMB_LEFT_THIGH,
    /* 0x08 */ LINK_GORON_LIMB_LEFT_SHIN,
    /* 0x09 */ LINK_GORON_LIMB_LEFT_FOOT,
    /* 0x0A */ LINK_GORON_LIMB_UPPER_ROOT,
    /* 0x0B */ LINK_GORON_LIMB_HEAD,
    /* 0x0C */ LINK_GORON_LIMB_HAT,
    /* 0x0D */ LINK_GORON_LIMB_COLLAR,
    /* 0x0E */ LINK_GORON_LIMB_LEFT_SHOULDER,
    /* 0x0F */ LINK_GORON_LIMB_LEFT_FOREARM,
    /* 0x10 */ LINK_GORON_LIMB_LEFT_HAND,
    /* 0x11 */ LINK_GORON_LIMB_RIGHT_SHOULDER,
    /* 0x12 */ LINK_GORON_LIMB_RIGHT_FOREARM,
    /* 0x13 */ LINK_GORON_LIMB_RIGHT_HAND,
    /* 0x14 */ LINK_GORON_LIMB_SHEATH,
    /* 0x15 */ LINK_GORON_LIMB_TORSO,
    /* 0x16 */ LINK_GORON_LIMB_MAX
} LinkGoronLimb;

extern u64 gLinkGoronEyesOpenTex[];
extern u64 gLinkGoronEyesHalfTex[];
extern u64 gLinkGoronEyesClosedTex[];
extern u64 gLinkGoronEyesSurprisedTex[];
extern u64 object_link_goron_TLUT_002000[];
extern u64 gLinkGoronSkinTLUT[];
extern u64 object_link_goron_Tex_002400[];
extern u64 object_link_goron_Tex_002600[];
extern u64 object_link_goron_Tex_002700[];
extern u64 object_link_goron_Tex_002780[];
extern u64 object_link_goron_Tex_002880[];
extern u64 object_link_goron_Tex_002900[];
extern u64 object_link_goron_Tex_002940[];
extern u64 object_link_goron_Tex_002D40[];
extern u64 object_link_goron_Tex_003140[];
extern u64 object_link_goron_Tex_003940[];
extern u64 object_link_goron_Tex_003A40[];
extern u64 object_link_goron_Tex_003E40[];
extern u64 object_link_goron_Tex_004240[];
extern u64 object_link_goron_Tex_004340[];
extern Vtx object_link_goronVtx_004380[];
extern Gfx gLinkGoronRightThighDL[];
extern Gfx gLinkGoronRightShinDL[];
extern Gfx gLinkGoronRightFootDL[];
extern Gfx gLinkGoronLeftThighDL[];
extern Gfx gLinkGoronLeftShinDL[];
extern Gfx gLinkGoronLeftFootDL[];
extern Gfx gLinkGoronWaistDL[];
extern Gfx gLinkGoronCollarDL[];
extern Gfx gLinkGoronTorsoDL[];
extern Gfx gLinkGoronHeadDL[];
extern Gfx gLinkGoronHatDL[];
extern Gfx gLinkGoronSheathDL[];
extern Gfx gLinkGoronRightShoulderDL[];
extern Gfx gLinkGoronRightForearmDL[];
extern Gfx gLinkGoronRightHandOpenDL[];
extern Gfx gLinkGoronLeftShoulderDL[];
extern Gfx gLinkGoronLeftForearmDL[];
extern Gfx gLinkGoronLeftHandOpenDL[];
extern Vtx object_link_goronVtx_00A6A8[];
extern Gfx gLinkGoronCurledDL[];
extern Gfx object_link_goron_DL_00C540[];
extern u64 object_link_goron_Tex_00C6B8[];
extern u64 object_link_goron_Tex_00CEB8[];
extern u64 object_link_goron_Tex_00CFB8[];
extern u64 object_link_goron_Tex_00D0B8[];
extern u64 object_link_goron_Tex_00D2B8[];
extern u64 object_link_goron_Tex_00D3B8[];
extern u64 object_link_goron_Tex_00D4B8[];
extern u64 object_link_goron_Tex_00D8B8[];
extern Vtx object_link_goronVtx_00D8F8[];
extern Gfx gLinkGoronLeftHandClosedDL[];
extern u64 object_link_goron_Tex_00E078[];
extern Vtx object_link_goronVtx_00E278[];
extern Gfx gLinkGoronRightHandClosedDL[];
extern Vtx object_link_goronVtx_00E9F8[];
extern Gfx object_link_goron_DL_00FC18[];
extern Gfx object_link_goron_DL_00FCF0[];
extern Gfx object_link_goron_DL_00FF18[];
extern Gfx object_link_goron_DL_010140[];
extern Gfx object_link_goron_DL_010368[];
extern Gfx object_link_goron_DL_010590[];
extern u64 object_link_goron_TLUT_0107B8[];
extern u64 object_link_goron_Tex_0109B8[];
extern u64 object_link_goron_Tex_010AB8[];
extern u64 object_link_goron_Tex_010B38[];
extern u64 object_link_goron_Tex_010C38[];
extern Vtx object_link_goronVtx_011038[];
extern Gfx gLinkGoronLeftHandHoldBottleDL[];
extern u64 object_link_goron_Tex_011628[];
extern u64 object_link_goron_Tex_011728[];
extern Vtx object_link_goronVtx_011928[];
extern Gfx gLinkGoronGoronPunchEffectDL[];
extern u64 object_link_goron_Tex_011C10[];
extern u64 object_link_goron_Tex_011E10[];
extern Vtx object_link_goronVtx_012610[];
extern Gfx object_link_goron_DL_0127B0[];
extern u64 object_link_goron_Tex_012930[];
extern AnimatedMatTexScrollParams object_link_goron_Matanimheader_013138TexScrollParams_013130[];
extern AnimatedMaterial object_link_goron_Matanimheader_013138[];
extern Vtx object_link_goronVtx_013140[];
extern Gfx object_link_goron_DL_0134D0[];
extern u64 object_link_goron_Tex_013660[];
extern u64 object_link_goron_Tex_013E60[];
extern F3DPrimColor object_link_goronTexColorChangingPrimColors_014660[];
extern F3DEnvColor object_link_goronTexColorChangingEnvColors_01466C[];
extern AnimatedMatColorParams object_link_goron_Matanimheader_014684ColorParams_014674;
extern AnimatedMaterial object_link_goron_Matanimheader_014684[];
extern Gfx gLinkGoronRollingSpikesAndEffectDL[];
extern Vtx object_link_goronVtx_0146B8[];
extern Gfx object_link_goron_DL_016A88[];
extern Gfx object_link_goron_DL_017180[];
extern Gfx object_link_goron_DL_017620[];
extern StandardLimb gLinkGoronShieldingRootLimb;
extern StandardLimb gLinkGoronShieldingBodyLimb;
extern StandardLimb gLinkGoronShieldingHeadLimb;
extern StandardLimb gLinkGoronShieldingArmsAndLegsLimb;
extern void* gLinkGoronShieldingSkelLimbs[];
extern FlexSkeletonHeader gLinkGoronShieldingSkel;
extern s16 sLinkGoronShieldingAnimFrameData[];
extern JointIndex sLinkGoronShieldingAnimJointIndices[];
extern AnimationHeader gLinkGoronShieldingAnim;
extern LodLimb gLinkGoronRootLimb;
extern LodLimb gLinkGoronWaistLimb;
extern LodLimb gLinkGoronLowerRootLimb;
extern LodLimb gLinkGoronRightThighLimb;
extern LodLimb gLinkGoronRightShinLimb;
extern LodLimb gLinkGoronRightFootLimb;
extern LodLimb gLinkGoronLeftThighLimb;
extern LodLimb gLinkGoronLeftShinLimb;
extern LodLimb gLinkGoronLeftFootLimb;
extern LodLimb gLinkGoronUpperRootLimb;
extern LodLimb gLinkGoronHeadLimb;
extern LodLimb gLinkGoronHatLimb;
extern LodLimb gLinkGoronCollarLimb;
extern LodLimb gLinkGoronLeftShoulderLimb;
extern LodLimb gLinkGoronLeftForearmLimb;
extern LodLimb gLinkGoronLeftHandLimb;
extern LodLimb gLinkGoronRightShoulderLimb;
extern LodLimb gLinkGoronRightForearmLimb;
extern LodLimb gLinkGoronRightHandLimb;
extern LodLimb gLinkGoronSheathLimb;
extern LodLimb gLinkGoronTorsoLimb;
extern void* gLinkGoronSkelLimbs[];
extern FlexSkeletonHeader gLinkGoronSkel;
#endif
