// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/tests/integrated/audit.rs
// @id            test.integrated.tools.audit
// @layer         Layer 4: Verification Suite
// @archetype     verification_suite | integration_pipeline
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-16
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Developer Toolchain // Audit Verification Suite
// @role          Four-Section Ontological Integration Suite for cmd::audit
// @maturity      Rev 5 Baseline | Four-Section Transpiler-Ready Verification
// @chassis       bounded-heap | Test runner execution chassis; stack frames bounded
// @scale         O(1) checks across workspace topology anchors
// @seams         in: tl_tools::cmd::audit | pairs: live repo topology
// @invariants    1. Four-Section Ontological Architecture strictly enforced:
//                   Section 1 (Always Pass), Section 2 (Never Fail),
//                   Section 3 (Invariants & Limits), Section 4 (Emergence).
//                2. External Consumer Purity: imports tl_tools via public API.
//                3. Zero Mocks: exercises live workspace topology anchors.
//                4. Strict <= 80-column line width discipline across documentation.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Tier 1 Integration verification harness evaluating the `cmd::audit` command
//   sub-assembly from the perspective of an external consumer or CI/CD runner.
//   Employs the identical 4-Section Ontological Test Protocol enforced across
//   Cornerstone C11 engine test suites (`tests/functional/` and `tests/unit/`),
//   establishing a universal transpiler-aligned verification grammar across
//   C11, Rust, and future native OmniCode.
// =============================================================================

//! # Four-Section Integration Suite for Diagnostic Audit Sub-Assembly
//!
//! Exercises `tl_tools::cmd::audit` across Always-Pass, Never-Fail,
//! Invariant, and Emergent execution tiers.

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Pre-Conditions, Constants, Test Fixtures & Harness Scaffolding )
// =============================================================================

use std::path::{Path, PathBuf};
use tl_tools::cmd::audit::{
    dispatch_standalone_audit, execute, is_audit_subsystem_ready,
    is_valid_audit_workspace, self_audit, AuditArgs, DEFAULT_ARCHIVE_DIR,
    DOCS_SUBDIR, TOOLS_SUBDIR,
};
use tl_tools::cmd::resolve_repo_root;

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Operational Test Constants & Suite Identifiers
// -----------------------------------------------------------------------------

/// Canonical test suite identifier matching engine convention.
pub const SUITE_NAME: &str = "Toolchain Audit Integration Surface";

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Test Fixtures & Pre-Condition Assertions
// -----------------------------------------------------------------------------

/// Constructs validated arguments bound to live repository root.
fn create_test_args() -> (PathBuf, AuditArgs) {
    let repo_root = resolve_repo_root();
    let args = AuditArgs {
        repo_root: repo_root.clone(),
        archive_dir: PathBuf::from(DEFAULT_ARCHIVE_DIR),
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

/// Verifies nominal golden execution of the diagnostic self-audit.
#[test]
fn test_section_1_nominal_audit_execution_passes() {
    let (_root, args) = create_test_args();
    let result = execute(&args);
    assert!(
        result.is_ok(),
        "[SEC 1: ALWAYS PASS] Diagnostic environment audit must succeed: {:?}",
        result.err()
    );
}

/// Verifies that live workspace root passes the workspace predicate.
#[test]
fn test_section_1_nominal_workspace_predicate_passes() {
    let root = resolve_repo_root();
    assert!(
        is_valid_audit_workspace(&root),
        "[SEC 1: ALWAYS PASS] Live workspace root must satisfy is_valid_audit_workspace"
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.2: [ SECTION 2: NEVER FAIL / FAULT INVERSION & DEGRADATION ]
// -----------------------------------------------------------------------------

/// Verifies graceful fault reporting when given an invalid workspace root.
#[test]
fn test_section_2_never_fail_invalid_workspace() {
    let bad_root = Path::new("/definitely/nonexistent/workspace/target");
    let args = AuditArgs {
        repo_root: bad_root.to_path_buf(),
        archive_dir: PathBuf::from(DEFAULT_ARCHIVE_DIR),
        verbose: false,
    };

    let result = execute(&args);
    assert!(
        result.is_err(),
        "[SEC 2: NEVER FAIL] Non-existent workspace must fail gracefully"
    );
    let err = result.unwrap_err();
    assert!(
        err.contains("Invalid workspace root"),
        "[SEC 2: NEVER FAIL] Expected error message indicating invalid root, got: {}",
        err
    );
}

/// Verifies standalone CLI dispatch handles invalid directory arguments.
#[test]
fn test_section_2_never_fail_bad_archive_in_dispatch() {
    let root = resolve_repo_root();
    let cli_args = vec![
        "--archive-dir".to_string(),
        "/nonexistent/archive/dir/for/test".to_string(),
    ];
    let result = dispatch_standalone_audit(&cli_args, &root);
    assert!(
        result.is_err(),
        "[SEC 2: NEVER FAIL] Missing archive directory should cause audit failure"
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.3: [ SECTION 3: INVARIANTS, SATURATION & LIMIT GUARDS ]
// -----------------------------------------------------------------------------

/// Verifies operational string constants are non-empty and well-formed.
#[test]
fn test_section_3_invariants_operational_constants() {
    assert!(
        !DEFAULT_ARCHIVE_DIR.is_empty(),
        "[SEC 3: INVARIANT] DEFAULT_ARCHIVE_DIR must be non-empty"
    );
    assert_eq!(
        TOOLS_SUBDIR, "tools",
        "[SEC 3: INVARIANT] TOOLS_SUBDIR must be 'tools'"
    );
    assert_eq!(
        DOCS_SUBDIR, "docs",
        "[SEC 3: INVARIANT] DOCS_SUBDIR must be 'docs'"
    );
}

/// Verifies subsystem readiness predicate and self-audit consistency.
#[test]
fn test_section_3_invariants_readiness_predicate() {
    assert!(
        is_audit_subsystem_ready(),
        "[SEC 3: INVARIANT] is_audit_subsystem_ready must return true"
    );
    assert!(
        self_audit().is_ok(),
        "[SEC 3: INVARIANT] self_audit must return Ok"
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.4: [ SECTION 4: EMERGENCE & COMPOSITE WORKFLOWS ]
// -----------------------------------------------------------------------------

/// Verifies end-to-end standalone CLI dispatch with verbose telemetry.
#[test]
fn test_section_4_emergence_verbose_standalone_cli_dispatch() {
    let root = resolve_repo_root();
    let cli_args = vec![
        "--verbose".to_string(),
        "--archive-dir".to_string(),
        DEFAULT_ARCHIVE_DIR.to_string(),
    ];
    let result = dispatch_standalone_audit(&cli_args, &root);
    assert!(
        result.is_ok(),
        "[SEC 4: EMERGENCE] Standalone verbose audit dispatch must succeed: {:?}",
        result.err()
    );
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// ( Preflight Diagnostics & Verification Tripwires )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 4.1: Preflight Diagnostic Gates
// -----------------------------------------------------------------------------

#[test]
fn test_preflight_diagnostics_self_audit() {
    assert!(
        !SUITE_NAME.is_empty(),
        "[BLOCK 4: PREFLIGHT] SUITE_NAME constant must not be empty"
    );
    assert!(
        self_audit().is_ok(),
        "[BLOCK 4: PREFLIGHT] Diagnostic self-audit failed"
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 4.2: Workspace Anchor Verification Gate
// -----------------------------------------------------------------------------

#[test]
fn test_preflight_workspace_anchors() {
    let root = resolve_repo_root();
    assert!(
        root.join("tools/Cargo.toml").is_file(),
        "[BLOCK 4: PREFLIGHT] Anchor missing: tools/Cargo.toml"
    );
    assert!(
        root.join("docs").is_dir(),
        "[BLOCK 4: PREFLIGHT] Anchor missing: docs/"
    );
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( Summary Handshake & Clean Epilogue )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 5.1: Epilogue & Clean State Handshake
// -----------------------------------------------------------------------------

#[test]
fn test_epilogue_clean_state_handshake() {
    assert!(
        is_audit_subsystem_ready(),
        "[BLOCK 5: EPILOGUE] Audit subsystem must report operational readiness"
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
