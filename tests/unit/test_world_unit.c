// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          test_world_unit.c
// @id            test.unit.world
// @layer         Layer 4: Verification Suite
// @subsystem     Simulation World Unit Verification (tests/unit)
// @version       1.0.0
// @interface_rev Rev 1
// @date          2026-11-03
// @updated       2026-11-03
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Comprehensive unit verification for the simulation world stratum:
//   provincial topology, dynamic map viewport scaling, camera boundary
//   clamping, canonical map catalog queries, and world JSON loader parsing.
//   Organized across the 4 Ontological Testing Sections.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: sim/world/world.h, sim/world/map_scaler.h,
//                            sim/world/world_topology.h, sim/world/maps.h,
//                            sim/world/world_loader.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic allocation).
//   - Concurrency / Safety:  CONCURRENCY_MAIN_THREAD (Sequential test runner).
//   - Latency Budget:        < 20 milliseconds for entire suite.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// =============================================================================

#include "sim/core/sim_state.h"
#include "sim/world/map_scaler.h"
#include "sim/world/map_types.h"
#include "sim/world/maps.h"
#include "sim/world/world.h"
#include "sim/world/world_loader.h"
#include "sim/world/world_topology.h"
#include <assert.h>
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

// =============================================================================
// BLOCK 3: BODY BLOCK / 4-SECTION TEST SUITES
// =============================================================================

static void test_world_always_pass(void)
{
    printf("  [PASS-1] Section 1: Always Pass (Nominal Baseline)...\n");

    /* 1.1 Province name queries */
    TEST_ASSERT(strcmp(world_topology_get_name(LOC_DIO_VILLAGE),
                       "Dio Village") == 0, "Province name Dio Village");
    TEST_ASSERT(strcmp(world_topology_get_name(LOC_THE_DREAMSCAPE),
                       "The Dreamscape") == 0, "Province name The Dreamscape");

    /* 1.2 Threat tiers */
    TEST_ASSERT(world_topology_get_threat(LOC_DIO_VILLAGE) == 0,
                "Dio Village threat tier == 0");
    TEST_ASSERT(world_topology_get_threat(LOC_THE_DREAMSCAPE) == 4,
                "The Dreamscape threat tier == 4");

    /* 1.3 Map catalog count */
    TEST_ASSERT(maps_get_count() >= 9, "Canonical map count >= 9");
    const map_def_t *dio_map = maps_get_by_id("dio_village_exterior");
    TEST_ASSERT(dio_map != NULL, "Found dio_village_exterior map");

    printf("  [PASS-1] Section 1 passed cleanly.\n");
}

static void test_world_never_fail(void)
{
    printf("  [FAIL-2] Section 2: Never Fail (Fault-Tolerant Hardening)...\n");

    /* 2.1 Invalid location enum */
    const char *name = world_topology_get_name((location_id_t)999);
    TEST_ASSERT(strcmp(name, "Unknown Province") == 0,
                "Out-of-bounds location returns fallback name");

    /* 2.2 Direction resolution on invalid inputs */
    location_id_t r1 = world_topology_resolve_direction(LOC_DIO_VILLAGE, NULL);
    TEST_ASSERT(r1 == LOC_COUNT, "NULL direction returns LOC_COUNT");

    location_id_t r2 = world_topology_resolve_direction(LOC_DIO_VILLAGE,
                                                       "invalid_heading");
    TEST_ASSERT(r2 == LOC_COUNT, "Invalid heading returns LOC_COUNT");

    /* 2.3 NULL map pointer in portal lookup */
    const map_portal_t *portal = maps_find_portal_at(NULL, 10, 10);
    TEST_ASSERT(portal == NULL, "NULL map pointer in portal handled safely");

    /* 2.4 Scaler NULL pointer safety */
    int32_t out_x = 0, out_y = 0;
    bool s_ok = map_scaler_world_to_screen(10, 10, NULL, NULL, &out_x, &out_y);
    TEST_ASSERT(!s_ok, "NULL camera/viewport in scaler handled safely");

    printf("  [FAIL-2] Section 2 passed cleanly.\n");
}

static void test_world_assertions_and_guarantees(void)
{
    printf("  [AXIOM-3] Section 3: Assertion & Guarantee (Invariants)...\n");

    /* 3.1 Dynamic map isotropic scaling invariant */
    map_viewport_t vp;
    map_scaler_init_viewport(&vp, 1920, 1080, 640, 360);
    TEST_ASSERT(vp.scale_factor > 2.99f && vp.scale_factor < 3.01f,
                "1080p scale factor is exactly 3.0");
    TEST_ASSERT(vp.offset_x == 0 && vp.offset_y == 0,
                "16:9 on 16:9 produces 0 letterbox/pillarbox");

    /* Pillarbox test: 800x600 screen (4:3) with 640x360 target (16:9) */
    map_viewport_t vp_43;
    map_scaler_init_viewport(&vp_43, 800, 600, 640, 360);
    TEST_ASSERT(vp_43.scale_factor > 1.24f && vp_43.scale_factor < 1.26f,
                "4:3 display selects minimum scale factor 1.25");
    TEST_ASSERT(vp_43.offset_y > 0, "Vertical letterbox offset is positive");

    /* 3.2 Coordinate inversion roundtrip */
    map_camera_t cam = {150, 80, 150, 80};
    int32_t scr_x = 0, scr_y = 0;
    map_scaler_world_to_screen(320, 200, &cam, &vp, &scr_x, &scr_y);

    int32_t inv_x = 0, inv_y = 0;
    map_scaler_screen_to_world(scr_x, scr_y, &cam, &vp, &inv_x, &inv_y);
    TEST_ASSERT(inv_x == 320 && inv_y == 200,
                "Screen-to-world exactly inverts world-to-screen");

    /* 3.3 Camera clamping bounds invariant */
    map_dimensions_t dim = {32, 32, 32}; // 1024x1024 px world
    cam.cam_x = 1500; // intentionally beyond right edge
    cam.cam_y = 1500; // intentionally beyond bottom edge
    map_scaler_clamp_camera(&cam, &dim, &vp);
    TEST_ASSERT(cam.cam_x <= (1024 - 640), "Camera X clamped within world max");
    TEST_ASSERT(cam.cam_y <= (1024 - 360), "Camera Y clamped within world max");
    TEST_ASSERT(cam.cam_x >= 0 && cam.cam_y >= 0, "Camera non-negative");

    printf("  [AXIOM-3] Section 3 passed cleanly.\n");
}

static void test_world_emergence(void)
{
    printf("  [EMERGE-4] Section 4: Emergence (Camera & Traversal)...\n");

    player_state_t state;
    sim_state_init(&state);

    /* Switch to Dio Village exterior */
    bool ok = world_set_active_map_by_id("dio_village_exterior");
    TEST_ASSERT(ok, "Set active map to dio_village_exterior");

    const map_camera_t *cam = world_get_camera();
    TEST_ASSERT(cam != NULL, "Camera available");

    /* Move protagonist on grid */
    char msg[128];
    bool moved = world_move_protagonist(&state, 2, 2, msg, sizeof(msg));
    TEST_ASSERT(moved, "Protagonist moved 2 cells on grid");

    /* Verify camera tracking */
    const map_def_t *active = world_get_active_map();
    int32_t step_px = (int32_t)active->dimensions.cell_size_px;
    TEST_ASSERT(cam->target_x == (active->spawn_x + 2) * step_px,
                "Camera target tracks protagonist horizontal position");

    printf("  [EMERGE-4] Section 4 passed cleanly.\n");
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// =============================================================================

static void test_world_preflight(void)
{
    printf("[AUDIT] Running World Subsystem Pre-Flight Verification...\n");
    diag_result_t res = world_preflight();
    TEST_ASSERT(res.passed, "World preflight passed");
    printf("[AUDIT] World preflight passed: %s\n", res.details);
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK / TEST RUNNER
// =============================================================================

int main(void)
{
    printf("============================================================\n");
    printf("       Tenebris and Lux - World Unit Verification Suite     \n");
    printf("============================================================\n");

    test_world_preflight();
    test_world_always_pass();
    test_world_never_fail();
    test_world_assertions_and_guarantees();
    test_world_emergence();

    printf("============================================================\n");
    printf("ALL WORLD UNIT TESTS PASSED (4/4 Sections Verified)\n");
    printf("============================================================\n");
    return 0;
}
