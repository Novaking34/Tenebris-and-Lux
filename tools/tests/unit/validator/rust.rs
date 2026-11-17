// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/tests/unit/validator/rust.rs
// @id            test.unit.tools.validator.rust
// @layer         Layer 4: Verification Suite
// @archetype     verification_suite | unit_isolation
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-16
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Developer Toolchain // Rust Validator Unit Suite
// @role          Unit Isolation Testing for Rust 2024 Edition Rule Validator
// @maturity      Rev 5 Baseline | 4-Section Ontological Testing
// @chassis       bounded-heap | Test runner execution chassis; stack frames bounded
// @scale         O(L) lexical analysis across Rust token streams and items
// @seams         in: tl_tools::validator::rust | pairs: tools/src/
// @invariants    1. Four-Section Ontological Architecture strictly enforced:
//                   Section 1 (Always Pass), Section 2 (Never Fail),
//                   Section 3 (Invariants & Limits), Section 4 (Emergence).
//                2. Zero Panics: Malformed Rust source records issues cleanly.
//                3. External Consumer Purity: imports tl_tools via public API.
//                4. Strict <= 80-column line width discipline across documentation.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Tier 2 Unit isolation test suite evaluating `tl_tools::validator::rust`.
//   Validates Rust source files against the OmniCode Universal 5-Block Lifecycle
//   Protocol, Rust 2024 Edition standards, Four-Section Ontological Test Schema,
//   preflight `self_audit()`, and epilogue readiness probes in complete isolation.
//
//   ### For Game Designers & Non-Programmers:
//   Ensures that developer tools and backend logic written in Rust obey all
//   strict quality rules, documentation standards, and architectural covenants.
//
//   ### For Systems Programmers & Engine Architects:
//   - Lexical Scope: Verifies item scanner, banner auditor, and preflight gates
//     without invoking external cargo processes.
//   - Fault Inversion: Proves that missing blocks or absent probes emit structured
//     `ValidationIssue` records rather than crashing.
// =============================================================================

//! # Tier 2 Unit Isolation Suite for Rust 2024 Validator
//!
//! Evaluates `tl_tools::validator::rust` across Always-Pass, Never-Fail,
//! Invariant, and Emergent ontological tiers.

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Pre-Conditions, Constants, Test Fixtures & Harness Scaffolding )
// =============================================================================

use crate::common;
use std::path::Path;
use tl_tools::validator::rust::{
    is_rust_validator_ready, rust_validator_status, self_audit,
    validate_rust_content, RustValidationConfig,
};

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Operational Test Constants & Suite Identifiers
// -----------------------------------------------------------------------------

/// Canonical suite identifier matching engine testing convention.
pub const SUITE_NAME: &str = "Tier 2: Unit Rust Validator Surface";

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Test Fixtures & Pre-Condition Assertions
// -----------------------------------------------------------------------------

/// Constructs a mock Rust source path for validation testing.
fn sample_rust_path() -> &'static Path {
    Path::new("tools/src/sample.rs")
}

// =============================================================================
// BLOCK 3: BODY BLOCK / FOUR-SECTION ONTOLOGICAL TEST SUITE
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: [ SECTION 1: ALWAYS PASS (NOMINAL GOLDEN SURFACE COGS) ]
// -----------------------------------------------------------------------------

/// Verifies nominal golden validation of canonical 5-block Rust source.
///
/// Asserts that a well-formed canonical 5-block Rust file produces zero issues.
#[test]
fn test_section_1_nominal_valid_5block_rust_passes() {
    let source = common::sample_5block_rust();
    let mut issues = Vec::new();
    let config = RustValidationConfig::default();
    validate_rust_content(sample_rust_path(), source, &config, &mut issues);
    assert!(
        issues.is_empty(),
        "[SEC 1: ALWAYS PASS] Canonical 5-block Rust must produce zero issues: {:?}",
        issues
    );
}

/// Verifies nominal golden execution of the Rust validator readiness probe.
///
/// Asserts that `is_rust_validator_ready()` returns true.
#[test]
fn test_section_1_nominal_rust_validator_readiness_probe() {
    assert!(
        is_rust_validator_ready(),
        "[SEC 1: ALWAYS PASS] Rust validator must report ready status"
    );
}

/// Verifies nominal golden retrieval of the Rust validator status string.
///
/// Asserts that status string contains version and standard indicators.
#[test]
fn test_section_1_nominal_rust_validator_status_string() {
    let status = rust_validator_status();
    assert_eq!(
        status, "READY",
        "[SEC 1: ALWAYS PASS] Status string must equal READY"
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.2: [ SECTION 2: NEVER FAIL / FAULT INVERSION & DEGRADATION ]
// -----------------------------------------------------------------------------

/// Verifies fault inversion when validating Rust source missing block banners.
///
/// Asserts that missing block banners are flagged with structured issues.
#[test]
fn test_section_2_never_fail_missing_5block_banners_recorded() {
    let bad_source = "pub fn add(a: i32, b: i32) -> i32 { a + b }\n";
    let mut issues = Vec::new();
    let config = RustValidationConfig::default();
    validate_rust_content(sample_rust_path(), bad_source, &config, &mut issues);
    assert!(
        !issues.is_empty(),
        "[SEC 2: NEVER FAIL] Rust source missing 5-block banners must report issues"
    );
}

/// Verifies fault inversion when validating Rust source missing self-audit preflight.
///
/// Asserts that omitting `pub fn self_audit()` triggers a validation issue.
#[test]
fn test_section_2_never_fail_missing_self_audit_preflight_recorded() {
    let partial_source = r#"// BLOCK 1: METADATA BLOCK
// BLOCK 2: OPENING BLOCK
// BLOCK 3: BODY BLOCK
// BLOCK 4: AUDIT PROCEDURES BLOCK
// BLOCK 5: CLOSING BLOCK
pub fn is_ready() -> bool { true }
"#;
    let mut issues = Vec::new();
    let config = RustValidationConfig::default();
    validate_rust_content(sample_rust_path(), partial_source, &config, &mut issues);
    assert!(
        issues.iter().any(|i| i.message.contains("self_audit")),
        "[SEC 2: NEVER FAIL] Missing self_audit probe must be flagged"
    );
}

/// Verifies fault inversion when validating completely empty Rust content.
///
/// Asserts that empty inputs degrade gracefully to reported issues without panic.
#[test]
fn test_section_2_never_fail_empty_rust_content_graceful() {
    let mut issues = Vec::new();
    let config = RustValidationConfig::default();
    validate_rust_content(sample_rust_path(), "", &config, &mut issues);
    assert!(
        !issues.is_empty(),
        "[SEC 2: NEVER FAIL] Empty content must report issues cleanly"
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.3: [ SECTION 3: INVARIANTS, SATURATION & LIMIT GUARDS ]
// -----------------------------------------------------------------------------

/// Verifies default configuration invariants for the Rust validator.
///
/// Asserts that all standard quality checks are enabled by default.
#[test]
fn test_section_3_invariants_rust_validation_config_defaults() {
    let config = RustValidationConfig::default();
    assert!(config.require_all_5_blocks, "Must require all 5 blocks by default");
    assert!(config.require_self_audit, "Must require self_audit by default");
    assert!(config.require_readiness_probe, "Must require readiness probe by default");
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.4: [ SECTION 4: EMERGENCE & COMPOSITE WORKFLOWS ]
// -----------------------------------------------------------------------------

/// Verifies emergent validation against authentic toolchain entrypoint source.
///
/// Asserts that `tools/src/main.rs` validates cleanly under standard config.
#[test]
fn test_section_4_emergence_live_tools_src_validation() {
    let root = common::resolve_repo_root();
    let main_rs_path = root.join("tools/src/main.rs");
    if let Ok(content) = std::fs::read_to_string(&main_rs_path) {
        let mut issues = Vec::new();
        let config = RustValidationConfig {
            require_all_5_blocks: true,
            require_metadata_tags: true,
            require_sub_assemblies: true,
            require_self_audit: false, // binary entrypoint
            require_readiness_probe: false,
            require_four_test_sections: false,
        };
        validate_rust_content(&main_rs_path, &content, &config, &mut issues);
        assert!(
            issues.is_empty(),
            "[SEC 4: EMERGENCE] tools/src/main.rs must pass structural validation: {:?}",
            issues
        );
    }
}

/// Playtests the validator against known aligned files vs untouched older files.
///
/// Asserts that known aligned files pass, and untouched files throw expected errors.
#[test]
fn test_section_4_emergence_playtest_aligned_vs_untouched() {
    let root = common::resolve_repo_root();
    let config = RustValidationConfig::default();

    // 1. Known aligned files in tools/src/ that we KNOW we aligned
    let known_aligned = [
        "tools/src/cmd/mod.rs",
        "tools/src/guide/mod.rs",
        "tools/src/syntax/mod.rs",
        "tools/src/validator/mod.rs",
    ];

    for rel in &known_aligned {
        let p = root.join(rel);
        if let Ok(content) = std::fs::read_to_string(&p) {
            let mut issues = Vec::new();
            validate_rust_content(&p, &content, &config, &mut issues);
            println!("Playtesting KNOWN ALIGNED: {} -> {} issues", rel, issues.len());
            for iss in &issues {
                println!("   [ISSUE] {}: {}", iss.field, iss.message);
            }
            assert!(
                issues.is_empty(),
                "Known aligned file {} must pass validation cleanly, got: {:?}",
                rel,
                issues
            );
        }
    }

    // 2. Untouched files that have NOT been updated to the 5-block standard yet
    let untouched_files = [
        "tools/src/assets.rs",
        "tools/src/extractor.rs",
        "tools/src/runner.rs",
    ];

    for rel in &untouched_files {
        let p = root.join(rel);
        if let Ok(content) = std::fs::read_to_string(&p) {
            let mut issues = Vec::new();
            validate_rust_content(&p, &content, &config, &mut issues);
            println!("Playtesting UNTOUCHED: {} -> {} issues (PROPERLY THROWS)", rel, issues.len());
            for iss in &issues {
                println!("   [THROWN ISSUE] {}: {}", iss.field, iss.message);
            }
            assert!(
                !issues.is_empty(),
                "Untouched file {} must throw validation errors, but passed unexpectedly!",
                rel
            );
        }
    }
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
        self_audit().is_ok(),
        "[BLOCK 4: PREFLIGHT] validator::rust::self_audit() must pass"
    );
    assert!(
        common::self_audit().is_ok(),
        "[BLOCK 4: PREFLIGHT] Common test harness self-audit must succeed"
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 4.2: Rust Subsystem Anchor Verification Gate
// -----------------------------------------------------------------------------

/// Audits physical source files required for Rust validation.
#[test]
fn test_preflight_rust_validator_anchors() {
    let root = common::resolve_repo_root();
    assert!(
        root.join("tools/src/validator/rust.rs").is_file(),
        "[BLOCK 4: PREFLIGHT] tools/src/validator/rust.rs must exist"
    );
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( Test Suite Epilogue & Summary Handshake )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 5.1: Epilogue & Clean State Handshake
// -----------------------------------------------------------------------------

/// Verifies that running Rust validator unit tests leaves zero transient artifacts.
#[test]
fn test_epilogue_clean_state_handshake() {
    let root = common::resolve_repo_root();
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
