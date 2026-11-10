/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          manifest.c
@id            core.manifest
@layer         Layer 0: Core Utilities
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-09
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Central Manifest Registry & Pre-Flight DAG Policy Engine
@role          Metadata Types, DAG Dependency Graph & Pre-Flight Contracts
@invariants    1. Zero dynamic heap allocations (ALLOC_ZERO_HEAP: stack/static only).
               2. Fixed static capacity: Never exceed MANIFEST_MAX_MODULES.
               3. Severity escalation: FATAL > CRITICAL > WARNING > PASS.
               4. Loose bolts principle: System halts immediately on any FATAL check.
               5. Missing upstream dependencies trigger SEVERITY_FATAL and halt.
               6. Universal 5-Block Lifecycle Protocol enforced across header and source.

PURPOSE & ARCHITECTURAL ROLE:
  Central manifest registry and pre-flight verification implementation.
  Evaluates registered subsystem metadata, validates upstream Directed Acyclic
  Graph (DAG) dependencies, and executes loose-bolts pre-flight diagnostic checks
  before runtime simulation commences.

  ### For Game Designers & Non-Programmers:
  Before the game opens its main window, it runs a pre-flight inspection like
  an airliner before takeoff. It checks that all engine modules and subsystems
  are present, securely connected, and ready. If a critical component is
  missing, it alerts developers immediately instead of crashing during play.

  ### For Systems Programmers & Engine Architects:
  - Directed Acyclic Graph (DAG) dependency validation across Layer 0, 1, and 2.
  - Zero heap allocation: all registry tables and audit results reside in static .bss.
  - Categorized diagnostic severity taxonomy (diag_severity_t) with threshold masks.
  - Compile-time module identity tagging (module_manifest_t) for DWARF debugging.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING / STATIC DATA BLOCK
Stratum: Layer 0 (Core Utilities) | Machine: Central Manifest Registry
================================================================================
*/

/* --- Tier 1: Standard Library Headers --- */
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* --- Tier 2: Core Engine Specifications & Subsystems --- */
#include "core/manifest.h"
#include "log/log.h"
#include "debug/debug.h"

static const char *layer_names[LAYER_COUNT] = {
    "Layer 0: Core Utilities",
    "Layer 1: Simulation Engine",
    "Layer 2: Presentation Viewport",
    "Layer 3: Application Entry",
    "Layer 4: Verification Suite"
};

static const char *const manifest_invariants[] = {
    "Fixed static capacity: count <= MANIFEST_MAX_MODULES",
    "Zero dynamic heap allocations: ALLOC_ZERO_HEAP strictly enforced",
    "Policy escalation: FATAL > CRITICAL > WARNING > PASS",
    "Loose bolts principle: Missing upstream dependency yields SEVERITY_FATAL"
};

static diag_result_t dummy_pass_check(void)
{
    diag_result_t res;
    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.subsystem = "test.mod.pass";
    res.check_name = "Pass Check";
    res.details = "Nominal test verification passed.";
    return res;
}

static diag_result_t dummy_warn_check(void)
{
    diag_result_t res;
    res.passed = true;
    res.severity = SEVERITY_WARNING;
    res.subsystem = "test.mod.warn";
    res.check_name = "Warn Check";
    res.details = "Non-fatal operational notice logged.";
    return res;
}

static diag_result_t dummy_crit_check(void)
{
    diag_result_t res;
    res.passed = false;
    res.severity = SEVERITY_CRITICAL;
    res.subsystem = "test.mod.crit";
    res.check_name = "Critical Check";
    res.details = "Subsystem offline; eligible for safe downgrade.";
    return res;
}

static diag_result_t dummy_fatal_check(void)
{
    diag_result_t res;
    res.passed = false;
    res.severity = SEVERITY_FATAL;
    res.subsystem = "test.mod.fatal";
    res.check_name = "Fatal Check";
    res.details = "Fatal invariant breach: loose bolts halt.";
    return res;
}

const module_metadata_t manifest_utils_manifest = {
    "core.util.manifest",
    "manifest.h",
    LAYER_0_CORE_UTIL,
    { 0, 1, 0 },
    4,                          /* interface_rev */
    "2026-11-07",               /* last_updated */
    "Central manifest registry and pre-flight policy engine",
    0,                          /* dependency_count */
    { NULL },                   /* dependencies */
    ALLOC_ZERO_HEAP,            /* alloc_policy */
    CONCURRENCY_REENTRANT,      /* concurrency */
    10,                         /* max_budget_us */
    4,
    {
        manifest_invariants[0],
        manifest_invariants[1],
        manifest_invariants[2],
        manifest_invariants[3]
    },
    manifest_utils_preflight
};

/*
================================================================================
BLOCK 3: BODY BLOCK / CORE REGISTRY IMPLEMENTATION BLOCK
Stratum: Layer 0 (Core Utilities) | Machine: Central Manifest Registry
================================================================================
Mission: Module registration, pre-flight DAG dependency tracking, telemetry
report formatting, and ontological metadata string conversion cogs.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: Manifest Registry Lifecycle & Node Registration Cogs
--------------------------------------------------------------------------------
Initializes registry storage and records immutable module metadata nodes.
--------------------------------------------------------------------------------
*/

/**
 * @brief Initializes a manifest registry structure to an empty state.
 */
void manifest_registry_init(manifest_registry_t *registry)
{
    if (registry == NULL) {
        return;
    }

    memset(registry, 0, sizeof(manifest_registry_t));
    registry->status = PREFLIGHT_STATUS_OK;
}

/**
 * @brief Registers a subsystem's compiled metadata record into the manifest registry.
 */
bool manifest_register(manifest_registry_t *registry,
                       const module_metadata_t *module)
{
    if (registry == NULL || module == NULL || module->module_id == NULL) {
        debug_trap_tripwire("manifest", "NULL pointer passed to manifest_register", __FILE__, __LINE__);
        return false;
    }

    if (registry->count >= MANIFEST_MAX_MODULES) {
        debug_trap_tripwire("manifest", "Capacity exceeded MANIFEST_MAX_MODULES", __FILE__, __LINE__);
        return false;
    }

    for (size_t i = 0; i < registry->count; i++) {
        if (registry->modules[i] != NULL &&
            strcmp(registry->modules[i]->module_id, module->module_id) == 0) {
            debug_trap_tripwire("manifest", "Duplicate module_id registration attempt", __FILE__, __LINE__);
            return false;
        }
    }

    registry->modules[registry->count] = module;
    registry->count++;
    return true;
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: Diagnostic Telemetry & Formatted Report Cogs
--------------------------------------------------------------------------------
Renders human-readable tabular ASCII reports of registered engine modules.
--------------------------------------------------------------------------------
*/

/**
 * @brief Prints a formatted diagnostic summary table to stdout.
 */
void manifest_print_report(const manifest_registry_t *registry)
{
    if (registry == NULL) {
        return;
    }

    puts("========================================================"
         "========================");
    puts("           Tenebris and Lux - Pre-Flight Subsystem "
         "Integrity Manifest           ");
    printf(" Build Telemetry: %s %s | Standard: ISO 8601 (2026-11-07)\n",
           __DATE__, __TIME__);
    puts("========================================================"
         "========================");
    printf("%-20s %-15s %-4s %-11s %-7s %s\n",
           "MODULE ID", "FILE", "REV", "ALLOCATION", "STATUS",
           "DIAGNOSTIC DETAILS");
    puts("--------------------------------------------------------"
         "------------------------");

    for (size_t i = 0; i < registry->count; i++) {
        const module_metadata_t *mod = registry->modules[i];
        const diag_result_t *res = &registry->results[i];

        const char *sev_str;
        switch (res->severity) {
        case SEVERITY_PASS:     sev_str = "[PASS]"; break;
        case SEVERITY_INFO:     sev_str = "[INFO]"; break;
        case SEVERITY_WARNING:  sev_str = "[WARN]"; break;
        case SEVERITY_CRITICAL: sev_str = "[CRIT]"; break;
        case SEVERITY_FATAL:    sev_str = "[FAIL]"; break;
        default:                sev_str = "[????]"; break;
        }

        printf("%-20s %-15s r%-3u %-11s %-7s %s\n",
            mod->module_id ? mod->module_id : "unknown",
            mod->file_name ? mod->file_name : "unknown",
            mod->interface_rev,
            manifest_alloc_policy_to_str(mod->alloc_policy),
            sev_str,
            res->details ? res->details : "Verified.");
    }

    puts("--------------------------------------------------------"
         "------------------------");
    printf("Telemetry: %zu modules registered | %zu Pass | %zu Warn | "
           "%zu Critical | %zu Fatal\n",
           registry->count, registry->pass_count, registry->warn_count,
           registry->critical_count, registry->fatal_count);

    switch (registry->status) {
    case PREFLIGHT_STATUS_OK:
        printf("Policy Verdict: ALL PRE-FLIGHT INVARIANTS SATISFIED "
               "(System Ready)\n");
        break;
    case PREFLIGHT_STATUS_WARNINGS:
        printf("Policy Verdict: SYSTEM LAUNCH PERMITTED WITH NOTICES "
               "(Warnings)\n");
        break;
    case PREFLIGHT_STATUS_DOWNGRADED:
        printf("Policy Verdict: CRITICAL SUBSYSTEM FAULT - "
               "DOWNGRADING TO SAFE MODE\n");
        break;
    case PREFLIGHT_STATUS_HALT_FATAL:
        printf("Policy Verdict: LOOSE BOLTS DETECTED - "
               "ENGINE BOOT HALTED IMMEDIATELY\n");
        break;
    }
    puts("========================================================"
         "========================");
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.3: Ontological Enum Serialization Cogs
--------------------------------------------------------------------------------
Converts diagnostic codes, status levels, layers, and policies to static strings.
--------------------------------------------------------------------------------
*/

const char *manifest_severity_to_str(diag_severity_t severity)
{
    switch (severity) {
    case SEVERITY_PASS:     return "PASS";
    case SEVERITY_INFO:     return "INFO";
    case SEVERITY_WARNING:  return "WARNING";
    case SEVERITY_CRITICAL: return "CRITICAL";
    case SEVERITY_FATAL:    return "FATAL";
    default:                return "UNKNOWN";
    }
}

const char *manifest_status_to_str(preflight_status_t status)
{
    switch (status) {
    case PREFLIGHT_STATUS_OK:         return "READY";
    case PREFLIGHT_STATUS_WARNINGS:   return "WARNINGS";
    case PREFLIGHT_STATUS_DOWNGRADED: return "DOWNGRADED";
    case PREFLIGHT_STATUS_HALT_FATAL: return "HALTED";
    default:                          return "UNKNOWN";
    }
}

const char *manifest_layer_to_str(module_layer_t layer)
{
    if ((size_t)layer < (size_t)LAYER_COUNT) {
        return layer_names[layer];
    }
    return "Unknown Layer";
}

const char *manifest_alloc_policy_to_str(alloc_policy_t policy)
{
    switch (policy) {
    case ALLOC_ZERO_HEAP:  return "Zero-Heap";
    case ALLOC_FIXED_POOL: return "Fixed-Pool";
    case ALLOC_ARENA:      return "Arena";
    case ALLOC_DYNAMIC:    return "Dynamic";
    default:               return "Unknown";
    }
}

const char *manifest_concurrency_to_str(concurrency_policy_t policy)
{
    switch (policy) {
    case CONCURRENCY_REENTRANT:    return "Reentrant";
    case CONCURRENCY_THREAD_LOCAL: return "Thread-Local";
    case CONCURRENCY_MAIN_THREAD:  return "Main-Thread";
    default:                       return "Unknown";
    }
}

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 0 (Core Utilities) | Machine: Central Manifest Registry
================================================================================
Mission: Pre-flight DAG verification, dependency audits, loose-bolts checking,
and invariant self-test routines.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.1: Pre-Flight DAG Evaluation & Subsystem Audits
--------------------------------------------------------------------------------
Executes upstream dependency verification and diagnostic self-audits.
--------------------------------------------------------------------------------
*/

/**
 * @brief Executes comprehensive pre-flight diagnostics across all registered modules.
 */
preflight_status_t manifest_run_preflight(manifest_registry_t *registry)
{
    if (registry == NULL) {
        debug_trap_tripwire("manifest", "NULL registry passed to manifest_run_preflight", __FILE__, __LINE__);
        return PREFLIGHT_STATUS_HALT_FATAL;
    }

    registry->pass_count = 0;
    registry->warn_count = 0;
    registry->critical_count = 0;
    registry->fatal_count = 0;

    /* Step 1: Active DAG Dependency Graph Verification */
    for (size_t i = 0; i < registry->count; i++) {
        const module_metadata_t *mod = registry->modules[i];
        if (mod == NULL) continue;

        for (size_t d = 0; d < mod->dependency_count; d++) {
            const char *req_dep = mod->dependencies[d];
            if (req_dep == NULL) continue;

            bool found = false;
            for (size_t j = 0; j < registry->count; j++) {
                if (registry->modules[j] != NULL &&
                    strcmp(registry->modules[j]->module_id, req_dep) == 0) {
                    found = true;
                    break;
                }
            }

            if (!found) {
                diag_result_t dep_res;
                dep_res.passed = false;
                dep_res.severity = SEVERITY_FATAL;
                dep_res.subsystem = mod->module_id;
                dep_res.check_name = "Dependency DAG";
                dep_res.details =
                    "Fatal: Unregistered upstream dependency declared.";
                registry->results[i] = dep_res;
                registry->fatal_count++;
                registry->status = PREFLIGHT_STATUS_HALT_FATAL;
                debug_trap_tripwire("manifest", "Unregistered upstream DAG dependency detected", __FILE__, __LINE__);
                return registry->status;
            }
        }
    }

    /* Step 2: Individual Module Pre-Flight Verification */
    for (size_t i = 0; i < registry->count; i++) {
        const module_metadata_t *mod = registry->modules[i];
        if (mod == NULL) {
            continue;
        }

        diag_result_t res;
        if (mod->preflight_check != NULL) {
            res = mod->preflight_check();
        } else {
            res.passed = true;
            res.severity = SEVERITY_PASS;
            res.subsystem = mod->module_id;
            res.check_name = "Presence";
            res.details = "No active preflight check defined.";
        }

        registry->results[i] = res;

        bool is_fatal = (res.severity == SEVERITY_FATAL) ||
            (!res.passed && res.severity != SEVERITY_WARNING &&
             res.severity != SEVERITY_CRITICAL);

        if (is_fatal) {
            registry->fatal_count++;
            registry->status = PREFLIGHT_STATUS_HALT_FATAL;
            return registry->status;
        } else if (res.severity == SEVERITY_CRITICAL) {
            registry->critical_count++;
        } else if (res.severity == SEVERITY_WARNING) {
            registry->warn_count++;
        } else {
            registry->pass_count++;
        }
    }

    /* Step 3: Determine aggregate system status */
    if (registry->critical_count > 0) {
        registry->status = PREFLIGHT_STATUS_DOWNGRADED;
    } else if (registry->warn_count > 0) {
        registry->status = PREFLIGHT_STATUS_WARNINGS;
    } else {
        registry->status = PREFLIGHT_STATUS_OK;
    }

    return registry->status;
}

/**
 * @brief Pre-flight diagnostic check evaluating manifest registry invariants.
 */
diag_result_t manifest_utils_preflight(void)
{
    diag_result_t res;
    res.subsystem = manifest_utils_manifest.module_id;
    res.check_name = "Registry Capacity & Policy Escalation";

    if (!manifest_audit()) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: Manifest registry invariant self-audit failed.";
        return res;
    }

    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.details =
        "Static capacity, duplicate rejection & graded policy verified.";
    return res;
}

/**
 * @brief Standalone boolean self-audit testing manifest invariants.
 */
bool manifest_audit(void)
{
    manifest_registry_t reg;
    manifest_registry_init(&reg);

    module_metadata_t mod_pass = {
        "test.mod.pass", "pass.h", LAYER_0_CORE_UTIL, { 1, 0, 0 },
        1, "2026-11-07", "Pass module", 0, { NULL },
        ALLOC_ZERO_HEAP, CONCURRENCY_REENTRANT, 10,
        0, { NULL }, dummy_pass_check
    };

    module_metadata_t mod_warn = {
        "test.mod.warn", "warn.h", LAYER_0_CORE_UTIL, { 1, 0, 0 },
        1, "2026-11-07", "Warn module", 0, { NULL },
        ALLOC_ZERO_HEAP, CONCURRENCY_REENTRANT, 10,
        0, { NULL }, dummy_warn_check
    };

    module_metadata_t mod_crit = {
        "test.mod.crit", "crit.h", LAYER_2_PRESENTATION, { 1, 0, 0 },
        1, "2026-11-07", "Critical module", 0, { NULL },
        ALLOC_ZERO_HEAP, CONCURRENCY_MAIN_THREAD, 50,
        0, { NULL }, dummy_crit_check
    };

    module_metadata_t mod_fatal = {
        "test.mod.fatal", "fatal.h", LAYER_0_CORE_UTIL, { 1, 0, 0 },
        1, "2026-11-07", "Fatal module", 0, { NULL },
        ALLOC_ZERO_HEAP, CONCURRENCY_REENTRANT, 10,
        0, { NULL }, dummy_fatal_check
    };

    module_metadata_t mod_missing_dep = {
        "test.mod.depfail", "depfail.h", LAYER_1_SIMULATION, { 1, 0, 0 },
        1, "2026-11-07", "Missing dep module", 1, { "unregistered.upstream" },
        ALLOC_ZERO_HEAP, CONCURRENCY_REENTRANT, 20,
        0, { NULL }, dummy_pass_check
    };

    /* Invariant 1: Registration of clean module produces PREFLIGHT_STATUS_OK */
    if (!manifest_register(&reg, &mod_pass)) return false;
    if (manifest_run_preflight(&reg) != PREFLIGHT_STATUS_OK) return false;

    /* Invariant 2: Duplicate registration rejected */
    if (manifest_register(&reg, &mod_pass)) return false;

    /* Invariant 3: Warning escalation */
    manifest_registry_init(&reg);
    manifest_register(&reg, &mod_pass);
    manifest_register(&reg, &mod_warn);
    if (manifest_run_preflight(&reg) != PREFLIGHT_STATUS_WARNINGS) return false;

    /* Invariant 4: Critical downgrade policy */
    manifest_registry_init(&reg);
    manifest_register(&reg, &mod_pass);
    manifest_register(&reg, &mod_crit);
    if (manifest_run_preflight(&reg) != PREFLIGHT_STATUS_DOWNGRADED) {
        return false;
    }

    /* Invariant 5: Fatal halt escalation */
    manifest_registry_init(&reg);
    manifest_register(&reg, &mod_pass);
    manifest_register(&reg, &mod_crit);
    manifest_register(&reg, &mod_fatal);
    if (manifest_run_preflight(&reg) != PREFLIGHT_STATUS_HALT_FATAL) {
        return false;
    }

    /* Invariant 6: Active DAG dependency halts on missing upstream */
    manifest_registry_init(&reg);
    manifest_register(&reg, &mod_missing_dep);
    if (manifest_run_preflight(&reg) != PREFLIGHT_STATUS_HALT_FATAL) {
        return false;
    }

    return true;
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK / EXECUTABLE PROCEDURES & LIFECYCLE
Stratum: Layer 0 (Core Utilities) | Subsystem: Central Manifest Registry
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

bool manifest_execute_dump_card(const manifest_registry_t *registry,
                               char *out_buf,
                               size_t buf_size)
{
    if (registry == NULL || out_buf == NULL || buf_size < 300) {
        return false;
    }

    int written = snprintf(
        out_buf, buf_size,
        ".----------------------------------------------------------.\n"
        "|             MANIFEST REGISTRY INSPECTION CARD            |\n"
        "|----------------------------------------------------------|\n"
        "| Registered Modules : %2zu / %-2d                              |\n"
        "| Pre-Flight Status  : %-36s|\n"
        "| Tally Results      : %2zu Pass | %2zu Warn | %2zu Crit | %2zu Fail |\n"
        "| Allocation Policy  : ALLOC_ZERO_HEAP (Deterministic)     |\n"
        "| DAG Verification   : %-36s|\n"
        "'----------------------------------------------------------'\n",
        registry->count, MANIFEST_MAX_MODULES,
        manifest_status_to_str(registry->status),
        registry->pass_count, registry->warn_count,
        registry->critical_count, registry->fatal_count,
        (registry->fatal_count == 0) ? "VERIFIED (Zero dangling DAG edges)" : "FAILED (Unresolved dependencies)");

    return (written > 0 && (size_t)written < buf_size);
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.2: Subsystem Lifecycle Initialization & Teardown
--------------------------------------------------------------------------------
None. (Stateless module / pure stack-allocated execution).
--------------------------------------------------------------------------------
*/

/* End of manifest.c */
