// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/tests/unit/cmd/args.rs
// @id            test.unit.tools.cmd.args
// @layer         Layer 4: Verification Suite
// @archetype     verification_suite | unit_isolation
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-16
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Developer Toolchain // Tier 2 Unit CMD Args Suite
// @role          Unit Isolation Testing for CLI Argument Parsing and Config
// @maturity      Rev 5 Baseline | 4-Section Ontological Testing
// @chassis       bounded-heap | Test runner execution chassis; stack frames bounded
// @scale         O(1) execution time across CLI argument token streams
// @seams         in: tl_tools::cmd::{parse_cli_args, resolve_repo_root, is_valid_repo_root}
// @invariants    1. Four-Section Ontological Architecture strictly enforced:
//                   Section 1 (Always Pass), Section 2 (Never Fail),
//                   Section 3 (Invariants & Limits), Section 4 (Emergence).
//                2. Zero Panics: Malformed token vectors degrade to default/help.
//                3. External Consumer Purity: imports tl_tools via public API.
//                4. Strict <= 80-column line width discipline across documentation.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Tier 2 Unit isolation test suite evaluating `tl_tools::cmd` argument parsing
//   logic. Verifies `parse_cli_args`, global flag extraction (`--verbose`, `-v`),
//   subcommand option decoding (`--scan`, `--strict`, `--json`), and workspace
//   root detection predicates in complete isolation.
//
//   ### For Game Designers & Non-Programmers:
//   Ensures the toolchain reliably interprets command options and mode switches
//   so artists and designers can run tools from any directory without error.
//
//   ### For Systems Programmers & Engine Architects:
//   - Token Stream Isolation: Tests individual parser branches without spawning
//     processes or mutating environment variables.
//   - Fault Inversion: Proves that unexpected tokens or omitted subcommands
//     gracefully downgrade to `ToolAction::Help`.
// =============================================================================

//! # Tier 2 Unit Isolation Suite for CLI Argument Parsing
//!
//! Evaluates `parse_cli_args`, flag decoders, and repo root resolution across
//! Always-Pass, Never-Fail, Invariant, and Emergent ontological tiers.

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Pre-Conditions, Constants, Test Fixtures & Harness Scaffolding )
// =============================================================================

use crate::common;
use std::path::Path;
use tl_tools::cmd::{
    assets::AssetMode,
    guide::GuideMode,
    is_valid_repo_root, parse_cli_args, resolve_repo_root,
    validate::ValidationDomain,
    ToolAction,
};

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Operational Test Constants & Suite Identifiers
// -----------------------------------------------------------------------------

/// Canonical suite identifier matching engine testing convention.
pub const SUITE_NAME: &str = "Tier 2: Unit CMD Argument Parsing Surface";

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Test Fixtures & Pre-Condition Assertions
// -----------------------------------------------------------------------------

/// Converts a slice of string tokens into an owned argument vector.
fn to_args(tokens: &[&str]) -> Vec<String> {
    tokens.iter().map(|s| s.to_string()).collect()
}

// =============================================================================
// BLOCK 3: BODY BLOCK / FOUR-SECTION ONTOLOGICAL TEST SUITE
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: [ SECTION 1: ALWAYS PASS (NOMINAL GOLDEN SURFACE COGS) ]
// -----------------------------------------------------------------------------

/// Verifies nominal golden parsing of the `--verbose` and `-v` flags.
///
/// Asserts that passing verbose switches sets `config.verbose = true`.
#[test]
fn test_section_1_nominal_verbose_flags() {
    let long_args = to_args(&["tl_tools", "help", "--verbose"]);
    let long_config = parse_cli_args(&long_args);
    assert!(
        long_config.verbose,
        "[SEC 1: ALWAYS PASS] --verbose flag must set verbose to true"
    );

    let short_args = to_args(&["tl_tools", "-v", "help"]);
    let short_config = parse_cli_args(&short_args);
    assert!(
        short_config.verbose,
        "[SEC 1: ALWAYS PASS] -v flag must set verbose to true"
    );
}

/// Verifies nominal golden parsing of guide subcommand modes.
///
/// Asserts that mode switches map to their corresponding `GuideMode` variants.
#[test]
fn test_section_1_nominal_guide_subcommand_modes() {
    let scan_args = to_args(&["tl_tools", "guide", "--scan"]);
    if let ToolAction::Guide(args) = parse_cli_args(&scan_args).action {
        assert_eq!(args.mode, GuideMode::ScanOnly);
    } else {
        panic!("Expected ToolAction::Guide");
    }

    let val_args = to_args(&["tl_tools", "guide", "--validate-layout"]);
    if let ToolAction::Guide(args) = parse_cli_args(&val_args).action {
        assert_eq!(args.mode, GuideMode::ValidateLayout);
    } else {
        panic!("Expected ToolAction::Guide");
    }

    let master_args = to_args(&["tl_tools", "guide", "--master"]);
    if let ToolAction::Guide(args) = parse_cli_args(&master_args).action {
        assert_eq!(args.mode, GuideMode::MasterOnly);
    } else {
        panic!("Expected ToolAction::Guide");
    }
}

/// Verifies nominal golden parsing of validate subcommand domains and flags.
///
/// Asserts that domain verbs and options map to `ValidateArgs`.
#[test]
fn test_section_1_nominal_validate_subcommand_domains() {
    let h_args = to_args(&["tl_tools", "validate", "headers", "--strict"]);
    if let ToolAction::Validate(args) = parse_cli_args(&h_args).action {
        assert_eq!(args.domain, ValidationDomain::Headers);
        assert!(args.strict);
    } else {
        panic!("Expected ToolAction::Validate");
    }

    let d_args = to_args(&["tl_tools", "validate", "data", "--json"]);
    if let ToolAction::Validate(args) = parse_cli_args(&d_args).action {
        assert_eq!(args.domain, ValidationDomain::Data);
        assert!(args.json_output);
    } else {
        panic!("Expected ToolAction::Validate");
    }
}

/// Verifies nominal golden parsing of assets subcommand modes.
///
/// Asserts that `generate` maps to `AssetMode::Generate` and default to `Audit`.
#[test]
fn test_section_1_nominal_assets_subcommand_modes() {
    let gen_args = to_args(&["tl_tools", "assets", "generate"]);
    if let ToolAction::Assets(args) = parse_cli_args(&gen_args).action {
        assert_eq!(args.mode, AssetMode::Generate);
    } else {
        panic!("Expected ToolAction::Assets");
    }

    let audit_args = to_args(&["tl_tools", "assets", "audit"]);
    if let ToolAction::Assets(args) = parse_cli_args(&audit_args).action {
        assert_eq!(args.mode, AssetMode::Audit);
    } else {
        panic!("Expected ToolAction::Assets");
    }
}

/// Verifies nominal golden parsing of the `curate` and `all` subcommands.
///
/// Asserts that `curate` maps to `ToolAction::Curate` and `all` to `ToolAction::All`.
#[test]
fn test_section_1_nominal_pipeline_subcommands() {
    let curate_args = to_args(&["tl_tools", "curate"]);
    assert_eq!(
        parse_cli_args(&curate_args).action,
        ToolAction::Curate,
        "[SEC 1: ALWAYS PASS] 'curate' must map to ToolAction::Curate"
    );

    let all_args = to_args(&["tl_tools", "all"]);
    assert_eq!(
        parse_cli_args(&all_args).action,
        ToolAction::All,
        "[SEC 1: ALWAYS PASS] 'all' must map to ToolAction::All"
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.2: [ SECTION 2: NEVER FAIL / FAULT INVERSION & DEGRADATION ]
// -----------------------------------------------------------------------------

/// Verifies fault inversion when probing invalid candidate paths for repository root.
///
/// Asserts that non-existent paths safely return false without filesystem panic.
#[test]
fn test_section_2_never_fail_invalid_repo_root_candidates() {
    assert!(
        !is_valid_repo_root(Path::new("/nonexistent/directory/xyz")),
        "[SEC 2: NEVER FAIL] Nonexistent directory cannot be valid repo root"
    );
    assert!(
        !is_valid_repo_root(Path::new("/tmp")),
        "[SEC 2: NEVER FAIL] Generic temporary folder cannot be valid repo root"
    );
}

/// Verifies fault inversion when argument vector is empty or malformed.
///
/// Asserts that unknown subcommands gracefully downgrade to ToolAction::Help.
#[test]
fn test_section_2_never_fail_unknown_subcommands() {
    let unknown = to_args(&["tl_tools", "unknown_subcommand_123"]);
    assert!(
        matches!(parse_cli_args(&unknown).action, ToolAction::Help),
        "[SEC 2: NEVER FAIL] Unknown subcommand must yield ToolAction::Help"
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.3: [ SECTION 3: INVARIANTS, SATURATION & LIMIT GUARDS ]
// -----------------------------------------------------------------------------

/// Verifies repository root invariant predicates against authentic workspace.
///
/// Asserts that the live repository satisfies all root validation invariants.
#[test]
fn test_section_3_invariants_authentic_repo_root() {
    let root = resolve_repo_root();
    assert!(
        is_valid_repo_root(&root),
        "[SEC 3: INVARIANTS] Resolved repository root must satisfy is_valid_repo_root"
    );
    assert!(
        root.join("CMakeLists.txt").is_file(),
        "[SEC 3: INVARIANTS] Repo root must contain CMakeLists.txt"
    );
    assert!(
        root.join("docs").is_dir(),
        "[SEC 3: INVARIANTS] Repo root must contain docs directory"
    );
}

/// Verifies default configuration invariants when zero flags are supplied.
///
/// Asserts that default execution config has `verbose = false`.
#[test]
fn test_section_3_invariants_default_config_invariants() {
    let args = to_args(&["tl_tools", "--help"]);
    let config = parse_cli_args(&args);
    assert!(
        !config.verbose,
        "[SEC 3: INVARIANTS] Default config must not be verbose"
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.4: [ SECTION 4: EMERGENCE & COMPOSITE WORKFLOWS ]
// -----------------------------------------------------------------------------

/// Verifies emergent composite subcommand argument parsing.
///
/// Asserts that subcommands with options and flags parse accurately together.
#[test]
fn test_section_4_emergence_compound_flag_parsing() {
    let args = to_args(&[
        "tl_tools",
        "validate",
        "headers",
        "--strict",
        "--verbose",
    ]);
    let config = parse_cli_args(&args);
    assert!(config.verbose, "Verbose must be parsed in compound invocation");
    if let ToolAction::Validate(vargs) = config.action {
        assert_eq!(vargs.domain, ValidationDomain::Headers);
        assert!(vargs.strict);
    } else {
        panic!("Expected ToolAction::Validate in compound invocation");
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
// Sub-Assembly 4.2: Command Subsystem Anchor Verification Gate
// -----------------------------------------------------------------------------

/// Audits physical parser implementation in `tools/src/cmd/mod.rs`.
#[test]
fn test_preflight_cmd_parser_anchors() {
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

/// Verifies that running argument parser tests leaves zero transient artifacts.
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
