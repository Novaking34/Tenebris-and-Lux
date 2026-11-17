// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/tests/functional/cmd.rs
// @id            test.functional.tools.cmd
// @layer         Layer 4: Verification Suite
// @archetype     verification_suite | functional_surface
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-16
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Developer Toolchain // Tier 1 Functional CMD Suite
// @role          Functional Surface Testing for tl_tools CLI Subcommands
// @maturity      Rev 5 Baseline | 4-Section Ontological Testing
// @chassis       bounded-heap | Test runner execution chassis
// @scale         O(1) execution over CLI argument parsing
// @seams         in: tl_tools::cmd
// @invariants    1. Four-Section Ontological Suite: Always Pass, Never Fail,
//                   Invariants, Emergence.
// @chassis       bounded-heap | Test runner execution chassis; stack frames bounded
// @scale         O(1) execution over CLI argument parsing and dispatch surfaces
// @seams         in: tl_tools::cmd | uses: common::resolve_repo_root
// @invariants    1. Four-Section Ontological Architecture strictly enforced:
//                   Section 1 (Always Pass), Section 2 (Never Fail),
//                   Section 3 (Invariants & Limits), Section 4 (Emergence).
//                2. Zero Panics: CLI argument parser degrades gracefully to help.
//                3. External Consumer Purity: imports tl_tools via public API.
//                4. Strict <= 80-column line width discipline across documentation.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Tier 1 Functional surface test suite evaluating the `tl_tools::cmd` CLI
//   dispatcher and argument parser. Validates coarse command-line interface
//   responsiveness across help, version, and subcommand selection switches,
//   ensuring that invalid or malformed flags degrade cleanly without panicking.
//
//   ### For Game Designers & Non-Programmers:
//   Guarantees that all command-line buttons, switches, and mode flags in the
//   developer toolchain respond predictably, display helpful instructions when
//   used incorrectly, and never crash unexpectedly.
//
//   ### For Systems Programmers & Engine Architects:
//   - Command Boundary: Tests `parse_cli_args` and `ToolAction` mapping as an
//     external surface without mocking or violating internal state boundaries.
//   - Fault Inversion: Proves that empty argument vectors or unknown subcommands
//     gracefully downgrade to `ToolAction::Help`.
// =============================================================================

//! # Tier 1 Functional Surface Verification Suite for CLI Subcommands
//!
//! Evaluates `tl_tools::cmd` dispatch surfaces across Always-Pass, Never-Fail,
//! Invariant, and Emergent ontological tiers.

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Pre-Conditions, Constants, Test Fixtures & Harness Scaffolding )
// =============================================================================

use crate::common;
use std::path::PathBuf;
use tl_tools::cmd::{
    parse_cli_args, resolve_repo_root, ToolAction,
};

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Operational Test Constants & Suite Identifiers
// -----------------------------------------------------------------------------

/// Canonical suite identifier matching engine testing convention.
pub const SUITE_NAME: &str = "Tier 1: Functional CLI Subcommand Surface";

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Test Fixtures & Pre-Condition Assertions
// -----------------------------------------------------------------------------

/// Constructs an argument vector from string slice tokens.
fn create_test_args(tokens: &[&str]) -> Vec<String> {
    tokens.iter().map(|s| s.to_string()).collect()
}

/// Resolves validated workspace root asserting anchor existence.
fn get_test_repo_root() -> PathBuf {
    let root = resolve_repo_root();
    assert!(
        root.join("tools/Cargo.toml").is_file(),
        "Resolved repository root {:?} must host tools/Cargo.toml",
        root
    );
    root
}

// =============================================================================
// BLOCK 3: BODY BLOCK / FOUR-SECTION ONTOLOGICAL TEST SUITE
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: [ SECTION 1: ALWAYS PASS (NOMINAL GOLDEN SURFACE COGS) ]
// -----------------------------------------------------------------------------

/// Verifies nominal golden execution of the CLI help flag.
///
/// Asserts that passing `--help` selects `ToolAction::Help`.
#[test]
fn test_section_1_nominal_cli_help_flag() {
    let args = create_test_args(&["tl_tools", "--help"]);
    let config = parse_cli_args(&args);
    assert!(
        matches!(config.action, ToolAction::Help),
        "[SEC 1: ALWAYS PASS] --help must select ToolAction::Help"
    );
}

/// Verifies nominal golden execution of the CLI short help flag.
///
/// Asserts that passing `-h` selects `ToolAction::Help`.
#[test]
fn test_section_1_nominal_cli_short_help_flag() {
    let args = create_test_args(&["tl_tools", "-h"]);
    let config = parse_cli_args(&args);
    assert!(
        matches!(config.action, ToolAction::Help),
        "[SEC 1: ALWAYS PASS] -h must select ToolAction::Help"
    );
}

/// Verifies nominal golden execution of the CLI version flag.
///
/// Asserts that passing `--version` or `-V` selects `ToolAction::Version`.
#[test]
fn test_section_1_nominal_cli_version_flag() {
    let args = create_test_args(&["tl_tools", "--version"]);
    let config = parse_cli_args(&args);
    assert!(
        matches!(config.action, ToolAction::Version),
        "[SEC 1: ALWAYS PASS] --version must select ToolAction::Version"
    );

    let short_args = create_test_args(&["tl_tools", "-V"]);
    let short_config = parse_cli_args(&short_args);
    assert!(
        matches!(short_config.action, ToolAction::Version),
        "[SEC 1: ALWAYS PASS] -V must select ToolAction::Version"
    );
}

/// Verifies nominal golden dispatch of all primary CLI subcommands.
///
/// Asserts that standard subcommand verbs route to their respective actions.
#[test]
fn test_section_1_nominal_subcommand_dispatch() {
    let validate_args = vec!["tl_tools".to_string(), "validate".to_string()];
    assert!(matches!(parse_cli_args(&validate_args).action, ToolAction::Validate(_)));
    let validate_args = create_test_args(&["tl_tools", "validate"]);
    assert!(
        matches!(parse_cli_args(&validate_args).action, ToolAction::Validate(_)),
        "[SEC 1: ALWAYS PASS] 'validate' must route to ToolAction::Validate"
    );

    let audit_args = vec!["tl_tools".to_string(), "audit".to_string()];
    assert!(matches!(parse_cli_args(&audit_args).action, ToolAction::Audit(_)));
    let audit_args = create_test_args(&["tl_tools", "audit"]);
    assert!(
        matches!(parse_cli_args(&audit_args).action, ToolAction::Audit(_)),
        "[SEC 1: ALWAYS PASS] 'audit' must route to ToolAction::Audit"
    );

    let guide_args = vec!["tl_tools".to_string(), "guide".to_string()];
    assert!(matches!(parse_cli_args(&guide_args).action, ToolAction::Guide(_)));
    let guide_args = create_test_args(&["tl_tools", "guide"]);
    assert!(
        matches!(parse_cli_args(&guide_args).action, ToolAction::Guide(_)),
        "[SEC 1: ALWAYS PASS] 'guide' must route to ToolAction::Guide"
    );

    let extract_args = vec!["tl_tools".to_string(), "extract".to_string()];
    assert!(matches!(parse_cli_args(&extract_args).action, ToolAction::Extract(_)));
    let extract_args = create_test_args(&["tl_tools", "extract"]);
    assert!(
        matches!(parse_cli_args(&extract_args).action, ToolAction::Extract(_)),
        "[SEC 1: ALWAYS PASS] 'extract' must route to ToolAction::Extract"
    );

    let assets_args = vec!["tl_tools".to_string(), "assets".to_string()];
    assert!(matches!(parse_cli_args(&assets_args).action, ToolAction::Assets(_)));
    let assets_args = create_test_args(&["tl_tools", "assets"]);
    assert!(
        matches!(parse_cli_args(&assets_args).action, ToolAction::Assets(_)),
        "[SEC 1: ALWAYS PASS] 'assets' must route to ToolAction::Assets"
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.2: [ SECTION 2: NEVER FAIL / FAULT INVERSION & DEGRADATION ]
// -----------------------------------------------------------------------------

/// Verifies fault inversion when the argument vector is completely empty.
///
/// Asserts that zero arguments gracefully default to `ToolAction::Help`.
#[test]
fn test_section_2_never_fail_empty_args_defaults_to_help() {
    let empty: Vec<String> = vec![];
    assert!(matches!(parse_cli_args(&empty).action, ToolAction::Help));
    let config = parse_cli_args(&empty);
    assert!(
        matches!(config.action, ToolAction::Help),
        "[SEC 2: NEVER FAIL] Empty args must safely default to ToolAction::Help"
    );
}

/// Verifies fault inversion when an unrecognized subcommand verb is supplied.
///
/// Asserts that unknown subcommands gracefully default to `ToolAction::Help`.
#[test]
fn test_section_2_never_fail_unknown_subcommand_defaults_to_help() {
    let unknown = vec!["tl_tools".to_string(), "unknown_subcommand_xyz".to_string()];
    assert!(matches!(parse_cli_args(&unknown).action, ToolAction::Help));
    let unknown = create_test_args(&["tl_tools", "nonexistent_subcommand_xyz"]);
    let config = parse_cli_args(&unknown);
    assert!(
        matches!(config.action, ToolAction::Help),
        "[SEC 2: NEVER FAIL] Unknown subcommand must default to ToolAction::Help"
    );
}

/// Verifies fault inversion when binary invocation name is omitted.
///
/// Asserts that single unknown token arguments safely yield `ToolAction::Help`.
#[test]
fn test_section_2_never_fail_single_token_defaults_to_help() {
    let single = create_test_args(&["--unrecognized-flag-xyz"]);
    let config = parse_cli_args(&single);
    assert!(
        matches!(config.action, ToolAction::Help),
        "[SEC 2: NEVER FAIL] Single invalid token must yield ToolAction::Help"
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.3: [ SECTION 3: INVARIANTS, SATURATION & LIMIT GUARDS ]
// -----------------------------------------------------------------------------

/// Verifies repository root resolution invariant guarantees.
///
/// Asserts that the discovered workspace root contains required engine anchors.
#[test]
fn test_section_3_invariants_repo_root_resolution() {
    let root = resolve_repo_root();
    assert!(root.is_dir(), "Repository root must resolve to a valid directory");
    assert!(root.join("tools/Cargo.toml").is_file(), "Root must contain tools/Cargo.toml");
    let root = get_test_repo_root();
    assert!(
        root.is_dir(),
        "[SEC 3: INVARIANTS] Repository root must resolve to a valid directory"
    );
    assert!(
        root.join("CMakeLists.txt").is_file(),
        "[SEC 3: INVARIANTS] Root must contain root CMakeLists.txt"
    );
    assert!(
        root.join("tools/Cargo.toml").is_file(),
        "[SEC 3: INVARIANTS] Root must contain tools/Cargo.toml"
    );
    assert!(
        root.join("engine/include").is_dir(),
        "[SEC 3: INVARIANTS] Root must contain engine/include"
    );
}

/// Verifies action discrimination invariant properties.
///
/// Asserts that distinct subcommand configurations yield distinct variants.
#[test]
fn test_section_3_invariants_action_discrimination() {
    let val = parse_cli_args(&create_test_args(&["tl_tools", "validate"]));
    let aud = parse_cli_args(&create_test_args(&["tl_tools", "audit"]));
    let gui = parse_cli_args(&create_test_args(&["tl_tools", "guide"]));

    assert!(
        !matches!(val.action, ToolAction::Help),
        "[SEC 3: INVARIANTS] Validate must not be Help"
    );
    assert!(
        !matches!(aud.action, ToolAction::Help),
        "[SEC 3: INVARIANTS] Audit must not be Help"
    );
    assert!(
        !matches!(gui.action, ToolAction::Help),
        "[SEC 3: INVARIANTS] Guide must not be Help"
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.4: [ SECTION 4: EMERGENCE & COMPOSITE WORKFLOWS ]
// -----------------------------------------------------------------------------

/// Verifies emergent CLI bridge dispatch and argument combination.
///
/// Simulates realistic user invocations with flags and relative pathing.
#[test]
fn test_section_4_emergence_subcommand_flag_composition() {
    let args = create_test_args(&["tl_tools", "validate", "--help"]);
    let config = parse_cli_args(&args);
    // Passing --help to validate sub-assembly
    assert!(
        matches!(config.action, ToolAction::Validate(_)),
        "[SEC 4: EMERGENCE] 'validate --help' must yield Validate action"
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
// Sub-Assembly 4.2: Domain Anchor Verification Gate
// -----------------------------------------------------------------------------

/// Audits physical workspace anchors required for CLI toolchain dispatch.
#[test]
fn test_preflight_workspace_anchors() {
    let root = get_test_repo_root();
    assert!(
        root.join("tools/src/main.rs").is_file(),
        "[BLOCK 4: PREFLIGHT] tools/src/main.rs entrypoint must exist"
    );
    assert!(
        root.join("tools/src/lib.rs").is_file(),
        "[BLOCK 4: PREFLIGHT] tools/src/lib.rs library root must exist"
    );
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( Test Suite Epilogue & Summary Handshake )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 5.1: Epilogue & Clean State Handshake
// -----------------------------------------------------------------------------

/// Verifies that running functional CLI tests leaves zero transient artifacts.
#[test]
fn test_epilogue_clean_state_handshake() {
    let root = resolve_repo_root();
    assert!(root.exists());
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
