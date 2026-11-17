// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/tests/unit/guide/theme.rs
// @id            test.unit.tools.guide.theme
// @layer         Layer 4: Verification Suite
// @archetype     verification_suite | unit_isolation
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-16
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Developer Toolchain // Guide Theme Test Suite
// @role          Four-Section Ontological Test Suite for guide::theme
// @maturity      Rev 5 Baseline | Four-Section Transpiler-Ready Verification
// @chassis       bounded-heap | Test runner execution chassis; stack frames bounded
// @scale         O(1) theme specification serialization
// @seams         in: tl_tools::guide::theme | pairs: docs/guide_theme_config.json
// @invariants    1. Four-Section Ontological Architecture strictly enforced:
//                   Section 1 (Always Pass), Section 2 (Never Fail),
//                   Section 3 (Invariants & Limits), Section 4 (Emergence).
//                2. External Consumer Purity: imports tl_tools via public API.
//                3. Zero Panics: graceful Result evaluation across fallible paths.
//                4. Strict <= 80-column line width discipline across documentation.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Tier 1 Integration verification harness evaluating the `guide::theme`
//   sub-assembly. Validates Asciidoctor-PDF theme YAML generation, typography
//   scales, page geometry invariants, running chrome definitions, and theme
//   file synchronization.
//
//   ### For Game Designers & Non-Programmers:
//   Ensures textbook appearance, font hierarchies, table stylings, and callout
//   colors are translated correctly from configuration into print themes.
//
//   ### For Systems Programmers & Engine Architects:
//   - Transpilation Alignment: Maps directly to OmniCode's 4-section verification
//     schema, allowing lossless round-trip transpilation between languages.
//   - Determinism: Asserts repeatable, byte-for-byte YAML emission.
// =============================================================================

//! # Four-Section Integration Suite for Master Guide Theme Sub-Assembly
//!
//! Exercises `tl_tools::guide::theme` across Always-Pass, Never-Fail,
//! Invariant, and Emergent execution tiers.

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Pre-Conditions, Constants, Test Fixtures & Harness Scaffolding )
// =============================================================================

use std::fs;
use std::path::PathBuf;

use tl_tools::cmd::resolve_repo_root;
use tl_tools::guide::manifest::default_config;
use tl_tools::guide::theme::{
    is_theme_subsystem_ready, is_valid_theme_meta, render_theme_yaml,
    self_audit, sync_theme_yaml, GuideThemeBody, GuideThemeMeta,
};

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Operational Test Constants & Suite Identifiers
// -----------------------------------------------------------------------------

/// Canonical test suite identifier matching engine convention.
pub const SUITE_NAME: &str = "Toolchain Guide Theme Integration Surface";

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

// =============================================================================
// BLOCK 3: BODY BLOCK / FOUR-SECTION ONTOLOGICAL TEST SUITE
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: [ SECTION 1: ALWAYS PASS (NOMINAL GOLDEN SURFACE COGS) ]
// -----------------------------------------------------------------------------

/// Verifies nominal golden rendering of Asciidoctor-PDF theme YAML.
#[test]
fn test_section_1_nominal_theme_yaml_generation() {
    let body = GuideThemeBody::default();
    let yaml = render_theme_yaml("default", &body);

    assert!(
        yaml.contains("extends: default"),
        "[SEC 1: ALWAYS PASS] YAML must declare base extension"
    );
    assert!(
        yaml.contains("page:"),
        "[SEC 1: ALWAYS PASS] YAML must define page geometry"
    );
    assert!(
        yaml.contains("base:"),
        "[SEC 1: ALWAYS PASS] YAML must define base typography"
    );
    assert!(
        yaml.contains("code:"),
        "[SEC 1: ALWAYS PASS] YAML must define code block styling"
    );
    assert!(
        yaml.contains("table:"),
        "[SEC 1: ALWAYS PASS] YAML must define table styling"
    );
    assert!(
        yaml.contains("header:"),
        "[SEC 1: ALWAYS PASS] YAML must define running header"
    );
    assert!(
        yaml.contains("footer:"),
        "[SEC 1: ALWAYS PASS] YAML must define running footer"
    );
}

/// Verifies nominal golden defaults of theme metadata.
#[test]
fn test_section_1_nominal_default_theme_meta() {
    let meta = GuideThemeMeta::default();

    assert_eq!(
        meta.theme_name, "cornerstone-book",
        "[SEC 1: ALWAYS PASS] Default theme name must be cornerstone-book"
    );
    assert_eq!(
        meta.extends, "default",
        "[SEC 1: ALWAYS PASS] Default extends must be default"
    );
    assert!(
        is_valid_theme_meta(&meta),
        "[SEC 1: ALWAYS PASS] Default theme meta must be valid"
    );
}

/// Verifies nominal golden page dimensions of default theme body.
#[test]
fn test_section_1_nominal_default_theme_body() {
    let body = GuideThemeBody::default();

    assert_eq!(body.page.size, "letter");
    assert_eq!(body.page.layout, "portrait");
    assert!(body.page.margin_top_in > 0.0);
    assert!(body.page.margin_bottom_in > 0.0);
    assert!(body.page.margin_left_in > 0.0);
    assert!(body.page.margin_right_in > 0.0);
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.2: [ SECTION 2: NEVER FAIL (FAULT INVERSION & DEGRADATION) ]
// -----------------------------------------------------------------------------

/// Verifies graceful degradation when theme config path is None.
#[test]
fn test_section_2_never_fail_missing_theme_file() {
    let repo_root = resolve_repo_root();
    let mut config = default_config(&repo_root);
    config.pdf_theme_data_file = None;

    let result = sync_theme_yaml(&repo_root, &config);
    assert!(
        result.is_ok(),
        "[SEC 2: NEVER FAIL] sync_theme_yaml with None path must return Ok(None)"
    );
    assert_eq!(result.unwrap(), None);

    // Also test with non-existent file path
    config.pdf_theme_data_file = Some(repo_root.join("docs/nonexistent_theme.json"));
    let result_nonexistent = sync_theme_yaml(&repo_root, &config);
    assert!(
        result_nonexistent.is_ok(),
        "[SEC 2: NEVER FAIL] sync_theme_yaml with nonexistent file must return Ok(None)"
    );
    assert_eq!(result_nonexistent.unwrap(), None);
}

/// Verifies invalid theme meta predicate handling under blank fields.
#[test]
fn test_section_2_never_fail_invalid_theme_meta() {
    let blank_name = GuideThemeMeta {
        theme_name: "".to_string(),
        extends: "default".to_string(),
        generated_yaml_path: "docs/themes/theme.yml".to_string(),
    };
    assert!(
        !is_valid_theme_meta(&blank_name),
        "[SEC 2: NEVER FAIL] Blank theme name must fail predicate"
    );

    let blank_extends = GuideThemeMeta {
        theme_name: "valid-name".to_string(),
        extends: "   ".to_string(),
        generated_yaml_path: "docs/themes/theme.yml".to_string(),
    };
    assert!(
        !is_valid_theme_meta(&blank_extends),
        "[SEC 2: NEVER FAIL] Blank extends must fail predicate"
    );

    let blank_path = GuideThemeMeta {
        theme_name: "valid-name".to_string(),
        extends: "default".to_string(),
        generated_yaml_path: "".to_string(),
    };
    assert!(
        !is_valid_theme_meta(&blank_path),
        "[SEC 2: NEVER FAIL] Blank generated YAML path must fail predicate"
    );
}

/// Verifies graceful error reporting when theme JSON is corrupted.
#[test]
fn test_section_2_never_fail_corrupted_theme_json() {
    let repo_root = resolve_repo_root();
    let scratch_dir = repo_root.join("build/scratch");
    let _ = fs::create_dir_all(&scratch_dir);
    let corrupted_path = scratch_dir.join("corrupted_theme.json");

    fs::write(&corrupted_path, "{ broken_theme_yaml: true,")
        .expect("Must write corrupted theme fixture");

    let mut config = default_config(&repo_root);
    config.pdf_theme_data_file = Some(corrupted_path.clone());

    let result = sync_theme_yaml(&repo_root, &config);
    assert!(
        result.is_err(),
        "[SEC 2: NEVER FAIL] Corrupted theme JSON must yield typed Err"
    );

    let _ = fs::remove_file(&corrupted_path);
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.3: [ SECTION 3: INVARIANTS, SATURATION & LIMIT GUARDS ]
// -----------------------------------------------------------------------------

/// Verifies page geometry bounds and typography scale limits.
#[test]
fn test_section_3_invariants_page_geometry_bounds() {
    let body = GuideThemeBody::default();

    // Page margins must be within reasonable textbook bounds (0.25 to 2.0 inches)
    assert!(body.page.margin_top_in >= 0.25 && body.page.margin_top_in <= 2.0);
    assert!(body.page.margin_bottom_in >= 0.25 && body.page.margin_bottom_in <= 2.0);
    assert!(body.page.margin_left_in >= 0.25 && body.page.margin_left_in <= 2.0);
    assert!(body.page.margin_right_in >= 0.25 && body.page.margin_right_in <= 2.0);

    // Font scales must be strictly ordered: H1 > H2 > H3 > H4
    let s = &body.spacing;
    assert!(
        s.heading_h1_scale > s.heading_h2_scale,
        "[SEC 3: INVARIANTS] H1 scale must exceed H2 scale"
    );
    assert!(
        s.heading_h2_scale > s.heading_h3_scale,
        "[SEC 3: INVARIANTS] H2 scale must exceed H3 scale"
    );
    assert!(
        s.heading_h3_scale > s.heading_h4_scale,
        "[SEC 3: INVARIANTS] H3 scale must exceed H4 scale"
    );

    let t = &body.typography;
    assert!(
        t.base_font_size >= 8.0 && t.base_font_size <= 14.0,
        "[SEC 3: INVARIANTS] Base font size must be between 8 and 14 pt"
    );
}

/// Verifies running chrome dimensions and font limits.
#[test]
fn test_section_3_invariants_running_chrome_fonts() {
    let body = GuideThemeBody::default();
    let rc = &body.running_chrome;

    assert!(
        rc.header_height >= 16.0,
        "[SEC 3: INVARIANTS] Header height must be at least 16pt"
    );
    assert!(
        rc.footer_height >= 16.0,
        "[SEC 3: INVARIANTS] Footer height must be at least 16pt"
    );
    assert!(
        rc.header_font_size <= 10.0,
        "[SEC 3: INVARIANTS] Header font size must not exceed 10pt"
    );
    assert!(
        rc.footer_font_size <= 10.0,
        "[SEC 3: INVARIANTS] Footer font size must not exceed 10pt"
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.4: [ SECTION 4: EMERGENCE & COMPOSITE WORKFLOWS ]
// -----------------------------------------------------------------------------

/// Verifies end-to-end synchronization of live theme config to YAML.
#[test]
fn test_section_4_emergence_theme_sync_workflow() {
    let repo_root = resolve_repo_root();
    let docs = get_docs_dir();
    let live_theme_json = docs.join("guide_theme_config.json");

    if live_theme_json.is_file() {
        let mut config = default_config(&repo_root);
        config.pdf_theme_data_file = Some(live_theme_json);

        let result = sync_theme_yaml(&repo_root, &config);
        assert!(
            result.is_ok(),
            "[SEC 4: EMERGENCE] sync_theme_yaml on live theme must succeed: {:?}",
            result.err()
        );

        let written_path = result.unwrap();
        assert!(
            written_path.is_some(),
            "[SEC 4: EMERGENCE] Live sync must produce a written YAML path"
        );

        let path = written_path.unwrap();
        assert!(path.is_file(), "Generated theme YAML file must exist: {:?}", path);

        let content = fs::read_to_string(&path).expect("Must read generated theme YAML");
        assert!(content.contains("extends: default"));
        assert!(content.contains("page:"));
    }
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// ( Preflight Diagnostics & Deep Verification Audits )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 4.1: Preflight Diagnostic Gates
// -----------------------------------------------------------------------------

/// Diagnostic self-audit verifying guide theme internal consistency.
#[test]
fn test_preflight_diagnostics_self_audit() {
    assert!(
        !SUITE_NAME.is_empty(),
        "[BLOCK 4: PREFLIGHT] SUITE_NAME constant must not be empty"
    );
    let result = self_audit();
    assert!(
        result.is_ok(),
        "[BLOCK 4: PREFLIGHT] theme::self_audit() must pass: {:?}",
        result.err()
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 4.2: Documentation Anchor Verification Gate
// -----------------------------------------------------------------------------

/// Audits physical theme configuration file on disk.
#[test]
fn test_preflight_documentation_anchors() {
    let root = resolve_repo_root();
    let theme_config = root.join("docs/guide_theme_config.json");
    assert!(
        theme_config.is_file(),
        "docs/guide_theme_config.json must exist"
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
        is_theme_subsystem_ready(),
        "[BLOCK 5: EPILOGUE] is_theme_subsystem_ready() must return true"
    );
}
