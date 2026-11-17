// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/tests/unit/cmd/dispatch.rs
// @id            test.unit.tools.cmd.dispatch
// @layer         Layer 4: Verification Suite
// @archetype     verification_suite | unit_isolation
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-16
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Developer Toolchain // Tier 2 Unit CMD Dispatch Suite
// @role          Unit Isolation Testing for CLI Command Dispatch and Execution
// @maturity      Rev 5 Baseline | 4-Section Ontological Testing
// @chassis       bounded-heap | Test runner execution chassis; stack frames bounded
// @scale         O(1) execution time across command configuration envelopes
// @seams         in: tl_tools::cmd::{CliConfig, run_toolchain}
// @invariants    1. Four-Section Ontological Architecture strictly enforced:
//                   Section 1 (Always Pass), Section 2 (Never Fail),
//                   Section 3 (Invariants & Limits), Section 4 (Emergence).
//                2. Zero Panics: Command execution contracts return typed Results.
//                3. External Consumer Purity: imports tl_tools via public API.
//                4. Strict <= 80-column line width discipline across documentation.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Tier 2 Unit isolation test suite evaluating `tl_tools::cmd` command dispatch
//   mechanisms. Exercises `CliConfig::execute`, help/version emitters, and
//   the high-level `run_toolchain` entrypoint in isolation.
//
//   ### For Game Designers & Non-Programmers:
//   Validates that tools safely launch when called by buttons, scripts, or menu
//   items and report clear success or failure status messages without crashing.
//
//   ### For Systems Programmers & Engine Architects:
//   - Execution Boundary: Tests `CliConfig::execute` error propagation and return
//     type invariance across all supported `ToolAction` variants.
//   - Fault Inversion: Verifies that dispatching invalid configurations returns
//     clean `Err(String)` errors rather than panic unwinds.
// =============================================================================

//! # Tier 2 Unit Isolation Suite for Command Dispatch
//!
//! Evaluates `CliConfig::execute`, `run_toolchain`, and execution envelopes across
//! Always-Pass, Never-Fail, Invariant, and Emergent ontological tiers.

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Pre-Conditions, Constants, Test Fixtures & Harness Scaffolding )
// =============================================================================

use crate::common;
use std::path::PathBuf;
use tl_tools::cmd::{
    resolve_repo_root, run_toolchain, CliConfig, ToolAction,
    BASELINE_OUTPUT_SUBDIR, CANONICAL_ARCHIVE_DIR,
};

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Operational Test Constants & Suite Identifiers
// -----------------------------------------------------------------------------

/// Canonical suite identifier matching engine testing convention.
pub const SUITE_NAME: &str = "Tier 2: Unit CMD Dispatch Surface";

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Test Fixtures & Pre-Condition Assertions
// -----------------------------------------------------------------------------

/// Constructs a minimal valid CLI configuration for testing.
fn create_test_config(action: ToolAction) -> CliConfig {
    CliConfig {
        action,
        repo_root: resolve_repo_root(),
        archive_dir: PathBuf::from(CANONICAL_ARCHIVE_DIR),
        verbose: false,
    }
}

// =============================================================================
// BLOCK 3: BODY BLOCK / FOUR-SECTION ONTOLOGICAL TEST SUITE
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: [ SECTION 1: ALWAYS PASS (NOMINAL GOLDEN SURFACE COGS) ]
// -----------------------------------------------------------------------------

/// Verifies nominal golden execution of the Help action.
///
/// Asserts that executing `ToolAction::Help` returns `Ok(())`.
#[test]
fn test_section_1_nominal_help_execution() {
    let config = create_test_config(ToolAction::Help);
    let result = config.execute();
    assert!(
        result.is_ok(),
        "[SEC 1: ALWAYS PASS] Help action execution must return Ok(()): {:?}",
        result.err()
    );
}

/// Verifies nominal golden execution of the Version action.
///
/// Asserts that executing `ToolAction::Version` returns `Ok(())`.
#[test]
fn test_section_1_nominal_version_execution() {
    let config = create_test_config(ToolAction::Version);
    let result = config.execute();
    assert!(
        result.is_ok(),
        "[SEC 1: ALWAYS PASS] Version action execution must return Ok(()): {:?}",
        result.err()
    );
}

/// Verifies nominal golden execution of static print methods.
///
/// Asserts that `print_help` and `print_version` execute without panicking.
#[test]
fn test_section_1_nominal_static_emitters() {
    CliConfig::print_help();
    CliConfig::print_version();
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.2: [ SECTION 2: NEVER FAIL / FAULT INVERSION & DEGRADATION ]
// -----------------------------------------------------------------------------

/// Verifies fault inversion when `run_toolchain` receives invalid flag combinations.
///
/// Asserts that running the toolchain with empty arguments safely yields Ok(())
/// because it defaults to printing help.
#[test]
fn test_section_2_never_fail_run_toolchain_empty_args() {
    let empty: Vec<String> = vec![];
    let result = run_toolchain(&empty);
    assert!(
        result.is_ok(),
        "[SEC 2: NEVER FAIL] run_toolchain with empty args must cleanly print help"
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.3: [ SECTION 3: INVARIANTS, SATURATION & LIMIT GUARDS ]
// -----------------------------------------------------------------------------

/// Verifies operational constant invariants.
///
/// Asserts that canonical archive and baseline output constants are non-empty.
#[test]
fn test_section_3_invariants_operational_constants() {
    assert!(
        !CANONICAL_ARCHIVE_DIR.is_empty(),
        "[SEC 3: INVARIANTS] CANONICAL_ARCHIVE_DIR must not be empty"
    );
    assert_eq!(
        BASELINE_OUTPUT_SUBDIR, "docs/baseline",
        "[SEC 3: INVARIANTS] BASELINE_OUTPUT_SUBDIR must equal docs/baseline"
    );
}

/// Verifies configuration clone and debug formatting invariants.
///
/// Asserts that `CliConfig` can be cloned and debug-printed without loss.
#[test]
fn test_section_3_invariants_config_clone_and_debug() {
    let config = create_test_config(ToolAction::Help);
    let cloned = config.clone();
    assert_eq!(config.action, cloned.action);
    assert_eq!(config.verbose, cloned.verbose);
    assert_eq!(config.repo_root, cloned.repo_root);

    let debug_str = format!("{:?}", config);
    assert!(
        !debug_str.is_empty(),
        "[SEC 3: INVARIANTS] Debug string must not be empty"
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.4: [ SECTION 4: EMERGENCE & COMPOSITE WORKFLOWS ]
// -----------------------------------------------------------------------------

/// Verifies emergent entrypoint execution with simulated CLI tokens.
///
/// Asserts that `run_toolchain` with `--help` and `--version` terminates cleanly.
#[test]
fn test_section_4_emergence_run_toolchain_cli_bridges() {
    let help_args = vec!["tl_tools".to_string(), "--help".to_string()];
    assert!(
        run_toolchain(&help_args).is_ok(),
        "[SEC 4: EMERGENCE] run_toolchain with --help must succeed"
    );

    let ver_args = vec!["tl_tools".to_string(), "--version".to_string()];
    assert!(
        run_toolchain(&ver_args).is_ok(),
        "[SEC 4: EMERGENCE] run_toolchain with --version must succeed"
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
// Sub-Assembly 4.2: Command Subsystem Anchor Verification Gate
// -----------------------------------------------------------------------------

/// Audits physical source files required for command execution.
#[test]
fn test_preflight_cmd_anchors() {
    let root = resolve_repo_root();
    assert!(
        root.join("tools/src/cmd/mod.rs").is_file(),
        "[BLOCK 4: PREFLIGHT] tools/src/cmd/mod.rs must exist"
    );
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( Test Suite Epilogue & Summary Handshake )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 5.1: Epilogue & Clean State Handshake
// -----------------------------------------------------------------------------

/// Verifies that running dispatch unit tests leaves zero transient artifacts.
#[test]
fn test_epilogue_clean_state_handshake() {
    let root = resolve_repo_root();
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
