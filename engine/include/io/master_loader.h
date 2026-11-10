/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          master_loader.h
@layer         Layer 0: Core Utilities
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-08
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Unified Master Data Loading & 5-Block Lifecycle Deserializer
@role          Universal central hub for reading, verifying 5-block headers,
               validating schema IDs, and decoding payload data into engine tables.
@invariants    1. Zero dynamic heap allocations in Layer 0 (ALLOC_ZERO_HEAP).
               2. Strict ISO C11 adherence (-std=c11, -Wall -Wextra -Wpedantic).
               3. Strict schema ID and version validation before body payload extraction.
               4. Scoped body isolation: queries target block_3_body, preventing key collisions.
               5. Caller buffers are strictly bounded; all extracted strings null-terminated.
               6. Universal 5-Block Lifecycle Protocol enforced across header and source.

PURPOSE & ARCHITECTURAL ROLE:
  Zero-heap master data loader for all engine and game subsystems.
  Provides a unified ingest pipeline across the 5 operational domains:
    1. Resolves paths dynamically via path_finder.
    2. Reads files into bounded stack buffers without dynamic allocation.
    3. Parses and validates Block 1 (Metadata) and Block 2 (Schema/Contracts).
    4. Scopes queries directly into Block 3 (Body), eliminating key collisions.
    5. Emits structured error codes from the diagnostics error taxonomy.

  ### For Game Designers & Non-Programmers:
  Instead of each game system writing its own fragile file loader, the master
  loader acts as a universal customs checkpoint. When loading any game data
  (weapons, provinces, characters, or formulas), the master loader verifies
  its identity, confirms its version matches the game version, and extracts
  the data safely without crashing if an optional number is missing.

  ### For Systems Programmers & Engine Architects:
  This module provides zero-allocation contract verification and data isolation:
  - Validates document structure against the OmniCode 5-Block protocol.
  - Guarantees zero heap allocations during loading, parsing, and extraction.
  - Tagged structural representations ensuring clean DWARF debugger integration.
  - Integrated diagnostic telemetry with categorized severity tripwires.
================================================================================
*/

#ifndef TENEBRIS_LUX_IO_MASTER_LOADER_H
#define TENEBRIS_LUX_IO_MASTER_LOADER_H

/*
================================================================================
BLOCK 2: OPENING BLOCK / TYPES & DOMAIN ONTOLOGY
Stratum: Layer 0 (Core Utilities) | Subsystem: Master Data Loader
================================================================================
Mission: Loader buffer bounds, include ladder, loader manifest, structural
ontology, and pre-conditional validation predicates for 5-block envelope ingestion.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Include Hierarchy: 3-Tier Downward Architecture
--------------------------------------------------------------------------------
Tier 1: Standard ISO C11 Library Headers
Tier 2: External Dependencies (None)
Tier 3: Internal Engine Foundation Headers
--------------------------------------------------------------------------------
*/

/* --- Tier 1: Standard ISO C11 Library Headers --- */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* --- Tier 2: External Dependencies (None) --- */

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "core/manifest.h"
#include "io/path_finder.h"
#include "io/fs_utils.h"
#include "io/json_utils.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Pre-Conditions & Buffer Limits
--------------------------------------------------------------------------------
Compile-time sizing limits for loader tokens and resolved filesystem paths.
For Designers: Guarantees that metadata identifiers and file paths fit inside
fast, fixed-size memory slots without allocating heap or leaking memory.
For Programmers: Enforces strict zero-heap bounds (ALLOC_ZERO_HEAP) in .rodata.
--------------------------------------------------------------------------------
*/

/** Maximum token string length in bytes for identifiers and schema keys. */
#define LOADER_MAX_TOKEN_LEN 64

/** Maximum path string length in bytes for source file attributes. */
#define LOADER_MAX_PATH_LEN  256

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Manifest & Pre-Flight Hooks
--------------------------------------------------------------------------------
Pre-flight verification anchor for the Master Loader engine.
--------------------------------------------------------------------------------
*/

/** Unique module identifier string for the Zero-Heap Master Loader Engine. */
#define MASTER_LOADER_MODULE_ID        "io.master_loader"

/**
 * Manifest record for the Zero-Heap Master Loader Engine.
 */
extern const module_metadata_t master_loader_manifest;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Structural Entity & Object Definitions
--------------------------------------------------------------------------------
Ontological types for loader diagnostic status, 5-block headers, and context state.
--------------------------------------------------------------------------------
*/

/**
 * @enum loader_status
 * @brief Diagnostic error codes aligned with system/diagnostics/taxonomy/error_codes.json.
 *
 * For Designers: Human-understandable error codes explaining why a JSON data file
 * could not be loaded (e.g. file missing, syntax error, or bad format).
 *
 * For Programmers: Explicitly numbered 16-bit diagnostic codes matching the error taxonomy.
 */
typedef enum loader_status {
    LOADER_STATUS_OK                  = 0,      /**< Operation succeeded cleanly. */
    LOADER_STATUS_ERR_NOT_FOUND       = 0x0201, /**< ERR_IO_FILE_NOT_FOUND (File missing on disk). */
    LOADER_STATUS_ERR_BUFFER_OVERFLOW = 0x0202, /**< ERR_IO_BUFFER_OVERFLOW (Payload exceeds capacity). */
    LOADER_STATUS_ERR_SCHEMA_MISMATCH = 0x0203, /**< ERR_IO_SCHEMA_MISMATCH (Schema contract breached). */
    LOADER_STATUS_ERR_SYNTAX_ERROR    = 0x0204, /**< ERR_IO_CORRUPT_MANIFEST (Malformed JSON tokens). */
    LOADER_STATUS_ERR_NULL_PARAM      = 0x0205, /**< Null parameter violation passed to loader cog. */
    LOADER_STATUS_ERR_BODY_MISSING    = 0x0206  /**< block_3_body missing from 5-block envelope. */
} loader_status_t;

/**
 * @struct loader_header
 * @brief Parsed header metadata extracted from Block 1 and Block 2.
 *
 * For Designers: The passport information stamped at the top of every data file,
 * recording project name, author, version, and schema standard.
 *
 * For Programmers: Stack-allocated record populated by loader_parse_header().
 */
typedef struct loader_header {
    char project[LOADER_MAX_TOKEN_LEN];   /**< Project identity string ("Tenebris and Lux"). */
    char file[LOADER_MAX_PATH_LEN];       /**< Relative source filename. */
    char id[LOADER_MAX_TOKEN_LEN];         /**< Unique dataset or module identifier. */
    char layer[LOADER_MAX_TOKEN_LEN];      /**< Architectural layer designation. */
    char version[32];                     /**< Semantic version string (e.g. "0.1.1-dev"). */
    char schema_id[LOADER_MAX_TOKEN_LEN];  /**< Schema contract identifier. */
    bool is_5block;                       /**< true if 5-block lifecycle envelope is valid. */
} loader_header_t;

/**
 * @struct master_loader
 * @brief Master loader state context instantiated on the stack by callers.
 *
 * For Designers: The master workbench holding a loaded data file while the game
 * reads characters, items, or visual layouts from it.
 *
 * For Programmers: Zero-heap execution context. Manages raw payload slices,
 * header metadata, and resolved VFS paths without dynamic memory allocation.
 */
typedef struct master_loader {
    const char     *raw_buf;                            /**< Pointer to immutable input JSON buffer. */
    size_t          raw_len;                            /**< Total length in bytes of raw input. */
    loader_header_t header;                             /**< Parsed 5-block lifecycle header. */
    const char     *body_start;                         /**< Pointer into raw_buf where block_3_body begins. */
    size_t          body_len;                           /**< Total byte length of block_3_body slice. */
    loader_status_t status;                             /**< Outcome status code of most recent operation. */
    char            resolved_path[PATH_FINDER_MAX_PATH];/**< Resolved disk path from VFS path finder. */
} master_loader_t;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.4: Pre-Conditional Helper Cogs & Non-Business Inlines
--------------------------------------------------------------------------------
Pure static inline validation predicates and non-business query helpers.
Guarantees zero heap, pure reentrancy, and fast pre-flight assertion checks.
--------------------------------------------------------------------------------
*/

/**
 * @brief Predicate verifying if a loader status represents complete success.
 *
 * @param[in] status Status code to check.
 * @return true if status equals LOADER_STATUS_OK.
 */
static inline bool loader_status_is_ok(loader_status_t status) {
    return status == LOADER_STATUS_OK;
}

/**
 * @brief Predicate verifying if a master loader context is fully loaded and valid.
 *
 * @param[in] loader Pointer to loader context to inspect.
 * @return true if loader is non-NULL, status is OK, and raw_buf is non-NULL.
 */
static inline bool loader_is_loaded(const master_loader_t *loader) {
    return (loader != NULL) && (loader->status == LOADER_STATUS_OK) && (loader->raw_buf != NULL);
}

/**
 * @brief Predicate verifying if a parsed header conforms to the 5-block envelope.
 *
 * @param[in] hdr Pointer to loader header to inspect.
 * @return true if hdr is non-NULL and is_5block flag is true.
 */
static inline bool loader_header_is_5block(const loader_header_t *hdr) {
    return (hdr != NULL) && hdr->is_5block;
}

/*
================================================================================
BLOCK 3: BODY BLOCK / CORE LOGIC & BUSINESS FUNCTIONS
Stratum: Layer 0 (Core Utilities) | Subsystem: Master Data Loader
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: Ingestion & Header Parsing Cogs
Mission: Locate, read, and decode 5-block lifecycle envelopes.
--------------------------------------------------------------------------------
*/

/**
 * @brief Loads and parses a dataset file using dynamic domain path resolution.
 *
 * ### For Game Designers & Non-Programmers:
 * Universal loader function. Finds a JSON file across game folders, verifies its passport,
 * and prepares its contents for reading.
 *
 * ### For Systems Programmers & Engine Architects:
 * Dynamic VFS domain resolution, bounded file reading, and 5-block envelope parsing
 * with zero dynamic heap allocation (ALLOC_ZERO_HEAP).
 *
 * @param[out] loader   Master loader context populated with parsed state.
 * @param[in]  domain   Operational domain ("config", "world", "entities", "mechanics", "presentation") or NULL.
 * @param[in]  rel_path Relative path to target JSON file.
 * @param[in]  buf      Caller memory destination buffer.
 * @param[in]  buf_size Capacity of caller buffer in bytes.
 * @return true if file was read and envelope parsed successfully; false otherwise.
 */
bool master_loader_load(master_loader_t *loader,
                        const char *domain,
                        const char *rel_path,
                        char *buf,
                        size_t buf_size);

/**
 * @brief Loads and parses a dataset from an explicit filesystem path.
 *
 * ### For Game Designers & Non-Programmers:
 * Loads a specific file directly from an exact filepath on disk.
 *
 * ### For Systems Programmers & Engine Architects:
 * Directly reads file from filepath via fs_read_file_bounded and parses Block 1, 2, and 3 slices.
 *
 * @param[out] loader   Master loader context.
 * @param[in]  filepath Absolute or relative path to file.
 * @param[in]  buf      Caller buffer.
 * @param[in]  buf_size Buffer capacity.
 * @return true if file read and parsed; false otherwise.
 */
bool master_loader_load_from_path(master_loader_t *loader,
                                  const char *filepath,
                                  char *buf,
                                  size_t buf_size);

/**
 * @brief Ingests an existing null-terminated JSON text buffer in memory.
 *
 * ### For Game Designers & Non-Programmers:
 * Ingests text that is already loaded in memory (e.g. downloaded over network or pre-cached).
 *
 * ### For Systems Programmers & Engine Architects:
 * Parses existing null-terminated JSON text buffer in-place without copying or allocating heap memory.
 *
 * @param[out] loader    Master loader context.
 * @param[in]  json_text Null-terminated JSON buffer.
 * @param[in]  json_len  Length of JSON buffer in bytes.
 * @return true if envelope parsed cleanly; false otherwise.
 */
bool master_loader_load_from_memory(master_loader_t *loader,
                                    const char *json_text,
                                    size_t json_len);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: Typed Body Extraction Cogs
Mission: Safely extract typed properties scoped inside Block 3 (Body).
--------------------------------------------------------------------------------
*/

/**
 * @brief Extracts a string property scoped within Block 3.
 *
 * ### For Game Designers & Non-Programmers:
 * Reads a text property (such as character name or item description) from the data file safely.
 *
 * ### For Systems Programmers & Engine Architects:
 * Scoped token search inside Block 3 body, copying into bounded destination buffer with null termination.
 *
 * @param[in]  loader   Populated loader context.
 * @param[in]  sub_obj  Optional sub-object key inside block_3_body, or NULL.
 * @param[in]  key      Property key to extract.
 * @param[out] out_val  Caller buffer receiving string.
 * @param[in]  out_size Capacity of destination buffer.
 * @return true if extracted; false otherwise.
 */
bool master_loader_get_string(const master_loader_t *loader,
                              const char *sub_obj,
                              const char *key,
                              char *out_val,
                              size_t out_size);

/**
 * @brief Extracts a signed 32-bit integer property scoped within Block 3.
 *
 * ### For Game Designers & Non-Programmers:
 * Reads a whole number (such as HP, defense rating, or price) from the file.
 *
 * ### For Systems Programmers & Engine Architects:
 * Scoped integer token extraction within Block 3 slice into caller-provided int pointer.
 *
 * @param[in]  loader  Populated loader context.
 * @param[in]  sub_obj Optional sub-object name containing key, or NULL.
 * @param[in]  key     Property key to extract.
 * @param[out] out_val Destination pointer receiving integer.
 * @return true if integer was extracted cleanly; false otherwise.
 */
bool master_loader_get_int(const master_loader_t *loader,
                           const char *sub_obj,
                           const char *key,
                           int *out_val);

/**
 * @brief Extracts an unsigned size_t integer property scoped within Block 3.
 *
 * ### For Game Designers & Non-Programmers:
 * Reads an unsigned quantity or memory size limit from the data file.
 *
 * ### For Systems Programmers & Engine Architects:
 * Scoped unsigned token extraction within Block 3 slice into caller-provided size_t pointer.
 *
 * @param[in]  loader  Populated loader context.
 * @param[in]  sub_obj Optional sub-object name containing key, or NULL.
 * @param[in]  key     Property key to extract.
 * @param[out] out_val Destination pointer receiving size_t.
 * @return true if size_t was extracted cleanly; false otherwise.
 */
bool master_loader_get_size_t(const master_loader_t *loader,
                              const char *sub_obj,
                              const char *key,
                              size_t *out_val);

/**
 * @brief Extracts a floating-point property scoped within Block 3.
 *
 * ### For Game Designers & Non-Programmers:
 * Reads a fractional multiplier or decimal rate (such as 1.25x damage or 0.5 cooldown).
 *
 * ### For Systems Programmers & Engine Architects:
 * Scoped floating-point token extraction within Block 3 slice into caller-provided float pointer.
 *
 * @param[in]  loader  Populated loader context.
 * @param[in]  sub_obj Optional sub-object name containing key, or NULL.
 * @param[in]  key     Property key to extract.
 * @param[out] out_val Destination pointer receiving float.
 * @return true if float was extracted cleanly; false otherwise.
 */
bool master_loader_get_float(const master_loader_t *loader,
                             const char *sub_obj,
                             const char *key,
                             float *out_val);

/**
 * @brief Extracts a boolean property scoped within Block 3.
 *
 * ### For Game Designers & Non-Programmers:
 * Reads a true/false toggle setting (such as whether an item is consumable or a quest is active).
 *
 * ### For Systems Programmers & Engine Architects:
 * Scoped boolean token extraction ("true" / "false") within Block 3 slice.
 *
 * @param[in]  loader  Populated loader context.
 * @param[in]  sub_obj Optional sub-object name containing key, or NULL.
 * @param[in]  key     Property key to extract.
 * @param[out] out_val Destination pointer receiving boolean.
 * @return true if boolean was extracted cleanly; false otherwise.
 */
bool master_loader_get_bool(const master_loader_t *loader,
                            const char *sub_obj,
                            const char *key,
                            bool *out_val);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.3: Array Traversal & Object Slice Cogs
Mission: Iterate array elements and extract discrete object slices.
--------------------------------------------------------------------------------
*/

/**
 * @brief Locates an array property scoped within Block 3.
 *
 * ### For Game Designers & Non-Programmers:
 * Finds a list of items, enemies, or skills inside a data file so the engine can loop through each one.
 *
 * ### For Systems Programmers & Engine Architects:
 * Locates array slice starting at '[' and ending at matching ']', returning slice pointer and element count.
 *
 * @param[in]  loader        Populated loader context.
 * @param[in]  sub_obj       Optional sub-object name containing the array, or NULL.
 * @param[in]  array_key     Key name of the array.
 * @param[out] out_arr_start Receives pointer to opening '[' bracket.
 * @param[out] out_arr_len   Receives byte length of array including brackets.
 * @param[out] out_count     Receives count of elements in array.
 * @return true if array located; false otherwise.
 */
bool master_loader_find_array(const master_loader_t *loader,
                              const char *sub_obj,
                              const char *array_key,
                              const char **out_arr_start,
                              size_t *out_arr_len,
                              size_t *out_count);

/**
 * @brief Extracts i-th object slice { ... } from an array.
 *
 * Wrapper around zero-heap json_array_get_object for convenient pipeline consumption.
 *
 * ### For Game Designers & Non-Programmers:
 * Grabs one specific entry from a list (e.g. getting the 3rd enemy in an encounter).
 *
 * ### For Systems Programmers & Engine Architects:
 * Zero-heap slice extractor copying the i-th JSON object into destination buffer.
 *
 * @param[in]  arr_start Pointer to opening '[' bracket.
 * @param[in]  arr_len   Length of array slice.
 * @param[in]  index     Zero-based index of object to extract.
 * @param[out] out_obj   Destination buffer receiving object text.
 * @param[in]  out_size  Capacity of destination buffer.
 * @return true if object extracted; false otherwise.
 */
bool master_loader_get_array_object(const char *arr_start,
                                    size_t arr_len,
                                    size_t index,
                                    char *out_obj,
                                    size_t out_size);

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 0 (Core Utilities) | Subsystem: Master Data Loader
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
 * @brief Verifies that loaded file's schema_id matches the expected contract.
 *
 * ### For Game Designers & Non-Programmers:
 * Checks that the file is actually the type of file the game expected (e.g. ensuring a weapon file isn't loaded as a character).
 *
 * ### For Systems Programmers & Engine Architects:
 * Exact contract comparison between header schema_id and expected_schema_id string.
 *
 * @param[in] loader             Populated loader context.
 * @param[in] expected_schema_id Required schema identifier (e.g. "tenlux.data.math_formulas.v1").
 * @return true if schema matches; false otherwise.
 */
bool master_loader_validate_schema(const master_loader_t *loader,
                                   const char *expected_schema_id);

/**
 * @brief Verifies that loaded file's version matches or satisfies requirement.
 *
 * ### For Game Designers & Non-Programmers:
 * Confirms that the file version matches the game version so outdated mods or files don't cause crashes.
 *
 * ### For Systems Programmers & Engine Architects:
 * Prefix semver comparison validating document format compatibility.
 *
 * @param[in] loader           Populated loader context.
 * @param[in] expected_version Target version string (e.g. "0.1.0").
 * @return true if version matches; false otherwise.
 */
bool master_loader_validate_version(const master_loader_t *loader,
                                    const char *expected_version);

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.2: Machine Self-Audit & Pre-Flight Validation Tripwires
Mission: Validate holistic system invariants, tripwires, and subsystem stability;
provide pre-flight clearance handshake before advancing to Block 5 execution.
--------------------------------------------------------------------------------
*/

/**
 * @brief Executes machine pre-flight diagnostic checks on master loader invariants.
 *
 * ### For Game Designers & Non-Programmers:
 * Verifies that the master data loader correctly opens 5-block envelopes and enforces
 * schema requirements before the game attempts to load data.
 *
 * ### For Systems Programmers & Engine Architects:
 * Validates zero-heap memory bounds, 5-block envelope slicing, schema validation
 * predicates, and error handling for malformed documents.
 *
 * @return diag_result_t reporting categorized diagnostic severity.
 */
diag_result_t master_loader_preflight(void);

/**
 * @brief High-level boolean audit wrapper for standalone test harnesses.
 *
 * ### For Game Designers & Non-Programmers:
 * Automated pass/fail health check ensuring all game data loading rules are respected.
 *
 * ### For Systems Programmers & Engine Architects:
 * Comprehensive validation audit verifying schema matching, version comparison, and envelope integrity.
 *
 * @return true if all master loader invariant tripwires pass cleanly; false otherwise.
 */
bool master_loader_audit(void);

/*
================================================================================
BLOCK 5: CLOSING BLOCK / EXECUTABLE PROCEDURES & LIFECYCLE
Stratum: Layer 0 (Core Utilities) | Subsystem: Master Data Loader
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
 * @brief Inspects a file's 5-block envelope and populates a loader_header_t object.
 *
 * ### For Game Designers & Non-Programmers:
 * Checks the title, version, and security tags on the envelope of a data file.
 *
 * ### For Systems Programmers & Engine Architects:
 * Parses Block 1 metadata and Block 2 contracts into stack-allocated loader_header_t.
 *
 * @param[in]  domain     Operational domain ("system", "world", etc.) or NULL.
 * @param[in]  rel_path   Relative path to target JSON file.
 * @param[out] out_header Header object populated with metadata & schema contracts.
 *
 * @return true if file header was read and parsed successfully; false otherwise.
 */
bool master_loader_execute_inspect(const char *domain,
                                   const char *rel_path,
                                   loader_header_t *out_header);

/**
 * @brief Formats an ASCII diagnostic card summarizing inspected loader envelope metadata.
 *
 * ### For Game Designers & Non-Programmers:
 * Displays a clean text card showing file schema ID, version, author, and integrity state.
 *
 * ### For Systems Programmers & Engine Architects:
 * Renders a 60-column terminal-friendly diagnostic card of loader_header_t into out_buf without heap allocation.
 *
 * @param[in]  header   Populated loader_header_t record.
 * @param[out] out_buf  Destination character buffer.
 * @param[in]  buf_size Size of destination buffer in bytes.
 * @return Number of characters written (excluding null terminator), or 0 on error.
 */
size_t master_loader_execute_dump_card(const loader_header_t *header,
                                       char *out_buf,
                                       size_t buf_size);

/**
 * @brief High-level pipeline procedure that loads and validates schema contract.
 *
 * ### For Game Designers & Non-Programmers:
 * Opens a file, confirms that it has the exact expected format, and prepares it for use.
 *
 * ### For Systems Programmers & Engine Architects:
 * Atomic execution pipeline resolving path, loading bytes into buffer, and asserting schema match.
 *
 * @param[in]  domain             Operational domain or NULL.
 * @param[in]  rel_path           Relative path to JSON file.
 * @param[in]  expected_schema_id Expected schema ID contract.
 * @param[in]  buf                Caller-supplied destination buffer.
 * @param[in]  buf_size           Capacity of caller buffer in bytes.
 * @param[out] out_loader         Configured master loader context.
 *
 * @return true if loaded, parsed, and schema contract matched; false otherwise.
 */
bool master_loader_execute_load_verified(const char *domain,
                                         const char *rel_path,
                                         const char *expected_schema_id,
                                         char *buf,
                                         size_t buf_size,
                                         master_loader_t *out_loader);

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.2: Subsystem Lifecycle Initialization & Teardown
Mission: Manage subsystem lifecycle, allocate runtime ring buffers,
open/close file streams, and tear down hardware handles.
--------------------------------------------------------------------------------
*/

/**
 * @brief Initializes the unified master loader subsystem.
 *
 * ### For Game Designers & Non-Programmers:
 * Prepares the universal data loading pipeline when the game boots.
 *
 * ### For Systems Programmers & Engine Architects:
 * Resets loader registry state and initializes schema validation tripwires.
 */
void master_loader_init(void);

/**
 * @brief Tears down the unified master loader subsystem.
 *
 * ### For Game Designers & Non-Programmers:
 * Shuts down the master loader and clears caches on game shutdown.
 *
 * ### For Systems Programmers & Engine Architects:
 * Flushes state and performs clean teardown of loader cache registries.
 */
void master_loader_shutdown(void);

#endif /* TENEBRIS_LUX_IO_MASTER_LOADER_H */
