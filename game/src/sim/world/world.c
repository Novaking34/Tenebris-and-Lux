// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          world.c
// @id            sim.engine.world
// @layer         Layer 1: Simulation Engine
// @subsystem     World Navigation & Dynamic Map Strata (sim/world)
// @version       2.0.0
// @interface_rev Rev 5
// @date          2026-11-03
// @updated       2026-11-03
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Master World Orchestrator coordinating provincial topology, dynamic map
//   viewport scaling, multi-tier maps (Overworld, Tactical Grids, Town
//   Exteriors, Interior Chambers), and directional movement routing.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: sim/world/world.h, util/manifest.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic allocation).
//   - Concurrency / Safety:  REENTRANT_PURE (Pure reentrant state evaluation).
//   - Latency Budget:        < 15 microseconds per spatial operation.
//
// AXIOMATIC INVARIANTS:
//   1. Location bounds strictly bounded: 0 <= location < LOC_COUNT.
//   2. Dynamic viewport preserves aspect ratio via isotropic scaling.
//   3. Camera clamp enforces bounds within active map dimensions.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// =============================================================================

#include "sim/world/world.h"
#include "util/log/log.h"
#include "util/string/string_utils.h"
#include <stdio.h>
#include <string.h>

const module_metadata_t world_manifest = {
    "sim.engine.world",
    "world.h",
    LAYER_1_SIMULATION,
    { 2, 0, 0 },
    5,
    "2026-11-03",
    "World orchestrator, dynamic map scaler, topology, and map catalogs",
    3,
    { "core.util.math", "core.util.string", "core.util.log", NULL },
    ALLOC_ZERO_HEAP,
    CONCURRENCY_REENTRANT,
    25,
    4,
    {
        "Location bounds strictly bounded: 0 <= location < LOC_COUNT",
        "Dynamic viewport preserves aspect ratio via isotropic scaling",
        "Camera clamp enforces bounds within active map dimensions",
        "Provincial navigation graph is deterministic and bi-directional"
    },
    world_preflight
};

static const map_def_t *s_active_map = NULL;
static map_camera_t     s_camera = {0, 0, 0, 0};
static map_viewport_t   s_viewport = {640, 360, 640, 360, 1.0f, 0, 0};
static bool             s_initialized = false;

static void ensure_world_initialized(void)
{
    if (!s_initialized) {
        s_active_map = maps_get_by_id("dio_village_exterior");
        map_scaler_init_viewport(&s_viewport, 640, 360, 640, 360);
        if (s_active_map) {
            s_camera.target_x = s_active_map->spawn_x *
                                (int32_t)s_active_map->dimensions.cell_size_px;
            s_camera.target_y = s_active_map->spawn_y *
                                (int32_t)s_active_map->dimensions.cell_size_px;
            s_camera.cam_x = s_camera.target_x -
                             (int32_t)(s_viewport.target_view_w / 2);
            s_camera.cam_y = s_camera.target_y -
                             (int32_t)(s_viewport.target_view_h / 2);
            map_scaler_clamp_camera(&s_camera, &s_active_map->dimensions,
                                    &s_viewport);
        }
        s_initialized = true;
    }
}

// =============================================================================
// BLOCK 3: BODY BLOCK / API IMPLEMENTATIONS
// =============================================================================

const char *world_get_location_name(location_id_t loc)
{
    return world_topology_get_name(loc);
}

const char *world_get_location_desc(location_id_t loc)
{
    return world_topology_get_desc(loc);
}

int world_get_threat_level(location_id_t loc)
{
    return world_topology_get_threat(loc);
}

const map_def_t *world_get_active_map(void)
{
    ensure_world_initialized();
    return s_active_map;
}

bool world_set_active_map_by_id(const char *map_id)
{
    ensure_world_initialized();
    const map_def_t *found = maps_get_by_id(map_id);
    if (!found) {
        return false;
    }
    s_active_map = found;
    s_camera.target_x = s_active_map->spawn_x *
                        (int32_t)s_active_map->dimensions.cell_size_px;
    s_camera.target_y = s_active_map->spawn_y *
                        (int32_t)s_active_map->dimensions.cell_size_px;
    s_camera.cam_x = s_camera.target_x -
                     (int32_t)(s_viewport.target_view_w / 2);
    s_camera.cam_y = s_camera.target_y -
                     (int32_t)(s_viewport.target_view_h / 2);
    map_scaler_clamp_camera(&s_camera, &s_active_map->dimensions, &s_viewport);
    return true;
}

const map_camera_t *world_get_camera(void)
{
    ensure_world_initialized();
    return &s_camera;
}

const map_viewport_t *world_get_viewport(void)
{
    ensure_world_initialized();
    return &s_viewport;
}

void world_update_viewport(uint32_t screen_w, uint32_t screen_h)
{
    ensure_world_initialized();
    map_scaler_init_viewport(&s_viewport, screen_w, screen_h, 640, 360);
    if (s_active_map) {
        map_scaler_clamp_camera(&s_camera, &s_active_map->dimensions,
                                &s_viewport);
    }
}

bool world_move_protagonist(player_state_t *state,
                            int32_t delta_x, int32_t delta_y,
                            char *out_msg, size_t msg_len)
{
    ensure_world_initialized();
    if (!state || !s_active_map) {
        return false;
    }

    int32_t step_px = (int32_t)s_active_map->dimensions.cell_size_px;
    s_camera.target_x += delta_x * step_px;
    s_camera.target_y += delta_y * step_px;

    /* Center camera on target */
    s_camera.cam_x = s_camera.target_x -
                     (int32_t)(s_viewport.target_view_w / 2);
    s_camera.cam_y = s_camera.target_y -
                     (int32_t)(s_viewport.target_view_h / 2);
    map_scaler_clamp_camera(&s_camera, &s_active_map->dimensions, &s_viewport);

    if (out_msg) {
        snprintf(out_msg, msg_len,
                 "Traversed grid on '%s' to (%d, %d). Camera centered.",
                 s_active_map->canonical_name,
                 s_camera.target_x / step_px,
                 s_camera.target_y / step_px);
    }
    return true;
}

bool world_handle_move(player_state_t *state, const char *direction,
                       char *out_msg, size_t msg_len)
{
    if (state == NULL || direction == NULL) {
        return false;
    }

    ensure_world_initialized();
    location_id_t target = world_topology_resolve_direction(state->location,
                                                           direction);
    if (target == LOC_COUNT) {
        if (out_msg) {
            snprintf(out_msg, msg_len,
                     "You cannot travel '%s' from %s. Path is blocked.",
                     direction, world_get_location_name(state->location));
        }
        return false;
    }

    char block_reason[128] = {0};
    if (!world_topology_can_traverse(state->location, target, false,
                                     state->has_dream_compass,
                                     block_reason, sizeof(block_reason))) {
        if (out_msg) {
            str_safe_copy(out_msg, block_reason, msg_len);
        }
        return false;
    }

    location_id_t prev = state->location;
    state->location = target;

    /* Switch active map to match new province */
    const map_def_t *new_map = maps_get_for_province(target);
    if (new_map) {
        s_active_map = new_map;
        s_camera.target_x = s_active_map->spawn_x *
                            (int32_t)s_active_map->dimensions.cell_size_px;
        s_camera.target_y = s_active_map->spawn_y *
                            (int32_t)s_active_map->dimensions.cell_size_px;
        s_camera.cam_x = s_camera.target_x -
                         (int32_t)(s_viewport.target_view_w / 2);
        s_camera.cam_y = s_camera.target_y -
                         (int32_t)(s_viewport.target_view_h / 2);
        map_scaler_clamp_camera(&s_camera, &s_active_map->dimensions,
                                &s_viewport);
    }

    /* Spawn provincial encounters */
    if (target == LOC_WHISPERING_WOODS && state->threat_hp <= 0) {
        str_safe_copy(state->threat_name, "ShadowStalker",
                      sizeof(state->threat_name));
        state->threat_hp = 50;
        state->threat_max_hp = 50;
        state->threat_def = 10;
        state->threat_mdf = 12;
        state->threat_surrounding_hp = 15;
    } else if (target == LOC_THE_DREAMSCAPE && state->threat_hp <= 0) {
        str_safe_copy(state->threat_name, "Dream Eater",
                      sizeof(state->threat_name));
        state->threat_hp = 35;
        state->threat_max_hp = 35;
        state->threat_def = 8;
        state->threat_mdf = 10;
        state->threat_surrounding_hp = 10;
    }

    LOG_INFO(
        LOG_CAT_LOG,
        "World",
        log_percent_to_grade(state->moral_sway),
        "Protagonist moved: %s -> %s.",
        world_get_location_name(prev),
        world_get_location_name(target)
    );

    if (out_msg) {
        snprintf(out_msg, msg_len,
                 "You journeyed to %s. %s",
                 world_get_location_name(target),
                 world_get_location_desc(target));
    }
    return true;
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// =============================================================================

diag_result_t world_preflight(void)
{
    diag_result_t res = {
        true,
        SEVERITY_PASS,
        "sim.engine.world",
        "world.c",
        "World orchestrator, scaler, topology, and map catalogs verified."
    };

    diag_result_t scaler_res = map_scaler_preflight();
    if (!scaler_res.passed) {
        return scaler_res;
    }

    diag_result_t topo_res = world_topology_preflight();
    if (!topo_res.passed) {
        return topo_res;
    }

    diag_result_t maps_res = maps_preflight();
    if (!maps_res.passed) {
        return maps_res;
    }

    diag_result_t loader_res = world_loader_preflight();
    if (loader_res.severity == SEVERITY_CRITICAL ||
        loader_res.severity == SEVERITY_FATAL) {
        return loader_res;
    }

    return res;
}
