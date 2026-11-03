// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          dispatcher.c
// @id            sim.engine.dispatcher
// @layer         Layer 1: Simulation Engine
// @version       1.2.0
// @interface_rev Rev 3
// @date          2026-10-30
// @updated       2026-10-30
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Simulation command parsing, verb tokenization, and action dispatch.
//   Routes parsed actions into combat mechanics, provincial topology, and
//   terminal status printers. Supports authentic 2017-2018 demo canon:
//   - Defend (Lux Barrio: 2-5 turn barrier, 25% damage reduction)
//   - Attack (Cuttatio Tenebris: 20% splash damage to flank)
//   - Special: Basic Magic, Basic Ward, Basic Charm, Duo-Loca Bashin n Slash
//   - Character identity customization (canonical: Elijah/Katrina)
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: sim.types.common, sim.engine.state,
//                            sim.engine.combat, sim.engine.world,
//                            core.util.string, core.util.log
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic allocation)
//   - Concurrency / Safety:  REENTRANT_PURE (Pure reentrant command dispatch)
//   - Latency Budget:        < 50 microseconds per command dispatch
//
// AXIOMATIC INVARIANTS:
//   1. Stack-only tokenization buffer; zero dynamic heap allocations.
//   2. Empty, whitespace-only, or unknown inputs produce no state mutations.
//   3. Turns increment deterministically on successfully executed actions.
//   4. Active buff durations decrement strictly on completed turns.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / STATIC DATA
// Stratum: Layer 1 (Simulation Engine) | Machine: Command Dispatch Core
// =============================================================================

#include "sim/dispatcher.h"
#include "sim/combat.h"
#include "sim/sim_state.h"
#include "sim/world.h"
#include "sim/game.h"
#include "util/log/log.h"
#include "util/string/string_utils.h"
#include "util/debug/debug.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *const s_dispatcher_invariants[4] = {
    "Pure stack buffer tokenization with zero heap allocations",
    "Invalid or unrecognized inputs preserve state deterministically",
    "Simulation turns increment strictly upon recognized action dispatch",
    "Buff and barrier durations decrement deterministically on each turn"
};

const module_metadata_t dispatcher_manifest = {
    "sim.engine.dispatcher",
    "dispatcher.h",
    LAYER_1_SIMULATION,
    { 1, 2, 0 },
    3,                          // interface_rev
    "2026-10-30",               // last_updated
    "Command line parsing, verb tokenization, and simulation routing",
    4,                          // dependency_count
    {
        "sim.engine.state",
        "sim.engine.combat",
        "sim.engine.world",
        "core.util.log"
    },
    ALLOC_ZERO_HEAP,            // alloc_policy
    CONCURRENCY_REENTRANT,      // concurrency
    50,                         // max_budget_us
    4,
    {
        s_dispatcher_invariants[0],
        s_dispatcher_invariants[1],
        s_dispatcher_invariants[2],
        s_dispatcher_invariants[3]
    },
    dispatcher_preflight
};

// =============================================================================
// BLOCK 3: BODY BLOCK / CORE LOGIC
// Stratum: Layer 1 (Simulation Engine) | Machine: Command Dispatch Core
// =============================================================================

void dispatcher_print_intro(void)
{
    printf("============================================================\n");
    printf("                  TENEBRIS AND LUX                          \n");
    printf("      Chronicles of the Shield of Lux & Sword of Tenebris   \n");
    printf("============================================================\n\n");
    printf("You awaken within the ethereal expanse of The Dreamscape.\n");
    printf("Beyond the veil of slumber, the Kingdom of Sacyra stirs,\n");
    printf("while in the deep shadows, The Dark Lord's vanguard gathers.\n");
    printf("Wielding the Shield of Lux and the Sword of Tenebris,\n");
    printf("your decisions shape the moral continuum of the realm.\n\n");
    printf("Type 'help' for tactical commands, or 'quit' to yield.\n\n");
}

void dispatcher_print_status(const player_state_t *state)
{
    if (state == NULL) return;

    printf("\n--- Bearer Covenant & Relic Status ---\n");
    printf("Bearer:        %s\n", state->name);
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
    printf("Honor Record:  %d Foes Vanquished | %d Warded | %d Turns\n",
        state->foes_vanquished, state->innocents_warded, state->turns_taken);
    printf("--------------------------------------\n\n");
}

void dispatcher_print_location(const player_state_t *state)
{
    if (state == NULL) return;

    printf("\n[%s]\n", world_get_location_name(state->location));
    printf("%s\n", world_get_location_desc(state->location));
    if (state->threat_hp > 0) {
        printf("[!] A hostile presence blocks your path: %s (Vigor: %d/%d)!\n",
            state->threat_name, state->threat_hp, state->threat_max_hp);
    }
    printf("\n");
}

void dispatcher_print_help(void)
{
    printf("\n--- Tactical Command Manual ---\n");
    printf("  defend [barrio]      - Raise Shield of Lux / Cast Lux Barrio (-25%% dmg)\n");
    printf("  attack [cuttatio]    - Strike with Sword / Cuttatio Tenebris (20%% splash)\n");
    printf("  special <skill>      - Cast: magic (neutral dmg), ward (magic def),\n");
    printf("                               charm (+20%% MAT), duo (adaptive dual strike)\n");
    printf("  stance <type>        - Shift: warding, striking, equilibrium, neutral\n");
    printf("  name <chosen_name>   - Custom hero designation (default: Elijah)\n");
    printf("  look                 - Survey current realm & terrain\n");
    printf("  status               - Inspect vitals, relic charges & buffs\n");
    printf("  move <n|s|e|w>       - Traverse the Dreamscape & Kingdom\n");
    printf("  help                 - Display this command manual\n");
    printf("  quit                 - Relinquish armaments and exit\n");
    printf("-------------------------------\n\n");
}

void dispatcher_process_command(player_state_t *state, const char *raw_input,
                                char *out_msg, size_t msg_len)
{
    if (state == NULL) {
        return;
    }

    char buffer[MAX_INPUT_LEN];
    str_safe_copy(buffer, raw_input, sizeof(buffer));
    str_trim_inplace(buffer);

    if (strlen(buffer) == 0) {
        return;
    }

    char *command = strtok(buffer, " ");
    if (command == NULL) {
        return;
    }
    char *arg = strtok(NULL, "");
    if (arg != NULL) {
        while (isspace((unsigned char)*arg)) {
            arg++;
        }
    }

    LOG_DEBUG(
        LOG_CAT_DEBUG,
        "Dispatcher",
        log_percent_to_grade(state->moral_sway),
        "Processing command '%s'%s%s (Turn: %d).",
        command,
        arg ? " arg: " : "",
        arg ? arg : "",
        state->turns_taken + 1
    );

    if (strcmp(command, "quit") == 0 || strcmp(command, "exit") == 0) {
        state->is_running = false;
        if (out_msg) {
            snprintf(out_msg, msg_len, "Relinquishing armaments. Farewell.");
        }
        printf("You lower weapons and step back from the frontline. Farewell.\n");

        LOG_INFO(
            LOG_CAT_LOG,
            "GameSim",
            log_percent_to_grade(state->moral_sway),
            "Relinquished armaments and exited game loop (Turns:%d, Foes:%d).",
            state->turns_taken,
            state->foes_vanquished
        );
        return;
    }

    if (strcmp(command, "help") == 0) {
        if (out_msg) {
            snprintf(out_msg, msg_len,
                "Commands: defend, attack, special, stance, name, look, status, move.");
        }
        dispatcher_print_help();
        return;
    }

    if (strcmp(command, "status") == 0) {
        if (out_msg) {
            snprintf(out_msg, msg_len,
                "%s | HP:%d | Shield:%d | Sword:%d | Stance:%s | Sway:%+d",
                state->name, state->hp, state->shield_power, state->sword_edge,
                combat_get_stance_name(state->stance), state->moral_sway);
        }
        dispatcher_print_status(state);
        return;
    }

    if (strcmp(command, "look") == 0) {
        if (out_msg) {
            snprintf(out_msg, msg_len, "Surveying %s.",
                     world_get_location_name(state->location));
        }
        dispatcher_print_location(state);
        return;
    }

    if (strcmp(command, "name") == 0 || strcmp(command, "hero") == 0) {
        sim_state_set_name(state, arg);
        if (out_msg) {
            snprintf(out_msg, msg_len, "Hero designated as: %s.", state->name);
        }
        printf("Bearer identity confirmed: %s.\n", state->name);
        return;
    }

    /* Debugger Commands */
    if (strcmp(command, "debug") == 0) {
        if (arg == NULL || strlen(arg) == 0 || strcmp(arg, "status") == 0) {
            printf("[Debugger Status] Active: %s | Profile: %s | Snapshots: %zu / %zu\n",
                   debug_is_active() ? "YES" : "NO",
                   debug_get_active_profile_name(),
                   debug_get_snapshot_count(),
                   (size_t)DEBUG_MAX_SNAPSHOTS);
        } else if (strcmp(arg, "inspect") == 0) {
            debug_state_fields_t fields;
            game_state_to_debug_fields(state, &fields);
            debug_dump_state(&fields);
        } else if (strcmp(arg, "history") == 0) {
            debug_dump_snapshot_history();
        } else {
            printf("Usage: debug [status | inspect | history]\n");
        }
        if (out_msg) {
            snprintf(out_msg, msg_len, "Debugger command '%s' processed.", (arg ? arg : "status"));
        }
        return;
    }

    if (strcmp(command, "inspect") == 0) {
        debug_state_fields_t fields;
        game_state_to_debug_fields(state, &fields);
        debug_dump_state(&fields);
        if (out_msg) {
            snprintf(out_msg, msg_len, "State inspected.");
        }
        return;
    }

    if (strcmp(command, "history") == 0) {
        debug_dump_snapshot_history();
        if (out_msg) {
            snprintf(out_msg, msg_len, "Snapshot history displayed (%zu snapshots).", debug_get_snapshot_count());
        }
        return;
    }

    if (strcmp(command, "rewind") == 0) {
        if (arg == NULL || strlen(arg) == 0) {
            printf("Usage: rewind <snapshot_index>\n");
            if (out_msg) snprintf(out_msg, msg_len, "Rewind requires snapshot index.");
            return;
        }
        size_t idx = (size_t)atoi(arg);
        if (game_rewind_debug_snapshot(idx, state)) {
            printf("Successfully rewound to snapshot #%zu (Turn %d).\n", idx, state->turns_taken);
            if (out_msg) snprintf(out_msg, msg_len, "Rewound to snapshot #%zu.", idx);
        } else {
            printf("Rewind failed: invalid snapshot index %zu (Total: %zu).\n", idx, debug_get_snapshot_count());
            if (out_msg) snprintf(out_msg, msg_len, "Rewind failed: invalid index.");
        }
        return;
    }

    if (strcmp(command, "snap") == 0) {
        uint64_t snap_id = game_record_debug_snapshot(state, "manual_snap", arg);
        printf("Recorded manual snapshot #%lu at Turn %d.\n", (unsigned long)snap_id, state->turns_taken);
        if (out_msg) snprintf(out_msg, msg_len, "Snapshot #%lu recorded.", (unsigned long)snap_id);
        return;
    }

    /* Combat Verb 1: Defend (or Ward / Shield / Barrio) */
    if (strcmp(command, "defend") == 0 || strcmp(command, "ward") == 0 ||
        strcmp(command, "shield") == 0) {
        bool use_barrio = (arg != NULL &&
            (strstr(arg, "barrio") != NULL || strstr(arg, "lux") != NULL));
        combat_handle_defend(state, use_barrio, out_msg, msg_len);
    } else if (strcmp(command, "barrio") == 0 || strcmp(command, "lux") == 0) {
        combat_handle_defend(state, true, out_msg, msg_len);
    }
    /* Combat Verb 2: Attack (or Strike / Slash / Cuttatio) */
    else if (strcmp(command, "attack") == 0 || strcmp(command, "strike") == 0 ||
             strcmp(command, "slash") == 0) {
        bool use_cuttatio = (arg != NULL &&
            (strstr(arg, "cuttatio") != NULL || strstr(arg, "tenebris") != NULL));
        combat_handle_attack(state, use_cuttatio, out_msg, msg_len);
    } else if (strcmp(command, "cuttatio") == 0) {
        combat_handle_attack(state, true, out_msg, msg_len);
    }
    /* Combat Verb 3: Special (Magic, Ward, Charm, Duo-Loca) */
    else if (strcmp(command, "special") == 0 || strcmp(command, "balance") == 0) {
        if (arg == NULL || strlen(arg) == 0) {
            combat_handle_special(state, SPECIAL_DUO_LOCA, out_msg, msg_len);
        } else if (strstr(arg, "mag") != NULL) {
            combat_handle_special(state, SPECIAL_BASIC_MAGIC, out_msg, msg_len);
        } else if (strstr(arg, "ward") != NULL) {
            combat_handle_special(state, SPECIAL_BASIC_WARD, out_msg, msg_len);
        } else if (strstr(arg, "charm") != NULL) {
            combat_handle_special(state, SPECIAL_BASIC_CHARM, out_msg, msg_len);
        } else {
            combat_handle_special(state, SPECIAL_DUO_LOCA, out_msg, msg_len);
        }
    } else if (strcmp(command, "magic") == 0) {
        combat_handle_special(state, SPECIAL_BASIC_MAGIC, out_msg, msg_len);
    } else if (strcmp(command, "charm") == 0) {
        combat_handle_special(state, SPECIAL_BASIC_CHARM, out_msg, msg_len);
    } else if (strcmp(command, "duo") == 0) {
        combat_handle_special(state, SPECIAL_DUO_LOCA, out_msg, msg_len);
    }
    /* Combat Stance & Spatial Movement */
    else if (strcmp(command, "stance") == 0) {
        combat_handle_stance(state, arg, out_msg, msg_len);
    } else if (strcmp(command, "move") == 0 || strcmp(command, "go") == 0) {
        world_handle_move(state, arg, out_msg, msg_len);
    } else {
        if (out_msg) {
            snprintf(out_msg, msg_len, "Unknown command '%s'.", command);
        }
        printf("Unknown command: '%s'. Type 'help' for options.\n", command);

        LOG_WARN(
            LOG_CAT_DEBUG,
            "Dispatcher",
            log_percent_to_grade(state->moral_sway),
            "Unrecognized command verb rejected: '%s'.",
            command
        );
        return;
    }

    /* Turn progression and buff duration countdown */
    state->turns_taken++;
    if (state->lux_barrio_turns > 0) {
        state->lux_barrio_turns--;
    }
    if (state->charm_turns > 0) {
        state->charm_turns--;
    }

    /* Record state snapshot into debugger ring buffer if active */
    if (debug_is_active()) {
        game_record_debug_snapshot(state, command, arg);

        char break_reason[128];
        debug_state_fields_t fields;
        game_state_to_debug_fields(state, &fields);

        if (debug_check_breakpoints(&fields, break_reason, sizeof(break_reason))) {
            printf("\n[DEBUG BREAKPOINT TRIGGERED] %s\n", break_reason);
            LOG_WARN(LOG_CAT_DEBUG, "Dispatcher", 0.0f,
                     "Breakpoint triggered: %s", break_reason);
        }
    }
}

void dispatcher_loop_cli(player_state_t *state)
{
    if (state == NULL) return;

    char input_line[MAX_INPUT_LEN];

    dispatcher_print_intro();
    dispatcher_print_location(state);

    while (state->is_running) {
        printf("[%s | HP:%d | Shield:%d | Sword:%d]> ",
            state->name, state->hp, state->shield_power, state->sword_edge);
        fflush(stdout);

        if (!fgets(input_line, sizeof(input_line), stdin)) {
            break;
        }

        input_line[strcspn(input_line, "\r\n")] = '\0';
        dispatcher_process_command(state, input_line, NULL, 0);
    }
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Command Dispatch Core
// =============================================================================

diag_result_t dispatcher_preflight(void)
{
    diag_result_t res;
    res.subsystem = dispatcher_manifest.module_id;
    res.check_name = "Command Tokenization & Dispatch Routing";

    player_state_t test;
    sim_state_init(&test);

    // 1. Verify whitespace and empty input rejection
    int turns_before = test.turns_taken;
    dispatcher_process_command(&test, "   ", NULL, 0);
    if (test.turns_taken != turns_before) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Whitespace input improperly incremented turn counter.";
        return res;
    }

    // 2. Verify hero name customization
    dispatcher_process_command(&test, "name Katrina", NULL, 0);
    if (strcmp(test.name, "Katrina") != 0) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Hero naming command failed to update state name.";
        return res;
    }

    // 3. Verify defend and Lux Barrio dispatch
    dispatcher_process_command(&test, "barrio", NULL, 0);
    if (test.turns_taken != turns_before + 1) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Valid combat command failed to increment turn counter.";
        return res;
    }

    // 4. Verify Cuttatio attack dispatch
    dispatcher_process_command(&test, "cuttatio", NULL, 0);
    if (test.turns_taken != turns_before + 2) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Cuttatio command failed to advance turn counter.";
        return res;
    }

    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.details = "Command tokenization, naming, defend/attack routing verified.";
    return res;
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================
