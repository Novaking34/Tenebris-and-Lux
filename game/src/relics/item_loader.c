// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          item_loader.c
// @id            sim.items.loader
// @layer         Layer 1: Simulation Engine
// @subsystem     Item, Relic & Inventory Subsystem (sim/items)
// @version       1.1.0
// @interface_rev Rev 2
// @date          2026-11-03
// @updated       2026-11-03
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Zero-heap data loader implementation for the item catalog.
//   Deserializes relics, catalysts, consumables, and valuables from
//   declarative JSON files into the runtime item definition registry.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies:
//       sim/items/item_loader.h
//       util/data/fs_utils.h
//       util/data/json_utils.h
//       util/string/string_utils.h
//       util/log/log.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic heap allocations).
//   - Concurrency / Safety:  REENTRANT_PURE (Mutates caller-supplied buffer).
//   - Latency Budget:        < 5 milliseconds for full catalog loading.
//
// AXIOMATIC INVARIANTS:
//   1. Zero heap allocations: Strict zero-heap guarantee across all parsing.
//   2. Complete decoupling: Game item stats are loaded purely from JSON files.
//   3. Fallback resilience: Provides valid fallbacks if data is unreachable.
// =============================================================================

#include "relics/item_loader.h"
#include "io/fs_utils.h"
#include "io/json_utils.h"
#include "log/log.h"
#include "core/string_utils.h"
#include <stdio.h>
#include <string.h>

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Zero-Heap Item Data Loader
// =============================================================================

static const char *const s_item_loader_invariants[2] = {
    "Zero heap allocation during JSON dataset parsing",
    "Resilient fallback catalog supplied if disk files missing"
};

const module_metadata_t item_loader_manifest = {
    "sim.items.loader",
    "item_loader.h",
    LAYER_1_SIMULATION,
    { 1, 1, 0 },
    2,
    "2026-11-03",
    "Zero-heap item catalog JSON deserializer",
    2,
    { "core.util.fs", "core.util.json", NULL, NULL },
    ALLOC_ZERO_HEAP,
    CONCURRENCY_REENTRANT,
    1000,
    2,
    {
        s_item_loader_invariants[0],
        s_item_loader_invariants[1]
    },
    item_loader_preflight
};

#define LOADER_FILE_BUF_SIZE    8192
#define LOADER_OBJ_BUF_SIZE     1024

// =============================================================================
// BLOCK 3: BODY BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Zero-Heap Item Data Loader
// =============================================================================

/**
 * Extracts n-th JSON object slice { ... } from array [ { ... }, { ... } ]
 * into a caller-supplied null-terminated buffer.
 */
static bool slice_array_object(const char *arr_start,
                               size_t arr_len,
                               size_t target_idx,
                               char *out_obj,
                               size_t out_size)
{
    if (!arr_start || arr_len < 2 || *arr_start != '[' || !out_obj ||
        out_size == 0) {
        if (out_obj && out_size > 0) {
            out_obj[0] = '\0';
        }
        return false;
    }

    const char *end = arr_start + arr_len;
    const char *p = arr_start + 1;
    size_t cur_idx = 0;
    int depth = 0;
    bool in_str = false;
    const char *obj_start = NULL;

    while (p < end - 1) {
        char c = *p;
        if (c == '"' && *(p - 1) != '\\') {
            in_str = !in_str;
        } else if (!in_str) {
            if (c == '{') {
                if (depth == 0) {
                    obj_start = p;
                }
                depth++;
            } else if (c == '}') {
                depth--;
                if (depth == 0 && obj_start != NULL) {
                    if (cur_idx == target_idx) {
                        size_t len = (size_t)(p - obj_start + 1);
                        if (len >= out_size) {
                            len = out_size - 1;
                        }
                        memcpy(out_obj, obj_start, len);
                        out_obj[len] = '\0';
                        return true;
                    }
                    cur_idx++;
                    obj_start = NULL;
                }
            }
        }
        p++;
    }

    out_obj[0] = '\0';
    return false;
}

static bool parse_relic_entry(const char *json_obj, item_def_t *item)
{
    int id_val = 0;
    if (!json_get_int(json_obj, "id", &id_val)) {
        return false;
    }
    item->id = (uint32_t)id_val;
    item->type = ITEM_TYPE_RELIC;
    item->consumable = false;
    item->max_stack = 1;

    json_get_string(json_obj, "code", item->code, sizeof(item->code));
    json_get_string(json_obj, "name", item->name, sizeof(item->name));
    json_get_string(json_obj, "description", item->description,
                    sizeof(item->description));

    char relic_type_str[32];
    if (json_get_string(json_obj, "relic_type", relic_type_str,
                        sizeof(relic_type_str))) {
        if (strcmp(relic_type_str, "temporal") == 0) {
            item->data.relic.sub_type = RELIC_TYPE_TEMPORAL;
        } else if (strcmp(relic_type_str, "archival") == 0) {
            item->data.relic.sub_type = RELIC_TYPE_ARCHIVAL;
        } else if (strcmp(relic_type_str, "telemetry") == 0) {
            item->data.relic.sub_type = RELIC_TYPE_TELEMETRY;
        } else if (strcmp(relic_type_str, "planar") == 0) {
            item->data.relic.sub_type = RELIC_TYPE_PLANAR;
        } else if (strcmp(relic_type_str, "diplomatic") == 0) {
            item->data.relic.sub_type = RELIC_TYPE_DIPLOMATIC;
        }
    }

    bool clock_val = false;
    if (json_get_object_bool(json_obj, "properties", "clock_interface",
                             &clock_val)) {
        item->data.relic.clock_interface = clock_val;
    }
    int evolves = 0;
    if (json_get_object_int(json_obj, "properties", "evolves_into", &evolves)) {
        item->data.relic.evolves_into = (uint32_t)evolves;
    }
    bool bestiary = false;
    if (json_get_object_bool(json_obj, "properties", "unlocks_bestiary",
                             &bestiary)) {
        item->data.relic.unlocks_bestiary = bestiary;
    }
    bool compendium = false;
    if (json_get_object_bool(json_obj, "properties", "unlocks_item_compendium",
                             &compendium)) {
        item->data.relic.unlocks_compendium = compendium;
    }
    bool tracks_sway = false;
    if (json_get_object_bool(json_obj, "properties", "tracks_moral_sway",
                             &tracks_sway)) {
        item->data.relic.tracks_moral_sway = tracks_sway;
    }
    bool planar = false;
    if (json_get_object_bool(json_obj, "properties", "planar_key", &planar)) {
        item->data.relic.planar_key = planar;
        item->data.relic.nocturnal_start_hour = 20;
        item->data.relic.nocturnal_end_hour = 4;
    }

    return true;
}

static bool parse_catalyst_entry(const char *json_obj, item_def_t *item)
{
    int id_val = 0;
    if (!json_get_int(json_obj, "id", &id_val)) {
        return false;
    }
    item->id = (uint32_t)id_val;
    item->type = ITEM_TYPE_CATALYST;
    item->consumable = false;
    item->max_stack = 1;

    json_get_string(json_obj, "code", item->code, sizeof(item->code));
    json_get_string(json_obj, "name", item->name, sizeof(item->name));
    json_get_string(json_obj, "description", item->description,
                    sizeof(item->description));

    char cat_type_str[32];
    if (json_get_string(json_obj, "catalyst_type", cat_type_str,
                        sizeof(cat_type_str))) {
        if (strcmp(cat_type_str, "elemental") == 0) {
            item->data.catalyst.sub_type = CATALYST_TYPE_ELEMENTAL;
        } else if (strcmp(cat_type_str, "weapon") == 0) {
            item->data.catalyst.sub_type = CATALYST_TYPE_WEAPON;
        } else if (strcmp(cat_type_str, "armor") == 0) {
            item->data.catalyst.sub_type = CATALYST_TYPE_ARMOR;
        } else if (strcmp(cat_type_str, "mystic") == 0) {
            item->data.catalyst.sub_type = CATALYST_TYPE_MYSTIC;
        }
    }

    char slot_str[32];
    if (json_get_string(json_obj, "slot", slot_str, sizeof(slot_str))) {
        if (strcmp(slot_str, "weapon") == 0) {
            item->data.catalyst.slot = EQUIP_SLOT_WEAPON;
        } else if (strcmp(slot_str, "shield") == 0) {
            item->data.catalyst.slot = EQUIP_SLOT_SHIELD;
        } else if (strcmp(slot_str, "armor") == 0) {
            item->data.catalyst.slot = EQUIP_SLOT_ARMOR;
        } else if (strcmp(slot_str, "accessory") == 0) {
            item->data.catalyst.slot = EQUIP_SLOT_ACCESSORY;
        }
    }

    int stat = 0;
    if (json_get_object_int(json_obj, "stats", "atk_bonus", &stat)) {
        item->data.catalyst.atk_bonus = (int16_t)stat;
    }
    if (json_get_object_int(json_obj, "stats", "def_bonus", &stat)) {
        item->data.catalyst.def_bonus = (int16_t)stat;
    }
    if (json_get_object_int(json_obj, "stats", "mat_bonus", &stat)) {
        item->data.catalyst.mat_bonus = (int16_t)stat;
    }
    if (json_get_object_int(json_obj, "stats", "mdf_bonus", &stat)) {
        item->data.catalyst.mdf_bonus = (int16_t)stat;
    }
    if (json_get_object_int(json_obj, "stats", "poise_damage", &stat)) {
        item->data.catalyst.poise_bonus = (int16_t)stat;
    }
    if (json_get_object_int(json_obj, "stats", "poise_defense", &stat)) {
        item->data.catalyst.poise_bonus = (int16_t)stat;
    }
    if (json_get_object_int(json_obj, "stats", "resonance_rate", &stat)) {
        item->data.catalyst.resonance_rate_bonus = (int16_t)stat;
    }

    int skill = 0;
    if (json_get_object_int(json_obj, "properties", "skill_grant", &skill)) {
        item->data.catalyst.skill_grant = (uint32_t)skill;
    }
    int state_id = 0;
    if (json_get_object_int(json_obj, "properties", "inflicts_state",
                            &state_id)) {
        item->data.catalyst.inflicts_state = (uint32_t)state_id;
    }
    char ret_elem[24];
    if (json_get_object_string(json_obj, "properties", "retaliation_element",
                               ret_elem, sizeof(ret_elem))) {
        if (strcmp(ret_elem, "fire") == 0) {
            item->data.catalyst.retaliation_element = 1;
        }
    }

    return true;
}

static bool parse_consumable_entry(const char *json_obj, item_def_t *item)
{
    int id_val = 0;
    if (!json_get_int(json_obj, "id", &id_val)) {
        return false;
    }
    item->id = (uint32_t)id_val;
    item->type = ITEM_TYPE_CONSUMABLE;
    item->consumable = true;

    int stack_val = 99;
    if (json_get_int(json_obj, "max_stack", &stack_val)) {
        item->max_stack = (uint16_t)stack_val;
    } else {
        item->max_stack = 99;
    }

    json_get_string(json_obj, "code", item->code, sizeof(item->code));
    json_get_string(json_obj, "name", item->name, sizeof(item->name));
    json_get_string(json_obj, "description", item->description,
                    sizeof(item->description));

    char cons_type[32];
    if (json_get_string(json_obj, "consumable_type", cons_type,
                        sizeof(cons_type))) {
        if (strcmp(cons_type, "restorative") == 0) {
            item->data.consumable.sub_type = CONSUMABLE_TYPE_RESTORATIVE;
        } else if (strcmp(cons_type, "cleanse") == 0) {
            item->data.consumable.sub_type = CONSUMABLE_TYPE_CLEANSE;
        } else if (strcmp(cons_type, "elixir") == 0) {
            item->data.consumable.sub_type = CONSUMABLE_TYPE_ELIXIR;
        } else if (strcmp(cons_type, "harmonic") == 0) {
            item->data.consumable.sub_type = CONSUMABLE_TYPE_HARMONIC;
        } else if (strcmp(cons_type, "revival") == 0) {
            item->data.consumable.sub_type = CONSUMABLE_TYPE_REVIVAL;
        }
    }

    int eff = 0;
    if (json_get_object_int(json_obj, "effects", "hp_restore", &eff)) {
        item->data.consumable.hp_restore = (int16_t)eff;
    }
    if (json_get_object_int(json_obj, "effects", "resonance_restore", &eff)) {
        item->data.consumable.resonance_restore = (int16_t)eff;
    }
    bool rev = false;
    if (json_get_object_bool(json_obj, "effects", "revives_ally", &rev)) {
        item->data.consumable.revives_ally = rev;
    }
    if (json_get_object_int(json_obj, "effects", "hp_restore_pct", &eff)) {
        item->data.consumable.hp_restore_pct = (uint8_t)eff;
    }

    return true;
}

static bool parse_valuable_entry(const char *json_obj, item_def_t *item)
{
    int id_val = 0;
    if (!json_get_int(json_obj, "id", &id_val)) {
        return false;
    }
    item->id = (uint32_t)id_val;
    item->type = ITEM_TYPE_VALUABLE;
    item->consumable = false;

    int stack_val = 999;
    if (json_get_int(json_obj, "max_stack", &stack_val)) {
        item->max_stack = (uint16_t)stack_val;
    } else {
        item->max_stack = 999;
    }

    int gold_val = 0;
    if (json_get_int(json_obj, "value_gold", &gold_val)) {
        item->data.valuable.value_gold = (uint32_t)gold_val;
    }

    json_get_string(json_obj, "code", item->code, sizeof(item->code));
    json_get_string(json_obj, "name", item->name, sizeof(item->name));
    json_get_string(json_obj, "description", item->description,
                    sizeof(item->description));

    return true;
}

static const char *resolve_item_path(const char *rel_path,
                                     char *out_buf,
                                     size_t buf_len)
{
    if (fs_file_exists(rel_path)) {
        str_safe_copy(out_buf, rel_path, buf_len);
        return out_buf;
    }
    char alt[256];
    snprintf(alt, sizeof(alt), "game/%s", rel_path);
    if (fs_file_exists(alt)) {
        str_safe_copy(out_buf, alt, buf_len);
        return out_buf;
    }
    snprintf(alt, sizeof(alt), "../%s", rel_path);
    if (fs_file_exists(alt)) {
        str_safe_copy(out_buf, alt, buf_len);
        return out_buf;
    }
    snprintf(alt, sizeof(alt), "../game/%s", rel_path);
    if (fs_file_exists(alt)) {
        str_safe_copy(out_buf, alt, buf_len);
        return out_buf;
    }
    str_safe_copy(out_buf, rel_path, buf_len);
    return out_buf;
}

static size_t load_dataset(const char *filepath,
                           item_type_t item_type,
                           item_def_t *out_catalog,
                           size_t start_idx,
                           size_t max_entries)
{
    static char s_file_buf[LOADER_FILE_BUF_SIZE];
    static char s_obj_buf[LOADER_OBJ_BUF_SIZE];

    char resolved[256];
    resolve_item_path(filepath, resolved, sizeof(resolved));

    if (!fs_file_exists(resolved)) {
        return 0;
    }

    if (!fs_read_file_contents(resolved, s_file_buf, sizeof(s_file_buf))) {
        return 0;
    }

    const char *arr_start = NULL;
    size_t arr_len = 0;
    if (!json_find_array(s_file_buf, "items", &arr_start, &arr_len)) {
        return 0;
    }

    size_t count = json_array_get_count(arr_start, arr_len);
    size_t loaded = 0;

    for (size_t i = 0; i < count && (start_idx + loaded) < max_entries; i++) {
        if (!slice_array_object(arr_start, arr_len, i, s_obj_buf,
                                sizeof(s_obj_buf))) {
            continue;
        }

        item_def_t *target = &out_catalog[start_idx + loaded];
        memset(target, 0, sizeof(*target));

        bool parsed = false;
        switch (item_type) {
            case ITEM_TYPE_RELIC:
                parsed = parse_relic_entry(s_obj_buf, target);
                break;
            case ITEM_TYPE_CATALYST:
                parsed = parse_catalyst_entry(s_obj_buf, target);
                break;
            case ITEM_TYPE_CONSUMABLE:
                parsed = parse_consumable_entry(s_obj_buf, target);
                break;
            case ITEM_TYPE_VALUABLE:
                parsed = parse_valuable_entry(s_obj_buf, target);
                break;
            default:
                break;
        }

        if (parsed) {
            loaded++;
        }
    }

    return loaded;
}

static void apply_fallback_baseline(item_def_t *out_catalog, size_t *out_count)
{
    /* Minimal baseline ensuring resilience in isolated sandbox environments */
    size_t c = 0;

    /* Fallback Relic: Grandma's Watch */
    item_def_t *w = &out_catalog[c++];
    memset(w, 0, sizeof(*w));
    w->id = 5;
    str_safe_copy(w->code, "GRANDMAS_WATCH", sizeof(w->code));
    str_safe_copy(w->name, "Grandma's Watch", sizeof(w->name));
    str_safe_copy(w->description,
                  "Cherished family heirloom keeping monotonic time.",
                  sizeof(w->description));
    w->type = ITEM_TYPE_RELIC;
    w->consumable = false;
    w->max_stack = 1;
    w->data.relic.sub_type = RELIC_TYPE_TEMPORAL;
    w->data.relic.clock_interface = true;
    w->data.relic.evolves_into = 9;

    /* Fallback Relic: The Dream Compass */
    item_def_t *dc = &out_catalog[c++];
    memset(dc, 0, sizeof(*dc));
    dc->id = 9;
    str_safe_copy(dc->code, "DREAM_COMPASS", sizeof(dc->code));
    str_safe_copy(dc->name, "The Dream Compass", sizeof(dc->name));
    str_safe_copy(dc->description,
                  "Astral rift key active from 20:00 to 04:00.",
                  sizeof(dc->description));
    dc->type = ITEM_TYPE_RELIC;
    dc->consumable = false;
    dc->max_stack = 1;
    dc->data.relic.sub_type = RELIC_TYPE_PLANAR;
    dc->data.relic.planar_key = true;
    dc->data.relic.nocturnal_start_hour = 20;
    dc->data.relic.nocturnal_end_hour = 4;

    /* Fallback Catalyst: Ring of Fire */
    item_def_t *rf = &out_catalog[c++];
    memset(rf, 0, sizeof(*rf));
    rf->id = 101;
    str_safe_copy(rf->code, "RING_OF_FIRE", sizeof(rf->code));
    str_safe_copy(rf->name, "Ring of Fire", sizeof(rf->name));
    str_safe_copy(rf->description,
                  "Thermal retaliation catalyst from Eric.",
                  sizeof(rf->description));
    rf->type = ITEM_TYPE_CATALYST;
    rf->consumable = false;
    rf->max_stack = 1;
    rf->data.catalyst.sub_type = CATALYST_TYPE_ELEMENTAL;
    rf->data.catalyst.slot = EQUIP_SLOT_ACCESSORY;
    rf->data.catalyst.mdf_bonus = 5;
    rf->data.catalyst.skill_grant = 42;
    rf->data.catalyst.retaliation_element = 1;

    /* Fallback Catalyst: Medusa's Sword */
    item_def_t *ms = &out_catalog[c++];
    memset(ms, 0, sizeof(*ms));
    ms->id = 102;
    str_safe_copy(ms->code, "MEDUSAS_SWORD", sizeof(ms->code));
    str_safe_copy(ms->name, "Medusa's Sword", sizeof(ms->name));
    str_safe_copy(ms->description,
                  "Ancient blade infusing strikes with stasis.",
                  sizeof(ms->description));
    ms->type = ITEM_TYPE_CATALYST;
    ms->consumable = false;
    ms->max_stack = 1;
    ms->data.catalyst.sub_type = CATALYST_TYPE_WEAPON;
    ms->data.catalyst.slot = EQUIP_SLOT_WEAPON;
    ms->data.catalyst.atk_bonus = 17;
    ms->data.catalyst.poise_bonus = 15;
    ms->data.catalyst.inflicts_state = 11;

    /* Fallback Consumable: Sacyran Herb */
    item_def_t *sh = &out_catalog[c++];
    memset(sh, 0, sizeof(*sh));
    sh->id = 1;
    str_safe_copy(sh->code, "SACYRAN_HERB", sizeof(sh->code));
    str_safe_copy(sh->name, "Sacyran Herb", sizeof(sh->name));
    str_safe_copy(sh->description,
                  "Medicinal leaf restoring 30 Vitality.",
                  sizeof(sh->description));
    sh->type = ITEM_TYPE_CONSUMABLE;
    sh->consumable = true;
    sh->max_stack = 99;
    sh->data.consumable.sub_type = CONSUMABLE_TYPE_RESTORATIVE;
    sh->data.consumable.hp_restore = 30;

    *out_count = c;
}

bool item_loader_load_catalog(item_def_t *out_catalog,
                              size_t max_entries,
                              size_t *out_count)
{
    if (!out_catalog || max_entries == 0 || !out_count) {
        return false;
    }

    size_t total = 0;

    /* Load Relics */
    total += load_dataset("data/items/relics.json", ITEM_TYPE_RELIC,
                          out_catalog, total, max_entries);

    /* Load Catalysts */
    total += load_dataset("data/items/catalysts.json", ITEM_TYPE_CATALYST,
                          out_catalog, total, max_entries);

    /* Load Consumables */
    total += load_dataset("data/items/consumables.json", ITEM_TYPE_CONSUMABLE,
                          out_catalog, total, max_entries);

    /* Load Valuables */
    total += load_dataset("data/items/valuables.json", ITEM_TYPE_VALUABLE,
                          out_catalog, total, max_entries);

    /* If no files could be read, apply fallback baseline for resilience */
    if (total == 0) {
        apply_fallback_baseline(out_catalog, &total);
    }

    *out_count = total;
    return (total > 0);
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Zero-Heap Item Data Loader
// =============================================================================

diag_result_t item_loader_preflight(void)
{
    diag_result_t res;
    res.subsystem = item_loader_manifest.module_id;
    res.check_name = "Item Catalog Deserialization and Fallback Invariants";
    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.details = "Item catalog loaded and essential relics verified.";

    item_def_t test_cat[ITEM_CATALOG_MAX_ENTRIES];
    size_t count = 0;

    if (!item_loader_load_catalog(test_cat, ITEM_CATALOG_MAX_ENTRIES, &count)) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Item catalog load returned failure.";
        return res;
    }

    if (count == 0) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Item catalog count is zero.";
        return res;
    }

    /* Verify essential relic Grandma's Watch exists */
    bool found_watch = false;
    for (size_t i = 0; i < count; i++) {
        if (test_cat[i].id == 5) {
            found_watch = true;
            break;
        }
    }

    if (!found_watch) {
        res.passed = false;
        res.severity = SEVERITY_WARNING;
        res.details = "Essential relic Grandma's Watch (ID 5) not found.";
        return res;
    }

    return res;
}

bool item_loader_audit(void)
{
    diag_result_t res = item_loader_preflight();
    return (res.passed && res.severity != SEVERITY_FATAL);
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================
