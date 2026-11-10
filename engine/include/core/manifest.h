/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          manifest.h
@id            core.util.manifest
@layer         Layer 0: Core Utilities
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-08
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Central Manifest Registry & Pre-Flight DAG Policy Engine
@role          Metadata Types, DAG Dependency Graph & Pre-Flight Contracts
@invariants    1. Zero dynamic heap allocations (ALLOC_ZERO_HEAP: stack/static only).
               2. Strict ISO C11 adherence (-std=c11, -Wall -Wextra -Wpedantic).
               3. Static allocation bounds: Capacity hard-capped at MANIFEST_MAX_MODULES.
               4. Severity taxonomy: PASS < INFO < WARNING < CRITICAL < FATAL.
               5. Fatal errors unconditionally halt boot; critical faults downgrade.
               6. Active DAG dependency validation: Missing dependencies trigger FATAL.
               7. Universal 5-Block Lifecycle Protocol enforced across header and source.

PURPOSE & ARCHITECTURAL ROLE:
  Central manifest registry and pre-flight system verification types.
  Transforms module metadata into compiled, machine-inspectable contracts and
  enforces the "loose bolts" pre-flight verification policy across all engine
  and game simulation strata before runtime simulation commences.

  ### For Game Designers & Non-Programmers:
  Before the game opens its main window, it runs a pre-flight inspection like
  an airliner before takeoff. It checks that all engine modules and subsystems
  are present, securely connected, and ready. If a critical component is
  missing, it alerts developers immediately instead of crashing during play.

  ### For Systems Programmers & Engine Architects:
  - Directed Acyclic Graph (DAG) dependency validation across Layer 0, 1, and 2.
  - Zero heap allocation: all registry tables and audit results reside in static .bss.
  - Categorized diagnostic severity taxonomy (`diag_severity_t`) with threshold masks.
  - Compile-time module identity tagging (`module_manifest_t`) for DWARF debugging.
================================================================================
*/

#ifndef TENEBRIS_LUX_CORE_MANIFEST_H
#define TENEBRIS_LUX_CORE_MANIFEST_H

/*
================================================================================
BLOCK 2: OPENING BLOCK / TYPES & DOMAIN ONTOLOGY
Stratum: Layer 0 (Core Utilities) | Subsystem: Central Manifest Registry
================================================================================
Mission: Pre-flight sizing limits, include ladder, metadata node ontology,
and pre-conditional static validation inlines for loose-bolts pre-flight audits.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Include Hierarchy: 3-Tier Downward Architecture
--------------------------------------------------------------------------------
Tier 1: Standard ISO C11 Library Headers
Tier 2: External Dependencies (None)
Tier 3: Internal Engine Foundation Headers (None - Bedrock Root)
--------------------------------------------------------------------------------
*/

/* --- Tier 1: Standard ISO C11 Library Headers --- */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* --- Tier 2: External Dependencies (None) --- */

/* --- Tier 3: Internal Engine Foundation Headers (None - Bedrock Root) --- */

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Pre-Conditions, Constants & Buffer Limits
--------------------------------------------------------------------------------
Compile-time sizing limits for registry module nodes, dependencies, and rules.
For Designers: Bounds ensure the engine registers up to 32 major game systems,
each with up to 8 core rules and 4 prerequisite dependencies, verifying all are
ready before the first screen appears.
For Programmers: Guarantees 100% zero-heap allocation (ALLOC_ZERO_HEAP) in .rodata.
--------------------------------------------------------------------------------
*/

/** Maximum number of registered engine/game subsystem metadata nodes. */
#define MANIFEST_MAX_MODULES      32

/** Maximum documented invariant rules declared per module node. */
#define MANIFEST_MAX_INVARIANTS   8

/** Maximum upstream prerequisite dependencies declared per module node. */
#define MANIFEST_MAX_DEPENDENCIES 4

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Manifest & Pre-Flight Hooks
--------------------------------------------------------------------------------
Bedrock self-manifest record for the Central Manifest Registry engine.
--------------------------------------------------------------------------------
*/

/** Unique module identifier string for the Core Manifest Registry & DAG engine. */
#define MANIFEST_MODULE_ID             "core.manifest"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Structural Entity & Object Definitions
--------------------------------------------------------------------------------
Ontological types for architectural strata, memory models, and diagnostic audits.
--------------------------------------------------------------------------------
*/

/**
 * @enum module_layer
 * @brief Architectural layer strata within the hierarchical engine topology.
 *
 * For Designers: The 5 vertical tiers of the game engine, from deep bedrock
 * utilities (Layer 0) up to the game rules (Layer 1) and graphical display (Layer 2).
 *
 * For Programmers: Strict downward dependency hierarchy enforced at compile time.
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
 * @enum alloc_policy
 * @brief Memory allocation contract governing a module's execution model.
 *
 * For Designers: Rules defining how a system manages memory, ensuring bedrock
 * systems never allocate heap memory to prevent crashes and stutter.
 *
 * For Programmers: Invariant memory policy verified during code audit.
 */
typedef enum alloc_policy {
    ALLOC_ZERO_HEAP = 0, /**< 100% stack or static layout; zero dynamic heap. */
    ALLOC_FIXED_POOL,    /**< Pre-allocated fixed-capacity buffer pools. */
    ALLOC_ARENA,         /**< Frame/scene temporary arena allocation. */
    ALLOC_DYNAMIC        /**< Managed dynamic heap allocation. */
} alloc_policy_t;

/**
 * @enum concurrency_policy
 * @brief Concurrency and reentrancy guarantees declared by a module.
 *
 * For Designers: Rules specifying whether a system can run simultaneously across
 * multiple CPU threads or must remain tied to the main display loop.
 *
 * For Programmers: Thread-safety model declared for scheduler evaluation.
 */
typedef enum concurrency_policy {
    CONCURRENCY_REENTRANT = 0, /**< Thread-safe, stateless pure functions. */
    CONCURRENCY_THREAD_LOCAL,  /**< Thread-local state isolation. */
    CONCURRENCY_MAIN_THREAD    /**< Main-thread execution affinity (GUI). */
} concurrency_policy_t;

/**
 * @enum diag_severity
 * @brief Five-tier diagnostic fault severity taxonomy for pre-flight machine audits.
 *
 * For Designers: A traffic light system for engine health:
 * PASS (green), INFO (blue notice), WARNING (yellow alert),
 * CRITICAL (orange, trigger safe fallback), FATAL (red, stop game safely).
 *
 * For Programmers: Strict escalation hierarchy: FATAL > CRITICAL > WARNING > PASS.
 */
typedef enum diag_severity {
    SEVERITY_PASS = 0, /**< Invariant holds cleanly; full readiness. */
    SEVERITY_INFO,     /**< Informational notice; no operational effect. */
    SEVERITY_WARNING,  /**< Non-fatal anomaly logged; execution proceeds. */
    SEVERITY_CRITICAL, /**< Subsystem offline; eligible for safe downgrade. */
    SEVERITY_FATAL     /**< Loose bolt detected; unrecoverable hard halt. */
} diag_severity_t;

/**
 * @enum preflight_status
 * @brief Aggregate pre-flight engine readiness state.
 *
 * For Designers: The overall health verdict after all engine systems test themselves.
 *
 * For Programmers: Composite state determined by highest severity finding across DAG.
 */
typedef enum preflight_status {
    PREFLIGHT_STATUS_OK = 0,    /**< Subsystems passed pre-flight audits. */
    PREFLIGHT_STATUS_WARNINGS,   /**< Non-fatal notices logged; system ready. */
    PREFLIGHT_STATUS_DOWNGRADED, /**< Critical fault downgraded to safe mode. */
    PREFLIGHT_STATUS_HALT_FATAL  /**< Loose bolt detected; engine halt. */
} preflight_status_t;

/**
 * @struct diag_result
 * @brief Individual diagnostic check outcome record.
 *
 * For Designers: A single test result card explaining if a test passed and any notes.
 *
 * For Programmers: Reentrant stack-allocated diagnostic result structure.
 */
typedef struct diag_result {
    bool            passed;     /**< Boolean pass/fail indicator. */
    diag_severity_t severity;   /**< Severity classification of the finding. */
    const char     *subsystem;  /**< Identifier of the audited subsystem. */
    const char     *check_name; /**< Descriptive name of the specific audit. */
    const char     *details;    /**< Human-readable diagnostic details. */
} diag_result_t;

/**
 * @struct module_metadata
 * @brief Compiled module metadata node binding identity, contracts, and tripwires.
 *
 * For Designers: The formal identity card of an engine subsystem, listing its name,
 * version, layer, who built it, what other systems it needs, and how to test it.
 *
 * For Programmers: Static immutable structure registered in central registry.
 */
typedef struct module_metadata {
    /* 1. Identity & Provenance */
    const char    *module_id;          /**< Canonical module ID. */
    const char    *file_name;          /**< Header file name. */
    module_layer_t layer;              /**< Architectural stratum. */
    struct {
        uint8_t major;
        uint8_t minor;
        uint8_t patch;
    } version;                         /**< Semantic version triplet. */
    uint8_t        interface_rev;      /**< Contract ABI revision counter. */
    const char    *last_updated;       /**< Full ISO 8601 date: YYYY-MM-DD. */
    const char    *description;        /**< Plain-English mission statement. */

    /* 2. Architectural Dependencies */
    size_t         dependency_count;   /**< Number of declared prerequisites. */
    const char    *dependencies[MANIFEST_MAX_DEPENDENCIES]; /**< Upstream IDs. */

    /* 3. Resource & Execution Contracts */
    alloc_policy_t       alloc_policy; /**< Memory allocation guarantee. */
    concurrency_policy_t concurrency;  /**< Concurrency / thread safety model. */
    uint32_t             max_budget_us;/**< Max latency budget (microseconds). */

    /* 4. Invariant Contracts */
    size_t         invariant_count;    /**< Number of documented invariants. */
    const char    *invariants[MANIFEST_MAX_INVARIANTS]; /**< Invariant rules. */

    /* 5. Pre-Flight Machine Self-Audit */
    diag_result_t (*preflight_check)(void); /**< Diagnostic callback. */
} module_metadata_t;

/**
 * @struct manifest_registry
 * @brief Central registry tracking registered modules, audit results, and status.
 *
 * For Designers: The master scoreboard storing every system's health report.
 *
 * For Programmers: Zero-heap fixed-capacity structure populated at engine boot.
 */
typedef struct manifest_registry {
    const module_metadata_t *modules[MANIFEST_MAX_MODULES]; /**< Pointers to registered module metadata nodes. */
    diag_result_t            results[MANIFEST_MAX_MODULES]; /**< Pre-flight check outcome records. */
    size_t                   count;                         /**< Number of currently registered modules. */
    size_t                   pass_count;                    /**< Total audits yielding SEVERITY_PASS. */
    size_t                   warn_count;                    /**< Total audits yielding SEVERITY_WARNING. */
    size_t                   critical_count;                /**< Total audits yielding SEVERITY_CRITICAL. */
    size_t                   fatal_count;                   /**< Total audits yielding SEVERITY_FATAL. */
    preflight_status_t       status;                        /**< Aggregate pre-flight system readiness status. */
} manifest_registry_t;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.4: Pre-Conditional Helper Cogs & Non-Business Inlines
--------------------------------------------------------------------------------
Pure static inline validation predicates and non-business query helpers.
Guarantees zero heap, pure reentrancy, and fast pre-flight assertion checks.
--------------------------------------------------------------------------------
*/

/**
 * @brief Predicate verifying if a diagnostic severity code is recognized.
 *
 * @param[in] sev Severity value to check.
 * @return true if severity is within [SEVERITY_PASS .. SEVERITY_FATAL].
 */
static inline bool diag_severity_is_valid(diag_severity_t sev) {
    return sev >= SEVERITY_PASS && sev <= SEVERITY_FATAL;
}

/**
 * @brief Predicate verifying if a diagnostic check outcome passed cleanly.
 *
 * @param[in] res Pointer to diagnostic result record.
 * @return true if res is non-NULL, passed is true, and severity is SEVERITY_PASS.
 */
static inline bool diag_result_is_pass(const diag_result_t *res) {
    return (res != NULL) && res->passed && (res->severity == SEVERITY_PASS);
}

/**
 * @brief Predicate verifying if an architectural module layer is valid.
 *
 * @param[in] layer Layer value to check.
 * @return true if layer is within [LAYER_0_CORE_UTIL .. LAYER_4_TESTS].
 */
static inline bool module_layer_is_valid(module_layer_t layer) {
    return layer >= LAYER_0_CORE_UTIL && layer < LAYER_COUNT;
}

/**
 * @brief Predicate verifying if an allocation policy is valid.
 *
 * @param[in] policy Memory allocation policy to check.
 * @return true if policy is within [ALLOC_ZERO_HEAP .. ALLOC_DYNAMIC].
 */
static inline bool alloc_policy_is_valid(alloc_policy_t policy) {
    return policy >= ALLOC_ZERO_HEAP && policy <= ALLOC_DYNAMIC;
}

/*
================================================================================
BLOCK 3: BODY BLOCK / CORE REGISTRY API & BUSINESS FUNCTIONS
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
For Designers: Ensures all engine subsystems (sound, graphics, combat, items)
are counted and checked before the game starts.
For Programmers: Static array insertion with duplicate rejection and zero heap.
--------------------------------------------------------------------------------
*/

/**
 * @brief Initializes a central manifest registry to zeroed, pristine state.
 *
 * For Designers:
 * Clears the engine's master checklist so each game subsystem can check in
 * fresh at boot time.
 *
 * For Programmers:
 * Bounded memset zeroing of the registry structure and sets status to
 * PREFLIGHT_STATUS_OK.
 *
 * @param[out] registry Pointer to uninitialized caller-allocated registry structure.
 */
void manifest_registry_init(manifest_registry_t *registry);

/**
 * @brief Registers an immutable module metadata node into the central registry.
 *
 * For Designers:
 * Adds a subsystem's specification card to the engine's pre-flight roster,
 * making sure no duplicate systems are running.
 *
 * For Programmers:
 * Collects compiled module records into the centralized pre-flight evaluation
 * registry. Rejects duplicate module IDs, null pointers, and enforces
 * MANIFEST_MAX_MODULES capacity.
 *
 * @param[in,out] registry Target registry collecting module records.
 * @param[in]     module   Pointer to static immutable module metadata record.
 * @return true if registration succeeded; false on duplicate ID, NULL, or overflow.
 */
bool manifest_register(manifest_registry_t *registry,
                       const module_metadata_t *module);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: Diagnostic Telemetry & Formatted Report Cogs
--------------------------------------------------------------------------------
Renders human-readable tabular ASCII reports of registered engine modules.
For Designers: Visual summary table displaying which systems are active.
For Programmers: Fixed-column stdout reporting without heap allocations.
--------------------------------------------------------------------------------
*/

/**
 * @brief Prints a formatted 80-column ASCII pre-flight audit report to stdout.
 *
 * For Designers:
 * Prints a clean diagnostic summary table to the console detailing every
 * subsystem, its version, its memory policy, and whether it passed preflight.
 *
 * For Programmers:
 * Formatted table traversal over static registry entries with zero heap.
 *
 * @param[in] registry Evaluated registry instance.
 */
void manifest_print_report(const manifest_registry_t *registry);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.3: Ontological Enum Serialization Cogs
--------------------------------------------------------------------------------
Converts diagnostic codes, status levels, layers, and policies to static strings.
For Designers: Translates technical enum numbers into readable labels.
For Programmers: Constant time O(1) table lookups returning static rodata.
--------------------------------------------------------------------------------
*/

/**
 * @brief Converts a diagnostic severity code to a human-readable string.
 *
 * For Designers:
 * Returns friendly labels like "PASS", "WARNING", "CRITICAL", or "FATAL".
 *
 * For Programmers:
 * Returns immutable null-terminated string literals residing in .rodata.
 *
 * @param[in] severity Diagnostic severity level enum value.
 * @return Null-terminated static string (e.g. "PASS", "WARNING", "FATAL").
 */
const char *manifest_severity_to_str(diag_severity_t severity);

/**
 * @brief Converts an aggregate preflight status code to a human-readable string.
 *
 * For Designers:
 * Returns the overall engine health verdict ("READY", "WARNINGS", "HALTED").
 *
 * For Programmers:
 * Constant time O(1) string lookup for aggregate status codes.
 *
 * @param[in] status Aggregate preflight status enum value.
 * @return Null-terminated static string (e.g. "READY", "WARNINGS", "HALTED").
 */
const char *manifest_status_to_str(preflight_status_t status);

/**
 * @brief Converts an architectural layer stratum code to a descriptive string.
 *
 * For Designers:
 * Returns the human-readable layer name ("Layer 0: Core Utilities", etc.).
 *
 * For Programmers:
 * Bounded array index lookup against static layer names table.
 *
 * @param[in] layer Architectural layer stratum enum value.
 * @return Null-terminated static string (e.g. "Layer 0: Core Utilities").
 */
const char *manifest_layer_to_str(module_layer_t layer);

/**
 * @brief Converts a memory allocation policy code to a human-readable string.
 *
 * For Designers:
 * Identifies whether a system uses Zero-Heap, Fixed-Pool, or Arena memory.
 *
 * For Programmers:
 * Constant time O(1) string lookup for memory contract tags.
 *
 * @param[in] policy Memory allocation contract policy enum value.
 * @return Null-terminated static string (e.g. "Zero-Heap", "Fixed-Pool").
 */
const char *manifest_alloc_policy_to_str(alloc_policy_t policy);

/**
 * @brief Converts a concurrency contract policy code to a human-readable string.
 *
 * For Designers:
 * Indicates if a subsystem is thread-safe or single-threaded.
 *
 * For Programmers:
 * Constant time O(1) string lookup for concurrency contract tags.
 *
 * @param[in] policy Concurrency and thread safety policy enum value.
 * @return Null-terminated static string (e.g. "Reentrant", "Main-Thread").
 */
const char *manifest_concurrency_to_str(concurrency_policy_t policy);

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 0 (Core Utilities) | Subsystem: Central Manifest Registry
================================================================================
Mission: Verify incoming parameters, contracts, and schema invariants;
validate standalone function execution and system stability for Block 5.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.1: Contract, Input & Cog Verification Cogs
Mission: Verify received parameters, configuration contracts, and data provenance;
verify that declared subsystem cogs run properly as standalone units.
--------------------------------------------------------------------------------
*/

/**
 * @brief Manifest metadata record for the Central Manifest Registry module.
 */
extern const module_metadata_t manifest_utils_manifest;

/**
 * @brief Executes full pre-flight evaluation across all registered engine modules.
 *
 * Step 1: Validates upstream DAG dependency graph. Missing modules yield FATAL.
 * Step 2: Executes each module's diagnostic preflight_check callback.
 * Step 3: Computes final system policy readiness state.
 *
 * ### For Game Designers & Non-Programmers:
 * Inspects all engine systems and their dependencies before the game boots up.
 * If a required system is missing or fails self-testing, the game safely alerts the team.
 *
 * ### For Systems Programmers & Engine Architects:
 * Evaluates directed acyclic dependency graph (DAG), dispatches individual
 * preflight_check function pointers, tabulates severity counts, and sets aggregate readiness.
 *
 * @param[in,out] registry Initialized registry with registered modules.
 * @return Overall preflight status (OK, WARNINGS, DOWNGRADED, HALT_FATAL).
 */
preflight_status_t manifest_run_preflight(manifest_registry_t *registry);

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.2: Machine Self-Audit & Pre-Flight Validation Tripwires
Mission: Validate holistic system invariants, tripwires, and subsystem stability;
provide pre-flight clearance handshake before advancing to Block 5 execution.
--------------------------------------------------------------------------------
*/

/**
 * @brief Pre-flight diagnostic check evaluating manifest registry invariants.
 *
 * ### For Game Designers & Non-Programmers:
 * Checks that the system scoreboard itself is working correctly and not full.
 *
 * ### For Systems Programmers & Engine Architects:
 * Verifies static capacity limits, duplicate registration rejection, and severity escalation rules.
 *
 * @return Diagnostic check outcome reporting SEVERITY_PASS or SEVERITY_FATAL.
 */
diag_result_t manifest_utils_preflight(void);

/**
 * @brief Standalone boolean self-audit testing manifest invariants.
 *
 * ### For Game Designers & Non-Programmers:
 * Automated check verifying that the manifest registry functions cleanly.
 *
 * ### For Systems Programmers & Engine Architects:
 * High-level boolean validation audit exercising manifest registration and DAG resolution.
 *
 * @return true if all manifest tests pass cleanly; false on violation.
 */
bool manifest_audit(void);

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
Mission: Run operational execution procedures, format terminal-friendly ASCII
status cards, and construct structured telemetry inspection objects.
--------------------------------------------------------------------------------
*/

/**
 * @brief Renders a 60-column terminal-friendly ASCII inspection card of the registry.
 *
 * ### For Game Designers & Non-Programmers:
 * Displays a clean text summary card showing all registered systems and whether they passed boot tests.
 *
 * ### For Systems Programmers & Engine Architects:
 * Formats 60-column terminal-friendly status card of manifest_registry_t without dynamic heap allocation.
 *
 * @param[in]  registry Pointer to evaluated registry.
 * @param[out] out_buf  Destination buffer for ASCII card output.
 * @param[in]  buf_size Size of destination buffer in bytes.
 * @return true if rendered successfully; false on NULL or truncation.
 */
bool manifest_execute_dump_card(const manifest_registry_t *registry,
                               char *out_buf,
                               size_t buf_size);

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.2: Subsystem Lifecycle Initialization & Teardown
Mission: Manage subsystem lifecycle, allocate runtime ring buffers,
open/close file streams, and tear down hardware handles.
--------------------------------------------------------------------------------
None. (Stateless module / pure stack-allocated execution).
--------------------------------------------------------------------------------
*/

#endif /* TENEBRIS_LUX_CORE_MANIFEST_H */
