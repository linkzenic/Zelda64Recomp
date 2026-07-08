#include "global.h"
#include "modding.h"

typedef struct KnobDoorActor KnobDoorActor;

static PlayState *sPlay;
static Player *sPlayer;
static KnobDoorActor *sKnobDoorActor;

void replaceDoorAnim_on_Player_Door_Knob(Player *player);

RECOMP_HOOK("Player_Door_Knob") void on_Player_Door_Knob(PlayState *play, Player *this, Actor *door) {
    sPlay = play;
    sPlayer = this;
    sKnobDoorActor = (KnobDoorActor *)door;

    replaceDoorAnim_on_Player_Door_Knob(sPlayer);
}

void replaceDoorAnim_on_return_Player_Door_Knob(Player *player);

RECOMP_HOOK_RETURN("Player_Door_Knob") void on_return_Player_Door_Knob(void) {
    replaceDoorAnim_on_return_Player_Door_Knob(sPlayer);
}
