// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          manifest.h
// @layer         Layer 0: Core Utilities
// @version       0.0.1
// @epoch         2026.10
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @role          Central Manifest Registry & Pre-Flight System Verification Types
// @invariants    Rank 1 (< >): Form precedes fullness.
//                1. Zero dynamic allocations: Registry operates within fixed static bounds.
//                2. Severity taxonomy strictly enforced: PASS < INFO < WARN < CRITICAL < FATAL.
//                3. Fatal errors unconditionally halt engine launch; critical faults trigger downgrade.
//                4. Metadata contracts are immutable, reentrant, and thread-safe.
// =============================================================================

#ifndef TENEBRIS_LUX_MANIFEST_H
#define TENEBRIS_LUX_MANIFEST_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// ( Standard Headers, Severity Enums, Metadata Structs & Registry Layout )
// =============================================================================

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define MANIFEST_MAX_MODULES    32
#define MANIFEST_MAX_INVARIANTS 8

typedef enum {
    LAYER_0_CORE_UTIL = 0,
    LAYER_1_SIMULATION,
    LAYER_2_PRESENTATION,
    LAYER_3_APPLICATION,
    LAYER_4_TESTS,
    LAYER_COUNT
} module_layer_t;

typedef enum {
    SEVERITY_PASS = 0,
    SEVERITY_INFO,
    SEVERITY_WARNING,
    SEVERITY_CRITICAL,  // Eligible for feature downgrade
    SEVERITY_FATAL      // Unrecoverable hard halt
} diag_severity_t;

typedef enum {
    PREFLIGHT_STATUS_OK = 0,
    PREFLIGHT_STATUS_WARNINGS,
    PREFLIGHT_STATUS_DOWNGRADED,
    PREFLIGHT_STATUS_HALT_FATAL
} preflight_status_t;

typedef struct {
    bool passed;
    diag_severity_t severity;
    const char *subsystem;
    const char *check_name;
    const char *details;
} diag_result_t;

typedef struct {
    const char *module_id;          // e.g. "core.util.time"
    const char *file_name;          // e.g. "time_utils.h"
    module_layer_t layer;           // Stratum in the architectural hierarchy
    struct {
        uint8_t major;
        uint8_t minor;
        uint8_t patch;
    } version;
    const char *description;
    size_t invariant_count;
    const char *invariants[MANIFEST_MAX_INVARIANTS];
    diag_result_t (*preflight_check)(void); // Machine integrity self-test
} module_metadata_t;

typedef struct {
    const module_metadata_t *modules[MANIFEST_MAX_MODULES];
    diag_result_t results[MANIFEST_MAX_MODULES];
    size_t count;
    size_t pass_count;
    size_t warn_count;
    size_t critical_count;
    size_t fatal_count;
    preflight_status_t status;
} manifest_registry_t;

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Central Registry Operations, Policy Evaluator & Reporting Functions )
// =============================================================================

void manifest_registry_init(manifest_registry_t *registry);
bool manifest_register(manifest_registry_t *registry, const module_metadata_t *module);
preflight_status_t manifest_run_preflight(manifest_registry_t *registry);
void manifest_print_report(const manifest_registry_t *registry);
const char *manifest_severity_to_str(diag_severity_t severity);
const char *manifest_status_to_str(preflight_status_t status);
const char *manifest_layer_to_str(module_layer_t layer);

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// ( Self-Audit of Central Registry Invariants )
// =============================================================================

extern const module_metadata_t manifest_utils_manifest;
diag_result_t manifest_utils_preflight(void);
bool manifest_audit(void);

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( Include Guard Teardown & Header Close )
// =============================================================================

#endif /* TENEBRIS_LUX_MANIFEST_H */
