#include "save_editor_builtin.h"

#include "variables.h"
#include "macros.h"
#include "functions.h"

#define SAVE_EDITOR_MAGIC_SINGLE_METER 0x30
#define MIN_HEARTS 3
#define MAX_HEARTS 20
#define MAX_BANK_RUPEES 5000
#define SKULL_TOKEN_SWAMP_SHIFT 16
#define SKULL_TOKEN_MASK 0xFFFF

static s32 clamp_s32(s32 value, s32 min, s32 max) {
    if (value < min) {
        return min;
    }
    if (value > max) {
        return max;
    }
    return value;
}

static s32 display_day_to_save_day(s32 day) {
    return clamp_s32(day, 1, 4) - 1;
}

static s32 save_day_to_display_day(s32 day) {
    return clamp_s32(day + 1, 1, 4);
}

static void set_upgrade_value(s32 upgrade, u32 value) {
    gSaveContext.save.saveInfo.inventory.upgrades =
        (GET_SAVE_INVENTORY_UPGRADES & gUpgradeNegMasks[upgrade]) | (value << gUpgradeShifts[upgrade]);
}

static void give_item_with_ammo(s32 item, s32 ammo) {
    INV_CONTENT(item) = item;
    AMMO(item) = ammo;
}

typedef struct {
    BuiltinSaveEditorValueId value_id;
    s32 item;
} SaveEditorItemToggle;

typedef struct {
    BuiltinSaveEditorValueId value_id;
    s32 quest;
} SaveEditorQuestToggle;

typedef struct {
    BuiltinSaveEditorValueId map_id;
    BuiltinSaveEditorValueId compass_id;
    BuiltinSaveEditorValueId boss_key_id;
    BuiltinSaveEditorValueId small_keys_id;
    BuiltinSaveEditorValueId stray_fairies_id;
    s32 dungeon;
    s32 max_keys;
} SaveEditorDungeon;

typedef struct {
    BuiltinSaveEditorValueId value_id;
    s32 tingle_map;
    s32 week_event_flag;
    u16 cloud_mask;
} SaveEditorTingleMap;

static const SaveEditorItemToggle s_item_toggles[] = {
    { SAVE_EDITOR_VALUE_POWDER_KEG, ITEM_POWDER_KEG },
    { SAVE_EDITOR_VALUE_OCARINA, ITEM_OCARINA_OF_TIME },
    { SAVE_EDITOR_VALUE_FIRE_ARROWS, ITEM_ARROW_FIRE },
    { SAVE_EDITOR_VALUE_ICE_ARROWS, ITEM_ARROW_ICE },
    { SAVE_EDITOR_VALUE_LIGHT_ARROWS, ITEM_ARROW_LIGHT },
    { SAVE_EDITOR_VALUE_MAGIC_BEANS, ITEM_MAGIC_BEANS },
    { SAVE_EDITOR_VALUE_PICTOGRAPH_BOX, ITEM_PICTOGRAPH_BOX },
    { SAVE_EDITOR_VALUE_LENS_OF_TRUTH, ITEM_LENS_OF_TRUTH },
    { SAVE_EDITOR_VALUE_HOOKSHOT, ITEM_HOOKSHOT },
    { SAVE_EDITOR_VALUE_GREAT_FAIRY_SWORD, ITEM_SWORD_GREAT_FAIRY },
    { SAVE_EDITOR_VALUE_ROOM_KEY, ITEM_ROOM_KEY },
    { SAVE_EDITOR_VALUE_LETTER_MAMA, ITEM_LETTER_MAMA },
    { SAVE_EDITOR_VALUE_LETTER_KAFEI, ITEM_LETTER_TO_KAFEI },
    { SAVE_EDITOR_VALUE_PENDANT_OF_MEMORIES, ITEM_PENDANT_OF_MEMORIES },
    { SAVE_EDITOR_VALUE_DEKU_MASK, ITEM_MASK_DEKU },
    { SAVE_EDITOR_VALUE_GORON_MASK, ITEM_MASK_GORON },
    { SAVE_EDITOR_VALUE_ZORA_MASK, ITEM_MASK_ZORA },
    { SAVE_EDITOR_VALUE_FIERCE_DEITY_MASK, ITEM_MASK_FIERCE_DEITY },
    { SAVE_EDITOR_VALUE_MASK_OF_TRUTH, ITEM_MASK_TRUTH },
    { SAVE_EDITOR_VALUE_KAFEIS_MASK, ITEM_MASK_KAFEIS_MASK },
    { SAVE_EDITOR_VALUE_ALL_NIGHT_MASK, ITEM_MASK_ALL_NIGHT },
    { SAVE_EDITOR_VALUE_BUNNY_HOOD, ITEM_MASK_BUNNY },
    { SAVE_EDITOR_VALUE_KEATON_MASK, ITEM_MASK_KEATON },
    { SAVE_EDITOR_VALUE_GARO_MASK, ITEM_MASK_GARO },
    { SAVE_EDITOR_VALUE_ROMANI_MASK, ITEM_MASK_ROMANI },
    { SAVE_EDITOR_VALUE_CIRCUS_LEADER_MASK, ITEM_MASK_CIRCUS_LEADER },
    { SAVE_EDITOR_VALUE_POSTMAN_HAT, ITEM_MASK_POSTMAN },
    { SAVE_EDITOR_VALUE_COUPLES_MASK, ITEM_MASK_COUPLE },
    { SAVE_EDITOR_VALUE_GREAT_FAIRY_MASK, ITEM_MASK_GREAT_FAIRY },
    { SAVE_EDITOR_VALUE_GIBDO_MASK, ITEM_MASK_GIBDO },
    { SAVE_EDITOR_VALUE_DON_GERO_MASK, ITEM_MASK_DON_GERO },
    { SAVE_EDITOR_VALUE_KAMARO_MASK, ITEM_MASK_KAMARO },
    { SAVE_EDITOR_VALUE_CAPTAINS_HAT, ITEM_MASK_CAPTAIN },
    { SAVE_EDITOR_VALUE_STONE_MASK, ITEM_MASK_STONE },
    { SAVE_EDITOR_VALUE_BREMEN_MASK, ITEM_MASK_BREMEN },
    { SAVE_EDITOR_VALUE_BLAST_MASK, ITEM_MASK_BLAST },
    { SAVE_EDITOR_VALUE_MASK_OF_SCENTS, ITEM_MASK_SCENTS },
    { SAVE_EDITOR_VALUE_GIANTS_MASK, ITEM_MASK_GIANT },
};

static const SaveEditorQuestToggle s_remains[] = {
    { SAVE_EDITOR_VALUE_ODOLWA_REMAINS, QUEST_REMAINS_ODOLWA },
    { SAVE_EDITOR_VALUE_GOHT_REMAINS, QUEST_REMAINS_GOHT },
    { SAVE_EDITOR_VALUE_GYORG_REMAINS, QUEST_REMAINS_GYORG },
    { SAVE_EDITOR_VALUE_TWINMOLD_REMAINS, QUEST_REMAINS_TWINMOLD },
};

static const SaveEditorQuestToggle s_songs[] = {
    { SAVE_EDITOR_VALUE_SONATA, QUEST_SONG_SONATA },
    { SAVE_EDITOR_VALUE_LULLABY, QUEST_SONG_LULLABY },
    { SAVE_EDITOR_VALUE_BOSSA_NOVA, QUEST_SONG_BOSSA_NOVA },
    { SAVE_EDITOR_VALUE_ELEGY, QUEST_SONG_ELEGY },
    { SAVE_EDITOR_VALUE_OATH, QUEST_SONG_OATH },
    { SAVE_EDITOR_VALUE_SARIAS_SONG, QUEST_SONG_SARIA },
    { SAVE_EDITOR_VALUE_SONG_OF_TIME, QUEST_SONG_TIME },
    { SAVE_EDITOR_VALUE_SONG_OF_HEALING, QUEST_SONG_HEALING },
    { SAVE_EDITOR_VALUE_EPONAS_SONG, QUEST_SONG_EPONA },
    { SAVE_EDITOR_VALUE_SONG_OF_SOARING, QUEST_SONG_SOARING },
    { SAVE_EDITOR_VALUE_SONG_OF_STORMS, QUEST_SONG_STORMS },
    { SAVE_EDITOR_VALUE_SUNS_SONG, QUEST_SONG_SUN },
};

static const SaveEditorDungeon s_dungeons[] = {
    { SAVE_EDITOR_VALUE_WOODFALL_MAP, SAVE_EDITOR_VALUE_WOODFALL_COMPASS,
      SAVE_EDITOR_VALUE_WOODFALL_BOSS_KEY, SAVE_EDITOR_VALUE_WOODFALL_SMALL_KEYS,
      SAVE_EDITOR_VALUE_WOODFALL_STRAY_FAIRIES, DUNGEON_INDEX_WOODFALL_TEMPLE, 1 },
    { SAVE_EDITOR_VALUE_SNOWHEAD_MAP, SAVE_EDITOR_VALUE_SNOWHEAD_COMPASS,
      SAVE_EDITOR_VALUE_SNOWHEAD_BOSS_KEY, SAVE_EDITOR_VALUE_SNOWHEAD_SMALL_KEYS,
      SAVE_EDITOR_VALUE_SNOWHEAD_STRAY_FAIRIES, DUNGEON_INDEX_SNOWHEAD_TEMPLE, 3 },
    { SAVE_EDITOR_VALUE_GREAT_BAY_MAP, SAVE_EDITOR_VALUE_GREAT_BAY_COMPASS,
      SAVE_EDITOR_VALUE_GREAT_BAY_BOSS_KEY, SAVE_EDITOR_VALUE_GREAT_BAY_SMALL_KEYS,
      SAVE_EDITOR_VALUE_GREAT_BAY_STRAY_FAIRIES, DUNGEON_INDEX_GREAT_BAY_TEMPLE, 1 },
    { SAVE_EDITOR_VALUE_STONE_TOWER_MAP, SAVE_EDITOR_VALUE_STONE_TOWER_COMPASS,
      SAVE_EDITOR_VALUE_STONE_TOWER_BOSS_KEY, SAVE_EDITOR_VALUE_STONE_TOWER_SMALL_KEYS,
      SAVE_EDITOR_VALUE_STONE_TOWER_STRAY_FAIRIES, DUNGEON_INDEX_STONE_TOWER_TEMPLE, 4 },
};

static const SaveEditorTingleMap s_tingle_maps[] = {
    { SAVE_EDITOR_VALUE_TINGLE_MAP_CLOCK_TOWN, TINGLE_MAP_CLOCK_TOWN,
      WEEKEVENTREG_TINGLE_MAP_BOUGHT_CLOCK_TOWN, 0x0003 },
    { SAVE_EDITOR_VALUE_TINGLE_MAP_WOODFALL, TINGLE_MAP_WOODFALL,
      WEEKEVENTREG_TINGLE_MAP_BOUGHT_WOODFALL, 0x001C },
    { SAVE_EDITOR_VALUE_TINGLE_MAP_SNOWHEAD, TINGLE_MAP_SNOWHEAD,
      WEEKEVENTREG_TINGLE_MAP_BOUGHT_SNOWHEAD, 0x00E0 },
    { SAVE_EDITOR_VALUE_TINGLE_MAP_ROMANI_RANCH, TINGLE_MAP_ROMANI_RANCH,
      WEEKEVENTREG_TINGLE_MAP_BOUGHT_ROMANI_RANCH, 0x0100 },
    { SAVE_EDITOR_VALUE_TINGLE_MAP_GREAT_BAY, TINGLE_MAP_GREAT_BAY,
      WEEKEVENTREG_TINGLE_MAP_BOUGHT_GREAT_BAY, 0x1E00 },
    { SAVE_EDITOR_VALUE_TINGLE_MAP_STONE_TOWER, TINGLE_MAP_STONE_TOWER,
      WEEKEVENTREG_TINGLE_MAP_BOUGHT_STONE_TOWER, 0x6000 },
};

static s32 wallet_cap(void) {
    switch (GET_CUR_UPG_VALUE(UPG_WALLET)) {
        case 0:
            return 99;
        case 1:
            return 200;
        case 2:
            return 500;
        default:
            return 999;
    }
}

static s32 current_magic_level(void) {
    if (!gSaveContext.save.saveInfo.playerData.isMagicAcquired) {
        return 0;
    }

    if (gSaveContext.save.saveInfo.playerData.magicLevel != 0) {
        return clamp_s32(gSaveContext.save.saveInfo.playerData.magicLevel, 1, 2);
    }

    return gSaveContext.save.saveInfo.playerData.isDoubleMagicAcquired ? 2 : 1;
}

static void set_quest_flag(s32 quest, bool enabled) {
    if (enabled) {
        SET_QUEST_ITEM(quest);
    } else {
        REMOVE_QUEST_ITEM(quest);
    }
}

static void set_dungeon_flag(s32 dungeon, s32 item, bool enabled) {
    if (enabled) {
        SET_DUNGEON_ITEM(item, dungeon);
    } else {
        gSaveContext.save.saveInfo.inventory.dungeonItems[dungeon] &= (u8)~gBitFlags[item];
    }
}

static void set_tingle_map(const SaveEditorTingleMap* map, bool enabled) {
    if (enabled) {
        Inventory_SetWorldMapCloudVisibility(map->tingle_map);
        SET_WEEKEVENTREG(map->week_event_flag);
    } else {
        CLEAR_WEEKEVENTREG(map->week_event_flag);
        gSaveContext.save.saveInfo.worldMapCloudVisibility &= (u16)~map->cloud_mask;
    }
}

static void set_heart_piece_count(u32 count) {
    gSaveContext.save.saveInfo.inventory.questItems &= ~0xF0000000;
    gSaveContext.save.saveInfo.inventory.questItems |= (clamp_s32(count, 0, 3) << QUEST_HEART_PIECE_COUNT);
}

static void apply_live_day_time(PlayState* play, s32 day, s32 time, s32 time_speed_offset) {
    u8 event_inf_bits;

    gSaveContext.save.day = display_day_to_save_day(day);
    gSaveContext.save.eventDayCount = gSaveContext.save.day;
    gSaveContext.save.time = clamp_s32(time, 0, 0xFFFF);
    gSaveContext.save.timeSpeedOffset = clamp_s32(time_speed_offset, -2, 18);
    gSaveContext.skyboxTime = gSaveContext.save.time;
    gSaveContext.save.isNight =
        ((CURRENT_TIME >= CLOCK_TIME(18, 0)) || (CURRENT_TIME < CLOCK_TIME(6, 0))) ? true : false;
    EVENTINF_SET_7_E0(gSaveContext.save.day, event_inf_bits);
    Interface_NewDay(play, CURRENT_DAY);
}

static void clamp_live_save(void) {
    s32 max_rupees = wallet_cap();

    gSaveContext.save.day = clamp_s32(gSaveContext.save.day, 0, 3);
    gSaveContext.save.eventDayCount = clamp_s32(gSaveContext.save.eventDayCount, 0, 3);
    gSaveContext.save.timeSpeedOffset = clamp_s32(gSaveContext.save.timeSpeedOffset, -2, 18);
    gSaveContext.save.saveInfo.playerData.healthCapacity =
        clamp_s32(gSaveContext.save.saveInfo.playerData.healthCapacity, MIN_HEARTS * 0x10, MAX_HEARTS * 0x10);
    gSaveContext.save.saveInfo.playerData.health =
        clamp_s32(gSaveContext.save.saveInfo.playerData.health, 0, gSaveContext.save.saveInfo.playerData.healthCapacity);
    gSaveContext.save.saveInfo.playerData.rupees =
        clamp_s32(gSaveContext.save.saveInfo.playerData.rupees, 0, max_rupees);
    gSaveContext.save.saveInfo.playerData.magicLevel =
        clamp_s32(gSaveContext.save.saveInfo.playerData.magicLevel, 0, 2);
    s32 magic_capacity = current_magic_level() * SAVE_EDITOR_MAGIC_SINGLE_METER;
    if (gSaveContext.magicCapacity < magic_capacity) {
        gSaveContext.magicCapacity = magic_capacity;
    }
    gSaveContext.magicCapacity = clamp_s32(gSaveContext.magicCapacity, 0, SAVE_EDITOR_MAGIC_SINGLE_METER * 2);
    gSaveContext.save.saveInfo.playerData.magic =
        clamp_s32(gSaveContext.save.saveInfo.playerData.magic, 0, magic_capacity);
}

static void sync_snapshot_from_save(void) {
    clamp_live_save();
    recomp_save_editor_set_snapshot_value(SAVE_EDITOR_VALUE_DAY, save_day_to_display_day(gSaveContext.save.day));
    recomp_save_editor_set_snapshot_value(SAVE_EDITOR_VALUE_TIME, gSaveContext.save.time);
    recomp_save_editor_set_snapshot_value(SAVE_EDITOR_VALUE_TIME_SPEED, gSaveContext.save.timeSpeedOffset);
    recomp_save_editor_set_snapshot_value(SAVE_EDITOR_VALUE_TATL, gSaveContext.save.hasTatl ? 1 : 0);
    recomp_save_editor_set_snapshot_value(SAVE_EDITOR_VALUE_INTRO_COMPLETE, gSaveContext.save.isFirstCycle ? 1 : 0);
    recomp_save_editor_set_snapshot_value(SAVE_EDITOR_VALUE_OWL_SAVE, gSaveContext.save.isOwlSave ? 1 : 0);
    recomp_save_editor_set_snapshot_value(SAVE_EDITOR_VALUE_WALLET, GET_CUR_UPG_VALUE(UPG_WALLET));
    recomp_save_editor_set_snapshot_value(SAVE_EDITOR_VALUE_RUPEES, gSaveContext.save.saveInfo.playerData.rupees);
    recomp_save_editor_set_snapshot_value(SAVE_EDITOR_VALUE_BANK_RUPEES, HS_GET_BANK_RUPEES());
    recomp_save_editor_set_snapshot_value(SAVE_EDITOR_VALUE_HEARTS, gSaveContext.save.saveInfo.playerData.healthCapacity / 0x10);
    recomp_save_editor_set_snapshot_value(SAVE_EDITOR_VALUE_HEALTH, gSaveContext.save.saveInfo.playerData.health);
    recomp_save_editor_set_snapshot_value(SAVE_EDITOR_VALUE_DOUBLE_DEFENSE, gSaveContext.save.saveInfo.playerData.doubleDefense ? 1 : 0);
    recomp_save_editor_set_snapshot_value(SAVE_EDITOR_VALUE_MAGIC_LEVEL, current_magic_level());
    recomp_save_editor_set_snapshot_value(SAVE_EDITOR_VALUE_MAGIC, gSaveContext.save.saveInfo.playerData.magic);
    recomp_save_editor_set_snapshot_value(SAVE_EDITOR_VALUE_SWORD, GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD));
    recomp_save_editor_set_snapshot_value(SAVE_EDITOR_VALUE_SHIELD, GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SHIELD));
    recomp_save_editor_set_snapshot_value(SAVE_EDITOR_VALUE_QUIVER, GET_CUR_UPG_VALUE(UPG_QUIVER));
    recomp_save_editor_set_snapshot_value(SAVE_EDITOR_VALUE_BOMB_BAG, GET_CUR_UPG_VALUE(UPG_BOMB_BAG));
    recomp_save_editor_set_snapshot_value(SAVE_EDITOR_VALUE_STICK_UPGRADE, GET_CUR_UPG_VALUE(UPG_DEKU_STICKS));
    recomp_save_editor_set_snapshot_value(SAVE_EDITOR_VALUE_NUT_UPGRADE, GET_CUR_UPG_VALUE(UPG_DEKU_NUTS));
    recomp_save_editor_set_snapshot_value(SAVE_EDITOR_VALUE_ARROWS, AMMO(ITEM_BOW));
    recomp_save_editor_set_snapshot_value(SAVE_EDITOR_VALUE_BOMBS, AMMO(ITEM_BOMB));
    recomp_save_editor_set_snapshot_value(SAVE_EDITOR_VALUE_BOMBCHUS, AMMO(ITEM_BOMBCHU));
    recomp_save_editor_set_snapshot_value(SAVE_EDITOR_VALUE_DEKU_STICKS, AMMO(ITEM_DEKU_STICK));
    recomp_save_editor_set_snapshot_value(SAVE_EDITOR_VALUE_DEKU_NUTS, AMMO(ITEM_DEKU_NUT));
    for (s32 i = 0; i < ARRAY_COUNT(s_item_toggles); i++) {
        recomp_save_editor_set_snapshot_value(
            s_item_toggles[i].value_id,
            GET_INV_CONTENT(s_item_toggles[i].item) == s_item_toggles[i].item ? 1 : 0);
    }
    for (s32 i = 0; i < ARRAY_COUNT(s_tingle_maps); i++) {
        recomp_save_editor_set_snapshot_value(s_tingle_maps[i].value_id,
                                              CHECK_WEEKEVENTREG(s_tingle_maps[i].week_event_flag) ? 1 : 0);
    }
    recomp_save_editor_set_snapshot_value(SAVE_EDITOR_VALUE_HEART_PIECES, GET_QUEST_HEART_PIECE_COUNT);
    recomp_save_editor_set_snapshot_value(SAVE_EDITOR_VALUE_BOMBERS_NOTEBOOK,
                                          CHECK_QUEST_ITEM(QUEST_BOMBERS_NOTEBOOK) ? 1 : 0);
    for (s32 i = 0; i < ARRAY_COUNT(s_remains); i++) {
        recomp_save_editor_set_snapshot_value(s_remains[i].value_id, CHECK_QUEST_ITEM(s_remains[i].quest) ? 1 : 0);
    }
    for (s32 i = 0; i < ARRAY_COUNT(s_songs); i++) {
        recomp_save_editor_set_snapshot_value(s_songs[i].value_id, CHECK_QUEST_ITEM(s_songs[i].quest) ? 1 : 0);
    }
    for (s32 i = 0; i < ARRAY_COUNT(s_dungeons); i++) {
        const SaveEditorDungeon* dungeon = &s_dungeons[i];
        recomp_save_editor_set_snapshot_value(dungeon->map_id, CHECK_DUNGEON_ITEM(DUNGEON_MAP, dungeon->dungeon) ? 1 : 0);
        recomp_save_editor_set_snapshot_value(dungeon->compass_id, CHECK_DUNGEON_ITEM(DUNGEON_COMPASS, dungeon->dungeon) ? 1 : 0);
        recomp_save_editor_set_snapshot_value(dungeon->boss_key_id, CHECK_DUNGEON_ITEM(DUNGEON_BOSS_KEY, dungeon->dungeon) ? 1 : 0);
        recomp_save_editor_set_snapshot_value(dungeon->small_keys_id, DUNGEON_KEY_COUNT(dungeon->dungeon));
        recomp_save_editor_set_snapshot_value(dungeon->stray_fairies_id,
                                              gSaveContext.save.saveInfo.inventory.strayFairies[dungeon->dungeon]);
    }
}

static s32 pending(BuiltinSaveEditorValueId id) {
    return recomp_save_editor_get_pending_value(id);
}

static void apply_pending_to_save(PlayState* play) {
    s32 hearts = clamp_s32(pending(SAVE_EDITOR_VALUE_HEARTS), MIN_HEARTS, MAX_HEARTS);
    s32 magic_level = clamp_s32(pending(SAVE_EDITOR_VALUE_MAGIC_LEVEL), 0, 2);
    s32 pending_magic = pending(SAVE_EDITOR_VALUE_MAGIC);

    apply_live_day_time(play, pending(SAVE_EDITOR_VALUE_DAY), pending(SAVE_EDITOR_VALUE_TIME),
                        pending(SAVE_EDITOR_VALUE_TIME_SPEED));
    gSaveContext.save.hasTatl = pending(SAVE_EDITOR_VALUE_TATL) != 0;
    gSaveContext.save.isFirstCycle = pending(SAVE_EDITOR_VALUE_INTRO_COMPLETE) != 0;
    gSaveContext.save.isOwlSave = pending(SAVE_EDITOR_VALUE_OWL_SAVE) != 0;
    set_upgrade_value(UPG_WALLET, clamp_s32(pending(SAVE_EDITOR_VALUE_WALLET), 0, 2));
    gSaveContext.save.saveInfo.playerData.rupees = pending(SAVE_EDITOR_VALUE_RUPEES);
    HS_SET_BANK_RUPEES(clamp_s32(pending(SAVE_EDITOR_VALUE_BANK_RUPEES), 0, MAX_BANK_RUPEES));
    gSaveContext.save.saveInfo.playerData.healthCapacity = hearts * 0x10;
    gSaveContext.save.saveInfo.playerData.health =
        clamp_s32(pending(SAVE_EDITOR_VALUE_HEALTH), 0, gSaveContext.save.saveInfo.playerData.healthCapacity);
    gSaveContext.save.saveInfo.playerData.doubleDefense = pending(SAVE_EDITOR_VALUE_DOUBLE_DEFENSE) != 0;
    gSaveContext.save.saveInfo.inventory.defenseHearts =
        gSaveContext.save.saveInfo.playerData.doubleDefense ? MAX_HEARTS : 0;
    gSaveContext.save.saveInfo.playerData.magicLevel = magic_level;
    gSaveContext.save.saveInfo.playerData.isMagicAcquired = magic_level >= 1;
    gSaveContext.save.saveInfo.playerData.isDoubleMagicAcquired = magic_level >= 2;
    gSaveContext.magicCapacity = magic_level * SAVE_EDITOR_MAGIC_SINGLE_METER;
    gSaveContext.save.saveInfo.playerData.magic =
        clamp_s32(pending_magic, 0, gSaveContext.magicCapacity);
    gSaveContext.magicFillTarget = gSaveContext.magicCapacity;

    SET_EQUIP_VALUE(EQUIP_TYPE_SWORD,
                    clamp_s32(pending(SAVE_EDITOR_VALUE_SWORD), EQUIP_VALUE_SWORD_NONE, EQUIP_VALUE_SWORD_GILDED));
    SET_EQUIP_VALUE(EQUIP_TYPE_SHIELD, clamp_s32(pending(SAVE_EDITOR_VALUE_SHIELD), 0, 2));
    set_upgrade_value(UPG_QUIVER, clamp_s32(pending(SAVE_EDITOR_VALUE_QUIVER), 0, 3));
    set_upgrade_value(UPG_BOMB_BAG, clamp_s32(pending(SAVE_EDITOR_VALUE_BOMB_BAG), 0, 3));
    set_upgrade_value(UPG_DEKU_STICKS, clamp_s32(pending(SAVE_EDITOR_VALUE_STICK_UPGRADE), 0, 3));
    set_upgrade_value(UPG_DEKU_NUTS, clamp_s32(pending(SAVE_EDITOR_VALUE_NUT_UPGRADE), 0, 3));
    if (pending(SAVE_EDITOR_VALUE_ARROWS) > 0) {
        give_item_with_ammo(ITEM_BOW, pending(SAVE_EDITOR_VALUE_ARROWS));
    }
    if (pending(SAVE_EDITOR_VALUE_BOMBS) > 0) {
        give_item_with_ammo(ITEM_BOMB, pending(SAVE_EDITOR_VALUE_BOMBS));
    }
    if (pending(SAVE_EDITOR_VALUE_BOMBCHUS) > 0) {
        give_item_with_ammo(ITEM_BOMBCHU, pending(SAVE_EDITOR_VALUE_BOMBCHUS));
    }
    if (pending(SAVE_EDITOR_VALUE_DEKU_STICKS) > 0) {
        give_item_with_ammo(ITEM_DEKU_STICK, pending(SAVE_EDITOR_VALUE_DEKU_STICKS));
    }
    if (pending(SAVE_EDITOR_VALUE_DEKU_NUTS) > 0) {
        give_item_with_ammo(ITEM_DEKU_NUT, pending(SAVE_EDITOR_VALUE_DEKU_NUTS));
    }
    for (s32 i = 0; i < ARRAY_COUNT(s_item_toggles); i++) {
        INV_CONTENT(s_item_toggles[i].item) = pending(s_item_toggles[i].value_id) != 0 ? s_item_toggles[i].item : ITEM_NONE;
    }
    for (s32 i = 0; i < ARRAY_COUNT(s_tingle_maps); i++) {
        set_tingle_map(&s_tingle_maps[i], pending(s_tingle_maps[i].value_id) != 0);
    }
    set_heart_piece_count(pending(SAVE_EDITOR_VALUE_HEART_PIECES));
    set_quest_flag(QUEST_BOMBERS_NOTEBOOK, pending(SAVE_EDITOR_VALUE_BOMBERS_NOTEBOOK) != 0);
    for (s32 i = 0; i < ARRAY_COUNT(s_remains); i++) {
        set_quest_flag(s_remains[i].quest, pending(s_remains[i].value_id) != 0);
    }
    for (s32 i = 0; i < ARRAY_COUNT(s_songs); i++) {
        set_quest_flag(s_songs[i].quest, pending(s_songs[i].value_id) != 0);
    }
    for (s32 i = 0; i < ARRAY_COUNT(s_dungeons); i++) {
        const SaveEditorDungeon* dungeon = &s_dungeons[i];
        set_dungeon_flag(dungeon->dungeon, DUNGEON_MAP, pending(dungeon->map_id) != 0);
        set_dungeon_flag(dungeon->dungeon, DUNGEON_COMPASS, pending(dungeon->compass_id) != 0);
        set_dungeon_flag(dungeon->dungeon, DUNGEON_BOSS_KEY, pending(dungeon->boss_key_id) != 0);
        gSaveContext.save.saveInfo.inventory.strayFairies[dungeon->dungeon] =
            clamp_s32(pending(dungeon->stray_fairies_id), 0, STRAY_FAIRY_SCATTERED_TOTAL);
        set_dungeon_flag(dungeon->dungeon, DUNGEON_STRAY_FAIRIES,
                         gSaveContext.save.saveInfo.inventory.strayFairies[dungeon->dungeon] >= STRAY_FAIRY_SCATTERED_TOTAL);
        DUNGEON_KEY_COUNT(dungeon->dungeon) = clamp_s32(pending(dungeon->small_keys_id), 0, dungeon->max_keys);
    }

    clamp_live_save();
    gSaveContext.save.saveInfo.checksum = Sram_CalcChecksum(&gSaveContext.save, sizeof(Save));
}

void save_editor_builtin_play_update(PlayState* play) {
    if (recomp_save_editor_should_apply_pending()) {
        apply_pending_to_save(play);
        recomp_save_editor_clear_pending();
    }
    sync_snapshot_from_save();
}
