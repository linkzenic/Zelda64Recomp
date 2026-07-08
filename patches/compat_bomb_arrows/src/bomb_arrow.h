#ifndef BOMB_ARROW_H_
#define BOMB_ARROW_H_

#include "modding.h"
#include "recompconfig.h"

#ifndef ARROW_LIMB_MAX
#define ARROW_LIMB_MAX 5
#endif

#include "overlays/actors/ovl_En_Arrow/z_en_arrow.h"
#include "overlays/actors/ovl_En_Bom/z_en_bom.h"

// NOLINTBEGIN(readability/casting)
#define CFG_BOMB_ARROWS_ENABLED \
  (bool)recomp_get_config_u32("enabled")
#define CFG_DEFUSE_ON_SUBMERGE \
  (bool)recomp_get_config_u32("defuse_on_submerge")
#define CFG_DETONATE_ON_AIRBORNE_IMPACT \
  (bool)recomp_get_config_u32("detonate_on_airborne_impact")
#define CFG_ENABLE_REMOTE_DETONATION \
  (bool)recomp_get_config_u32("enable_remote_detonation")
// NOLINTEND

typedef struct {
  EnArrow* arrow;
  EnBom* bomb;
  bool loosed;
} BombArrowLink;

#define MAX_BOMB_ARROWS 16

#endif  // BOMB_ARROW_H_
