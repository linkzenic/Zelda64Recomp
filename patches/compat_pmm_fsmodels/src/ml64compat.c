#include "modding.h"
#include "global.h"
#include "recomputils.h"
#include "recompconfig.h"
#include "recompdata.h"
#include "defines_ooto.h"
#include "ml64compat.h"
#include "globalobjects_api.h"

#define OOTO_CHILD_SKEL_PTR 0x060053A8
#define OOTO_ADULT_SKEL_PTR 0x06005380

#define READ_U32(arr, offset) (*(u32 *)(arr + offset))

void OotoFixHeaderSkelPtr(void *obj) {
    u8 *zobj = obj;

    uintptr_t *skelHeaderPtrPtr = (uintptr_t *)(zobj + Z64O_SKELETON_HEADER_POINTER);

    if (*skelHeaderPtrPtr == 0xFFFFFFFF) {
        switch (zobj[Z64O_FORM_BYTE]) {
            case OOTO_FORM_BYTE_CHILD:
                *skelHeaderPtrPtr = OOTO_CHILD_SKEL_PTR;
                break;

            case OOTO_FORM_BYTE_ADULT:
                *skelHeaderPtrPtr = OOTO_ADULT_SKEL_PTR;
                break;

            default:
                break;
        }
    }
}

void OotoFixChildLeftShoulder(void *obj) {
    u8 *zobj = obj;

    if (zobj[Z64O_FORM_BYTE] == OOTO_FORM_BYTE_CHILD) {
        Gfx *lShoulderLUT = (Gfx *)(zobj + OOTO_CHILD_LUT_DL_LSHOULDER);
        Gfx *lForearmLUT = (Gfx *)(zobj + OOTO_CHILD_LUT_DL_LFOREARM);

        Gfx *lShoulder = (Gfx *)(lShoulderLUT->words.w1);
        Gfx *lForearm = (Gfx *)(lForearmLUT->words.w1);

        if (lShoulder == lForearm) {
            FlexSkeletonHeader *skel = (FlexSkeletonHeader *)(zobj + SEGMENT_OFFSET(*(uintptr_t *)(zobj + Z64O_SKELETON_HEADER_POINTER)));

            LodLimb **limbs = (LodLimb **)(zobj + SEGMENT_OFFSET(skel->sh.segment));

            LodLimb *lShoulderLimb = (LodLimb *)(zobj + SEGMENT_OFFSET(limbs[PLAYER_LIMB_LEFT_SHOULDER - 1]));

            lShoulderLUT->words.w1 = (uintptr_t)lShoulderLimb->dLists[0];
        }
    }
}
