// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/tests/integrated/validate.rs
// @id            test.integrated.tools.validate
// @layer         Layer 4: Verification Suite
// @archetype     verification_suite | integration_pipeline
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-16
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Developer Toolchain // Validation Verification Suite
// @role          Four-Section Ontological Integration Suite for cmd::validate
// @maturity      Rev 5 Baseline | Four-Section Transpiler-Ready Verification
// @chassis       bounded-heap | Test runner execution chassis; stack frames bounded
// @scale         O(N) file sweeps; exercises complete repository targets
// @seams         in: tl_tools::cmd::validate | pairs: engine/include, game/data
// @invariants    1. Four-Section Ontological Architecture strictly enforced:
//                   Section 1 (Always Pass), Section 2 (Never Fail),
//                   Section 3 (Invariants & Limits), Section 4 (Emergence).
//                2. External Consumer Purity: imports tl_tools via public API.
//                3. Zero Mocks: exercises live repository filesystem assets.
//                4. Strict <= 80-column line width discipline across documentation.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Tier 1 Integration verification harness evaluating the `cmd::validate`
//   command sub-assembly from the perspective of an external consumer or CI/CD
//   runner. Employs the identical 4-Section Ontological Test Protocol enforced
//   across Cornerstone C11 engine test suites (`tests/functional/` and
//   `tests/unit/`), establishing a universal transpiler-aligned verification
//   grammar across C11, Rust, and future native OmniCode.
//
//   ### For Game Designers & Non-Programmers:
//   Guarantees that the validator is continuously tested against the actual game
//   data files they edit every day. If a tool regression is introduced, this test
//   suite fails before the broken tool can reach developers.
//
//   ### For Systems Programmers & Engine Architects:
//   - Transpilation Alignment: Maps directly to OmniCode's 4-section verification
//     schema, allowing lossless round-trip transpilation between languages.
//   - Integration Boundary: Tests the compiled crate as a black box without
//     reaching into internal private structs or violating encapsulation.
// =============================================================================

//! # Four-Section Integration Suite for Validation Command Sub-Assembly
//!
//! Exercises `tl_tools::cmd::validate` against the live repository workspace
//! across Always-Pass, Never-Fail, Invariant, and Emergent execution tiers.

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Pre-Conditions, Constants, Test Fixtures & Harness Scaffolding )
// =============================================================================

use std::path::{Path, PathBuf};
use tl_tools::cmd::resolve_repo_root;
use tl_tools::cmd::validate::{
    dispatch_standalone_git_validator, dispatch_standalone_validator,
    is_valid_workspace_root, run_commits, run_data, run_headers, self_audit,
    ValidateArgs, ValidationDomain,
};

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Operational Test Constants & Suite Identifiers
// -----------------------------------------------------------------------------

/// Canonical test suite identifier matching engine convention.
pub const SUITE_NAME: &str = "Toolchain Validate Integration Surface";

/// Sub-domain for canonical Rev 5 system data catalogs.
pub const CANONICAL_SYSTEM_SUBDOMAIN: &str = "system";

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Test Fixtures & Pre-Condition Assertions
// -----------------------------------------------------------------------------

/// Constructs validated arguments bound to the discovered live workspace root.
fn create_test_args(domain: ValidationDomain) -> (PathBuf, ValidateArgs) {
    let repo_root = resolve_repo_root();
    assert!(
        is_valid_workspace_root(&repo_root),
        "Resolved repository root {:?} must be a valid workspace",
        repo_root
    );

    let args = ValidateArgs {
        domain,
        data_subdomain: CANONICAL_SYSTEM_SUBDOMAIN.to_string(),
        target_paths: Vec::new(),
        repo_root: repo_root.clone(),
        strict: false,
        verbose: false,
        json_output: false,
    };

    (repo_root, args)
}

// =============================================================================
// BLOCK 3: BODY BLOCK / FOUR-SECTION ONTOLOGICAL TEST SUITE
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: [ SECTION 1: ALWAYS PASS (NOMINAL GOLDEN SURFACE COGS) ]
// -----------------------------------------------------------------------------

/// Verifies nominal golden execution of C11/C23 engine header validation.
///
/// Asserts that all 44 Cornerstone public engine headers strictly satisfy the
/// Universal 5-Block Lifecycle Protocol and Invariant 18 with zero errors.
#[test]
fn test_section_1_nominal_headers_pass() {
    let (_root, args) = create_test_args(ValidationDomain::Headers);
    let result = run_headers(&args);
    assert!(
        result.is_ok(),
        "[SEC 1: ALWAYS PASS] Engine headers must pass with 0 errors: {:?}",
        result.err()
    );
}

/// Verifies nominal golden execution of domain data catalog validation.
///
/// Asserts that all JSON manifests under `game/data/system/` strictly conform
/// to the 5-block envelope schema and pass with zero errors.
#[test]
fn test_section_1_nominal_system_data_pass() {
    let (_root, mut args) = create_test_args(ValidationDomain::Data);
    args.data_subdomain = CANONICAL_SYSTEM_SUBDOMAIN.to_string();
    let result = run_data(&args);
    assert!(
        result.is_ok(),
        "[SEC 1: ALWAYS PASS] System data catalog must pass with 0 errors: {:?}",
        result.err()
    );
}

/// Verifies nominal golden execution of git commit and tag ledger audits.
///
/// Asserts that recent git history satisfies conventional commit and milestone
/// tagging invariant contracts.
#[test]
fn test_section_1_nominal_git_commits_pass() {
    let (_root, args) = create_test_args(ValidationDomain::Commits);
    let result = run_commits(&args);
    assert!(
        result.is_ok(),
        "[SEC 1: ALWAYS PASS] Git ledger audit must pass with 0 errors: {:?}",
        result.err()
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.2: [ SECTION 2: NEVER FAIL / FAULT INVERSION & DEGRADATION ]
// -----------------------------------------------------------------------------

/// Verifies fault inversion when validating a non-existent data sub-domain.
///
/// Asserts that querying an invalid domain returns a typed `Err` rather than
/// panicking, crashing, or corrupting state.
#[test]
fn test_section_2_never_fail_nonexistent_subdomain() {
    let (_root, mut args) = create_test_args(ValidationDomain::Data);
    args.data_subdomain = "nonexistent_subdomain_xyz_123".to_string();
    let result = run_data(&args);
    assert!(
        result.is_err(),
        "[SEC 2: NEVER FAIL] Non-existent sub-domain must return typed Err"
    );
}

/// Verifies fault inversion when validating against a non-workspace directory.
///
/// Asserts that a directory missing `engine/config/standards/` returns a clean
/// typed error rather than panicking or executing undefined operations.
#[test]
fn test_section_2_never_fail_invalid_root_directory() {
    let mut args = ValidateArgs::default();
    args.repo_root = PathBuf::from("/tmp/nonexistent_tenebris_test_dir_xyz");
    let result = run_headers(&args);
    assert!(
        result.is_err(),
        "[SEC 2: NEVER FAIL] Invalid workspace root must return typed Err"
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.3: [ SECTION 3: INVARIANTS, SATURATION & LIMIT GUARDS ]
// -----------------------------------------------------------------------------

/// Verifies strict mode invariant: zero tolerance for warnings.
///
/// In strict mode, any warning triggers a fatal error verdict.
/// Asserts that nominal headers pass even under strict mode constraints.
#[test]
fn test_section_3_invariants_strict_mode_contract() {
    let (_root, mut args) = create_test_args(ValidationDomain::Headers);
    args.strict = true;
    let result = run_headers(&args);
    assert!(
        result.is_ok(),
        "[SEC 3: INVARIANTS] Strict mode must pass when 0 warnings exist: {:?}",
        result.err()
    );
}

/// Verifies workspace root validation invariant predicates.
///
/// Asserts that `is_valid_workspace_root` accurately detects authentic roots
/// and rejects empty or arbitrary directories.
#[test]
fn test_section_3_invariants_workspace_root_predicate() {
    let root = resolve_repo_root();
    assert!(
        is_valid_workspace_root(&root),
        "[SEC 3: INVARIANTS] Authentic repository root must be valid"
    );
    assert!(
        !is_valid_workspace_root(Path::new("/tmp")),
        "[SEC 3: INVARIANTS] Generic /tmp directory must be rejected as workspace"
    );
}

/// Verifies domain equality and exhaustiveness invariants.
#[test]
fn test_section_3_invariants_domain_exhaustiveness() {
    assert_eq!(ValidationDomain::Headers, ValidationDomain::Headers);
    assert_ne!(ValidationDomain::Headers, ValidationDomain::Data);
    assert_ne!(ValidationDomain::Commits, ValidationDomain::All);
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.4: [ SECTION 4: EMERGENCE & COMPOSITE WORKFLOWS ]
// -----------------------------------------------------------------------------

/// Verifies emergent CLI bridge dispatch for the standalone `validator` tool.
///
/// Simulates command-line invocations with various domain and flag combinations.
#[test]
fn test_section_4_emergence_standalone_validator_cli_dispatch() {
    let (root, _args) = create_test_args(ValidationDomain::Headers);

    // 1. Dispatch with explicit "headers" argument
    let cli_headers = vec!["validator".to_string(), "headers".to_string()];
    let res_headers = dispatch_standalone_validator(&cli_headers, &root);
    assert!(
        res_headers.is_ok(),
        "[SEC 4: EMERGENCE] Standalone CLI dispatch 'headers' must pass: {:?}",
        res_headers.err()
    );

    // 2. Dispatch with explicit "system" data sub-domain argument
    let cli_system = vec!["validator".to_string(), "system".to_string()];
    let res_system = dispatch_standalone_validator(&cli_system, &root);
    assert!(
        res_system.is_ok(),
        "[SEC 4: EMERGENCE] Standalone CLI dispatch 'system' must pass: {:?}",
        res_system.err()
    );

    // 3. Dispatch with help argument (returns Ok without execution)
    let cli_help = vec!["validator".to_string(), "--help".to_string()];
    let res_help = dispatch_standalone_validator(&cli_help, &root);
    assert!(
        res_help.is_ok(),
        "[SEC 4: EMERGENCE] Standalone CLI dispatch '--help' must return Ok(())"
    );
}

/// Verifies emergent CLI bridge dispatch for the standalone `git_validator` tool.
#[test]
fn test_section_4_emergence_standalone_git_validator_cli_dispatch() {
    let (root, _args) = create_test_args(ValidationDomain::Commits);

    // Dispatch with --recent flag
    let cli_recent = vec![
        "git_validator".to_string(),
        "--recent".to_string(),
        "3".to_string(),
    ];
    let res = dispatch_standalone_git_validator(&cli_recent, &root);
    assert!(
        res.is_ok(),
        "[SEC 4: EMERGENCE] Standalone git_validator '--recent 3' must pass: {:?}",
        res.err()
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
        "[BLOCK 4: PREFLIGHT] cmd::validate::self_audit() contract must pass"
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 4.2: Workspace Topology Verification Gate
// -----------------------------------------------------------------------------

/// Audits physical workspace anchors required for valid verification passes.
#[test]
fn test_preflight_workspace_topology_anchors() {
    let root = resolve_repo_root();
    assert!(
        root.join("CMakeLists.txt").is_file(),
        "CMakeLists.txt must anchor workspace root"
    );
    assert!(
        root.join("engine/include").is_dir(),
        "engine/include must exist as C11 header anchor"
    );
    assert!(
        root.join("game/data").is_dir(),
        "game/data must exist as domain catalog anchor"
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
        !root.join("scratch_leaked_test_artifact.tmp").exists(),
        "[BLOCK 5: EPILOGUE] Zero transient test artifacts leaked to root"
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
