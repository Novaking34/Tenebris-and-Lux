// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          master_loader.c
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
//       io/master_loader.h
//       io/path_finder.h
//       io/json_utils.h
//       io/fs_utils.h
//       core/string_utils.h
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

// =============================================================================
// BLOCK 2: OPENING BLOCK / STATIC DATA
// Stratum: Layer 0 (Core Utilities) | Machine: Master Data Loader
// =============================================================================

#include "io/master_loader.h"
#include "io/path_finder.h"
#include "io/json_utils.h"
#include "io/fs_utils.h"
#include "core/string_utils.h"
#include <stdio.h>
#include <string.h>

static const char *const s_master_loader_invariants[] = {
    "Zero dynamic heap allocation during dataset loading",
    "Strict schema ID and semver validation contract",
    "Scoped extraction inside Block 3 body with legacy fallback",
    "Universal 5-Block Lifecycle Protocol enforced across header and source"
};

const module_metadata_t master_loader_manifest = {
    "core.io.master_loader",
    "master_loader.h",
    LAYER_0_CORE_UTIL,
    { 0, 1, 0 },
    2,
    "2026-11-05",
    "Zero-heap universal master data loader and 5-block parser",
    3,
    { "core.io.path_finder", "core.util.json", "core.util.fs", NULL },
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

// =============================================================================
// BLOCK 3: BODY BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: Master Data Loader
// =============================================================================

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

    /* Step 4: Graceful legacy fallback - entire payload treated as body */
    loader->body_start = json_text;
    loader->body_len = json_len;
    loader->status = LOADER_STATUS_OK;
    return true;
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

    return master_loader_load_from_memory(loader, buf, bytes_read);
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
        return false;
    }

    return master_loader_load_from_memory(loader, buf, bytes_read);
}

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

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: Master Data Loader
// =============================================================================

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
    if (!master_loader_load(&loader, "system", "runtime/math/formulas.json",
                            file_buf, sizeof(file_buf))) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "master_loader failed to load system/runtime/math/formulas.json.";
        return res;
    }

    if (!loader.header.is_5block) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "formulas.json was not identified as a 5-block document.";
        return res;
    }

    if (!master_loader_validate_schema(&loader, "tenlux.data.math_formulas.v1")) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Schema ID validation failed for math formulas.";
        return res;
    }

    if (!master_loader_validate_version(&loader, "0.1.0")) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Version validation failed for math formulas (expected 0.1.0).";
        return res;
    }

    /* Check 3: Extract body property */
    char formula_str[128];
    if (!master_loader_get_string(&loader, "physical_damage", "formula",
                                  formula_str, sizeof(formula_str))) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Failed to extract physical_damage formula string from body.";
        return res;
    }

    int cap_pct = 0;
    if (!master_loader_get_int(&loader, "physical_mitigation", "mitigation_cap_pct", &cap_pct) ||
        cap_pct != 90) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Failed to extract physical_mitigation cap percentage (expected 90).";
        return res;
    }

    /* Check 4: Block 5 Executable Procedures */
    loader_header_t hdr;
    if (!master_loader_execute_inspect("system", "runtime/math/formulas.json", &hdr) ||
        !hdr.is_5block || strcmp(hdr.schema_id, "tenlux.data.math_formulas.v1") != 0) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "master_loader_execute_inspect failed header inspection.";
        return res;
    }

    master_loader_t verified_loader;
    static char verified_buf[FS_BUFFER_LARGE_BYTES];
    if (!master_loader_execute_load_verified("system", "runtime/math/formulas.json",
                                            "tenlux.data.math_formulas.v1",
                                            verified_buf, sizeof(verified_buf),
                                            &verified_loader)) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
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

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: Master Data Loader
// =============================================================================

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 5.1: EXECUTABLE PROCEDURES & OBJECT CONSTRUCTORS ]
// -----------------------------------------------------------------------------

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
            return false;
        }
    }

    return true;
}

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 5.2: SUBSYSTEM LIFECYCLE INITIALIZATION & TEARDOWN ]
// -----------------------------------------------------------------------------

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

// =============================================================================
// End of master_loader.c
// =============================================================================
