#include "global.h"
#include "modding.h"

static Player *sPlayer;
static u16 sSfxId;

RECOMP_HOOK("Player_AnimSfx_PlayVoice") void on_Player_AnimSfx_PlayVoice(Player *player, u16 sfxId) {
    sPlayer = player;
    sSfxId = sfxId;
}

void playAdultLinkVoice_on_return_Player_AnimSfx_PlayVoice(Player *player, u16 sfxId);

RECOMP_HOOK_RETURN("Player_AnimSfx_PlayVoice") void on_return_Player_AnimSfx_PlayVoice(void) {
    playAdultLinkVoice_on_return_Player_AnimSfx_PlayVoice(sPlayer, sSfxId);
}
