// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          test_functional.c
// @id            test.functional.cogs
// @layer         Layer 4: Verification Suite
// @subsystem     Functional Debugging Harness (tests/functional)
// @version       2.0.0
// @interface_rev Rev 2
// @date          2026-11-03
// @updated       2026-11-03
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Dedicated functional test harness isolating 36 granular cogs across
//   Layer 0 utilities and Layer 1 simulation modules (Core, Combat, Items,
//   World, Dispatch) with diagnostic telemetry traces across 4 sections:
//     1. Always Pass (Nominal Function Evaluation across 36 cogs)
//     2. Never Fail (Fault-Tolerant Function Hardening)
//     3. Assertion & Guarantee (Individual Function Post-Condition Axioms)
//     4. Emergence (Pipelined Dataflow Composition)
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: core.util.*, sim.engine.*
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Deterministic test execution).
//   - Concurrency / Safety:  CONCURRENCY_MAIN_THREAD (Sequential test runner).
//   - Latency Budget:        Entire suite executes in < 50 milliseconds.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// =============================================================================

#include "core/manifest.h"
#include "core/math_utils.h"
#include "core/geom_utils.h"
#include "io/fs_utils.h"
#include "io/json_utils.h"
#include "core/string_utils.h"
#include "core/time_utils.h"
#include "log/log.h"
#include "debug/debug.h"
#include "hero/sim_types.h"
#include "hero/sim_state.h"
#include "combat/combat.h"
#include "combat/tactical.h"
#include "combat/action_resolve.h"
#include "combat/stances.h"
#include "relics/items.h"
#include "relics/relics.h"
#include "relics/catalysts.h"
#include "relics/consumables.h"
#include "relics/inventory.h"
#include "world/world.h"
#include "world/map_scaler.h"
#include "world/world_topology.h"
#include "world/maps.h"
#include "dispatch/dispatcher.h"
#include "dispatch/dispatch_types.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEST_ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            fprintf(stderr, "[FAIL] %s:%d: Assertion failed: %s (%s)\n", \
                __FILE__, __LINE__, #cond, msg); \
            exit(1); \
        } \
    } while (0)

#define DEBUG_LOG_FN(fn_name, input_str, output_str) \
    printf("    [DEBUG-TRACE] %s(%s) -> %s\n", fn_name, input_str, output_str)

// =============================================================================
// BLOCK 3: BODY BLOCK
// =============================================================================

// -----------------------------------------------------------------------------
// [ SECTION 1: TESTS THAT SHOULD ALWAYS PASS (36 COGS EVALUATED) ]
// -----------------------------------------------------------------------------

static void test_functional_always_pass(void)
{
    printf("  [PASS-1] Running Section 1: Always Pass (36 Cogs Evaluated)...\n");

    /* Cog 1: math_clamp_int */
    int c1 = math_clamp_int(45, 0, 100);
    DEBUG_LOG_FN("math_clamp_int", "45, [0, 100]", "45");
    TEST_ASSERT(c1 == 45, "math_clamp_int nominal evaluation");

    /* Cog 2: math_saturating_add */
    int s_add = math_saturating_add(80, 30, 100);
    DEBUG_LOG_FN("math_saturating_add", "80 + 30, max=100", "100");
    TEST_ASSERT(s_add == 100, "math_saturating_add capped ceiling");

    /* Cog 3: math_saturating_sub */
    int s_sub = math_saturating_sub(40, 25);
    DEBUG_LOG_FN("math_saturating_sub", "40 - 25", "15");
    TEST_ASSERT(s_sub == 15, "math_saturating_sub nominal difference");

    /* Cog 4: math_nudge_toward */
    int nudge = math_nudge_toward(50, 0, 15);
    DEBUG_LOG_FN("math_nudge_toward", "50 toward 0 by 15", "35");
    TEST_ASSERT(nudge == 35, "math_nudge_toward step applied");

    /* Cog 5: math_scale_ratio */
    int scaled = math_scale_ratio(60, 100, 300);
    DEBUG_LOG_FN("math_scale_ratio", "60/100 of 300", "180");
    TEST_ASSERT(scaled == 180, "math_scale_ratio proportional mapping");

    /* Cog 6: geom_make_rect & geom_point_in_rect */
    geom_rect_t rect = geom_make_rect(50, 50, 100, 100);
    bool inside = geom_point_in_rect(75, 75, rect);
    DEBUG_LOG_FN("geom_point_in_rect", "Point(75,75)", inside ? "true" : "false");
    TEST_ASSERT(inside, "geom_point_in_rect interior hit");

    /* Cog 7: geom_rect_intersects */
    geom_rect_t rect2 = geom_make_rect(100, 100, 50, 50);
    bool intersects = geom_rect_intersects(rect, rect2);
    DEBUG_LOG_FN("geom_rect_intersects", "Rect overlap", intersects ? "true" : "false");
    TEST_ASSERT(intersects, "geom_rect_intersects overlap detected");

    /* Cog 8: str_safe_copy */
    char copy_dest[32];
    str_safe_copy(copy_dest, "Warding Stance", sizeof(copy_dest));
    DEBUG_LOG_FN("str_safe_copy", "\"Warding Stance\"", copy_dest);
    TEST_ASSERT(strcmp(copy_dest, "Warding Stance") == 0, "str_safe_copy match");

    /* Cog 9: str_trim_inplace */
    char trim_test[32];
    strcpy(trim_test, "   Duo-Loca   ");
    str_trim_inplace(trim_test);
    DEBUG_LOG_FN("str_trim_inplace", "\"   Duo-Loca   \"", trim_test);
    TEST_ASSERT(strcmp(trim_test, "Duo-Loca") == 0, "str_trim_inplace trimmed");

    /* Cog 10: str_case_eq */
    bool eq = str_case_eq("EQUILIBRIUM", "equilibrium");
    DEBUG_LOG_FN("str_case_eq", "EQUILIBRIUM eq", eq ? "true" : "false");
    TEST_ASSERT(eq, "str_case_eq case-insensitive match");

    /* Cog 11: str_parent_dir */
    char pdir[64];
    bool p_ok = str_parent_dir(pdir, sizeof(pdir), "data/combat/manifest.json");
    DEBUG_LOG_FN("str_parent_dir", "\"data/combat/manifest.json\"", pdir);
    TEST_ASSERT(p_ok && strcmp(pdir, "data/combat") == 0, "str_parent_dir match");

    /* Cog 12: str_path_join */
    char joined_p[64];
    bool j_ok = str_path_join(joined_p, sizeof(joined_p), "data/combat", "default.json");
    DEBUG_LOG_FN("str_path_join", "join path", joined_p);
    TEST_ASSERT(j_ok && strcmp(joined_p, "data/combat/default.json") == 0, "joined");

    /* Cog 13: json_get_string */
    const char *mock_json = "{\"character\": \"Casatti\", \"level\": 15}";
    char char_name[32];
    bool g_str = json_get_string(mock_json, "character", char_name, sizeof(char_name));
    DEBUG_LOG_FN("json_get_string", "key=\"character\"", char_name);
    TEST_ASSERT(g_str && strcmp(char_name, "Casatti") == 0, "extracted 'Casatti'");

    /* Cog 14: json_get_int */
    int char_lvl = 0;
    bool g_int = json_get_int(mock_json, "level", &char_lvl);
    DEBUG_LOG_FN("json_get_int", "key=\"level\"", "15");
    TEST_ASSERT(g_int && char_lvl == 15, "extracted 15");

    /* Cog 15: time_get_monotonic_ms */
    double t_now = time_get_monotonic_ms();
    DEBUG_LOG_FN("time_get_monotonic_ms", "void", "valid");
    TEST_ASSERT(t_now > 0.0, "time_get_monotonic_ms positive");

    /* Cog 16: combat_get_stance_name */
    const char *st_name = combat_get_stance_name(STANCE_WARDING);
    DEBUG_LOG_FN("combat_get_stance_name", "STANCE_WARDING", st_name);
    TEST_ASSERT(strstr(st_name, "Warding") != NULL, "stance name has Warding");

    /* Cog 17: combat_handle_attack */
    player_state_t sim_st;
    sim_state_init(&sim_st);
    char combat_msg[128];
    combat_handle_attack(&sim_st, false, combat_msg, sizeof(combat_msg));
    DEBUG_LOG_FN("combat_handle_attack", "attack standard", combat_msg);
    TEST_ASSERT(sim_st.threat_hp < sim_st.threat_max_hp, "reduced threat hp");

    /* Cog 18: debug_compute_state_hash */
    debug_state_fields_t dbg_fields;
    memset(&dbg_fields, 0, sizeof(dbg_fields));
    dbg_fields.turn = 1;
    dbg_fields.hp = 100;
    uint32_t state_hash = debug_compute_state_hash(&dbg_fields);
    DEBUG_LOG_FN("debug_compute_state_hash", "hp=100", "hash valid");
    TEST_ASSERT(state_hash != 0, "valid non-zero hash");

    /* Cog 19: debug_add_breakpoint */
    debug_init(NULL);
    debug_clear_breakpoints();
    int bp_id = debug_add_breakpoint(DEBUG_BREAK_TURN, 10, "Turn 10");
    DEBUG_LOG_FN("debug_add_breakpoint", "type=TURN, target=10", "registered");
    TEST_ASSERT(bp_id > 0, "registered breakpoint");
    debug_clear_breakpoints();

    /* Cog 20: debug_diff_snapshots */
    debug_snapshot_t s1, s2;
    memset(&s1, 0, sizeof(s1));
    memset(&s2, 0, sizeof(s2));
    s1.state.hp = 100;
    s2.state.hp = 85;
    char diff_str[256];
    size_t dlen = debug_diff_snapshots(&s1, &s2, diff_str, sizeof(diff_str));
    DEBUG_LOG_FN("debug_diff_snapshots", "hp 100->85", diff_str);
    TEST_ASSERT(dlen > 0, "diff evaluated");
    debug_shutdown();

    /* Cog 21: tactical_grid_distance */
    grid_pos_t pos_a = {1, 2};
    grid_pos_t pos_b = {4, 6};
    int16_t m_dist = tactical_grid_distance(pos_a, pos_b);
    DEBUG_LOG_FN("tactical_grid_distance", "(1,2) to (4,6)", "7");
    TEST_ASSERT(m_dist == 7, "manhattan distance is 7");

    /* Cog 22: action_resolve_parry */
    bool is_perf = false;
    bool p_act = action_resolve_parry(50, 200, &is_perf);
    DEBUG_LOG_FN("action_resolve_parry", "50ms within 200ms",
                 p_act ? "true" : "false");
    TEST_ASSERT(p_act && is_perf, "parry active and perfect in frame window");

    /* Cog 23: stances_can_transition */
    bool st_v = stances_can_transition(STANCE_NEUTRAL, STANCE_WARDING);
    DEBUG_LOG_FN("stances_can_transition", "Neutral->Warding",
                 st_v ? "true" : "false");
    TEST_ASSERT(st_v, "stance transition valid");

    /* Cog 24: relics_inspect_clock */
    const item_def_t *r_watch = items_find_def(5);
    char clock_str[64] = {0};
    bool clock_ok = relics_inspect_clock(r_watch, clock_str, sizeof(clock_str));
    DEBUG_LOG_FN("relics_inspect_clock", "Grandma's Watch (5)", clock_str);
    TEST_ASSERT(clock_ok, "inspected Grandma's Watch clock");

    /* Cog 25: catalysts_apply_stats */
    const item_def_t *cat_medusa = items_find_def(102);
    int16_t cat_atk = 10, cat_def = 10, cat_mat = 10, cat_mdf = 10;
    bool cat_ok = catalysts_apply_stats(cat_medusa, &cat_atk, &cat_def,
                                        &cat_mat, &cat_mdf);
    DEBUG_LOG_FN("catalysts_apply_stats", "Medusa Sword (102)", "applied");
    TEST_ASSERT(cat_ok && cat_atk > 10, "applied Medusa's Sword stat mod");

    /* Cog 26: consumables_apply_effect */
    const item_def_t *c_herb = items_find_def(1);
    int32_t t_hp = 100, t_lux = 50, t_ten = 50;
    char eff_msg[128];
    bool eff_ok = consumables_apply_effect(c_herb, &t_hp, 450, &t_lux, &t_ten,
                                           eff_msg, sizeof(eff_msg));
    DEBUG_LOG_FN("consumables_apply_effect", "Sacyran Herb (1)", eff_msg);
    TEST_ASSERT(eff_ok && t_hp > 100, "restored HP with consumable");

    /* Cog 27: inventory_add & inventory_get_count */
    player_inventory_t f_inv;
    inventory_init(&f_inv);
    inventory_add(&f_inv, NULL, 0, 1, 4);
    uint16_t cnt = inventory_get_count(&f_inv, 1);
    DEBUG_LOG_FN("inventory_add", "id=1, count=4", "4");
    TEST_ASSERT(cnt == 4, "inventory count == 4");

    /* Cog 28: map_scaler_init_viewport */
    map_viewport_t f_vp;
    map_scaler_init_viewport(&f_vp, 1280, 720, 640, 360);
    DEBUG_LOG_FN("map_scaler_init_viewport", "1280x720", "scale=2.0");
    TEST_ASSERT(f_vp.scale_factor > 1.99f && f_vp.scale_factor < 2.01f,
                "scale factor == 2.0");

    /* Cog 29: map_scaler_world_to_screen */
    map_camera_t f_cam = {50, 50, 50, 50};
    int32_t f_sx = 0, f_sy = 0;
    map_scaler_world_to_screen(70, 80, &f_cam, &f_vp, &f_sx, &f_sy);
    DEBUG_LOG_FN("map_scaler_world_to_screen", "(70,80)", "(40,60)");
    TEST_ASSERT(f_sx == 40 && f_sy == 60, "projected (70,80) to (40,60)");

    /* Cog 30: map_scaler_screen_to_world */
    int32_t f_wx = 0, f_wy = 0;
    map_scaler_screen_to_world(40, 60, &f_cam, &f_vp, &f_wx, &f_wy);
    DEBUG_LOG_FN("map_scaler_screen_to_world", "(40,60)", "(70,80)");
    TEST_ASSERT(f_wx == 70 && f_wy == 80, "inverted to (70,80)");

    /* Cog 31: map_scaler_clamp_camera */
    map_dimensions_t f_dim = {20, 20, 32};
    f_cam.cam_x = 900;
    f_cam.cam_y = 900;
    map_scaler_clamp_camera(&f_cam, &f_dim, &f_vp);
    DEBUG_LOG_FN("map_scaler_clamp_camera", "(900,900)", "clamped");
    TEST_ASSERT(f_cam.cam_x == 0, "cam_x clamped safely");

    /* Cog 32: world_topology_resolve_direction */
    location_id_t f_loc = world_topology_resolve_direction(LOC_DIO_VILLAGE,
                                                          "north");
    DEBUG_LOG_FN("world_topology_resolve_direction", "Dio Village+North",
                 "Whispering Woods");
    TEST_ASSERT(f_loc == LOC_WHISPERING_WOODS, "resolved Whispering Woods");

    /* Cog 33: maps_get_by_id */
    const map_def_t *f_map = maps_get_by_id("dio_village_exterior");
    DEBUG_LOG_FN("maps_get_by_id", "\"dio_village_exterior\"",
                 f_map ? f_map->canonical_name : "NULL");
    TEST_ASSERT(f_map != NULL, "found dio_village_exterior");

    /* Cog 34: dispatcher_resolve_alias */
    char f_can[32] = {0};
    char f_arg[32] = {0};
    bool f_al = dispatcher_resolve_alias("strike", f_can, sizeof(f_can),
                                         f_arg, sizeof(f_arg));
    DEBUG_LOG_FN("dispatcher_resolve_alias", "\"strike\"", f_can);
    TEST_ASSERT(f_al && strcmp(f_can, "attack") == 0, "strike -> attack");

    /* Cog 35: dispatcher_classify_verb */
    dispatch_domain_t f_dom = dispatcher_classify_verb("attack");
    DEBUG_LOG_FN("dispatcher_classify_verb", "\"attack\"", "DOMAIN_COMBAT");
    TEST_ASSERT(f_dom == DOMAIN_COMBAT, "classified as DOMAIN_COMBAT");

    /* Cog 36: items_is_nocturnal_window */
    bool f_noc = items_is_nocturnal_window(&sim_st);
    (void)f_noc;
    DEBUG_LOG_FN("items_is_nocturnal_window", "clock query",
                 f_noc ? "nocturnal" : "diurnal");

    printf("  [PASS-1] Section 1 passed unconditionally: 36 individual cogs evaluated.\n");
}

// -----------------------------------------------------------------------------
// [ SECTION 2: TESTS THAT SHOULD NEVER FAIL (FUNCTION HARDENING) ]
// -----------------------------------------------------------------------------

static void test_functional_never_fail(void)
{
    printf("  [FAIL-2] Running Section 2: Never Fail (Negative Function Hardening)...\n");

    /* 2.1 math_clamp_int with inverted bounds */
    int inv = math_clamp_int(50, 80, 20);
    (void)inv;
    DEBUG_LOG_FN("math_clamp_int", "val=50, min=80, max=20", "absorbed safely");

    /* 2.2 math_scale_ratio with zero max */
    int zero_scale = math_scale_ratio(50, 0, 100);
    DEBUG_LOG_FN("math_scale_ratio", "val=50, max=0", "0");
    TEST_ASSERT(zero_scale == 0, "Zero-divisor safely yielded 0");

    /* 2.3 geom_point_in_rect on zero-area rect */
    geom_rect_t zero_rect = geom_make_rect(10, 10, 0, 0);
    bool in_zero = geom_point_in_rect(15, 10, zero_rect);
    DEBUG_LOG_FN("geom_point_in_rect", "Point(15,10) in zero-area Rect",
                 in_zero ? "true" : "false");
    TEST_ASSERT(!in_zero, "Zero-area rect contains no offset points");

    /* 2.4 str_safe_copy with NULL destination or source */
    str_safe_copy(NULL, "Source", 10);
    DEBUG_LOG_FN("str_safe_copy", "NULL dst", "no-op safely");

    char dest_b[16];
    dest_b[0] = 'Z';
    str_safe_copy(dest_b, NULL, sizeof(dest_b));
    DEBUG_LOG_FN("str_safe_copy", "NULL src", "dest[0] = '\\0'");
    TEST_ASSERT(dest_b[0] == '\0', "NULL src cleared destination");

    /* 2.5 str_trim_inplace with NULL */
    str_trim_inplace(NULL);
    DEBUG_LOG_FN("str_trim_inplace", "NULL str", "no-op safely");

    /* 2.6 json_get_string with NULL arguments */
    char out_buf[16];
    bool g_null1 = json_get_string(NULL, "key", out_buf, sizeof(out_buf));
    bool g_null2 = json_get_string("{}", NULL, out_buf, sizeof(out_buf));
    DEBUG_LOG_FN("json_get_string", "NULL json / NULL key", "false");
    TEST_ASSERT(!g_null1 && !g_null2, "NULL queries safely return false");

    /* 2.7 fs_is_regular_file on NULL */
    bool is_reg = fs_is_regular_file(NULL);
    DEBUG_LOG_FN("fs_is_regular_file", "NULL path", is_reg ? "true" : "false");
    TEST_ASSERT(!is_reg, "NULL path regular query returns false");

    /* 2.8 Dispatcher unknown verb safety */
    player_state_t st;
    sim_state_init(&st);
    char out_msg[128];
    dispatcher_process_command(&st, "unknown_action_xyz", out_msg, sizeof(out_msg));
    DEBUG_LOG_FN("dispatcher_process_command", "unknown verb", out_msg);
    TEST_ASSERT(st.turns_taken == 0, "Unknown command did not advance turns");

    printf("  [FAIL-2] Section 2 passed: All negative function inputs absorbed safely.\n");
}

// -----------------------------------------------------------------------------
// [ SECTION 3: TESTS THAT ARE AN ASSERTION AND GUARANTEE (POST-CONDITIONS) ]
// -----------------------------------------------------------------------------

static void test_functional_assertions_and_guarantees(void)
{
    printf("  [AXIOM-3] Running Section 3: Assertion & Guarantee (Post-Condition Axioms)...\n");

    /* 3.1 Clamping contract: result is always within [min, max] */
    for (int i = -100; i <= 200; i += 13) {
        int r = math_clamp_int(i, -20, 50);
        TEST_ASSERT(r >= -20 && r <= 50, "math_clamp_int post-condition");
    }

    /* 3.2 Saturating sub contract: result is always non-negative */
    for (int a = 0; a <= 100; a += 10) {
        for (int b = 0; b <= 120; b += 15) {
            int sub_res = math_saturating_sub(a, b);
            TEST_ASSERT(sub_res >= 0, "math_saturating_sub non-negative");
        }
    }

    /* 3.3 Safe copy truncation contract: always null-terminated */
    char trunc_buf[6];
    str_safe_copy(trunc_buf, "ABCDEFGHIJKLMN", sizeof(trunc_buf));
    TEST_ASSERT(trunc_buf[5] == '\0', "str_safe_copy termination byte");
    TEST_ASSERT(strcmp(trunc_buf, "ABCDE") == 0, "length 5 string retained");

    /* 3.4 Rectangle constructor contract: non-negative dimensions */
    geom_rect_t r_neg = geom_make_rect(-10, -20, -30, -40);
    TEST_ASSERT(r_neg.w >= 0 && r_neg.h >= 0, "geom_make_rect non-negative");

    /* 3.5 Monotonic clock guarantee */
    double t1 = time_get_monotonic_ms();
    double t2 = time_get_monotonic_ms();
    TEST_ASSERT(t2 >= t1, "time_get_monotonic_ms non-decreasing");

    /* 3.6 Map scaler invertibility guarantee */
    map_viewport_t vp;
    map_scaler_init_viewport(&vp, 1920, 1080, 640, 360);
    map_camera_t cam = {200, 100, 200, 100};
    int32_t sx = 0, sy = 0, wx = 0, wy = 0;
    map_scaler_world_to_screen(350, 220, &cam, &vp, &sx, &sy);
    map_scaler_screen_to_world(sx, sy, &cam, &vp, &wx, &wy);
    TEST_ASSERT(wx == 350 && wy == 220, "scaler exact invertibility axiom");

    printf("  [AXIOM-3] Section 3 passed: All post-condition contracts verified.\n");
}

// -----------------------------------------------------------------------------
// [ SECTION 4: TESTS THAT ACCOUNT FOR EMERGENCE (FUNCTION PIPELINE CHAINING) ]
// -----------------------------------------------------------------------------

static void test_functional_emergence(void)
{
    printf("  [EMERGE-4] Running Section 4: Emergence (Pipelined Function Chaining)...\n");

    /* 4.1 Dataflow Pipeline: Clean -> Parse -> Clamp -> Scale -> Screen Rect */
    const char *raw_input = "   125   ";
    char step1[32];
    str_safe_copy(step1, raw_input, sizeof(step1));
    str_trim_inplace(step1);
    TEST_ASSERT(strcmp(step1, "125") == 0, "Pipeline Step 1: Cleaned raw input");

    char step2_json[128];
    snprintf(step2_json, sizeof(step2_json), "{\"resonance_target\": %s}", step1);

    int target_val = 0;
    bool parsed = json_get_int(step2_json, "resonance_target", &target_val);
    TEST_ASSERT(parsed && target_val == 125, "Pipeline Step 2: Deserialized raw target");

    int clamped_res = math_clamp_int(target_val, 0, 100);
    TEST_ASSERT(clamped_res == 100, "Pipeline Step 3: Clamped to 100");

    int bar_width = math_scale_ratio(clamped_res, 100, 250);
    TEST_ASSERT(bar_width == 250, "Pipeline Step 4: Scaled to 250 px");

    geom_rect_t bar_rect = geom_make_rect(20, 40, bar_width, 16);
    TEST_ASSERT(bar_rect.w == 250 && bar_rect.h == 16, "Pipeline Step 5: Screen-space rect");

    bool hit = geom_point_in_rect(100, 45, bar_rect);
    TEST_ASSERT(hit, "Pipeline Step 6: Hit test succeeds");

    /* 4.2 Dynamic Map & Viewport Pipeline:
     * Screen Size (1920x1080) -> Isotropic Scale -> World Grid ->
     * Camera Tracking -> Screen Project -> Mouse Click Invert -> Cell Test
     */
    map_viewport_t vp_1080;
    map_scaler_init_viewport(&vp_1080, 1920, 1080, 640, 360);
    TEST_ASSERT(vp_1080.scale_factor > 2.99f && vp_1080.scale_factor < 3.01f,
                "Pipeline 2 Step 1: 1080p isotropic scale factor 3.0");

    const map_def_t *dio_map = maps_get_by_id("dio_village_exterior");
    TEST_ASSERT(dio_map != NULL, "Pipeline 2 Step 2: Loaded dio village map");

    map_camera_t dyn_cam;
    dyn_cam.target_x = dio_map->spawn_x * (int32_t)dio_map->dimensions.cell_size_px;
    dyn_cam.target_y = dio_map->spawn_y * (int32_t)dio_map->dimensions.cell_size_px;
    dyn_cam.cam_x = dyn_cam.target_x - (int32_t)(vp_1080.target_view_w / 2);
    dyn_cam.cam_y = dyn_cam.target_y - (int32_t)(vp_1080.target_view_h / 2);
    map_scaler_clamp_camera(&dyn_cam, &dio_map->dimensions, &vp_1080);
    TEST_ASSERT(dyn_cam.cam_x >= 0 && dyn_cam.cam_y >= 0,
                "Pipeline 2 Step 3: Camera clamped within positive map domain");

    int32_t screen_click_x = 0, screen_click_y = 0;
    map_scaler_world_to_screen(dyn_cam.target_x, dyn_cam.target_y,
                               &dyn_cam, &vp_1080,
                               &screen_click_x, &screen_click_y);
    int32_t world_click_x = 0, world_click_y = 0;
    map_scaler_screen_to_world(screen_click_x, screen_click_y,
                               &dyn_cam, &vp_1080,
                               &world_click_x, &world_click_y);
    TEST_ASSERT(world_click_x == dyn_cam.target_x &&
                world_click_y == dyn_cam.target_y,
                "Pipeline 2 Step 4: Click projection roundtrip verified");

    printf("  [EMERGE-4] Section 4 passed: Multi-stage pipelines completed cleanly.\n");
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// =============================================================================

static void test_functional_preflight(void)
{
    printf("[AUDIT] Running Functional Subsystem Pre-Flight Verification...\n");
    TEST_ASSERT(math_utils_audit(), "math_utils audit passed");
    TEST_ASSERT(geom_utils_audit(), "geom_utils audit passed");
    TEST_ASSERT(string_utils_audit(), "string_utils audit passed");
    TEST_ASSERT(time_utils_audit(), "time_utils audit passed");
    TEST_ASSERT(debug_audit(false).passed, "debug audit passed");
    TEST_ASSERT(combat_preflight().passed, "combat preflight passed");
    TEST_ASSERT(items_preflight().passed, "items preflight passed");
    TEST_ASSERT(world_preflight().passed, "world preflight passed");
    TEST_ASSERT(dispatcher_preflight().passed, "dispatcher preflight passed");
    printf("[AUDIT] Functional cog audits passed.\n");
}

// =============================================================================
// BLOCK 5: CLOSING / TEST & CLI BLOCK
// =============================================================================

int main(void)
{
    printf("============================================================\n");
    printf("     Tenebris and Lux - Functional Debugging Test Harness   \n");
    printf("============================================================\n");

    test_functional_preflight();
    test_functional_always_pass();
    test_functional_never_fail();
    test_functional_assertions_and_guarantees();
    test_functional_emergence();

    printf("============================================================\n");
    printf("ALL FUNCTIONAL DEBUGGING TESTS PASSED (4/4 Sections Verified)\n");
    printf("============================================================\n");
    return 0;
}
