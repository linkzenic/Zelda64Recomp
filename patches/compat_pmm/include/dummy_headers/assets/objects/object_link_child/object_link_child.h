#ifndef OBJECT_LINK_CHILD_H
#define OBJECT_LINK_CHILD_H 1

typedef enum LinkHumanLimb {
    /* 0x00 */ LINK_HUMAN_LIMB_NONE,
    /* 0x01 */ LINK_HUMAN_LIMB_ROOT,
    /* 0x02 */ LINK_HUMAN_LIMB_WAIST,
    /* 0x03 */ LINK_HUMAN_LIMB_LOWER_ROOT,
    /* 0x04 */ LINK_HUMAN_LIMB_RIGHT_THIGH,
    /* 0x05 */ LINK_HUMAN_LIMB_RIGHT_SHIN,
    /* 0x06 */ LINK_HUMAN_LIMB_RIGHT_FOOT,
    /* 0x07 */ LINK_HUMAN_LIMB_LEFT_THIGH,
    /* 0x08 */ LINK_HUMAN_LIMB_LEFT_SHIN,
    /* 0x09 */ LINK_HUMAN_LIMB_LEFT_FOOT,
    /* 0x0A */ LINK_HUMAN_LIMB_UPPER_ROOT,
    /* 0x0B */ LINK_HUMAN_LIMB_HEAD,
    /* 0x0C */ LINK_HUMAN_LIMB_HAT,
    /* 0x0D */ LINK_HUMAN_LIMB_COLLAR,
    /* 0x0E */ LINK_HUMAN_LIMB_LEFT_SHOULDER,
    /* 0x0F */ LINK_HUMAN_LIMB_LEFT_FOREARM,
    /* 0x10 */ LINK_HUMAN_LIMB_LEFT_HAND,
    /* 0x11 */ LINK_HUMAN_LIMB_RIGHT_SHOULDER,
    /* 0x12 */ LINK_HUMAN_LIMB_RIGHT_FOREARM,
    /* 0x13 */ LINK_HUMAN_LIMB_RIGHT_HAND,
    /* 0x14 */ LINK_HUMAN_LIMB_SHEATH,
    /* 0x15 */ LINK_HUMAN_LIMB_TORSO,
    /* 0x16 */ LINK_HUMAN_LIMB_MAX
} LinkHumanLimb;

extern u64 gLinkHumanEyesOpenTex[];
extern u64 gLinkHumanEyesHalfTex[];
extern u64 gLinkHumanEyesClosedTex[];
extern u64 gLinkHumanEyesRollRightTex[];
extern u64 gLinkHumanEyesRollLeftTex[];
extern u64 gLinkHumanEyesRollUpTex[];
extern u64 gLinkHumanEyesRollDownTex[];
extern u64 object_link_child_Tex_003800[];
extern u64 gLinkHumanMouthClosedTex[];
extern u64 gLinkHumanMouthTeethTex[];
extern u64 gLinkHumanMouthAngryTex[];
extern u64 gLinkHumanMouthHappyTex[];
extern u64 gLinkHumanSkinTLUT[];
extern u64 gLinkHumanMouthTLUT[];
extern u64 object_link_child_Tex_005400[];
extern u64 object_link_child_Tex_005500[];
extern u64 object_link_child_Tex_005540[];
extern u64 gLinkHumanEarTex[];
extern u64 gLinkHumanBeltClaspTex[];
extern u64 gLinkHumanBeltTex[];
extern u64 object_link_child_Tex_005C40[];
extern u64 object_link_child_Tex_005D40[];
extern u64 object_link_child_Tex_005D80[];
extern u64 object_link_child_Tex_005E00[];
extern u64 object_link_child_Tex_006200[];
extern u64 object_link_child_Tex_006A00[];
extern u64 object_link_child_Tex_006B00[];
extern u64 object_link_child_Tex_007700[];
extern Vtx object_link_childVtx_007900[];
extern Gfx gLinkHumanRightThighDL[];
extern Gfx gLinkHumanRightShinDL[];
extern Gfx gLinkHumanRightFootDL[];
extern Gfx gLinkHumanLeftThighDL[];
extern Gfx gLinkHumanLeftShinDL[];
extern Gfx gLinkHumanLeftFootDL[];
extern Gfx gLinkHumanWaistDL[];
extern Gfx gLinkHumanCollarDL[];
extern Gfx gLinkHumanTorsoDL[];
extern Gfx gLinkHumanHeadDL[];
extern Gfx gLinkHumanHatDL[];
extern Gfx gLinkHumanSwordScabbardDL[];
extern Gfx gLinkHumanRightShoulderDL[];
extern Gfx gLinkHumanRightForearmDL[];
extern Gfx gLinkHumanRightHandOpenDL[];
extern Gfx gLinkHumanLeftShoulderDL[];
extern Gfx gLinkHumanLeftForearmDL[];
extern Gfx gLinkHumanLeftHandOpenDL[];
extern u64 gLinkHumanOcarinaTex[];
extern u64 object_link_child_TLUT_00DA80[];
extern u64 object_link_child_Tex_00DB08[];
extern Vtx object_link_childVtx_00DF68[];
extern Gfx gLinkHumanLeftHandClosedDL[];
extern Vtx object_link_childVtx_00E4D8[];
extern Gfx gLinkHumanRightHandClosedDL[];
extern Vtx object_link_childVtx_00E858[];
extern Gfx gLinkHumanRightHandHoldingOcarinaDL[];
extern Vtx object_link_childVtx_00EF68[];
extern Gfx object_link_child_DL_00F1D8[];
extern Vtx object_link_childVtx_00F318[];
extern Gfx object_link_child_DL_00F548[];
extern Vtx object_link_childVtx_00F668[];
extern Gfx object_link_child_DL_00F998[];
extern Vtx object_link_childVtx_00FAD8[];
extern Gfx object_link_child_DL_00FD08[];
extern Vtx object_link_childVtx_00FE28[];
extern Gfx object_link_child_DL_0102E8[];
extern Vtx object_link_childVtx_010538[];
extern Gfx object_link_child_DL_015F98[];
extern Gfx object_link_child_DL_016048[];
extern Gfx object_link_child_DL_016058[];
extern Gfx object_link_child_DL_0161C8[];
extern Gfx object_link_child_DL_016270[];
extern Gfx object_link_child_DL_016320[];
extern Gfx object_link_child_DL_0163D0[];
extern Gfx gLinkHumanMirrorShieldDL[];
extern Gfx object_link_child_DL_0164B8[];
extern Gfx object_link_child_DL_016608[];
extern Gfx object_link_child_DL_0166F8[];
extern Gfx object_link_child_DL_0167D0[];
extern Gfx gLinkHumanGreatFairysSwordDL[];
extern Gfx object_link_child_DL_0168C0[];
extern Gfx object_link_child_DL_016950[];
extern Gfx object_link_child_DL_0169E0[];
extern Gfx gLinkHumanGildedSwordSheathDL[];
extern Gfx object_link_child_DL_016BA0[];
extern Gfx object_link_child_DL_016C88[];
extern Gfx object_link_child_DL_016D58[];
extern Gfx object_link_child_DL_016E10[];
extern Gfx object_link_child_DL_016F38[];
extern Gfx object_link_child_DL_016FC8[];
extern Gfx gLinkHumanGildedSwordHandleDL[];
extern Gfx object_link_child_DL_017090[];
extern Gfx object_link_child_DL_017130[];
extern Gfx object_link_child_DL_0171D0[];
extern Gfx object_link_child_DL_017270[];
extern Gfx gLinkHumanGildedSwordBladeDL[];
extern Gfx gLinkHumanRazorSwordSheathDL[];
extern Gfx gLinkHumanHerosShieldDL[];
extern Gfx gLinkHumanKokiriSwordSheathDL[];
extern Gfx object_link_child_DL_017818[];
extern Gfx gLinkHumanHookshotDL[];
extern Gfx object_link_child_DL_017B40[];
extern Gfx gLinkHumanBowDL[];
extern Gfx object_link_child_DL_018490[];
extern u64 object_link_child_TLUT_018A40[];
extern u64 gLinkHumanHerosShieldFrontTLUT[];
extern u64 gLinkHumanHerosShieldBackTLUT[];
extern u64 gLinkHumanShieldHandleTLUT[];
extern u64 object_link_child_Tex_019240[];
extern u64 gLinkHumanHerosShieldFrontUpperTex[];
extern u64 object_link_child_Tex_019A80[];
extern u64 object_link_child_Tex_01A280[];
extern u64 object_link_child_Tex_01A380[];
extern u64 object_link_child_Tex_01B380[];
extern u64 gLinkHumanHerosShieldFrontLowerTex[];
extern u64 gLinkHumanHerosShieldBackTex[];
extern u64 gLinkHumanShieldHandleTex[];
extern u64 object_link_child_Tex_01BF00[];
extern u64 object_link_child_Tex_01BF80[];
extern u64 object_link_child_Tex_01C180[];
extern u64 object_link_child_Tex_01C380[];
extern u64 object_link_child_Tex_01C400[];
extern u64 object_link_child_Tex_01C800[];
extern u64 object_link_child_Tex_01C900[];
extern u64 object_link_child_Tex_01CB00[];
extern u64 object_link_child_Tex_01D300[];
extern u64 object_link_child_Tex_01D340[];
extern u64 object_link_child_Tex_01D440[];
extern Vtx object_link_childVtx_01D840[];
extern Gfx object_link_child_DL_01D960[];
extern Gfx gLinkHumanSheathEmptyDL[];
extern Mtx gLinkHumanSheathedKokiriSwordMtx;
extern Gfx gLinkHumanSheathedKokiriSwordDL[];
extern Mtx gLinkHumanSheathedRazorSwordMtx;
extern Gfx gLinkHumanSheathedRazorSwordDL[];
extern Gfx gLinkHumanSheathedGildedSwordDL[];
extern Mtx gLinkHumanHerosShieldMtx;
extern Gfx gLinkHumanHerosShieldWithMtxDL[];
extern Mtx gLinkHumanMirrorShieldMtx;
extern Gfx gLinkHumanMirrorShieldWithMtxDL[];
extern Gfx gLinkHumanRightHandHoldingHerosShieldDL[];
extern Gfx object_link_child_DL_01DC38[];
extern Gfx gLinkHumanRightHandHoldingMirrorShieldDL[];
extern Gfx object_link_child_DL_01DC58[];
extern Gfx gLinkHumanLeftHandHoldingKokiriSwordDL[];
extern Gfx object_link_child_DL_01DC78[];
extern Gfx gLinkHumanLeftHandHoldingRazorSwordDL[];
extern Gfx object_link_child_DL_01DC98[];
extern Gfx gLinkHumanLeftHandHoldingGildedSwordDL[];
extern Gfx object_link_child_DL_01DCC0[];
extern Gfx gLinkHumanLeftHandHoldingGreatFairysSwordDL[];
extern Gfx object_link_child_DL_01DCE8[];
extern Gfx gLinkHumanRightHandHoldingHookshotDL[];
extern Gfx object_link_child_DL_01DD08[];
extern Gfx gLinkHumanRightHandHoldingBowDL[];
extern Gfx object_link_child_DL_01DD28[];
extern Vtx object_link_childVtx_01DD38[];
extern Gfx gLinkHumanLeftHandHoldBottleDL[];
extern LodLimb gLinkHumanRootLimb;
extern LodLimb gLinkHumanWaistLimb;
extern LodLimb gLinkHumanLowerRootLimb;
extern LodLimb gLinkHumanRightThighLimb;
extern LodLimb gLinkHumanRightShinLimb;
extern LodLimb gLinkHumanRightFootLimb;
extern LodLimb gLinkHumanLeftThighLimb;
extern LodLimb gLinkHumanLeftShinLimb;
extern LodLimb gLinkHumanLeftFootLimb;
extern LodLimb gLinkHumanUpperRootLimb;
extern LodLimb gLinkHumanHeadLimb;
extern LodLimb gLinkHumanHatLimb;
extern LodLimb gLinkHumanCollarLimb;
extern LodLimb gLinkHumanLeftShoulderLimb;
extern LodLimb gLinkHumanLeftForearmLimb;
extern LodLimb gLinkHumanLeftHandLimb;
extern LodLimb gLinkHumanRightShoulderLimb;
extern LodLimb gLinkHumanRightForearmLimb;
extern LodLimb gLinkHumanRightHandLimb;
extern LodLimb gLinkHumanSheathLimb;
extern LodLimb gLinkHumanTorsoLimb;
extern void* gLinkHumanSkelLimbs[];
extern FlexSkeletonHeader gLinkHumanSkel;
#endif
