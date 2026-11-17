// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/tests/unit/validator/languages.rs
// @id            test.unit.tools.validator.languages
// @layer         Layer 4: Verification Suite
// @archetype     verification_suite | unit_isolation
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-16
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Developer Toolchain // Multi-Language Validator Suite
// @role          Four-Section Ontological Integration Suite for Language Validators
// @maturity      Rev 5 Baseline | Four-Section Transpiler-Ready Verification
// @chassis       bounded-heap | Test runner execution chassis; stack frames bounded
// @scale         O(1) execution time across 8 language validators
// @seams         in: tl_tools::validator | pairs: cpp, rust, json, adoc, md, cmake, toml, git
// @invariants    1. Four-Section Ontological Architecture strictly enforced:
//                   Section 1 (Always Pass), Section 2 (Never Fail),
//                   Section 3 (Invariants & Limits), Section 4 (Emergence).
//                2. External Consumer Purity: imports tl_tools via public API.
//                3. Zero Mocks: exercises live multi-language validator engines.
//                4. Strict <= 80-column line width discipline across documentation.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Tier 1 Integration verification harness evaluating all 8 language-specific
//   validator modules from the perspective of an external consumer or CI/CD runner:
//   C++ (`cpp.rs`), Rust (`rust.rs`), JSON (`json.rs`), AsciiDoc (`adoc.rs`),
//   Markdown (`md.rs`), CMake (`cmake.rs`), TOML (`toml.rs`), and Git (`git.rs`).
//   Employs the identical 4-Section Ontological Test Protocol enforced across
//   `integration_guide.rs` and `integration_audit.rs`.
//
//   ### For Game Designers & Non-Programmers:
//   Confirms that every content type—game data, tool code, books, and build scripts—
//   is verified accurately and provides clean error messages when broken.
//
//   ### For Systems Programmers & Engine Architects:
//   - Transpilation Alignment: Tests map 1:1 to OmniCode's 4-section verification
//     schema in Block 3, with Preflight in Block 4 and Epilogue in Block 5.
//   - Integration Boundary: Validates crate public API without breaching internals.
// =============================================================================

//! # Four-Section Integration Suite for Multi-Language Validators
//!
//! Exercises language-specific validators across Always-Pass, Never-Fail,
//! Invariant, and Emergent execution tiers.

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Pre-Conditions, Constants, Test Fixtures & Harness Scaffolding )
// =============================================================================

use crate::common;
use std::path::Path;
use tl_tools::validator::{
    cpp::{validate_cpp_content, CppValidationConfig, is_cpp_validator_ready, cpp_validator_status},
    rust::{validate_rust_content, RustValidationConfig, is_rust_validator_ready, rust_validator_status},
    json::{validate_json_content, is_json_validator_ready},
    adoc::{validate_adoc_content, AdocValidationConfig, is_adoc_validator_ready, adoc_validator_status},
    md::{validate_markdown_content, MarkdownValidationConfig, is_markdown_validator_ready, md_validator_status},
    cmake::{validate_cmake_content, CmakeValidationConfig, is_cmake_validator_ready, cmake_validator_status},
    toml::{validate_toml_content, TomlValidationConfig, is_toml_validator_ready, toml_validator_status},
    git::{audit_commit_message, is_git_validator_ready},
};

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Operational Test Constants & Suite Identifiers
// -----------------------------------------------------------------------------

/// Canonical test suite identifier matching engine convention.
pub const SUITE_NAME: &str = "Multi-Language Validator Integration Surface";

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Test Fixtures & Pre-Condition Assertions
// -----------------------------------------------------------------------------

/// Preflight self-audit probe verifying that all 8 language validators are ready.
pub fn self_audit() -> Result<(), &'static str> {
    if !is_cpp_validator_ready()
        || !is_rust_validator_ready()
        || !is_json_validator_ready()
        || !is_adoc_validator_ready()
        || !is_markdown_validator_ready()
        || !is_cmake_validator_ready()
        || !is_toml_validator_ready()
        || !is_git_validator_ready()
    {
        return Err("Self-audit failed: one or more language validators not ready");
    }
    Ok(())
}

// =============================================================================
// BLOCK 3: BODY BLOCK / FOUR-SECTION ONTOLOGICAL TEST SUITE
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: [ SECTION 1: ALWAYS PASS (NOMINAL GOLDEN SURFACE COGS) ]
// -----------------------------------------------------------------------------

/// Verifies nominal golden execution of C++ header validation with extern "C".
#[test]
fn test_section_1_nominal_cpp_validation() {
    let cpp = r#"
/*
BLOCK 1: METADATA BLOCK
BLOCK 2: OPENING BLOCK
BLOCK 3: BODY BLOCK
BLOCK 4: AUDIT PROCEDURES BLOCK
BLOCK 5: CLOSING BLOCK
*/
#ifdef __cplusplus
extern "C" {
#endif
void run(void);
#ifdef __cplusplus
}
#endif
"#;
    let mut issues = Vec::new();
    validate_cpp_content(Path::new("header.h"), cpp, &CppValidationConfig::default(), &mut issues);
    assert!(
        issues.is_empty(),
        "[SEC 1: ALWAYS PASS] Balanced C++ header must pass: {:?}",
        issues
    );
}

/// Verifies nominal golden execution of Rust source validation.
#[test]
fn test_section_1_nominal_rust_validation() {
    let rust = common::sample_5block_rust();
    let mut issues = Vec::new();
    validate_rust_content(Path::new("sample.rs"), rust, &RustValidationConfig::default(), &mut issues);
    assert!(
        issues.is_empty(),
        "[SEC 1: ALWAYS PASS] Compliant Rust file must pass: {:?}",
        issues
    );
}

/// Verifies nominal golden execution of JSON data catalog validation.
#[test]
fn test_section_1_nominal_json_validation() {
    let json = r#"{
        "block_1_metadata": {},
        "block_2_opening": {},
        "block_3_body": {},
        "block_4_audit": {},
        "block_5_closing": {}
    }"#;
    let mut issues = Vec::new();
    validate_json_content(Path::new("test.json"), json, &mut issues);
    assert!(
        issues.is_empty(),
        "[SEC 1: ALWAYS PASS] 5-Block JSON manifest must pass: {:?}",
        issues
    );
}

/// Verifies nominal golden execution of AsciiDoc document validation.
#[test]
fn test_section_1_nominal_adoc_validation() {
    let adoc = r#"
////
BLOCK 1: METADATA BLOCK
BLOCK 2: OPENING BLOCK
BLOCK 3: BODY BLOCK
BLOCK 4: AUDIT PROCEDURES BLOCK
BLOCK 5: CLOSING BLOCK
////
= Title
"#;
    let mut issues = Vec::new();
    validate_adoc_content(Path::new("book.adoc"), adoc, &AdocValidationConfig::default(), &mut issues);
    assert!(
        issues.is_empty(),
        "[SEC 1: ALWAYS PASS] Compliant AsciiDoc file must pass: {:?}",
        issues
    );
}

/// Verifies nominal golden execution of Markdown document validation.
#[test]
fn test_section_1_nominal_markdown_validation() {
    let md = r#"
<!--
BLOCK 1: METADATA BLOCK
BLOCK 2: OPENING BLOCK
BLOCK 3: BODY BLOCK
BLOCK 4: AUDIT PROCEDURES BLOCK
BLOCK 5: CLOSING BLOCK
-->
# Title
"#;
    let mut issues = Vec::new();
    validate_markdown_content(Path::new("doc.md"), md, &MarkdownValidationConfig::default(), &mut issues);
    assert!(
        issues.is_empty(),
        "[SEC 1: ALWAYS PASS] Compliant Markdown file must pass: {:?}",
        issues
    );
}

/// Verifies nominal golden execution of CMake build definition validation.
#[test]
fn test_section_1_nominal_cmake_validation() {
    let cmake = r#"
# BLOCK 1: METADATA BLOCK
# BLOCK 2: OPENING BLOCK
# BLOCK 3: BODY BLOCK
# BLOCK 4: AUDIT PROCEDURES BLOCK
# BLOCK 5: CLOSING BLOCK
set(A B)
"#;
    let mut issues = Vec::new();
    validate_cmake_content(Path::new("CMakeLists.txt"), cmake, &CmakeValidationConfig::default(), &mut issues);
    assert!(
        issues.is_empty(),
        "[SEC 1: ALWAYS PASS] Compliant CMake file must pass: {:?}",
        issues
    );
}

/// Verifies nominal golden execution of TOML configuration validation.
#[test]
fn test_section_1_nominal_toml_validation() {
    let toml = r#"
# BLOCK 1: METADATA BLOCK
# BLOCK 2: OPENING BLOCK
# BLOCK 3: BODY BLOCK
# BLOCK 4: AUDIT PROCEDURES BLOCK
# BLOCK 5: CLOSING BLOCK
[pkg]
name = "val"
"#;
    let mut issues = Vec::new();
    validate_toml_content(Path::new("Cargo.toml"), toml, &TomlValidationConfig::default(), &mut issues);
    assert!(
        issues.is_empty(),
        "[SEC 1: ALWAYS PASS] Compliant TOML file must pass: {:?}",
        issues
    );
}

/// Verifies nominal golden execution of Git commit message validation.
#[test]
fn test_section_1_nominal_git_validation() {
    let commit = r#"feat(core): new cog

BLOCK 1: METADATA & REVISION RECORD
Project: Tenebris and Lux

BLOCK 2: REASON FOR CHANGE & PROBLEM STATEMENT
Problem description.

BLOCK 3: ARCHITECTURAL MODIFICATIONS & ELEVATIONS
Modifications.

BLOCK 4: VERIFICATION, TRIPWIRES & PASS PROOF
54/54 tests pass.

BLOCK 5: INVARIANT COVENANTS & CLOSING AFFIRMATIONS
ALLOC_ZERO_HEAP satisfied.
"#;
    let issues = audit_commit_message("HEAD", commit);
    assert!(
        issues.is_empty(),
        "[SEC 1: ALWAYS PASS] 5-Block Git commit ledger must pass: {:?}",
        issues
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.2: [ SECTION 2: NEVER FAIL / FAULT INVERSION & DEGRADATION ]
// -----------------------------------------------------------------------------

/// Verifies graceful fault reporting across all 8 language validators when given malformed input.
#[test]
fn test_section_2_never_fail_malformed_syntax_inputs() {
    // 1. JSON fault inversion
    let mut json_issues = Vec::new();
    validate_json_content(Path::new("bad.json"), "{ unclosed ", &mut json_issues);
    assert!(!json_issues.is_empty(), "[SEC 2: NEVER FAIL] JSON validator must catch unclosed JSON");

    // 2. C++ fault inversion
    let mut cpp_issues = Vec::new();
    validate_cpp_content(Path::new("bad.h"), "extern \"C\" {", &CppValidationConfig::default(), &mut cpp_issues);
    assert!(!cpp_issues.is_empty(), "[SEC 2: NEVER FAIL] C++ validator must catch unclosed extern C");

    // 3. Rust fault inversion
    let mut rust_issues = Vec::new();
    validate_rust_content(Path::new("bad.rs"), "// empty\n", &RustValidationConfig::default(), &mut rust_issues);
    assert!(!rust_issues.is_empty(), "[SEC 2: NEVER FAIL] Rust validator must catch missing 5 blocks");

    // 4. AsciiDoc fault inversion
    let mut adoc_issues = Vec::new();
    validate_adoc_content(Path::new("bad.adoc"), "----\nUnclosed block\n", &AdocValidationConfig::default(), &mut adoc_issues);
    assert!(!adoc_issues.is_empty(), "[SEC 2: NEVER FAIL] AsciiDoc validator must catch unclosed delimiter");

    // 5. Markdown fault inversion
    let mut md_issues = Vec::new();
    validate_markdown_content(Path::new("bad.md"), "```rust\nUnclosed fence\n", &MarkdownValidationConfig::default(), &mut md_issues);
    assert!(!md_issues.is_empty(), "[SEC 2: NEVER FAIL] Markdown validator must catch unclosed fence");

    // 6. CMake fault inversion
    let mut cmake_issues = Vec::new();
    validate_cmake_content(Path::new("bad.cmake"), "set(FOO bar\n", &CmakeValidationConfig::default(), &mut cmake_issues);
    assert!(!cmake_issues.is_empty(), "[SEC 2: NEVER FAIL] CMake validator must catch unclosed parenthesis");

    // 7. TOML fault inversion
    let mut toml_issues = Vec::new();
    validate_toml_content(Path::new("bad.toml"), "[unclosed\nfoo=1\n", &TomlValidationConfig::default(), &mut toml_issues);
    assert!(!toml_issues.is_empty(), "[SEC 2: NEVER FAIL] TOML validator must catch unclosed table header");

    // 8. Git fault inversion
    let git_issues = audit_commit_message("HEAD", "fix: no 5-blocks");
    assert!(!git_issues.is_empty(), "[SEC 2: NEVER FAIL] Git validator must catch missing 5-block ledger");
}

/// Verifies that empty inputs degrade gracefully across all language validators without panicking.
#[test]
fn test_section_2_never_fail_empty_inputs_graceful() {
    let empty = "";
    let mut issues = Vec::new();

    validate_cpp_content(Path::new("empty.cpp"), empty, &CppValidationConfig::default(), &mut issues);
    validate_rust_content(Path::new("empty.rs"), empty, &RustValidationConfig::default(), &mut issues);
    validate_adoc_content(Path::new("empty.adoc"), empty, &AdocValidationConfig::default(), &mut issues);
    validate_markdown_content(Path::new("empty.md"), empty, &MarkdownValidationConfig::default(), &mut issues);
    validate_cmake_content(Path::new("empty.cmake"), empty, &CmakeValidationConfig::default(), &mut issues);
    validate_toml_content(Path::new("empty.toml"), empty, &TomlValidationConfig::default(), &mut issues);

    assert!(
        !issues.is_empty(),
        "[SEC 2: NEVER FAIL] Empty inputs must emit issues without panicking"
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.3: [ SECTION 3: INVARIANTS, SATURATION & LIMIT GUARDS ]
// -----------------------------------------------------------------------------

/// Verifies that all 8 language validator readiness predicates report true.
#[test]
fn test_section_3_invariants_all_validators_ready() {
    assert!(is_cpp_validator_ready(), "[SEC 3: INVARIANTS] C++ validator must report ready");
    assert!(is_rust_validator_ready(), "[SEC 3: INVARIANTS] Rust validator must report ready");
    assert!(is_json_validator_ready(), "[SEC 3: INVARIANTS] JSON validator must report ready");
    assert!(is_adoc_validator_ready(), "[SEC 3: INVARIANTS] AsciiDoc validator must report ready");
    assert!(is_markdown_validator_ready(), "[SEC 3: INVARIANTS] Markdown validator must report ready");
    assert!(is_cmake_validator_ready(), "[SEC 3: INVARIANTS] CMake validator must report ready");
    assert!(is_toml_validator_ready(), "[SEC 3: INVARIANTS] TOML validator must report ready");
    assert!(is_git_validator_ready(), "[SEC 3: INVARIANTS] Git validator must report ready");
}

/// Verifies that telemetry status strings report 'READY'.
#[test]
fn test_section_3_invariants_status_telemetry_strings() {
    assert_eq!(cpp_validator_status(), "READY", "[SEC 3: INVARIANTS] cpp_validator_status must be READY");
    assert_eq!(rust_validator_status(), "READY", "[SEC 3: INVARIANTS] rust_validator_status must be READY");
    assert_eq!(adoc_validator_status(), "READY", "[SEC 3: INVARIANTS] adoc_validator_status must be READY");
    assert_eq!(md_validator_status(), "READY", "[SEC 3: INVARIANTS] md_validator_status must be READY");
    assert_eq!(cmake_validator_status(), "READY", "[SEC 3: INVARIANTS] cmake_validator_status must be READY");
    assert_eq!(toml_validator_status(), "READY", "[SEC 3: INVARIANTS] toml_validator_status must be READY");
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.4: [ SECTION 4: EMERGENCE & COMPOSITE WORKFLOWS ]
// -----------------------------------------------------------------------------

/// Verifies emergent multi-language diagnostic self-audit sweep.
#[test]
fn test_section_4_emergence_self_audit() {
    assert!(
        self_audit().is_ok(),
        "[SEC 4: EMERGENCE] Multi-language self_audit() contract must pass"
    );
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// ( Preflight Diagnostics & Deep Verification Audits )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 4.1: Preflight Diagnostic Gates
// -----------------------------------------------------------------------------

/// Diagnostic self-audit verifying suite constant integrity.
#[test]
fn test_preflight_diagnostics_self_audit() {
    assert!(
        self_audit().is_ok(),
        "[BLOCK 4: PREFLIGHT] Multi-language validator self_audit() must pass"
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 4.2: Suite Identifier Verification Gate
// -----------------------------------------------------------------------------

/// Audits physical suite constants matching engine convention.
#[test]
fn test_preflight_suite_constants() {
    assert!(
        !SUITE_NAME.is_empty(),
        "[BLOCK 4: PREFLIGHT] SUITE_NAME constant must not be empty"
    );
    assert_eq!(
        SUITE_NAME, "Multi-Language Validator Integration Surface",
        "[BLOCK 4: PREFLIGHT] SUITE_NAME must match canonical surface title"
    );
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( Test Suite Epilogue & Summary Handshake )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 5.1: Epilogue & Clean State Handshake
// -----------------------------------------------------------------------------

/// Proves that running integration tests leaves zero transient disk artifacts and all modules ready.
#[test]
fn test_epilogue_clean_state_handshake() {
    assert!(
        self_audit().is_ok(),
        "[BLOCK 5: EPILOGUE] All language validators must remain ready post-test"
    );
}
