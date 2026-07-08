#ifndef __PLAYERMODELMANAGER_API__
#define __PLAYERMODELMANAGER_API__

typedef unsigned long PlayerModelManagerHandle;

// Used for keeping compatibility between versions
// DO NOT EDIT
#define PMM_API_VERSION 3UL

#define YAZMT_PMM_MOD_NAME "yazmt_mm_playermodelmanager"

#define PMM_MAX_INTERNAL_NAME_LENGTH 64
#define PMM_MAX_AUTHOR_NAME_LENGTH 64
#define PMM_MAX_DISPLAY_NAME_LENGTH 32

// NOTE: If a DL is prefixed with PMM_DL_OPT_, specifying it is OPTIONAL,
// and a fallback will be used if available.
// (e.g. not specifying a DL for PMM_DL_OPT_RHAND_OCARINA will result in
// the DL registered to PMM_DL_RHAND being used)

// Swords:
//      1: Kokiri Sword
//      2: Razor Sword
//      3: Gilded Sword
//      4: Fierce Deity's Sword
//      5: Great Fairy Sword

// Shields:
//      1: Deku Shield
//      2: Hero's Shield
//      3: Mirror Shield

typedef enum PlayerModelManagerDisplayListId {

    // Main skeleton DLs
    PMM_DL_WAIST = 0,
    PMM_DL_RTHIGH,
    PMM_DL_RSHIN,
    PMM_DL_RFOOT,
    PMM_DL_LTHIGH,
    PMM_DL_LSHIN,
    PMM_DL_LFOOT,
    PMM_DL_HEAD,
    PMM_DL_HAT,
    PMM_DL_COLLAR,
    PMM_DL_LSHOULDER,
    PMM_DL_LFOREARM,
    PMM_DL_LHAND,
    PMM_DL_RSHOULDER,
    PMM_DL_RFOREARM,
    PMM_DL_RHAND,
    PMM_DL_SHEATH_NONE,
    PMM_DL_TORSO,

    // Other hand DLs
    PMM_DL_OPT_LFIST,
    PMM_DL_OPT_LHAND_BOTTLE,
    PMM_DL_OPT_LHAND_GUITAR,
    PMM_DL_OPT_LFIST_ITEM = 235,
    PMM_DL_OPT_LFIST_SWORD,
    PMM_DL_OPT_LFIST_SWORD1,
    PMM_DL_OPT_LFIST_SWORD2,
    PMM_DL_OPT_LFIST_SWORD3,
    PMM_DL_OPT_LFIST_SWORD_TWO_HANDED,
    PMM_DL_OPT_LFIST_SWORD4,
    PMM_DL_OPT_LFIST_SWORD5,
    PMM_DL_OPT_LFIST_DEKU_STICK,
    PMM_DL_OPT_LFIST_HAMMER,
    PMM_DL_OPT_LFIST_BOOMERANG,
    PMM_DL_OPT_RFIST = 21,
    PMM_DL_OPT_RHAND_OCARINA = 246,
    PMM_DL_OPT_RHAND_OCARINA_FAIRY,
    PMM_DL_OPT_RHAND_OCARINA_TIME,
    PMM_DL_OPT_RFIST_ITEM,
    PMM_DL_OPT_RFIST_SHIELD,
    PMM_DL_OPT_RFIST_SHIELD1,
    PMM_DL_OPT_RFIST_SHIELD2,
    PMM_DL_OPT_RFIST_SHIELD3,
    PMM_DL_OPT_RFIST_SWORD_TWO_HANDED,
    PMM_DL_OPT_RFIST_SWORD4,
    PMM_DL_OPT_RFIST_SWORD5,
    PMM_DL_OPT_RFIST_DEKU_STICK,
    PMM_DL_OPT_RFIST_HAMMER,
    PMM_DL_OPT_RFIST_HOOKSHOT,
    PMM_DL_OPT_RFIST_LONGSHOT,
    PMM_DL_OPT_RFIST_BOW,
    PMM_DL_OPT_RFIST_SLINGSHOT,

    PMM_DL_SWORD1_SHEATH = 22,
    PMM_DL_SWORD2_SHEATH,
    PMM_DL_SWORD3_SHEATH,
    PMM_DL_SWORD4_SHEATH,
    PMM_DL_SWORD5_SHEATH,
    PMM_DL_SWORD1_HILT,
    PMM_DL_SWORD2_HILT,
    PMM_DL_SWORD3_HILT,
    PMM_DL_SWORD4_HILT,
    PMM_DL_SWORD5_HILT,
    PMM_DL_SWORD1_BLADE,
    PMM_DL_SWORD2_BLADE,
    PMM_DL_SWORD3_BLADE,
    PMM_DL_SWORD4_BLADE,
    PMM_DL_SWORD4_BLADE_BROKEN,
    PMM_DL_SWORD4_BLADE_FRAGMENT,
    PMM_DL_SWORD5_BLADE,

    PMM_DL_SHIELD1,
    PMM_DL_SHIELD2,
    PMM_DL_SHIELD3,
    // Mirror Shield projection flat image DL
    // should be axis-aligned
    PMM_DL_SHIELD3_RAY,
    PMM_DL_SHIELD3_RAY_BEAM = 229, // Mirror Shield Reflective Beam DL

    // Items
    PMM_DL_BOTTLE_GLASS = 43,
    PMM_DL_BOTTLE_CONTENTS,
    PMM_DL_OCARINA_FAIRY,
    PMM_DL_OCARINA_TIME,
    PMM_DL_DEKU_STICK,
    PMM_DL_BOW,
    PMM_DL_OPT_FPS_BOW = 226,
    PMM_DL_BOW_STRING = 49,
    PMM_DL_BOW_ARROW,
    PMM_DL_SLINGSHOT,
    PMM_DL_OPT_FPS_SLINGSHOT = 227,
    PMM_DL_SLINGSHOT_STRING = 52,
    PMM_DL_HOOKSHOT,
    PMM_DL_HOOKSHOT_CHAIN,
    PMM_DL_HOOKSHOT_HOOK,
    PMM_DL_HOOKSHOT_RETICLE,
    PMM_DL_OPT_FPS_HOOKSHOT,
    PMM_DL_OPT_LONGSHOT = 230,
    PMM_DL_OPT_LONGSHOT_CHAIN,
    PMM_DL_OPT_LONGSHOT_HOOK,
    PMM_DL_OPT_LONGSHOT_RETICLE,
    PMM_DL_OPT_FPS_LONGSHOT,
    PMM_DL_BOOMERANG = 58,   // Boomerang model while held in hand
    PMM_DL_BOOMERANG_FLYING, // Boomerang model after being thrown
    PMM_DL_HAMMER,
    PMM_DL_BOMB_BODY_2D = 291,
    PMM_DL_BOMB_BODY_3D,
    PMM_DL_BOMB_CAP,
    PMM_DL_BOMBCHU = 297,

    // First Person
    PMM_DL_OPT_FPS_LSHOULDER = 263,
    PMM_DL_OPT_FPS_LSHOULDER_HOOKSHOT,
    PMM_DL_OPT_FPS_LSHOULDER_LONGSHOT,
    PMM_DL_OPT_FPS_LSHOULDER_BOW,
    PMM_DL_OPT_FPS_LSHOULDER_SLINGSHOT,

    PMM_DL_OPT_FPS_LFOREARM = 61,
    PMM_DL_OPT_FPS_LFOREARM_HOOKSHOT = 268,
    PMM_DL_OPT_FPS_LFOREARM_LONGSHOT,
    PMM_DL_OPT_FPS_LFOREARM_BOW,
    PMM_DL_OPT_FPS_LFOREARM_SLINGSHOT,

    PMM_DL_OPT_FPS_LHAND = 62,
    PMM_DL_OPT_FPS_LHAND_HOOKSHOT = 272,
    PMM_DL_OPT_FPS_LHAND_LONGSHOT,
    PMM_DL_OPT_FPS_LHAND_BOW,
    PMM_DL_OPT_FPS_LHAND_SLINGSHOT,

    PMM_DL_OPT_FPS_RSHOULDER,
    PMM_DL_OPT_FPS_RSHOULDER_HOOKSHOT,
    PMM_DL_OPT_FPS_RSHOULDER_LONGSHOT,
    PMM_DL_OPT_FPS_RSHOULDER_BOW,
    PMM_DL_OPT_FPS_RSHOULDER_SLINGSHOT,

    PMM_DL_OPT_FPS_RFOREARM = 63,
    PMM_DL_OPT_FPS_RFOREARM_HOOKSHOT = 281,
    PMM_DL_OPT_FPS_RFOREARM_LONGSHOT,
    PMM_DL_OPT_FPS_RFOREARM_BOW,
    PMM_DL_OPT_FPS_RFOREARM_SLINGSHOT,

    PMM_DL_OPT_FPS_RHAND = 64,
    PMM_DL_OPT_FPS_RHAND_HOOKSHOT = 285,
    PMM_DL_OPT_FPS_RHAND_LONGSHOT,
    PMM_DL_OPT_FPS_RHAND_BOW,
    PMM_DL_OPT_FPS_RHAND_SLINGSHOT,

    // Masks
    PMM_DL_MASK_SKULL = 65,
    PMM_DL_MASK_SPOOKY,
    PMM_DL_MASK_GERUDO,
    PMM_DL_MASK_TRUTH,
    PMM_DL_MASK_KAFEIS_MASK,
    PMM_DL_MASK_ALL_NIGHT,
    PMM_DL_MASK_BUNNY, // May look odd due to animation
    PMM_DL_MASK_KEATON,
    PMM_DL_MASK_GARO,
    PMM_DL_MASK_ROMANI,
    PMM_DL_MASK_CIRCUS_LEADER,
    PMM_DL_MASK_COUPLE,
    PMM_DL_MASK_POSTMAN,
    PMM_DL_MASK_GREAT_FAIRY, // May look odd due to animation
    PMM_DL_MASK_GIBDO,
    PMM_DL_MASK_DON_GERO,
    PMM_DL_MASK_KAMARO,
    PMM_DL_MASK_CAPTAIN,
    PMM_DL_MASK_STONE,
    PMM_DL_MASK_BREMEN,
    PMM_DL_MASK_BLAST,
    PMM_DL_MASK_BLAST_COOLING_DOWN,
    PMM_DL_MASK_SCENTS,
    PMM_DL_MASK_GIANT,
    PMM_DL_MASK_DEKU,                // Only used if model is Deku
    PMM_DL_MASK_GORON,               // Only used if model is Goron
    PMM_DL_MASK_ZORA,                // Only used if model is Zora
    PMM_DL_MASK_FIERCE_DEITY,        // Only used if model is Fierce Deity
    PMM_DL_MASK_DEKU_SCREAM,         // Only used if model is Deku
    PMM_DL_MASK_GORON_SCREAM,        // Only used if model is Goron
    PMM_DL_MASK_ZORA_SCREAM,         // Only used if model is Zora
    PMM_DL_MASK_FIERCE_DEITY_SCREAM, // Only used if model is Fierce Deity

    // Elegy of Emptiness statues
    PMM_DL_ELEGY_OF_EMPTINESS_SHELL_HUMAN,              // Only used if model is human
    PMM_DL_ELEGY_OF_EMPTINESS_SHELL_DEKU,               // Only used if model is Deku
    PMM_DL_ELEGY_OF_EMPTINESS_SHELL_GORON,              // Only used if model is Goron
    PMM_DL_ELEGY_OF_EMPTINESS_SHELL_ZORA,               // Only used if model is Zora
    PMM_DL_ELEGY_OF_EMPTINESS_SHELL_FIERCE_DEITY = 228, // Only used if model is Fierce Deity

    // Strength Upgrades (OoT)
    PMM_DL_BRACELET_LFOREARM = 101,
    PMM_DL_GAUNTLET_LFOREARM,
    PMM_DL_GAUNTLET_LHAND,
    PMM_DL_GAUNTLET_LFIST,
    PMM_DL_GAUNTLET_RFOREARM,
    PMM_DL_GAUNTLET_RHAND,
    PMM_DL_GAUNTLET_RFIST,

    // Boots (OoT)
    PMM_DL_BOOT_LIRON,
    PMM_DL_BOOT_RIRON,
    PMM_DL_BOOT_LHOVER,
    PMM_DL_BOOT_RHOVER,

    // Deku Link shield
    PMM_DL_DEKU_GUARD,

    // Deku Instrument / Pipes
    PMM_DL_PIPE_MOUTH,
    PMM_DL_PIPE_RIGHT,
    PMM_DL_PIPE_UP,
    PMM_DL_PIPE_DOWN,
    PMM_DL_PIPE_LEFT,
    PMM_DL_PIPE_A,

    // Deku Propellers
    PMM_DL_STEM_RIGHT,
    PMM_DL_STEM_LEFT,
    PMM_DL_PETAL_PARTICLE,
    PMM_DL_FLOWER_PROPELLER_CLOSED,
    PMM_DL_FLOWER_CENTER_CLOSED,
    PMM_DL_FLOWER_PROPELLER_OPEN,
    PMM_DL_FLOWER_CENTER_OPEN,

    // Burrowed into a Deku Flower models
    PMM_DL_PAD_WOOD,
    PMM_DL_PAD_GRASS,
    PMM_DL_PAD_OPENING,

    // Goron Instrument / Drums
    PMM_DL_DRUM_STRAP,
    PMM_DL_DRUM_UP,
    PMM_DL_DRUM_LEFT,
    PMM_DL_DRUM_RIGHT,
    PMM_DL_DRUM_DOWN,
    PMM_DL_DRUM_A,

    // Goron shield animation skeleton DLs
    PMM_DL_BODY_SHIELD_HEAD,
    PMM_DL_BODY_SHIELD_BODY,
    PMM_DL_BODY_SHIELD_ARMS_AND_LEGS,

    // Goron Roll / Goron Magic DLs
    PMM_DL_CURLED,
    PMM_DL_SPIKES,
    PMM_DL_FIRE_INIT,
    PMM_DL_FIRE_ROLL,
    PMM_DL_FIRE_PUNCH,

    // Zora Fins
    PMM_DL_LFIN,
    PMM_DL_LFIN_SWIM,
    PMM_DL_LFIN_BOOMERANG,
    PMM_DL_RFIN,
    PMM_DL_RFIN_SWIM,
    PMM_DL_RFIN_BOOMERANG,
    PMM_DL_FIN_SHIELD,

    // Zora Magic
    PMM_DL_MAGIC_BARRIER,

    // Zora Instrument / Guitar
    PMM_DL_GUITAR,

    // Hilt + Blade
    PMM_DL_SHIM_SWORD1,
    PMM_DL_SHIM_SWORD2,
    PMM_DL_SHIM_SWORD3,
    PMM_DL_SHIM_SWORD4,
    PMM_DL_SHIM_SWORD4_BROKEN,
    PMM_DL_SHIM_SWORD5,

    // Hilt + Back Matrix
    PMM_DL_SHIM_SWORD1_HILT_BACK,
    PMM_DL_SHIM_SWORD2_HILT_BACK,
    PMM_DL_SHIM_SWORD3_HILT_BACK,
    PMM_DL_SHIM_SWORD4_HILT_BACK,
    PMM_DL_SHIM_SWORD5_HILT_BACK,

    // Hilt w/ Back Matrix + Sheath
    PMM_DL_SHIM_SWORD1_SHEATHED,
    PMM_DL_SHIM_SWORD2_SHEATHED,
    PMM_DL_SHIM_SWORD3_SHEATHED,
    PMM_DL_SHIM_SWORD4_SHEATHED,
    PMM_DL_SHIM_SWORD5_SHEATHED,

    // Shield w/ Back Matrix
    PMM_DL_SHIM_SHIELD1_BACK,
    PMM_DL_SHIM_SHIELD2_BACK,
    PMM_DL_SHIM_SHIELD3_BACK,

    // Sheath + Shield w/ Back Matrix
    PMM_DL_SHIM_SWORD1_SHIELD1_SHEATH,
    PMM_DL_SHIM_SWORD1_SHIELD2_SHEATH,
    PMM_DL_SHIM_SWORD1_SHIELD3_SHEATH,
    PMM_DL_SHIM_SWORD2_SHIELD1_SHEATH,
    PMM_DL_SHIM_SWORD2_SHIELD2_SHEATH,
    PMM_DL_SHIM_SWORD2_SHIELD3_SHEATH,
    PMM_DL_SHIM_SWORD3_SHIELD1_SHEATH,
    PMM_DL_SHIM_SWORD3_SHIELD2_SHEATH,
    PMM_DL_SHIM_SWORD3_SHIELD3_SHEATH,
    PMM_DL_SHIM_SWORD4_SHIELD1_SHEATH,
    PMM_DL_SHIM_SWORD4_SHIELD2_SHEATH,
    PMM_DL_SHIM_SWORD4_SHIELD3_SHEATH,
    PMM_DL_SHIM_SWORD5_SHIELD1_SHEATH,
    PMM_DL_SHIM_SWORD5_SHIELD2_SHEATH,
    PMM_DL_SHIM_SWORD5_SHIELD3_SHEATH,

    // Sheath + Shield w/ Back Matrix + Hilt w/ Back Matrix
    PMM_DL_SHIM_SWORD1_SHIELD1_SHEATHED,
    PMM_DL_SHIM_SWORD1_SHIELD2_SHEATHED,
    PMM_DL_SHIM_SWORD1_SHIELD3_SHEATHED,
    PMM_DL_SHIM_SWORD2_SHIELD1_SHEATHED,
    PMM_DL_SHIM_SWORD2_SHIELD2_SHEATHED,
    PMM_DL_SHIM_SWORD2_SHIELD3_SHEATHED,
    PMM_DL_SHIM_SWORD3_SHIELD1_SHEATHED,
    PMM_DL_SHIM_SWORD3_SHIELD2_SHEATHED,
    PMM_DL_SHIM_SWORD3_SHIELD3_SHEATHED,
    PMM_DL_SHIM_SWORD4_SHIELD1_SHEATHED,
    PMM_DL_SHIM_SWORD4_SHIELD2_SHEATHED,
    PMM_DL_SHIM_SWORD4_SHIELD3_SHEATHED,
    PMM_DL_SHIM_SWORD5_SHIELD1_SHEATHED,
    PMM_DL_SHIM_SWORD5_SHIELD2_SHEATHED,
    PMM_DL_SHIM_SWORD5_SHIELD3_SHEATHED,

    // Left Fist + Sword w/ Blade
    PMM_DL_SHIM_LFIST_SWORD1,
    PMM_DL_SHIM_LFIST_SWORD2,
    PMM_DL_SHIM_LFIST_SWORD3,
    PMM_DL_SHIM_LFIST_SWORD3_PEDESTAL_GRABBED,
    PMM_DL_SHIM_LFIST_SWORD4,
    PMM_DL_SHIM_LFIST_SWORD4_BROKEN,
    PMM_DL_SHIM_LFIST_SWORD5,

    // Right Fist + Shield
    PMM_DL_SHIM_RFIST_SHIELD1,
    PMM_DL_SHIM_RFIST_SHIELD2,
    PMM_DL_SHIM_RFIST_SHIELD3,

    // Reserved (OoT)
    PMM_DL_SHIM_SHIELD1_ITEM,            // Burning Deku Shield
    PMM_DL_SHIM_SWORD3_PEDESTAL,         // Master Sword rotated to be inside pedestal
    PMM_DL_SHIM_SWORD3_PEDESTAL_GRABBED, // Master Sword rotated for grabbed from pedestal

    // Left Fist + Items
    PMM_DL_SHIM_LFIST_HAMMER,
    PMM_DL_SHIM_LFIST_BOOMERANG,
    PMM_DL_SHIM_RFIST_BOW,

    // Right Fist + Items
    PMM_DL_SHIM_RFIST_SLINGSHOT,
    PMM_DL_SHIM_RFIST_HOOKSHOT,
    PMM_DL_SHIM_RFIST_LONGSHOT = 289,
    PMM_DL_SHIM_RHAND_OCARINA_FAIRY = 219,
    PMM_DL_SHIM_RHAND_OCARINA_TIME,

    // First Person Right Hand + Items
    PMM_DL_SHIM_FPS_RHAND_BOW,
    PMM_DL_SHIM_FPS_RHAND_SLINGSHOT,
    PMM_DL_SHIM_FPS_RHAND_HOOKSHOT,
    PMM_DL_SHIM_FPS_RHAND_LONGSHOT = 290,

    // Deku Link flower propellers
    PMM_DL_SHIM_CENTER_FLOWER_PROPELLER_OPEN = 224,
    PMM_DL_SHIM_CENTER_FLOWER_PROPELLER_CLOSED,

    PMM_DL_MAX = 298
} PlayerModelManagerDisplayListId;

// Keep these defines for backwards compatibility
#define PMM_DL_RFIST PMM_DL_OPT_RFIST
#define PMM_DL_LFIST PMM_DL_OPT_LFIST
#define PMM_DL_LHAND_BOTTLE PMM_DL_OPT_LHAND_BOTTLE
#define PMM_DL_LHAND_GUITAR PMM_DL_OPT_LHAND_GUITAR
#define PMM_DL_FPS_HOOKSHOT PMM_DL_OPT_FPS_HOOKSHOT
#define PMM_DL_FPS_BOW PMM_DL_OPT_FPS_BOW
#define PMM_DL_FPS_SLINGSHOT PMM_DL_OPT_FPS_SLINGSHOT
#define PMM_DL_FPS_LFOREARM PMM_DL_OPT_FPS_LFOREARM
#define PMM_DL_FPS_LHAND PMM_DL_OPT_FPS_LHAND
#define PMM_DL_FPS_RFOREARM PMM_DL_OPT_FPS_RFOREARM
#define PMM_DL_FPS_RHAND PMM_DL_OPT_FPS_RHAND

// Other helper defines
#define PMM_DL_SWORD_KOKIRI_HILT PMM_DL_SWORD1_HILT
#define PMM_DL_SWORD_KOKIRI_BLADE PMM_DL_SWORD1_BLADE
#define PMM_DL_SWORD_KOKIRI_SHEATH PMM_DL_SWORD1_SHEATH

#define PMM_DL_SWORD_RAZOR_HILT PMM_DL_SWORD2_HILT
#define PMM_DL_SWORD_RAZOR_BLADE PMM_DL_SWORD2_BLADE
#define PMM_DL_SWORD_RAZOR_SHEATH PMM_DL_SWORD2_SHEATH

#define PMM_DL_SWORD_GILDED_HILT PMM_DL_SWORD3_HILT
#define PMM_DL_SWORD_GILDED_BLADE PMM_DL_SWORD3_BLADE
#define PMM_DL_SWORD_GILDED_SHEATH PMM_DL_SWORD3_SHEATH

#define PMM_DL_SWORD_GREAT_FAIRY_HILT PMM_DL_SWORD5_HILT
#define PMM_DL_SWORD_GREAT_FAIRY_BLADE PMM_DL_SWORD5_BLADE

#define PMM_DL_SWORD_FIERCE_DEITY_HILT PMM_DL_SWORD4_HILT
#define PMM_DL_SWORD_FIERCE_DEITY_BLADE PMM_DL_SWORD4_BLADE

#define PMM_DL_SHIELD_DEKU PMM_DL_SHIELD1

#define PMM_DL_SHIELD_HERO PMM_DL_SHIELD2
#define PMM_DL_SHIELD_HYLIAN PMM_DL_SHIELD2

#define PMM_DL_SHIELD_MIRROR PMM_DL_SHIELD3
#define PMM_DL_SHIELD_MIRROR_RAY PMM_DL_SHIELD3_RAY
#define PMM_DL_SHIELD_MIRROR_RAY_BEAM PMM_DL_SHIELD3_RAY_BEAM

typedef enum {
    PMM_MTX_SWORD1_BACK,
    PMM_MTX_SWORD2_BACK,
    PMM_MTX_SWORD3_BACK,
    PMM_MTX_SWORD4_BACK,
    PMM_MTX_SWORD5_BACK,
    PMM_MTX_SHIELD1_BACK,
    PMM_MTX_SHIELD2_BACK,
    PMM_MTX_SHIELD3_BACK,
    PMM_MTX_SHIELD1_ITEM,
    PMM_MTX_SWORD3_PEDESTAL,
    PMM_MTX_SWORD3_PEDESTAL_GRABBED,
    PMM_MTX_MASKS,
    PMM_MTX_ARROW_DRAWN,
    PMM_MTX_HOOKSHOT_CHAIN_AND_HOOK,
    PMM_MTX_MAX,
} PlayerModelManagerMatrixId;

#define PMM_MTX_SWORD_KOKIRI_BACK PMM_MTX_SWORD1_BACK
#define PMM_MTX_SWORD_RAZOR_BACK PMM_MTX_SWORD2_BACK
#define PMM_MTX_SWORD_GILDED_BACK PMM_MTX_SWORD3_BACK

#define PMM_MTX_SHIELD_DEKU_BACK PMM_MTX_SHIELD1_BACK
#define PMM_MTX_SHIELD_HERO_BACK PMM_MTX_SHIELD2_BACK
#define PMM_MTX_SHIELD_HYLIAN_BACK PMM_MTX_SHIELD2_BACK
#define PMM_MTX_SHIELD_MIRROR_BACK PMM_MTX_SHIELD3_BACK

typedef enum PlayerModelManagerModelType {
    PMM_MODEL_TYPE_NONE,
    PMM_MODEL_TYPE_CHILD,
    PMM_MODEL_TYPE_ADULT,
    PMM_MODEL_TYPE_DEKU,
    PMM_MODEL_TYPE_GORON,
    PMM_MODEL_TYPE_ZORA,
    PMM_MODEL_TYPE_FIERCE_DEITY,
    PMM_MODEL_TYPE_SWORD1,
    PMM_MODEL_TYPE_SWORD2,
    PMM_MODEL_TYPE_SWORD3,
    PMM_MODEL_TYPE_SWORD4,
    PMM_MODEL_TYPE_SWORD5,
    PMM_MODEL_TYPE_SHIELD1,
    PMM_MODEL_TYPE_SHIELD2,
    PMM_MODEL_TYPE_SHIELD3,
    PMM_MODEL_TYPE_HOOKSHOT,
    PMM_MODEL_TYPE_BOW,
    PMM_MODEL_TYPE_SLINGSHOT,
    PMM_MODEL_TYPE_BOTTLE,
    PMM_MODEL_TYPE_OCARINA_FAIRY,
    PMM_MODEL_TYPE_OCARINA_TIME,
    PMM_MODEL_TYPE_BOOMERANG,
    PMM_MODEL_TYPE_HAMMER,
    PMM_MODEL_TYPE_DEKU_STICK,
    PMM_MODEL_TYPE_PIPES,
    PMM_MODEL_TYPE_DRUMS,
    PMM_MODEL_TYPE_GUITAR,
    PMM_MODEL_TYPE_MASK_SKULL,
    PMM_MODEL_TYPE_MASK_SPOOKY,
    PMM_MODEL_TYPE_MASK_GERUDO,
    PMM_MODEL_TYPE_MASK_TRUTH,
    PMM_MODEL_TYPE_MASK_KAFEIS_MASK,
    PMM_MODEL_TYPE_MASK_ALL_NIGHT,
    PMM_MODEL_TYPE_MASK_BUNNY,
    PMM_MODEL_TYPE_MASK_KEATON,
    PMM_MODEL_TYPE_MASK_GARO,
    PMM_MODEL_TYPE_MASK_ROMANI,
    PMM_MODEL_TYPE_MASK_CIRCUS_LEADER,
    PMM_MODEL_TYPE_MASK_COUPLE,
    PMM_MODEL_TYPE_MASK_POSTMAN,
    PMM_MODEL_TYPE_MASK_GREAT_FAIRY,
    PMM_MODEL_TYPE_MASK_GIBDO,
    PMM_MODEL_TYPE_MASK_DON_GERO,
    PMM_MODEL_TYPE_MASK_KAMARO,
    PMM_MODEL_TYPE_MASK_CAPTAIN,
    PMM_MODEL_TYPE_MASK_STONE,
    PMM_MODEL_TYPE_MASK_BREMEN,
    PMM_MODEL_TYPE_MASK_BLAST,
    PMM_MODEL_TYPE_MASK_SCENTS,
    PMM_MODEL_TYPE_MASK_GIANT,
    PMM_MODEL_TYPE_MASK_DEKU,
    PMM_MODEL_TYPE_MASK_GORON,
    PMM_MODEL_TYPE_MASK_ZORA,
    PMM_MODEL_TYPE_MASK_FIERCE_DEITY,
    PMM_MODEL_TYPE_MODEL_PACK,
    PMM_MODEL_TYPE_BOMB,
    PMM_MODEL_TYPE_BOMBCHU,
    PMM_MODEL_TYPE_MAX
} PlayerModelManagerModelType;

#define PMM_MODEL_TYPE_SWORD_KOKIRI PMM_MODEL_TYPE_SWORD1
#define PMM_MODEL_TYPE_SWORD_RAZOR PMM_MODEL_TYPE_SWORD2
#define PMM_MODEL_TYPE_SWORD_GILDED PMM_MODEL_TYPE_SWORD3
#define PMM_MODEL_TYPE_SWORD_FIERCE_DIETY PMM_MODEL_TYPE_SWORD4
#define PMM_MODEL_TYPE_SWORD_GREAT_FAIRY PMM_MODEL_TYPE_SWORD5
#define PMM_MODEL_TYPE_SHIELD_DEKU PMM_MODEL_TYPE_SHIELD1
#define PMM_MODEL_TYPE_SHIELD_HYLIAN PMM_MODEL_TYPE_SHIELD2
#define PMM_MODEL_TYPE_SHIELD_HERO PMM_MODEL_TYPE_SHIELD2
#define PMM_MODEL_TYPE_SHIELD_MIRROR PMM_MODEL_TYPE_SHIELD3

typedef enum PlayerModelManagerModelEvent {
    PMM_EVENT_MODEL_APPLIED,
    PMM_EVENT_MODEL_REMOVED,
} PlayerModelManagerModelEvent;

typedef void PlayerModelManagerEventHandler(PlayerModelManagerHandle handle, PlayerModelManagerModelEvent event, void *userdata);

#ifndef YAZMT_PMM_NO_API_IMPORTS

#include "global.h"
#include "modding.h"

// Registers a new player model and returns a handle to it.
//
// A handle represents an entry in the in-game model manager menu, so hold onto it.
//
// Choose a unique string identifier. It will not show up in the menu if you set a display name, so it need not be human readable.
// There is a maximum length of 64 characters.
//
// This function can only be used during the PlayerModelManager_onRegisterModels event. Otherwise, an invalid handle will be returned.
RECOMP_IMPORT(YAZMT_PMM_MOD_NAME, PlayerModelManagerHandle PlayerModelManager_registerModel(unsigned long apiVersion, const char *internalName, PlayerModelManagerModelType modelType));
#define PLAYERMODELMANAGER_REGISTER_MODEL(internalName, modelType) PlayerModelManager_registerModel(PMM_API_VERSION, internalName, modelType)

// Sets the name that will appear in the menu for the passed in model handle.
//
// Limited to 32 characters. Longer names will be truncated.
//
// This function can only be used during the PlayerModelManager_onRegisterModels event.
//
// Returns true if successfully set, false otherwise.
RECOMP_IMPORT(YAZMT_PMM_MOD_NAME, bool PlayerModelManager_setDisplayName(PlayerModelManagerHandle h, const char *displayName));

// Sets the name that will appear in the author field of the menu.
//
// Limited to 64 characters. Longer names will be truncated.
//
// This function can only be used during the PlayerModelManager_onRegisterModels event.
//
// Returns true if successfully set, false otherwise.
RECOMP_IMPORT(YAZMT_PMM_MOD_NAME, bool PlayerModelManager_setAuthor(PlayerModelManagerHandle h, const char *author));

// Sets the skeleton and any display lists attached to it on a custom model.
//
// Returns true if successfully set, false otherwise.
RECOMP_IMPORT(YAZMT_PMM_MOD_NAME, bool PlayerModelManager_setSkeleton(PlayerModelManagerHandle h, FlexSkeletonHeader *skel));

// Sets the skeleton and any display lists attached to it on the shielding skeleton of a custom model.
//
// This is used for Goron Link's guard animation, so make sure to pass in a skeleton with the appropriate
// amount of limbs (4).
//
// Returns true if successfully set, false otherwise.
RECOMP_IMPORT(YAZMT_PMM_MOD_NAME, bool PlayerModelManager_setShieldingSkeleton(PlayerModelManagerHandle h, FlexSkeletonHeader *skel));

// Set a display list on the model. The ID can be obtained from PlayerModelManagerDisplayListId.
//
// Display lists prefixed with SHIM are automatically generated, and while you can replace them, you are encouraged not to
// to increase compatibility with equipment replacement mods.
//
// Returns true upon successful display list set, false otherwise.
RECOMP_IMPORT(YAZMT_PMM_MOD_NAME, bool PlayerModelManager_setDisplayList(PlayerModelManagerHandle h, PlayerModelManagerDisplayListId dlId, Gfx *dl));

// Targetted display list replacement for equipment model types.
//
// When an equipment model is used on a model with the passed in type, the display list specified here will be used instead.
//
// Any unspecified forms will use the display list passed into PlayerModelManager_setDisplayList if one is set.
//
// Returns true if display list was successfully set, false otherwise.
RECOMP_IMPORT(YAZMT_PMM_MOD_NAME, bool PlayerModelManager_setDisplayListForModelType(PlayerModelManagerHandle h, PlayerModelManagerModelType type, PlayerModelManagerDisplayListId dlId, Gfx *dl));

// Sets a matrix on the custom model.
//
// Returns true if matrix was successfully set, false otherwise.
RECOMP_IMPORT(YAZMT_PMM_MOD_NAME, bool PlayerModelManager_setMatrix(PlayerModelManagerHandle h, PlayerModelManagerMatrixId mtxId, Mtx *matrix));

// Targetted matrix setting for equipment model types.
//
// When an equipment model is used on a model with the passed in type, the matrix specified here will be used instead.
//
// Any unspecified forms will use the matrix passed into PlayerModelManager_setMatrix if one is set.
//
// Returns true if matrix was successfully set, false otherwise.
RECOMP_IMPORT(YAZMT_PMM_MOD_NAME, bool PlayerModelManager_setMatrixForModelType(PlayerModelManagerHandle h, PlayerModelManagerModelType type, PlayerModelManagerMatrixId mtxId, Mtx *matrix));

// Adds a handle to a model pack. Models will be applied in the order that their handles are added in.
//
// Returns true if handle was successfully added, false otherwise.
RECOMP_IMPORT(YAZMT_PMM_MOD_NAME, bool PlayerModelManager_addHandleToPack(PlayerModelManagerHandle h, PlayerModelManagerHandle toAdd));

// Set a callback function for this model. The types of events that can be responded to are in the PlayerModelManagerModelEvent enum.
//
// Returns true if callback successfully set, false otherwise.
RECOMP_IMPORT(YAZMT_PMM_MOD_NAME, bool PlayerModelManager_setCallback(PlayerModelManagerHandle h, PlayerModelManagerEventHandler *callback, void *userdata));

// Set eye flipbook textures for this model. Passed in array is expected to be at least 8 elements large.
//
// Returns true if successfully set, false otherwise.
RECOMP_IMPORT(YAZMT_PMM_MOD_NAME, bool PlayerModelManager_setEyesTextures(PlayerModelManagerHandle h, TexturePtr eyesTextures[]));

// Set mouth flipbook textures for this model. Passed in array is expected to be at least 4 elements large.
//
// Returns true if successfully set, false otherwise.
RECOMP_IMPORT(YAZMT_PMM_MOD_NAME, bool PlayerModelManager_setMouthTextures(PlayerModelManagerHandle h, TexturePtr mouthTextures[]));

// Get a static pointer to a DL in a particular form. The visual will be automatically updated if the model for that form changes.
//
// If an invalid display list ID is passed in, NULL will be returned.
RECOMP_IMPORT(YAZMT_PMM_MOD_NAME, Gfx *PlayerModelManager_getFormDisplayList(unsigned long apiVersion, PlayerTransformation form, PlayerModelManagerDisplayListId dlId));

// Helper define for PlayerModelManager_getFormDisplayList. See PlayerModelManager_getFormDisplayList description for functionality.
#define PLAYERMODELMANAGER_GET_FORM_DISPLAY_LIST(form, displayListId) PlayerModelManager_getFormDisplayList(PMM_API_VERSION, form, displayListId)

// Returns true if the model attached to the passed in handle is currently equipped, false otherwise.
RECOMP_IMPORT(YAZMT_PMM_MOD_NAME, bool PlayerModelManager_isApplied(PlayerModelManagerHandle h));

// Set tunic color of a specific form.
RECOMP_IMPORT(YAZMT_PMM_MOD_NAME, void PlayerModelManager_requestOverrideFormTunicColor(PlayerTransformation form, u8 r, u8 g, u8 b, u8 a));

// Set tunic color for all forms. Replaces any form-specific tunic colors.
RECOMP_IMPORT(YAZMT_PMM_MOD_NAME, void PlayerModelManager_requestOverrideTunicColor(u8 r, u8 g, u8 b, u8 a));

// Allows mods to override display lists used by the models used when no custom model is found.
// This should be used sparingly, but there are some use cases, like if you are exchanging
// the first person models for non-human forms.
//
// Attempts to override shim DLs will be ignored.
//
// This function can only be used during the PlayerModelManager_onRegisterModels event.
//
// This returns true on successful override, false otherwise.
RECOMP_IMPORT(YAZMT_PMM_MOD_NAME, bool PlayerModelManager_overrideVanillaDisplayList(unsigned long apiVersion, PlayerTransformation form, PlayerModelManagerDisplayListId displayListId, Gfx *displayList));

// Helper define for PlayerModelManager_overrideVanillaDisplayList. See PlayerModelManager_overrideVanillaDisplayList description for functionality.
#define PLAYERMODELMANAGER_OVERRIDE_VANILLA_DISPLAY_LIST(form, displayListId, displayList) PlayerModelManager_overrideVanillaDisplayList(PMM_API_VERSION, form, displayListId, displayList)

// Allows mods to override matrixes used by the models used when no custom matrix is found.
// This should be used sparingly, but there are some use cases, like if you are exchanging
// equipment models with PlayerModelManager_overrideVanillaDisplayList.
//
// This function can only be used during the PlayerModelManager_onRegisterModels event.
//
// This returns true on successful override, false otherwise.
RECOMP_IMPORT(YAZMT_PMM_MOD_NAME, bool PlayerModelManager_overrideVanillaMatrix(unsigned long apiVersion, PlayerTransformation form, PlayerModelManagerMatrixId displayListId, Mtx *mtx));

// Helper define for PlayerModelManager_overrideVanillaDisplayList. See PlayerModelManager_overrideVanillaDisplayList description for functionality.
#define PLAYERMODELMANAGER_OVERRIDE_VANILLA_MATRIX(form, matrixId, matrix) PlayerModelManager_overrideVanillaMatrix(PMM_API_VERSION, form, matrixId, matrix)

// Returns true if there is a custom player model applied to the passed in form, false otherwise.
RECOMP_IMPORT(YAZMT_PMM_MOD_NAME, bool PlayerModelManager_isCustomModelApplied(PlayerTransformation form));

// Helper define for register models event.
#define PLAYERMODELMANAGER_CALLBACK_REGISTER_MODELS RECOMP_CALLBACK(YAZMT_PMM_MOD_NAME, onRegisterModels)

#endif

#endif
