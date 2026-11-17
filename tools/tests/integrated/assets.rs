// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/tests/integrated/assets.rs
// @id            test.integrated.tools.assets
// @layer         Layer 4: Verification Suite
// @archetype     verification_suite | integration_pipeline
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-16
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Developer Toolchain // Assets Verification Suite
// @role          Four-Section Ontological Integration Suite for cmd::assets
// @maturity      Rev 5 Baseline | Four-Section Transpiler-Ready Verification
// @chassis       bounded-heap | Test runner execution chassis; stack frames bounded
// @scale         O(A) across presentation assets
// @seams         in: tl_tools::cmd::assets | pairs: game/assets
// @invariants    1. Four-Section Ontological Architecture strictly enforced:
//                   Section 1 (Always Pass), Section 2 (Never Fail),
//                   Section 3 (Invariants & Limits), Section 4 (Emergence).
//                2. External Consumer Purity: imports tl_tools via public API.
//                3. Zero Mocks: exercises live repository filesystem assets.
//                4. Strict <= 80-column line width discipline across documentation.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Tier 1 Integration verification harness evaluating the `cmd::assets`
//   command sub-assembly from the perspective of an external consumer or CI/CD
//   runner. Employs the identical 4-Section Ontological Test Protocol enforced
//   across Cornerstone C11 engine test suites (`tests/functional/` and
//   `tests/unit/`), establishing a universal transpiler-aligned verification
//   grammar across C11, Rust, and future native OmniCode.
// =============================================================================

//! # Four-Section Integration Suite for Asset Pipeline Sub-Assembly
//!
//! Exercises `tl_tools::cmd::assets` across Always-Pass, Never-Fail,
//! Invariant, and Emergent execution tiers.

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Pre-Conditions, Constants, Test Fixtures & Harness Scaffolding )
// =============================================================================

use std::path::{Path, PathBuf};
use tl_tools::cmd::assets::{
    dispatch_standalone_asset_generator, execute, has_valid_assets_directory,
    self_audit, AssetArgs, AssetMode,
};
use tl_tools::cmd::resolve_repo_root;

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Operational Test Constants & Suite Identifiers
// -----------------------------------------------------------------------------

/// Canonical test suite identifier matching engine convention.
pub const SUITE_NAME: &str = "Toolchain Assets Integration Surface";

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Test Fixtures & Pre-Condition Assertions
// -----------------------------------------------------------------------------

/// Constructs validated arguments bound to live repository root.
fn create_test_args(mode: AssetMode) -> (PathBuf, AssetArgs) {
    let repo_root = resolve_repo_root();
    assert!(
        has_valid_assets_directory(&repo_root),
        "Resolved repository root {:?} must host valid game/assets directory",
        repo_root
    );

    let args = AssetArgs {
        mode,
        repo_root: repo_root.clone(),
        verbose: false,
    };

    (repo_root, args)
}

// =============================================================================
// BLOCK 3: BODY BLOCK / FOUR-SECTION ONTOLOGICAL TEST SUITE
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: [ SECTION 1: ALWAYS PASS (NOMINAL GOLDEN SURFACE COGS) ]
// -----------------------------------------------------------------------------

/// Verifies nominal golden execution of asset help menu.
#[test]
fn test_section_1_nominal_help_passes() {
    let (_root, args) = create_test_args(AssetMode::Help);
    let result = execute(&args);
    assert!(
        result.is_ok(),
        "[SEC 1: ALWAYS PASS] Asset help execution must pass: {:?}",
        result.err()
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.2: [ SECTION 2: NEVER FAIL / FAULT INVERSION & DEGRADATION ]
// -----------------------------------------------------------------------------

/// Verifies fault inversion when auditing an empty root directory.
#[test]
fn test_section_2_never_fail_empty_root_audit() {
    let args = AssetArgs {
        mode: AssetMode::Audit,
        repo_root: PathBuf::from("/tmp/nonexistent_tenebris_assets_xyz"),
        verbose: false,
    };
    let result = execute(&args);
    assert!(
        result.is_err(),
        "[SEC 2: NEVER FAIL] Auditing non-existent root must return typed Err"
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.3: [ SECTION 3: INVARIANTS, SATURATION & LIMIT GUARDS ]
// -----------------------------------------------------------------------------

/// Verifies asset directory detection invariant predicates.
#[test]
fn test_section_3_invariants_assets_predicate() {
    let root = resolve_repo_root();
    assert!(
        has_valid_assets_directory(&root),
        "[SEC 3: INVARIANTS] Authentic repository root must have valid assets dir"
    );
    assert!(
        !has_valid_assets_directory(Path::new("/tmp")),
        "[SEC 3: INVARIANTS] Generic /tmp must be rejected as asset directory"
    );
}

/// Verifies asset mode equality and exhaustiveness invariants.
#[test]
fn test_section_3_invariants_asset_mode_equality() {
    assert_eq!(AssetMode::Audit, AssetMode::Audit);
    assert_ne!(AssetMode::Audit, AssetMode::Generate);
    assert_ne!(AssetMode::Generate, AssetMode::Help);
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.4: [ SECTION 4: EMERGENCE & COMPOSITE WORKFLOWS ]
// -----------------------------------------------------------------------------

/// Verifies emergent CLI bridge dispatch for standalone asset_generator.
#[test]
fn test_section_4_emergence_standalone_asset_generator_cli_dispatch() {
    let (root, _args) = create_test_args(AssetMode::Help);

    // Test dispatch with help argument
    let cli_help = vec!["asset_generator".to_string(), "help".to_string()];
    let res_help = dispatch_standalone_asset_generator(&cli_help, &root);
    assert!(
        res_help.is_ok(),
        "[SEC 4: EMERGENCE] Standalone CLI dispatch 'help' must return Ok(())"
    );
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// =============================================================================

#[test]
fn test_preflight_diagnostics_self_audit() {
    assert!(
        !SUITE_NAME.is_empty(),
        "[BLOCK 4: PREFLIGHT] SUITE_NAME constant must not be empty"
    );
    assert!(
        self_audit(),
        "[BLOCK 4: PREFLIGHT] cmd::assets::self_audit() contract must pass"
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 4.2: Asset Anchor Verification Gate
// -----------------------------------------------------------------------------

#[test]
fn test_preflight_asset_catalog_anchors() {
    let root = resolve_repo_root();
    assert!(
        root.join("game/assets").is_dir(),
        "game/assets directory must exist"
    );
    assert!(
        root.join("game/data/presentation").is_dir()
            || root.join("engine/config/viewport").is_dir(),
        "presentation catalogs or engine config must exist"
    );
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================

#[test]
fn test_epilogue_clean_state_handshake() {
    let root = resolve_repo_root();
    assert!(
        !root.join("scratch_leaked_assets.tmp").exists(),
        "[BLOCK 5: EPILOGUE] Zero transient test artifacts leaked"
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
