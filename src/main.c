// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          main.c
// @layer         Core Engine & Game Simulation
// @version       0.0.1
// @epoch         2026.10
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @role          Core Simulation Loop, Dual CLI/GUI Dispatcher & Audit Harness
// @invariants    Rank 1 (< >): Form precedes fullness.
//                1. Cognition precedes action: Input validation precedes state.
//                2. Finite energy: Lux attenuation is monotonic per action turn.
//                3. Diagnostic self-audit returns 0 on pure invariant passage.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / STATIC DATA
// ( System Includes, Static Lookups, Room World Definitions )
// =============================================================================

#include "main.h"
#include "gui.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *room_names[ROOM_COUNT] = {
    "The Fading Sanctuary",
    "The Corridor of Shadows",
    "The Hall of Whispers",
    "The Chamber of Ancient Runes"
};

static const char *room_descriptions[ROOM_COUNT] = {
    "A quiet stone haven where faint embers glow upon a cracked altar. "
    "The air is still.",
    "A long passageway draped in heavy gloom. Shadows flicker along the cold "
    "walls.",
    "A cavernous hall where distant murmurs echo, speaking in tongues forgotten.",
    "A sacred circular chamber carved with luminous sigils pulsing faintly."
};

static void trim_whitespace(char *str)
{
    char *end;

    while (isspace((unsigned char)*str)) {
        str++;
    }

    if (*str == '\0') {
        return;
    }

    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) {
        end--;
    }
    end[1] = '\0';
}

const char *game_get_room_name(room_id_t room)
{
    if (room >= 0 && room < ROOM_COUNT) {
        return room_names[room];
    }
    return "Unknown Void";
}

const char *game_get_room_desc(room_id_t room)
{
    if (room >= 0 && room < ROOM_COUNT) {
        return room_descriptions[room];
    }
    return "Shifting shadows obscure all physical features.";
}

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Core Simulation, Command Interpreter, State Transitions & Mechanics )
// =============================================================================

void game_init(player_state_t *state)
{
    state->lux = INITIAL_LUX;
    state->lumina_shards = 2;
    state->current_room = ROOM_SANCTUARY;
    state->is_running = true;
    state->turns_survived = 0;
}

void game_print_intro(void)
{
    printf("============================================================\n");
    printf("                  %s  [v%s]\n", APP_NAME, APP_VERSION);
    printf("               %s\n", APP_DESCRIPTION);
    printf("============================================================\n");
    printf("Balance your Lux (Light) against creeping Tenebris (Shadow).\n");
    printf("Type 'help' to view available commands.\n\n");
}

void game_print_status(const player_state_t *state)
{
    printf("\n--- Status ---\n");
    printf("Current Location: %s\n", room_names[state->current_room]);
    printf("Lux (Light Level): %d/%d [", state->lux, MAX_LUX);

    int bar_width = 20;
    int filled = (state->lux * bar_width) / MAX_LUX;
    for (int i = 0; i < bar_width; i++) {
        if (i < filled) {
            putchar('#');
        } else {
            putchar('.');
        }
    }
    printf("]\n");
    printf("Lumina Shards:    %d\n", state->lumina_shards);
    printf("Turns Survived:   %d\n", state->turns_survived);
    printf("--------------\n\n");
}

void game_print_room(const player_state_t *state)
{
    printf("\n[%s]\n", room_names[state->current_room]);
    printf("%s\n\n", room_descriptions[state->current_room]);
}

void game_print_help(void)
{
    printf("\n--- Available Commands ---\n");
    printf("  look       - Inspect your current surroundings\n");
    printf("  status     - Check light level and inventory\n");
    printf("  explore    - Scour the gloom for Lumina shards and ancient relics\n");
    printf("  kindle     - Consume 1 Lumina shard to restore 30 Lux\n");
    printf("  move <dir> - Travel: north, south, east, or west\n");
    printf("  help       - Display this command manual\n");
    printf("  quit       - Exit the game\n");
    printf("--------------------------\n\n");
}

static void handle_move(player_state_t *state, const char *arg,
    char *out_msg, size_t msg_len)
{
    if (arg == NULL || strlen(arg) == 0) {
        if (out_msg) {
            snprintf(out_msg, msg_len, "Specify a direction: north, south, east, west.");
        }
        printf("Move where? Specify a direction (e.g., 'move north').\n");
        return;
    }

    room_id_t next_room = state->current_room;

    if (strcmp(arg, "north") == 0 || strcmp(arg, "n") == 0) {
        if (state->current_room == ROOM_SANCTUARY) {
            next_room = ROOM_CORRIDOR_SHADOWS;
        } else if (state->current_room == ROOM_CORRIDOR_SHADOWS) {
            next_room = ROOM_HALL_WHISPERS;
        } else {
            if (out_msg) snprintf(out_msg, msg_len, "The path north is sealed by solid stone.");
            printf("The path north is sealed by solid stone.\n");
            return;
        }
    } else if (strcmp(arg, "south") == 0 || strcmp(arg, "s") == 0) {
        if (state->current_room == ROOM_HALL_WHISPERS) {
            next_room = ROOM_CORRIDOR_SHADOWS;
        } else if (state->current_room == ROOM_CORRIDOR_SHADOWS) {
            next_room = ROOM_SANCTUARY;
        } else {
            if (out_msg) snprintf(out_msg, msg_len, "The path south is consumed by darkness.");
            printf("The path south is consumed by impenetrable darkness.\n");
            return;
        }
    } else if (strcmp(arg, "east") == 0 || strcmp(arg, "e") == 0) {
        if (state->current_room == ROOM_CORRIDOR_SHADOWS) {
            next_room = ROOM_CHAMBER_RUNES;
        } else {
            if (out_msg) snprintf(out_msg, msg_len, "You cannot travel east from here.");
            printf("You cannot travel east from here.\n");
            return;
        }
    } else if (strcmp(arg, "west") == 0 || strcmp(arg, "w") == 0) {
        if (state->current_room == ROOM_CHAMBER_RUNES) {
            next_room = ROOM_CORRIDOR_SHADOWS;
        } else {
            if (out_msg) snprintf(out_msg, msg_len, "You cannot travel west from here.");
            printf("You cannot travel west from here.\n");
            return;
        }
    } else {
        if (out_msg) snprintf(out_msg, msg_len, "Unknown direction '%s'.", arg);
        printf("Unknown direction '%s'. Valid: north, south, east, west.\n", arg);
        return;
    }

    state->current_room = next_room;
    if (out_msg) {
        snprintf(out_msg, msg_len, "Moved into %s.", room_names[next_room]);
    }
    printf("You journey forward into the dark...\n");
    game_print_room(state);
}

static void handle_explore(player_state_t *state, char *out_msg, size_t msg_len)
{
    printf("You search cautiously through the gloom...\n");
    int roll = rand() % 100;

    if (roll < 45) {
        state->lumina_shards++;
        if (out_msg) {
            snprintf(out_msg, msg_len, "Discovered 1 Lumina Shard! (Total: %d)",
                state->lumina_shards);
        }
        printf("A glimmer catches your eye! Found 1 Lumina Shard (Total: %d).\n",
            state->lumina_shards);
    } else if (roll < 75) {
        if (out_msg) {
            snprintf(out_msg, msg_len, "Old inscriptions found, but no shards were uncovered.");
        }
        printf("Old inscriptions line the walls, but no relics were found.\n");
    } else {
        int drain = 5;
        state->lux = (state->lux > drain) ? (state->lux - drain) : 0;
        if (out_msg) {
            snprintf(out_msg, msg_len, "A cold shadow chill leeched %d Lux!", drain);
        }
        printf("A sudden cold chill sweeps past, leeching %d Lux!\n", drain);
    }
}

static void handle_kindle(player_state_t *state, char *out_msg, size_t msg_len)
{
    if (state->lumina_shards <= 0) {
        if (out_msg) {
            snprintf(out_msg, msg_len, "No Lumina shards remaining to kindle.");
        }
        printf("You have no Lumina shards left to kindle. Explore to find more.\n");
        return;
    }

    state->lumina_shards--;
    state->lux += KINDLE_RESTORE_AMOUNT;
    if (state->lux > MAX_LUX) {
        state->lux = MAX_LUX;
    }

    if (out_msg) {
        snprintf(out_msg, msg_len, "Kindled shard! Restored %d Lux (Now: %d/%d).",
            KINDLE_RESTORE_AMOUNT, state->lux, MAX_LUX);
    }
    printf("You crush a Lumina shard. Radiant warmth surges into your aura!\n");
    printf("Restored %d Lux (Current Lux: %d/%d).\n",
        KINDLE_RESTORE_AMOUNT, state->lux, MAX_LUX);
}

void game_process_command(player_state_t *state, const char *raw_input,
    char *out_msg, size_t msg_len)
{
    char buffer[MAX_INPUT_LEN];
    strncpy(buffer, raw_input, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    trim_whitespace(buffer);
    if (strlen(buffer) == 0) {
        return;
    }

    char *command = strtok(buffer, " ");
    char *arg = strtok(NULL, "");
    if (arg != NULL) {
        while (isspace((unsigned char)*arg)) {
            arg++;
        }
    }

    if (strcmp(command, "quit") == 0 || strcmp(command, "exit") == 0) {
        state->is_running = false;
        if (out_msg) snprintf(out_msg, msg_len, "Stepping back into the veil. Farewell.");
        printf("You step back into the veil. Farewell, traveler.\n");
        return;
    }

    if (strcmp(command, "help") == 0) {
        if (out_msg) snprintf(out_msg, msg_len, "Commands: look, explore, kindle, status, move.");
        game_print_help();
        return;
    }

    if (strcmp(command, "status") == 0) {
        if (out_msg) {
            snprintf(out_msg, msg_len, "Status: %d/%d Lux | %d Shards | Turn %d",
                state->lux, MAX_LUX, state->lumina_shards, state->turns_survived);
        }
        game_print_status(state);
        return;
    }

    if (strcmp(command, "look") == 0) {
        if (out_msg) {
            snprintf(out_msg, msg_len, "Observing %s.", room_names[state->current_room]);
        }
        game_print_room(state);
        return;
    }

    if (strcmp(command, "explore") == 0) {
        handle_explore(state, out_msg, msg_len);
    } else if (strcmp(command, "kindle") == 0) {
        handle_kindle(state, out_msg, msg_len);
    } else if (strcmp(command, "move") == 0 || strcmp(command, "go") == 0) {
        handle_move(state, arg, out_msg, msg_len);
    } else {
        if (out_msg) snprintf(out_msg, msg_len, "Unknown command '%s'.", command);
        printf("Unknown command: '%s'. Type 'help' for options.\n", command);
        return;
    }

    /* Action turn progression */
    state->turns_survived++;
    state->lux -= LUX_DECAY_RATE;

    if (state->lux <= 0) {
        state->lux = 0;
        if (out_msg) {
            snprintf(out_msg, msg_len, "The Lux died. Darkness claims you.");
        }
        printf("\n============================================================\n");
        printf("The last ember of your Lux has died.\n");
        printf("The cold tendrils of Tenebris encompass you completely...\n");
        printf("You survived %d turns before falling to the dark.\n",
            state->turns_survived);
        printf("============================================================\n");
        state->is_running = false;
    } else if (state->lux <= LOW_LUX_THRESHOLD) {
        printf("\n[WARNING] Your Lux is waning dangerously low (%d/%d)!\n",
            state->lux, MAX_LUX);
    }
}

void game_loop_cli(player_state_t *state)
{
    char input_line[MAX_INPUT_LEN];

    game_print_intro();
    game_print_room(state);

    while (state->is_running) {
        printf("lux:%d> ", state->lux);
        if (fgets(input_line, sizeof(input_line), stdin) == NULL) {
            printf("\n");
            break;
        }

        input_line[strcspn(input_line, "\r\n")] = '\0';
        game_process_command(state, input_line, NULL, 0);
    }
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// ( Diagnostic Audit Reports, Verification Tripwires & Telemetry )
// =============================================================================

game_audit_report_t game_audit_state(const player_state_t *state)
{
    game_audit_report_t report;
    report.is_valid = true;
    report.error_count = 0;
    report.error_log[0] = '\0';

    if (state == NULL) {
        report.is_valid = false;
        report.error_count++;
        snprintf(report.error_log, sizeof(report.error_log),
            "Audit Error: Player state pointer is NULL.");
        return report;
    }

    /* Invariant 1: Lux must be bounded in [0, MAX_LUX] */
    if (state->lux < 0 || state->lux > MAX_LUX) {
        report.is_valid = false;
        report.error_count++;
        snprintf(report.error_log + strlen(report.error_log),
            sizeof(report.error_log) - strlen(report.error_log),
            "Invariant Violation: Lux (%d) out of bounds [0, %d]. ",
            state->lux, MAX_LUX);
    }

    /* Invariant 2: Lumina shards must be non-negative */
    if (state->lumina_shards < 0) {
        report.is_valid = false;
        report.error_count++;
        snprintf(report.error_log + strlen(report.error_log),
            sizeof(report.error_log) - strlen(report.error_log),
            "Invariant Violation: Lumina shards (%d) is negative. ",
            state->lumina_shards);
    }

    /* Invariant 3: Room index must be valid */
    if (state->current_room < 0 || state->current_room >= ROOM_COUNT) {
        report.is_valid = false;
        report.error_count++;
        snprintf(report.error_log + strlen(report.error_log),
            sizeof(report.error_log) - strlen(report.error_log),
            "Invariant Violation: Room index (%d) out of range [0, %d). ",
            state->current_room, ROOM_COUNT);
    }

    /* Invariant 4: Turns survived must be non-negative */
    if (state->turns_survived < 0) {
        report.is_valid = false;
        report.error_count++;
        snprintf(report.error_log + strlen(report.error_log),
            sizeof(report.error_log) - strlen(report.error_log),
            "Invariant Violation: Turns survived (%d) is negative. ",
            state->turns_survived);
    }

    return report;
}

int game_run_self_audit(void)
{
    printf("============================================================\n");
    printf("         Tenebris and Lux - Subsystem Invariant Audit       \n");
    printf("============================================================\n");

    player_state_t test_state;
    game_init(&test_state);

    /* Test 1: Initial State Invariant Audit */
    game_audit_report_t r1 = game_audit_state(&test_state);
    if (!r1.is_valid) {
        fprintf(stderr, "[FAIL] Test 1: Initial state audit failed: %s\n",
            r1.error_log);
        return 1;
    }
    printf("[PASS] Test 1: Initial state invariants verified.\n");

    /* Test 2: Kindle Mechanic Bound Protection */
    test_state.lux = MAX_LUX;
    test_state.lumina_shards = 5;
    game_process_command(&test_state, "kindle", NULL, 0);
    if (test_state.lux > MAX_LUX) {
        fprintf(stderr, "[FAIL] Test 2: Lux exceeded MAX_LUX after kindle: %d\n",
            test_state.lux);
        return 1;
    }
    printf("[PASS] Test 2: Kindle ceiling invariant verified.\n");

    /* Test 3: Simulated Invariant Tripwire Check */
    player_state_t corrupt_state = test_state;
    corrupt_state.lux = -10;
    game_audit_report_t r3 = game_audit_state(&corrupt_state);
    if (r3.is_valid) {
        fprintf(stderr, "[FAIL] Test 3: Corrupt state was not detected!\n");
        return 1;
    }
    printf("[PASS] Test 3: Diagnostic tripwire successfully caught corrupt state.\n");

    /* Test 4: GUI Layout Invariant Audit */
    if (!gui_audit_layout()) {
        fprintf(stderr, "[FAIL] Test 4: GUI button layout bounds exceeded!\n");
        return 1;
    }
    printf("[PASS] Test 4: GUI viewport layout invariants verified.\n");

    printf("============================================================\n");
    printf("Audit Result: ALL INVARIANT CHECKS PASSED (4/4)\n");
    printf("============================================================\n");
    return 0;
}

// =============================================================================
// BLOCK 5: CLOSING / TEST & CLI BLOCK
// ( CLI/GUI Dispatcher, Argument Parsing & Teardown )
// =============================================================================

int main(int argc, char *argv[])
{
    bool force_cli = false;
    bool force_gui = false;

    if (argc > 1) {
        if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
            printf("Usage: tenebris-lux [options]\n\n");
            printf("Options:\n");
            printf("  -h, --help     Show this help message and exit\n");
            printf("  -v, --version  Show version information and exit\n");
            printf("  -a, --audit    Run internal invariant audit and exit\n");
            printf("  -c, --cli      Launch game in terminal CLI mode\n");
            printf("  -g, --gui      Launch game in native desktop GUI mode\n");
            return 0;
        }
        if (strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-v") == 0) {
            printf("%s v%s\n", APP_NAME, APP_VERSION);
            return 0;
        }
        if (strcmp(argv[1], "--audit") == 0 || strcmp(argv[1], "-a") == 0) {
            return game_run_self_audit();
        }
        if (strcmp(argv[1], "--cli") == 0 || strcmp(argv[1], "-c") == 0) {
            force_cli = true;
        } else if (strcmp(argv[1], "--gui") == 0 || strcmp(argv[1], "-g") == 0) {
            force_gui = true;
        } else {
            fprintf(stderr, "Unknown option: %s\nUse --help for usage.\n", argv[1]);
            return 1;
        }
    }

    player_state_t state;
    game_init(&state);

    if (force_cli) {
        game_loop_cli(&state);
        return 0;
    }

    if (force_gui) {
        if (!gui_is_available()) {
            fprintf(stderr, "Error: No graphical X11 display available for --gui.\n");
            return 1;
        }
        return gui_run(&state);
    }

    /* Default behavior: Launch native GUI if display available, otherwise CLI */
    if (gui_is_available()) {
        return gui_run(&state);
    }

    printf("[Notice] No graphical display detected. Launching in CLI mode.\n\n");
    game_loop_cli(&state);
    return 0;
}
