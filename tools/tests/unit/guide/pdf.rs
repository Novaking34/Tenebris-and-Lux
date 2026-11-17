// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/tests/unit/guide/pdf.rs
// @id            test.unit.tools.guide.pdf
// @layer         Layer 4: Verification Suite
// @archetype     verification_suite | unit_isolation
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-16
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Developer Toolchain // Guide PDF Test Suite
// @role          Four-Section Ontological Test Suite for guide::pdf
// @maturity      Rev 5 Baseline | Four-Section Transpiler-Ready Verification
// @chassis       bounded-heap | Test runner execution chassis; stack frames bounded
// @scale         O(D * L) across documents and line lengths
// @seams         in: tl_tools::guide::pdf | pairs: docs/adoc_pdf_layout_rules.json
// @invariants    1. Four-Section Ontological Architecture strictly enforced:
//                   Section 1 (Always Pass), Section 2 (Never Fail),
//                   Section 3 (Invariants & Limits), Section 4 (Emergence).
//                2. External Consumer Purity: imports tl_tools via public API.
//                3. Zero Panics: graceful Result evaluation across fallible paths.
//                4. Strict <= 80-column line width discipline across documentation.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Tier 1 Integration verification harness evaluating the `guide::pdf`
//   sub-assembly. Validates AsciiDoc PDF layout rules loading, single-page
//   geometry budgets, delimiter stack balance, strict 5-block banner checks,
//   and line width discipline enforcement.
//
//   ### For Game Designers & Non-Programmers:
//   Prevents awkward page splits, clipped tables, and unclosed code fences in
//   the published PDF technical manuals and rulebooks.
//
//   ### For Systems Programmers & Engine Architects:
//   - Transpilation Alignment: Maps directly to OmniCode's 4-section verification
//     schema, allowing lossless round-trip transpilation between languages.
//   - Strict Linting: Enforces 80-column text and 128-column hard code limits.
// =============================================================================

//! # Four-Section Integration Suite for Master Guide PDF Sub-Assembly
//!
//! Exercises `tl_tools::guide::pdf` across Always-Pass, Never-Fail,
//! Invariant, and Emergent execution tiers.

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Pre-Conditions, Constants, Test Fixtures & Harness Scaffolding )
// =============================================================================

use std::path::{Path, PathBuf};

use tl_tools::cmd::resolve_repo_root;
use tl_tools::guide::pdf::{
    is_pdf_subsystem_ready, is_strict_layout_target, load_layout_rules,
    self_audit, validate_adoc_text, AdocLayoutReport, AdocLayoutRulesFile,
    AdocPageGeometry, LayoutSeverity,
};

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Operational Test Constants & Suite Identifiers
// -----------------------------------------------------------------------------

/// Canonical test suite identifier matching engine convention.
pub const SUITE_NAME: &str = "Toolchain Guide PDF Integration Surface";

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Test Fixtures & Pre-Condition Assertions
// -----------------------------------------------------------------------------

/// Resolves the live workspace docs directory.
fn get_docs_dir() -> PathBuf {
    let repo_root = resolve_repo_root();
    let docs = repo_root.join("docs");
    assert!(
        docs.is_dir(),
        "Repository root {:?} must host docs/ directory",
        repo_root
    );
    docs
}

/// Creates a valid synthetic 5-block AsciiDoc document for golden testing.
fn create_valid_5block_adoc() -> String {
    "\
////
BLOCK 1: METADATA BLOCK
@project Tenebris and Lux
////
= Document Title

////
BLOCK 2: OPENING BLOCK
////
Opening content.

////
BLOCK 3: BODY BLOCK
////
Body content.

////
BLOCK 4: AUDIT PROCEDURES BLOCK
////
Audit content.

////
BLOCK 5: CLOSING BLOCK
////
Closing content.
"
    .to_string()
}

// =============================================================================
// BLOCK 3: BODY BLOCK / FOUR-SECTION ONTOLOGICAL TEST SUITE
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: [ SECTION 1: ALWAYS PASS (NOMINAL GOLDEN SURFACE COGS) ]
// -----------------------------------------------------------------------------

/// Verifies nominal golden page geometry defaults.
#[test]
fn test_section_1_nominal_default_page_geometry() {
    let geom = AdocPageGeometry::default();

    assert_eq!(geom.size, "letter");
    assert_eq!(geom.layout, "portrait");
    assert_eq!(geom.text_cols_max, 80);
    assert_eq!(geom.safe_code_cols_with_linenums, 104);
    assert_eq!(geom.hard_code_cols_max, 128);
    assert_eq!(geom.max_lines_per_single_page, 60);
}

/// Verifies nominal golden report state on clean layout audits.
#[test]
fn test_section_1_nominal_clean_report() {
    let report = AdocLayoutReport::default();

    assert!(report.is_clean());
    assert_eq!(report.error_count(), 0);
    assert_eq!(report.warning_count(), 0);
    assert_eq!(report.files_checked, 0);
}

/// Verifies nominal golden strict target path resolution.
#[test]
fn test_section_1_nominal_strict_target_detection() {
    let rules = AdocLayoutRulesFile::default();

    assert!(is_strict_layout_target("docs/README.adoc", &rules));
    assert!(is_strict_layout_target("docs/architecture/ARCHITECTURE.adoc", &rules));
    assert!(!is_strict_layout_target("docs/lore/random_note.adoc", &rules));
    assert!(!is_strict_layout_target("untracked_file.txt", &rules));
}

/// Verifies nominal golden validation of clean 5-block AsciiDoc text.
#[test]
fn test_section_1_nominal_valid_adoc_text_validation() {
    let docs = get_docs_dir();
    let rules = AdocLayoutRulesFile::default();
    let content = create_valid_5block_adoc();

    let issues = validate_adoc_text(
        "docs/architecture/ARCHITECTURE.adoc",
        &docs,
        &content,
        true,
        false,
        &rules,
    );

    let errors: Vec<_> = issues
        .iter()
        .filter(|i| i.severity == LayoutSeverity::Error)
        .collect();

    assert!(
        errors.is_empty(),
        "[SEC 1: ALWAYS PASS] Valid 5-block doc must produce zero errors: {:?}",
        errors
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.2: [ SECTION 2: NEVER FAIL (FAULT INVERSION & DEGRADATION) ]
// -----------------------------------------------------------------------------

/// Verifies graceful fallback to defaults when layout rules file is missing.
#[test]
fn test_section_2_never_fail_missing_layout_rules() {
    let missing_dir = Path::new("fictional_missing_dir_xyz");
    let result = load_layout_rules(missing_dir);

    assert!(
        result.is_ok(),
        "[SEC 2: NEVER FAIL] Missing layout rules must return default struct"
    );
    let rules = result.unwrap();
    assert_eq!(rules.block_3_body.page_geometry.text_cols_max, 80);
}

/// Verifies detection of unclosed verbatim code fences.
#[test]
fn test_section_2_never_fail_unclosed_fence() {
    let docs = get_docs_dir();
    let rules = AdocLayoutRulesFile::default();
    let unclosed = "= Unclosed Code Fence\n\n[source,c]\n----\nint x = 1;\n";

    let issues = validate_adoc_text(
        "sample.adoc",
        &docs,
        unclosed,
        false,
        false,
        &rules,
    );

    let has_unclosed = issues
        .iter()
        .any(|i| i.severity == LayoutSeverity::Error && i.message.contains("Unclosed"));

    assert!(
        has_unclosed,
        "[SEC 2: NEVER FAIL] Unclosed code fence must emit Error finding"
    );
}

/// Verifies strict 5-block validation failure when document lacks leading metadata block.
#[test]
fn test_section_2_never_fail_missing_leading_metadata() {
    let docs = get_docs_dir();
    let rules = AdocLayoutRulesFile::default();
    let no_meta = "= Document Without Metadata Block\n\n== Section 1\nBody text\n";

    let issues = validate_adoc_text(
        "docs/architecture/sample.adoc",
        &docs,
        no_meta,
        true,
        false,
        &rules,
    );

    let has_meta_error = issues
        .iter()
        .any(|i| i.message.contains("BLOCK 1 banner"));

    assert!(
        has_meta_error,
        "[SEC 2: NEVER FAIL] Missing leading metadata must emit finding under strict mode"
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.3: [ SECTION 3: INVARIANTS, SATURATION & LIMIT GUARDS ]
// -----------------------------------------------------------------------------

/// Verifies hard line width discipline: code exceeding hard column limit triggers issue.
#[test]
fn test_section_3_invariants_line_width_discipline() {
    let docs = get_docs_dir();
    let rules = AdocLayoutRulesFile::default();

    // Line with 150 characters inside a code fence
    let long_line = "x".repeat(150);
    let content = format!("= Title\n\n[source,c]\n----\n{}\n----\n", long_line);

    let issues = validate_adoc_text(
        "sample.adoc",
        &docs,
        &content,
        false,
        true,
        &rules,
    );

    let has_overflow_issue = issues
        .iter()
        .any(|i| i.rule == "hard_code_cols_max" || i.message.contains("exceeds hard PDF wrap limit"));

    assert!(
        has_overflow_issue,
        "[SEC 3: INVARIANTS] Code line > 128 chars must trigger overflow issue"
    );
}

/// Verifies ordering of layout severity enum: Error is greater than Warning.
#[test]
fn test_section_3_invariants_severity_ordering() {
    assert!(LayoutSeverity::Error > LayoutSeverity::Warning);
    assert_ne!(LayoutSeverity::Error, LayoutSeverity::Warning);
}

/// Verifies report issue categorization counts.
#[test]
fn test_section_3_invariants_report_counts() {
    let mut report = AdocLayoutReport::default();
    assert_eq!(report.error_count(), 0);
    assert_eq!(report.warning_count(), 0);

    report.issues.push(tl_tools::guide::pdf::AdocLayoutIssue {
        file: "test.adoc".to_string(),
        line: 10,
        severity: LayoutSeverity::Warning,
        rule: "LINE_WIDTH".to_string(),
        message: "Warning message".to_string(),
    });
    assert_eq!(report.error_count(), 0);
    assert_eq!(report.warning_count(), 1);
    assert!(report.is_clean()); // clean only requires 0 errors

    report.issues.push(tl_tools::guide::pdf::AdocLayoutIssue {
        file: "test.adoc".to_string(),
        line: 20,
        severity: LayoutSeverity::Error,
        rule: "FENCE_BALANCE".to_string(),
        message: "Error message".to_string(),
    });
    assert_eq!(report.error_count(), 1);
    assert_eq!(report.warning_count(), 1);
    assert!(!report.is_clean());
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.4: [ SECTION 4: EMERGENCE & COMPOSITE WORKFLOWS ]
// -----------------------------------------------------------------------------

/// Verifies live layout rules loading and live README validation.
#[test]
fn test_section_4_emergence_live_layout_rules() {
    let docs = get_docs_dir();
    let rules_result = load_layout_rules(&docs);

    assert!(
        rules_result.is_ok(),
        "[SEC 4: EMERGENCE] Loading live layout rules must succeed: {:?}",
        rules_result.err()
    );

    let rules = rules_result.unwrap();
    assert_eq!(rules.block_3_body.page_geometry.hard_code_cols_max, 128);
    assert_eq!(rules.block_3_body.page_geometry.safe_code_cols_with_linenums, 104);

    // Validate live README.adoc
    let readme_path = docs.join("README.adoc");
    if readme_path.is_file() {
        let content = std::fs::read_to_string(&readme_path).expect("Must read README.adoc");
        let issues = validate_adoc_text(
            "docs/README.adoc",
            &docs,
            &content,
            true,
            false,
            &rules,
        );

        let errors: Vec<_> = issues
            .iter()
            .filter(|i| i.severity == LayoutSeverity::Error)
            .collect();

        assert!(
            errors.is_empty(),
            "[SEC 4: EMERGENCE] Live README.adoc must have zero layout errors: {:?}",
            errors
        );
    }
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// ( Preflight Diagnostics & Deep Verification Audits )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 4.1: Preflight Diagnostic Gates
// -----------------------------------------------------------------------------

/// Diagnostic self-audit verifying guide PDF internal consistency.
#[test]
fn test_preflight_diagnostics_self_audit() {
    assert!(
        !SUITE_NAME.is_empty(),
        "[BLOCK 4: PREFLIGHT] SUITE_NAME constant must not be empty"
    );
    let result = self_audit();
    assert!(
        result.is_ok(),
        "[BLOCK 4: PREFLIGHT] pdf::self_audit() must pass: {:?}",
        result.err()
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 4.2: Documentation Anchor Verification Gate
// -----------------------------------------------------------------------------

/// Audits physical layout rules file on disk.
#[test]
fn test_preflight_documentation_anchors() {
    let root = resolve_repo_root();
    let rules_path = root.join("docs/adoc_pdf_layout_rules.json");
    assert!(
        rules_path.is_file(),
        "docs/adoc_pdf_layout_rules.json must exist"
    );
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( Test Suite Epilogue & Summary Handshake )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 5.1: Epilogue & Clean State Handshake
// -----------------------------------------------------------------------------

/// Epilogue handshake confirming subsystem readiness.
#[test]
fn test_epilogue_clean_state_handshake() {
    assert!(
        is_pdf_subsystem_ready(),
        "[BLOCK 5: EPILOGUE] is_pdf_subsystem_ready() must return true"
    );
}
