#ifndef OBJECT_RD_H
#define OBJECT_RD_H 1

typedef enum GibdoLimb {
    /* 0x00 */ GIBDO_LIMB_NONE,
    /* 0x01 */ GIBDO_LIMB_ROOT,
    /* 0x02 */ GIBDO_LIMB_LEFT_LEG_ROOT,
    /* 0x03 */ GIBDO_LIMB_LEFT_THIGH,
    /* 0x04 */ GIBDO_LIMB_LEFT_SHIN,
    /* 0x05 */ GIBDO_LIMB_LEFT_ANKLE,
    /* 0x06 */ GIBDO_LIMB_LEFT_FOOT,
    /* 0x07 */ GIBDO_LIMB_RIGHT_LEG_ROOT,
    /* 0x08 */ GIBDO_LIMB_RIGHT_THIGH,
    /* 0x09 */ GIBDO_LIMB_RIGHT_SHIN,
    /* 0x0A */ GIBDO_LIMB_RIGHT_ANKLE,
    /* 0x0B */ GIBDO_LIMB_RIGHT_FOOT,
    /* 0x0C */ GIBDO_LIMB_UPPER_BODY_ROOT,
    /* 0x0D */ GIBDO_LIMB_UPPER_BODY,
    /* 0x0E */ GIBDO_LIMB_TORSO,
    /* 0x0F */ GIBDO_LIMB_LEFT_ARM_ROOT,
    /* 0x10 */ GIBDO_LIMB_LEFT_SHOULDER_AND_UPPER_ARM,
    /* 0x11 */ GIBDO_LIMB_LEFT_FOREARM,
    /* 0x12 */ GIBDO_LIMB_LEFT_HAND,
    /* 0x13 */ GIBDO_LIMB_RIGHT_ARM_ROOT,
    /* 0x14 */ GIBDO_LIMB_RIGHT_SHOULDER_AND_UPPER_ARM,
    /* 0x15 */ GIBDO_LIMB_RIGHT_FOREARM,
    /* 0x16 */ GIBDO_LIMB_RIGHT_HAND,
    /* 0x17 */ GIBDO_LIMB_HEAD_ROOT,
    /* 0x18 */ GIBDO_LIMB_HEAD,
    /* 0x19 */ GIBDO_LIMB_PELVIS,
    /* 0x1A */ GIBDO_LIMB_MAX
} GibdoLimb;

typedef enum RedeadLimb {
    /* 0x00 */ REDEAD_LIMB_NONE,
    /* 0x01 */ REDEAD_LIMB_ROOT,
    /* 0x02 */ REDEAD_LIMB_LEFT_LEG_ROOT,
    /* 0x03 */ REDEAD_LIMB_LEFT_THIGH,
    /* 0x04 */ REDEAD_LIMB_LEFT_SHIN,
    /* 0x05 */ REDEAD_LIMB_LEFT_ANKLE,
    /* 0x06 */ REDEAD_LIMB_LEFT_FOOT,
    /* 0x07 */ REDEAD_LIMB_RIGHT_LEG_ROOT,
    /* 0x08 */ REDEAD_LIMB_RIGHT_THIGH,
    /* 0x09 */ REDEAD_LIMB_RIGHT_SHIN,
    /* 0x0A */ REDEAD_LIMB_RIGHT_ANKLE,
    /* 0x0B */ REDEAD_LIMB_RIGHT_FOOT,
    /* 0x0C */ REDEAD_LIMB_UPPER_BODY_ROOT,
    /* 0x0D */ REDEAD_LIMB_UPPER_BODY,
    /* 0x0E */ REDEAD_LIMB_TORSO,
    /* 0x0F */ REDEAD_LIMB_LEFT_ARM_ROOT,
    /* 0x10 */ REDEAD_LIMB_LEFT_SHOULDER_AND_UPPER_ARM,
    /* 0x11 */ REDEAD_LIMB_LEFT_FOREARM,
    /* 0x12 */ REDEAD_LIMB_LEFT_HAND,
    /* 0x13 */ REDEAD_LIMB_RIGHT_ARM_ROOT,
    /* 0x14 */ REDEAD_LIMB_RIGHT_SHOULDER_AND_UPPER_ARM,
    /* 0x15 */ REDEAD_LIMB_RIGHT_FOREARM,
    /* 0x16 */ REDEAD_LIMB_RIGHT_HAND,
    /* 0x17 */ REDEAD_LIMB_HEAD_ROOT,
    /* 0x18 */ REDEAD_LIMB_HEAD,
    /* 0x19 */ REDEAD_LIMB_PELVIS,
    /* 0x1A */ REDEAD_LIMB_MAX
} RedeadLimb;

extern s16 sGibdoRedeadArmsUpStartAnimFrameData[];
extern JointIndex sGibdoRedeadArmsUpStartAnimJointIndices[];
extern AnimationHeader gGibdoRedeadArmsUpStartAnim;
extern s16 sGibdoRedeadArmsUpLoopAnimFrameData[];
extern JointIndex sGibdoRedeadArmsUpLoopAnimJointIndices[];
extern AnimationHeader gGibdoRedeadArmsUpLoopAnim;
extern s16 sGibdoRedeadConvulsionAnimFrameData[];
extern JointIndex sGibdoRedeadConvulsionAnimJointIndices[];
extern AnimationHeader gGibdoRedeadConvulsionAnim;
extern u64 gGibdoWrappingTex[];
extern u64 gGibdoEyeTex[];
extern Vtx object_rdVtx_002A10[];
extern Gfx gGibdoPelvisDL[];
extern Gfx gGibdoHeadDL[];
extern Gfx gGibdoTorsoDL[];
extern Gfx gGibdoRightShoulderAndUpperArmDL[];
extern Gfx gGibdoRightForearmDL[];
extern Gfx gGibdoRightHandDL[];
extern Gfx gGibdoLeftShoulderAndUpperArmDL[];
extern Gfx gGibdoLeftForearmDL[];
extern Gfx gGibdoLeftHandDL[];
extern Gfx gGibdoRightThighDL[];
extern Gfx gGibdoRightShinDL[];
extern Gfx gGibdoRightFootDL[];
extern Gfx gGibdoLeftThighDL[];
extern Gfx gGibdoLeftShinDL[];
extern Gfx gGibdoLeftFootDL[];
extern StandardLimb gGibdoRootLimb;
extern StandardLimb gGibdoLeftLegRootLimb;
extern StandardLimb gGibdoLeftThighLimb;
extern StandardLimb gGibdoLeftShinLimb;
extern StandardLimb gGibdoLeftAnkleLimb;
extern StandardLimb gGibdoLeftFootLimb;
extern StandardLimb gGibdoRightLegRootLimb;
extern StandardLimb gGibdoRightThighLimb;
extern StandardLimb gGibdoRightShinLimb;
extern StandardLimb gGibdoRightAnkleLimb;
extern StandardLimb gGibdoRightFootLimb;
extern StandardLimb gGibdoUpperBodyRootLimb;
extern StandardLimb gGibdoUpperBodyLimb;
extern StandardLimb gGibdoTorsoLimb;
extern StandardLimb gGibdoLeftArmRootLimb;
extern StandardLimb gGibdoLeftShoulderAndUpperArmLimb;
extern StandardLimb gGibdoLeftForearmLimb;
extern StandardLimb gGibdoLeftHandLimb;
extern StandardLimb gGibdoRightArmRootLimb;
extern StandardLimb gGibdoRightShoulderAndUpperArmLimb;
extern StandardLimb gGibdoRightForearmLimb;
extern StandardLimb gGibdoRightHandLimb;
extern StandardLimb gGibdoHeadRootLimb;
extern StandardLimb gGibdoHeadLimb;
extern StandardLimb gGibdoPelvisLimb;
extern void* gGibdoSkelLimbs[];
extern FlexSkeletonHeader gGibdoSkel;
extern s16 sGibdoRedeadSlumpStartAnimFrameData[];
extern JointIndex sGibdoRedeadSlumpStartAnimJointIndices[];
extern AnimationHeader gGibdoRedeadSlumpStartAnim;
extern s16 sGibdoRedeadSlumpLoopAnimFrameData[];
extern JointIndex sGibdoRedeadSlumpLoopAnimJointIndices[];
extern AnimationHeader gGibdoRedeadSlumpLoopAnim;
extern s16 sGibdoRedeadGrabAttackAnimFrameData[];
extern JointIndex sGibdoRedeadGrabAttackAnimJointIndices[];
extern AnimationHeader gGibdoRedeadGrabAttackAnim;
extern s16 sGibdoRedeadGrabEndAnimFrameData[];
extern JointIndex sGibdoRedeadGrabEndAnimJointIndices[];
extern AnimationHeader gGibdoRedeadGrabEndAnim;
extern s16 sGibdoRedeadGrabStartAnimFrameData[];
extern JointIndex sGibdoRedeadGrabStartAnimJointIndices[];
extern AnimationHeader gGibdoRedeadGrabStartAnim;
extern s16 sGibdoRedeadLookBackAnimFrameData[];
extern JointIndex sGibdoRedeadLookBackAnimJointIndices[];
extern AnimationHeader gGibdoRedeadLookBackAnim;
extern s16 sGibdoRedeadWipingTearsAnimFrameData[];
extern JointIndex sGibdoRedeadWipingTearsAnimJointIndices[];
extern AnimationHeader gGibdoRedeadWipingTearsAnim;
extern s16 sGibdoRedeadSobbingAnimFrameData[];
extern JointIndex sGibdoRedeadSobbingAnimJointIndices[];
extern AnimationHeader gGibdoRedeadSobbingAnim;
extern s16 sGibdoRedeadDeathAnimFrameData[];
extern JointIndex sGibdoRedeadDeathAnimJointIndices[];
extern AnimationHeader gGibdoRedeadDeathAnim;
extern s16 sGibdoRedeadDamageAnimFrameData[];
extern JointIndex sGibdoRedeadDamageAnimJointIndices[];
extern AnimationHeader gGibdoRedeadDamageAnim;
extern s16 sGibdoRedeadStandUpAnimFrameData[];
extern JointIndex sGibdoRedeadStandUpAnimJointIndices[];
extern AnimationHeader gGibdoRedeadStandUpAnim;
extern s16 sGibdoRedeadIdleAnimFrameData[];
extern JointIndex sGibdoRedeadIdleAnimJointIndices[];
extern AnimationHeader gGibdoRedeadIdleAnim;
extern u64 gRedeadBackTex[];
extern u64 gRedeadTorsoTex[];
extern u64 gRedeadFingersAndToesTex[];
extern u64 gRedeadMuscleTex[];
extern u64 gRedeadMaskTex[];
extern u64 gRedeadFaceTex[];
extern u64 gRedeadHipsTex[];
extern Vtx object_rdVtx_00EBF0[];
extern Gfx gRedeadPelvisDL[];
extern Gfx gRedeadTorsoDL[];
extern Gfx gRedeadLeftShoulderAndUpperArmDL[];
extern Gfx gRedeadLeftForearmDL[];
extern Gfx gRedeadLeftHandDL[];
extern Gfx gRedeadRightShoulderAndUpperArmDL[];
extern Gfx gRedeadRightForearmDL[];
extern Gfx gRedeadRightHandDL[];
extern Gfx gRedeadHeadDL[];
extern Gfx gRedeadLeftThighDL[];
extern Gfx gRedeadLeftShinDL[];
extern Gfx gRedeadLeftFootDL[];
extern Gfx gRedeadRightThighDL[];
extern Gfx gRedeadRightShinDL[];
extern Gfx gRedeadRightFootDL[];
extern StandardLimb gRedeadRootLimb;
extern StandardLimb gRedeadLeftLegRootLimb;
extern StandardLimb gRedeadLeftThighLimb;
extern StandardLimb gRedeadLeftShinLimb;
extern StandardLimb gRedeadLeftAnkleLimb;
extern StandardLimb gRedeadLeftFootLimb;
extern StandardLimb gRedeadRightLegRootLimb;
extern StandardLimb gRedeadRightThighLimb;
extern StandardLimb gRedeadRightShinLimb;
extern StandardLimb gRedeadRightAnkleLimb;
extern StandardLimb gRedeadRightFootLimb;
extern StandardLimb gRedeadUpperBodyRootLimb;
extern StandardLimb gRedeadUpperBodyLimb;
extern StandardLimb gRedeadTorsoLimb;
extern StandardLimb gRedeadLeftArmRootLimb;
extern StandardLimb gRedeadLeftShoulderAndUpperArmLimb;
extern StandardLimb gRedeadLeftForearmLimb;
extern StandardLimb gRedeadLeftHandLimb;
extern StandardLimb gRedeadRightArmRootLimb;
extern StandardLimb gRedeadRightShoulderAndUpperArmLimb;
extern StandardLimb gRedeadRightForearmLimb;
extern StandardLimb gRedeadRightHandLimb;
extern StandardLimb gRedeadHeadRootLimb;
extern StandardLimb gRedeadHeadLimb;
extern StandardLimb gRedeadPelvisLimb;
extern void* gRedeadSkelLimbs[];
extern FlexSkeletonHeader gRedeadSkel;
extern s16 sGibdoRedeadWalkAnimFrameData[];
extern JointIndex sGibdoRedeadWalkAnimJointIndices[];
extern AnimationHeader gGibdoRedeadWalkAnim;
extern s16 sGibdoRedeadPirouetteAnimFrameData[];
extern JointIndex sGibdoRedeadPirouetteAnimJointIndices[];
extern AnimationHeader gGibdoRedeadPirouetteAnim;
extern s16 sGibdoRedeadClappingDanceAnimFrameData[];
extern JointIndex sGibdoRedeadClappingDanceAnimJointIndices[];
extern AnimationHeader gGibdoRedeadClappingDanceAnim;
extern s16 sGibdoRedeadSquattingDanceAnimFrameData[];
extern JointIndex sGibdoRedeadSquattingDanceAnimJointIndices[];
extern AnimationHeader gGibdoRedeadSquattingDanceAnim;
#endif
