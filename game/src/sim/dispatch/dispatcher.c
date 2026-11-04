// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          dispatcher.c
// @id            sim.engine.dispatcher
// @layer         Layer 1: Simulation Engine
// @subsystem     Command Dispatcher Subsystem (sim/dispatch)
// @version       2.0.0
// @interface_rev Rev 4
// @date          2026-11-03
// @updated       2026-11-03
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Master Command Orchestrator tokenizing input strings, resolving synonym
//   and shorthand aliases, classifying command domains (Combat, World, Items,
//   System), and dispatching execution to domain-specific handlers.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies:
//       sim/core/sim_types.h, sim/dispatch/dispatcher.h,
//       sim/dispatch/dispatch_combat.h, sim/dispatch/dispatch_world.h,
//       sim/dispatch/dispatch_items.h, sim/dispatch/dispatch_system.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic allocation).
//   - Concurrency / Safety:  REENTRANT_PURE (Pure reentrant command dispatch).
//   - Latency Budget:        < 50 microseconds per command dispatch.
//
// AXIOMATIC INVARIANTS:
//   1. Stack-only tokenization buffer; zero dynamic heap allocations.
//   2. Empty, whitespace-only, or unknown inputs produce no state mutations.
//   3. Alias resolution converges deterministically in 1 hop.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// =============================================================================

#include "sim/dispatch/dispatcher.h"
#include "sim/combat/combat.h"
#include "sim/world/world.h"
#include "util/log/log.h"
#include "util/string/string_utils.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

const module_metadata_t dispatcher_manifest = {
    "sim.engine.dispatcher",
    "dispatcher.h",
    LAYER_1_SIMULATION,
    { 2, 0, 0 },
    4,
    "2026-11-03",
    "Master Command Orchestrator, tokenization, alias map, & domain dispatch",
    4,
    { "sim.engine.combat", "sim.engine.world",
      "sim.engine.items", "sim.engine.state" },
    ALLOC_ZERO_HEAP,
    CONCURRENCY_REENTRANT,
    50,
    3,
    {
        "Stack-only tokenization buffer; zero dynamic heap allocations",
        "Empty, whitespace-only, or unknown inputs produce no state mutations",
        "Alias resolution converges deterministically in 1 hop"
    },
    dispatcher_preflight
};

static const alias_def_t ALIAS_TABLE[] = {
    {"strike",   "attack",    "cuttatio"},
    {"slash",    "attack",    ""},
    {"tenebris", "attack",    "cuttatio"},
    {"ward",     "defend",    "barrio"},
    {"shield",   "defend",    "barrio"},
    {"lux",      "defend",    "barrio"},
    {"duo",      "special",   "duo"},
    {"balance",  "special",   "duo"},
    {"magic",    "special",   "magic"},
    {"charm",    "special",   "charm"},
    {"go",       "move",      ""},
    {"north",    "move",      "north"},
    {"south",    "move",      "south"},
    {"east",     "move",      "east"},
    {"west",     "move",      "west"},
    {"n",        "move",      "north"},
    {"s",        "move",      "south"},
    {"e",        "move",      "east"},
    {"w",        "move",      "west"},
    {"inv",      "inventory", ""},
    {"item",     "items",     ""},
    {"exit",     "quit",      ""},
    {"?",        "help",      ""},
    {"h",        "help",      ""}
};

static const size_t ALIAS_COUNT =
    sizeof(ALIAS_TABLE) / sizeof(ALIAS_TABLE[0]);

// =============================================================================
// BLOCK 3: BODY BLOCK / ORCHESTRATION ENGINE
// =============================================================================

bool dispatcher_resolve_alias(const char *input_verb,
                              char *out_canonical, size_t can_len,
                              char *out_default_arg, size_t arg_len)
{
    if (!input_verb || !out_canonical) {
        return false;
    }
    for (size_t i = 0; i < ALIAS_COUNT; i++) {
        if (str_case_eq(input_verb, ALIAS_TABLE[i].alias)) {
            str_safe_copy(out_canonical, ALIAS_TABLE[i].canonical, can_len);
            if (out_default_arg && arg_len > 0) {
                str_safe_copy(out_default_arg, ALIAS_TABLE[i].default_arg,
                              arg_len);
            }
            return true;
        }
    }
    str_safe_copy(out_canonical, input_verb, can_len);
    return false;
}

dispatch_domain_t dispatcher_classify_verb(const char *verb)
{
    if (!verb || verb[0] == '\0') {
        return DOMAIN_UNKNOWN;
    }

    /* System Domain */
    if (str_case_eq(verb, "quit") || str_case_eq(verb, "exit") ||
        str_case_eq(verb, "help") || str_case_eq(verb, "status") ||
        str_case_eq(verb, "name") || str_case_eq(verb, "debug") ||
        str_case_eq(verb, "snap") || str_case_eq(verb, "rewind") ||
        str_case_eq(verb, "history")) {
        return DOMAIN_SYSTEM;
    }

    /* Combat Domain */
    if (str_case_eq(verb, "attack") || str_case_eq(verb, "cuttatio") ||
        str_case_eq(verb, "defend") || str_case_eq(verb, "barrio") ||
        str_case_eq(verb, "special") || str_case_eq(verb, "stance") ||
        str_case_eq(verb, "mode") || str_case_eq(verb, "flee")) {
        return DOMAIN_COMBAT;
    }

    /* World Domain */
    if (str_case_eq(verb, "move") || str_case_eq(verb, "look") ||
        str_case_eq(verb, "map") || str_case_eq(verb, "travel")) {
        return DOMAIN_WORLD;
    }

    /* Items Domain */
    if (str_case_eq(verb, "items") || str_case_eq(verb, "inventory") ||
        str_case_eq(verb, "use") || str_case_eq(verb, "inspect") ||
        str_case_eq(verb, "equip") || str_case_eq(verb, "clock") ||
        str_case_eq(verb, "morality")) {
        return DOMAIN_ITEMS;
    }

    return DOMAIN_UNKNOWN;
}

void dispatcher_process_command(player_state_t *state, const char *raw_input,
                                char *out_msg, size_t msg_len)
{
    if (state == NULL || raw_input == NULL) {
        return;
    }

    char buffer[MAX_DISPATCH_INPUT];
    str_safe_copy(buffer, raw_input, sizeof(buffer));
    str_trim_inplace(buffer);

    if (strlen(buffer) == 0) {
        return;
    }

    char *raw_verb = strtok(buffer, " ");
    if (raw_verb == NULL) {
        return;
    }
    char *raw_arg = strtok(NULL, "");
    if (raw_arg != NULL) {
        while (isspace((unsigned char)*raw_arg)) {
            raw_arg++;
        }
    }

    dispatch_context_t ctx;
    memset(&ctx, 0, sizeof(ctx));
    str_safe_copy(ctx.raw_input, raw_input, sizeof(ctx.raw_input));

    char default_arg[MAX_DISPATCH_ARG_LEN] = {0};
    dispatcher_resolve_alias(raw_verb, ctx.canonical_verb,
                             sizeof(ctx.canonical_verb),
                             default_arg, sizeof(default_arg));

    if (raw_arg && strlen(raw_arg) > 0) {
        str_safe_copy(ctx.arg, raw_arg, sizeof(ctx.arg));
    } else if (strlen(default_arg) > 0) {
        str_safe_copy(ctx.arg, default_arg, sizeof(ctx.arg));
    }

    ctx.domain = dispatcher_classify_verb(ctx.canonical_verb);

    LOG_DEBUG(
        LOG_CAT_DEBUG,
        "Dispatcher",
        log_percent_to_grade(state->moral_sway),
        "Routing command '%s' (Canonical: '%s', Domain: %d, Turn: %d).",
        raw_verb,
        ctx.canonical_verb,
        ctx.domain,
        state->turns_taken + 1
    );

    bool handled = false;
    switch (ctx.domain) {
        case DOMAIN_SYSTEM:
            handled = dispatch_system_handle(state, &ctx, out_msg, msg_len);
            break;
        case DOMAIN_COMBAT:
            handled = dispatch_combat_handle(state, &ctx, out_msg, msg_len);
            break;
        case DOMAIN_WORLD:
            handled = dispatch_world_handle(state, &ctx, out_msg, msg_len);
            break;
        case DOMAIN_ITEMS:
            handled = dispatch_items_handle(state, &ctx, out_msg, msg_len);
            break;
        case DOMAIN_UNKNOWN:
        default:
            break;
    }

    if (!handled) {
        if (out_msg) {
            snprintf(out_msg, msg_len,
                     "Unknown tactical command '%s'. Enter 'help' for manual.",
                     raw_verb);
        }
        printf("Unknown command '%s'. Enter 'help' for manual.\n",
               raw_verb);
    }
}

void dispatcher_print_intro(void)
{
    printf("\n==========================================================\n");
    printf("   TENEBRIS AND LUX: The Chronicle of Sacyra & The Veil   \n");
    printf("==========================================================\n");
    printf("You awaken within the ethereal sanctum of The Dreamscape.\n");
    printf("Shield of Lux gleams on arm; Sword of Tenebris rests at hand.\n");
    printf("Type 'help' for commands, or 'move north' to reach Dio.\n\n");
}

void dispatcher_print_status(const player_state_t *state)
{
    dispatch_context_t ctx;
    memset(&ctx, 0, sizeof(ctx));
    str_safe_copy(ctx.canonical_verb, "status", sizeof(ctx.canonical_verb));
    ctx.domain = DOMAIN_SYSTEM;
    dispatch_system_handle((player_state_t *)state, &ctx, NULL, 0);
}

void dispatcher_print_location(const player_state_t *state)
{
    dispatch_context_t ctx;
    memset(&ctx, 0, sizeof(ctx));
    str_safe_copy(ctx.canonical_verb, "look", sizeof(ctx.canonical_verb));
    ctx.domain = DOMAIN_WORLD;
    dispatch_world_handle((player_state_t *)state, &ctx, NULL, 0);
}

void dispatcher_print_help(void)
{
    dispatch_context_t ctx;
    memset(&ctx, 0, sizeof(ctx));
    str_safe_copy(ctx.canonical_verb, "help", sizeof(ctx.canonical_verb));
    ctx.domain = DOMAIN_SYSTEM;
    player_state_t dummy;
    memset(&dummy, 0, sizeof(dummy));
    dispatch_system_handle(&dummy, &ctx, NULL, 0);
}

void dispatcher_loop_cli(player_state_t *state)
{
    if (state == NULL) return;

    dispatcher_print_intro();
    char input_line[MAX_DISPATCH_INPUT];

    while (state->is_running) {
        printf("\n[%s | HP:%d/%d | Stance:%s] > ",
               world_get_location_name(state->location),
               state->hp, state->max_hp,
               combat_get_stance_name(state->stance));
        fflush(stdout);

        if (!fgets(input_line, sizeof(input_line), stdin)) {
            break;
        }
        dispatcher_process_command(state, input_line, NULL, 0);
    }
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// =============================================================================

diag_result_t dispatcher_preflight(void)
{
    diag_result_t res = {
        true,
        SEVERITY_PASS,
        "sim.engine.dispatcher",
        "dispatcher.c",
        "Dispatcher orchestrator, domain routers, and alias tables verified."
    };

    diag_result_t combat_res = dispatch_combat_preflight();
    if (!combat_res.passed) {
        return combat_res;
    }

    diag_result_t world_res = dispatch_world_preflight();
    if (!world_res.passed) {
        return world_res;
    }

    diag_result_t items_res = dispatch_items_preflight();
    if (!items_res.passed) {
        return items_res;
    }

    diag_result_t sys_res = dispatch_system_preflight();
    if (!sys_res.passed) {
        return sys_res;
    }

    diag_result_t load_res = dispatch_loader_preflight();
    if (load_res.severity == SEVERITY_CRITICAL ||
        load_res.severity == SEVERITY_FATAL) {
        return load_res;
    }

    char canonical[32] = {0};
    char def_arg[64] = {0};
    dispatcher_resolve_alias("strike", canonical, sizeof(canonical),
                             def_arg, sizeof(def_arg));
    if (strcmp(canonical, "attack") != 0 || strcmp(def_arg, "cuttatio") != 0) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "Alias resolution for 'strike' failed";
        return res;
    }

    return res;
}
