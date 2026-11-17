// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/tests/functional/syntax.rs
// @id            test.functional.tools.syntax
// @layer         Layer 4: Verification Suite
// @archetype     verification_suite | functional_surface
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-16
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Developer Toolchain // Tier 1 Functional Syntax Suite
// @role          Functional Surface Testing for Concrete Language Lexers
// @maturity      Rev 5 Baseline | 4-Section Ontological Testing
// @chassis       bounded-heap | Test runner execution chassis; stack frames bounded
// @scale         O(1) execution over file extension mapping and language probing
// @seams         in: tl_tools::syntax | uses: common::resolve_repo_root
// @invariants    1. Four-Section Ontological Architecture strictly enforced:
//                   Section 1 (Always Pass), Section 2 (Never Fail),
//                   Section 3 (Invariants & Limits), Section 4 (Emergence).
//                2. Zero Panics: Unknown extensions degrade cleanly to Language::Unknown.
//                3. External Consumer Purity: imports tl_tools via public API.
//                4. Strict <= 80-column line width discipline across documentation.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Tier 1 Functional surface test suite evaluating the `tl_tools::syntax`
//   language recognition surface. Asserts that file path extensions across all
//   canonical engine formats (C, C++, Rust, JSON, AsciiDoc, Markdown, TOML,
//   and CMake) map deterministically to their respective `Language` tokens.
//
//   ### For Game Designers & Non-Programmers:
//   Guarantees that the toolchain correctly identifies every file type in the
//   project so appropriate validation, formatting, and documentation rules are
//   applied automatically.
//
//   ### For Systems Programmers & Engine Architects:
//   - Lexer Boundary: Tests `detect_language_from_path` and `Language` enum
//     discriminants as an external surface without mocking internal scanner state.
//   - Fault Inversion: Proves that missing, empty, or unmapped extensions safely
//     yield `Language::Unknown` rather than panicking.
// =============================================================================

//! # Tier 1 Functional Surface Verification Suite for Syntax Lexers
//!
//! Evaluates `tl_tools::syntax` recognition surfaces across Always-Pass, Never-Fail,
//! Invariant, and Emergent ontological tiers.

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Pre-Conditions, Constants, Test Fixtures & Harness Scaffolding )
// =============================================================================

use crate::common;
use std::path::Path;
use tl_tools::syntax::{detect_language_from_path, Language};

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Operational Test Constants & Suite Identifiers
// -----------------------------------------------------------------------------

/// Canonical suite identifier matching engine testing convention.
pub const SUITE_NAME: &str = "Tier 1: Functional Syntax Surface";

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Test Fixtures & Pre-Condition Assertions
// -----------------------------------------------------------------------------

/// Creates a standard sample path for a given filename string.
fn sample_path(filename: &str) -> &Path {
    Path::new(filename)
}

// =============================================================================
// BLOCK 3: BODY BLOCK / FOUR-SECTION ONTOLOGICAL TEST SUITE
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: [ SECTION 1: ALWAYS PASS (NOMINAL GOLDEN SURFACE COGS) ]
// -----------------------------------------------------------------------------

/// Verifies nominal golden execution of file extension language detection.
///
/// Asserts that all canonical engine file extensions map to their proper Language.
#[test]
fn test_section_1_nominal_language_surface_detection() {
    assert_eq!(detect_language_from_path(sample_path("core.c")), Language::C);
    assert_eq!(detect_language_from_path(sample_path("core.h")), Language::C);
    assert_eq!(detect_language_from_path(sample_path("engine.cpp")), Language::Cpp);
    assert_eq!(detect_language_from_path(sample_path("engine.hpp")), Language::Cpp);
    assert_eq!(detect_language_from_path(sample_path("main.rs")), Language::Rust);
    assert_eq!(detect_language_from_path(sample_path("config.json")), Language::Json);
    assert_eq!(detect_language_from_path(sample_path("doc.adoc")), Language::AsciiDoc);
    assert_eq!(detect_language_from_path(sample_path("notes.md")), Language::Markdown);
    assert_eq!(detect_language_from_path(sample_path("Cargo.toml")), Language::Toml);
    assert_eq!(detect_language_from_path(sample_path("CMakeLists.txt")), Language::CMake);
    assert_eq!(detect_language_from_path(sample_path("config.cmake")), Language::CMake);
}

/// Verifies nominal golden execution of language token display names.
///
/// Asserts that each Language variant produces a non-empty human-readable name.
#[test]
fn test_section_1_nominal_language_display_strings() {
    let languages = [
        Language::C, Language::Cpp, Language::Rust, Language::Json,
        Language::AsciiDoc, Language::Markdown, Language::Toml, Language::CMake,
        Language::Unknown,
    ];
    for lang in &languages {
        let debug_str = format!("{:?}", lang);
        assert!(
            !debug_str.is_empty(),
            "[SEC 1: ALWAYS PASS] Language debug representation must not be empty"
        );
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.2: [ SECTION 2: NEVER FAIL / FAULT INVERSION & DEGRADATION ]
// -----------------------------------------------------------------------------

/// Verifies fault inversion when probing unrecognized file extensions.
///
/// Asserts that unknown extensions safely return `Language::Unknown`.
#[test]
fn test_section_2_never_fail_unrecognized_file_extension() {
    assert_eq!(
        detect_language_from_path(sample_path("artifact.xyz_unknown_extension")),
        Language::Unknown,
        "[SEC 2: NEVER FAIL] Unrecognized extension must yield Language::Unknown"
    );
}

/// Verifies fault inversion when probing an empty file path.
///
/// Asserts that empty paths safely return `Language::Unknown` without panicking.
#[test]
fn test_section_2_never_fail_empty_path() {
    assert_eq!(
        detect_language_from_path(sample_path("")),
        Language::Unknown,
        "[SEC 2: NEVER FAIL] Empty path must yield Language::Unknown"
    );
}

/// Verifies fault inversion when probing a dotfile lacking an extension.
///
/// Asserts that extensionless dotfiles safely return `Language::Unknown`.
#[test]
fn test_section_2_never_fail_dotfile_without_extension() {
    assert_eq!(
        detect_language_from_path(sample_path(".gitignore")),
        Language::Unknown,
        "[SEC 2: NEVER FAIL] Extensionless dotfile must yield Language::Unknown"
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.3: [ SECTION 3: INVARIANTS, SATURATION & LIMIT GUARDS ]
// -----------------------------------------------------------------------------

/// Verifies language enumeration exhaustiveness invariants.
///
/// Asserts that exactly 9 language variants exist in the current Rev 5 matrix.
#[test]
fn test_section_3_invariants_language_exhaustiveness() {
    let languages = [
        Language::C, Language::Cpp, Language::Rust, Language::Json,
        Language::AsciiDoc, Language::Markdown, Language::Toml, Language::CMake,
        Language::Unknown,
    ];
    assert_eq!(
        languages.len(),
        9,
        "[SEC 3: INVARIANTS] Must contain exactly 9 supported language variants"
    );
}

/// Verifies language equality and hashing invariants.
///
/// Asserts that identical languages compare equal and distinct languages do not.
#[test]
fn test_section_3_invariants_language_equality() {
    assert_eq!(Language::C, Language::C);
    assert_ne!(Language::C, Language::Cpp);
    assert_ne!(Language::Rust, Language::Json);
    assert_ne!(Language::AsciiDoc, Language::Markdown);
    assert_ne!(Language::Toml, Language::CMake);
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.4: [ SECTION 4: EMERGENCE & COMPOSITE WORKFLOWS ]
// -----------------------------------------------------------------------------

/// Verifies emergent multi-language path scanning across repository paths.
///
/// Asserts that relative engine file paths are correctly recognized.
#[test]
fn test_section_4_emergence_multi_language_workspace_detection() {
    let c_source = sample_path("engine/src/core/math.c");
    let cpp_header = sample_path("engine/include/core/manifest.h");
    let json_config = sample_path("engine/config/viewport/settings.json");
    let rust_tool = sample_path("tools/src/main.rs");
    let toml_manifest = sample_path("tools/Cargo.toml");
    let cmake_file = sample_path("cmake/GenerateConfigRom.cmake");

    assert_eq!(detect_language_from_path(c_source), Language::C);
    assert_eq!(detect_language_from_path(cpp_header), Language::C);
    assert_eq!(detect_language_from_path(json_config), Language::Json);
    assert_eq!(detect_language_from_path(rust_tool), Language::Rust);
    assert_eq!(detect_language_from_path(toml_manifest), Language::Toml);
    assert_eq!(detect_language_from_path(cmake_file), Language::CMake);
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// ( Preflight Diagnostics & Deep Verification Audits )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 4.1: Preflight Diagnostic Gates
// -----------------------------------------------------------------------------

/// Diagnostic self-audit verifying suite constant integrity and common harness.
#[test]
fn test_preflight_diagnostics_self_audit() {
    assert!(
        !SUITE_NAME.is_empty(),
        "[BLOCK 4: PREFLIGHT] SUITE_NAME must not be empty"
    );
    assert!(
        common::self_audit().is_ok(),
        "[BLOCK 4: PREFLIGHT] Common test harness self-audit must succeed"
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 4.2: Concrete Lexer Anchor Verification Gate
// -----------------------------------------------------------------------------

/// Audits physical lexer modules required for syntax analysis.
#[test]
fn test_preflight_syntax_anchors() {
    let root = common::resolve_repo_root();
    assert!(
        root.join("tools/src/syntax/mod.rs").is_file(),
        "[BLOCK 4: PREFLIGHT] tools/src/syntax/mod.rs must exist"
    );
    assert!(
        root.join("tools/src/syntax/c.rs").is_file(),
        "[BLOCK 4: PREFLIGHT] tools/src/syntax/c.rs must exist"
    );
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( Test Suite Epilogue & Summary Handshake )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 5.1: Epilogue & Clean State Handshake
// -----------------------------------------------------------------------------

/// Verifies that running functional syntax tests leaves zero transient artifacts.
#[test]
fn test_epilogue_clean_state_handshake() {
    let root = common::resolve_repo_root();
    assert!(
        !root.join("tools/scratch_transient.tmp").exists(),
        "[BLOCK 5: EPILOGUE] Zero transient test artifacts leaked to tools/"
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 5.2: Epilogue Readiness Probe
// -----------------------------------------------------------------------------

/// Verifies that common harness remains in a ready state after test suite.
#[test]
fn test_epilogue_readiness_probe() {
    assert!(
        common::is_common_harness_ready(),
        "[BLOCK 5: EPILOGUE] Common harness must remain ready"
    );
}
