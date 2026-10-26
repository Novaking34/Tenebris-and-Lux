// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @id            core.util.manifest
// @file          manifest.h
// @layer         Layer 0: Core Utilities
// @version       0.0.2 (Phase 1: Dual-Frontend Prototype)
// @interface_rev Rev 2 (Added DAG dependencies, allocation & concurrency contracts)
// @date          2026-10-26
// @updated       2026-10-26
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Central manifest registry and pre-flight system verification types.
//   Transforms file metadata into compiled, machine-inspectable contracts and
//   enforces the "loose bolts" pre-flight verification policy.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: None (Foundational leaf module).
//   - Memory Allocation:     ALLOC_ZERO_HEAP (100% stack and static execution).
//   - Concurrency / Safety:  REENTRANT_PURE (Thread-safe, static capacity bounds).
//
// AXIOMATIC INVARIANTS:
//   1. Static allocation bounds: Capacity hard-capped at MANIFEST_MAX_MODULES.
//   2. Severity taxonomy strictly enforced: PASS < INFO < WARN < CRITICAL < FATAL.
//   3. Fatal errors unconditionally halt engine launch; critical faults trigger safe downgrade.
//   4. Active DAG dependency validation: Missing upstream dependencies trigger SEVERITY_FATAL.
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

#define MANIFEST_MAX_MODULES      32
#define MANIFEST_MAX_INVARIANTS   8
#define MANIFEST_MAX_DEPENDENCIES 4

typedef enum {
    LAYER_0_CORE_UTIL = 0,
    LAYER_1_SIMULATION,
    LAYER_2_PRESENTATION,
    LAYER_3_APPLICATION,
    LAYER_4_TESTS,
    LAYER_COUNT
} module_layer_t;

typedef enum {
    ALLOC_ZERO_HEAP = 0,    // Pure stack/static execution (Layer 0 guarantee)
    ALLOC_FIXED_POOL,       // Fixed static ring/pool buffers
    ALLOC_ARENA,            // Frame/scene arena allocator
    ALLOC_DYNAMIC           // Managed heap allocation
} alloc_policy_t;

typedef enum {
    CONCURRENCY_REENTRANT = 0, // Thread-safe, stateless pure functions
    CONCURRENCY_THREAD_LOCAL,  // Per-thread state
    CONCURRENCY_MAIN_THREAD    // Viewport/GUI event loop affinity
} concurrency_policy_t;

typedef enum {
    SEVERITY_PASS = 0,
    SEVERITY_INFO,
    SEVERITY_WARNING,
    SEVERITY_CRITICAL,  // Eligible for feature downgrade (e.g. GUI -> CLI)
    SEVERITY_FATAL      // Unrecoverable hard halt ("Loose bolts")
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
    /* 1. Identity & Provenance */
    const char *module_id;          // e.g. "core.util.time"
    const char *file_name;          // e.g. "time_utils.h"
    module_layer_t layer;           // Stratum in the architectural hierarchy
    struct {
        uint8_t major;
        uint8_t minor;
        uint8_t patch;
    } version;
    uint8_t interface_rev;          // Contract ABI revision
    const char *last_updated;       // ISO 8601 full date: "YYYY-MM-DD"
    const char *description;        // Plain-English mission statement
    
    /* 2. Architectural Dependencies */
    size_t dependency_count;
    const char *dependencies[MANIFEST_MAX_DEPENDENCIES]; // Upstream module IDs required
    
    /* 3. Resource & Execution Contracts */
    alloc_policy_t alloc_policy;
    concurrency_policy_t concurrency;
    uint32_t max_budget_us;         // Max execution latency budget (in microseconds)
    
    /* 4. Invariant Contracts */
    size_t invariant_count;
    const char *invariants[MANIFEST_MAX_INVARIANTS];
    
    /* 5. Pre-Flight Machine Self-Audit */
    diag_result_t (*preflight_check)(void);
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
const char *manifest_alloc_policy_to_str(alloc_policy_t policy);
const char *manifest_concurrency_to_str(concurrency_policy_t policy);

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
