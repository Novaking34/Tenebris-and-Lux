// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          master_loader.h
// @id            core.io.master_loader
// @layer         Layer 0: Core Utilities
// @subsystem     Data & Filesystem Subsystem (util/io)
// @version       0.1.0
// @interface_rev Rev 2
// @date          2026-11-05
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        Unified Master Data Loading & 5-Block Lifecycle Deserializer
// @role          Universal central hub for reading, verifying 5-block headers,
//                validating schema IDs, and decoding payload data into engine tables.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Zero-heap master data loader for all engine and game subsystems.
//   Provides a unified ingest pipeline across the 5 operational domains:
//     1. Resolves paths dynamically via path_finder.
//     2. Reads files into bounded stack buffers without dynamic allocation.
//     3. Parses and validates Block 1 (Metadata) and Block 2 (Schema/Contracts).
//     4. Scopes queries directly into Block 3 (Body), eliminating key collisions.
//     5. Emits structured error codes from the diagnostics error taxonomy.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies:
//       io/path_finder.h
//       io/json_utils.h
//       io/fs_utils.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic heap allocations).
//   - Concurrency / Safety:  REENTRANT_PURE on caller-supplied master_loader_t.
//   - Latency Budget:        < 500 microseconds per dataset load.
//
// AXIOMATIC INVARIANTS:
//   1. Zero dynamic heap allocations across all loading and parsing routines.
//   2. Strict schema ID and version validation before body payload extraction.
//   3. Resilient fallback support: handles both 5-block and legacy flat JSON.
//   4. Caller buffers are strictly bounded; all extracted strings null-terminated.
//   5. Universal 5-Block Lifecycle Protocol enforced across header and source.
// =============================================================================

#ifndef TENEBRIS_LUX_IO_MASTER_LOADER_H
#define TENEBRIS_LUX_IO_MASTER_LOADER_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: Master Data Loader
// =============================================================================

#include "core/manifest.h"
#include "io/path_finder.h"
#include "io/json_utils.h"
#include "io/fs_utils.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define LOADER_MAX_TOKEN_LEN 64
#define LOADER_MAX_PATH_LEN  256

/**
 * Diagnostic error codes aligned with system/diagnostics/taxonomy/error_codes.json.
 */
typedef enum {
    LOADER_STATUS_OK                  = 0,
    LOADER_STATUS_ERR_NOT_FOUND       = 0x0201, /**< ERR_IO_FILE_NOT_FOUND */
    LOADER_STATUS_ERR_BUFFER_OVERFLOW = 0x0202, /**< ERR_IO_BUFFER_OVERFLOW */
    LOADER_STATUS_ERR_SCHEMA_MISMATCH = 0x0203, /**< ERR_IO_SCHEMA_MISMATCH */
    LOADER_STATUS_ERR_SYNTAX_ERROR    = 0x0204, /**< ERR_IO_CORRUPT_MANIFEST */
    LOADER_STATUS_ERR_NULL_PARAM      = 0x0205, /**< Null parameter violation */
    LOADER_STATUS_ERR_BODY_MISSING    = 0x0206  /**< block_3_body missing */
} loader_status_t;

/**
 * Parsed header metadata extracted from Block 1 and Block 2.
 */
typedef struct {
    char project[LOADER_MAX_TOKEN_LEN];
    char file[LOADER_MAX_PATH_LEN];
    char id[LOADER_MAX_TOKEN_LEN];
    char layer[LOADER_MAX_TOKEN_LEN];
    char version[32];
    char schema_id[LOADER_MAX_TOKEN_LEN];
    bool is_5block;
} loader_header_t;

/**
 * Master loader state context instantiated on the stack by callers.
 */
typedef struct {
    const char *raw_buf;
    size_t raw_len;
    loader_header_t header;
    const char *body_start;
    size_t body_len;
    loader_status_t status;
    char resolved_path[PATH_FINDER_MAX_PATH];
} master_loader_t;

/**
 * Manifest record for the Zero-Heap Master Loader Engine.
 */
extern const module_metadata_t master_loader_manifest;

// =============================================================================
// BLOCK 3: BODY BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: Master Data Loader
// =============================================================================

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 3.1: INGESTION & HEADER PARSING COGS ]
// Mission: Locate, read, and decode 5-block lifecycle envelopes.
// -----------------------------------------------------------------------------

/**
 * @brief Loads and parses a dataset file using dynamic domain path resolution.
 *
 * @param[out] loader   Master loader context populated with parsed state.
 * @param[in]  domain   Operational domain ("system", "world", "entities",
 *                      "mechanics", "presentation") or NULL.
 * @param[in]  rel_path Relative path to target JSON file.
 * @param[in]  buf      Caller memory destination buffer.
 * @param[in]  buf_size Capacity of caller buffer in bytes.
 *
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
 * @param[out] loader   Master loader context.
 * @param[in]  filepath Absolute or relative path to file.
 * @param[in]  buf      Caller buffer.
 * @param[in]  buf_size Buffer capacity.
 *
 * @return true if file read and parsed; false otherwise.
 */
bool master_loader_load_from_path(master_loader_t *loader,
                                  const char *filepath,
                                  char *buf,
                                  size_t buf_size);

/**
 * @brief Ingests an existing null-terminated JSON text buffer in memory.
 *
 * @param[out] loader    Master loader context.
 * @param[in]  json_text Null-terminated JSON buffer.
 * @param[in]  json_len  Length of JSON buffer in bytes.
 *
 * @return true if envelope parsed cleanly; false otherwise.
 */
bool master_loader_load_from_memory(master_loader_t *loader,
                                    const char *json_text,
                                    size_t json_len);

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 3.2: CONTRACT VALIDATION COGS ]
// Mission: Verify schema_id and semantic version constraints.
// -----------------------------------------------------------------------------

/**
 * @brief Verifies that loaded file's schema_id matches the expected contract.
 *
 * @param[in] loader             Populated loader context.
 * @param[in] expected_schema_id Required schema identifier (e.g. "tenlux.data.math_formulas.v1").
 *
 * @return true if schema matches; false otherwise.
 */
bool master_loader_validate_schema(const master_loader_t *loader,
                                   const char *expected_schema_id);

/**
 * @brief Verifies that loaded file's version matches or satisfies requirement.
 *
 * @param[in] loader           Populated loader context.
 * @param[in] expected_version Target version string (e.g. "0.1.0").
 *
 * @return true if version matches; false otherwise.
 */
bool master_loader_validate_version(const master_loader_t *loader,
                                    const char *expected_version);

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 3.3: TYPED BODY EXTRACTION COGS ]
// Mission: Safely extract typed properties scoped inside Block 3 (Body).
// -----------------------------------------------------------------------------

/**
 * @brief Extracts a string property scoped within Block 3.
 *
 * @param[in]  loader   Populated loader context.
 * @param[in]  sub_obj  Optional sub-object key inside block_3_body, or NULL.
 * @param[in]  key      Property key to extract.
 * @param[out] out_val  Caller buffer receiving string.
 * @param[in]  out_size Capacity of destination buffer.
 *
 * @return true if extracted; false otherwise.
 */
bool master_loader_get_string(const master_loader_t *loader,
                              const char *sub_obj,
                              const char *key,
                              char *out_val,
                              size_t out_size);

/**
 * @brief Extracts a signed 32-bit integer property scoped within Block 3.
 */
bool master_loader_get_int(const master_loader_t *loader,
                           const char *sub_obj,
                           const char *key,
                           int *out_val);

/**
 * @brief Extracts an unsigned size_t integer property scoped within Block 3.
 */
bool master_loader_get_size_t(const master_loader_t *loader,
                              const char *sub_obj,
                              const char *key,
                              size_t *out_val);

/**
 * @brief Extracts a floating-point property scoped within Block 3.
 */
bool master_loader_get_float(const master_loader_t *loader,
                             const char *sub_obj,
                             const char *key,
                             float *out_val);

/**
 * @brief Extracts a boolean property scoped within Block 3.
 */
bool master_loader_get_bool(const master_loader_t *loader,
                            const char *sub_obj,
                            const char *key,
                            bool *out_val);

/**
 * @brief Locates an array property scoped within Block 3.
 *
 * @param[in]  loader        Populated loader context.
 * @param[in]  sub_obj       Optional sub-object name containing the array, or NULL.
 * @param[in]  array_key     Key name of the array.
 * @param[out] out_arr_start Receives pointer to opening '[' bracket.
 * @param[out] out_arr_len   Receives byte length of array including brackets.
 * @param[out] out_count     Receives count of elements in array.
 *
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
 */
bool master_loader_get_array_object(const char *arr_start,
                                    size_t arr_len,
                                    size_t index,
                                    char *out_obj,
                                    size_t out_size);

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: Master Data Loader
// =============================================================================

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 4.1: MACHINE SELF-AUDIT & PRE-FLIGHT TRIPWIRES ]
// Mission: Verify master loader invariants, schema validation, and envelope parsing.
// -----------------------------------------------------------------------------

/**
 * @brief Executes machine pre-flight diagnostic checks on master loader invariants.
 *
 * @return diag_result_t reporting SEVERITY_PASS or SEVERITY_FATAL.
 */
diag_result_t master_loader_preflight(void);

/**
 * @brief High-level boolean audit wrapper for standalone test harnesses.
 *
 * @return true if all master loader invariant tripwires pass cleanly; false otherwise.
 */
bool master_loader_audit(void);

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: Master Data Loader
// =============================================================================

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 5.1: EXECUTABLE PROCEDURES & OBJECT CONSTRUCTORS ]
// -----------------------------------------------------------------------------

/**
 * @brief Inspects a file's 5-block envelope and populates a loader_header_t object.
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
 * @brief High-level pipeline procedure that loads and validates schema contract.
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

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 5.2: SUBSYSTEM LIFECYCLE INITIALIZATION & TEARDOWN ]
// -----------------------------------------------------------------------------

/**
 * @brief Initializes the unified master loader subsystem.
 */
void master_loader_init(void);

/**
 * @brief Tears down the unified master loader subsystem.
 */
void master_loader_shutdown(void);

#endif /* TENEBRIS_LUX_IO_MASTER_LOADER_H */
