#include "modding.h"
#include "global.h"
#include "utils.h"
#include "globalobjects_api.h"
#include "modelmatrixids.h"
#include "playermodelmanager_api.h"
#include "customdls.h"
#include "apilocal.h"
#include "fallbackmodels.h"
#include "logger.h"
#include "modelentrymanager.h"

PlayerModelManagerHandle gKafeiModelHandle;

void registerKafei(void) {
    extern FlexSkeletonHeader gKafeiSkel;
    extern u64 gKafeiEyesOpenTex[];
    extern u64 gKafeiEyesHalfTex[];
    extern u64 gKafeiEyesClosedTex[];
    extern u64 gKafeiEyesRollRightTex[];
    extern u64 gKafeiEyesRollLeftTex[];
    extern u64 gKafeiEyesRollUpTex[];
    extern u64 gKafeiEyesRollDownTex[];
    extern u64 object_test3_Tex_006680[];
    static TexturePtr eyeTextures[] = {
        gKafeiEyesOpenTex,
        gKafeiEyesHalfTex,
        gKafeiEyesClosedTex,
        gKafeiEyesRollRightTex,
        gKafeiEyesRollLeftTex,
        gKafeiEyesRollUpTex,
        gKafeiEyesRollDownTex,
        object_test3_Tex_006680,
    };
    extern u64 gKafeiMouthClosedTex[];
    extern u64 gKafeiMouthTeethTex[];
    extern u64 gKafeiMouthAngryTex[];
    extern u64 gKafeiMouthHappyTex[];
    static TexturePtr mouthTextures[] = {
        gKafeiMouthClosedTex,
        gKafeiMouthTeethTex,
        gKafeiMouthAngryTex,
        gKafeiMouthHappyTex,
    };

    void *kafei = GlobalObjects_getGlobalObject(OBJECT_TEST3);

    FlexSkeletonHeader *skel = SEGMENTED_TO_GLOBAL_PTR(kafei, &gKafeiSkel);

    gKafeiModelHandle = PlayerModelManager_registerModel(PMM_API_VERSION, "__mm_object_test3__", PMM_MODEL_TYPE_CHILD);

    FallbackModelsCommon_doCommonAssignments(gKafeiModelHandle, skel, kafei, GlobalObjects_getGlobalObject(GAMEPLAY_KEEP));

    for (int i = 0; i < PLAYER_EYES_MAX; ++i) {
        eyeTextures[i] = SEGMENTED_TO_GLOBAL_PTR(kafei, eyeTextures[i]);
    }

    for (int i = 0; i < PLAYER_MOUTH_MAX; ++i) {
        mouthTextures[i] = SEGMENTED_TO_GLOBAL_PTR(kafei, mouthTextures[i]);
    }

    PlayerModelManager_setEyesTextures(gKafeiModelHandle, eyeTextures);
    PlayerModelManager_setMouthTextures(gKafeiModelHandle, mouthTextures);
}
