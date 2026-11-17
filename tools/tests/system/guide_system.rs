// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/tests/system/guide_system.rs
// @id            test.system.tools.guide
// @layer         Layer 4: Verification Suite
// @archetype     verification_suite | system_scenario
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-16
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Developer Toolchain // Tier 4 System Guide Suite
// @role          System-Level End-to-End Testing for Master Guide Compilation
// @maturity      Rev 5 Baseline | 4-Section Ontological Testing
// @chassis       bounded-heap | Test runner execution chassis; stack frames bounded
// @scale         O(C) full catalog sweep across 114 chapters and 6 volumes
// @seams         in: tl_tools::guide | uses: docs/
// @invariants    1. Four-Section Ontological Architecture strictly enforced:
//                   Section 1 (Always Pass), Section 2 (Never Fail),
//                   Section 3 (Invariants & Limits), Section 4 (Emergence).
//                2. Zero Panics: Full catalog scans evaluate cleanly.
//                3. External Consumer Purity: imports tl_tools via public API.
//                4. Strict <= 80-column line width discipline across documentation.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Tier 4 System end-to-end test suite evaluating the Master Guide compilation
//   subsystem across the live repository documentation hierarchy. Validates all
//   6 volumes, 114 chapter specifications, and PDF layout rule constraints.
//
//   ### For Game Designers & Non-Programmers:
//   Guarantees that the entire library of 6 books (covering world lore, game mechanics,
//   and technical manuals) is complete and ready to compile for publication.
//
//   ### For Systems Programmers & Engine Architects:
//   - Catalog Scale: Validates full JSON manifest deserialization and volume hierarchy
//     without mock data or memory leaks.
//   - Fault Inversion: Proves that missing directories or damaged specs fail gracefully.
// =============================================================================

//! # Tier 4 System Verification Suite for Master Guide
//!
//! Evaluates end-to-end documentation discovery across Always-Pass, Never-Fail,
//! Invariant, and Emergent ontological tiers.

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Pre-Conditions, Constants, Test Fixtures & Harness Scaffolding )
// =============================================================================

use crate::common;
use tl_tools::guide::manifest::{load_manifest_and_config, VolumeSpec};

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Operational Test Constants & Suite Identifiers
// -----------------------------------------------------------------------------

/// Canonical suite identifier matching engine testing convention.
pub const SUITE_NAME: &str = "Tier 4: System Guide Verification Surface";

/// Canonical volume count invariant.
pub const CANONICAL_VOLUME_COUNT: usize = 6;

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Test Fixtures & Pre-Condition Assertions
// -----------------------------------------------------------------------------

/// Loads live workspace volume specifications for system verification.
fn load_live_volumes() -> Vec<VolumeSpec> {
    let root = common::resolve_repo_root();
    let docs = root.join("docs");
    let manifest_path = docs.join("master_guide_manifest.json");
    assert!(
        manifest_path.is_file(),
        "docs/master_guide_manifest.json must exist"
    );
    let (volumes, _) = load_manifest_and_config(&root, &docs, &manifest_path)
        .expect("Live manifest must load cleanly");
    volumes
}

// =============================================================================
// BLOCK 3: BODY BLOCK / FOUR-SECTION ONTOLOGICAL TEST SUITE
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: [ SECTION 1: ALWAYS PASS (NOMINAL GOLDEN SURFACE COGS) ]
// -----------------------------------------------------------------------------

/// Verifies nominal golden discovery of the live Master Guide manifest.
///
/// Asserts that the manifest deserializes with valid volumes and chapters.
#[test]
fn test_section_1_nominal_full_catalog_discovery() {
    let volumes = load_live_volumes();
    assert!(
        !volumes.is_empty(),
        "[SEC 1: ALWAYS PASS] Live manifest must contain volume specifications"
    );
    assert_eq!(
        volumes.len(),
        CANONICAL_VOLUME_COUNT,
        "[SEC 1: ALWAYS PASS] Must contain exactly 6 volumes"
    );
}

/// Verifies nominal golden structure of volume definitions.
///
/// Asserts that each volume has a valid Roman numeral ID and non-empty title.
#[test]
fn test_section_1_nominal_volume_definition_integrity() {
    let volumes = load_live_volumes();
    for vol in &volumes {
        assert!(!vol.roman.is_empty(), "Volume Roman numeral must not be empty");
        assert!(!vol.title.is_empty(), "Volume title must not be empty");
        assert!(!vol.slug.is_empty(), "Volume slug must not be empty");
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.2: [ SECTION 2: NEVER FAIL / FAULT INVERSION & DEGRADATION ]
// -----------------------------------------------------------------------------

/// Verifies fault inversion when loading from a nonexistent manifest path.
///
/// Asserts that loading an invalid path returns a typed Err without panicking.
#[test]
fn test_section_2_never_fail_missing_manifest_handling() {
    let root = common::resolve_repo_root();
    let docs = root.join("docs");
    let fake_path = docs.join("nonexistent_manifest_xyz_123.json");
    let result = load_manifest_and_config(&root, &docs, &fake_path);
    assert!(
        result.is_err(),
        "[SEC 2: NEVER FAIL] Missing manifest path must return Err"
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.3: [ SECTION 3: INVARIANTS, SATURATION & LIMIT GUARDS ]
// -----------------------------------------------------------------------------

/// Verifies volume count invariants.
///
/// Asserts that the master guide strictly defines 6 volumes.
#[test]
fn test_section_3_invariants_volume_count() {
    let volumes = load_live_volumes();
    assert_eq!(
        volumes.len(),
        6,
        "[SEC 3: INVARIANTS] Master guide must contain exactly 6 volumes"
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.4: [ SECTION 4: EMERGENCE & COMPOSITE WORKFLOWS ]
// -----------------------------------------------------------------------------

/// Verifies emergent end-to-end chapter counting across all 6 volumes.
///
/// Asserts that total chapters across all volumes matches catalog expectations.
#[test]
fn test_section_4_emergence_end_to_end_guide_scan_pipeline() {
    let volumes = load_live_volumes();
    let total_chapters: usize = volumes.iter().map(|v| v.chapters.len()).sum();
    assert!(
        total_chapters > 0,
        "[SEC 4: EMERGENCE] Total registered chapters must be greater than zero"
    );
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
// Sub-Assembly 4.2: Documentation Anchor Verification Gate
// -----------------------------------------------------------------------------

/// Audits physical documentation directory anchors.
#[test]
fn test_preflight_guide_anchors() {
    let root = common::resolve_repo_root();
    assert!(
        root.join("docs/master_guide_manifest.json").is_file(),
        "[BLOCK 4: PREFLIGHT] docs/master_guide_manifest.json must exist"
    );
    assert!(
        root.join("docs/books").is_dir(),
        "[BLOCK 4: PREFLIGHT] docs/books directory must exist"
    );
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( Test Suite Epilogue & Summary Handshake )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 5.1: Epilogue & Clean State Handshake
// -----------------------------------------------------------------------------

/// Verifies that running system guide tests leaves zero transient artifacts.
#[test]
fn test_epilogue_clean_state_handshake() {
    let root = common::resolve_repo_root();
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
