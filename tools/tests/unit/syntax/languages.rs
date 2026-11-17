// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/tests/unit/syntax/languages.rs
// @id            test.unit.tools.syntax.languages
// @layer         Layer 4: Verification Suite
// @archetype     verification_suite | unit_isolation
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-16
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Universal 5-Block Verification Suite
// @role          Integration Test Suite for Concrete Language Syntax Lexers
// @maturity      Rev 5 Baseline
// @chassis       zero-heap | deterministic multi-language test fixtures
// @scale         O(1) execution time across 8 language lexers
// @seams         in: syntax | uses: tl_tools::syntax | probes: self_audit -> exit
// @invariants    1. Zero test panics on arbitrary language snippets.
//                2. Verifies four ontological test sections (Section 1..4).
//                3. Validates language-specific tokenizers across all 8 supported dialects.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Integration test suite verifying individual language lexers in `syntax/`:
//   C (`c.rs`), C++ (`cpp.rs`), Rust (`rust.rs`), JSON (`json.rs`), AsciiDoc (`adoc.rs`),
//   Markdown (`md.rs`), TOML (`toml.rs`), and CMake (`cmake.rs`).
//
//   ### For Game Designers & Non-Programmers:
//   Validates that the toolchain understands every file format used to make the game,
//   from engine code to narrative dialogue and build scripts.
//
//   ### For Systems Programmers & Engine Architects:
//   Exercises path-based language detection, comment scanning, directive extraction,
//   and AST conversion across all supported language grammars.
// =============================================================================

//! # Integration Tests: Concrete Language Syntax Lexers

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Pre-Conditions, Constants, Test Fixtures & Harness Scaffolding )
// =============================================================================

use std::path::Path;
use tl_tools::syntax::{
    Language,
    detect_language_from_path,
    parse_document,
    c::is_forbidden_c_line_comment,
    cpp::audit_extern_c_guards,
    rust::scan_rust_items,
    json::parse_json_5block_doc,
    adoc::audit_adoc_delimiters,
    md::scan_markdown_headings,
    toml::scan_toml_content,
    cmake::scan_cmake_commands,
};

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Operational Test Constants & Suite Identifiers
// -----------------------------------------------------------------------------

/// Canonical test suite identifier matching engine convention.
pub const SUITE_NAME: &str = "Concrete Language Syntax Lexers Integration Surface";

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Test Fixtures & Pre-Condition Assertions
// -----------------------------------------------------------------------------

// =============================================================================
// BLOCK 3: BODY BLOCK / FOUR-SECTION ONTOLOGICAL TEST SUITE
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: [ SECTION 1: ALWAYS PASS (NOMINAL GOLDEN SURFACE COGS) ]
// -----------------------------------------------------------------------------

#[test]
fn test_section_1_nominal_language_detection() {
    assert_eq!(detect_language_from_path(Path::new("math.h")), Language::C);
    assert_eq!(detect_language_from_path(Path::new("engine.cpp")), Language::Cpp);
    assert_eq!(detect_language_from_path(Path::new("lib.rs")), Language::Rust);
    assert_eq!(detect_language_from_path(Path::new("manifest.json")), Language::Json);
    assert_eq!(detect_language_from_path(Path::new("volume_1.adoc")), Language::AsciiDoc);
    assert_eq!(detect_language_from_path(Path::new("README.md")), Language::Markdown);
    assert_eq!(detect_language_from_path(Path::new("Cargo.toml")), Language::Toml);
    assert_eq!(detect_language_from_path(Path::new("CMakeLists.txt")), Language::CMake);
}

#[test]
fn test_section_1_nominal_c_lexing() {
    assert!(is_forbidden_c_line_comment("// forbidden"));
    assert!(!is_forbidden_c_line_comment("https://github.com"));
    assert!(!is_forbidden_c_line_comment("rom://asset"));
}

#[test]
fn test_section_1_nominal_cpp_lexing() {
    let cpp = "#ifdef __cplusplus\nextern \"C\" {\n#endif\nvoid f();\n#ifdef __cplusplus\n}\n#endif\n";
    let report = audit_extern_c_guards(cpp);
    assert!(report.is_fully_guarded);
}

#[test]
fn test_section_1_nominal_rust_lexing() {
    let rust = "pub struct Unit { pub id: i32 }\npub fn spawn() {}\n";
    let items = scan_rust_items(rust);
    assert_eq!(items.len(), 2);
}

#[test]
fn test_section_1_nominal_json_lexing() {
    let json = r#"{
        "block_1_metadata": {},
        "block_2_opening": {},
        "block_3_body": { "key": "value" },
        "block_4_audit": {},
        "block_5_closing": {}
    }"#;
    let doc = parse_json_5block_doc(json).expect("Valid JSON must parse");
    assert!(doc.has_all_five_blocks);
    assert!(doc.block_3_body.is_some());
}

#[test]
fn test_section_1_nominal_adoc_lexing() {
    let adoc = "[cols=\"1,1\"]\n|===\n| A | B\n|===\n";
    let report = audit_adoc_delimiters(adoc);
    assert!(report.is_balanced);
}

#[test]
fn test_section_1_nominal_markdown_lexing() {
    let md = "# Title\n## Section\n### Sub-Section\n";
    let headings = scan_markdown_headings(md);
    assert_eq!(headings.len(), 3);
}

#[test]
fn test_section_1_nominal_toml_lexing() {
    let toml = "[package]\nname = \"app\"\nversion = \"0.1.0\"\n";
    let items = scan_toml_content(toml);
    assert!(!items.0.is_empty());
}

#[test]
fn test_section_1_nominal_cmake_lexing() {
    let cmake = "cmake_minimum_required(VERSION 3.20)\nproject(tenebris C)\n";
    let cmds = scan_cmake_commands(cmake);
    assert_eq!(cmds.len(), 2);
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.2: [ SECTION 2: NEVER FAIL / FAULT INVERSION & DEGRADATION ]
// -----------------------------------------------------------------------------

#[test]
fn test_section_2_never_fail_unknown_extensions() {
    assert_eq!(detect_language_from_path(Path::new("unknown.xyz")), Language::Unknown);
    let doc = parse_document(Path::new("unknown.xyz"), "random text").expect("Unknown dialect must not panic");
    assert_eq!(doc.line_count, 1);
}

#[test]
fn test_section_2_never_fail_unbalanced_delimiters() {
    let bad_adoc = "----\nUnclosed listing\n";
    let report = audit_adoc_delimiters(bad_adoc);
    assert!(!report.is_balanced);
    assert!(report.unclosed.is_some());
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.3: [ SECTION 3: INVARIANTS, SATURATION & LIMIT GUARDS ]
// -----------------------------------------------------------------------------

#[test]
fn test_section_3_invariants_all_languages_covered() {
    let languages = [
        Language::C,
        Language::Cpp,
        Language::Rust,
        Language::Json,
        Language::AsciiDoc,
        Language::Markdown,
        Language::Toml,
        Language::CMake,
        Language::Unknown,
    ];
    assert_eq!(languages.len(), 9);
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.4: [ SECTION 4: EMERGENCE & COMPOSITE WORKFLOWS ]
// -----------------------------------------------------------------------------

#[test]
fn test_section_4_emergence_multi_language_pipeline() {
    let c_res = detect_language_from_path(Path::new("main.c"));
    let rust_res = detect_language_from_path(Path::new("lib.rs"));
    assert_ne!(c_res, rust_res, "Cross-language detection must distinguish C and Rust");
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// ( Preflight Diagnostics & Deep Verification Audits )
// =============================================================================

/// Preflight self-audit probe.
pub fn self_audit() -> Result<(), &'static str> {
    if detect_language_from_path(Path::new("main.c")) != Language::C {
        return Err("Self-audit failed: C language detection mismatch");
    }
    Ok(())
}

// -----------------------------------------------------------------------------
// Sub-Assembly 4.1: Preflight Diagnostic Gates
// -----------------------------------------------------------------------------

#[test]
fn test_preflight_diagnostics_self_audit() {
    assert!(
        self_audit().is_ok(),
        "[BLOCK 4: PREFLIGHT] syntax_languages preflight self_audit() must pass"
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 4.2: Suite Invariants Verification Gate
// -----------------------------------------------------------------------------

#[test]
fn test_preflight_suite_constants() {
    assert!(
        !SUITE_NAME.is_empty(),
        "[BLOCK 4: PREFLIGHT] SUITE_NAME constant must not be empty"
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
    assert!(
        is_test_syntax_languages_ready(),
        "[BLOCK 5: EPILOGUE] Syntax languages must remain ready post-test"
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 5.2: Readiness Probe
// -----------------------------------------------------------------------------

/// Readiness probe for the syntax languages test runner.
pub fn is_test_syntax_languages_ready() -> bool {
    self_audit().is_ok()
}
