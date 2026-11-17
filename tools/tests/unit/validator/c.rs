// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/tests/unit/validator/c.rs
// @id            test.unit.tools.validator.c
// @layer         Layer 4: Verification Suite
// @archetype     verification_suite | unit_isolation
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-16
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Developer Toolchain // C11/C23 Validator Suite
// @role          Four-Section Ontological Integration Suite for C Validator
// @maturity      Rev 5 Baseline | Four-Section Transpiler-Ready Verification
// @chassis       bounded-heap | Test runner execution chassis; stack frames bounded
// @scale         O(N) execution time over engine/include/ headers
// @seams         in: validator::c | uses: tl_tools::validator | probes: self_audit -> exit
// @invariants    1. Four-Section Ontological Architecture strictly enforced:
//                   Section 1 (Always Pass), Section 2 (Never Fail),
//                   Section 3 (Invariants & Limits), Section 4 (Emergence).
//                2. External Consumer Purity: imports tl_tools via public API.
//                3. Zero Mocks: exercises live engine/include/ header files.
//                4. Strict <= 80-column line width discipline across documentation.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Tier 1 Integration verification harness evaluating the `validator::c`
//   subsystem from the perspective of an external consumer or CI/CD runner.
//   Exercises `HeaderValidator::run_audit`, Invariant 18 enforcement,
//   Cornerstone header guard derivation, Block 2 sub-assembly sequences,
//   and paired `.c` source parity across the 4-Section ontological standard.
//
//   ### For Game Designers & Non-Programmers:
//   Guarantees that the Cornerstone Game Engine's C codebase adheres to its
//   bedrock physical laws and architectural invariants.
//
//   ### For Systems Programmers & Engine Architects:
//   - Transpilation Alignment: Tests map 1:1 to OmniCode's 4-section verification
//     schema in Block 3, with Preflight in Block 4 and Epilogue in Block 5.
//   - Integration Boundary: Audits engine/include/ against subsystems.json without
//     violating crate encapsulation boundaries.
// =============================================================================

//! # Four-Section Integration Suite for C11/C23 Header & Source Validator
//!
//! Exercises `tl_tools::validator::c` against engine headers across
//! Always-Pass, Never-Fail, Invariant, and Emergent execution tiers.

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Pre-Conditions, Constants, Test Fixtures & Harness Scaffolding )
// =============================================================================

use std::path::{Path, PathBuf};
use tl_tools::validator::{
    c::{HeaderValidator, is_c_validator_ready},
    config::{load_header_rules_config, load_subsystems_registry, HeaderRulesConfig, SubsystemRegistry},
};

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Operational Test Constants & Suite Identifiers
// -----------------------------------------------------------------------------

/// Canonical test suite identifier matching engine convention.
pub const SUITE_NAME: &str = "C11/C23 Header Validator Integration Surface";

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Test Fixtures & Pre-Condition Assertions
// -----------------------------------------------------------------------------

/// Resolves the repository root by locating the `engine/include` directory.
fn resolve_repo_root() -> PathBuf {
    let candidates = [Path::new("."), Path::new(".."), Path::new("../..")];
    candidates
        .iter()
        .find(|p| p.join("engine/include").exists())
        .map(|p| p.canonicalize().unwrap_or_else(|_| p.to_path_buf()))
        .unwrap_or_else(|| PathBuf::from("."))
}

/// Preflight self-audit probe verifying that the C validator is ready.
pub fn self_audit() -> Result<(), &'static str> {
    if !is_c_validator_ready() {
        return Err("Self-audit failed: C validator is not ready");
    }
    Ok(())
}

// =============================================================================
// BLOCK 3: BODY BLOCK / FOUR-SECTION ONTOLOGICAL TEST SUITE
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: [ SECTION 1: ALWAYS PASS (NOMINAL GOLDEN SURFACE COGS) ]
// -----------------------------------------------------------------------------

/// Verifies nominal golden execution of whole-engine header validation.
#[test]
fn test_section_1_nominal_all_engine_headers_valid() {
    let repo_root = resolve_repo_root();
    if !repo_root.join("engine/include").exists() {
        return;
    }

    let registry = load_subsystems_registry(&repo_root).expect("Must load registry");
    let rules = load_header_rules_config(&repo_root);
    let validator = HeaderValidator::new(repo_root, registry, rules, false);
    let summary = validator.run_audit().expect("Audit run must succeed");

    assert!(
        summary.is_success(false),
        "[SEC 1: ALWAYS PASS] C11 Header validation failed: {} errors across {} files",
        summary.error_count(),
        summary.failed_files
    );
}

/// Playtests HeaderValidator on known aligned headers vs untouched header under strict mode.
#[test]
fn test_section_4_emergence_playtest_c_headers() {
    let repo_root = resolve_repo_root();
    let registry = load_subsystems_registry(&repo_root).expect("Must load registry");
    let mut rules = load_header_rules_config(&repo_root);

    // 1. Known aligned headers in log/, debug/, io/
    let known_aligned = [
        "log/log.h",
        "debug/debug.h",
        "io/path_finder.h",
    ];

    let validator_default = HeaderValidator::new(repo_root.clone(), registry.clone(), rules.clone(), false);
    for rel in &known_aligned {
        let p = repo_root.join("engine/include").join(rel);
        let mut issues = Vec::new();
        validator_default.validate_header(&p, &mut issues);
        let errs: Vec<_> = issues.iter().filter(|i| i.severity == tl_tools::validator::report::IssueSeverity::Error).collect();
        println!("Playtesting C KNOWN ALIGNED: {} -> {} errors", rel, errs.len());
        for e in &errs {
            println!("   [ERROR] {}: {}", e.field, e.message);
        }
        assert!(errs.is_empty(), "Header {} must pass validation cleanly", rel);
    }

    // 2. Untouched header in core/ (clock_pacer.h) when evaluated under strict rules
    // Add "core" to strict_template_domains to see the validator throw!
    rules.strict_template_domains.push("core".to_string());
    let validator_strict = HeaderValidator::new(repo_root.clone(), registry, rules, false);

    let untouched = "core/clock_pacer.h";
    let p = repo_root.join("engine/include").join(untouched);
    let mut issues = Vec::new();
    validator_strict.validate_header(&p, &mut issues);
    let errs: Vec<_> = issues.iter().filter(|i| i.severity == tl_tools::validator::report::IssueSeverity::Error).collect();
    println!("Playtesting C UNTOUCHED STRICT: {} -> {} errors (PROPERLY THROWS)", untouched, errs.len());
    for e in &errs {
        println!("   [THROWN ERROR] {}: {}", e.field, e.message);
    }
    assert!(
        !errs.is_empty(),
        "Untouched file {} under strict mode must throw validation errors!",
        untouched
    );
}


/// Verifies nominal golden derivation of canonical Cornerstone include guards.
#[test]
fn test_section_1_nominal_guard_derivation() {
    let rules = HeaderRulesConfig::default();
    let validator = HeaderValidator::new(PathBuf::from("."), SubsystemRegistry::default(), rules, false);

    assert_eq!(
        validator.expected_cornerstone_guard("log/log.h"),
        "CORNERSTONE_LOG_LOG_H",
        "[SEC 1: ALWAYS PASS] Expected CORNERSTONE_LOG_LOG_H"
    );
    assert_eq!(
        validator.expected_cornerstone_guard("debug/debug_types.h"),
        "CORNERSTONE_DEBUG_DEBUG_TYPES_H",
        "[SEC 1: ALWAYS PASS] Expected CORNERSTONE_DEBUG_DEBUG_TYPES_H"
    );
    assert_eq!(
        validator.expected_cornerstone_guard("core/manifest.h"),
        "CORNERSTONE_CORE_MANIFEST_H",
        "[SEC 1: ALWAYS PASS] Expected CORNERSTONE_CORE_MANIFEST_H"
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.2: [ SECTION 2: NEVER FAIL / FAULT INVERSION & DEGRADATION ]
// -----------------------------------------------------------------------------

/// Verifies fault inversion when header discovery runs on a nonexistent directory.
#[test]
fn test_section_2_never_fail_empty_include_directory() {
    let headers = HeaderValidator::discover_headers(Path::new("nonexistent_folder_path"));
    assert!(
        headers.is_empty(),
        "[SEC 2: NEVER FAIL] Nonexistent folder must return empty list without crashing"
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.3: [ SECTION 3: INVARIANTS, SATURATION & LIMIT GUARDS ]
// -----------------------------------------------------------------------------

/// Verifies that the C validator readiness predicate evaluates to true.
#[test]
fn test_section_3_invariants_c_validator_ready() {
    assert!(
        is_c_validator_ready(),
        "[SEC 3: INVARIANTS] is_c_validator_ready must return true"
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.4: [ SECTION 4: EMERGENCE & COMPOSITE WORKFLOWS ]
// -----------------------------------------------------------------------------

/// Verifies emergent self-audit execution of the C validator suite.
#[test]
fn test_section_4_emergence_self_audit() {
    assert!(
        self_audit().is_ok(),
        "[SEC 4: EMERGENCE] C validator self_audit must pass"
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
        self_audit().is_ok(),
        "[BLOCK 4: PREFLIGHT] C validator preflight self_audit() must pass"
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 4.2: Repository Anchor Verification Gate
// -----------------------------------------------------------------------------

/// Audits physical repository anchors required for C11 header verification.
#[test]
fn test_preflight_engine_anchors() {
    let root = resolve_repo_root();
    assert!(
        root.join("engine/include").is_dir(),
        "[BLOCK 4: PREFLIGHT] Anchor missing: engine/include/"
    );
    assert!(
        root.join("engine/config/standards/subsystems.json").is_file(),
        "[BLOCK 4: PREFLIGHT] Anchor missing: subsystems.json"
    );
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( Test Suite Epilogue & Summary Handshake )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 5.1: Epilogue & Clean State Handshake
// -----------------------------------------------------------------------------

/// Readiness probe for the C validator integration test runner.
#[test]
fn test_epilogue_clean_state_handshake() {
    assert!(
        is_c_validator_ready(),
        "[BLOCK 5: EPILOGUE] C validator must remain ready post-test"
    );
}
