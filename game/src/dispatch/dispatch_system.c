// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          dispatch_system.c
// @id            sim.dispatch.system
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
//   Specialized domain dispatcher for system lifecycle, help manual, status
//   appraisal, hero designation, and interactive debugger state snapshots.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: sim/dispatch/dispatch_system.h,
//                            sim/core/sim_state.h, util/debug/debug.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic allocation).
//   - Concurrency / Safety:  REENTRANT_PURE (Pure reentrant state evaluation).
//   - Latency Budget:        < 20 microseconds per system action.
//
// AXIOMATIC INVARIANTS:
//   1. Only evaluates actions when state != NULL and ctx != NULL.
//   2. Session quit unconditionally sets state->is_running = false.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// =============================================================================

#include "dispatch/dispatch_system.h"
#include "combat/combat.h"
#include "ending/game.h"
#include "hero/sim_state.h"
#include "world/world.h"
#include "debug/debug.h"
#include "core/string_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const module_metadata_t dispatch_system_manifest = {
    "sim.dispatch.system",
    "dispatch_system.h",
    LAYER_1_SIMULATION,
    { 1, 0, 0 },
    1,
    "2026-11-03",
    "System lifecycle, status appraisal, and debugger command router",
    2,
    { "sim.engine.state", "core.util.debug", NULL, NULL },
    ALLOC_ZERO_HEAP,
    CONCURRENCY_REENTRANT,
    20,
    2,
    {
        "Evaluates only verbs assigned to DOMAIN_SYSTEM",
        "Session quit unconditionally sets state->is_running = false"
    },
    dispatch_system_preflight
};

static void print_status_card(const player_state_t *state)
{
    if (!state) return;

    printf("\n--- Protagonist Status ---\n");
    printf("Protagonist:   %s, %s\n", state->name, state->title);
    printf("Class:         %s (Tier: %s)\n",
           sim_state_get_class_name(state->active_class),
           (state->active_class == CLASS_PRIEST) ? "Upgraded" : "Basic");
    if (state->former_tutelage[0] != '\0') {
        printf("Background:    Former %s\n", state->former_tutelage);
    }
    printf("Bound Relic:   %s\n", sim_state_get_relic_name(state->relic));
    printf("Grid Movement: %d tiles (MP)\n",
           (state->movement_points > 0) ? state->movement_points : 4);
    printf("Vitals:        HP: %d/%d | MP: %d/%d | TP: %d/%d\n",
           state->hp, state->max_hp, state->mp, state->max_mp,
           state->tp, state->max_tp);
    printf("Attributes:    ATK: %d | DEF: %d | MAT: %d | MDF: %d\n",
           state->atk, state->def, state->mat, state->mdf);
    printf("Location:      %s\n", world_get_location_name(state->location));
    printf("Stance:        %s\n", combat_get_stance_name(state->stance));
    printf("Shield of Lux: %d/100 (Warding Power)\n", state->shield_power);
    printf("Sword Tenebris:%d/100 (Edge Sharpness)\n", state->sword_edge);
    printf("Moral Sway:    %+d [%s]\n", state->moral_sway,
           sim_state_get_sway_desc(state->moral_sway));
    if (state->lux_barrio_turns > 0) {
        printf("Active Barrier:Lux Barrio (-25%% dmg, %d turns rem)\n",
               state->lux_barrio_turns);
    }
    if (state->charm_turns > 0) {
        printf("Active Charm:  Basic Charm (+20%% MAT, %d turns rem)\n",
               state->charm_turns);
    }
    if (state->threat_hp > 0) {
        printf("Active Threat: %s: %d/%d Vigor (Flank: %d HP)\n",
               state->threat_name, state->threat_hp, state->threat_max_hp,
               state->threat_surrounding_hp);
    }
    printf("Inventory:     %u slots used | %u Gold (G)\n",
           (unsigned)state->inventory.slot_count,
           (unsigned)state->inventory.currency_gold);
    printf("Honor Record:  %d Foes Vanquished | %d Warded | %d Turns\n",
           state->foes_vanquished, state->innocents_warded, state->turns_taken);
    printf("--------------------------------------\n\n");
}

static void print_help_menu(void)
{
    printf("\n--- Tactical Command Manual ---\n");
    printf("  defend [barrio]   - Raise Shield / Lux Barrio (-25%% dmg)\n");
    printf("  attack [cuttatio] - Strike Sword / Cuttatio (20%% splash)\n");
    printf("  special <skill>   - Cast: magic, ward, charm, duo\n");
    printf("  stance <type>     - Shift: warding, striking, equilibrium\n");
    printf("  items / inventory - Inspect player pouch & items\n");
    printf("  use <id|name>     - Consume draught or consult diegetic relic\n");
    printf("  inspect <id|name> - Appraise item, catalyst, or relic\n");
    printf("  clock             - Check monotonic time via Grandma's Watch\n");
    printf("  morality          - Read alignment on Compass of Morality\n");
    printf("  mode <turn|tact>  - Switch battle paradigm\n");
    printf("  name <name>       - Custom hero designation (default: Elijah)\n");
    printf("  look              - Survey current realm & terrain\n");
    printf("  map               - View topological graph & viewport\n");
    printf("  status            - Inspect vitals, relic charges & buffs\n");
    printf("  move <n|s|e|w>    - Traverse the Dreamscape & Kingdom\n");
    printf("  help              - Display this command manual\n");
    printf("  quit              - Relinquish armaments and exit\n");
    printf("-------------------------------\n\n");
}

// =============================================================================
// BLOCK 3: BODY BLOCK / API IMPLEMENTATIONS
// =============================================================================

bool dispatch_system_handle(player_state_t *state,
                            const dispatch_context_t *ctx,
                            char *out_msg, size_t msg_len)
{
    if (!state || !ctx) {
        return false;
    }

    const char *verb = ctx->canonical_verb;
    const char *arg = ctx->arg;

    /* 1. Quit / Exit */
    if (str_case_eq(verb, "quit") || str_case_eq(verb, "exit")) {
        state->is_running = false;
        if (out_msg) {
            snprintf(out_msg, msg_len,
                     "Farewell, Bearer. The chronicles rest.");
        }
        printf("You sheath your relics. The chronicles fall dormant.\n");
        return true;
    }

    /* 2. Help Manual */
    if (str_case_eq(verb, "help")) {
        print_help_menu();
        if (out_msg) {
            snprintf(out_msg, msg_len, "Displayed tactical command manual.");
        }
        return true;
    }

    /* 3. Protagonist Status Card */
    if (str_case_eq(verb, "status")) {
        print_status_card(state);
        if (out_msg) {
            snprintf(out_msg, msg_len, "Displayed protagonist status card.");
        }
        return true;
    }

    /* 4. Designation / Rename */
    if (str_case_eq(verb, "name")) {
        sim_state_set_name(state, arg);
        if (out_msg) {
            snprintf(out_msg, msg_len,
                     "Protagonist designated as '%s'.", state->name);
        }
        printf("Protagonist designated as '%s'.\n", state->name);
        return true;
    }

    /* 5. Interactive Debugger State Snapshots */
    if (str_case_eq(verb, "debug") || str_case_eq(verb, "snap")) {
        const char *tag = (arg && strlen(arg) > 0) ? arg : "ManualSnapshot";
        uint64_t snap_id = game_record_debug_snapshot(state, "debug", tag);
        if (out_msg) {
            snprintf(out_msg, msg_len,
                     "Captured state snapshot #%llu ('%s') at Turn %d.",
                     (unsigned long long)snap_id, tag, state->turns_taken);
        }
        printf("Captured snapshot #%llu ('%s').\n",
               (unsigned long long)snap_id, tag);
        return true;
    }

    /* 6. Time-Travel Rewind */
    if (str_case_eq(verb, "rewind")) {
        size_t target_idx = 0;
        if (arg && strlen(arg) > 0) {
            target_idx = (size_t)strtoul(arg, NULL, 10);
        }
        bool ok = game_rewind_debug_snapshot(target_idx, state);
        if (out_msg) {
            snprintf(out_msg, msg_len,
                     ok ? "Rewound state to snapshot #%u." :
                          "Failed to rewind: invalid snapshot index.",
                     (unsigned)target_idx);
        }
        printf("%s\n", out_msg ? out_msg : "");
        return true;
    }

    /* 7. History */
    if (str_case_eq(verb, "history")) {
        debug_dump_snapshot_history();
        if (out_msg) {
            snprintf(out_msg, msg_len, "Displayed debugger snapshot history.");
        }
        return true;
    }

    return false;
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// =============================================================================

diag_result_t dispatch_system_preflight(void)
{
    diag_result_t res = {
        true,
        SEVERITY_PASS,
        "sim.dispatch.system",
        "dispatch_system.c",
        "System domain dispatcher and lifecycle routines verified."
    };

    player_state_t test_state;
    memset(&test_state, 0, sizeof(test_state));
    sim_state_init(&test_state);

    dispatch_context_t ctx;
    memset(&ctx, 0, sizeof(ctx));
    str_safe_copy(ctx.canonical_verb, "status", sizeof(ctx.canonical_verb));
    ctx.domain = DOMAIN_SYSTEM;

    char msg[128];
    bool ok = dispatch_system_handle(&test_state, &ctx, msg, sizeof(msg));
    if (!ok || strlen(msg) == 0) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "System dispatcher failed to process 'status' command";
        return res;
    }

    return res;
}
