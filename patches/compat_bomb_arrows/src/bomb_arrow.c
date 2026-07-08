#include "./bomb_arrow.h"

#include "overlays/actors/ovl_En_Clear_Tag/z_en_clear_tag.h"

bool BombArrow_IsActiveBombArrowEquip(PlayState* play);
u32 BombArrow_FindAssignedButton();

static EnArrow* sCurrentArrow = NULL;
static PlayState* sCurrentPlayState = NULL;

static BombArrowLink sBombArrowLinks[MAX_BOMB_ARROWS];

static void BombArrow_Link(EnArrow* arrow, EnBom* bomb) {
  for (s32 i = 0; i < MAX_BOMB_ARROWS; i++) {
    if (sBombArrowLinks[i].arrow == NULL) {
      sBombArrowLinks[i].arrow = arrow;
      sBombArrowLinks[i].bomb = bomb;
      sBombArrowLinks[i].loosed = false;
      return;
    }
  }
}

static EnBom* BombArrow_Unlink(EnArrow* arrow) {
  for (s32 i = 0; i < MAX_BOMB_ARROWS; i++) {
    if (sBombArrowLinks[i].arrow == arrow) {
      EnBom* bomb = sBombArrowLinks[i].bomb;
      sBombArrowLinks[i].arrow = NULL;
      sBombArrowLinks[i].bomb = NULL;
      sBombArrowLinks[i].loosed = false;
      return bomb;
    }
  }

  return NULL;
}

static BombArrowLink* BombArrow_FindLinkByArrow(EnArrow* arrow) {
  for (s32 i = 0; i < MAX_BOMB_ARROWS; i++) {
    if (sBombArrowLinks[i].arrow == arrow) {
      return &sBombArrowLinks[i];
    }
  }

  return NULL;
}

static BombArrowLink* BombArrow_FindNockedLink() {
  for (s32 i = 0; i < MAX_BOMB_ARROWS; i++) {
    if ((sBombArrowLinks[i].arrow != NULL) &&
        (sBombArrowLinks[i].bomb != NULL) && !sBombArrowLinks[i].loosed) {
      return &sBombArrowLinks[i];
    }
  }

  return NULL;
}

static BombArrowLink* BombArrow_FindFirstLoosedLink() {
  for (s32 i = 0; i < MAX_BOMB_ARROWS; i++) {
    if ((sBombArrowLinks[i].arrow != NULL) &&
        (sBombArrowLinks[i].bomb != NULL) && sBombArrowLinks[i].loosed) {
      return &sBombArrowLinks[i];
    }
  }

  return NULL;
}

static bool BombArrow_IsLoosed(EnArrow* arrow, PlayState* play) {
  if (arrow == NULL) {
    return false;
  }
  return Actor_HasNoParent(&arrow->actor, play);
}

static void BombArrow_UpdateNockedBombFromPlayerLimb(Player* player,
                                                     s32 limbIndex) {
  if (player == NULL) {
    return;
  }

  if ((limbIndex != PLAYER_LIMB_RIGHT_HAND) ||
      (player->rightHandType != PLAYER_MODELTYPE_RH_BOW)) {
    return;
  }

  BombArrowLink* link = BombArrow_FindNockedLink();

  if ((link == NULL) || (link->bomb == NULL)) {
    return;
  }

  EnBom* bomb = link->bomb;

  /*
   * Local-space offset from the right hand limb.
   */
  static Vec3f sNockedBombOffset = {275, 1000, 275};

  Vec3f bombPos;
  MtxF bombMf;

  Matrix_MultVec3f(&sNockedBombOffset, &bombPos);
  Matrix_Get(&bombMf);

  Actor_SetScale(&bomb->actor, 0.0025f);

  Math_Vec3f_Copy(&bomb->actor.world.pos, &bombPos);
  Math_Vec3f_Copy(&bomb->actor.prevPos, &bombPos);

  Matrix_MtxFToYXZRot(&bombMf, &bomb->actor.world.rot, false);
  Math_Vec3s_Copy(&bomb->actor.shape.rot, &bomb->actor.world.rot);
}

static void InitializeBombArrow(EnArrow* arrow, PlayState* play) {
  if (!CFG_BOMB_ARROWS_ENABLED || (play == NULL) || (arrow == NULL) ||
      arrow->actor.params != ARROW_TYPE_NORMAL || AMMO(ITEM_BOMB) <= 0 ||
      !BombArrow_IsActiveBombArrowEquip(play) ||
      BombArrow_FindLinkByArrow(arrow) != NULL) {
    return;
  }

  EnBom* bomb = (EnBom*)Actor_SpawnAsChild(
      &play->actorCtx, &arrow->actor, play, ACTOR_EN_BOM,
      arrow->actor.world.pos.x, arrow->actor.world.pos.y,
      arrow->actor.world.pos.z, arrow->actor.world.rot.x,
      arrow->actor.world.rot.y, arrow->actor.world.rot.z, BOMB_TYPE_BODY);

  if (bomb != NULL) {
    Actor_SetScale(&bomb->actor, 0.0025f);
    /*
     * Prevent the arrow's collider from immediately hitting its own bomb.
     */
    bomb->collider1.base.acFlags = AC_NONE;
    BombArrow_Link(arrow, bomb);
  }
}

static void BombArrow_Submerge(EnArrow* arrow, PlayState* play) {
  if (arrow == NULL) {
    return;
  }
  BombArrowLink* link = BombArrow_FindLinkByArrow(arrow);
  if (link == NULL) {
    return;
  }
  EnBom* bomb = link->bomb;
  if (bomb == NULL) {
    return;
  }
  if (bomb->actor.depthInWater >= 3.0f) {
    bomb = BombArrow_Unlink(arrow);

    Vec3f effPos;

    effPos.x = bomb->actor.world.pos.x;
    effPos.y = bomb->actor.world.pos.y + bomb->actor.depthInWater;
    effPos.z = bomb->actor.world.pos.z;
    effPos.y += 10.0f;
    EffectSsGSplash_Spawn(play, &effPos, NULL, NULL, 1, 500);
    Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG, effPos.x, effPos.y,
                effPos.z, 0, 0, 1, CLEAR_TAG_PARAMS(CLEAR_TAG_SMOKE));
    SoundSource_PlaySfxAtFixedWorldPos(play, &bomb->actor.world.pos, 30,
                                       NA_SE_IT_BOMB_UNEXPLOSION);
    Actor_Kill(&bomb->actor);
  } else {
    Actor_PlaySfx(&bomb->actor, NA_SE_EV_BOMB_DROP_WATER);
  }
}

static bool BombArrow_IsBombExploding(EnBom* bomb) {
  return (bomb != NULL) && (bomb->actor.params == BOMB_TYPE_EXPLOSION);
}

static void BombArrow_TryDetonate(EnArrow* arrow) {
  if (arrow == NULL) {
    return;
  }
  BombArrowLink* link = BombArrow_FindLinkByArrow(arrow);
  if (link == NULL) {
    return;
  }
  bool loosed = link->loosed;
  EnBom* bomb = BombArrow_Unlink(arrow);
  if (bomb == NULL) {
    return;
  }

  /*
   * The bomb has already entered its explosion actor state.
   * Do not kill it or force timer to 0, because EnBom_Explode needs to run
   * its full countdown to fade the environment lighting back down.
   */
  if (BombArrow_IsBombExploding(bomb)) {
    return;
  }

  if (loosed) {
    /*
     * Arrow either collided with something,
     * or despawned likely because it flew too far.
     * Drop fuse timer to zero, resulting in immediate detonation.
     */
    bomb->timer = 0;
    /*
     * Forcibly despawn arrow to prevent bounce off / geometry embed animation,
     * since the explosion would destroy it.
     */
    Actor_Kill(&arrow->actor);
  } else {
    /*
     * Arrow actor destroyed likely because equipment has been swapped.
     * Despawn bomb.
     */
    Actor_Kill(&bomb->actor);
  }
}

static void BombArrow_UpdateAttachedBomb(EnArrow* arrow, PlayState* play) {
  BombArrowLink* link = BombArrow_FindLinkByArrow(arrow);

  if ((link == NULL) || (link->bomb == NULL)) {
    return;
  }

  EnBom* bomb = link->bomb;

  if (arrow->actor.params == ARROW_TYPE_NORMAL_LIT) {
    /*
     * Arrow has been lit on fire. Detonate bomb.
     */
    BombArrow_TryDetonate(arrow);
  }

  if (!link->loosed) {
    if (!BombArrow_IsLoosed(arrow, play)) {
      return;
    }

    Inventory_ChangeAmmo(ITEM_BOMB, -1);

    /*
     * Bomb is exploding whilst nocked.
     * Expend an arrow also, as the explosion destroys it.
     */
    if (BombArrow_IsBombExploding(bomb)) {
      Inventory_ChangeAmmo(ITEM_BOW, -1);
    }

    link->loosed = true;
  }

  Actor_SetScale(&bomb->actor, 0.0025f);

  /*
   * Local-space offset from the center of the arrow shaft.
   */
  static Vec3f loosedBombOffset = {0, 0, 4};
  Vec3f bombPos;
  MtxF bombMf;

  Matrix_Push();

  Matrix_SetTranslateRotateYXZ(
      arrow->actor.world.pos.x, arrow->actor.world.pos.y,
      arrow->actor.world.pos.z, &arrow->actor.shape.rot);

  Matrix_MultVec3f(&loosedBombOffset, &bombPos);
  Matrix_Get(&bombMf);

  Matrix_Pop();

  Math_Vec3f_Copy(&bomb->actor.world.pos, &bombPos);
  Math_Vec3f_Copy(&bomb->actor.prevPos, &bombPos);

  Matrix_MtxFToYXZRot(&bombMf, &bomb->actor.world.rot, false);
  Math_Vec3s_Copy(&bomb->actor.shape.rot, &bomb->actor.world.rot);
}

static void BombArrow_UpdateNockedEquipState(EnArrow* arrow, PlayState* play) {
  if ((play == NULL) || (arrow == NULL)) {
    return;
  }

  BombArrowLink* link = BombArrow_FindLinkByArrow(arrow);
  bool isLoosed = BombArrow_IsLoosed(arrow, play);
  bool wantsBombArrow = BombArrow_IsActiveBombArrowEquip(play);

  /*
   * Existing bomb arrow is still nocked, but bomb arrows are no longer
   * equipped. Remove only the bomb, leaving the normal arrow nocked.
   */
  if ((link != NULL) && (link->bomb != NULL) && !link->loosed && !isLoosed &&
      !wantsBombArrow) {
    EnBom* bomb = BombArrow_Unlink(arrow);

    if (bomb != NULL) {
      Actor_Kill(&bomb->actor);
    }

    return;
  }

  /*
   * Existing normal arrow is still nocked, and bomb arrows became equipped.
   * Upgrade it in-place by attaching a bomb.
   */
  if ((link == NULL) && !isLoosed && wantsBombArrow &&
      arrow->actor.params == ARROW_TYPE_NORMAL && AMMO(ITEM_BOMB) > 0) {
    InitializeBombArrow(arrow, play);
  }
}

static void BombArrow_RemoteDetonateCheck(PlayState* play, Input* input) {
  BombArrowLink* link = BombArrow_FindFirstLoosedLink();
  if (link == NULL || link->arrow == NULL ||
      !BombArrow_IsActiveBombArrowEquip(play)) {
    return;
  }
  u32 button = BombArrow_FindAssignedButton();

  if (button == 0) {
    return;
  }

  if (CHECK_BTN_ALL(input->press.button, button)) {
    input->press.button &= ~button;
    BombArrow_TryDetonate(link->arrow);
  }
}

RECOMP_HOOK("EnArrow_Init")
void bomb_arrow_init(Actor* thisx, PlayState* play) {
  sCurrentArrow = (EnArrow*)thisx;
  sCurrentPlayState = play;
}

RECOMP_HOOK_RETURN("EnArrow_Init")
void bomb_arrow_init_return() {
  EnArrow* arrow = sCurrentArrow;
  sCurrentArrow = NULL;

  PlayState* play = sCurrentPlayState;
  sCurrentPlayState = NULL;

  InitializeBombArrow(arrow, play);
}

RECOMP_HOOK("Player_PostLimbDrawGameplay")
void bomb_arrow_post_limb_draw(PlayState* play, s32 limbIndex, Gfx** dList1,
                               Gfx** dList2, Vec3s* rot, Actor* actor) {
  BombArrow_UpdateNockedBombFromPlayerLimb((Player*)actor, limbIndex);
}

RECOMP_HOOK("EnArrow_Update")
void bomb_arrow_update(Actor* thisx, PlayState* play) {
  sCurrentArrow = (EnArrow*)thisx;
  sCurrentPlayState = play;
}

RECOMP_HOOK_RETURN("EnArrow_Update")
void bomb_arrow_update_return() {
  EnArrow* arrow = sCurrentArrow;
  sCurrentArrow = NULL;
  PlayState* play = sCurrentPlayState;
  sCurrentPlayState = NULL;
  BombArrow_UpdateNockedEquipState(arrow, play);
  BombArrow_UpdateAttachedBomb(arrow, play);
}

RECOMP_HOOK("EnArrow_Destroy")
void bomb_arrow_destroy(Actor* thisx, PlayState* play) {
  BombArrow_TryDetonate((EnArrow*)thisx);
}

RECOMP_HOOK("func_8088A7D8")
void bomb_arrow_actor_impact(PlayState* play, EnArrow* arrow) {
  BombArrow_TryDetonate(arrow);
}

/*
 * Arrow hit keese/guay
 */
RECOMP_HOOK("func_8088A894")
void bomb_arrow_flyer_impact(EnArrow* arrow, PlayState* play) {
  if (CFG_DETONATE_ON_AIRBORNE_IMPACT) {
    BombArrow_TryDetonate(arrow);
  }
}

RECOMP_HOOK("func_8088AA98")
void bomb_arrow_water_impact_check(EnArrow* arrow, PlayState* play) {
  if (!CFG_DEFUSE_ON_SUBMERGE) {
    return;
  }
  WaterBox* waterBox;
  f32 sp50 = arrow->actor.world.pos.y;

  if (WaterBox_GetSurface1(play, &play->colCtx, arrow->actor.world.pos.x,
                           arrow->actor.world.pos.z, &sp50, &waterBox) &&
      (arrow->actor.world.pos.y < sp50) &&
      !(arrow->actor.bgCheckFlags & BGCHECKFLAG_WATER)) {
    BombArrow_Submerge(arrow, play);
  }
}

RECOMP_HOOK("func_8088B630")
void bomb_arrow_world_impact(EnArrow* arrow, PlayState* play) {
  BombArrow_TryDetonate(arrow);
}

RECOMP_HOOK("Player_UpdateCommon")
void bomb_arrow_player_update_common(Player* this, PlayState* play,
                                     Input* input) {
  if (CFG_ENABLE_REMOTE_DETONATION) {
    BombArrow_RemoteDetonateCheck(play, input);
  }
}
