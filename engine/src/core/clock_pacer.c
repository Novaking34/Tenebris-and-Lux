/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          clock_pacer.c
@id            core.util.clock_pacer
@layer         Layer 0: Core Utilities
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-09
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Clock & Frame Pacing Orchestrator Subsystem (core/pacing)
@role          Fixed Simulation Timestep Pacing, Spiral-of-Death Clamping & Render Interpolation
@invariants    1. Zero dynamic heap allocations during timing evaluation (ALLOC_ZERO_HEAP).
               2. Strict ISO C11 adherence (-std=c11, -Wall -Wextra -Wpedantic).
               3. Frame delta strictly clamped to [0.0, max_delta_clamp_ms].
               4. Maximum consecutive simulation steps per render frame strictly clamped to max_substeps.
               5. Render interpolation alpha strictly conserved within [0.0, 1.0].
               6. Simulation determinism strictly conserved across variable display refresh rates.
               7. Universal 5-Block Lifecycle Protocol enforced across header and source.

PURPOSE & ARCHITECTURAL ROLE:
  Implementation of the universal clock and frame pacing orchestrator cog.
  Provides decoupled fixed simulation steps (16.666ms) with accumulator management,
  lag spike clamping preventing simulation death spirals, and render alpha interpolation.
  Ingests pacing policies from engine/config/runtime/time/pacing_policy.json.

  ### For Game Designers & Non-Programmers:
  This system ensures the game plays at the exact same physical speed on a 60Hz
  office monitor, a 144Hz gaming laptop, or a handheld device. If the computer
  stutters, it prevents the game from speeding up like fast-forward to catch up.

  ### For Systems Programmers & Engine Architects:
  - Fixed-timestep simulation accumulator pattern (`dt = 16.666ms`).
  - Strict upper bound on frame delta (`100ms`) preventing death-spiral freezes.
  - Normalized render interpolation alpha (`alpha = remainder / dt`) in `[0.0, 1.0]`.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK / STATIC DATA & DEPENDENCIES
Stratum: Layer 0 (Core Utilities) | Machine: Clock & Pacing Orchestrator
================================================================================
*/

/*
--------------------------------------------------------------------------------
Include Hierarchy (Downward Dependency Ladder)
--------------------------------------------------------------------------------
Tier 1: Standard ISO C11 Library Headers
Tier 2: External Dependencies (None)
Tier 3: Internal Engine Foundation Headers
--------------------------------------------------------------------------------
*/

/* --- Tier 1: Standard ISO C11 Library Headers --- */
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

/* --- Tier 2: External Dependencies (None) --- */

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "core/clock_pacer.h"
#include "core/clock_pacer_fallback.h"
#include "core/manifest.h"
#include "core/string_utils.h"
#include "core/time_utils.h"
#include "io/master_loader.h"
#include "log/log.h"
#include "debug/debug.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Axiomatic Invariants & Central Manifest Anchor
--------------------------------------------------------------------------------
*/

static const char *const clock_pacer_invariants[] = {
    "Zero dynamic heap allocations during timing evaluation (ALLOC_ZERO_HEAP)",
    "Frame delta strictly clamped to [0.0, max_delta_clamp_ms]",
    "Consecutive simulation steps per frame capped at max_substeps",
    "Render interpolation alpha bounded in [0.0, 1.0]",
    "Simulation determinism strictly conserved across refresh rates"
};

const module_metadata_t clock_pacer_manifest = {
    "core.util.pacer",
    "clock_pacer.h",
    LAYER_0_CORE_UTIL,
    { 0, 1, 0 },
    4,                          /* interface_rev */
    "2026-11-07",               /* last_updated */
    "Fixed-timestep simulation pacing, spiral-of-death clamp and render interpolation",
    0,                          /* dependency_count: foundational leaf cog */
    { NULL },                   /* dependencies */
    ALLOC_ZERO_HEAP,            /* alloc_policy */
    CONCURRENCY_REENTRANT,      /* concurrency */
    5,                          /* max_budget_us */
    5,
    {
        clock_pacer_invariants[0],
        clock_pacer_invariants[1],
        clock_pacer_invariants[2],
        clock_pacer_invariants[3],
        clock_pacer_invariants[4]
    },
    clock_pacer_preflight
};

/*
================================================================================
BLOCK 3: BODY BLOCK / CORE ENGINE IMPLEMENTATION
Stratum: Layer 0 (Core Utilities) | Machine: Clock & Pacing Orchestrator
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: Lifecycle & Configuration Implementation
--------------------------------------------------------------------------------
*/

clock_pacer_config_t clock_pacer_get_default_config(void)
{
    /* Step 1: Zero-initialize destination structure */
    clock_pacer_config_t cfg;
    memset(&cfg, 0, sizeof(cfg));

    /* Step 2: Populate canonical baseline metadata and timing invariants */
    str_safe_copy(cfg.id, CLOCK_PACER_FALLBACK_PROFILE_NAME, sizeof(cfg.id));
    str_safe_copy(cfg.display_name, "Canonical 60Hz Pacing Baseline", sizeof(cfg.display_name));
    cfg.fixed_step_ms = CLOCK_PACER_FALLBACK_STEP_MS;
    cfg.max_delta_clamp_ms = CLOCK_PACER_FALLBACK_CLAMP_MS;
    cfg.max_substeps = CLOCK_PACER_FALLBACK_MAX_SUBSTEPS;
    cfg.max_debt_ms = CLOCK_PACER_FALLBACK_DEBT_MS;
    cfg.loaded_from_data = false;

    /* Step 3: Return initialized configuration */
    return cfg;
}

bool clock_pacer_load_config_from_file(const char *profile_path, clock_pacer_config_t *out_config)
{
    /* Step 1: Precondition check on destination pointer */
    if (out_config == NULL) {
        debug_trap_tripwire("pacer", "NULL out_config passed to clock_pacer_load_config_from_file", __FILE__, __LINE__);
        return false;
    }

    /* Step 2: Fallback initialization in case file read fails */
    *out_config = clock_pacer_get_default_config();

    /* Step 3: Resolve target path in engine/config/ */
    const char *target_rel = profile_path;
    if (target_rel == NULL || target_rel[0] == '\0') {
        target_rel = "runtime/time/pacing_policy.json";
    }

    /* Step 4: Ingest JSON manifest via master_loader */
    master_loader_t loader;
    char file_buf[8192];
    if (!master_loader_load(&loader, "config", target_rel, file_buf, sizeof(file_buf))) {
        LOG_RECORD(LOG_SEV_WARNING, LOG_CAT_LOG, "Pacer", -0.2f,
                   "Failed to load pacing policy '%s', falling back to defaults", target_rel);
        return false;
    }

    /* Step 5: Extract schema attributes */
    master_loader_get_string(&loader, "block_1_metadata", "id", out_config->id, sizeof(out_config->id));
    master_loader_get_string(&loader, "block_1_metadata", "role", out_config->display_name, sizeof(out_config->display_name));

    float fixed_step_f = 0.0f;
    if (master_loader_get_float(&loader, "static_declarations", "fixed_sim_step_ms", &fixed_step_f)) {
        out_config->fixed_step_ms = (double)fixed_step_f;
    }

    float max_clamp_f = 0.0f;
    if (master_loader_get_float(&loader, "static_declarations", "max_delta_clamp_ms", &max_clamp_f)) {
        out_config->max_delta_clamp_ms = (double)max_clamp_f;
    }

    int substeps_i = 0;
    if (master_loader_get_int(&loader, "static_declarations", "max_substeps_per_frame", &substeps_i)) {
        out_config->max_substeps = (uint32_t)substeps_i;
    }

    /* Step 6: Sanitize extracted bounds */
    if (out_config->fixed_step_ms <= 0.0) out_config->fixed_step_ms = CLOCK_PACER_FALLBACK_STEP_MS;
    if (out_config->max_delta_clamp_ms <= 0.0) out_config->max_delta_clamp_ms = CLOCK_PACER_FALLBACK_CLAMP_MS;
    if (out_config->max_substeps == 0) out_config->max_substeps = CLOCK_PACER_FALLBACK_MAX_SUBSTEPS;

    out_config->loaded_from_data = true;

    LOG_RECORD(LOG_SEV_DEBUG, LOG_CAT_LOG, "Pacer", 0.0f,
               "Pacing policy loaded: fixed_step=%.2fms, max_delta=%.2fms, max_substeps=%u",
               out_config->fixed_step_ms, out_config->max_delta_clamp_ms, out_config->max_substeps);
    return true;
}

void clock_pacer_init(clock_pacer_t *pacer, const clock_pacer_config_t *config)
{
    /* Step 1: Precondition audit */
    if (pacer == NULL) {
        debug_trap_tripwire("pacer", "NULL pacer passed to clock_pacer_init", __FILE__, __LINE__);
        return;
    }

    /* Step 2: Zero state machine memory layout */
    memset(pacer, 0, sizeof(*pacer));

    /* Step 3: Hydrate configuration */
    if (config != NULL) {
        pacer->config = *config;
    } else {
        pacer->config = clock_pacer_get_default_config();
    }

    /* Step 4: Mark initialized */
    pacer->initialized = true;
}

void clock_pacer_reset(clock_pacer_t *pacer)
{
    if (pacer == NULL) return;
    pacer->last_time_ns = 0;
    pacer->accumulator_ms = 0.0;
    pacer->render_alpha = 0.0;
    pacer->total_sim_ticks = 0;
    pacer->total_frames = 0;
    pacer->spiral_clamp_count = 0;
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: Frame Timing & Accumulator Stepping Implementation
--------------------------------------------------------------------------------
*/

double clock_pacer_begin_frame(clock_pacer_t *pacer, uint64_t current_time_ns)
{
    if (pacer == NULL) return 0.0;

    double delta_ms = 0.0;

    /* Step 1: Compute elapsed nanoseconds and convert to milliseconds */
    if (pacer->last_time_ns == 0 || current_time_ns <= pacer->last_time_ns) {
        delta_ms = pacer->config.fixed_step_ms;
    } else {
        uint64_t delta_ns = current_time_ns - pacer->last_time_ns;
        delta_ms = (double)delta_ns / 1000000.0;
    }

    /* Step 2: Detect negative delta anomalies */
    if (delta_ms < 0.0) {
        debug_trap_tripwire("pacer", "Negative frame delta observed, clamped to 0.0", __FILE__, __LINE__);
        delta_ms = 0.0;
    }

    /* Step 3: Clamp lag spikes to prevent spiral of death */
    if (delta_ms > pacer->config.max_delta_clamp_ms) {
        pacer->spiral_clamp_count++;
        LOG_RECORD(LOG_SEV_WARNING, LOG_CAT_LOG, "Pacer", -0.1f,
                   "Spiral of death lag clamp active: delta %.2fms clamped to %.2fms",
                   delta_ms, pacer->config.max_delta_clamp_ms);
        delta_ms = pacer->config.max_delta_clamp_ms;
    }

    /* Step 4: Deposit clamped delta into simulation accumulator */
    pacer->accumulator_ms += delta_ms;

    /* Step 5: Enforce unconsumed debt cap */
    if (pacer->accumulator_ms > pacer->config.max_debt_ms) {
        pacer->accumulator_ms = pacer->config.max_debt_ms;
    }

    /* Step 6: Advance timestamp anchor */
    pacer->last_time_ns = current_time_ns;
    return delta_ms;
}

bool clock_pacer_should_step(const clock_pacer_t *pacer, uint32_t substep_counter)
{
    if (pacer == NULL) return false;
    return (pacer->accumulator_ms >= pacer->config.fixed_step_ms) &&
           (substep_counter < pacer->config.max_substeps);
}

void clock_pacer_consume_step(clock_pacer_t *pacer, uint32_t *substep_counter)
{
    if (pacer == NULL) return;

    /* Step 1: Subtract one fixed simulation quantum */
    pacer->accumulator_ms -= pacer->config.fixed_step_ms;
    if (pacer->accumulator_ms < 0.0) {
        pacer->accumulator_ms = 0.0;
    }

    /* Step 2: Increment substep counter and total tick counter */
    if (substep_counter != NULL) {
        (*substep_counter)++;
    }
    pacer->total_sim_ticks++;
}

void clock_pacer_end_frame(clock_pacer_t *pacer)
{
    if (pacer == NULL) return;

    /* Step 1: Compute normalized render interpolation alpha */
    if (pacer->config.fixed_step_ms > 0.0) {
        pacer->render_alpha = pacer->accumulator_ms / pacer->config.fixed_step_ms;
    } else {
        pacer->render_alpha = 0.0;
    }

    /* Step 2: Clamp alpha strictly within [0.0, 1.0] */
    if (pacer->render_alpha < 0.0) pacer->render_alpha = 0.0;
    if (pacer->render_alpha > 1.0) pacer->render_alpha = 1.0;

    /* Step 3: Increment total rendered frame counter */
    pacer->total_frames++;
}

double clock_pacer_get_alpha(const clock_pacer_t *pacer)
{
    if (pacer == NULL) return 0.0;
    return pacer->render_alpha;
}

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK / TRIPWIRES
Stratum: Layer 0 (Core Utilities) | Machine: Clock & Pacing Orchestrator
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.1: Machine Pre-Flight Tripwire & Self-Audit Procedures
--------------------------------------------------------------------------------
*/

diag_result_t clock_pacer_preflight(void)
{
    diag_result_t res;
    res.subsystem = clock_pacer_manifest.module_id;
    res.check_name = "Clock & Simulation Pacing Integrity";

    /* Test 1: Default configuration limits */
    clock_pacer_config_t cfg = clock_pacer_get_default_config();
    if (cfg.fixed_step_ms < 16.6 || cfg.fixed_step_ms > 16.7 ||
        cfg.max_delta_clamp_ms != 100.0 || cfg.max_substeps != 3) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: Default pacing config violates canonical baseline.";
        return res;
    }

    /* Test 2: Accumulator consumption and alpha bounds */
    clock_pacer_t pacer;
    clock_pacer_init(&pacer, &cfg);

    /* Simulate 51ms passing (yields 3 ticks at 16.666ms with ~1ms remainder) */
    pacer.last_time_ns = 1000000000ULL;
    clock_pacer_begin_frame(&pacer, 1051000000ULL);

    uint32_t steps = 0;
    while (clock_pacer_should_step(&pacer, steps)) {
        clock_pacer_consume_step(&pacer, &steps);
    }

    if (steps != 3) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: Pacer failed to execute expected 3 substeps for 51ms.";
        return res;
    }

    clock_pacer_end_frame(&pacer);
    double alpha = clock_pacer_get_alpha(&pacer);
    if (alpha < 0.0 || alpha > 1.0) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: Render interpolation alpha out of [0.0, 1.0] bounds.";
        return res;
    }

    /* Test 3: Spiral of death lag clamp tripwire */
    clock_pacer_reset(&pacer);
    pacer.last_time_ns = 1000000000ULL;
    double clamped_delta = clock_pacer_begin_frame(&pacer, 2000000000ULL); /* 1000ms hitch */
    if (clamped_delta != 100.0 || pacer.spiral_clamp_count != 1) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: Spiral of death 100ms clamp failed to engage.";
        return res;
    }

    /* Test 4: Manifest data hydration */
    clock_pacer_config_t data_cfg;
    if (clock_pacer_load_config_from_file("runtime/time/pacing_policy.json", &data_cfg)) {
        if (!data_cfg.loaded_from_data || data_cfg.max_substeps != 3) {
            res.passed = false;
            res.severity = SEVERITY_FATAL;
            res.details = "Fatal: Pacing policy data hydration failed invariants.";
            return res;
        }
    }

    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.details = "Fixed-step pacing, lag clamp & render interpolation verified.";
    return res;
}

bool clock_pacer_audit(void)
{
    diag_result_t pf = clock_pacer_preflight();
    return pf.passed && (pf.severity != SEVERITY_FATAL);
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK / EXECUTABLE PROCEDURES & LIFECYCLE
Stratum: Layer 0 (Core Utilities) | Subsystem: Clock & Pacing Orchestrator
================================================================================
Mission: Execute operational pipelines, format ASCII diagnostic cards,
construct programmatic inspection objects, and manage subsystem lifecycles.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.1: Executable Procedures, Inspection & Diagnostic Cards
--------------------------------------------------------------------------------
*/

bool clock_pacer_execute_dump_card(const clock_pacer_t *pacer, char *out_buf, size_t buf_size)
{
    if (pacer == NULL || out_buf == NULL || buf_size < 700) {
        return false;
    }

    int written = snprintf(
        out_buf, buf_size,
        ".----------------------------------------------------------.\n"
        "|              CLOCK PACER STATUS INSPECTION CARD          |\n"
        "|----------------------------------------------------------|\n"
        "| Profile Identifier : %-36s|\n"
        "| Fixed Sim Step     : %6.2f ms (%4.1f Hz)                     |\n"
        "| Max Delta Clamp    : %6.2f ms                             |\n"
        "| Max Substeps/Frame : %4u substeps                        |\n"
        "| Accumulator Debt   : %6.2f ms                             |\n"
        "| Render Alpha       : %6.4f                                 |\n"
        "| Total Sim Ticks    : %8u                              |\n"
        "| Spiral Clamps      : %8u                              |\n"
        "'----------------------------------------------------------'\n",
        pacer->config.id, pacer->config.fixed_step_ms,
        pacer->config.fixed_step_ms > 0 ? (1000.0 / pacer->config.fixed_step_ms) : 0.0,
        pacer->config.max_delta_clamp_ms, pacer->config.max_substeps,
        pacer->accumulator_ms, pacer->render_alpha,
        pacer->total_sim_ticks, pacer->spiral_clamp_count);

    return (written > 0 && (size_t)written < buf_size);
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.2: Subsystem Lifecycle Initialization & Teardown
--------------------------------------------------------------------------------
None. (Stateless module / pure stack-allocated execution).
--------------------------------------------------------------------------------
*/

/* End of clock_pacer.c */
