// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          relics.c
// @id            sim.items.relics
// @layer         Layer 1: Simulation Engine
// @subsystem     Item, Relic & Inventory Subsystem (sim/items)
// @version       1.1.0
// @interface_rev Rev 2
// @date          2026-11-03
// @updated       2026-11-03
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Domain handlers for diegetic relics. Implements monotonic clock inspection
//   (Grandma's Watch), nocturnal astral window detection (The Dream Compass),
//   moral sway telemetry (Compass of Morality), and lore compendium unlocks.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies:
//       sim/items/relics.h
//       util/time/time_utils.h
//       util/string/string_utils.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic heap allocations).
//   - Concurrency / Safety:  REENTRANT_PURE (Read-only inspection).
//   - Latency Budget:        < 20 microseconds.
//
// AXIOMATIC INVARIANTS:
//   1. Nocturnal gating: Dream Compass active strictly between 20:00 and 04:00.
//   2. Moral sway bounds: Compass of Morality accurately maps [-100, +100].
//   3. Non-consumable: Relics are never depleted or consumed on use.
// =============================================================================

#include "relics/relics.h"
#include "core/string_utils.h"
#include "core/time_utils.h"
#include <stdio.h>
#include <string.h>

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Relic Domain Handler
// =============================================================================

static const char *const s_relics_invariants[3] = {
    "Nocturnal gating: Dream Compass active strictly between 20:00 and 04:00",
    "Moral sway bounds: Compass of Morality accurately maps [-100, +100]",
    "Non-consumable: Relics are never depleted or consumed on use"
};

const module_metadata_t relics_manifest = {
    "sim.items.relics",
    "relics.h",
    LAYER_1_SIMULATION,
    { 1, 1, 0 },
    2,
    "2026-11-03",
    "Diegetic key relics handler and telemetries",
    2,
    { "core.util.time", "core.util.string", NULL, NULL },
    ALLOC_ZERO_HEAP,
    CONCURRENCY_REENTRANT,
    20,
    3,
    {
        s_relics_invariants[0],
        s_relics_invariants[1],
        s_relics_invariants[2]
    },
    relics_preflight
};

// =============================================================================
// BLOCK 3: BODY BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Relic Domain Handler
// =============================================================================

bool relics_inspect_clock(const item_def_t *relic,
                          char *out_buf,
                          size_t buf_len)
{
    if (!relic || !out_buf || buf_len == 0) {
        return false;
    }

    if (!relic->data.relic.clock_interface) {
        snprintf(out_buf, buf_len,
                 "[RELIC-CLOCK] %s does not feature a clock interface.",
                 relic->name);
        return true;
    }

    char ts[64];
    time_get_iso_timestamp(ts, sizeof(ts));

    snprintf(out_buf, buf_len,
             "[GRANDMA'S WATCH] Monotonic Time: %s | Dial: Steady ticking. "
             "The gears keep perfect time unyielding to planar drift.",
             ts);
    return true;
}

bool relics_dream_compass_is_active(const item_def_t *relic, uint8_t hour)
{
    if (!relic || !relic->data.relic.planar_key) {
        return false;
    }

    /* Active window: 20:00 to 04:00 */
    return (hour >= 20 || hour < 4);
}

bool relics_read_moral_compass(const item_def_t *relic,
                               int8_t moral_sway,
                               char *out_buf,
                               size_t buf_len)
{
    if (!relic || !out_buf || buf_len == 0) {
        return false;
    }

    const char *alignment_str = "Equilibrium (Neutral)";
    const char *flavor_str = "The needle rests in calm stillness.";

    if (moral_sway >= 75) {
        alignment_str = "Radiant Polarization (Virtue / Mercy)";
        flavor_str = "The needle pulses with a brilliant crystal light.";
    } else if (moral_sway <= -75) {
        alignment_str = "Abyssal Polarization (Severance / Dominion)";
        flavor_str = "The needle is shrouded in a creeping, cold shadow.";
    } else if (moral_sway > 25) {
        alignment_str = "Altruistic Drift";
        flavor_str = "The needle leans toward the radiant crest.";
    } else if (moral_sway < -25) {
        alignment_str = "Ruthless Drift";
        flavor_str = "The needle tilts into the shadowy trough.";
    } else {
        alignment_str = "Harmonic Equilibrium (Bystander Corridor)";
        flavor_str = "The needle wavers gently, balanced between light & dark.";
    }

    snprintf(out_buf, buf_len,
             "[COMPASS OF MORALITY] Sway: %+d/100 | Alignment: %s\n"
             "Telemetry: %s",
             (int)moral_sway, alignment_str, flavor_str);
    return true;
}

bool relics_inspect(const item_def_t *relic, char *out_buf, size_t buf_len)
{
    if (!relic || !out_buf || buf_len == 0) {
        return false;
    }

    switch (relic->data.relic.sub_type) {
        case RELIC_TYPE_TEMPORAL:
            return relics_inspect_clock(relic, out_buf, buf_len);

        case RELIC_TYPE_TELEMETRY:
            return relics_read_moral_compass(relic, 0, out_buf, buf_len);

        case RELIC_TYPE_PLANAR: {
            uint8_t cur_hour = 21; /* Mocked current nocturnal hour */
            bool active = relics_dream_compass_is_active(relic, cur_hour);
            snprintf(out_buf, buf_len,
                     "[THE DREAM COMPASS] Status: %s\n"
                     "Description: %s\n"
                     "Window: Active 20:00 - 04:00 (Current Hour: %02u:00).",
                     active ? "RESONATING (ASTRAL RIFT OPEN)" : "DORMANT",
                     relic->description, (unsigned)cur_hour);
            return true;
        }

        case RELIC_TYPE_ARCHIVAL:
            snprintf(out_buf, buf_len,
                     "[%s] Archival Compendium\n"
                     "Description: %s\n"
                     "Telemetry: Grants immediate tactical encyclopedia access.",
                     relic->name, relic->description);
            return true;

        case RELIC_TYPE_DIPLOMATIC:
            snprintf(out_buf, buf_len,
                     "[%s] Diplomatic Relic\n"
                     "Description: %s\n"
                     "Sovereignty Clause: 'Joint Realm, Separate Kingdoms'.",
                     relic->name, relic->description);
            return true;

        default:
            snprintf(out_buf, buf_len, "[%s] %s", relic->name,
                     relic->description);
            return true;
    }
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Relic Domain Handler
// =============================================================================

diag_result_t relics_preflight(void)
{
    diag_result_t res;
    res.subsystem = relics_manifest.module_id;
    res.check_name = "Relic Nocturnal Window and Morality Invariants";
    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.details = "Nocturnal window and moral compass telemetry verified.";

    /* Test 1: Nocturnal window logic */
    item_def_t dc;
    memset(&dc, 0, sizeof(dc));
    dc.data.relic.planar_key = true;

    if (!relics_dream_compass_is_active(&dc, 22)) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Dream compass failed to activate at hour 22.";
        return res;
    }
    if (!relics_dream_compass_is_active(&dc, 2)) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Dream compass failed to activate at hour 02.";
        return res;
    }
    if (relics_dream_compass_is_active(&dc, 12)) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Dream compass active during daylight hour 12.";
        return res;
    }

    /* Test 2: Moral telemetry formatting */
    item_def_t mc;
    memset(&mc, 0, sizeof(mc));
    char buf[256];
    if (!relics_read_moral_compass(&mc, 80, buf, sizeof(buf))) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Moral compass formatting failed.";
        return res;
    }
    if (strstr(buf, "Radiant") == NULL) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Moral compass output missing Radiant indicator.";
        return res;
    }

    return res;
}

bool relics_audit(void)
{
    diag_result_t res = relics_preflight();
    return (res.passed && res.severity != SEVERITY_FATAL);
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================
