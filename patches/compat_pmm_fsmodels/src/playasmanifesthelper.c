#include "global.h"
#include "yazmtcorelib_api.h"
#include "playermodelmanager_api.h"

typedef struct {
    char *key;
    PlayerModelManagerDisplayListId dlId;
} ManifestDictEntry;

#define DECLARE_MANIFEST_ENTRY(k, dl) {.key = k, .dlId = PMM_DL_##dl}

// riggedmesh.(limb name)               = z64playas manifest
// Limb X / (limb name w/ no prefix)    = zzplayas manifest
#define DECLARE_COMMON_MANIFEST_ENTRIES DECLARE_MANIFEST_ENTRY("riggedmesh.Waist", WAIST),          \
                                        DECLARE_MANIFEST_ENTRY("riggedmesh.Thigh.R", RTHIGH),       \
                                        DECLARE_MANIFEST_ENTRY("riggedmesh.Shin.R", RSHIN),         \
                                        DECLARE_MANIFEST_ENTRY("riggedmesh.Foot.R", RFOOT),         \
                                        DECLARE_MANIFEST_ENTRY("riggedmesh.Thigh.L", LTHIGH),       \
                                        DECLARE_MANIFEST_ENTRY("riggedmesh.Shin.L", LSHIN),         \
                                        DECLARE_MANIFEST_ENTRY("riggedmesh.Foot.L", LFOOT),         \
                                        DECLARE_MANIFEST_ENTRY("riggedmesh.Head", HEAD),            \
                                        DECLARE_MANIFEST_ENTRY("riggedmesh.Hat", HAT),              \
                                        DECLARE_MANIFEST_ENTRY("riggedmesh.Collar", COLLAR),        \
                                        DECLARE_MANIFEST_ENTRY("riggedmesh.Shoulder.L", LSHOULDER), \
                                        DECLARE_MANIFEST_ENTRY("riggedmesh.Forearm.L", LFOREARM),   \
                                        DECLARE_MANIFEST_ENTRY("riggedmesh.Shoulder.R", RSHOULDER), \
                                        DECLARE_MANIFEST_ENTRY("riggedmesh.Forearm.R", RFOREARM),   \
                                        DECLARE_MANIFEST_ENTRY("riggedmesh.Torso", TORSO),          \
                                        DECLARE_MANIFEST_ENTRY("riggedmesh.Hand.L", LHAND),         \
                                        DECLARE_MANIFEST_ENTRY("riggedmesh.Hand.R", RHAND),         \
                                        DECLARE_MANIFEST_ENTRY("Waist", WAIST),                     \
                                        DECLARE_MANIFEST_ENTRY("Thigh.R", RTHIGH),                  \
                                        DECLARE_MANIFEST_ENTRY("Shin.R", RSHIN),                    \
                                        DECLARE_MANIFEST_ENTRY("Foot.R", RFOOT),                    \
                                        DECLARE_MANIFEST_ENTRY("Thigh.L", LTHIGH),                  \
                                        DECLARE_MANIFEST_ENTRY("Shin.L", LSHIN),                    \
                                        DECLARE_MANIFEST_ENTRY("Foot.L", LFOOT),                    \
                                        DECLARE_MANIFEST_ENTRY("Head", HEAD),                       \
                                        DECLARE_MANIFEST_ENTRY("Hat", HAT),                         \
                                        DECLARE_MANIFEST_ENTRY("Collar", COLLAR),                   \
                                        DECLARE_MANIFEST_ENTRY("Shoulder.L", LSHOULDER),            \
                                        DECLARE_MANIFEST_ENTRY("Forearm.L", LFOREARM),              \
                                        DECLARE_MANIFEST_ENTRY("Shoulder.R", RSHOULDER),            \
                                        DECLARE_MANIFEST_ENTRY("Forearm.R", RFOREARM),              \
                                        DECLARE_MANIFEST_ENTRY("Torso", TORSO),                     \
                                        DECLARE_MANIFEST_ENTRY("Hand.L", LHAND),                    \
                                        DECLARE_MANIFEST_ENTRY("Hand.R", RHAND),                    \
                                        DECLARE_MANIFEST_ENTRY("Limb 1", WAIST),                    \
                                        DECLARE_MANIFEST_ENTRY("Limb 3", RTHIGH),                   \
                                        DECLARE_MANIFEST_ENTRY("Limb 4", RSHIN),                    \
                                        DECLARE_MANIFEST_ENTRY("Limb 5", RFOOT),                    \
                                        DECLARE_MANIFEST_ENTRY("Limb 6", LTHIGH),                   \
                                        DECLARE_MANIFEST_ENTRY("Limb 7", LSHIN),                    \
                                        DECLARE_MANIFEST_ENTRY("Limb 8", LFOOT),                    \
                                        DECLARE_MANIFEST_ENTRY("Limb 10", HEAD),                    \
                                        DECLARE_MANIFEST_ENTRY("Limb 11", HAT),                     \
                                        DECLARE_MANIFEST_ENTRY("Limb 12", COLLAR),                  \
                                        DECLARE_MANIFEST_ENTRY("Limb 13", LSHOULDER),               \
                                        DECLARE_MANIFEST_ENTRY("Limb 14", LFOREARM),                \
                                        DECLARE_MANIFEST_ENTRY("Limb 15", LHAND),                   \
                                        DECLARE_MANIFEST_ENTRY("Limb 16", RSHOULDER),               \
                                        DECLARE_MANIFEST_ENTRY("Limb 17", RFOREARM),                \
                                        DECLARE_MANIFEST_ENTRY("Limb 18", RHAND),                   \
                                        DECLARE_MANIFEST_ENTRY("Limb 20", TORSO),                   \
                                        DECLARE_MANIFEST_ENTRY("Fist.L", LFIST),                    \
                                        DECLARE_MANIFEST_ENTRY("Bottle.Hand.L", LHAND_BOTTLE),      \
                                        DECLARE_MANIFEST_ENTRY("Fist.R", RFIST)

const static ManifestDictEntry sOoTChildManifest[] = {
    DECLARE_COMMON_MANIFEST_ENTRIES,
    DECLARE_MANIFEST_ENTRY("Sheath", SWORD1_SHEATH),
    DECLARE_MANIFEST_ENTRY("Hilt.1", SWORD1_HILT),
    DECLARE_MANIFEST_ENTRY("Blade.1", SWORD1_BLADE),
    // DECLARE_MANIFEST_ENTRY("Blade.2", SWORD3_BLADE),
    DECLARE_MANIFEST_ENTRY("Bottle", BOTTLE_GLASS),
    DECLARE_MANIFEST_ENTRY("Boomerang", BOOMERANG),
    DECLARE_MANIFEST_ENTRY("Shield.1", SHIELD1),
    DECLARE_MANIFEST_ENTRY("Shield.2", SHIELD2),
    DECLARE_MANIFEST_ENTRY("Slingshot", SLINGSHOT),
    DECLARE_MANIFEST_ENTRY("Ocarina.1", OCARINA_FAIRY),
    DECLARE_MANIFEST_ENTRY("Ocarina.2", OCARINA_TIME),
    DECLARE_MANIFEST_ENTRY("DekuStick", DEKU_STICK),
    DECLARE_MANIFEST_ENTRY("GoronBracelet", BRACELET_LFOREARM),
    DECLARE_MANIFEST_ENTRY("Mask.Skull", MASK_SKULL),
    DECLARE_MANIFEST_ENTRY("Mask.Spooky", MASK_SPOOKY),
    DECLARE_MANIFEST_ENTRY("Mask.Keaton", MASK_KEATON),
    DECLARE_MANIFEST_ENTRY("Mask.Truth", MASK_TRUTH),
    DECLARE_MANIFEST_ENTRY("Mask.Goron", MASK_GORON),
    DECLARE_MANIFEST_ENTRY("Mask.Zora", MASK_ZORA),
    DECLARE_MANIFEST_ENTRY("Mask.Gerudo", MASK_GERUDO),
    DECLARE_MANIFEST_ENTRY("Mask.Bunny", MASK_BUNNY),
    DECLARE_MANIFEST_ENTRY("FPS.Forearm.R", FPS_RHAND),
    DECLARE_MANIFEST_ENTRY("Slingshot.String", SLINGSHOT_STRING),
};

const static ManifestDictEntry sOoTAdultManifest[] = {
    DECLARE_COMMON_MANIFEST_ENTRIES,
    DECLARE_MANIFEST_ENTRY("Sheath", SWORD3_SHEATH),
    DECLARE_MANIFEST_ENTRY("Hilt.2", SWORD3_HILT),
    DECLARE_MANIFEST_ENTRY("Blade.2", SWORD3_BLADE),
    DECLARE_MANIFEST_ENTRY("Hilt.3", SWORD4_HILT),
    DECLARE_MANIFEST_ENTRY("Blade.3", SWORD4_BLADE),
    DECLARE_MANIFEST_ENTRY("Broken.Blade.3", SWORD4_BLADE_BROKEN),
    DECLARE_MANIFEST_ENTRY("Shield.2", SHIELD2),
    DECLARE_MANIFEST_ENTRY("Shield.3", SHIELD3),
    DECLARE_MANIFEST_ENTRY("Hammer", HAMMER),
    DECLARE_MANIFEST_ENTRY("Bottle", BOTTLE_GLASS),
    DECLARE_MANIFEST_ENTRY("Bow", BOW),
    DECLARE_MANIFEST_ENTRY("Ocarina.2", OCARINA_TIME),
    DECLARE_MANIFEST_ENTRY("Hookshot", HOOKSHOT),
    DECLARE_MANIFEST_ENTRY("Gauntlet.Forearm.L", GAUNTLET_LFOREARM),
    DECLARE_MANIFEST_ENTRY("Gauntlet.Hand.L", GAUNTLET_LHAND),
    DECLARE_MANIFEST_ENTRY("Gauntlet.Fist.L", GAUNTLET_LFIST),
    DECLARE_MANIFEST_ENTRY("Gauntlet.Forearm.R", GAUNTLET_RFOREARM),
    DECLARE_MANIFEST_ENTRY("Gauntlet.Hand.R", GAUNTLET_RHAND),
    DECLARE_MANIFEST_ENTRY("Gauntlet.Fist.R", GAUNTLET_RFIST),
    DECLARE_MANIFEST_ENTRY("Foot.2.L", BOOT_LIRON),
    DECLARE_MANIFEST_ENTRY("Foot.2.R", BOOT_RIRON),
    DECLARE_MANIFEST_ENTRY("Foot.3.L", BOOT_LHOVER),
    DECLARE_MANIFEST_ENTRY("Foot.3.R", BOOT_RHOVER),
    DECLARE_MANIFEST_ENTRY("FPS.Forearm.L", FPS_LFOREARM),
    DECLARE_MANIFEST_ENTRY("FPS.Hand.L", FPS_LHAND),
    DECLARE_MANIFEST_ENTRY("FPS.Forearm.R", FPS_RFOREARM),
    DECLARE_MANIFEST_ENTRY("FPS.Hand.R", FPS_RHAND),
    DECLARE_MANIFEST_ENTRY("FPS.Hookshot", FPS_HOOKSHOT),
    DECLARE_MANIFEST_ENTRY("Hookshot.Chain", HOOKSHOT_CHAIN),
    DECLARE_MANIFEST_ENTRY("Hookshot.Spike", HOOKSHOT_HOOK),
    DECLARE_MANIFEST_ENTRY("Hookshot.Aiming.Reticule", HOOKSHOT_RETICLE),
    DECLARE_MANIFEST_ENTRY("Bow.String", BOW_STRING),
    DECLARE_MANIFEST_ENTRY("Blade.3.Break", SWORD4_BLADE_FRAGMENT),
};

const static ManifestDictEntry sMMHumanManifest[] = {
    DECLARE_COMMON_MANIFEST_ENTRIES,
    // DECLARE_MANIFEST_ENTRY("Shield.2.Face", SHIELD_MIRROR_FACE),
    DECLARE_MANIFEST_ENTRY("Shield.2", SHIELD3),
    DECLARE_MANIFEST_ENTRY("Sword.4", SWORD5_BLADE),
    DECLARE_MANIFEST_ENTRY("Sheath.3", SWORD3_SHEATH),
    DECLARE_MANIFEST_ENTRY("Hilt.3", SWORD3_HILT),
    DECLARE_MANIFEST_ENTRY("Blade.3", SWORD3_BLADE),
    DECLARE_MANIFEST_ENTRY("Sheath.2", SWORD2_SHEATH),
    DECLARE_MANIFEST_ENTRY("Shield.1", SHIELD2),
    DECLARE_MANIFEST_ENTRY("Sheath.1", SWORD1_SHEATH),
    DECLARE_MANIFEST_ENTRY("Hookshot", HOOKSHOT),
    DECLARE_MANIFEST_ENTRY("Bow", BOW),
    DECLARE_MANIFEST_ENTRY("Hookshot.Spike", HOOKSHOT_HOOK),
    DECLARE_MANIFEST_ENTRY("Ocarina.2", OCARINA_TIME),
    DECLARE_MANIFEST_ENTRY("FPS.Forearm.R", FPS_RHAND),
    DECLARE_MANIFEST_ENTRY("Bow.String", BOW_STRING),
};

const static ManifestDictEntry sMMDekuManifest[] = {
    DECLARE_COMMON_MANIFEST_ENTRIES,
    DECLARE_MANIFEST_ENTRY("Shield", DEKU_GUARD),
    DECLARE_MANIFEST_ENTRY("Sheath", SWORD1_SHEATH),
    DECLARE_MANIFEST_ENTRY("Pipe.Mouth", PIPE_MOUTH),
    DECLARE_MANIFEST_ENTRY("Pipe.Right", PIPE_RIGHT),
    DECLARE_MANIFEST_ENTRY("Pipe.Up", PIPE_UP),
    DECLARE_MANIFEST_ENTRY("Pipe.Down", PIPE_DOWN),
    DECLARE_MANIFEST_ENTRY("Pipe.Left", PIPE_LEFT),
    DECLARE_MANIFEST_ENTRY("Pipe.A", PIPE_A),
    DECLARE_MANIFEST_ENTRY("Flower.Stem.R", STEM_RIGHT),
    DECLARE_MANIFEST_ENTRY("Flower.Stem.L", STEM_LEFT),
    DECLARE_MANIFEST_ENTRY("Flower.Petal", PETAL_PARTICLE),
    DECLARE_MANIFEST_ENTRY("Flower.Propeller.Closed", FLOWER_PROPELLER_CLOSED),
    DECLARE_MANIFEST_ENTRY("Flower.Center.Closed", FLOWER_CENTER_CLOSED),
    DECLARE_MANIFEST_ENTRY("Flower.Propeller.Open", FLOWER_PROPELLER_OPEN),
    DECLARE_MANIFEST_ENTRY("Flower.Center.Open", FLOWER_CENTER_OPEN),
    DECLARE_MANIFEST_ENTRY("Pad.Wood", PAD_WOOD),
    DECLARE_MANIFEST_ENTRY("Pad.Grass", PAD_GRASS),
    DECLARE_MANIFEST_ENTRY("Pad.Open", PAD_OPENING),
};

const static ManifestDictEntry sMMZoraManifest[] = {
    DECLARE_COMMON_MANIFEST_ENTRIES,
    DECLARE_MANIFEST_ENTRY("Fin.L", LFIN),
    DECLARE_MANIFEST_ENTRY("Fin.Swim.L", LFIN_SWIM),
    DECLARE_MANIFEST_ENTRY("Fin.R", RFIN),
    DECLARE_MANIFEST_ENTRY("Fin.Swim.R", RFIN_SWIM),
    DECLARE_MANIFEST_ENTRY("Hand.Guitar.L", LHAND_GUITAR),
    DECLARE_MANIFEST_ENTRY("Guitar", GUITAR),
    DECLARE_MANIFEST_ENTRY("Shield", FIN_SHIELD),
    DECLARE_MANIFEST_ENTRY("Electricity", MAGIC_BARRIER),
};

const static ManifestDictEntry sMMFierceDeityManifest[] = {
    DECLARE_COMMON_MANIFEST_ENTRIES,
    DECLARE_MANIFEST_ENTRY("Sword", SWORD4_BLADE),
    DECLARE_MANIFEST_ENTRY("Bottle.Empty", BOTTLE_GLASS),
};

const static ManifestDictEntry sZ64AllManifest[] = {
    DECLARE_COMMON_MANIFEST_ENTRIES,
    DECLARE_MANIFEST_ENTRY("FPS.Forearm.L", FPS_LFOREARM),
    DECLARE_MANIFEST_ENTRY("FPS.Hand.L", FPS_LHAND),
    DECLARE_MANIFEST_ENTRY("FPS.Forearm.R", FPS_RFOREARM),
    DECLARE_MANIFEST_ENTRY("FPS.Hand.R", FPS_RHAND),
    DECLARE_MANIFEST_ENTRY("Hilt.1", SWORD1_HILT),
    DECLARE_MANIFEST_ENTRY("Hilt.2", SWORD2_HILT),
    DECLARE_MANIFEST_ENTRY("Hilt.3", SWORD3_HILT),
    DECLARE_MANIFEST_ENTRY("Hilt.4", SWORD4_HILT),
    DECLARE_MANIFEST_ENTRY("Hilt.5", SWORD5_HILT),
    DECLARE_MANIFEST_ENTRY("Blade.1", SWORD1_BLADE),
    DECLARE_MANIFEST_ENTRY("Blade.2", SWORD2_BLADE),
    DECLARE_MANIFEST_ENTRY("Blade.3", SWORD3_BLADE),
    DECLARE_MANIFEST_ENTRY("Blade.4", SWORD4_BLADE),
    DECLARE_MANIFEST_ENTRY("Blade.4.Broken", SWORD4_BLADE_BROKEN),
    DECLARE_MANIFEST_ENTRY("Blade.4.Fragment", SWORD4_BLADE_FRAGMENT),
    DECLARE_MANIFEST_ENTRY("Blade.5", SWORD5_BLADE),
    DECLARE_MANIFEST_ENTRY("Sheath.1", SWORD1_SHEATH),
    DECLARE_MANIFEST_ENTRY("Sheath.2", SWORD2_SHEATH),
    DECLARE_MANIFEST_ENTRY("Sheath.3", SWORD3_SHEATH),
    DECLARE_MANIFEST_ENTRY("Sheath.4", SWORD4_SHEATH),
    DECLARE_MANIFEST_ENTRY("Sheath.5", SWORD5_SHEATH),
    DECLARE_MANIFEST_ENTRY("Shield.1", SHIELD1),
    DECLARE_MANIFEST_ENTRY("Shield.2", SHIELD2),
    DECLARE_MANIFEST_ENTRY("Shield.3", SHIELD3),
    DECLARE_MANIFEST_ENTRY("Shield.3.Ray", SHIELD3_RAY),
    DECLARE_MANIFEST_ENTRY("Bracelet.Forearm.L", BRACELET_LFOREARM),
    DECLARE_MANIFEST_ENTRY("Gauntlet.Forearm.L", GAUNTLET_LFOREARM),
    DECLARE_MANIFEST_ENTRY("Gauntlet.Hand.L", GAUNTLET_LHAND),
    DECLARE_MANIFEST_ENTRY("Gauntlet.Fist.L", GAUNTLET_LFIST),
    DECLARE_MANIFEST_ENTRY("Gauntlet.Forearm.R", GAUNTLET_RFOREARM),
    DECLARE_MANIFEST_ENTRY("Gauntlet.Hand.R", GAUNTLET_RHAND),
    DECLARE_MANIFEST_ENTRY("Gauntlet.Fist.R", GAUNTLET_RFIST),
    DECLARE_MANIFEST_ENTRY("Boot.Iron.L", BOOT_LIRON),
    DECLARE_MANIFEST_ENTRY("Boot.Iron.R", BOOT_RIRON),
    DECLARE_MANIFEST_ENTRY("Boot.Hover.L", BOOT_LHOVER),
    DECLARE_MANIFEST_ENTRY("Boot.Hover.R", BOOT_RHOVER),
    DECLARE_MANIFEST_ENTRY("Foot.Hover.R", BOOT_RHOVER),
    DECLARE_MANIFEST_ENTRY("Bottle.Glass", BOTTLE_GLASS),
    DECLARE_MANIFEST_ENTRY("Bottle.Contents", BOTTLE_CONTENTS),
    DECLARE_MANIFEST_ENTRY("Boomerang", BOOMERANG),
    DECLARE_MANIFEST_ENTRY("Boomerang.Flying", BOOMERANG_FLYING),
    DECLARE_MANIFEST_ENTRY("Hammer", HAMMER),
    DECLARE_MANIFEST_ENTRY("Deku.Stick", DEKU_STICK),
    DECLARE_MANIFEST_ENTRY("Slingshot", SLINGSHOT),
    DECLARE_MANIFEST_ENTRY("Ocarina.Fairy", OCARINA_FAIRY),
    DECLARE_MANIFEST_ENTRY("Ocarina.Time", OCARINA_TIME),
    DECLARE_MANIFEST_ENTRY("Slingshot", SLINGSHOT),
    DECLARE_MANIFEST_ENTRY("Slingshot.String", SLINGSHOT_STRING),
    DECLARE_MANIFEST_ENTRY("Bow", BOW),
    DECLARE_MANIFEST_ENTRY("Bow.String", BOW_STRING),
    DECLARE_MANIFEST_ENTRY("Bow.Arrow", BOW_ARROW),
    DECLARE_MANIFEST_ENTRY("Hookshot", HOOKSHOT),
    DECLARE_MANIFEST_ENTRY("Hookshot.Chain", HOOKSHOT_CHAIN),
    DECLARE_MANIFEST_ENTRY("Hookshot.Hook", HOOKSHOT_HOOK),
    DECLARE_MANIFEST_ENTRY("Hookshot.Reticle", HOOKSHOT_RETICLE),
    DECLARE_MANIFEST_ENTRY("FPS.Hookshot", FPS_HOOKSHOT),
    DECLARE_MANIFEST_ENTRY("Mask.Skull", MASK_SKULL),
    DECLARE_MANIFEST_ENTRY("Mask.Spooky", MASK_SPOOKY),
    DECLARE_MANIFEST_ENTRY("Mask.Gerudo", MASK_GERUDO),
    DECLARE_MANIFEST_ENTRY("Mask.Truth", MASK_TRUTH),
    DECLARE_MANIFEST_ENTRY("Mask.Kafei", MASK_KAFEIS_MASK),
    DECLARE_MANIFEST_ENTRY("Mask.All.Night", MASK_ALL_NIGHT),
    DECLARE_MANIFEST_ENTRY("Mask.Bunny", MASK_BUNNY),
    DECLARE_MANIFEST_ENTRY("Mask.Keaton", MASK_KEATON),
    DECLARE_MANIFEST_ENTRY("Mask.Garo", MASK_GARO),
    DECLARE_MANIFEST_ENTRY("Mask.Romani", MASK_ROMANI),
    DECLARE_MANIFEST_ENTRY("Mask.Circus.Leader", MASK_CIRCUS_LEADER),
    DECLARE_MANIFEST_ENTRY("Mask.Couple", MASK_COUPLE),
    DECLARE_MANIFEST_ENTRY("Mask.Postman", MASK_POSTMAN),
    DECLARE_MANIFEST_ENTRY("Mask.Great.Fairy", MASK_GREAT_FAIRY),
    DECLARE_MANIFEST_ENTRY("Mask.Gibdo", MASK_GIBDO),
    DECLARE_MANIFEST_ENTRY("Mask.Don.Gero", MASK_DON_GERO),
    DECLARE_MANIFEST_ENTRY("Mask.Kamaro", MASK_KAMARO),
    DECLARE_MANIFEST_ENTRY("Mask.Captain", MASK_CAPTAIN),
    DECLARE_MANIFEST_ENTRY("Mask.Stone", MASK_STONE),
    DECLARE_MANIFEST_ENTRY("Mask.Bremen", MASK_BREMEN),
    DECLARE_MANIFEST_ENTRY("Mask.Blast", MASK_BLAST),
    DECLARE_MANIFEST_ENTRY("Mask.Blast.Cooldown", MASK_BLAST_COOLING_DOWN),
    DECLARE_MANIFEST_ENTRY("Mask.Scents", MASK_SCENTS),
    DECLARE_MANIFEST_ENTRY("Mask.Giant", MASK_GIANT),
    DECLARE_MANIFEST_ENTRY("Mask.Deku", MASK_DEKU),
    DECLARE_MANIFEST_ENTRY("Mask.Goron", MASK_GORON),
    DECLARE_MANIFEST_ENTRY("Mask.Zora", MASK_ZORA),
    DECLARE_MANIFEST_ENTRY("Mask.Fierce.Deity", MASK_FIERCE_DEITY),
    DECLARE_MANIFEST_ENTRY("Mask.Deku.Scream", MASK_DEKU_SCREAM),
    DECLARE_MANIFEST_ENTRY("Mask.Goron.Scream", MASK_GORON_SCREAM),
    DECLARE_MANIFEST_ENTRY("Mask.Zora.Scream", MASK_ZORA_SCREAM),
    DECLARE_MANIFEST_ENTRY("Mask.Fierce.Deity.Scream", MASK_FIERCE_DEITY_SCREAM),
    DECLARE_MANIFEST_ENTRY("Elegy.Shell.Human", ELEGY_OF_EMPTINESS_SHELL_HUMAN),
    DECLARE_MANIFEST_ENTRY("Elegy.Shell.Deku", ELEGY_OF_EMPTINESS_SHELL_DEKU),
    DECLARE_MANIFEST_ENTRY("Elegy.Shell.Goron", ELEGY_OF_EMPTINESS_SHELL_GORON),
    DECLARE_MANIFEST_ENTRY("Elegy.Shell.Zora", ELEGY_OF_EMPTINESS_SHELL_ZORA),
    DECLARE_MANIFEST_ENTRY("Deku.Guard", DEKU_GUARD),
    DECLARE_MANIFEST_ENTRY("Pipe.Mouth", PIPE_MOUTH),
    DECLARE_MANIFEST_ENTRY("Pipe.Right", PIPE_RIGHT),
    DECLARE_MANIFEST_ENTRY("Pipe.Up", PIPE_UP),
    DECLARE_MANIFEST_ENTRY("Pipe.Down", PIPE_DOWN),
    DECLARE_MANIFEST_ENTRY("Pipe.Left", PIPE_UP),
    DECLARE_MANIFEST_ENTRY("Pipe.A", PIPE_A),
    DECLARE_MANIFEST_ENTRY("Stem.R", STEM_RIGHT),
    DECLARE_MANIFEST_ENTRY("Stem.L", STEM_LEFT),
    DECLARE_MANIFEST_ENTRY("Petal.Particle", PETAL_PARTICLE),
    DECLARE_MANIFEST_ENTRY("Flower.Propeller.Closed", FLOWER_PROPELLER_CLOSED),
    DECLARE_MANIFEST_ENTRY("Flower.Center.Closed", FLOWER_CENTER_CLOSED),
    DECLARE_MANIFEST_ENTRY("Flower.Propeller.Open", FLOWER_PROPELLER_OPEN),
    DECLARE_MANIFEST_ENTRY("Flower.Center.Open", FLOWER_CENTER_OPEN),
    DECLARE_MANIFEST_ENTRY("Pad.Wood", PAD_WOOD),
    DECLARE_MANIFEST_ENTRY("Pad.Grass", PAD_GRASS),
    DECLARE_MANIFEST_ENTRY("Pad.Opening", PAD_OPENING),
    DECLARE_MANIFEST_ENTRY("Drum.Strap", DRUM_STRAP),
    DECLARE_MANIFEST_ENTRY("Drum.Up", DRUM_UP),
    DECLARE_MANIFEST_ENTRY("Drum.Left", DRUM_LEFT),
    DECLARE_MANIFEST_ENTRY("Drum.Right", DRUM_RIGHT),
    DECLARE_MANIFEST_ENTRY("Drum.Down", DRUM_DOWN),
    DECLARE_MANIFEST_ENTRY("Drum.A", DRUM_A),
    // TODO: HANDLE GORON SHIELDING
    DECLARE_MANIFEST_ENTRY("Curled", CURLED),
    DECLARE_MANIFEST_ENTRY("Spikes", SPIKES),
    DECLARE_MANIFEST_ENTRY("Fire.Init", FIRE_INIT),
    DECLARE_MANIFEST_ENTRY("Fire.Roll", FIRE_ROLL),
    DECLARE_MANIFEST_ENTRY("Fire.Punch", FIRE_PUNCH),
    DECLARE_MANIFEST_ENTRY("Fin.L", LFIN),
    DECLARE_MANIFEST_ENTRY("Fin.Swim.L", LFIN_SWIM),
    DECLARE_MANIFEST_ENTRY("Fin.R", RFIN),
    DECLARE_MANIFEST_ENTRY("Fin.Swim.R", RFIN_SWIM),
    DECLARE_MANIFEST_ENTRY("Guitar.Hand.L", LHAND_GUITAR),
    DECLARE_MANIFEST_ENTRY("Guitar", GUITAR),
    DECLARE_MANIFEST_ENTRY("Shield", FIN_SHIELD),
    DECLARE_MANIFEST_ENTRY("Electricity", MAGIC_BARRIER),
};

typedef enum {
    MDID_OOT_CHILD,
    MDID_OOT_ADULT,
    MDID_MM_HUMAN,
    MDID_MM_DEKU,
    MDID_MM_ZORA,
    MDID_MM_FIERCE_DEITY,
    MDID_Z64_ALL,
    MDID_MAX
} ManifestDictionaryId;

static YAZMTCore_StringU32Dictionary* sDictChild;
static YAZMTCore_StringU32Dictionary* sDictAdult;
static YAZMTCore_StringU32Dictionary* sDictHuman;
static YAZMTCore_StringU32Dictionary* sDictDeku;
static YAZMTCore_StringU32Dictionary* sDictZora;
static YAZMTCore_StringU32Dictionary* sDictFierceDeity;
static YAZMTCore_StringU32Dictionary* sDictGeneric;

YAZMTCore_StringU32Dictionary *newDict(const ManifestDictEntry m[], size_t n) {
    YAZMTCore_StringU32Dictionary *h = YAZMTCore_StringU32Dictionary_new();

    for (size_t i = 0; i < n; ++i) {
        YAZMTCore_StringU32Dictionary_set(h, m[i].key, m[i].dlId);
    }

    return h;
}

void initManifestHelperDict() {
    sDictChild = newDict(sOoTChildManifest, ARRAY_COUNT(sOoTChildManifest));
    sDictAdult = newDict(sOoTAdultManifest, ARRAY_COUNT(sOoTAdultManifest));
    sDictHuman = newDict(sMMHumanManifest, ARRAY_COUNT(sMMHumanManifest));
    sDictDeku = newDict(sMMDekuManifest, ARRAY_COUNT(sMMDekuManifest));
    sDictZora = newDict(sMMZoraManifest, ARRAY_COUNT(sMMZoraManifest));
    sDictFierceDeity = newDict(sMMFierceDeityManifest, ARRAY_COUNT(sMMFierceDeityManifest));
    sDictGeneric = newDict(sZ64AllManifest, ARRAY_COUNT(sZ64AllManifest));
}
