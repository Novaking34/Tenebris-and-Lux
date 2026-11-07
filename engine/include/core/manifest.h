/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          manifest.h
@id            core.util.manifest
@layer         Layer 0: Core Utilities
@version       0.1.0-dev
@interface_rev Rev 4
@date          2026-11-07
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Central Manifest Registry & Pre-Flight DAG Policy Engine
@role          Metadata Types, DAG Dependency Graph & Pre-Flight Contracts
@invariants    1. Zero dynamic heap allocations (ALLOC_ZERO_HEAP: stack/static only).
               2. Static allocation bounds: Capacity hard-capped at MANIFEST_MAX_MODULES.
               3. Severity taxonomy: PASS < INFO < WARNING < CRITICAL < FATAL.
               4. Fatal errors unconditionally halt boot; critical faults downgrade.
               5. Active DAG dependency validation: Missing dependencies trigger FATAL.

PURPOSE & ARCHITECTURAL ROLE:
  Central manifest registry and pre-flight system verification types.
  Transforms module metadata into compiled, machine-inspectable contracts and
  enforces the "loose bolts" pre-flight verification policy across all engine
  and game simulation strata before runtime simulation commences.
================================================================================
*/

#ifndef TENEBRIS_LUX_CORE_MANIFEST_H
#define TENEBRIS_LUX_CORE_MANIFEST_H

/*
================================================================================
BLOCK 2: OPENING / TYPES & STATIC DATA BLOCK
Stratum: Layer 0 (Core Utilities) | Machine: Central Manifest Registry
================================================================================
*/

/* --- Tier 1: Standard Library Headers --- */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* --- Tier 2: Core Engine Specifications & Bounds --- */
#define MANIFEST_MAX_MODULES      32
#define MANIFEST_MAX_INVARIANTS   8
#define MANIFEST_MAX_DEPENDENCIES 4

/*
--------------------------------------------------------------------------------
[ SUB-ASSEMBLY 2.1: ARCHITECTURAL METADATA ENUMS & STRUCTS ]
Mission: Define structural strata, allocation models, and fault severities.
--------------------------------------------------------------------------------
*/

/**
 * @brief Architectural layer strata within the hierarchical engine topology.
 */
typedef enum module_layer {
    LAYER_0_CORE_UTIL = 0, /**< Layer 0: Foundational zero-dependency utils. */
    LAYER_1_SIMULATION,    /**< Layer 1: Headless game simulation engine. */
    LAYER_2_PRESENTATION,  /**< Layer 2: Graphical and text viewports. */
    LAYER_3_APPLICATION,   /**< Layer 3: Application entry and routing. */
    LAYER_4_TESTS,         /**< Layer 4: Verification test harness suites. */
    LAYER_COUNT
} module_layer_t;

/**
 * @brief Memory allocation contract governing a module's execution model.
 */
typedef enum alloc_policy {
    ALLOC_ZERO_HEAP = 0, /**< 100% stack or static layout; zero dynamic heap. */
    ALLOC_FIXED_POOL,    /**< Pre-allocated fixed-capacity buffer pools. */
    ALLOC_ARENA,         /**< Frame/scene temporary arena allocation. */
    ALLOC_DYNAMIC        /**< Managed dynamic heap allocation. */
} alloc_policy_t;

/**
 * @brief Concurrency and reentrancy guarantees declared by a module.
 */
typedef enum concurrency_policy {
    CONCURRENCY_REENTRANT = 0, /**< Thread-safe, stateless pure functions. */
    CONCURRENCY_THREAD_LOCAL,  /**< Thread-local state isolation. */
    CONCURRENCY_MAIN_THREAD    /**< Main-thread execution affinity (GUI). */
} concurrency_policy_t;

/**
 * @brief Five-tier diagnostic fault severity taxonomy for pre-flight machine audits.
 */
typedef enum diag_severity {
    SEVERITY_PASS = 0, /**< Invariant holds cleanly; full readiness. */
    SEVERITY_INFO,     /**< Informational notice; no operational effect. */
    SEVERITY_WARNING,  /**< Non-fatal anomaly logged; execution proceeds. */
    SEVERITY_CRITICAL, /**< Subsystem offline; eligible for safe downgrade. */
    SEVERITY_FATAL     /**< Loose bolt detected; unrecoverable hard halt. */
} diag_severity_t;

/**
 * @brief Aggregate pre-flight engine readiness state.
 */
typedef enum preflight_status {
    PREFLIGHT_STATUS_OK = 0,    /**< Subsystems passed pre-flight audits. */
    PREFLIGHT_STATUS_WARNINGS,   /**< Non-fatal notices logged; system ready. */
    PREFLIGHT_STATUS_DOWNGRADED, /**< Critical fault downgraded to safe mode. */
    PREFLIGHT_STATUS_HALT_FATAL  /**< Loose bolt detected; engine halt. */
} preflight_status_t;

/**
 * @brief Individual diagnostic check outcome record.
 */
typedef struct diag_result {
    bool passed;               /**< Boolean pass/fail indicator. */
    diag_severity_t severity;  /**< Severity classification of the finding. */
    const char *subsystem;     /**< Identifier of the audited subsystem. */
    const char *check_name;    /**< Descriptive name of the specific audit. */
    const char *details;       /**< Human-readable diagnostic details. */
} diag_result_t;

/**
 * @brief Compiled module metadata node binding identity, contracts, and tripwires.
 *
 * Serves as the static identity card for every source module in the engine.
 * Declares architectural stratum, interface revision, full ISO update date,
 * memory model, concurrency guarantees, DAG upstream dependencies, and the
 * machine self-audit function pointer.
 */
typedef struct module_metadata {
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

/*
--------------------------------------------------------------------------------
[ SUB-ASSEMBLY 2.2: CENTRAL REGISTRY RECORD ]
Mission: Fixed-capacity zero-heap storage for all registered engine modules.
--------------------------------------------------------------------------------
*/

/**
 * @brief Central registry tracking registered modules, audit results, and status.
 */
typedef struct manifest_registry {
    const module_metadata_t *modules[MANIFEST_MAX_MODULES];
    diag_result_t results[MANIFEST_MAX_MODULES];
    size_t count;
    size_t pass_count;
    size_t warn_count;
    size_t critical_count;
    size_t fatal_count;
    preflight_status_t status;
} manifest_registry_t;

/*
================================================================================
BLOCK 3: BODY / CORE REGISTRY API BLOCK
Stratum: Layer 0 (Core Utilities) | Machine: Central Manifest Registry
================================================================================
*/

/**
 * @brief Initializes a central manifest registry to zeroed, pristine state.
 *
 * @param[out] registry Pointer to uninitialized caller-allocated registry structure.
 */
void manifest_registry_init(manifest_registry_t *registry);

/**
 * @brief Registers an immutable module metadata node into the central registry.
 *
 * Collects compiled module records into the centralized pre-flight evaluation
 * registry. Rejects duplicate module IDs and enforces MANIFEST_MAX_MODULES.
 *
 * @param[in,out] registry Target registry collecting module records.
 * @param[in]     module   Pointer to static immutable module metadata record.
 * @return true if registration succeeded; false on duplicate ID or overflow.
 */
bool manifest_register(manifest_registry_t *registry,
                       const module_metadata_t *module);

/**
 * @brief Executes full pre-flight evaluation across all registered engine modules.
 *
 * Step 1: Validates upstream DAG dependency graph. Missing modules yield FATAL.
 * Step 2: Executes each module's diagnostic preflight_check callback.
 * Step 3: Computes final system policy readiness state.
 *
 * @param[in,out] registry Initialized registry with registered modules.
 * @return Overall preflight status (OK, WARNINGS, DOWNGRADED, HALT_FATAL).
 */
preflight_status_t manifest_run_preflight(manifest_registry_t *registry);

/**
 * @brief Prints a formatted 80-column ASCII pre-flight audit report to stdout.
 *
 * @param[in] registry Evaluated registry instance.
 */
void manifest_print_report(const manifest_registry_t *registry);

/**
 * @brief Converts a diagnostic severity code to a human-readable string.
 *
 * @param[in] severity Diagnostic severity level enum value.
 * @return Null-terminated static string (e.g. "PASS", "WARNING", "FATAL").
 */
const char *manifest_severity_to_str(diag_severity_t severity);

/**
 * @brief Converts an aggregate preflight status code to a human-readable string.
 *
 * @param[in] status Aggregate preflight status enum value.
 * @return Null-terminated static string (e.g. "READY", "WARNINGS", "HALTED").
 */
const char *manifest_status_to_str(preflight_status_t status);

/**
 * @brief Converts an architectural layer stratum code to a descriptive string.
 *
 * @param[in] layer Architectural layer stratum enum value.
 * @return Null-terminated static string (e.g. "Layer 0: Core Utilities").
 */
const char *manifest_layer_to_str(module_layer_t layer);

/**
 * @brief Converts a memory allocation policy code to a human-readable string.
 *
 * @param[in] policy Memory allocation contract policy enum value.
 * @return Null-terminated static string (e.g. "Zero-Heap", "Fixed-Pool").
 */
const char *manifest_alloc_policy_to_str(alloc_policy_t policy);

/**
 * @brief Converts a concurrency contract policy code to a human-readable string.
 *
 * @param[in] policy Concurrency and thread safety policy enum value.
 * @return Null-terminated static string (e.g. "Reentrant", "Main-Thread").
 */
const char *manifest_concurrency_to_str(concurrency_policy_t policy);

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 0 (Core Utilities) | Machine: Central Manifest Registry
================================================================================
*/

/**
 * @brief Manifest metadata record for the Central Manifest Registry module.
 */
extern const module_metadata_t manifest_utils_manifest;

/**
 * @brief Pre-flight diagnostic check evaluating manifest registry invariants.
 *
 * Verifies static capacity, duplicate rejection, and policy escalation rules.
 *
 * @return Diagnostic check outcome reporting SEVERITY_PASS or SEVERITY_FATAL.
 */
diag_result_t manifest_utils_preflight(void);

/**
 * @brief Standalone boolean self-audit testing manifest invariants.
 *
 * @return true if all manifest tests pass cleanly; false on violation.
 */
bool manifest_audit(void);

/*
================================================================================
BLOCK 5: CLOSING & DIAGNOSTIC CARD DUMP BLOCK
================================================================================
*/

/**
 * @brief Renders a 60-column terminal-friendly ASCII inspection card of the registry.
 *
 * @param[in]  registry Pointer to evaluated registry.
 * @param[out] out_buf  Destination buffer for ASCII card output.
 * @param[in]  buf_size Size of destination buffer in bytes.
 * @return true if rendered successfully; false on NULL or truncation.
 */
bool manifest_execute_dump_card(const manifest_registry_t *registry,
                               char *out_buf,
                               size_t buf_size);

#endif /* TENEBRIS_LUX_CORE_MANIFEST_H */
