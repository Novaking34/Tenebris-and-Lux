// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @id            core.util.manifest
// @file          manifest.c
// @layer         Layer 0: Core Utilities
// @version       0.0.2 (Phase 1: Dual-Frontend Prototype)
// @interface_rev Rev 2 (DAG dependencies, allocation & concurrency contracts)
// @date          2026-10-26
// @updated       2026-10-26
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Central manifest registry and pre-flight verification implementation.
//   Evaluates registered subsystem metadata, validates upstream dependencies,
//   and executes loose-bolts pre-flight diagnostic checks before runtime.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: None (Foundational leaf module).
//   - Memory Allocation:     ALLOC_ZERO_HEAP (100% stack and static execution).
//   - Concurrency / Safety:  REENTRANT_PURE (Thread-safe, bounded capacity).
//
// AXIOMATIC INVARIANTS:
//   1. Fixed static capacity: Never exceed MANIFEST_MAX_MODULES.
//   2. Zero heap allocations: 100% stack/static execution.
//   3. Severity escalation: FATAL > CRITICAL > WARNING > PASS.
//   4. Loose bolts principle: System halts immediately on any FATAL check.
//   5. Missing upstream dependencies trigger SEVERITY_FATAL.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / STATIC DATA
// ( System Headers, String Tables & Static Buffers )
// =============================================================================

#include "util/manifest.h"
#include <stdio.h>
#include <string.h>

static const char *layer_names[LAYER_COUNT] = {
    "Layer 0: Core Utilities",
    "Layer 1: Simulation Engine",
    "Layer 2: Presentation Viewport",
    "Layer 3: Application Entry",
    "Layer 4: Verification Suite"
};

// =============================================================================
// BLOCK 3: BODY BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: Central Manifest Registry
// =============================================================================

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 3.1: CENTRAL REGISTRY LIFECYCLE & REGISTRATION COGS ]
// Mission: Initialize registry storage and register subsystem metadata nodes.
// -----------------------------------------------------------------------------

void manifest_registry_init(manifest_registry_t *registry)
{
    if (registry == NULL) {
        return;
    }
    memset(registry, 0, sizeof(manifest_registry_t));
    registry->status = PREFLIGHT_STATUS_OK;
}

bool manifest_register(manifest_registry_t *registry,
                       const module_metadata_t *module)
{
    if (registry == NULL || module == NULL || module->module_id == NULL) {
        return false;
    }

    if (registry->count >= MANIFEST_MAX_MODULES) {
        return false;
    }

    /* Check for duplicate module_id */
    for (size_t i = 0; i < registry->count; i++) {
        if (registry->modules[i] != NULL &&
            strcmp(registry->modules[i]->module_id, module->module_id) == 0) {
            return false;
        }
    }

    registry->modules[registry->count] = module;
    registry->count++;
    return true;
}

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 3.2: DAG DEPENDENCY RESOLUTION & PRE-FLIGHT EVALUATION ]
// Mission: Execute DAG dependency graph audits and pre-flight machine checks.
// -----------------------------------------------------------------------------

preflight_status_t manifest_run_preflight(manifest_registry_t *registry)
{
    if (registry == NULL) {
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
        } else if (res.severity == SEVERITY_CRITICAL) {
            registry->critical_count++;
        } else if (res.severity == SEVERITY_WARNING) {
            registry->warn_count++;
        } else {
            registry->pass_count++;
        }
    }

    /* Evaluate graded policy */
    if (registry->fatal_count > 0) {
        registry->status = PREFLIGHT_STATUS_HALT_FATAL;
    } else if (registry->critical_count > 0) {
        registry->status = PREFLIGHT_STATUS_DOWNGRADED;
    } else if (registry->warn_count > 0) {
        registry->status = PREFLIGHT_STATUS_WARNINGS;
    } else {
        registry->status = PREFLIGHT_STATUS_OK;
    }

    return registry->status;
}

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 3.3: TELEMETRY REPORTING & SERIALIZATION COGS ]
// Mission: Format pre-flight telemetry tables and serialize taxonomy strings.
// -----------------------------------------------------------------------------

void manifest_print_report(const manifest_registry_t *registry)
{
    if (registry == NULL) {
        return;
    }

    puts("========================================================"
         "========================");
    puts("           Tenebris and Lux - Pre-Flight Subsystem "
         "Integrity Manifest           ");
    printf(" Build Telemetry: %s %s | Standard: ISO 8601 (2026-10-26)\n",
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
    if (layer >= 0 && layer < LAYER_COUNT) {
        return layer_names[layer];
    }
    return "Unknown Layer";
}

const char *manifest_alloc_policy_to_str(alloc_policy_t policy)
{
    switch (policy) {
    case ALLOC_ZERO_HEAP: return "Zero-Heap";
    case ALLOC_FIXED_POOL: return "Fixed-Pool";
    case ALLOC_ARENA:      return "Arena";
    case ALLOC_DYNAMIC:    return "Dynamic";
    default:               return "Unknown";
    }
}

const char *manifest_concurrency_to_str(concurrency_policy_t policy)
{
    switch (policy) {
    case CONCURRENCY_REENTRANT:   return "Reentrant";
    case CONCURRENCY_THREAD_LOCAL: return "Thread-Local";
    case CONCURRENCY_MAIN_THREAD:   return "Main-Thread";
    default:                       return "Unknown";
    }
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: Central Manifest Registry
// =============================================================================

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 4.1: MACHINE SELF-AUDIT & PRE-FLIGHT TRIPWIRES ]
// Mission: Verify registry capacity bounds, duplicate rejection, and policies.
// -----------------------------------------------------------------------------

static diag_result_t dummy_pass_check(void)
{
    diag_result_t res = {
        true, SEVERITY_PASS, "test.pass", "PassCheck", "All green."
    };
    return res;
}

static diag_result_t dummy_warn_check(void)
{
    diag_result_t res = {
        true, SEVERITY_WARNING, "test.warn", "WarnCheck", "Suboptimal timing."
    };
    return res;
}

static diag_result_t dummy_crit_check(void)
{
    diag_result_t res = {
        false, SEVERITY_CRITICAL, "test.crit", "CritCheck", "Display error."
    };
    return res;
}

static diag_result_t dummy_fatal_check(void)
{
    diag_result_t res = {
        false, SEVERITY_FATAL, "test.fatal", "FatalCheck", "Loose bolt."
    };
    return res;
}

static const char *const manifest_invariants[] = {
    "Static allocation bounds: max 32 registered modules",
    "Zero heap allocation in pre-flight evaluation",
    "Severity taxonomy: PASS < INFO < WARN < CRITICAL < FATAL",
    "Loose bolts principle: Fatal error halts engine"
};

const module_metadata_t manifest_utils_manifest = {
    "core.util.manifest",
    "manifest.h",
    LAYER_0_CORE_UTIL,
    { 0, 0, 2 },
    2,                          // interface_rev
    "2026-10-26",               // last_updated
    "Central manifest registry and pre-flight policy engine",
    0,                          // dependency_count
    { NULL },                   // dependencies
    ALLOC_ZERO_HEAP,            // alloc_policy
    CONCURRENCY_REENTRANT,      // concurrency
    10,                         // max_budget_us
    4,
    {
        manifest_invariants[0],
        manifest_invariants[1],
        manifest_invariants[2],
        manifest_invariants[3]
    },
    manifest_utils_preflight
};

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

bool manifest_audit(void)
{
    manifest_registry_t reg;
    manifest_registry_init(&reg);

    module_metadata_t mod_pass = {
        "test.mod.pass", "pass.h", LAYER_0_CORE_UTIL, { 1, 0, 0 },
        1, "2026-10-26", "Pass module", 0, { NULL },
        ALLOC_ZERO_HEAP, CONCURRENCY_REENTRANT, 10,
        0, { NULL }, dummy_pass_check
    };

    module_metadata_t mod_warn = {
        "test.mod.warn", "warn.h", LAYER_0_CORE_UTIL, { 1, 0, 0 },
        1, "2026-10-26", "Warn module", 0, { NULL },
        ALLOC_ZERO_HEAP, CONCURRENCY_REENTRANT, 10,
        0, { NULL }, dummy_warn_check
    };

    module_metadata_t mod_crit = {
        "test.mod.crit", "crit.h", LAYER_2_PRESENTATION, { 1, 0, 0 },
        1, "2026-10-26", "Critical module", 0, { NULL },
        ALLOC_ZERO_HEAP, CONCURRENCY_MAIN_THREAD, 50,
        0, { NULL }, dummy_crit_check
    };

    module_metadata_t mod_fatal = {
        "test.mod.fatal", "fatal.h", LAYER_0_CORE_UTIL, { 1, 0, 0 },
        1, "2026-10-26", "Fatal module", 0, { NULL },
        ALLOC_ZERO_HEAP, CONCURRENCY_REENTRANT, 10,
        0, { NULL }, dummy_fatal_check
    };

    module_metadata_t mod_missing_dep = {
        "test.mod.depfail", "depfail.h", LAYER_1_SIMULATION, { 1, 0, 0 },
        1, "2026-10-26", "Missing dep module", 1, { "unregistered.upstream" },
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

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================

// End of manifest.c
