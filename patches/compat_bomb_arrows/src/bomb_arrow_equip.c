#include "modding.h"
#include "./bomb_arrow.h"
#include "overlays/kaleido_scope/ovl_kaleido_scope/z_kaleido_scope.h"

typedef struct {
  bool valid;
  PlayState* play;
  s32 equipSlot;
  s32 existingBombArrowEquipSlot;
  s32 existingBombEquipSlot;
  s32 entryMainState;
  u8 currentItem;
  u8 currentSlot;
  u8 targetItem;
  u8 targetSlot;
} BombArrowEquipContext;

static BombArrowEquipContext sBombArrowEquipContext;

static s32 BombArrow_GetEquipSlotFromPauseTarget(s32 equipTargetCBtn) {
  switch (equipTargetCBtn) {
    case PAUSE_EQUIP_C_LEFT:
      return EQUIP_SLOT_C_LEFT;

    case PAUSE_EQUIP_C_DOWN:
      return EQUIP_SLOT_C_DOWN;

    case PAUSE_EQUIP_C_RIGHT:
      return EQUIP_SLOT_C_RIGHT;

    default:
      return -1;
  }
}

bool BombArrow_IsEquipSlotBombArrow(s32 equipSlot) {
  return ((BUTTON_ITEM_EQUIP(0, equipSlot) & 0xFF) == ITEM_BOW) &&
         ((C_SLOT_EQUIP(0, equipSlot) & 0xFF) == SLOT_BOMB);
}

static bool BombArrow_IsEquip(u8 item, u8 slot) {
  return (item == ITEM_BOW) && (slot == SLOT_BOMB);
}

static bool BombArrow_IsNormalBowEquip(u8 item, u8 slot) {
  return (item == ITEM_BOW) && (slot == SLOT_BOW);
}

static bool BombArrow_IsBombEquip(u8 item, u8 slot) {
  return (item == ITEM_BOMB) && (slot == SLOT_BOMB);
}

static bool BombArrow_IsMagicBowEquip(u8 item, u8 slot) {
  return (slot == SLOT_BOW) && (item >= ITEM_BOW_FIRE) &&
         (item <= ITEM_BOW_LIGHT);
}

static bool BombArrow_IsAnyBowEquip(u8 item, u8 slot) {
  return BombArrow_IsNormalBowEquip(item, slot) ||
         BombArrow_IsMagicBowEquip(item, slot) || BombArrow_IsEquip(item, slot);
}

static EquipSlot sCEquipSlots[] = {
    EQUIP_SLOT_C_LEFT,
    EQUIP_SLOT_C_DOWN,
    EQUIP_SLOT_C_RIGHT,
};

static EquipSlot BombArrow_FindBombArrowEquipSlot(void) {
  for (s32 i = 0; i < ARRAY_COUNT(sCEquipSlots); i++) {
    if (BombArrow_IsEquipSlotBombArrow(sCEquipSlots[i])) {
      return sCEquipSlots[i];
    }
  }
  return EQUIP_SLOT_NONE;
}

static EquipSlot BombArrow_FindBombEquipSlot() {
  for (s32 i = 0; i < ARRAY_COUNT(sCEquipSlots); i++) {
    s32 equipSlot = sCEquipSlots[i];
    u8 item = BUTTON_ITEM_EQUIP(0, equipSlot) & 0xFF;
    u8 slot = C_SLOT_EQUIP(0, equipSlot) & 0xFF;

    if (BombArrow_IsBombEquip(item, slot)) {
      return equipSlot;
    }
  }
  return EQUIP_SLOT_NONE;
}

// static u32 sCButtons[] = {BTN_CLEFT, BTN_CDOWN, BTN_CRIGHT};

u32 BombArrow_FindAssignedButton() {
  EquipSlot slot = BombArrow_FindBombArrowEquipSlot();
  u32 button = 0;
  switch (slot) {
    case EQUIP_SLOT_B:
      button = BTN_B;
    case EQUIP_SLOT_C_LEFT:
      button = BTN_CLEFT;
      break;
    case EQUIP_SLOT_C_DOWN:
      button = BTN_CDOWN;
      break;
    case EQUIP_SLOT_C_RIGHT:
      button = BTN_CRIGHT;
      break;
    default:
      break;
  }
  return button;
}

static void BombArrow_ResolveEquip(u8 currentItem, u8 currentSlot,
                                   u8 targetItem, u8 targetSlot,
                                   u8* resolvedItem, u8* resolvedSlot) {
  *resolvedItem = targetItem;
  *resolvedSlot = targetSlot;

  /*
   * Bomb arrows already assigned: assigning bow or bombs replaces them with the
   * normal item.
   */
  if (BombArrow_IsEquip(currentItem, currentSlot)) {
    if (targetItem == ITEM_BOW) {
      *resolvedItem = ITEM_BOW;
      *resolvedSlot = SLOT_BOW;
      return;
    }

    if (targetItem == ITEM_BOMB) {
      *resolvedItem = ITEM_BOMB;
      *resolvedSlot = SLOT_BOMB;
      return;
    }
  }

  /*
   * Bow + bombs on the same C-button becomes bomb arrows.
   */
  if (BombArrow_IsAnyBowEquip(currentItem, currentSlot) &&
      BombArrow_IsBombEquip(targetItem, targetSlot)) {
    *resolvedItem = ITEM_BOW;
    *resolvedSlot = SLOT_BOMB;
    return;
  }

  /*
   * Bombs + bow on the same C-button becomes bomb arrows.
   */
  if (BombArrow_IsBombEquip(currentItem, currentSlot) &&
      BombArrow_IsNormalBowEquip(targetItem, targetSlot)) {
    *resolvedItem = ITEM_BOW;
    *resolvedSlot = SLOT_BOMB;
    return;
  }
}

bool BombArrow_IsActiveBombArrowEquip(PlayState* play) {
  if (!CFG_BOMB_ARROWS_ENABLED) {
    return false;
  }

  if (play == NULL) {
    return false;
  }

  Player* player = GET_PLAYER(play);

  if (player == NULL) {
    return false;
  }

  switch (player->heldItemButton) {
    case EQUIP_SLOT_C_LEFT:
    case EQUIP_SLOT_C_DOWN:
    case EQUIP_SLOT_C_RIGHT:
      return BombArrow_IsEquipSlotBombArrow(player->heldItemButton);

    default:
      return false;
  }
}

static bool BombArrow_IsUsableDisplacedEquip(u8 item, u8 slot) {
  return (item != ITEM_NONE) && (slot != SLOT_NONE) &&
         !BombArrow_IsAnyBowEquip(item, slot);
}

static void BombArrow_SetEquipSlot(PlayState* play, s32 equipSlot, u8 item,
                                   u8 slot) {
  BUTTON_ITEM_EQUIP(0, equipSlot) = item;
  C_SLOT_EQUIP(0, equipSlot) = slot;
  Interface_LoadItemIconImpl(play, equipSlot);
}

static void BombArrow_EnforceSingleBowEquip(PlayState* play,
                                            s32 targetEquipSlot,
                                            u8 displacedItem,
                                            u8 displacedSlot) {
  static s32 sCEquipSlots[] = {
      EQUIP_SLOT_C_LEFT,
      EQUIP_SLOT_C_DOWN,
      EQUIP_SLOT_C_RIGHT,
  };

  u8 targetItem = BUTTON_ITEM_EQUIP(0, targetEquipSlot) & 0xFF;
  u8 targetSlot = C_SLOT_EQUIP(0, targetEquipSlot) & 0xFF;

  if (!BombArrow_IsAnyBowEquip(targetItem, targetSlot)) {
    return;
  }

  bool targetIsBombArrow = BombArrow_IsEquip(targetItem, targetSlot);
  bool usedDisplacedItem = false;

  for (s32 i = 0; i < ARRAY_COUNT(sCEquipSlots); i++) {
    s32 equipSlot = sCEquipSlots[i];

    if (equipSlot == targetEquipSlot) {
      continue;
    }

    u8 item = BUTTON_ITEM_EQUIP(0, equipSlot) & 0xFF;
    u8 slot = C_SLOT_EQUIP(0, equipSlot) & 0xFF;

    if (!BombArrow_IsAnyBowEquip(item, slot)) {
      continue;
    }

    /*
     * If a normal/magic bow replaces bomb arrows, the old bomb-arrow slot
     * should receive the item displaced from the target slot when possible.
     * If nothing useful was displaced, leave bombs behind.
     */
    if (BombArrow_IsEquip(item, slot)) {
      if (!usedDisplacedItem &&
          BombArrow_IsUsableDisplacedEquip(displacedItem, displacedSlot)) {
        BombArrow_SetEquipSlot(play, equipSlot, displacedItem, displacedSlot);
        usedDisplacedItem = true;
      } else {
        BombArrow_SetEquipSlot(play, equipSlot, ITEM_BOMB, SLOT_BOMB);
      }

      continue;
    }

    /*
     * If bomb arrows replace another bow assignment, the old bow slot becomes
     * bombs so bombs remain separately equippable.
     */
    if (targetIsBombArrow) {
      BombArrow_SetEquipSlot(play, equipSlot, ITEM_BOMB, SLOT_BOMB);
      continue;
    }

    /*
     * Normal bow-family behavior: move the displaced non-bow item into the old
     * bow slot, otherwise clear it.
     */
    if (!usedDisplacedItem &&
        BombArrow_IsUsableDisplacedEquip(displacedItem, displacedSlot)) {
      BombArrow_SetEquipSlot(play, equipSlot, displacedItem, displacedSlot);
      usedDisplacedItem = true;
    } else {
      BombArrow_SetEquipSlot(play, equipSlot, ITEM_NONE, SLOT_NONE);
    }
  }
}

RECOMP_HOOK("KaleidoScope_UpdateItemEquip")
void kaleido_scope_update_item_equip(PlayState* play) {
  sBombArrowEquipContext.valid = false;

  if (!CFG_BOMB_ARROWS_ENABLED || (play == NULL)) {
    return;
  }

  PauseContext* pauseCtx = &play->pauseCtx;
  s32 equipSlot =
      BombArrow_GetEquipSlotFromPauseTarget(pauseCtx->equipTargetCBtn);

  if (equipSlot < 0) {
    return;
  }

  sBombArrowEquipContext.valid = true;
  sBombArrowEquipContext.play = play;
  sBombArrowEquipContext.equipSlot = equipSlot;

  sBombArrowEquipContext.existingBombArrowEquipSlot =
      BombArrow_FindBombArrowEquipSlot();
  sBombArrowEquipContext.existingBombEquipSlot = BombArrow_FindBombEquipSlot();

  sBombArrowEquipContext.entryMainState = pauseCtx->mainState;

  sBombArrowEquipContext.currentItem = BUTTON_ITEM_EQUIP(0, equipSlot) & 0xFF;
  sBombArrowEquipContext.currentSlot = C_SLOT_EQUIP(0, equipSlot) & 0xFF;

  sBombArrowEquipContext.targetItem = pauseCtx->equipTargetItem & 0xFF;
  sBombArrowEquipContext.targetSlot = pauseCtx->equipTargetSlot & 0xFF;
}

RECOMP_HOOK_RETURN("KaleidoScope_UpdateItemEquip")
void kaleido_scope_update_item_equip_return() {
  BombArrowEquipContext ctx = sBombArrowEquipContext;
  sBombArrowEquipContext.valid = false;

  if (!CFG_BOMB_ARROWS_ENABLED || !ctx.valid || (ctx.play == NULL)) {
    return;
  }

  PauseContext* pauseCtx = &ctx.play->pauseCtx;

  /*
   * KaleidoScope_UpdateItemEquip is an animation update. Only override the
   * final result after the original function has actually equipped the item.
   */
  if (pauseCtx->mainState != PAUSE_MAIN_STATE_IDLE) {
    return;
  }

  if (ctx.entryMainState == PAUSE_MAIN_STATE_IDLE) {
    return;
  }

  u8 resolvedItem;
  u8 resolvedSlot;

  BombArrow_ResolveEquip(ctx.currentItem, ctx.currentSlot, ctx.targetItem,
                         ctx.targetSlot, &resolvedItem, &resolvedSlot);

  BUTTON_ITEM_EQUIP(0, ctx.equipSlot) = resolvedItem;
  C_SLOT_EQUIP(0, ctx.equipSlot) = resolvedSlot;

  if ((ctx.existingBombArrowEquipSlot >= 0) &&
      (ctx.existingBombArrowEquipSlot != ctx.equipSlot) &&
      BombArrow_IsBombEquip(resolvedItem, resolvedSlot)) {
    BombArrow_SetEquipSlot(ctx.play, ctx.existingBombArrowEquipSlot, ITEM_BOW,
                           SLOT_BOMB);
  }

  if ((ctx.existingBombEquipSlot >= 0) &&
      (ctx.existingBombEquipSlot != ctx.equipSlot) &&
      BombArrow_IsBombEquip(resolvedItem, resolvedSlot)) {
    if (ctx.existingBombArrowEquipSlot == ctx.equipSlot) {
      BombArrow_SetEquipSlot(ctx.play, ctx.existingBombEquipSlot, ITEM_BOW,
                             SLOT_BOMB);
    } else if (BombArrow_IsUsableDisplacedEquip(ctx.currentItem,
                                                ctx.currentSlot)) {
      BombArrow_SetEquipSlot(ctx.play, ctx.existingBombEquipSlot,
                             ctx.currentItem, ctx.currentSlot);
    } else {
      BombArrow_SetEquipSlot(ctx.play, ctx.existingBombEquipSlot, ITEM_NONE,
                             SLOT_NONE);
    }
  }

  BombArrow_EnforceSingleBowEquip(ctx.play, ctx.equipSlot, ctx.currentItem,
                                  ctx.currentSlot);

  Interface_LoadItemIconImpl(ctx.play, ctx.equipSlot);
}
