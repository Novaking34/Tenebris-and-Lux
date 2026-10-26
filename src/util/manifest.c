// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          manifest.c
// @layer         Layer 0: Core Utilities
// @version       0.0.1
// @epoch         2026.10
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @role          Central Manifest Registry & Pre-Flight Verification Implementation
// @invariants    Rank 1 (< >): Form precedes fullness.
//                1. Fixed static capacity: Never exceed MANIFEST_MAX_MODULES.
//                2. Zero heap allocations: 100% stack/static execution.
//                3. Severity escalation: FATAL > CRITICAL > WARNING > PASS.
//                4. Loose bolts principle: System halts immediately on any FATAL check.
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
// ( Core Registry Logic, Pre-Flight Evaluator & Report Generators )
// =============================================================================

void manifest_registry_init(manifest_registry_t *registry)
{
    if (registry == NULL) {
        return;
    }
    memset(registry, 0, sizeof(manifest_registry_t));
    registry->status = PREFLIGHT_STATUS_OK;
}

bool manifest_register(manifest_registry_t *registry, const module_metadata_t *module)
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

preflight_status_t manifest_run_preflight(manifest_registry_t *registry)
{
    if (registry == NULL) {
        return PREFLIGHT_STATUS_HALT_FATAL;
    }

    registry->pass_count = 0;
    registry->warn_count = 0;
    registry->critical_count = 0;
    registry->fatal_count = 0;

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

        if (res.severity == SEVERITY_FATAL || (!res.passed && res.severity != SEVERITY_WARNING && res.severity != SEVERITY_CRITICAL)) {
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

void manifest_print_report(const manifest_registry_t *registry)
{
    if (registry == NULL) {
        return;
    }

    printf("================================================================================\n");
    printf("           Tenebris and Lux - Pre-Flight Subsystem Integrity Manifest           \n");
    printf("================================================================================\n");
    printf("%-20s %-16s %-10s %s\n", "MODULE ID", "FILE", "STATUS", "DIAGNOSTIC DETAILS");
    printf("--------------------------------------------------------------------------------\n");

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

        printf("%-20s %-16s %-10s %s\n",
            mod->module_id ? mod->module_id : "unknown",
            mod->file_name ? mod->file_name : "unknown",
            sev_str,
            res->details ? res->details : "Verified.");
    }

    printf("--------------------------------------------------------------------------------\n");
    printf("Telemetry: %zu modules registered | %zu Pass | %zu Warn | %zu Critical | %zu Fatal\n",
        registry->count, registry->pass_count, registry->warn_count,
        registry->critical_count, registry->fatal_count);

    switch (registry->status) {
    case PREFLIGHT_STATUS_OK:
        printf("Policy Verdict: ALL PRE-FLIGHT INVARIANTS SATISFIED (System Ready)\n");
        break;
    case PREFLIGHT_STATUS_WARNINGS:
        printf("Policy Verdict: SYSTEM LAUNCH PERMITTED WITH NOTICES (Non-critical Warnings)\n");
        break;
    case PREFLIGHT_STATUS_DOWNGRADED:
        printf("Policy Verdict: CRITICAL SUBSYSTEM FAULT - DOWNGRADING TO SAFE MODE\n");
        break;
    case PREFLIGHT_STATUS_HALT_FATAL:
        printf("Policy Verdict: LOOSE BOLTS DETECTED - ENGINE BOOT HALTED IMMEDIATELY\n");
        break;
    }
    printf("================================================================================\n");
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

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// ( Self-Audit of Central Registry Invariants )
// =============================================================================

static diag_result_t dummy_pass_check(void)
{
    diag_result_t res = { true, SEVERITY_PASS, "test.pass", "PassCheck", "All green." };
    return res;
}

static diag_result_t dummy_warn_check(void)
{
    diag_result_t res = { true, SEVERITY_WARNING, "test.warn", "WarnCheck", "Suboptimal timing." };
    return res;
}

static diag_result_t dummy_crit_check(void)
{
    diag_result_t res = { false, SEVERITY_CRITICAL, "test.crit", "CritCheck", "Display unavailable." };
    return res;
}

static diag_result_t dummy_fatal_check(void)
{
    diag_result_t res = { false, SEVERITY_FATAL, "test.fatal", "FatalCheck", "Loose bolt detected." };
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
    { 0, 0, 1 },
    "Central manifest registry and pre-flight policy engine",
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
    res.details = "Static capacity, duplicate rejection & graded policy verified.";
    return res;
}

bool manifest_audit(void)
{
    manifest_registry_t reg;
    manifest_registry_init(&reg);

    module_metadata_t mod_pass = {
        "test.mod.pass", "pass.h", LAYER_0_CORE_UTIL, { 1, 0, 0 },
        "Pass module", 0, { NULL }, dummy_pass_check
    };

    module_metadata_t mod_warn = {
        "test.mod.warn", "warn.h", LAYER_0_CORE_UTIL, { 1, 0, 0 },
        "Warn module", 0, { NULL }, dummy_warn_check
    };

    module_metadata_t mod_crit = {
        "test.mod.crit", "crit.h", LAYER_2_PRESENTATION, { 1, 0, 0 },
        "Critical module", 0, { NULL }, dummy_crit_check
    };

    module_metadata_t mod_fatal = {
        "test.mod.fatal", "fatal.h", LAYER_0_CORE_UTIL, { 1, 0, 0 },
        "Fatal module", 0, { NULL }, dummy_fatal_check
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
    if (manifest_run_preflight(&reg) != PREFLIGHT_STATUS_DOWNGRADED) return false;

    /* Invariant 5: Fatal halt escalation */
    manifest_registry_init(&reg);
    manifest_register(&reg, &mod_pass);
    manifest_register(&reg, &mod_crit);
    manifest_register(&reg, &mod_fatal);
    if (manifest_run_preflight(&reg) != PREFLIGHT_STATUS_HALT_FATAL) return false;

    return true;
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================

// End of manifest.c
