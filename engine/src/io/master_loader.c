/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          master_loader.c
@id            io.loader
@layer         Layer 0: Core Utilities
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-09
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
  Zero-heap master data loader implementation.
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

/*
================================================================================
BLOCK 2: OPENING BLOCK / MODULE ONTOLOGY & 3-TIER INCLUDES
Stratum: Layer 0 (Core Utilities) | Machine: Master Data Loader
================================================================================
Mission: Multi-tier dataset loading, 3-tier downward include hierarchy,
schema validation, stateless loader ontology, and module manifest.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Include Hierarchy: 3-Tier Downward Architecture
--------------------------------------------------------------------------------
Tier 1: Standard ISO C11 Library Headers
Tier 2: External Library Dependencies (None)
Tier 3: Internal Engine Foundation Headers
--------------------------------------------------------------------------------
*/

/* --- Tier 1: Standard ISO C11 Library Headers --- */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* --- Tier 2: External Library Dependencies (None) --- */

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "core/manifest.h"
#include "core/string_utils.h"
#include "debug/debug.h"
#include "io/fs_utils.h"
#include "io/json_utils.h"
#include "io/master_loader.h"
#include "io/path_finder.h"
#include "log/log.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Module Manifest Anchor, Category Tags & Compile-Time Constants
--------------------------------------------------------------------------------
*/
#define MODULE_ID          "core.io.master_loader"
#define LOCAL_LOG_CAT      LOG_CAT_IO
#define LOCAL_MODULE_TAG   "MasterLoader"

static const char *const s_master_loader_invariants[4] = {
    "Zero dynamic heap allocation during dataset loading",
    "Strict schema ID and semver validation contract",
    "Scoped extraction inside Block 3 body preventing key collisions",
    "Universal 5-Block Lifecycle Protocol enforced across header and source"
};

const module_metadata_t master_loader_manifest = {
    "core.io.master_loader",
    "master_loader.h",
    LAYER_0_CORE_UTIL,
    { 0, 1, 1 },
    5,
    "2026-11-10",
    "Zero-heap universal master data loader and 5-block parser",
    3,
    { "core.io.path_finder", "core.util.json", "core.util.fs" },
    ALLOC_ZERO_HEAP,
    CONCURRENCY_REENTRANT,
    500,
    4,
    {
        s_master_loader_invariants[0],
        s_master_loader_invariants[1],
        s_master_loader_invariants[2],
        s_master_loader_invariants[3]
    },
    master_loader_preflight
};

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Private Structural Ontology & Non-Anonymous Data Types
--------------------------------------------------------------------------------
None. (Stateless data loader; loaded asset records populate caller buffers).
--------------------------------------------------------------------------------
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Static Module State Storage (.bss Allocations)
--------------------------------------------------------------------------------
None. (Stateless module / pure stack-allocated execution under ALLOC_ZERO_HEAP).
--------------------------------------------------------------------------------
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.4: Pre-Conditional Helper Inlines & Fast Tripwires
--------------------------------------------------------------------------------
*/

static inline bool loader_is_valid_buffer(const void *buf, size_t cap)
{
    return (buf != NULL && cap > 0);
}

/*
================================================================================
BLOCK 3: BODY BLOCK / CORE LOGIC & BUSINESS FUNCTIONS
Stratum: Layer 0 (Core Utilities) | Machine: Master Data Loader
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: Ingestion & Header Parsing Implementation
--------------------------------------------------------------------------------
*/

bool master_loader_load_from_memory(master_loader_t *loader,
                                    const char *json_text,
                                    size_t json_len)
{
    if (loader == NULL || json_text == NULL || json_len == 0) {
        if (loader) {
            loader->status = LOADER_STATUS_ERR_NULL_PARAM;
        }
        return false;
    }

    loader->raw_buf = json_text;
    loader->raw_len = json_len;
    loader->status = LOADER_STATUS_OK;

    /* Step 1: Detect 5-Block Metadata */
    const char *meta_start = NULL;
    size_t meta_len = 0;
    if (json_find_object(json_text, "block_1_metadata", &meta_start, &meta_len)) {
        loader->header.is_5block = true;
        json_get_object_string(json_text, "block_1_metadata", "project",
                               loader->header.project, sizeof(loader->header.project));
        json_get_object_string(json_text, "block_1_metadata", "file",
                               loader->header.file, sizeof(loader->header.file));
        json_get_object_string(json_text, "block_1_metadata", "id",
                               loader->header.id, sizeof(loader->header.id));
        json_get_object_string(json_text, "block_1_metadata", "layer",
                               loader->header.layer, sizeof(loader->header.layer));
        json_get_object_string(json_text, "block_1_metadata", "version",
                               loader->header.version, sizeof(loader->header.version));
    } else {
        loader->header.is_5block = false;
        loader->header.project[0] = '\0';
        loader->header.file[0] = '\0';
        json_get_string(json_text, "subsystem", loader->header.id,
                        sizeof(loader->header.id));
        loader->header.layer[0] = '\0';
        json_get_string(json_text, "version", loader->header.version,
                        sizeof(loader->header.version));
    }

    /* Step 2: Extract Schema ID from Block 2 (or top-level) */
    loader->header.schema_id[0] = '\0';
    json_get_string(json_text, "schema_id", loader->header.schema_id,
                    sizeof(loader->header.schema_id));

    /* Step 3: Locate Block 3 (Body) */
    if (json_find_object(json_text, "block_3_body",
                         &loader->body_start, &loader->body_len)) {
        loader->status = LOADER_STATUS_OK;
        return true;
    }

    /* Step 4: Strict 5-block envelope - block_3_body must exist */
    loader->body_start = NULL;
    loader->body_len = 0;
    loader->status = LOADER_STATUS_ERR_BODY_MISSING;
    debug_trap_tripwire("master_loader", "Missing block_3_body in asset JSON", __FILE__, __LINE__);
    return false;
}

bool master_loader_load_from_path(master_loader_t *loader,
                                  const char *filepath,
                                  char *buf,
                                  size_t buf_size)
{
    if (loader == NULL || filepath == NULL || filepath[0] == '\0' ||
        buf == NULL || buf_size == 0) {
        if (loader) {
            loader->status = LOADER_STATUS_ERR_NULL_PARAM;
        }
        return false;
    }

    memset(loader, 0, sizeof(*loader));

    /* Resolve path via path_finder */
    if (!path_finder_resolve(NULL, filepath, loader->resolved_path,
                             sizeof(loader->resolved_path))) {
        loader->status = LOADER_STATUS_ERR_NOT_FOUND;
        return false;
    }

    /* Read bounded content */
    size_t bytes_read = 0;
    if (!fs_read_file_bounded(loader->resolved_path, buf, buf_size, &bytes_read)) {
        loader->status = LOADER_STATUS_ERR_BUFFER_OVERFLOW;
        return false;
    }

    bool parsed = master_loader_load_from_memory(loader, buf, bytes_read);
    if (parsed) {
        LOG_DEBUG(LOG_CAT_LOG, "MasterLoader", 0.0f,
                  "Loaded asset from path '%s' (%zu bytes, schema: %s)",
                  filepath, bytes_read, loader->header.schema_id);
    }
    return parsed;
}

bool master_loader_load(master_loader_t *loader,
                        const char *domain,
                        const char *rel_path,
                        char *buf,
                        size_t buf_size)
{
    if (loader == NULL || rel_path == NULL || rel_path[0] == '\0' ||
        buf == NULL || buf_size == 0) {
        if (loader) {
            loader->status = LOADER_STATUS_ERR_NULL_PARAM;
        }
        return false;
    }

    memset(loader, 0, sizeof(*loader));

    /* Resolve dynamic path */
    if (!path_finder_resolve(domain, rel_path, loader->resolved_path,
                             sizeof(loader->resolved_path))) {
        loader->status = LOADER_STATUS_ERR_NOT_FOUND;
        return false;
    }

    /* Read bounded file */
    size_t bytes_read = 0;
    if (!fs_read_file_bounded(loader->resolved_path, buf, buf_size, &bytes_read)) {
        loader->status = LOADER_STATUS_ERR_BUFFER_OVERFLOW;
        debug_trap_tripwire("master_loader", "Bounded file read failed or capacity exceeded", __FILE__, __LINE__);
        return false;
    }

    bool parsed = master_loader_load_from_memory(loader, buf, bytes_read);
    if (parsed) {
        LOG_DEBUG(LOG_CAT_LOG, "MasterLoader", 0.0f,
                  "Loaded asset '%s/%s' (%zu bytes, schema: %s)",
                  domain ? domain : "root", rel_path, bytes_read, loader->header.schema_id);
    }
    return parsed;
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: Typed Body Property Extraction Implementation
--------------------------------------------------------------------------------
*/

bool master_loader_get_string(const master_loader_t *loader,
                              const char *sub_obj,
                              const char *key,
                              char *out_val,
                              size_t out_size)
{
    if (loader == NULL || key == NULL || out_val == NULL || out_size == 0) {
        if (out_val && out_size > 0) {
            out_val[0] = '\0';
        }
        return false;
    }

    const char *scope = loader->body_start ? loader->body_start : loader->raw_buf;
    if (!scope) {
        out_val[0] = '\0';
        return false;
    }

    if (sub_obj != NULL && sub_obj[0] != '\0') {
        const char *obj_start = NULL;
        size_t obj_len = 0;
        if (!json_find_object(scope, sub_obj, &obj_start, &obj_len)) {
            out_val[0] = '\0';
            return false;
        }
        return json_get_string(obj_start, key, out_val, out_size);
    }

    return json_get_string(scope, key, out_val, out_size);
}

bool master_loader_get_int(const master_loader_t *loader,
                           const char *sub_obj,
                           const char *key,
                           int *out_val)
{
    if (loader == NULL || key == NULL || out_val == NULL) {
        return false;
    }

    const char *scope = loader->body_start ? loader->body_start : loader->raw_buf;
    if (!scope) {
        return false;
    }

    if (sub_obj != NULL && sub_obj[0] != '\0') {
        const char *obj_start = NULL;
        size_t obj_len = 0;
        if (!json_find_object(scope, sub_obj, &obj_start, &obj_len)) {
            return false;
        }
        return json_get_int(obj_start, key, out_val);
    }

    return json_get_int(scope, key, out_val);
}

bool master_loader_get_size_t(const master_loader_t *loader,
                              const char *sub_obj,
                              const char *key,
                              size_t *out_val)
{
    if (loader == NULL || key == NULL || out_val == NULL) {
        return false;
    }

    const char *scope = loader->body_start ? loader->body_start : loader->raw_buf;
    if (!scope) {
        return false;
    }

    if (sub_obj != NULL && sub_obj[0] != '\0') {
        const char *obj_start = NULL;
        size_t obj_len = 0;
        if (!json_find_object(scope, sub_obj, &obj_start, &obj_len)) {
            return false;
        }
        return json_get_size_t(obj_start, key, out_val);
    }

    return json_get_size_t(scope, key, out_val);
}

bool master_loader_get_float(const master_loader_t *loader,
                             const char *sub_obj,
                             const char *key,
                             float *out_val)
{
    if (loader == NULL || key == NULL || out_val == NULL) {
        return false;
    }

    const char *scope = loader->body_start ? loader->body_start : loader->raw_buf;
    if (!scope) {
        return false;
    }

    if (sub_obj != NULL && sub_obj[0] != '\0') {
        const char *obj_start = NULL;
        size_t obj_len = 0;
        if (!json_find_object(scope, sub_obj, &obj_start, &obj_len)) {
            return false;
        }
        return json_get_float(obj_start, key, out_val);
    }

    return json_get_float(scope, key, out_val);
}

bool master_loader_get_bool(const master_loader_t *loader,
                            const char *sub_obj,
                            const char *key,
                            bool *out_val)
{
    if (loader == NULL || key == NULL || out_val == NULL) {
        return false;
    }

    const char *scope = loader->body_start ? loader->body_start : loader->raw_buf;
    if (!scope) {
        return false;
    }

    if (sub_obj != NULL && sub_obj[0] != '\0') {
        const char *obj_start = NULL;
        size_t obj_len = 0;
        if (!json_find_object(scope, sub_obj, &obj_start, &obj_len)) {
            return false;
        }
        return json_get_bool(obj_start, key, out_val);
    }

    return json_get_bool(scope, key, out_val);
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.3: Array Traversal & Object Slice Implementation
--------------------------------------------------------------------------------
*/

bool master_loader_find_array(const master_loader_t *loader,
                              const char *sub_obj,
                              const char *array_key,
                              const char **out_arr_start,
                              size_t *out_arr_len,
                              size_t *out_count)
{
    if (loader == NULL || array_key == NULL ||
        out_arr_start == NULL || out_arr_len == NULL) {
        return false;
    }

    const char *scope = loader->body_start ? loader->body_start : loader->raw_buf;
    if (!scope) {
        return false;
    }

    if (sub_obj != NULL && sub_obj[0] != '\0') {
        const char *obj_start = NULL;
        size_t obj_len = 0;
        if (!json_find_object(scope, sub_obj, &obj_start, &obj_len)) {
            return false;
        }
        scope = obj_start;
    }

    if (!json_find_array(scope, array_key, out_arr_start, out_arr_len)) {
        return false;
    }

    if (out_count) {
        *out_count = json_array_get_count(*out_arr_start, *out_arr_len);
    }

    return true;
}

bool master_loader_get_array_object(const char *arr_start,
                                    size_t arr_len,
                                    size_t index,
                                    char *out_obj,
                                    size_t out_size)
{
    return json_array_get_object(arr_start, arr_len, index, out_obj, out_size);
}

/*
================================================================================
BLOCK 4: AUDIT & VERIFICATION PROCEDURES BLOCK
Stratum: Layer 0 (Core Utilities) | Machine: Master Data Loader
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.1: Contract & Schema Verification Implementation
--------------------------------------------------------------------------------
*/

bool master_loader_validate_schema(const master_loader_t *loader,
                                   const char *expected_schema_id)
{
    if (loader == NULL || expected_schema_id == NULL) {
        return false;
    }

    if (loader->header.schema_id[0] == '\0') {
        return false;
    }

    return (strcmp(loader->header.schema_id, expected_schema_id) == 0);
}

bool master_loader_validate_version(const master_loader_t *loader,
                                    const char *expected_version)
{
    if (loader == NULL || expected_version == NULL) {
        return false;
    }

    if (loader->header.version[0] == '\0') {
        return false;
    }

    return (strncmp(loader->header.version, expected_version,
                    strlen(expected_version)) == 0);
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.2: Machine Self-Audit & Pre-Flight Tripwires
--------------------------------------------------------------------------------
*/

diag_result_t master_loader_preflight(void)
{
    diag_result_t res;
    res.subsystem = master_loader_manifest.module_id;
    res.check_name = "Master Loader 5-Block Ingest & Contract Validation";
    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.details = "Master loader: 5-block envelope, schema validation, and typed getters verified.";

    /* Check 1: NULL resilience */
    master_loader_t loader;
    char buf[128];
    if (master_loader_load(NULL, NULL, NULL, NULL, 0) != false ||
        master_loader_load(&loader, NULL, NULL, buf, sizeof(buf)) != false ||
        master_loader_validate_schema(NULL, NULL) != false) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "master_loader NULL input failed resilience contract.";
        return res;
    }

    /* Check 2: Load and validate 5-block math formulas */
    static char file_buf[FS_BUFFER_LARGE_BYTES];
    if (!master_loader_load(&loader, "config", "runtime/math/formulas.json",
                            file_buf, sizeof(file_buf))) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "master_loader failed to load config/runtime/math/formulas.json.";
        return res;
    }

    if (!loader.header.is_5block) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "formulas.json was not identified as a 5-block document.";
        return res;
    }

    if (!master_loader_validate_schema(&loader, "tenlux.data.math_formulas.v1")) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "Schema ID validation failed for math formulas.";
        return res;
    }

    if (!master_loader_validate_version(&loader, "0.1.1-dev")) {
        res.passed = false;
        res.severity = SEVERITY_WARNING;
        res.details = "Version validation failed for math formulas (expected 0.1.1-dev).";
        return res;
    }

    /* Check 3: Extract body property */
    char formula_str[128];
    if (!master_loader_get_string(&loader, "physical_damage", "formula",
                                  formula_str, sizeof(formula_str))) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "Failed to extract physical_damage formula string from body.";
        return res;
    }

    int cap_pct = 0;
    if (!master_loader_get_int(&loader, "physical_mitigation", "mitigation_cap_pct", &cap_pct) ||
        cap_pct != 90) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "Failed to extract physical_mitigation cap percentage (expected 90).";
        return res;
    }

    /* Check 4: Block 5 Executable Procedures */
    loader_header_t hdr;
    if (!master_loader_execute_inspect("config", "runtime/math/formulas.json", &hdr) ||
        !hdr.is_5block || strcmp(hdr.schema_id, "tenlux.data.math_formulas.v1") != 0) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "master_loader_execute_inspect failed header inspection.";
        return res;
    }

    master_loader_t verified_loader;
    static char verified_buf[FS_BUFFER_LARGE_BYTES];
    if (!master_loader_execute_load_verified("config", "runtime/math/formulas.json",
                                            "tenlux.data.math_formulas.v1",
                                            verified_buf, sizeof(verified_buf),
                                            &verified_loader)) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "master_loader_execute_load_verified failed verified load procedure.";
        return res;
    }

    return res;
}

bool master_loader_audit(void)
{
    diag_result_t res = master_loader_preflight();
    return res.passed && res.severity != SEVERITY_FATAL;
}

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
--------------------------------------------------------------------------------
*/

bool master_loader_execute_inspect(const char *domain,
                                   const char *rel_path,
                                   loader_header_t *out_header)
{
    if (out_header == NULL) {
        return false;
    }
    memset(out_header, 0, sizeof(*out_header));

    if (rel_path == NULL || rel_path[0] == '\0') {
        return false;
    }

    char file_buf[FS_BUFFER_MEDIUM_BYTES];
    master_loader_t loader;
    if (!master_loader_load(&loader, domain, rel_path, file_buf, sizeof(file_buf))) {
        return false;
    }

    *out_header = loader.header;
    return true;
}

size_t master_loader_execute_dump_card(const loader_header_t *header,
                                       char *out_buf,
                                       size_t buf_size)
{
    if (header == NULL || out_buf == NULL || buf_size == 0) {
        return 0;
    }
    int written = snprintf(
        out_buf, buf_size,
        "============================================================\n"
        "             MASTER DATA LOADER ENVELOPE CARD               \n"
        "============================================================\n"
        "Project:       %s\n"
        "Dataset ID:    %s\n"
        "Layer:         %s\n"
        "Version:       %s\n"
        "Schema ID:     %s\n"
        "Envelope:      %s\n"
        "Source File:   %s\n"
        "============================================================\n",
        header->project[0] ? header->project : "(Unset)",
        header->id[0] ? header->id : "(Unset)",
        header->layer[0] ? header->layer : "(Unset)",
        header->version[0] ? header->version : "(Unset)",
        header->schema_id[0] ? header->schema_id : "(Unset)",
        header->is_5block ? "VALID 5-BLOCK PROTOCOL" : "LEGACY / NON-CONFORMANT",
        header->file[0] ? header->file : "(Memory Stream)"
    );
    if (written < 0 || (size_t)written >= buf_size) {
        return 0;
    }
    return (size_t)written;
}

bool master_loader_execute_load_verified(const char *domain,
                                         const char *rel_path,
                                         const char *expected_schema_id,
                                         char *buf,
                                         size_t buf_size,
                                         master_loader_t *out_loader)
{
    if (out_loader == NULL || buf == NULL || buf_size == 0 ||
        rel_path == NULL || rel_path[0] == '\0') {
        return false;
    }

    if (!master_loader_load(out_loader, domain, rel_path, buf, buf_size)) {
        return false;
    }

    if (expected_schema_id && expected_schema_id[0] != '\0') {
        if (!master_loader_validate_schema(out_loader, expected_schema_id)) {
            out_loader->status = LOADER_STATUS_ERR_SCHEMA_MISMATCH;
            debug_trap_tripwire("master_loader", "Schema contract verification failed in execute_load_verified", __FILE__, __LINE__);
            return false;
        }
    }

    return true;
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.2: Subsystem Lifecycle Initialization & Teardown
--------------------------------------------------------------------------------
*/

void master_loader_init(void)
{
    path_finder_init();
    json_utils_init();
    fs_utils_init();
}

void master_loader_shutdown(void)
{
    json_utils_shutdown();
    path_finder_shutdown();
    fs_utils_shutdown();
}

/* End of master_loader.c */
