// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/tests/functional/guide.rs
// @id            test.functional.tools.guide
// @layer         Layer 4: Verification Suite
// @archetype     verification_suite | functional_surface
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-16
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Developer Toolchain // Tier 1 Functional Guide Suite
// @role          Functional Surface Testing for Master Guide Compilation Modes
// @maturity      Rev 5 Baseline | 4-Section Ontological Testing
// @chassis       bounded-heap | Test runner execution chassis; stack frames bounded
// @scale         O(1) execution over guide mode options and argument envelopes
// @seams         in: tl_tools::cmd::guide | uses: common::resolve_repo_root
// @invariants    1. Four-Section Ontological Architecture strictly enforced:
//                   Section 1 (Always Pass), Section 2 (Never Fail),
//                   Section 3 (Invariants & Limits), Section 4 (Emergence).
//                2. Zero Panics: Documentation hierarchy probe degrades safely.
//                3. External Consumer Purity: imports tl_tools via public API.
//                4. Strict <= 80-column line width discipline across documentation.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Tier 1 Functional surface test suite evaluating the `tl_tools::cmd::guide`
//   command surface. Exercises mode dispatch configurations, argument envelope
//   construction, and physical documentation hierarchy discovery gates without
//   triggering expensive full-volume PDF compilations.
//
//   ### For Game Designers & Non-Programmers:
//   Ensures that every mode switch of the Master Guide compiler (scan-only,
//   layout validation, single-volume, snapshots) can be configured without
//   unexpected errors or faulty directory detection.
//
//   ### For Systems Programmers & Engine Architects:
//   - Hierarchy Boundary: Verifies `has_valid_docs_hierarchy` invariant predicates
//     across authentic repository roots and arbitrary non-doc paths.
//   - Mode Orthogonality: Proves that all `GuideMode` variants maintain strict
//     algebraic inequality and clean envelope encapsulation.
// =============================================================================

//! # Tier 1 Functional Surface Verification Suite for Master Guide Command
//!
//! Evaluates `tl_tools::cmd::guide` mode surfaces across Always-Pass, Never-Fail,
//! Invariant, and Emergent ontological tiers.

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Pre-Conditions, Constants, Test Fixtures & Harness Scaffolding )
// =============================================================================

use crate::common;
use std::path::{Path, PathBuf};
use tl_tools::cmd::guide::{
    has_valid_docs_hierarchy, self_audit, GuideArgs, GuideMode,
};
use tl_tools::cmd::resolve_repo_root;

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Operational Test Constants & Suite Identifiers
// -----------------------------------------------------------------------------

/// Canonical suite identifier matching engine testing convention.
pub const SUITE_NAME: &str = "Tier 1: Functional Guide Surface";

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Test Fixtures & Pre-Condition Assertions
// -----------------------------------------------------------------------------

/// Constructs validated arguments bound to discovered live workspace root.
fn create_test_guide_args(mode: GuideMode) -> (PathBuf, GuideArgs) {
    let root = resolve_repo_root();
    assert!(
        has_valid_docs_hierarchy(&root),
        "Repository root {:?} must host valid docs hierarchy",
        root
    );
    let args = GuideArgs {
        mode,
        repo_root: root.clone(),
    };
    (root, args)
}

// =============================================================================
// BLOCK 3: BODY BLOCK / FOUR-SECTION ONTOLOGICAL TEST SUITE
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: [ SECTION 1: ALWAYS PASS (NOMINAL GOLDEN SURFACE COGS) ]
// -----------------------------------------------------------------------------

/// Verifies nominal golden construction of all primary guide mode variants.
///
/// Asserts that `GuideArgs` properly encapsulates distinct operational modes.
#[test]
fn test_section_1_nominal_guide_mode_enum_variants() {
    let (_root, args_all) = create_test_guide_args(GuideMode::All);
    assert_eq!(args_all.mode, GuideMode::All);

    let (_root, args_scan) = create_test_guide_args(GuideMode::ScanOnly);
    assert_eq!(args_scan.mode, GuideMode::ScanOnly);

    let (_root, args_val) = create_test_guide_args(GuideMode::ValidateLayout);
    assert_eq!(args_val.mode, GuideMode::ValidateLayout);

    let (_root, args_snap) = create_test_guide_args(GuideMode::SnapshotsOnly);
    assert_eq!(args_snap.mode, GuideMode::SnapshotsOnly);

    let (_root, args_vol) = create_test_guide_args(GuideMode::VolumesOnly);
    assert_eq!(args_vol.mode, GuideMode::VolumesOnly);

    let (_root, args_master) = create_test_guide_args(GuideMode::MasterOnly);
    assert_eq!(args_master.mode, GuideMode::MasterOnly);

    let (_root, args_single) = create_test_guide_args(GuideMode::SingleVolume("I".into()));
    assert_eq!(args_single.mode, GuideMode::SingleVolume("I".into()));
}

/// Verifies nominal golden execution of the documentation hierarchy probe.
///
/// Asserts that the live workspace contains a valid documentation tree.
#[test]
fn test_section_1_nominal_docs_hierarchy_probe() {
    let root = resolve_repo_root();
    assert!(
        has_valid_docs_hierarchy(&root),
        "[SEC 1: ALWAYS PASS] Live repository root must host valid docs hierarchy"
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.2: [ SECTION 2: NEVER FAIL / FAULT INVERSION & DEGRADATION ]
// -----------------------------------------------------------------------------

/// Verifies fault inversion when probing an invalid documentation directory.
///
/// Asserts that missing or arbitrary directories safely return false.
#[test]
fn test_section_2_never_fail_invalid_docs_directory() {
    let fake_dir = Path::new("/tmp/nonexistent_docs_directory_xyz_123");
    assert!(
        !has_valid_docs_hierarchy(fake_dir),
        "[SEC 2: NEVER FAIL] Nonexistent directory must return false cleanly"
    );
}

/// Verifies fault inversion when probing an empty relative path.
///
/// Asserts that empty paths do not panic during hierarchy probing.
#[test]
fn test_section_2_never_fail_empty_path() {
    let empty_path = Path::new("");
    assert!(
        !has_valid_docs_hierarchy(empty_path),
        "[SEC 2: NEVER FAIL] Empty path must return false without panicking"
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.3: [ SECTION 3: INVARIANTS, SATURATION & LIMIT GUARDS ]
// -----------------------------------------------------------------------------

/// Verifies guide mode inequality and exhaustiveness invariants.
///
/// Asserts that different modes never compare equal.
#[test]
fn test_section_3_invariants_guide_mode_inequality() {
    assert_ne!(GuideMode::All, GuideMode::MasterOnly);
    assert_ne!(GuideMode::VolumesOnly, GuideMode::SnapshotsOnly);
    assert_ne!(GuideMode::ScanOnly, GuideMode::ValidateLayout);
    assert_ne!(
        GuideMode::SingleVolume("I".into()),
        GuideMode::SingleVolume("II".into())
    );
}

/// Verifies guide arguments clone equality invariants.
///
/// Asserts that cloning `GuideArgs` preserves identical mode and path state.
#[test]
fn test_section_3_invariants_guide_args_clone_equality() {
    let (_root, args) = create_test_guide_args(GuideMode::ScanOnly);
    let cloned = args.clone();
    assert_eq!(args.mode, cloned.mode);
    assert_eq!(args.repo_root, cloned.repo_root);
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.4: [ SECTION 4: EMERGENCE & COMPOSITE WORKFLOWS ]
// -----------------------------------------------------------------------------

/// Verifies emergent multi-mode configuration switching.
///
/// Simulates switching between fast scan and detailed layout validation modes.
#[test]
fn test_section_4_emergence_multi_mode_dispatch_surface() {
    let root = resolve_repo_root();
    let mut args = GuideArgs {
        mode: GuideMode::ScanOnly,
        repo_root: root.clone(),
    };
    assert_eq!(args.mode, GuideMode::ScanOnly);

    // Switch mode to ValidateLayout dynamically
    args.mode = GuideMode::ValidateLayout;
    assert_eq!(args.mode, GuideMode::ValidateLayout);

    // Switch mode to SingleVolume dynamically
    args.mode = GuideMode::SingleVolume("VI".into());
    assert_eq!(args.mode, GuideMode::SingleVolume("VI".into()));
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// ( Preflight Diagnostics & Deep Verification Audits )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 4.1: Preflight Diagnostic Gates
// -----------------------------------------------------------------------------

/// Diagnostic self-audit verifying suite constant integrity and command audit.
#[test]
fn test_preflight_diagnostics_self_audit() {
    assert!(
        !SUITE_NAME.is_empty(),
        "[BLOCK 4: PREFLIGHT] SUITE_NAME must not be empty"
    );
    assert!(
        self_audit(),
        "[BLOCK 4: PREFLIGHT] cmd::guide::self_audit() contract must pass"
    );
    assert!(
        common::self_audit().is_ok(),
        "[BLOCK 4: PREFLIGHT] Common test harness self-audit must succeed"
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
        "[BLOCK 4: PREFLIGHT] docs/master_guide_manifest.json must exist"
    );
    assert!(
        root.join("docs/adoc_pdf_layout_rules.json").is_file(),
        "[BLOCK 4: PREFLIGHT] docs/adoc_pdf_layout_rules.json must exist"
    );
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( Test Suite Epilogue & Summary Handshake )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 5.1: Epilogue & Clean State Handshake
// -----------------------------------------------------------------------------

/// Verifies that running functional guide tests leaves zero transient artifacts.
#[test]
fn test_epilogue_clean_state_handshake() {
    let root = resolve_repo_root();
    assert!(
        !root.join("docs/scratch_transient.tmp").exists(),
        "[BLOCK 5: EPILOGUE] Zero transient test artifacts leaked to docs/"
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
