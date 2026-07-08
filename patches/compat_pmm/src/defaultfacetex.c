#include "global.h"
#include "defaultfacetex.h"
#include "assets/objects/object_link_child/object_link_child.h"

TexturePtr gDefaultEyesTextures[PLAYER_EYES_MAX] = {
    gLinkHumanEyesOpenTex,        // PLAYER_EYES_OPEN
    gLinkHumanEyesHalfTex,        // PLAYER_EYES_HALF
    gLinkHumanEyesClosedTex,      // PLAYER_EYES_CLOSED
    gLinkHumanEyesRollRightTex,   // PLAYER_EYES_ROLL_RIGHT
    gLinkHumanEyesRollLeftTex,    // PLAYER_EYES_ROLL_LEFT
    gLinkHumanEyesRollUpTex,      // PLAYER_EYES_ROLL_UP
    gLinkHumanEyesRollDownTex,    // PLAYER_EYES_ROLL_DOWN
    object_link_child_Tex_003800, // PLAYER_EYES_7
};

TexturePtr gDefaultMouthTextures[PLAYER_MOUTH_MAX] = {
    gLinkHumanMouthClosedTex, // PLAYER_MOUTH_CLOSED
    gLinkHumanMouthTeethTex,  // PLAYER_MOUTH_TEETH
    gLinkHumanMouthAngryTex,  // PLAYER_MOUTH_ANGRY
    gLinkHumanMouthHappyTex,  // PLAYER_MOUTH_HAPPY
};
