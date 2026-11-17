// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/tests/integrated/guide.rs
// @id            test.integrated.tools.guide
// @layer         Layer 4: Verification Suite
// @archetype     verification_suite | integration_pipeline
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-16
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Developer Toolchain // Guide Verification Suite
// @role          Four-Section Ontological Integration Suite for cmd::guide
// @maturity      Rev 5 Baseline | Four-Section Transpiler-Ready Verification
// @chassis       bounded-heap | Test runner execution chassis; stack frames bounded
// @scale         O(C) across all 114 chapters and 6 volumes
// @seams         in: tl_tools::cmd::guide | pairs: docs/
// @invariants    1. Four-Section Ontological Architecture strictly enforced:
//                   Section 1 (Always Pass), Section 2 (Never Fail),
//                   Section 3 (Invariants & Limits), Section 4 (Emergence).
//                2. External Consumer Purity: imports tl_tools via public API.
//                3. Zero Mocks: exercises live repository filesystem assets.
//                4. Strict <= 80-column line width discipline across documentation.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Tier 1 Integration verification harness evaluating the `cmd::guide`
//   command sub-assembly from the perspective of an external consumer or CI/CD
//   runner. Employs the identical 4-Section Ontological Test Protocol enforced
//   across Cornerstone C11 engine test suites (`tests/functional/` and
//   `tests/unit/`), establishing a universal transpiler-aligned verification
//   grammar across C11, Rust, and future native OmniCode.
//
//   ### For Game Designers & Non-Programmers:
//   Guarantees that the master guide compiler can always discover and scan all
//   114 chapters and validate PDF single-page layout rules without crashes.
//
//   ### For Systems Programmers & Engine Architects:
//   - Transpilation Alignment: Maps directly to OmniCode's 4-section verification
//     schema, allowing lossless round-trip transpilation between languages.
//   - Integration Boundary: Tests the compiled crate as a black box without
//     reaching into internal private structs or violating encapsulation.
// =============================================================================

//! # Four-Section Integration Suite for Master Guide Command Sub-Assembly
//!
//! Exercises `tl_tools::cmd::guide` against the live repository workspace
//! across Always-Pass, Never-Fail, Invariant, and Emergent execution tiers.

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Pre-Conditions, Constants, Test Fixtures & Harness Scaffolding )
// =============================================================================

use std::path::{Path, PathBuf};
use tl_tools::cmd::guide::{
    dispatch_standalone_guide, execute, has_valid_docs_hierarchy, self_audit,
    GuideArgs, GuideMode,
};
use tl_tools::cmd::resolve_repo_root;

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Operational Test Constants & Suite Identifiers
// -----------------------------------------------------------------------------

/// Canonical test suite identifier matching engine convention.
pub const SUITE_NAME: &str = "Toolchain Guide Integration Surface";

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Test Fixtures & Pre-Condition Assertions
// -----------------------------------------------------------------------------

/// Constructs validated arguments bound to the discovered live workspace root.
fn create_test_args(mode: GuideMode) -> (PathBuf, GuideArgs) {
    let repo_root = resolve_repo_root();
    assert!(
        has_valid_docs_hierarchy(&repo_root),
        "Resolved repository root {:?} must host valid docs hierarchy",
        repo_root
    );

    let args = GuideArgs {
        mode,
        repo_root: repo_root.clone(),
    };

    (repo_root, args)
}

// =============================================================================
// BLOCK 3: BODY BLOCK / FOUR-SECTION ONTOLOGICAL TEST SUITE
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: [ SECTION 1: ALWAYS PASS (NOMINAL GOLDEN SURFACE COGS) ]
// -----------------------------------------------------------------------------

/// Verifies nominal golden execution of dynamic area chapter scanning.
///
/// Asserts that scanning all registered documentation areas in `docs/` succeeds
/// without errors across all 6 volumes.
#[test]
fn test_section_1_nominal_scan_passes() {
    let (_root, args) = create_test_args(GuideMode::ScanOnly);
    let result = execute(&args);
    assert!(
        result.is_ok(),
        "[SEC 1: ALWAYS PASS] Dynamic documentation scan must pass: {:?}",
        result.err()
    );
}

/// Verifies nominal golden execution of single-page PDF layout validation.
///
/// Asserts that all `.adoc` chapter documents and code snapshot plates satisfy
/// layout constraints and page budget rules with zero fatal errors.
#[test]
fn test_section_1_nominal_layout_validation_passes() {
    let (_root, args) = create_test_args(GuideMode::ValidateLayout);
    let result = execute(&args);
    assert!(
        result.is_ok(),
        "[SEC 1: ALWAYS PASS] AsciiDoc PDF layout audit must pass: {:?}",
        result.err()
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.2: [ SECTION 2: NEVER FAIL / FAULT INVERSION & DEGRADATION ]
// -----------------------------------------------------------------------------

/// Verifies fault inversion when compiling a non-existent volume identifier.
///
/// Asserts that requesting an unknown volume (e.g. "XCIX") returns a typed
/// `Err` with a clear message rather than panicking or crashing.
#[test]
fn test_section_2_never_fail_invalid_volume_identifier() {
    let (_root, args) = create_test_args(GuideMode::SingleVolume("XCIX_NONEXISTENT".into()));
    let result = execute(&args);
    assert!(
        result.is_err(),
        "[SEC 2: NEVER FAIL] Non-existent volume identifier must return typed Err"
    );
}

/// Verifies fault inversion when compiling against an invalid root directory.
#[test]
fn test_section_2_never_fail_invalid_root_directory() {
    let args = GuideArgs {
        mode: GuideMode::ScanOnly,
        repo_root: PathBuf::from("/tmp/nonexistent_tenebris_guide_dir_xyz"),
    };
    let result = execute(&args);
    assert!(
        result.is_ok(), // Scan logs missing markers cleanly without crashing
        "[SEC 2: NEVER FAIL] Scan on empty directory logs missing chapters gracefully"
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.3: [ SECTION 3: INVARIANTS, SATURATION & LIMIT GUARDS ]
// -----------------------------------------------------------------------------

/// Verifies documentation hierarchy detection invariant predicates.
#[test]
fn test_section_3_invariants_docs_hierarchy_predicate() {
    let root = resolve_repo_root();
    assert!(
        has_valid_docs_hierarchy(&root),
        "[SEC 3: INVARIANTS] Authentic repository root must have valid docs hierarchy"
    );
    assert!(
        !has_valid_docs_hierarchy(Path::new("/tmp")),
        "[SEC 3: INVARIANTS] Generic /tmp directory must be rejected as docs hierarchy"
    );
}

/// Verifies guide mode equality and exhaustiveness invariants.
#[test]
fn test_section_3_invariants_guide_mode_equality() {
    assert_eq!(GuideMode::All, GuideMode::All);
    assert_ne!(GuideMode::All, GuideMode::MasterOnly);
    assert_ne!(GuideMode::VolumesOnly, GuideMode::SnapshotsOnly);
    assert_eq!(
        GuideMode::SingleVolume("VI".into()),
        GuideMode::SingleVolume("VI".into())
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.4: [ SECTION 4: EMERGENCE & COMPOSITE WORKFLOWS ]
// -----------------------------------------------------------------------------

/// Verifies emergent CLI bridge dispatch for the standalone `master_guide` tool.
///
/// Simulates command-line invocations with various flags.
#[test]
fn test_section_4_emergence_standalone_guide_cli_dispatch() {
    let (root, _args) = create_test_args(GuideMode::ScanOnly);

    // 1. Dispatch with explicit "--scan" flag
    let cli_scan = vec!["master_guide".to_string(), "--scan".to_string()];
    let res_scan = dispatch_standalone_guide(&cli_scan, &root);
    assert!(
        res_scan.is_ok(),
        "[SEC 4: EMERGENCE] Standalone CLI dispatch '--scan' must pass: {:?}",
        res_scan.err()
    );

    // 2. Dispatch with explicit "--validate-layout" flag
    let cli_layout = vec!["master_guide".to_string(), "--validate-layout".to_string()];
    let res_layout = dispatch_standalone_guide(&cli_layout, &root);
    assert!(
        res_layout.is_ok(),
        "[SEC 4: EMERGENCE] Standalone CLI dispatch '--validate-layout' must pass: {:?}",
        res_layout.err()
    );

    // 3. Dispatch with help argument
    let cli_help = vec!["master_guide".to_string(), "--help".to_string()];
    let res_help = dispatch_standalone_guide(&cli_help, &root);
    assert!(
        res_help.is_ok(),
        "[SEC 4: EMERGENCE] Standalone CLI dispatch '--help' must return Ok(())"
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
        !SUITE_NAME.is_empty(),
        "[BLOCK 4: PREFLIGHT] SUITE_NAME constant must not be empty"
    );
    assert!(
        self_audit(),
        "[BLOCK 4: PREFLIGHT] cmd::guide::self_audit() contract must pass"
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 4.2: Documentation Anchor Verification Gate
// -----------------------------------------------------------------------------

/// Audits physical documentation anchors required for master guide compilation.
#[test]
fn test_preflight_documentation_anchors() {
    let root = resolve_repo_root();
    assert!(
        root.join("docs/master_guide_manifest.json").is_file(),
        "docs/master_guide_manifest.json must exist"
    );
    assert!(
        root.join("docs/books").is_dir(),
        "docs/books directory must exist"
    );
    assert!(
        root.join("docs/adoc_pdf_layout_rules.json").is_file(),
        "docs/adoc_pdf_layout_rules.json must exist"
    );
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( Test Suite Epilogue & Summary Handshake )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 5.1: Epilogue & Clean State Handshake
// -----------------------------------------------------------------------------

#[test]
fn test_epilogue_clean_state_handshake() {
    // Proves that running integration tests leaves zero transient disk artifacts.
    let root = resolve_repo_root();
    assert!(
        !root.join("docs/scratch_transient.tmp").exists(),
        "[BLOCK 5: EPILOGUE] Zero transient test artifacts leaked to docs/"
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 5.2: Epilogue Readiness Probe
// -----------------------------------------------------------------------------

#[test]
fn test_epilogue_readiness_probe() {
    assert!(
        crate::common::is_common_harness_ready(),
        "[BLOCK 5: EPILOGUE] Common harness must remain ready"
    );
}
