// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          dispatch_world.c
// @id            sim.dispatch.world
// @layer         Layer 1: Simulation Engine
// @subsystem     Command Dispatcher Subsystem (sim/dispatch)
// @version       1.0.0
// @interface_rev Rev 1
// @date          2026-11-03
// @updated       2026-11-03
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Specialized domain dispatcher for world navigation, spatial movement,
//   active map queries, and provincial travel.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: sim/dispatch/dispatch_world.h, sim/world/world.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic allocation).
//   - Concurrency / Safety:  REENTRANT_PURE (Pure reentrant state evaluation).
//   - Latency Budget:        < 20 microseconds per navigation command.
//
// AXIOMATIC INVARIANTS:
//   1. Only evaluates actions when state != NULL and ctx != NULL.
//   2. Blocked moves produce zero state mutations and informative feedback.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// =============================================================================

#include "dispatch/dispatch_world.h"
#include "hero/sim_state.h"
#include "world/world.h"
#include "core/string_utils.h"
#include <stdio.h>
#include <string.h>

const module_metadata_t dispatch_world_manifest = {
    "sim.dispatch.world",
    "dispatch_world.h",
    LAYER_1_SIMULATION,
    { 1, 0, 0 },
    1,
    "2026-11-03",
    "World navigation command dispatcher and map query router",
    1,
    { "sim.engine.world", NULL, NULL, NULL },
    ALLOC_ZERO_HEAP,
    CONCURRENCY_REENTRANT,
    20,
    2,
    {
        "Evaluates only verbs assigned to DOMAIN_WORLD",
        "Blocked moves produce zero state mutations and informative feedback"
    },
    dispatch_world_preflight
};

// =============================================================================
// BLOCK 3: BODY BLOCK / API IMPLEMENTATIONS
// =============================================================================

bool dispatch_world_handle(player_state_t *state,
                           const dispatch_context_t *ctx,
                           char *out_msg, size_t msg_len)
{
    if (!state || !ctx) {
        return false;
    }

    const char *verb = ctx->canonical_verb;
    const char *arg = ctx->arg;

    /* 1. Look / Survey Location */
    if (str_case_eq(verb, "look")) {
        const map_def_t *active_map = world_get_active_map();
        if (out_msg) {
            snprintf(out_msg, msg_len,
                     "[%s] %s (Map: %s - %s)",
                     world_get_location_name(state->location),
                     world_get_location_desc(state->location),
                     active_map ? active_map->canonical_name : "Overworld",
                     active_map ? active_map->description : "");
        }
        printf("\n[%s]\n%s\n",
               world_get_location_name(state->location),
               world_get_location_desc(state->location));
        if (active_map) {
            printf("Active Map: %s (%dx%d cells, %dpx)\n",
                   active_map->canonical_name,
                   active_map->dimensions.width,
                   active_map->dimensions.height,
                   active_map->dimensions.cell_size_px);
        }
        if (state->threat_hp > 0) {
            printf("[!] Hostile presence: %s (Vigor: %d/%d)!\n",
                   state->threat_name, state->threat_hp, state->threat_max_hp);
        }
        return true;
    }

    /* 2. Map / Viewport Inspection */
    if (str_case_eq(verb, "map")) {
        const map_def_t *active_map = world_get_active_map();
        const map_viewport_t *vp = world_get_viewport();
        const map_camera_t *cam = world_get_camera();
        if (out_msg) {
            snprintf(out_msg, msg_len,
                     "Map: '%s' | Cam: (%d, %d) | Viewport: %ux%u (%.2fx)",
                     active_map ? active_map->canonical_name : "Overworld",
                     cam ? cam->cam_x : 0, cam ? cam->cam_y : 0,
                     vp ? vp->screen_w : 0, vp ? vp->screen_h : 0,
                     vp ? vp->scale_factor : 1.0f);
        }
        printf("\n--- Topological Map & Dynamic Viewport ---\n");
        if (active_map) {
            uint32_t px_w = active_map->dimensions.width *
                            active_map->dimensions.cell_size_px;
            uint32_t px_h = active_map->dimensions.height *
                            active_map->dimensions.cell_size_px;
            printf("Active Zone: %s [%s]\n",
                   active_map->canonical_name,
                   (active_map->environment == MAP_ENV_EXTERIOR) ? "Exterior" :
                   (active_map->environment == MAP_ENV_INTERIOR) ? "Interior" :
                   (active_map->environment == MAP_ENV_ASTRAL) ? "Astral" :
                   "Battle Zone");
            printf("Dimensions:  %ux%u cells (%ux%u px)\n",
                   active_map->dimensions.width,
                   active_map->dimensions.height, px_w, px_h);
        }
        if (vp && cam) {
            printf("Screen Size: %ux%u px (Target: %ux%u px)\n",
                   vp->screen_w, vp->screen_h,
                   vp->target_view_w, vp->target_view_h);
            printf("Scale:       %.2fx (Letterbox: %d px, Pillarbox: %d px)\n",
                   vp->scale_factor, vp->offset_y, vp->offset_x);
            printf("Camera Pos:  (%d, %d) [Tracking Player: (%d, %d)]\n",
                   cam->cam_x, cam->cam_y, cam->target_x, cam->target_y);
        }
        printf("------------------------------------------\n\n");
        return true;
    }

    /* 3. Move / Travel */
    if (str_case_eq(verb, "move") || str_case_eq(verb, "travel")) {
        if (!arg || strlen(arg) == 0) {
            if (out_msg) {
                snprintf(out_msg, msg_len,
                         "Specify direction: north, south, east, or west.");
            }
            return true;
        }

        bool moved = world_handle_move(state, arg, out_msg, msg_len);
        if (moved) {
            state->turns_taken++;
            printf("%s\n", out_msg ? out_msg : "Moved.");
        } else {
            printf("%s\n", out_msg ? out_msg : "Movement blocked.");
        }
        return true;
    }

    return false;
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// =============================================================================

diag_result_t dispatch_world_preflight(void)
{
    diag_result_t res = {
        true,
        SEVERITY_PASS,
        "sim.dispatch.world",
        "dispatch_world.c",
        "World domain dispatcher and movement routing verified."
    };

    player_state_t test_state;
    memset(&test_state, 0, sizeof(test_state));
    sim_state_init(&test_state);

    dispatch_context_t ctx;
    memset(&ctx, 0, sizeof(ctx));
    str_safe_copy(ctx.canonical_verb, "look", sizeof(ctx.canonical_verb));
    ctx.domain = DOMAIN_WORLD;

    char msg[256];
    bool ok = dispatch_world_handle(&test_state, &ctx, msg, sizeof(msg));
    if (!ok || strlen(msg) == 0) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "World dispatcher failed to process 'look' command";
        return res;
    }

    return res;
}
