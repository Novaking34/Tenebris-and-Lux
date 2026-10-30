// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          dispatcher.c
// @id            sim.engine.dispatcher
// @layer         Layer 1: Simulation Engine
// @version       1.0.0
// @interface_rev Rev 1
// @date          2026-10-30
// @updated       2026-10-30
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Simulation command parsing, verb tokenization, and action dispatch.
//   Routes parsed actions into combat mechanics, provincial topology, and
//   terminal status printers.
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
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / STATIC DATA
// Stratum: Layer 1 (Simulation Engine) | Machine: Command Dispatch Core
// =============================================================================

#include "sim/dispatcher.h"
#include "sim/combat.h"
#include "sim/sim_state.h"
#include "sim/world.h"
#include "util/log.h"
#include "util/string_utils.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

static const char *const s_dispatcher_invariants[3] = {
    "Pure stack buffer tokenization with zero heap allocations",
    "Invalid or unrecognized inputs preserve state deterministically",
    "Simulation turns increment strictly upon recognized action dispatch"
};

const module_metadata_t dispatcher_manifest = {
    "sim.engine.dispatcher",
    "dispatcher.h",
    LAYER_1_SIMULATION,
    { 1, 0, 0 },
    1,                          // interface_rev
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
    3,
    {
        s_dispatcher_invariants[0],
        s_dispatcher_invariants[1],
        s_dispatcher_invariants[2]
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
    printf("The Kingdom of Sacyra teeters upon the knife-edge of oblivion.\n");
    printf("As bearer of both relics, your actions balance the continuum.\n");
    printf("Type 'help' for commands, or 'quit' to yield.\n\n");
}

void dispatcher_print_status(const player_state_t *state)
{
    if (state == NULL) return;

    printf("\n--- Relic Status & Bearer Covenant ---\n");
    printf("Location:      %s\n", world_get_location_name(state->location));
    printf("Stance:        %s\n", combat_get_stance_name(state->stance));
    printf("Shield of Lux: %d/100 (Warding Power)\n", state->shield_power);
    printf("Sword Tenebris:%d/100 (Edge Sharpness)\n", state->sword_edge);
    printf("Moral Sway:    %+d [%s]\n", state->moral_sway,
        sim_state_get_sway_desc(state->moral_sway));
    if (state->threat_hp > 0) {
        printf("Active Threat: Vanguard Vigor: %d/100\n", state->threat_hp);
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
        printf("[!] A vanguard patrol blocks your path (Vigor: %d/100)!\n",
            state->threat_hp);
    }
    printf("\n");
}

void dispatcher_print_help(void)
{
    printf("\n--- Tactical Command Manual ---\n");
    printf("  ward                 - Raise Shield of Lux (defend, protect)\n");
    printf("  strike               - Draw Sword of Tenebris (sever, strike)\n");
    printf("  balance              - Harmonize twin relics to parry\n");
    printf("  stance <type>        - Shift: warding, striking, equilibrium\n");
    printf("  look                 - Survey current region & terrain\n");
    printf("  status               - Inspect relic charges & moral sway\n");
    printf("  move <n|s|e|w>       - Traverse the fractured Kingdom\n");
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
        printf("You lower weapons and step back from frontline. Farewell.\n");

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
                "Commands: ward, strike, balance, stance, look, status, move.");
        }
        dispatcher_print_help();
        return;
    }

    if (strcmp(command, "status") == 0) {
        if (out_msg) {
            snprintf(out_msg, msg_len,
                "Shield: %d | Sword: %d | Stance: %s | Sway: %+d",
                state->shield_power, state->sword_edge,
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

    if (strcmp(command, "ward") == 0 || strcmp(command, "shield") == 0) {
        combat_handle_ward(state, out_msg, msg_len);
    } else if (strcmp(command, "strike") == 0 ||
               strcmp(command, "slash") == 0) {
        combat_handle_strike(state, out_msg, msg_len);
    } else if (strcmp(command, "balance") == 0 ||
               strcmp(command, "harmonize") == 0) {
        combat_handle_balance(state, out_msg, msg_len);
    } else if (strcmp(command, "stance") == 0) {
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

    state->turns_taken++;
}

void dispatcher_loop_cli(player_state_t *state)
{
    if (state == NULL) return;

    char input_line[MAX_INPUT_LEN];

    dispatcher_print_intro();
    dispatcher_print_location(state);

    while (state->is_running) {
        printf("relics [Shield:%d | Sword:%d]> ",
            state->shield_power, state->sword_edge);
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

    // Verify whitespace and empty input rejection
    int turns_before = test.turns_taken;
    dispatcher_process_command(&test, "   ", NULL, 0);
    if (test.turns_taken != turns_before) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Whitespace input improperly incremented turn counter.";
        return res;
    }

    // Verify recognized command routing
    dispatcher_process_command(&test, "ward", NULL, 0);
    if (test.turns_taken != turns_before + 1) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Valid command failed to increment turn counter.";
        return res;
    }

    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.details = "Command tokenization, whitespace rejection, and turns verified.";
    return res;
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================
