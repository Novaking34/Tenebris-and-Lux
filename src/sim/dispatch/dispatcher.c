// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          dispatcher.c
// @id            sim.engine.dispatcher
// @layer         Layer 1: Simulation Engine
// @subsystem     Command Dispatcher Subsystem (sim/dispatch)
// @version       1.2.0
// @interface_rev Rev 3
// @date          2026-11-03
// @updated       2026-11-03
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Command dispatcher core implementation. Translates player commands from
//   CLI and GUI frontends into state transitions across combat, inventory,
//   relic telemetries, and provincial movements.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies:
//       sim/dispatch/dispatcher.h
//       sim/combat/combat.h
//       sim/core/sim_state.h
//       sim/items/items.h
//       sim/items/inventory.h
//       sim/world/world.h
//       util/log/log.h
//       util/string/string_utils.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic allocation)
//   - Concurrency / Safety:  REENTRANT_PURE (Pure reentrant command dispatch)
//   - Latency Budget:        < 50 microseconds per command dispatch
//
// AXIOMATIC INVARIANTS:
//   1. Zero heap allocations: All parsing and tokenization is stack-buffered.
//   2. Empty inputs cause no state mutations.
//   3. Successfully executed actions increment turns_taken deterministically.
// =============================================================================

#include "sim/dispatch/dispatcher.h"
#include "sim/combat/combat.h"
#include "sim/core/game.h"
#include "sim/core/sim_state.h"
#include "sim/items/inventory.h"
#include "sim/items/items.h"
#include "sim/world/world.h"
#include "util/debug/debug.h"
#include "util/log/log.h"
#include "util/string/string_utils.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// =============================================================================
// BLOCK 2: OPENING BLOCK / STATIC DATA
// Stratum: Layer 1 (Simulation Engine) | Machine: Command Dispatch Core
// =============================================================================

static const char *const s_dispatcher_invariants[3] = {
    "Tokenization uses fixed stack buffer without dynamic allocation",
    "Empty or invalid command strings cause zero state mutations",
    "Command dispatches route deterministically to domain subsystems"
};

const module_metadata_t dispatcher_manifest = {
    "sim.engine.dispatcher",
    "dispatcher.h",
    LAYER_1_SIMULATION,
    { 1, 2, 0 },
    3,                          // interface_rev
    "2026-11-03",               // last_updated
    "Simulation command parsing, verb tokenization, & action routing",
    4,                          // dependency_count
    {
        "sim.engine.combat",
        "sim.engine.world",
        "core.util.string",
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

static const char *get_class_name(class_id_t class_id)
{
    switch (class_id) {
        case CLASS_WARRIOR:      return "Warrior";
        case CLASS_MAGE:         return "Mage";
        case CLASS_ROGUE:        return "Rogue";
        case CLASS_PRIEST:       return "Priest (Upgrade)";
        case CLASS_HERO:         return "Hero (Advanced)";
        case CLASS_SUMMONER:     return "Summoner";
        case CLASS_GUARD:        return "Royal Guard";
        case CLASS_WIND_SPIRIT:  return "Wind Spirit";
        case CLASS_PIXIE:        return "Pixie";
        case CLASS_FIRE_CHICKEN: return "Fire Chicken";
        default:                 return "Adventurer";
    }
}

static const char *get_relic_name(relic_affinity_t relic)
{
    switch (relic) {
        case RELIC_LUX:         return "Shield of Lux";
        case RELIC_TENEBRIS:    return "Sword of Tenebris";
        case RELIC_EQUILIBRIUM: return "Twin Relics Harmonized";
        default:                return "Unbound";
    }
}

void dispatcher_print_intro(void)
{
    printf("\n======================================================\n");
    printf("                  TENEBRIS AND LUX                    \n");
    printf("         The Chronicles of Light and Shadow           \n");
    printf("======================================================\n");
    printf("The Kingdom of Sacyra rests on an ancient Accord.\n");
    printf("You awaken within The Dreamscape, bound between\n");
    printf("the Shield of Lux and the Sword of Tenebris.\n\n");
    printf("Type 'help' for tactical commands, or 'look' to survey.\n");
    printf("======================================================\n\n");
}

void dispatcher_print_status(const player_state_t *state)
{
    if (state == NULL) return;

    printf("\n--- Protagonist Status ---\n");
    printf("Protagonist:   %s, %s\n", state->name, state->title);
    printf("Class:         %s (Tier: %s)\n",
           get_class_name(state->active_class),
           (state->active_class == CLASS_PRIEST) ? "Upgraded" : "Basic");
    if (state->former_tutelage[0] != '\0') {
        printf("Background:    Former %s\n", state->former_tutelage);
    }
    printf("Bound Relic:   %s\n", get_relic_name(state->relic));
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
    printf("  special <skill>      - Cast: magic, ward, charm, duo\n");
    printf("  stance <type>        - Shift: warding, striking, equilibrium, neutral\n");
    printf("  items / inventory    - Inspect player pouch & items\n");
    printf("  use <item_id|name>   - Consume draught or consult diegetic relic\n");
    printf("  inspect <id|name>    - Appraise item, catalyst, or relic properties\n");
    printf("  clock                - Check monotonic time via Grandma's Watch\n");
    printf("  morality             - Read party alignment on Compass of Morality\n");
    printf("  mode <turn|tact|act> - Switch battle paradigm\n");
    printf("  name <chosen_name>   - Custom hero designation (default: Elijah)\n");
    printf("  look                 - Survey current realm & terrain\n");
    printf("  status               - Inspect vitals, relic charges & buffs\n");
    printf("  move <n|s|e|w>       - Traverse the Dreamscape & Kingdom\n");
    printf("  help                 - Display this command manual\n");
    printf("  quit                 - Relinquish armaments and exit\n");
    printf("-------------------------------\n\n");
}

static uint32_t resolve_item_id_from_arg(const char *arg)
{
    if (!arg || arg[0] == '\0') {
        return 0;
    }
    /* Try numeric parse */
    char *endptr = NULL;
    long val = strtol(arg, &endptr, 10);
    if (endptr != arg && *endptr == '\0' && val > 0) {
        return (uint32_t)val;
    }
    /* Try name lookup in catalog */
    const item_def_t *def = items_find_by_name(arg);
    if (def) {
        return def->id;
    }
    return 0;
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
            snprintf(out_msg, msg_len, "Farewell, Bearer. The chronicles rest.");
        }
        printf("You sheath your relics. The chronicles fall dormant.\n");
        return;
    }

    if (strcmp(command, "help") == 0) {
        dispatcher_print_help();
        if (out_msg) snprintf(out_msg, msg_len, "Displayed tactical command manual.");
        return;
    }

    if (strcmp(command, "status") == 0) {
        dispatcher_print_status(state);
        if (out_msg) snprintf(out_msg, msg_len, "Displayed protagonist status card.");
        return;
    }

    if (strcmp(command, "look") == 0) {
        dispatcher_print_location(state);
        if (out_msg) snprintf(out_msg, msg_len, "Surveyed location: %s.",
                              world_get_location_name(state->location));
        return;
    }

    if (strcmp(command, "name") == 0) {
        sim_state_set_name(state, arg);
        if (out_msg) {
            snprintf(out_msg, msg_len, "Protagonist designated as '%s'.", state->name);
        }
        printf("Protagonist designated as '%s'.\n", state->name);
        return;
    }

    if (strcmp(command, "items") == 0 || strcmp(command, "inventory") == 0 ||
        strcmp(command, "item") == 0) {
        size_t cat_len = 0;
        const item_def_t *cat = items_get_catalog(&cat_len);
        char inv_buf[1024];
        inventory_format_listing(&state->inventory, cat, cat_len, inv_buf,
                                 sizeof(inv_buf));
        printf("%s\n", inv_buf);
        if (out_msg) {
            str_safe_copy(out_msg, inv_buf, msg_len);
        }
        return;
    }

    if (strcmp(command, "use") == 0) {
        if (!arg || arg[0] == '\0') {
            if (out_msg) snprintf(out_msg, msg_len, "Specify item to use: use <id|name>.");
            printf("Specify item to use. Syntax: use <id|name>\n");
            return;
        }
        uint32_t item_id = resolve_item_id_from_arg(arg);
        char use_msg[256];
        if (items_use(&state->inventory, item_id, state, use_msg, sizeof(use_msg))) {
            printf("%s\n", use_msg);
            if (out_msg) str_safe_copy(out_msg, use_msg, msg_len);
            state->turns_taken++;
        } else {
            printf("%s\n", use_msg);
            if (out_msg) str_safe_copy(out_msg, use_msg, msg_len);
        }
        return;
    }

    if (strcmp(command, "debug") == 0) {
        if (arg == NULL || strlen(arg) == 0 || strcmp(arg, "status") == 0) {
            printf("[Debugger Status] Active: %s | Profile: %s | "
                   "Snapshots: %zu / %zu\n",
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
            snprintf(out_msg, msg_len, "Debugger command '%s' processed.",
                     (arg ? arg : "status"));
        }
        return;
    }

    if (strcmp(command, "history") == 0) {
        debug_dump_snapshot_history();
        if (out_msg) {
            snprintf(out_msg, msg_len,
                     "Snapshot history displayed (%zu snapshots).",
                     debug_get_snapshot_count());
        }
        return;
    }

    if (strcmp(command, "snap") == 0) {
        uint64_t snap_id = game_record_debug_snapshot(state, "manual_snap", arg);
        printf("Recorded manual snapshot #%lu at Turn %d.\n",
               (unsigned long)snap_id, state->turns_taken);
        if (out_msg) {
            snprintf(out_msg, msg_len, "Snapshot #%lu recorded.",
                     (unsigned long)snap_id);
        }
        return;
    }

    if (strcmp(command, "rewind") == 0) {
        if (arg == NULL || strlen(arg) == 0) {
            printf("Usage: rewind <snapshot_index>\n");
            if (out_msg) {
                snprintf(out_msg, msg_len, "Rewind requires snapshot index.");
            }
            return;
        }
        size_t idx = (size_t)atoi(arg);
        if (game_rewind_debug_snapshot(idx, state)) {
            printf("Successfully rewound to snapshot #%zu (Turn %d).\n",
                   idx, state->turns_taken);
            if (out_msg) {
                snprintf(out_msg, msg_len, "Rewound to snapshot #%zu.", idx);
            }
        } else {
            printf("Rewind failed: invalid snapshot index %zu (Total: %zu).\n",
                   idx, debug_get_snapshot_count());
            if (out_msg) {
                snprintf(out_msg, msg_len, "Rewind failed: invalid index.");
            }
        }
        return;
    }

    if (strcmp(command, "class") == 0) {
        if (arg == NULL || strlen(arg) == 0) {
            printf("Current Combat Class: %s\n",
                   get_class_name(state->active_class));
            printf("Available Basic Classes: warrior, mage, rogue\n");
            printf("First Upgraded Class:   priest (Unlocked Upgrade)\n");
            if (out_msg) {
                snprintf(out_msg, msg_len, "Current class: %s",
                         get_class_name(state->active_class));
            }
            return;
        }

        if (str_case_eq(arg, "warrior")) {
            state->active_class = CLASS_WARRIOR;
            state->movement_points = 4;
            printf("Switched Combat Class to: Warrior [Basic Tier - Martial Frontline]\n");
            if (out_msg) snprintf(out_msg, msg_len, "Class shifted to Warrior.");
        } else if (str_case_eq(arg, "mage")) {
            state->active_class = CLASS_MAGE;
            state->movement_points = 4;
            printf("Switched Combat Class to: Mage [Basic Tier - Planar Channeling]\n");
            if (out_msg) snprintf(out_msg, msg_len, "Class shifted to Mage.");
        } else if (str_case_eq(arg, "rogue") || str_case_eq(arg, "thief")) {
            state->active_class = CLASS_ROGUE;
            state->movement_points = 5;
            printf("Switched Combat Class to: Rogue [Basic Tier - Mobility & Flanking]\n");
            if (out_msg) snprintf(out_msg, msg_len, "Class shifted to Rogue.");
        } else if (str_case_eq(arg, "priest")) {
            state->active_class = CLASS_PRIEST;
            state->movement_points = 4;
            printf("Promoted Combat Class to: Priest [Upgraded Tier 1 - Warding & Restoration]\n");
            if (out_msg) snprintf(out_msg, msg_len, "Class promoted to Priest.");
        } else {
            printf("Unrecognized class '%s'. Available: warrior, mage, rogue, priest\n", arg);
            if (out_msg) snprintf(out_msg, msg_len, "Unknown class '%s'.", arg);
        }
        return;
    }

    if (strcmp(command, "inspect") == 0) {
        uint32_t item_id = 0;
        if (arg && arg[0] != '\0' && strcmp(arg, "state") != 0) {
            item_id = resolve_item_id_from_arg(arg);
        }
        if (item_id > 0) {
            char insp_msg[512];
            if (items_inspect(&state->inventory, item_id, state, insp_msg,
                              sizeof(insp_msg))) {
                printf("%s\n", insp_msg);
                if (out_msg) str_safe_copy(out_msg, insp_msg, msg_len);
            } else {
                printf("%s\n", insp_msg);
                if (out_msg) str_safe_copy(out_msg, insp_msg, msg_len);
            }
            return;
        }

        /* Debug state inspection fallback */
        debug_state_fields_t fields;
        game_state_to_debug_fields(state, &fields);
        debug_dump_state(&fields);
        if (out_msg) {
            snprintf(out_msg, msg_len, "State inspected.");
        }
        return;
    }

    if (strcmp(command, "clock") == 0) {
        char clock_msg[256];
        const item_def_t *watch = items_find_def(5); /* Grandma's Watch */
        relics_inspect_clock(watch, clock_msg, sizeof(clock_msg));
        printf("%s\n", clock_msg);
        if (out_msg) str_safe_copy(out_msg, clock_msg, msg_len);
        return;
    }

    if (strcmp(command, "morality") == 0 || strcmp(command, "compass") == 0) {
        char sway_msg[256];
        const item_def_t *mc = items_find_def(8); /* Compass of Morality */
        relics_read_moral_compass(mc, (int8_t)state->moral_sway, sway_msg,
                                  sizeof(sway_msg));
        printf("%s\n", sway_msg);
        if (out_msg) str_safe_copy(out_msg, sway_msg, msg_len);
        return;
    }

    if (strcmp(command, "mode") == 0) {
        if (arg && (strcmp(arg, "tactical") == 0 || strcmp(arg, "trpg") == 0)) {
            combat_set_battle_mode(BATTLE_MODE_TACTICAL);
            if (out_msg) {
                snprintf(out_msg, msg_len,
                         "Battle paradigm switched to Tactical Grid (TRPG).");
            }
            printf("Battle paradigm switched to Tactical Grid (TRPG).\n");
        } else if (arg && (strcmp(arg, "action") == 0 ||
                           strcmp(arg, "realtime") == 0)) {
            combat_set_battle_mode(BATTLE_MODE_ACTION);
            if (out_msg) {
                snprintf(out_msg, msg_len,
                         "Battle paradigm switched to Cinematic Action.");
            }
            printf("Battle paradigm switched to Cinematic Action.\n");
        } else {
            combat_set_battle_mode(BATTLE_MODE_TURN_BASED);
            if (out_msg) {
                snprintf(out_msg, msg_len,
                         "Battle paradigm switched to Traditional Turn-Based.");
            }
            printf("Battle paradigm switched to Traditional Turn-Based.\n");
        }
        return;
    }

    /* Defend / Ward / Shield / Barrio / Lux */
    if (strcmp(command, "defend") == 0 || strcmp(command, "ward") == 0 ||
        strcmp(command, "shield") == 0 || strcmp(command, "barrio") == 0 ||
        strcmp(command, "lux") == 0) {
        bool barrio = (strcmp(command, "barrio") == 0 ||
                       strcmp(command, "lux") == 0 ||
                       (arg != NULL && (strstr(arg, "barrio") != NULL ||
                                        strstr(arg, "lux") != NULL)));
        combat_handle_defend(state, barrio, out_msg, msg_len);
        state->turns_taken++;
        if (state->threat_hp > 0) {
            combat_resolve_enemy_turn(state);
        }
        return;
    }

    /* Attack / Strike / Slash / Cuttatio / Tenebris */
    if (strcmp(command, "attack") == 0 || strcmp(command, "strike") == 0 ||
        strcmp(command, "slash") == 0 || strcmp(command, "cuttatio") == 0 ||
        strcmp(command, "tenebris") == 0) {
        bool cuttatio = (strcmp(command, "cuttatio") == 0 ||
                         strcmp(command, "tenebris") == 0 ||
                         (arg != NULL && (strstr(arg, "cuttatio") != NULL ||
                                          strstr(arg, "tenebris") != NULL)));
        combat_handle_attack(state, cuttatio, out_msg, msg_len);
        state->turns_taken++;
        if (state->threat_hp > 0) {
            combat_resolve_enemy_turn(state);
        }
        return;
    }

    /* Special / Balance / Magic / Charm / Duo / Duo-Loca */
    if (strcmp(command, "special") == 0 || strcmp(command, "balance") == 0 ||
        strcmp(command, "magic") == 0 || strcmp(command, "charm") == 0 ||
        strcmp(command, "duo") == 0 || strcmp(command, "duo-loca") == 0) {
        special_skill_t sk = SPECIAL_DUO_LOCA;
        if (strcmp(command, "magic") == 0) {
            sk = SPECIAL_BASIC_MAGIC;
        } else if (strcmp(command, "charm") == 0) {
            sk = SPECIAL_BASIC_CHARM;
        } else if (strcmp(command, "balance") == 0 ||
                   strcmp(command, "duo") == 0 ||
                   strcmp(command, "duo-loca") == 0) {
            sk = SPECIAL_DUO_LOCA;
        } else if (arg != NULL && strlen(arg) > 0) {
            if (strstr(arg, "mag") != NULL) {
                sk = SPECIAL_BASIC_MAGIC;
            } else if (strstr(arg, "ward") != NULL) {
                sk = SPECIAL_BASIC_WARD;
            } else if (strstr(arg, "charm") != NULL) {
                sk = SPECIAL_BASIC_CHARM;
            } else {
                sk = SPECIAL_DUO_LOCA;
            }
        }
        combat_handle_special(state, sk, out_msg, msg_len);
        state->turns_taken++;
        if (state->threat_hp > 0) {
            combat_resolve_enemy_turn(state);
        }
        return;
    }

    if (strcmp(command, "stance") == 0) {
        if (arg == NULL) {
            if (out_msg) {
                snprintf(out_msg, msg_len,
                         "Specify stance: neutral, warding, striking, equilibrium.");
            }
            printf("Specify stance: neutral, warding, striking, equilibrium.\n");
            return;
        }
        combat_handle_stance(state, arg, out_msg, msg_len);
        state->turns_taken++;
        return;
    }

    if (strcmp(command, "move") == 0 || strcmp(command, "go") == 0) {
        if (arg == NULL) {
            if (out_msg) {
                snprintf(out_msg, msg_len,
                         "Specify direction: north, south, east, west, or awaken.");
            }
            printf("Specify direction: north, south, east, west, or awaken.\n");
            return;
        }
        if (world_handle_move(state, arg, out_msg, msg_len)) {
            state->turns_taken++;
        }
        return;
    }

    if (out_msg) {
        snprintf(out_msg, msg_len, "Unknown tactical command '%s'. Type 'help'.", command);
    }
    printf("Unknown tactical command '%s'. Type 'help' for command listing.\n", command);
}

void dispatcher_loop_cli(player_state_t *state)
{
    if (state == NULL) return;

    dispatcher_print_intro();
    dispatcher_print_location(state);

    char input[MAX_INPUT_LEN];
    char msg[256];

    while (state->is_running) {
        printf("\n[%s | HP:%d | Lux:%d | Teneb:%d | Sway:%+d] > ",
               state->name, state->hp, state->shield_power,
               state->sword_edge, state->moral_sway);
        fflush(stdout);

        if (!fgets(input, sizeof(input), stdin)) {
            break;
        }

        msg[0] = '\0';
        dispatcher_process_command(state, input, msg, sizeof(msg));
    }
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Command Dispatch Core
// =============================================================================

diag_result_t dispatcher_preflight(void)
{
    diag_result_t res;
    memset(&res, 0, sizeof(res));

    player_state_t test;
    memset(&test, 0, sizeof(test));
    sim_state_init(&test);

    /* Test 1: Empty command does not increment turns */
    int t0 = test.turns_taken;
    dispatcher_process_command(&test, "   ", NULL, 0);
    if (test.turns_taken != t0) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Empty command mutated turn counter.";
        return res;
    }

    /* Test 2: Defend command increments turns */
    dispatcher_process_command(&test, "defend", NULL, 0);
    if (test.turns_taken != t0 + 1) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Defend command failed to increment turn counter.";
        return res;
    }

    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.details = "Command tokenization and action dispatch verified.";
    return res;
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================
