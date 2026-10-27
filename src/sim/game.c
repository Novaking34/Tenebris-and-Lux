// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          game.c
// @id            sim.engine.core
// @layer         Layer 1: Simulation Engine
// @version       1.0.0
// @interface_rev Rev 1
// @date          2026-10-26
// @updated       2026-10-26
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Core simulation logic, twin relics resonance transitions, moral sway
//   calculations, and deterministic stance state dispatcher.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: core.util.math, core.util.string
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Caller-owned state, zero heap).
//   - Concurrency / Safety:  REENTRANT_PURE (Pure reentrant state transitions).
//   - Latency Budget:        < 50 microseconds per simulation tick.
//
// AXIOMATIC INVARIANTS:
//   1. Shield of Lux power bounded: 0 <= shield_power <= MAX_RESONANCE.
//   2. Sword of Tenebris edge bounded: 0 <= sword_edge <= MAX_RESONANCE.
//   3. Moral sway bounded continuum: -100 <= moral_sway <= +100.
//   4. Valid location and stance enumerations strictly enforced.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / STATIC DATA
// Stratum: Layer 1 (Simulation Engine) | Machine: Tactical Simulation Core
// =============================================================================

#include "sim/game.h"
#include "util/math_utils.h"
#include "util/string_utils.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *location_names[LOC_COUNT] = {
    "The Garrison Altar",
    "The Sunken Citadel Gates",
    "The Fractured Marches",
    "The Abyssal Threshold"
};

static const char *location_descriptions[LOC_COUNT] = {
    "Ancestral redoubt of the Wardens. Twin pedestals of solar silver and "
    "star-quenched obsidian stand ready to channel the Shield of Lux and "
    "Sword of Tenebris.",
    "White granite battlements scorched by void-fire. Beyond the fractured "
    "portcullis, the vanguard legions of the Dark Lord Malakor gather.",
    "A windswept highland where Solar Order knights and Umbral Covenant "
    "partisans clash in civil strife, blind to the true encroaching terror.",
    "The shattered monolith gates of the Dark Lord Malakor. Reality bends "
    "as shadow tendrils strike against the radiant barrier of starlight."
};

static const char *stance_names[STANCE_COUNT] = {
    "Neutral (At Ease)",
    "Aegis Warding (Shield High Guard)",
    "Edge Striking (Sword Drawn)",
    "Equilibrium (Twin Harmonized)"
};

// =============================================================================
// BLOCK 3: BODY BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Tactical Simulation Core
// =============================================================================

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 3.1: SIMULATION LIFECYCLE & STATE QUERIES ]
// Mission: Initialize state, resolve geography, and format narrative labels.
// -----------------------------------------------------------------------------

const char *game_get_location_name(location_id_t loc)
{
    if (loc >= 0 && loc < LOC_COUNT) {
        return location_names[loc];
    }
    return "Uncharted Realm";
}

const char *game_get_location_desc(location_id_t loc)
{
    if (loc >= 0 && loc < LOC_COUNT) {
        return location_descriptions[loc];
    }
    return "Gloom obscures the contours of the land.";
}

const char *game_get_stance_name(combat_stance_t stance)
{
    if (stance >= 0 && stance < STANCE_COUNT) {
        return stance_names[stance];
    }
    return "Unknown Stance";
}

const char *game_get_sway_desc(int sway)
{
    if (sway >= 40) return "Saint of Radiant Mercy (Virtue Unyielding)";
    if (sway >= 15) return "Protector of the Weak (Virtue)";
    if (sway > -15) return "Equilibrium (Cosmic Balance)";
    if (sway > -40) return "Harbinger of Ruin (Dominion)";
    return "Lord of the Abyssal Void (Ruthless Dominion)";
}

void game_init(player_state_t *state)
{
    if (state == NULL) return;

    state->shield_power = INITIAL_SHIELD_POWER;
    state->sword_edge = INITIAL_SWORD_EDGE;
    state->moral_sway = INITIAL_MORAL_SWAY;
    state->threat_hp = 0;
    state->stance = STANCE_NEUTRAL;
    state->location = LOC_GARRISON_ALTAR;
    state->is_running = true;
    state->turns_taken = 0;
    state->foes_vanquished = 0;
    state->innocents_warded = 0;
}

void game_print_intro(void)
{
    printf("============================================================\n");
    printf("                  TENEBRIS AND LUX                          \n");
    printf("      Chronicles of the Shield of Lux & Sword of Tenebris   \n");
    printf("============================================================\n\n");
    printf("The Kingdom of Sacyra teeters upon the knife-edge of oblivion.\n");
    printf("As bearer of both relics, your actions balance the continuum.\n");
    printf("Type 'help' for commands, or 'quit' to yield.\n\n");
}

void game_print_status(const player_state_t *state)
{
    printf("\n--- Relic Status & Bearer Covenant ---\n");
    printf("Location:      %s\n", game_get_location_name(state->location));
    printf("Stance:        %s\n", game_get_stance_name(state->stance));
    printf("Shield of Lux: %d/100 (Warding Power)\n", state->shield_power);
    printf("Sword Tenebris:%d/100 (Edge Sharpness)\n", state->sword_edge);
    printf("Moral Sway:    %+d [%s]\n", state->moral_sway,
        game_get_sway_desc(state->moral_sway));
    if (state->threat_hp > 0) {
        printf("Active Threat: Vanguard Vigor: %d/100\n", state->threat_hp);
    }
    printf("Honor Record:  %d Foes Vanquished | %d Warded | %d Turns\n",
        state->foes_vanquished, state->innocents_warded, state->turns_taken);
    printf("--------------------------------------\n\n");
}

void game_print_location(const player_state_t *state)
{
    printf("\n[%s]\n", game_get_location_name(state->location));
    printf("%s\n", game_get_location_desc(state->location));
    if (state->threat_hp > 0) {
        printf("[!] A vanguard patrol blocks your path (Vigor: %d/100)!\n",
            state->threat_hp);
    }
    printf("\n");
}

void game_print_help(void)
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

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 3.2: COMMAND PARSING & SIMULATION DISPATCH ]
// Mission: Parse commands and execute deterministic state transitions.
// -----------------------------------------------------------------------------

static void handle_ward(player_state_t *state, char *out_msg, size_t msg_len)
{
    if (state->threat_hp > 0) {
        /* Combat Warding */
        int boost = (state->stance == STANCE_WARDING) ? 15 : 10;
        int stagger = (state->stance == STANCE_WARDING) ? 25 : 15;

        state->shield_power =
            math_saturating_add(state->shield_power, boost, MAX_RESONANCE);
        state->threat_hp = math_saturating_sub(state->threat_hp, stagger);
        state->moral_sway = math_clamp_int(state->moral_sway + 3, -100, 100);
        state->innocents_warded++;

        if (state->threat_hp <= 0) {
            state->threat_hp = 0;
            state->foes_vanquished++;
            if (out_msg) {
                snprintf(out_msg, msg_len,
                    "Shield of Lux flared! Blinded and routed vanguard.");
            }
            printf("You raise the Shield of Lux! A radiant shockwave "
                   "shatters the vanguard's resolve!\n");
        } else {
            if (out_msg) {
                snprintf(out_msg, msg_len,
                    "Warded assault! Staggered foe (-%d HP). Shield at %d.",
                    stagger, state->shield_power);
            }
            printf("You lock the Shield of Lux into the earth. "
                   "The void-strike glances off in a golden flash!\n");
        }
    } else {
        /* Non-combat Warding */
        state->shield_power =
            math_saturating_add(state->shield_power, 20, MAX_RESONANCE);
        state->moral_sway = math_clamp_int(state->moral_sway + 1, -100, 100);

        if (out_msg) {
            snprintf(out_msg, msg_len,
                "Shield of Lux projected a protective aura (Power: %d/100).",
                state->shield_power);
        }
        printf("You raise the Shield of Lux. "
               "Warm golden starlight wards the perimeter.\n");
    }
}

static void handle_strike(player_state_t *state, char *out_msg, size_t msg_len)
{
    if (state->threat_hp > 0) {
        /* Combat Strike */
        int cost = (state->stance == STANCE_STRIKING) ? 8 : 12;
        int damage = (state->stance == STANCE_STRIKING) ? 45 : 30;

        state->sword_edge = math_saturating_sub(state->sword_edge, cost);
        state->threat_hp = math_saturating_sub(state->threat_hp, damage);

        if (state->threat_hp <= 0) {
            state->threat_hp = 0;
            state->foes_vanquished++;
            state->moral_sway =
                math_clamp_int(state->moral_sway + 2, -100, 100);

            if (out_msg) {
                snprintf(out_msg, msg_len,
                    "Sword of Tenebris cleaved through enemy vanguard!");
            }
            printf("With a twilight arc, the Sword of Tenebris "
                   "severs the shadow fiend completely!\n");
        } else {
            if (out_msg) {
                snprintf(out_msg, msg_len,
                    "Struck with Sword of Tenebris (-%d HP). Foe at %d HP.",
                    damage, state->threat_hp);
            }
            printf("The obsidian blade whistles through the gloom, "
                   "slicing through enchanted armor!\n");
        }
    } else {
        /* Non-combat Hone/Sever */
        state->sword_edge =
            math_saturating_add(state->sword_edge, 15, MAX_RESONANCE);

        if (out_msg) {
            snprintf(out_msg, msg_len,
                "Sword of Tenebris attuned to the void (Edge: %d/100).",
                state->sword_edge);
        }
        printf("You trace the edge of the Sword of Tenebris. "
               "Cold abyssal mist sharpens the blade.\n");
    }
}

static void handle_balance(player_state_t *state, char *out_msg, size_t msg_len)
{
    state->shield_power =
        math_saturating_add(state->shield_power, 15, MAX_RESONANCE);
    state->sword_edge =
        math_saturating_add(state->sword_edge, 15, MAX_RESONANCE);

    /* Nudge moral sway toward cosmic center (0) */
    state->moral_sway = math_nudge_toward(state->moral_sway, 0, 2);

    if (state->threat_hp > 0) {
        state->threat_hp = math_saturating_sub(state->threat_hp, 20);
        if (state->threat_hp == 0) state->foes_vanquished++;

        if (out_msg) {
            snprintf(out_msg, msg_len,
                "Equilibrium Parry & Riposte! Foe (-20 HP). Relics balanced.");
        }
        printf("You parry with the Shield of Lux and riposte with the "
               "Sword of Tenebris in perfect synergy!\n");
    } else {
        if (out_msg) {
            snprintf(out_msg, msg_len,
                "Harmonized twin relics. Shield: %d/100 | Sword: %d/100.",
                state->shield_power, state->sword_edge);
        }
        printf("You cross the Shield and Sword before your chest. "
               "Golden light and twilight mist intertwine.\n");
    }
}

static void handle_stance(player_state_t *state, const char *arg,
    char *out_msg, size_t msg_len)
{
    if (arg == NULL || strlen(arg) == 0) {
        if (out_msg) {
            snprintf(out_msg, msg_len, "Current stance: %s.",
                game_get_stance_name(state->stance));
        }
        printf("Current stance: %s. Valid: warding, striking, "
               "equilibrium, neutral.\n",
               game_get_stance_name(state->stance));
        return;
    }

    if (strstr(arg, "ward") != NULL) {
        state->stance = STANCE_WARDING;
    } else if (strstr(arg, "strik") != NULL) {
        state->stance = STANCE_STRIKING;
    } else if (strstr(arg, "equil") != NULL || strstr(arg, "balance") != NULL) {
        state->stance = STANCE_EQUILIBRIUM;
    } else if (strstr(arg, "neut") != NULL) {
        state->stance = STANCE_NEUTRAL;
    } else {
        if (out_msg) snprintf(out_msg, msg_len, "Unknown stance '%s'.", arg);
        printf("Unknown stance '%s'. Valid: warding, striking, "
               "equilibrium, neutral.\n", arg);
        return;
    }

    if (out_msg) {
        snprintf(out_msg, msg_len, "Shifted stance to: %s.",
            game_get_stance_name(state->stance));
    }
    printf("You shift your footing. Active Stance: %s.\n",
        game_get_stance_name(state->stance));
}

static void handle_move(player_state_t *state, const char *direction,
    char *out_msg, size_t msg_len)
{
    if (direction == NULL || strlen(direction) == 0) {
        if (out_msg) {
            snprintf(out_msg, msg_len,
                     "Specify a direction (north, south, east, west).");
        }
        printf("Travel where? Specify a direction (e.g. 'move north').\n");
        return;
    }

    location_id_t next_loc = state->location;
    char dir = (char)tolower((unsigned char)direction[0]);

    switch (state->location) {
    case LOC_GARRISON_ALTAR:
        if (dir == 'n') next_loc = LOC_CITADEL_GATES;
        break;
    case LOC_CITADEL_GATES:
        if (dir == 's') next_loc = LOC_GARRISON_ALTAR;
        else if (dir == 'n') next_loc = LOC_DIVIDED_MARCHES;
        break;
    case LOC_DIVIDED_MARCHES:
        if (dir == 's') next_loc = LOC_CITADEL_GATES;
        else if (dir == 'e') next_loc = LOC_ABYSSAL_THRESHOLD;
        break;
    case LOC_ABYSSAL_THRESHOLD:
        if (dir == 'w') next_loc = LOC_DIVIDED_MARCHES;
        break;
    default:
        break;
    }

    if (next_loc == state->location) {
        if (out_msg) {
            snprintf(out_msg, msg_len, "Path blocked in that direction.");
        }
        printf("The path is impassable or contested in that direction.\n");
        return;
    }

    state->location = next_loc;

    /* Spawn location encounters */
    if (state->location == LOC_CITADEL_GATES && state->foes_vanquished == 0) {
        state->threat_hp = 50;
    } else if (state->location == LOC_ABYSSAL_THRESHOLD &&
               state->threat_hp == 0) {
        state->threat_hp = 90;
    }

    if (out_msg) {
        snprintf(out_msg, msg_len, "Marched into %s.",
                 location_names[next_loc]);
    }
    printf("You march forward across the Kingdom...\n");
    game_print_location(state);
}

void game_process_command(player_state_t *state, const char *raw_input,
    char *out_msg, size_t msg_len)
{
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

    if (strcmp(command, "quit") == 0 || strcmp(command, "exit") == 0) {
        state->is_running = false;
        if (out_msg) {
            snprintf(out_msg, msg_len, "Relinquishing armaments. Farewell.");
        }
        printf("You lower weapons and step back from frontline. Farewell.\n");
        return;
    }

    if (strcmp(command, "help") == 0) {
        if (out_msg) {
            snprintf(out_msg, msg_len,
                "Commands: ward, strike, balance, stance, look, status, move.");
        }
        game_print_help();
        return;
    }

    if (strcmp(command, "status") == 0) {
        if (out_msg) {
            snprintf(out_msg, msg_len,
                "Shield: %d | Sword: %d | Stance: %s | Sway: %+d",
                state->shield_power, state->sword_edge,
                game_get_stance_name(state->stance), state->moral_sway);
        }
        game_print_status(state);
        return;
    }

    if (strcmp(command, "look") == 0) {
        if (out_msg) {
            snprintf(out_msg, msg_len, "Surveying %s.",
                     location_names[state->location]);
        }
        game_print_location(state);
        return;
    }

    if (strcmp(command, "ward") == 0 || strcmp(command, "shield") == 0) {
        handle_ward(state, out_msg, msg_len);
    } else if (strcmp(command, "strike") == 0 ||
               strcmp(command, "slash") == 0) {
        handle_strike(state, out_msg, msg_len);
    } else if (strcmp(command, "balance") == 0 ||
               strcmp(command, "harmonize") == 0) {
        handle_balance(state, out_msg, msg_len);
    } else if (strcmp(command, "stance") == 0) {
        handle_stance(state, arg, out_msg, msg_len);
    } else if (strcmp(command, "move") == 0 || strcmp(command, "go") == 0) {
        handle_move(state, arg, out_msg, msg_len);
    } else {
        if (out_msg) {
            snprintf(out_msg, msg_len, "Unknown command '%s'.", command);
        }
        printf("Unknown command: '%s'. Type 'help' for options.\n", command);
        return;
    }

    state->turns_taken++;
}

void game_loop_cli(player_state_t *state)
{
    char input_line[MAX_INPUT_LEN];

    game_print_intro();
    game_print_location(state);

    while (state->is_running) {
        printf("relics [Shield:%d | Sword:%d]> ",
            state->shield_power, state->sword_edge);
        fflush(stdout);

        if (!fgets(input_line, sizeof(input_line), stdin)) {
            break;
        }

        input_line[strcspn(input_line, "\r\n")] = '\0';
        game_process_command(state, input_line, NULL, 0);
    }
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Tactical Simulation Core
// =============================================================================

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 4.1: SIMULATION INVARIANT AUDITS & PRE-FLIGHT ]
// Mission: Verify state bounds, stance determinism, and mathematical safety.
// -----------------------------------------------------------------------------

game_audit_report_t game_audit_state(const player_state_t *state)
{
    game_audit_report_t report;
    report.is_valid = true;
    report.error_count = 0;
    report.error_log[0] = '\0';

    if (state->shield_power < 0 || state->shield_power > MAX_RESONANCE) {
        report.is_valid = false;
        report.error_count++;
        snprintf(report.error_log + strlen(report.error_log),
            sizeof(report.error_log) - strlen(report.error_log),
            "[ERROR] Shield power (%d) outside bounds [0, %d]. ",
            state->shield_power, MAX_RESONANCE);
    }

    if (state->sword_edge < 0 || state->sword_edge > MAX_RESONANCE) {
        report.is_valid = false;
        report.error_count++;
        snprintf(report.error_log + strlen(report.error_log),
            sizeof(report.error_log) - strlen(report.error_log),
            "[ERROR] Sword edge (%d) outside bounds [0, %d]. ",
            state->sword_edge, MAX_RESONANCE);
    }

    if (state->moral_sway < -100 || state->moral_sway > 100) {
        report.is_valid = false;
        report.error_count++;
        snprintf(report.error_log + strlen(report.error_log),
            sizeof(report.error_log) - strlen(report.error_log),
            "[ERROR] Moral sway (%d) outside bounds [-100, +100]. ",
            state->moral_sway);
    }

    if (state->stance < 0 || state->stance >= STANCE_COUNT) {
        report.is_valid = false;
        report.error_count++;
        snprintf(report.error_log + strlen(report.error_log),
            sizeof(report.error_log) - strlen(report.error_log),
            "[ERROR] Stance (%d) outside valid range [0, %d). ",
            state->stance, STANCE_COUNT);
    }

    if (state->location < 0 || state->location >= LOC_COUNT) {
        report.is_valid = false;
        report.error_count++;
        snprintf(report.error_log + strlen(report.error_log),
            sizeof(report.error_log) - strlen(report.error_log),
            "[ERROR] Location (%d) outside valid bounds [0, %d). ",
            state->location, LOC_COUNT);
    }

    return report;
}

static const char *const game_invariants[] = {
    "Shield of Lux resonance bounded: [0, MAX_RESONANCE]",
    "Sword of Tenebris edge bounded: [0, MAX_RESONANCE]",
    "Moral sway continuum bounded: [-100, +100]",
    "Deterministic stance transitions & topology"
};

const module_metadata_t game_manifest = {
    "sim.engine.core",
    "game.h",
    LAYER_1_SIMULATION,
    { 1, 0, 0 },
    1,                          // interface_rev
    "2026-10-26",               // last_updated
    "Core simulation engine, twin relics resonance and stance machine",
    2,                          // dependency_count
    { "core.util.math", "core.util.string", NULL, NULL },
    ALLOC_ZERO_HEAP,            // alloc_policy
    CONCURRENCY_REENTRANT,      // concurrency
    50,                         // max_budget_us
    4,
    {
        game_invariants[0],
        game_invariants[1],
        game_invariants[2],
        game_invariants[3]
    },
    game_preflight
};

diag_result_t game_preflight(void)
{
    diag_result_t res;
    res.subsystem = game_manifest.module_id;
    res.check_name = "Relic Invariants & Stance Determinism";

    player_state_t test_state;
    game_init(&test_state);

    /* Test 1: Initial state invariant audit */
    game_audit_report_t report = game_audit_state(&test_state);
    if (!report.is_valid) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: Initial game state violated relic invariants.";
        return res;
    }

    /* Test 2: Bounded capacity saturation on twin relics */
    test_state.shield_power = 95;
    test_state.shield_power =
        math_saturating_add(test_state.shield_power, 15, MAX_RESONANCE);
    if (test_state.shield_power != MAX_RESONANCE) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details =
            "Fatal: Shield of Lux resonance exceeded maximum capacity.";
        return res;
    }

    test_state.sword_edge = 90;
    test_state.sword_edge =
        math_saturating_add(test_state.sword_edge, 20, MAX_RESONANCE);
    if (test_state.sword_edge != MAX_RESONANCE) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details =
            "Fatal: Sword of Tenebris edge exceeded maximum capacity.";
        return res;
    }

    /* Test 3: Stance state machine validity */
    test_state.stance = STANCE_STRIKING;
    const char *st_name = game_get_stance_name(test_state.stance);
    if (test_state.stance != STANCE_STRIKING ||
        strcmp(st_name, "Edge Striking (Sword Drawn)") != 0) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details =
            "Fatal: Stance state machine failed STANCE_STRIKING transition.";
        return res;
    }

    /* Test 4: Diagnostic tripwire on corrupt states */
    player_state_t corrupt = test_state;
    corrupt.moral_sway = 500;
    if (game_audit_state(&corrupt).is_valid) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details =
            "Fatal: Corrupt moral sway was not trapped by audit tripwire.";
        return res;
    }

    corrupt = test_state;
    corrupt.shield_power = -5;
    if (game_audit_state(&corrupt).is_valid) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details =
            "Fatal: Corrupt negative shield was not trapped by tripwire.";
        return res;
    }

    corrupt = test_state;
    corrupt.location = LOC_COUNT + 1;
    if (game_audit_state(&corrupt).is_valid) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details =
            "Fatal: Out-of-bounds location was not trapped by tripwire.";
        return res;
    }

    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.details =
        "Simulation invariants, stance transitions & bounds verified.";
    return res;
}

int game_run_self_audit(void)
{
    printf("============================================================\n");
    printf("   Tenebris and Lux - Relic & Stance Invariant Self-Audit   \n");
    printf("============================================================\n");

    player_state_t test_state;
    game_init(&test_state);

    game_audit_report_t report = game_audit_state(&test_state);
    if (!report.is_valid) {
        printf("[FAIL] Initial state audit failed: %s\n", report.error_log);
        return 1;
    }
    printf("[PASS] Test 1: Initial twin relic state verified.\n");

    /* Test 2: Bounded capacity enforcement on ward and strike */
    test_state.shield_power = 95;
    game_process_command(&test_state, "ward", NULL, 0);
    if (test_state.shield_power != MAX_RESONANCE) {
        printf("[FAIL] Shield power exceeded ceiling: %d\n",
               test_state.shield_power);
        return 1;
    }
    printf("[PASS] Test 2: Shield of Lux ceiling invariant verified.\n");

    /* Test 3: Stance transition state machine */
    game_process_command(&test_state, "stance striking", NULL, 0);
    if (test_state.stance != STANCE_STRIKING) {
        printf("[FAIL] Stance transition to striking failed.\n");
        return 1;
    }
    game_process_command(&test_state, "stance equilibrium", NULL, 0);
    if (test_state.stance != STANCE_EQUILIBRIUM) {
        printf("[FAIL] Stance transition to equilibrium failed.\n");
        return 1;
    }
    printf("[PASS] Test 3: Stance state machine transitions verified.\n");

    /* Test 4: Diagnostic tripwire on corrupt state */
    player_state_t corrupt_state = test_state;
    corrupt_state.moral_sway = 999;
    game_audit_report_t corrupt_report = game_audit_state(&corrupt_state);
    if (corrupt_report.is_valid) {
        printf("[FAIL] Corrupt moral sway was not caught by tripwire!\n");
        return 1;
    }
    printf("[PASS] Test 4: Diagnostic tripwire caught corrupt state.\n");

    return 0;
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( Teardown & Verification Helper )
// =============================================================================

// End of game.c
