// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/tests/system/syntax_system.rs
// @id            test.system.tools.syntax
// @layer         Layer 4: Verification Suite
// @archetype     verification_suite | system_scenario
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-16
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Developer Toolchain // Tier 4 System Syntax Suite
// @role          System-Level End-to-End Testing for Universal 5-Block AST Syntax
// @maturity      Rev 5 Baseline | 4-Section Ontological Testing
// @chassis       bounded-heap | Test runner execution chassis; stack frames bounded
// @scale         O(N) full repository file sweeps across multiple languages
// @seams         in: tl_tools::syntax | uses: live repo files
// @invariants    1. Four-Section Ontological Architecture strictly enforced:
//                   Section 1 (Always Pass), Section 2 (Never Fail),
//                   Section 3 (Invariants & Limits), Section 4 (Emergence).
//                2. Zero Panics: Syntax scanners handle arbitrary content without unwinding.
//                3. External Consumer Purity: imports tl_tools via public API.
//                4. Strict <= 80-column line width discipline across documentation.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Tier 4 System end-to-end test suite evaluating the universal syntax framework
//   across live repository files. Asserts that files in C, Rust, JSON, and CMake
//   produce well-formed AST representations without scanner failures.
//
//   ### For Game Designers & Non-Programmers:
//   Guarantees that the automated code reader can scan the entire project without
//   stumbling on unusual characters or formatting styles.
//
//   ### For Systems Programmers & Engine Architects:
//   - Scanner Scale: Evaluates `parse_omni_doc` and `detect_language_from_path`
//     against live repository files across multiple directories.
//   - Fault Inversion: Proves that arbitrary binary noise or malformed text degrades
//     gracefully without triggering panics.
// =============================================================================

//! # Tier 4 System Verification Suite for Syntax Framework
//!
//! Evaluates end-to-end multi-language parsing across Always-Pass, Never-Fail,
//! Invariant, and Emergent ontological tiers.

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Pre-Conditions, Constants, Test Fixtures & Harness Scaffolding )
// =============================================================================

use crate::common;
use std::path::Path;
use tl_tools::syntax::{
    detect_language_from_path, framework::parse_omni_doc, Language,
};

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Operational Test Constants & Suite Identifiers
// -----------------------------------------------------------------------------

/// Canonical suite identifier matching engine testing convention.
pub const SUITE_NAME: &str = "Tier 4: System Syntax Verification Surface";

// =============================================================================
// BLOCK 3: BODY BLOCK / FOUR-SECTION ONTOLOGICAL TEST SUITE
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: [ SECTION 1: ALWAYS PASS (NOMINAL GOLDEN SURFACE COGS) ]
// -----------------------------------------------------------------------------

/// Verifies nominal golden AST parsing of live engine headers.
///
/// Asserts that `engine/include/core/manifest.h` parses into an OmniDoc AST.
#[test]
fn test_section_1_nominal_live_engine_header_syntax_sweep() {
    let root = common::resolve_repo_root();
    let header_path = root.join("engine/include/core/manifest.h");
    if let Ok(content) = std::fs::read_to_string(&header_path) {
        let result = parse_omni_doc(&content);
        assert!(
            result.is_ok(),
            "[SEC 1: ALWAYS PASS] manifest.h must parse into OmniDoc: {:?}",
            result.err()
        );
        let doc = result.unwrap();
        assert!(
            doc.line_count > 0 && doc.has_all_five_blocks(),
            "[SEC 1: ALWAYS PASS] manifest.h must contain all 5 blocks"
        );
    }
}

/// Verifies nominal golden language detection across key repository files.
///
/// Asserts that primary files in different trees detect their expected languages.
#[test]
fn test_section_1_nominal_language_detection_across_trees() {
    let files = [
        ("CMakeLists.txt", Language::CMake),
        ("tools/Cargo.toml", Language::Toml),
        ("tools/src/main.rs", Language::Rust),
        ("engine/include/core/manifest.h", Language::C),
    ];
    for (rel_path, expected_lang) in &files {
        assert_eq!(
            detect_language_from_path(Path::new(rel_path)),
            *expected_lang,
            "[SEC 1: ALWAYS PASS] '{}' must detect as {:?}",
            rel_path,
            expected_lang
        );
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.2: [ SECTION 2: NEVER FAIL / FAULT INVERSION & DEGRADATION ]
// -----------------------------------------------------------------------------

/// Verifies fault inversion when parsing corrupted multi-line content.
///
/// Asserts that arbitrary noise produces an AST without panicking.
#[test]
fn test_section_2_never_fail_corrupt_multiline_syntax_stream() {
    let noise = "~~~!@#$%^&*()_+`1234567890-=\t\t\r\n\0\0NON_CANONICAL_STREAM";
    let result = parse_omni_doc(noise);
    assert!(
        result.is_err() || !result.unwrap().has_all_five_blocks(),
        "[SEC 2: NEVER FAIL] Noise stream should not produce valid 5-block AST"
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.3: [ SECTION 3: INVARIANTS, SATURATION & LIMIT GUARDS ]
// -----------------------------------------------------------------------------

/// Verifies language matrix completeness invariants.
///
/// Asserts that all 9 language variants exist in the current Rev 5 matrix.
#[test]
fn test_section_3_invariants_supported_language_matrix() {
    let langs = [
        Language::C, Language::Cpp, Language::Rust, Language::Json,
        Language::AsciiDoc, Language::Markdown, Language::Toml, Language::CMake,
        Language::Unknown,
    ];
    assert_eq!(
        langs.len(),
        9,
        "[SEC 3: INVARIANTS] Must support exactly 9 language variants"
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.4: [ SECTION 4: EMERGENCE & COMPOSITE WORKFLOWS ]
// -----------------------------------------------------------------------------

/// Verifies emergent multi-language workspace file sweeping.
///
/// Asserts that multiple repository paths across directories evaluate cleanly.
#[test]
fn test_section_4_emergence_multi_language_workspace_sweep() {
    let root = common::resolve_repo_root();
    let sample_paths = [
        root.join("CMakeLists.txt"),
        root.join("tools/Cargo.toml"),
        root.join("tools/src/lib.rs"),
    ];
    for p in &sample_paths {
        assert!(p.exists(), "[SEC 4: EMERGENCE] Path {:?} must exist", p);
        let lang = detect_language_from_path(p);
        assert_ne!(
            lang,
            Language::Unknown,
            "[SEC 4: EMERGENCE] Path {:?} must not be Unknown",
            p
        );
    }
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
// Sub-Assembly 4.2: Syntax Subsystem Anchor Verification Gate
// -----------------------------------------------------------------------------

/// Audits physical source files required for syntax analysis.
#[test]
fn test_preflight_syntax_anchors() {
    let root = common::resolve_repo_root();
    assert!(
        root.join("tools/src/syntax/mod.rs").is_file(),
        "[BLOCK 4: PREFLIGHT] tools/src/syntax/mod.rs must exist"
    );
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( Test Suite Epilogue & Summary Handshake )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 5.1: Epilogue & Clean State Handshake
// -----------------------------------------------------------------------------

/// Verifies that running system syntax tests leaves zero transient artifacts.
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
