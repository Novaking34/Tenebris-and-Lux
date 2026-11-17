// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/tests/unit/guide/compiler.rs
// @id            test.unit.tools.guide.compiler
// @layer         Layer 4: Verification Suite
// @archetype     verification_suite | unit_isolation
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-16
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Developer Toolchain // Guide Compiler Test Suite
// @role          Four-Section Ontological Test Suite for guide::compiler
// @maturity      Rev 5 Baseline | Four-Section Transpiler-Ready Verification
// @chassis       bounded-heap | Test runner execution chassis; stack frames bounded
// @scale         O(L + F) across document lines and includes
// @seams         in: tl_tools::guide::compiler | pairs: docs/
// @invariants    1. Four-Section Ontological Architecture strictly enforced:
//                   Section 1 (Always Pass), Section 2 (Never Fail),
//                   Section 3 (Invariants & Limits), Section 4 (Emergence).
//                2. External Consumer Purity: imports tl_tools via public API.
//                3. Zero Panics: graceful Result evaluation across fallible paths.
//                4. Strict <= 80-column line width discipline across documentation.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Tier 1 Integration verification harness evaluating the `guide::compiler`
//   sub-assembly. Validates AsciiDoc heading shifts, header stripping, include
//   expansion, fence preservation invariants, AsciiDoc-to-Markdown translation,
//   and area chapter scanning.
//
//   ### For Game Designers & Non-Programmers:
//   Guarantees that lore and systems documents written as independent files
//   merge seamlessly into multi-volume books with correct heading hierarchies.
//
//   ### For Systems Programmers & Engine Architects:
//   - Transpilation Alignment: Maps directly to OmniCode's 4-section verification
//     schema, allowing lossless round-trip transpilation between languages.
//   - Verbatim Invariant: Asserts that code fences and comments are untouched.
// =============================================================================

//! # Four-Section Integration Suite for Master Guide Compiler Sub-Assembly
//!
//! Exercises `tl_tools::guide::compiler` across Always-Pass, Never-Fail,
//! Invariant, and Emergent execution tiers.

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Pre-Conditions, Constants, Test Fixtures & Harness Scaffolding )
// =============================================================================

use std::path::{Path, PathBuf};

use tl_tools::cmd::resolve_repo_root;
use tl_tools::guide::compiler::{
    adoc_to_markdown, extract_title_from_file, is_compiler_subsystem_ready,
    is_fence_delimiter, scan_area_for_chapters, self_audit,
    shift_section_levels, strip_adoc_header, MAX_INCLUDE_DEPTH,
};

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Operational Test Constants & Suite Identifiers
// -----------------------------------------------------------------------------

/// Canonical test suite identifier matching engine convention.
pub const SUITE_NAME: &str = "Toolchain Guide Compiler Integration Surface";

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

/// Verifies nominal golden section level shifting by positive deltas.
#[test]
fn test_section_1_nominal_heading_level_shifts() {
    let input = "== Section 1\n=== Subsection 1.1\nParagraph text";
    let shifted = shift_section_levels(input, 1, false);

    assert!(shifted.contains("=== Section 1"));
    assert!(shifted.contains("==== Subsection 1.1"));
    assert!(shifted.contains("Paragraph text"));

    // Shift by additional level with number stripping option
    let shifted_twice = shift_section_levels(&shifted, 1, true);
    assert!(shifted_twice.contains("==== Section 1"));
    assert!(shifted_twice.contains("===== Subsection 1.1"));
}

/// Verifies nominal golden header stripping of title, metadata, and attributes.
#[test]
fn test_section_1_nominal_strip_adoc_header() {
    let raw = "////\nBLOCK 1: METADATA BLOCK\n////\n= Document Title\nAuthor Name\nv1.0\n:toc: left\n\n== Section 1\nBody text";
    let stripped = strip_adoc_header(raw);

    assert!(
        stripped.starts_with("== Section 1"),
        "Stripped document must begin at first body section: {:?}",
        stripped
    );
    assert!(!stripped.contains("Document Title"));
    assert!(!stripped.contains(":toc:"));
    assert!(stripped.contains("Body text"));
}

/// Verifies nominal golden delimiter detection across code and comment fences.
#[test]
fn test_section_1_nominal_fence_delimiter_detection() {
    assert!(is_fence_delimiter("----"));
    assert!(is_fence_delimiter("------"));
    assert!(is_fence_delimiter("...."));
    assert!(is_fence_delimiter("////"));
    assert!(is_fence_delimiter("===="));
    assert!(is_fence_delimiter("****"));
    assert!(is_fence_delimiter("|==="));
    assert!(is_fence_delimiter("--"));
    assert!(is_fence_delimiter("```"));
    assert!(is_fence_delimiter("```rust"));

    assert!(!is_fence_delimiter("== Heading"));
    assert!(!is_fence_delimiter("regular text"));
    assert!(!is_fence_delimiter("---"));
}

/// Verifies nominal golden translation from AsciiDoc to Markdown.
#[test]
fn test_section_1_nominal_adoc_to_markdown() {
    let adoc = "= Document\n\n== Heading 2\n\n=== Heading 3\n\n* bold text *\n_ italic text _\n\n[source,c]\n----\nint x = 0;\n----\n";
    let md = adoc_to_markdown(adoc);

    assert!(md.contains("# Document") || md.contains("## Heading 2"));
    assert!(md.contains("```c"));
    assert!(md.contains("int x = 0;"));
    assert!(md.contains("```"));
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.2: [ SECTION 2: NEVER FAIL (FAULT INVERSION & DEGRADATION) ]
// -----------------------------------------------------------------------------

/// Verifies graceful handling of empty strings across all compiler transforms.
#[test]
fn test_section_2_never_fail_empty_document() {
    assert_eq!(strip_adoc_header(""), "");
    assert_eq!(shift_section_levels("", 2, false), "");
    assert_eq!(shift_section_levels("", 0, true), "");
    assert_eq!(adoc_to_markdown(""), "");
}

/// Verifies graceful degradation when scanning a non-existent documentation area.
#[test]
fn test_section_2_never_fail_nonexistent_scan_area() {
    let docs = get_docs_dir();
    let chapters = scan_area_for_chapters(&docs, "fictional_nonexistent_area_xyz");

    assert!(
        chapters.is_empty(),
        "[SEC 2: NEVER FAIL] Scanning nonexistent directory must return empty Vec"
    );
}

/// Verifies graceful fallback when extracting title from a missing file.
#[test]
fn test_section_2_never_fail_extract_title_missing_file() {
    let title = extract_title_from_file(Path::new("missing_chapter_file.adoc"));

    assert_eq!(
        title, "missing chapter file",
        "[SEC 2: NEVER FAIL] Missing file must fall back to cleaned stem"
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.3: [ SECTION 3: INVARIANTS, SATURATION & LIMIT GUARDS ]
// -----------------------------------------------------------------------------

/// Verifies verbatim fence preservation: code lines inside fences are never shifted.
#[test]
fn test_section_3_invariants_fence_preservation_during_shifts() {
    let doc_with_code = "\
== Section Heading
[source,c]
----
=== Not A Heading (Verbatim Code)
int x = 10;
----
=== Valid Subsection
Text here";

    let shifted = shift_section_levels(doc_with_code, 1, false);

    assert!(
        shifted.contains("=== Not A Heading (Verbatim Code)"),
        "[SEC 3: INVARIANTS] Lines inside verbatim code fence must not be shifted"
    );
    assert!(
        shifted.contains("=== Section Heading"),
        "[SEC 3: INVARIANTS] Heading outside fence must be shifted"
    );
    assert!(
        shifted.contains("==== Valid Subsection"),
        "[SEC 3: INVARIANTS] Subsection outside fence must be shifted"
    );
}

/// Verifies comment preservation: comment lines are never shifted as headings.
#[test]
fn test_section_3_invariants_comment_preservation_during_shifts() {
    let doc_with_comments = "\
== Section
// == Comment That Looks Like A Heading
Body text";

    let shifted = shift_section_levels(doc_with_comments, 1, false);

    assert!(
        shifted.contains("// == Comment That Looks Like A Heading"),
        "[SEC 3: INVARIANTS] Comments must never be transformed by heading shift"
    );
}

/// Verifies include depth limit constant bounds.
#[test]
fn test_section_3_invariants_max_include_depth() {
    assert!(
        MAX_INCLUDE_DEPTH >= 4,
        "[SEC 3: INVARIANTS] MAX_INCLUDE_DEPTH must be at least 4"
    );
    assert!(
        MAX_INCLUDE_DEPTH <= 10,
        "[SEC 3: INVARIANTS] MAX_INCLUDE_DEPTH must not exceed 10 to avoid stack limits"
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.4: [ SECTION 4: EMERGENCE & COMPOSITE WORKFLOWS ]
// -----------------------------------------------------------------------------

/// Verifies live area chapter discovery across architecture and lore directories.
#[test]
fn test_section_4_emergence_live_chapter_scanning() {
    let docs = get_docs_dir();

    // 1. Scan architecture documentation area
    let arch_chapters = scan_area_for_chapters(&docs, "architecture");
    assert!(
        !arch_chapters.is_empty(),
        "[SEC 4: EMERGENCE] Architecture area must yield chapters"
    );

    for (title, path) in &arch_chapters {
        assert!(
            !title.trim().is_empty(),
            "[SEC 4: EMERGENCE] Discovered chapter must have non-empty title"
        );
        assert!(
            path.is_file(),
            "[SEC 4: EMERGENCE] Chapter path must exist on disk: {:?}",
            path
        );
    }

    // 2. Scan lore documentation area
    let lore_chapters = scan_area_for_chapters(&docs, "lore");
    assert!(
        !lore_chapters.is_empty(),
        "[SEC 4: EMERGENCE] Lore area must yield chapters"
    );
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// ( Preflight Diagnostics & Deep Verification Audits )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 4.1: Preflight Diagnostic Gates
// -----------------------------------------------------------------------------

/// Diagnostic self-audit verifying guide compiler internal consistency.
#[test]
fn test_preflight_diagnostics_self_audit() {
    assert!(
        !SUITE_NAME.is_empty(),
        "[BLOCK 4: PREFLIGHT] SUITE_NAME constant must not be empty"
    );
    let result = self_audit();
    assert!(
        result.is_ok(),
        "[BLOCK 4: PREFLIGHT] compiler::self_audit() must pass: {:?}",
        result.err()
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 4.2: Documentation Anchor Verification Gate
// -----------------------------------------------------------------------------

/// Audits physical documentation directory anchors.
#[test]
fn test_preflight_documentation_anchors() {
    let root = resolve_repo_root();
    assert!(root.join("docs/README.adoc").is_file());
    assert!(root.join("docs/architecture/ARCHITECTURE.adoc").is_file());
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
        is_compiler_subsystem_ready(),
        "[BLOCK 5: EPILOGUE] is_compiler_subsystem_ready() must return true"
    );
}
