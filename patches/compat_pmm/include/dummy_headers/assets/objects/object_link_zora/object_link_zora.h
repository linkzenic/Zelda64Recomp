#ifndef OBJECT_LINK_ZORA_H
#define OBJECT_LINK_ZORA_H 1

typedef enum LinkZoraLimb {
    /* 0x00 */ LINK_ZORA_LIMB_NONE,
    /* 0x01 */ LINK_ZORA_LIMB_ROOT,
    /* 0x02 */ LINK_ZORA_LIMB_WAIST,
    /* 0x03 */ LINK_ZORA_LIMB_LOWER_ROOT,
    /* 0x04 */ LINK_ZORA_LIMB_RIGHT_THIGH,
    /* 0x05 */ LINK_ZORA_LIMB_RIGHT_SHIN,
    /* 0x06 */ LINK_ZORA_LIMB_RIGHT_FOOT,
    /* 0x07 */ LINK_ZORA_LIMB_LEFT_THIGH,
    /* 0x08 */ LINK_ZORA_LIMB_LEFT_SHIN,
    /* 0x09 */ LINK_ZORA_LIMB_LEFT_FOOT,
    /* 0x0A */ LINK_ZORA_LIMB_UPPER_ROOT,
    /* 0x0B */ LINK_ZORA_LIMB_HEAD,
    /* 0x0C */ LINK_ZORA_LIMB_HAT,
    /* 0x0D */ LINK_ZORA_LIMB_COLLAR,
    /* 0x0E */ LINK_ZORA_LIMB_LEFT_SHOULDER,
    /* 0x0F */ LINK_ZORA_LIMB_LEFT_FOREARM,
    /* 0x10 */ LINK_ZORA_LIMB_LEFT_HAND,
    /* 0x11 */ LINK_ZORA_LIMB_RIGHT_SHOULDER,
    /* 0x12 */ LINK_ZORA_LIMB_RIGHT_FOREARM,
    /* 0x13 */ LINK_ZORA_LIMB_RIGHT_HAND,
    /* 0x14 */ LINK_ZORA_LIMB_SHEATH,
    /* 0x15 */ LINK_ZORA_LIMB_TORSO,
    /* 0x16 */ LINK_ZORA_LIMB_MAX
} LinkZoraLimb;

extern u64 gLinkZoraEyesOpenTex[];
extern u64 gLinkZoraEyesHalfTex[];
extern u64 gLinkZoraEyesClosedTex[];
extern u64 gLinkZoraEyesRollRightTex[];
extern u64 gLinkZoraEyesRollLeftTex[];
extern u64 gLinkZoraEyesRollUpTex[];
extern u64 gLinkZoraEyesRollDownTex[];
extern u64 object_link_zora_Tex_003800[];
extern u64 gLinkZoraMouthClosedTex[];
extern u64 object_link_zora_Tex_004400[];
extern u64 gLinkZoraMouthAngryTex[];
extern u64 gLinkZoraMouthHappyTex[];
extern u64 object_link_zora_TLUT_005000[];
extern u64 gLinkZoraSkinTLUT[];
extern u64 object_link_zora_Tex_005400[];
extern u64 object_link_zora_Tex_005800[];
extern u64 object_link_zora_Tex_005900[];
extern u64 object_link_zora_Tex_005A00[];
extern u64 object_link_zora_Tex_005A80[];
extern u64 object_link_zora_Tex_005B00[];
extern u64 object_link_zora_Tex_005D00[];
extern u64 object_link_zora_Tex_005F00[];
extern u64 object_link_zora_Tex_006000[];
extern u64 object_link_zora_Tex_006100[];
extern u64 object_link_zora_Tex_006180[];
extern u64 object_link_zora_Tex_0061C0[];
extern Vtx object_link_zoraVtx_0062C0[];
extern Gfx gLinkZoraRightThighDL[];
extern Gfx gLinkZoraRightShinDL[];
extern Gfx gLinkZoraRightFootDL[];
extern Gfx gLinkZoraLeftThighDL[];
extern Gfx gLinkZoraLeftShinDL[];
extern Gfx gLinkZoraLeftFootDL[];
extern Gfx gLinkZoraWaistDL[];
extern Gfx gLinkZoraCollarDL[];
extern Gfx gLinkZoraTorsoDL[];
extern Gfx gLinkZoraHeadDL[];
extern Gfx gLinkZoraHatDL[];
extern Gfx gLinkZoraSheathDL[];
extern Gfx gLinkZoraRightShoulderDL[];
extern Gfx gLinkZoraRightForearmDL[];
extern Gfx gLinkZoraRightHandClosedDL[];
extern Gfx gLinkZoraLeftShoulderDL[];
extern Gfx gLinkZoraLeftForearmDL[];
extern Gfx gLinkZoraLeftHandClosedDL[];
extern u64 object_link_zora_TLUT_00C578[];
extern u64 object_link_zora_Tex_00C778[];
extern u64 object_link_zora_Tex_00C978[];
extern Vtx object_link_zoraVtx_00CA78[];
extern Gfx object_link_zora_DL_00CC38[];
extern Gfx object_link_zora_DL_00CDA0[];
extern Vtx object_link_zoraVtx_00CF08[];
extern Gfx object_link_zora_DL_00E088[];
extern Gfx object_link_zora_DL_00E2A0[];
extern u64 object_link_zora_Tex_00E678[];
extern u64 object_link_zora_Tex_00E778[];
extern u64 object_link_zora_Tex_00E878[];
extern u64 object_link_zora_Tex_00E8F8[];
extern u64 object_link_zora_Tex_00EAF8[];
extern u64 object_link_zora_Tex_00ECF8[];
extern u64 object_link_zora_Tex_00EEF8[];
extern u64 object_link_zora_Tex_00F2F8[];
extern Vtx object_link_zoraVtx_00F6F8[];
extern Gfx gLinkZoraRightHandOpenDL[];
extern Gfx gLinkZoraLeftHandOpenDL[];
extern u64 object_link_zora_Tex_010028[];
extern u64 object_link_zora_Tex_010228[];
extern Vtx object_link_zoraVtx_010628[];
extern Gfx object_link_zora_DL_010868[];
extern Gfx object_link_zora_DL_010978[];
extern Vtx object_link_zoraVtx_010A88[];
extern Gfx gLinkZoraLeftHandHoldBottleDL[];
extern Vtx object_link_zoraVtx_010F38[];
extern Gfx object_link_zora_DL_0110A8[];
extern Vtx object_link_zora_Vtx_011210[];
extern u8 object_link_zora_U8_011710[];
extern Gfx object_link_zora_DL_011760[];
extern Gfx object_link_zora_DL_011A58[];
extern u64 object_link_zora_Tex_011A60[];
extern u64 object_link_zora_Tex_012260[];
extern AnimatedMatTexScrollParams object_link_zora_Matanimheader_012A68TexScrollParams_012A60[];
extern AnimatedMaterial object_link_zora_Matanimheader_012A68[];
extern AnimatedMatTexScrollParams object_link_zora_Matanimheader_012A80TexScrollParams_012A70[];
extern AnimatedMatTexScrollParams object_link_zora_Matanimheader_012A80TexScrollParams_012A78[];
extern AnimatedMaterial object_link_zora_Matanimheader_012A80[];
extern LodLimb gLinkZoraRootLimb;
extern LodLimb gLinkZoraWaistLimb;
extern LodLimb gLinkZoraLowerRootLimb;
extern LodLimb gLinkZoraRightThighLimb;
extern LodLimb gLinkZoraRightShinLimb;
extern LodLimb gLinkZoraRightFootLimb;
extern LodLimb gLinkZoraLeftThighLimb;
extern LodLimb gLinkZoraLeftShinLimb;
extern LodLimb gLinkZoraLeftFootLimb;
extern LodLimb gLinkZoraUpperRootLimb;
extern LodLimb gLinkZoraHeadLimb;
extern LodLimb gLinkZoraHatLimb;
extern LodLimb gLinkZoraCollarLimb;
extern LodLimb gLinkZoraLeftShoulderLimb;
extern LodLimb gLinkZoraLeftForearmLimb;
extern LodLimb gLinkZoraLeftHandLimb;
extern LodLimb gLinkZoraRightShoulderLimb;
extern LodLimb gLinkZoraRightForearmLimb;
extern LodLimb gLinkZoraRightHandLimb;
extern LodLimb gLinkZoraSheathLimb;
extern LodLimb gLinkZoraTorsoLimb;
extern void* gLinkZoraSkelLimbs[];
extern FlexSkeletonHeader gLinkZoraSkel;
#endif
