// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          dispatch_items.c
// @id            sim.dispatch.items
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
//   Specialized domain dispatcher for item management, pouch inspection,
//   potion consumption, catalyst equipping, and diegetic relic appraisal.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: sim/dispatch/dispatch_items.h, sim/items/items.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic allocation).
//   - Concurrency / Safety:  REENTRANT_PURE (Pure reentrant state evaluation).
//   - Latency Budget:        < 25 microseconds per item action.
//
// AXIOMATIC INVARIANTS:
//   1. Only evaluates actions when state != NULL and ctx != NULL.
//   2. Item consumption safely verifies inventory counts before depletion.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// =============================================================================

#include "dispatch/dispatch_items.h"
#include "hero/sim_state.h"
#include "relics/items.h"
#include "core/string_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const module_metadata_t dispatch_items_manifest = {
    "sim.dispatch.items",
    "dispatch_items.h",
    LAYER_1_SIMULATION,
    { 1, 0, 0 },
    1,
    "2026-11-03",
    "Item and inventory command dispatcher, potion, and catalyst router",
    1,
    { "sim.engine.items", NULL, NULL, NULL },
    ALLOC_ZERO_HEAP,
    CONCURRENCY_REENTRANT,
    25,
    2,
    {
        "Evaluates only verbs assigned to DOMAIN_ITEMS",
        "Item consumption safely verifies inventory counts before depletion"
    },
    dispatch_items_preflight
};

static uint32_t resolve_item_id_from_arg(const char *arg)
{
    if (!arg || arg[0] == '\0') {
        return 0;
    }
    char *endptr = NULL;
    long val = strtol(arg, &endptr, 10);
    if (endptr != arg && *endptr == '\0' && val > 0) {
        return (uint32_t)val;
    }
    const item_def_t *def = items_find_by_name(arg);
    if (def) {
        return def->id;
    }
    return 0;
}

// =============================================================================
// BLOCK 3: BODY BLOCK / API IMPLEMENTATIONS
// =============================================================================

bool dispatch_items_handle(player_state_t *state,
                           const dispatch_context_t *ctx,
                           char *out_msg, size_t msg_len)
{
    if (!state || !ctx) {
        return false;
    }

    const char *verb = ctx->canonical_verb;
    const char *arg = ctx->arg;

    /* 1. Inventory / Pouch Listing */
    if (str_case_eq(verb, "items") || str_case_eq(verb, "inventory") ||
        str_case_eq(verb, "item")) {
        size_t cat_len = 0;
        const item_def_t *cat = items_get_catalog(&cat_len);
        char inv_buf[1024];
        inventory_format_listing(&state->inventory, cat, cat_len, inv_buf,
                                 sizeof(inv_buf));
        printf("%s\n", inv_buf);
        if (out_msg) {
            str_safe_copy(out_msg, inv_buf, msg_len);
        }
        return true;
    }

    /* 2. Use Item */
    if (str_case_eq(verb, "use")) {
        if (!arg || strlen(arg) == 0) {
            if (out_msg) {
                snprintf(out_msg, msg_len,
                         "Specify item to use. E.g. 'use 1' or 'use potion'.");
            }
            return true;
        }

        uint32_t item_id = resolve_item_id_from_arg(arg);
        if (item_id == 0) {
            if (out_msg) {
                snprintf(out_msg, msg_len,
                         "Unknown item '%s'. Check 'items' for catalog.", arg);
            }
            return true;
        }

        bool used = items_use(&state->inventory, item_id, state,
                              out_msg, msg_len);
        if (used) {
            state->turns_taken++;
            printf("%s\n", out_msg ? out_msg : "Item consumed.");
        } else {
            printf("%s\n", out_msg ? out_msg : "Could not use item.");
        }
        return true;
    }

    /* 3. Inspect Item / Relic / Catalyst */
    if (str_case_eq(verb, "inspect")) {
        if (!arg || strlen(arg) == 0) {
            if (out_msg) {
                snprintf(out_msg, msg_len,
                         "Specify item to inspect. E.g. 'inspect watch'.");
            }
            return true;
        }

        uint32_t item_id = resolve_item_id_from_arg(arg);
        if (item_id == 0) {
            if (out_msg) {
                snprintf(out_msg, msg_len,
                         "Cannot appraise '%s': not cataloged.", arg);
            }
            return true;
        }

        items_inspect(&state->inventory, item_id, state, out_msg, msg_len);
        printf("%s\n", out_msg ? out_msg : "");
        return true;
    }

    /* 4. Equip Catalyst */
    if (str_case_eq(verb, "equip")) {
        if (!arg || strlen(arg) == 0) {
            if (out_msg) {
                snprintf(out_msg, msg_len,
                         "Specify catalyst to equip. E.g. 'equip medusa'.");
            }
            return true;
        }

        bool eq = false;
        if (str_case_eq(arg, "medusa") || str_case_eq(arg, "medusas_sword")) {
            state->has_medusas_sword = true;
            eq = true;
            if (out_msg) snprintf(out_msg, msg_len, "Equipped Medusa's Sword.");
        } else if (str_case_eq(arg, "fire") ||
                   str_case_eq(arg, "ring_of_fire")) {
            state->has_ring_of_fire = true;
            eq = true;
            if (out_msg) snprintf(out_msg, msg_len, "Equipped Ring of Fire.");
        } else if (str_case_eq(arg, "heart") ||
                   str_case_eq(arg, "heart_shield")) {
            state->has_heart_shield = true;
            eq = true;
            if (out_msg) snprintf(out_msg, msg_len, "Equipped Heart Shield.");
        } else if (str_case_eq(arg, "lyri") ||
                   str_case_eq(arg, "lyris_offering")) {
            state->has_lyris_offering = true;
            eq = true;
            if (out_msg) {
                snprintf(out_msg, msg_len, "Equipped Lyri's Offering.");
            }
        } else {
            if (out_msg) {
                snprintf(out_msg, msg_len, "Unknown catalyst '%s'.", arg);
            }
        }
        printf("%s\n", out_msg ? out_msg : (eq ? "Equipped." : "Failed."));
        return true;
    }

    /* 5. Clock / Grandma's Watch */
    if (str_case_eq(verb, "clock")) {
        items_inspect(&state->inventory, 100, state, out_msg, msg_len);
        printf("%s\n", out_msg ? out_msg : "");
        return true;
    }

    /* 6. Morality / Compass of Morality */
    if (str_case_eq(verb, "morality")) {
        items_inspect(&state->inventory, 102, state, out_msg, msg_len);
        printf("%s\n", out_msg ? out_msg : "");
        return true;
    }

    return false;
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// =============================================================================

diag_result_t dispatch_items_preflight(void)
{
    diag_result_t res = {
        true,
        SEVERITY_PASS,
        "sim.dispatch.items",
        "dispatch_items.c",
        "Items domain dispatcher and inventory queries verified."
    };

    player_state_t test_state;
    memset(&test_state, 0, sizeof(test_state));
    sim_state_init(&test_state);

    dispatch_context_t ctx;
    memset(&ctx, 0, sizeof(ctx));
    str_safe_copy(ctx.canonical_verb, "items", sizeof(ctx.canonical_verb));
    ctx.domain = DOMAIN_ITEMS;

    char msg[1024];
    bool ok = dispatch_items_handle(&test_state, &ctx, msg, sizeof(msg));
    if (!ok || strlen(msg) == 0) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "Items dispatcher failed to format inventory listing";
        return res;
    }

    return res;
}
