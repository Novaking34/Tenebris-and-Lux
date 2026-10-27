// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          manifest.h
// @id            core.util.manifest
// @layer         Layer 0: Core Utilities
// @version       1.0.0
// @interface_rev Rev 2
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
//   - Memory Allocation:     ALLOC_ZERO_HEAP (100% stack and static memory).
//   - Concurrency / Safety:  REENTRANT_PURE (Thread-safe, static capacity).
//   - Latency Budget:        < 5 milliseconds total pre-flight evaluation.
//
// AXIOMATIC INVARIANTS:
//   1. Static allocation bounds: Capacity hard-capped at MANIFEST_MAX_MODULES.
//   2. Severity taxonomy: PASS < INFO < WARN < CRITICAL < FATAL.
//   3. Fatal errors unconditionally halt boot; critical faults downgrade.
//   4. Active DAG dependency validation: Missing dependencies trigger FATAL.
// =============================================================================

#ifndef TENEBRIS_LUX_MANIFEST_H
#define TENEBRIS_LUX_MANIFEST_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: Central Manifest Registry
// =============================================================================

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define MANIFEST_MAX_MODULES      32
#define MANIFEST_MAX_INVARIANTS   8
#define MANIFEST_MAX_DEPENDENCIES 4

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 2.1: ARCHITECTURAL METADATA ENUMS & STRUCTS ]
// Mission: Define structural strata, allocation models, and fault severities.
// -----------------------------------------------------------------------------

/**
 * Architectural layer strata within the hierarchical engine topology.
 */
typedef enum {
    LAYER_0_CORE_UTIL = 0, /**< Layer 0: Foundational zero-dependency utils. */
    LAYER_1_SIMULATION,    /**< Layer 1: Headless game simulation engine. */
    LAYER_2_PRESENTATION,  /**< Layer 2: Graphical and text viewports. */
    LAYER_3_APPLICATION,   /**< Layer 3: Application entry and routing. */
    LAYER_4_TESTS,         /**< Layer 4: Verification test harness suites. */
    LAYER_COUNT
} module_layer_t;

/**
 * Memory allocation contract governing a module's execution model.
 */
typedef enum {
    ALLOC_ZERO_HEAP = 0, /**< 100% stack or static layout; zero dynamic heap. */
    ALLOC_FIXED_POOL,    /**< Pre-allocated fixed-capacity buffer pools. */
    ALLOC_ARENA,         /**< Frame/scene temporary arena allocation. */
    ALLOC_DYNAMIC        /**< Managed dynamic heap allocation. */
} alloc_policy_t;

/**
 * Concurrency and reentrancy guarantees declared by a module.
 */
typedef enum {
    CONCURRENCY_REENTRANT = 0, /**< Thread-safe, stateless pure functions. */
    CONCURRENCY_THREAD_LOCAL,  /**< Thread-local state isolation. */
    CONCURRENCY_MAIN_THREAD    /**< Main-thread execution affinity (GUI). */
} concurrency_policy_t;

/**
 * Five-tier diagnostic fault severity taxonomy for pre-flight machine audits.
 */
typedef enum {
    SEVERITY_PASS = 0, /**< Invariant holds cleanly; full readiness. */
    SEVERITY_INFO,     /**< Informational notice; no operational effect. */
    SEVERITY_WARNING,  /**< Non-fatal anomaly logged; execution proceeds. */
    SEVERITY_CRITICAL, /**< Subsystem offline; eligible for safe downgrade. */
    SEVERITY_FATAL     /**< Loose bolt detected; unrecoverable hard halt. */
} diag_severity_t;

/**
 * Aggregate pre-flight engine readiness state.
 */
typedef enum {
    PREFLIGHT_STATUS_OK = 0,    /**< Subsystems passed pre-flight audits. */
    PREFLIGHT_STATUS_WARNINGS,   /**< Non-fatal notices logged; system ready. */
    PREFLIGHT_STATUS_DOWNGRADED, /**< Critical fault downgraded to safe mode. */
    PREFLIGHT_STATUS_HALT_FATAL  /**< Loose bolt detected; engine halt. */
} preflight_status_t;

/**
 * Individual diagnostic check outcome record.
 */
typedef struct {
    bool passed;               /**< Boolean pass/fail indicator. */
    diag_severity_t severity;  /**< Severity classification of the finding. */
    const char *subsystem;     /**< Identifier of the audited subsystem. */
    const char *check_name;    /**< Descriptive name of the specific audit. */
    const char *details;       /**< Human-readable diagnostic details. */
} diag_result_t;

/**
 * Compiled module metadata node binding identity, contracts, and tripwires.
 *
 * ### Machine Role & Mission
 *
 * Serves as the static identity card for every source module in the engine.
 * Declares architectural stratum, interface revision, full ISO update date,
 * memory model, concurrency guarantees, DAG upstream dependencies, and the
 * machine self-audit function pointer.
 */
typedef struct {
    /* 1. Identity & Provenance */
    const char *module_id;          /**< Canonical module ID. */
    const char *file_name;          /**< Header file name. */
    module_layer_t layer;           /**< Architectural stratum. */
    struct {
        uint8_t major;
        uint8_t minor;
        uint8_t patch;
    } version;                      /**< Semantic version triplet. */
    uint8_t interface_rev;          /**< Contract ABI revision counter. */
    const char *last_updated;       /**< Full ISO 8601 date: YYYY-MM-DD. */
    const char *description;        /**< Plain-English mission statement. */

    /* 2. Architectural Dependencies */
    size_t dependency_count;        /**< Number of declared prerequisites. */
    const char *dependencies[MANIFEST_MAX_DEPENDENCIES]; /**< Upstream IDs. */

    /* 3. Resource & Execution Contracts */
    alloc_policy_t alloc_policy;     /**< Memory allocation guarantee. */
    concurrency_policy_t concurrency;/**< Concurrency / thread safety model. */
    uint32_t max_budget_us;          /**< Max latency budget (microseconds). */

    /* 4. Invariant Contracts */
    size_t invariant_count;         /**< Number of documented invariants. */
    const char *invariants[MANIFEST_MAX_INVARIANTS]; /**< Invariant rules. */

    /* 5. Pre-Flight Machine Self-Audit */
    diag_result_t (*preflight_check)(void); /**< Diagnostic callback. */
} module_metadata_t;

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 2.2: CENTRAL REGISTRY RECORD ]
// Mission: Fixed-capacity zero-heap storage for all registered engine modules.
// -----------------------------------------------------------------------------

/**
 * Central registry tracking registered modules, audit results, and status.
 */
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
// Stratum: Layer 0 (Core Utilities) | Machine: Central Manifest Registry
// =============================================================================

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 3.1: CENTRAL REGISTRY LIFECYCLE & REGISTRATION COGS ]
// Mission: Initialize registry storage and register subsystem metadata nodes.
// -----------------------------------------------------------------------------

/**
 * Initializes a central manifest registry to zeroed, pristine state.
 *
 * @param[out] registry Pointer to uninitialized registry structure.
 */
void manifest_registry_init(manifest_registry_t *registry);

/**
 * Registers an immutable module metadata node into the central registry.
 *
 * @param[in,out] registry Target registry instance.
 * @param[in]     module   Pointer to static module metadata record.
 *
 * @return `true` if registration succeeded; `false` on duplicate or overflow.
 */
bool manifest_register(manifest_registry_t *registry,
                       const module_metadata_t *module);

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 3.2: DAG DEPENDENCY RESOLUTION & PRE-FLIGHT EVALUATION ]
// Mission: Execute DAG dependency graph audits and pre-flight machine checks.
// -----------------------------------------------------------------------------

/**
 * Executes full pre-flight evaluation across all registered engine modules.
 *
 * Step 1: Validates upstream DAG dependency graph. Missing modules yield FATAL.
 * Step 2: Executes each module's diagnostic `preflight_check` callback.
 * Step 3: Computes final system policy readiness state.
 *
 * @param[in,out] registry Initialized registry with registered modules.
 *
 * @return Overall preflight status (`OK`, `WARNINGS`, `DOWNGRADED`, `FATAL`).
 */
preflight_status_t manifest_run_preflight(manifest_registry_t *registry);

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 3.3: TELEMETRY REPORTING & SERIALIZATION COGS ]
// Mission: Format pre-flight telemetry tables and serialize taxonomy strings.
// -----------------------------------------------------------------------------

/**
 * Prints a formatted 80-column ASCII pre-flight audit report to stdout.
 *
 * Displays build telemetry timestamp, target standard, and an inspection
 * matrix containing module ID, header file, revision, allocation model,
 * status tag, and diagnostic trace.
 *
 * @param[in] registry Evaluated registry instance.
 */
void manifest_print_report(const manifest_registry_t *registry);

const char *manifest_severity_to_str(diag_severity_t severity);
const char *manifest_status_to_str(preflight_status_t status);
const char *manifest_layer_to_str(module_layer_t layer);
const char *manifest_alloc_policy_to_str(alloc_policy_t policy);
const char *manifest_concurrency_to_str(concurrency_policy_t policy);

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: Central Manifest Registry
// =============================================================================

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 4.1: MACHINE SELF-AUDIT & PRE-FLIGHT TRIPWIRES ]
// Mission: Verify registry capacity bounds, duplicate rejection, and policies.
// -----------------------------------------------------------------------------

extern const module_metadata_t manifest_utils_manifest;
diag_result_t manifest_utils_preflight(void);
bool manifest_audit(void);

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( Include Guard Teardown & Header Close )
// =============================================================================

#endif /* TENEBRIS_LUX_MANIFEST_H */
