#include "global.h"
#include "string.h"
#include "modding.h"
#include "recomputils.h"
#include "rt64_extended_gbi.h"
#include "stdbool.h"
#include "z64animation.h"
#include "helpers.h"
#include "objectmanager.h"
#include "repoint.h"

RECOMP_EXPORT void GlobalObjects_rebaseDL(Gfx *globalPtr, SegmentMap segments) {
    if (!isObjectManagerReady("GlobalObjects_rebaseDL")) {
        return;
    }

    if (!globalPtr) {
        recomp_printf("GlobalObjects_rebaseDL: Incorrectly passed in NULL pointer 0x%X as globalPtr\n", globalPtr);
    }

    if (isSegmentedPtr(globalPtr)) {
        recomp_printf("GlobalObjects_rebaseDL: Incorrectly passed in non-global pointer 0x%X as globalPtr\n", globalPtr);
        return;
    }

    unsigned opcode;

    unsigned currentSegment;

    bool isEndDl = false;

    Gfx *rdpWord1Gfx = NULL;

#define STACK_MAX_SIZE 64
    Gfx *dlStack[STACK_MAX_SIZE];
    dlStack[0] = globalPtr;
    int stackSize = 1;

    while (stackSize > 0) {
        stackSize--;
        globalPtr = dlStack[stackSize];
        // recomp_printf("Popping 0x%X off the return stack\n", dlStack[stackSize]);

        isEndDl = false;

        while (!isEndDl) {
            opcode = globalPtr->words.w0 >> 24;

            currentSegment = globalPtr->words.w1 >> 24;

            void *segmentAddr = currentSegment < 0x10 ? segments[currentSegment] : NULL;

            bool isBranchInstruction;

            switch (opcode) {
                case G_ENDDL:
                    // recomp_printf("Processing %llX\n", *((u64 *)globalPtr));
                    isEndDl = true;
                    break;

                case G_RDPHALF_1:
                    rdpWord1Gfx = globalPtr;
                    break;

                case G_BRANCH_Z:
                    if (rdpWord1Gfx) {
                        // recomp_printf("Processing %llX\n", *((u64 *)globalPtr));
                        currentSegment = rdpWord1Gfx->words.w1 >> 24;
                        segmentAddr = currentSegment < 0x10 ? segments[currentSegment] : NULL;

                        if (segmentAddr) {
                            if (stackSize < STACK_MAX_SIZE) {
                                dlStack[stackSize++] = globalPtr + 1;
                            }

                            Gfx *newGlobal = TO_GLOBAL_PTR(segmentAddr, rdpWord1Gfx->words.w1);
                            rdpWord1Gfx->words.w1 = (uintptr_t)newGlobal;
                            globalPtr = newGlobal - 1;
                        }
                    }
                    break;

                case G_DL:
                    // recomp_printf("%llX\n", *((u64 *)globalPtr));
                    isBranchInstruction = ((globalPtr->words.w0 >> 16) & 0xFF) == G_DL_NOPUSH;

                    if (segmentAddr) {
                        Gfx *newGlobal = TO_GLOBAL_PTR(segmentAddr, globalPtr->words.w1);

                        if (stackSize < STACK_MAX_SIZE && !isBranchInstruction) {
                            // recomp_printf("Pushing 0x%X (0x%X) to the return stack\n", globalPtr + 1, SEGMENT_ADDR(currentSegment, (uintptr_t)(globalPtr + 1) - (uintptr_t)segmentAddr));
                            dlStack[stackSize] = globalPtr + 1;
                            stackSize++;
                        } else {
                            // recomp_printf("No push 0x%X (0x%X) to the return stack\n", globalPtr + 1, SEGMENT_ADDR(currentSegment, (uintptr_t)(globalPtr + 1) - (uintptr_t)segmentAddr));
                        }

                        // recomp_printf("Repoint 0x0%X -> 0x%X\n", globalPtr->words.w1, newGlobal);
                        globalPtr->words.w1 = (uintptr_t)newGlobal;
                        globalPtr = newGlobal - 1;
                    } else if (isBranchInstruction) {
                        isEndDl = true;
                    }
                    break;

                case G_VTX:
                case G_MTX:
                case G_SETTIMG:
                case G_SETZIMG:
                case G_SETCIMG:
                case G_MOVEMEM:
                    // recomp_printf("Processing %llX\n", *((u64 *)globalPtr));
                    if (segmentAddr) {
                        // recomp_printf("Repoint 0x0%X -> 0x%X\n", globalPtr->words.w1, TO_GLOBAL_PTR(segmentAddr, globalPtr->words.w1));
                        globalPtr->words.w1 = (uintptr_t)TO_GLOBAL_PTR(segmentAddr, globalPtr->words.w1);
                    }
                    break;

                default:
                    break;
            }

            globalPtr++;
        }
    }

#undef STACK_MAX_SIZE
}

static SegmentMap sGlobalizeSegmentedDLMap = {0};

RECOMP_EXPORT void GlobalObjects_globalizeSegmentedDL(void *obj, Gfx *segmentedPtr) {
    if (!isObjectManagerReady("GlobalObjects_globalizeSegmentedDL")) {
        return;
    }

    if (!isSegmentedPtr(segmentedPtr)) {
        recomp_printf("GlobalObjects_globalizeSegmentedDL: Incorrectly passed in global pointer 0x%X as segmentedPtr\n", segmentedPtr);
        return;
    }

    unsigned segment = (uintptr_t)segmentedPtr >> 24;
    sGlobalizeSegmentedDLMap[segment] = obj;

    GlobalObjects_rebaseDL(TO_GLOBAL_PTR(obj, segmentedPtr), sGlobalizeSegmentedDLMap);

    sGlobalizeSegmentedDLMap[segment] = NULL;
}

RECOMP_EXPORT void GlobalObjects_globalizeLodLimbSkeleton(void *obj, FlexSkeletonHeader *skel) {
    if (!skel) {
        recomp_printf("GlobalObjects_globalizeStandardLimbSkeleton: FlexSkeletonHeader skel is NULL!\n");
        return;
    }

    skel = TO_GLOBAL_PTR(obj, skel);

    skel->sh.segment = TO_GLOBAL_PTR(obj, skel->sh.segment);

    LodLimb **limbs = (LodLimb **)skel->sh.segment;

    LodLimb *limb;

    u8 limbCount = skel->sh.limbCount;

    for (u8 i = 0; i < limbCount; ++i) {
        limb = TO_GLOBAL_PTR(obj, limbs[i]);

        if (limb) {
            if (limb->dLists[0]) { // do not repoint limbs without display lists
                limb->dLists[0] = TO_GLOBAL_PTR(obj, limb->dLists[0]);
            }

            if (limb->dLists[1]) {
                limb->dLists[1] = TO_GLOBAL_PTR(obj, limb->dLists[1]);
            }

            limbs[i] = limb;
        }
    }
}

RECOMP_EXPORT void GlobalObjects_globalizeStandardLimbSkeleton(void *obj, FlexSkeletonHeader *skel) {
    if (!skel) {
        recomp_printf("GlobalObjects_globalizeStandardLimbSkeleton: FlexSkeletonHeader skel is NULL!");
        return;
    }

    skel = TO_GLOBAL_PTR(obj, skel);

    skel->sh.segment = TO_GLOBAL_PTR(obj, skel->sh.segment);

    StandardLimb **limbs = (StandardLimb **)skel->sh.segment;

    StandardLimb *limb;

    u8 limbCount = skel->sh.limbCount;

    for (u8 i = 0; i < limbCount; ++i) {
        limb = TO_GLOBAL_PTR(obj, limbs[i]);

        if (limb) {
            if (limb->dList) { // do not repoint limbs without display lists
                limb->dList = TO_GLOBAL_PTR(obj, limb->dList);
            }

            limbs[i] = limb;
        }
    }
}
