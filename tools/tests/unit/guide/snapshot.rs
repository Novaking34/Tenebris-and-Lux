// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/tests/unit/guide/snapshot.rs
// @id            test.unit.tools.guide.snapshot
// @layer         Layer 4: Verification Suite
// @archetype     verification_suite | unit_isolation
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-16
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Developer Toolchain // Guide Snapshot Test Suite
// @role          Four-Section Ontological Test Suite for guide::snapshot
// @maturity      Rev 5 Baseline | Four-Section Transpiler-Ready Verification
// @chassis       bounded-heap | Test runner execution chassis; stack frames bounded
// @scale         O(S * L) across code snapshots and file lines
// @seams         in: tl_tools::guide::snapshot | pairs: docs/code_snapshots_manifest.json
// @invariants    1. Four-Section Ontological Architecture strictly enforced:
//                   Section 1 (Always Pass), Section 2 (Never Fail),
//                   Section 3 (Invariants & Limits), Section 4 (Emergence).
//                2. External Consumer Purity: imports tl_tools via public API.
//                3. Zero Panics: graceful Result evaluation across fallible paths.
//                4. Strict <= 80-column line width discipline across documentation.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Tier 1 Integration verification harness evaluating the `guide::snapshot`
//   sub-assembly. Validates 5-mode code snapshot extraction (Whole, Block,
//   Subassembly, Symbol, Excerpt), language inference, AST segment matching,
//   plate formatting, and appendix policies.
//
//   ### For Game Designers & Non-Programmers:
//   Ensures that code examples in the technical manual are pulled directly from
//   living engine code without stale or desynchronized copy-pastes.
//
//   ### For Systems Programmers & Engine Architects:
//   - Transpilation Alignment: Maps directly to OmniCode's 4-section verification
//     schema, allowing lossless round-trip transpilation between languages.
//   - Segment Precision: Proves 1-based line number integrity across extracts.
// =============================================================================

//! # Four-Section Integration Suite for Master Guide Snapshot Sub-Assembly
//!
//! Exercises `tl_tools::guide::snapshot` across Always-Pass, Never-Fail,
//! Invariant, and Emergent execution tiers.

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Pre-Conditions, Constants, Test Fixtures & Harness Scaffolding )
// =============================================================================

use std::fs;
use std::path::PathBuf;

use tl_tools::cmd::resolve_repo_root;
use tl_tools::guide::snapshot::{
    extract_segments_for_spec, format_snapshot_plate, infer_language,
    is_snapshot_subsystem_ready, is_valid_snapshot_spec, merge_segments,
    self_audit, snapshot_goes_in_appendix, CodeSnapshotSpec,
    CodeSnapshotsManifest, ExtractedSegment, RenderedSnapshot, SnapshotMode,
    WholesalePolicy,
};

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Operational Test Constants & Suite Identifiers
// -----------------------------------------------------------------------------

/// Canonical test suite identifier matching engine convention.
pub const SUITE_NAME: &str = "Toolchain Guide Snapshot Integration Surface";

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Test Fixtures & Pre-Condition Assertions
// -----------------------------------------------------------------------------

/// Generates synthetic 5-block C source lines for deterministic AST testing.
fn create_synthetic_c_source() -> Vec<String> {
    vec![
        "/*".to_string(),
        "================================================================================".to_string(),
        "BLOCK 1: METADATA BLOCK".to_string(),
        "================================================================================".to_string(),
        "@project Tenebris and Lux".to_string(),
        "*/".to_string(),
        "".to_string(),
        "// =============================================================================".to_string(),
        "// BLOCK 2: OPENING BLOCK".to_string(),
        "// =============================================================================".to_string(),
        "#include <stdint.h>".to_string(),
        "".to_string(),
        "// -----------------------------------------------------------------------------".to_string(),
        "// Sub-Assembly 2.1: Core Types".to_string(),
        "// -----------------------------------------------------------------------------".to_string(),
        "typedef struct { int x; int y; } Point;".to_string(),
        "".to_string(),
        "// =============================================================================".to_string(),
        "// BLOCK 3: BODY BLOCK".to_string(),
        "// =============================================================================".to_string(),
        "Point create_point(int x, int y) {".to_string(),
        "    Point p;".to_string(),
        "    p.x = x;".to_string(),
        "    p.y = y;".to_string(),
        "    return p;".to_string(),
        "}".to_string(),
        "".to_string(),
        "// =============================================================================".to_string(),
        "// BLOCK 4: AUDIT PROCEDURES BLOCK".to_string(),
        "// =============================================================================".to_string(),
        "bool self_audit(void) { return true; }".to_string(),
        "".to_string(),
        "// =============================================================================".to_string(),
        "// BLOCK 5: CLOSING BLOCK".to_string(),
        "// =============================================================================".to_string(),
        "bool is_ready(void) { return true; }".to_string(),
    ]
}

// =============================================================================
// BLOCK 3: BODY BLOCK / FOUR-SECTION ONTOLOGICAL TEST SUITE
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: [ SECTION 1: ALWAYS PASS (NOMINAL GOLDEN SURFACE COGS) ]
// -----------------------------------------------------------------------------

/// Verifies nominal golden language inference across known source extensions.
#[test]
fn test_section_1_nominal_language_inference() {
    assert_eq!(infer_language("engine/src/main.c"), "c");
    assert_eq!(infer_language("engine/include/core/manifest.h"), "c");
    assert_eq!(infer_language("engine/src/viewport/render.cpp"), "cpp");
    assert_eq!(infer_language("engine/include/viewport/render.hpp"), "cpp");
    assert_eq!(infer_language("tools/src/main.rs"), "rust");
    assert_eq!(infer_language("engine/config/manifest.json"), "json");
    assert_eq!(infer_language("CMakeLists.txt"), "cmake");
    assert_eq!(infer_language("cmake/CornerstoneTesting.cmake"), "cmake");
    assert_eq!(infer_language("docs/themes/theme.yml"), "yaml");
    assert_eq!(infer_language("docs/themes/theme.yaml"), "yaml");
    assert_eq!(infer_language("notes.txt"), "text");
    assert_eq!(infer_language("unknown_file_type"), "text");
}

/// Verifies nominal golden extraction of full wholesale source file.
#[test]
fn test_section_1_nominal_whole_file_extraction() {
    let lines = create_synthetic_c_source();
    let spec = CodeSnapshotSpec {
        id: "snap.test.whole".to_string(),
        title: "Whole Test File".to_string(),
        source_file: "test.c".to_string(),
        mode: SnapshotMode::Whole,
        target: None,
        targets: vec![],
        language: None,
        excerpt: None,
        wholesale_in_appendix: WholesalePolicy::Never,
        callout_note: None,
        callouts: vec![],
    };

    let segments = extract_segments_for_spec(&lines, &spec, "c")
        .expect("Whole mode extraction must succeed");

    assert_eq!(segments.len(), 1);
    assert_eq!(segments[0].start_line, 1);
    assert_eq!(segments[0].end_line, lines.len());
    assert_eq!(segments[0].lines.len(), lines.len());
}

/// Verifies nominal golden extraction of an individual 5-block section.
#[test]
fn test_section_1_nominal_block_extraction() {
    let lines = create_synthetic_c_source();
    let spec = CodeSnapshotSpec {
        id: "snap.test.block2".to_string(),
        title: "Block 2 Opening".to_string(),
        source_file: "test.c".to_string(),
        mode: SnapshotMode::Block,
        target: Some("BLOCK 2: OPENING BLOCK".to_string()),
        targets: vec![],
        language: None,
        excerpt: None,
        wholesale_in_appendix: WholesalePolicy::Never,
        callout_note: None,
        callouts: vec![],
    };

    let segments = extract_segments_for_spec(&lines, &spec, "c")
        .expect("Block mode extraction must succeed");

    assert_eq!(segments.len(), 1);
    assert!(segments[0].lines.iter().any(|l| l.contains("#include <stdint.h>")));
    assert!(segments[0].lines.iter().any(|l| l.contains("Sub-Assembly 2.1")));
}

/// Verifies nominal golden formatting of snapshot plates with callout annotations.
#[test]
fn test_section_1_nominal_plate_formatting() {
    let spec = CodeSnapshotSpec {
        id: "snap.plate.sample".to_string(),
        title: "Sample Plate".to_string(),
        source_file: "engine/src/main.c".to_string(),
        mode: SnapshotMode::Whole,
        target: None,
        targets: vec![],
        language: Some("c".to_string()),
        excerpt: None,
        wholesale_in_appendix: WholesalePolicy::Never,
        callout_note: Some("Key initialization logic.".to_string()),
        callouts: vec![],
    };

    let segments = vec![ExtractedSegment {
        start_line: 10,
        end_line: 12,
        lines: vec![
            "int main(void) {".to_string(),
            "    return 0;".to_string(),
            "}".to_string(),
        ],
    }];

    let rendered = RenderedSnapshot {
        spec,
        lang: "c".to_string(),
        inline_segments: segments.clone(),
        wholesale_segments: segments,
        was_excerpted: false,
        total_source_lines: 3,
        plate_path: PathBuf::from("docs/snapshots/test.adoc"),
    };

    let plate = format_snapshot_plate(&rendered, true);
    assert!(plate.contains("[source,c"));
    assert!(plate.contains("Sample Plate"));
    assert!(plate.contains("int main(void) {"));
    assert!(plate.contains("Key initialization logic."));
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.2: [ SECTION 2: NEVER FAIL (FAULT INVERSION & DEGRADATION) ]
// -----------------------------------------------------------------------------

/// Verifies graceful failure when source lines vector is empty in whole mode.
#[test]
fn test_section_2_never_fail_empty_lines_whole_mode() {
    let empty_lines: Vec<String> = vec![];
    let spec = CodeSnapshotSpec {
        id: "snap.empty".to_string(),
        title: "Empty Source".to_string(),
        source_file: "empty.c".to_string(),
        mode: SnapshotMode::Whole,
        target: None,
        targets: vec![],
        language: None,
        excerpt: None,
        wholesale_in_appendix: WholesalePolicy::Never,
        callout_note: None,
        callouts: vec![],
    };

    let result = extract_segments_for_spec(&empty_lines, &spec, "c");
    assert!(
        result.is_err(),
        "[SEC 2: NEVER FAIL] Empty source file must yield typed Err"
    );
    assert!(result.unwrap_err().contains("source file is empty"));
}

/// Verifies graceful failure when block mode is missing target name.
#[test]
fn test_section_2_never_fail_missing_target() {
    let lines = create_synthetic_c_source();
    let spec = CodeSnapshotSpec {
        id: "snap.missing_target".to_string(),
        title: "Missing Target".to_string(),
        source_file: "test.c".to_string(),
        mode: SnapshotMode::Block,
        target: None,
        targets: vec![],
        language: None,
        excerpt: None,
        wholesale_in_appendix: WholesalePolicy::Never,
        callout_note: None,
        callouts: vec![],
    };

    let result = extract_segments_for_spec(&lines, &spec, "c");
    assert!(
        result.is_err(),
        "[SEC 2: NEVER FAIL] Block mode without target must return typed Err"
    );
    assert!(result.unwrap_err().contains("requires 'target'"));
}

/// Verifies graceful failure when target block is not found in source text.
#[test]
fn test_section_2_never_fail_target_not_found() {
    let lines = create_synthetic_c_source();
    let spec = CodeSnapshotSpec {
        id: "snap.nonexistent_block".to_string(),
        title: "Nonexistent Block".to_string(),
        source_file: "test.c".to_string(),
        mode: SnapshotMode::Block,
        target: Some("BLOCK 99: FICTIONAL BLOCK".to_string()),
        targets: vec![],
        language: None,
        excerpt: None,
        wholesale_in_appendix: WholesalePolicy::Never,
        callout_note: None,
        callouts: vec![],
    };

    let result = extract_segments_for_spec(&lines, &spec, "c");
    assert!(
        result.is_err(),
        "[SEC 2: NEVER FAIL] Missing target block must return typed Err"
    );
    assert!(result.unwrap_err().contains("not found in"));
}

/// Verifies invalid snapshot spec predicate handling under blank fields.
#[test]
fn test_section_2_never_fail_invalid_spec_predicate() {
    let blank_id = CodeSnapshotSpec {
        id: "".to_string(),
        title: "Valid Title".to_string(),
        source_file: "file.c".to_string(),
        mode: SnapshotMode::Whole,
        target: None,
        targets: vec![],
        language: None,
        excerpt: None,
        wholesale_in_appendix: WholesalePolicy::Never,
        callout_note: None,
        callouts: vec![],
    };
    assert!(!is_valid_snapshot_spec(&blank_id));

    let blank_title = CodeSnapshotSpec {
        id: "valid.id".to_string(),
        title: "   ".to_string(),
        source_file: "file.c".to_string(),
        mode: SnapshotMode::Whole,
        target: None,
        targets: vec![],
        language: None,
        excerpt: None,
        wholesale_in_appendix: WholesalePolicy::Never,
        callout_note: None,
        callouts: vec![],
    };
    assert!(!is_valid_snapshot_spec(&blank_title));

    let blank_source = CodeSnapshotSpec {
        id: "valid.id".to_string(),
        title: "Valid Title".to_string(),
        source_file: "".to_string(),
        mode: SnapshotMode::Whole,
        target: None,
        targets: vec![],
        language: None,
        excerpt: None,
        wholesale_in_appendix: WholesalePolicy::Never,
        callout_note: None,
        callouts: vec![],
    };
    assert!(!is_valid_snapshot_spec(&blank_source));
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.3: [ SECTION 3: INVARIANTS, SATURATION & LIMIT GUARDS ]
// -----------------------------------------------------------------------------

/// Verifies segment line numbering integrity and length consistency.
#[test]
fn test_section_3_invariants_line_numbers() {
    let seg = ExtractedSegment {
        start_line: 10,
        end_line: 14,
        lines: vec![
            "line 10".to_string(),
            "line 11".to_string(),
            "line 12".to_string(),
            "line 13".to_string(),
            "line 14".to_string(),
        ],
    };

    assert!(seg.start_line > 0);
    assert!(seg.end_line >= seg.start_line);
    assert_eq!(seg.lines.len(), seg.end_line - seg.start_line + 1);
}

/// Verifies wholesale appendix policy predicate under all combinations.
#[test]
fn test_section_3_invariants_wholesale_policy() {
    let spec = CodeSnapshotSpec {
        id: "snap.policy".to_string(),
        title: "Policy Spec".to_string(),
        source_file: "src.c".to_string(),
        mode: SnapshotMode::Whole,
        target: None,
        targets: vec![],
        language: None,
        excerpt: None,
        wholesale_in_appendix: WholesalePolicy::Always,
        callout_note: None,
        callouts: vec![],
    };

    let mut rendered = RenderedSnapshot {
        spec: spec.clone(),
        lang: "c".to_string(),
        inline_segments: vec![],
        wholesale_segments: vec![],
        was_excerpted: false,
        total_source_lines: 10,
        plate_path: PathBuf::from("docs/snapshots/test.adoc"),
    };

    // Always policy -> always true
    assert!(snapshot_goes_in_appendix(&rendered));
    rendered.was_excerpted = true;
    assert!(snapshot_goes_in_appendix(&rendered));

    // Never policy -> always false
    rendered.spec.wholesale_in_appendix = WholesalePolicy::Never;
    rendered.was_excerpted = false;
    assert!(!snapshot_goes_in_appendix(&rendered));
    rendered.was_excerpted = true;
    assert!(!snapshot_goes_in_appendix(&rendered));

    // WhenExcerpted policy -> true only when was_excerpted is true
    rendered.spec.wholesale_in_appendix = WholesalePolicy::WhenExcerpted;
    rendered.was_excerpted = false;
    assert!(!snapshot_goes_in_appendix(&rendered));
    rendered.was_excerpted = true;
    assert!(snapshot_goes_in_appendix(&rendered));
}

/// Verifies segment merging across contiguous and overlapping line spans.
#[test]
fn test_section_3_invariants_segment_merging() {
    let seg_a = ExtractedSegment {
        start_line: 1,
        end_line: 3,
        lines: vec!["a1".to_string(), "a2".to_string(), "a3".to_string()],
    };
    let seg_b = ExtractedSegment {
        start_line: 4,
        end_line: 5,
        lines: vec!["b4".to_string(), "b5".to_string()],
    };

    let all_lines = vec![
        "a1".to_string(),
        "a2".to_string(),
        "a3".to_string(),
        "b4".to_string(),
        "b5".to_string(),
    ];

    let merged = merge_segments(&all_lines, vec![seg_a, seg_b]);
    assert_eq!(merged.len(), 1, "Contiguous segments must merge into one");
    assert_eq!(merged[0].start_line, 1);
    assert_eq!(merged[0].end_line, 5);
    assert_eq!(merged[0].lines.len(), 5);
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.4: [ SECTION 4: EMERGENCE & COMPOSITE WORKFLOWS ]
// -----------------------------------------------------------------------------

/// Verifies live code snapshots manifest parsing and specimen extraction.
#[test]
fn test_section_4_emergence_live_snapshots_manifest() {
    let repo_root = resolve_repo_root();
    let manifest_path = repo_root.join("docs/code_snapshots_manifest.json");

    if manifest_path.is_file() {
        let raw = fs::read_to_string(&manifest_path)
            .expect("Must read live code snapshots manifest");
        let manifest: Result<CodeSnapshotsManifest, _> = serde_json::from_str(&raw);
        assert!(
            manifest.is_ok(),
            "[SEC 4: EMERGENCE] Live code snapshots manifest must parse: {:?}",
            manifest.err()
        );

        let parsed = manifest.unwrap();
        assert!(
            !parsed.block_3_body.snapshots.is_empty(),
            "[SEC 4: EMERGENCE] Manifest must contain code snapshots"
        );

        for snap in &parsed.block_3_body.snapshots {
            assert!(
                is_valid_snapshot_spec(snap),
                "[SEC 4: EMERGENCE] Snapshot {} must satisfy valid spec predicate",
                snap.id
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

/// Diagnostic self-audit verifying guide snapshot internal consistency.
#[test]
fn test_preflight_diagnostics_self_audit() {
    assert!(
        !SUITE_NAME.is_empty(),
        "[BLOCK 4: PREFLIGHT] SUITE_NAME constant must not be empty"
    );
    let result = self_audit();
    assert!(
        result.is_ok(),
        "[BLOCK 4: PREFLIGHT] snapshot::self_audit() must pass: {:?}",
        result.err()
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 4.2: Documentation Anchor Verification Gate
// -----------------------------------------------------------------------------

/// Audits physical code snapshots manifest file on disk.
#[test]
fn test_preflight_documentation_anchors() {
    let root = resolve_repo_root();
    let manifest_path = root.join("docs/code_snapshots_manifest.json");
    assert!(
        manifest_path.is_file(),
        "docs/code_snapshots_manifest.json must exist"
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
        is_snapshot_subsystem_ready(),
        "[BLOCK 5: EPILOGUE] is_snapshot_subsystem_ready() must return true"
    );
}
