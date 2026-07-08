#ifndef ZELDA_COMPAT_PMM_BUILD_SHIMS_H
#define ZELDA_COMPAT_PMM_BUILD_SHIMS_H

typedef struct KeyFrame {
    char unused;
} KeyFrame;
typedef struct KeyFrameAnimation {
    char unused;
} KeyFrameAnimation;
typedef struct KeyFrameFlexLimb {
    char unused;
} KeyFrameFlexLimb;
typedef struct KeyFrameFlexSkeleton {
    char unused;
} KeyFrameFlexSkeleton;

#ifndef SFX_VOICE_BANK_SIZE
#define SFX_VOICE_BANK_SIZE 0x20
#endif

#define zoraBoomerangActor boomerangActor

#ifndef MATRIX_FINALIZE_AND_LOAD
#define MATRIX_FINALIZE_AND_LOAD(pkt, gfxCtx) \
    gSPMatrix((pkt), Matrix_NewMtx(gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW)
#endif

#endif
