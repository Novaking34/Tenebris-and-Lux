// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/tests/unit/guide/manifest.rs
// @id            test.unit.tools.guide.manifest
// @layer         Layer 4: Verification Suite
// @archetype     verification_suite | unit_isolation
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-16
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Developer Toolchain // Guide Manifest Test Suite
// @role          Four-Section Ontological Test Suite for guide::manifest
// @maturity      Rev 5 Baseline | Four-Section Transpiler-Ready Verification
// @chassis       bounded-heap | Test runner execution chassis; stack frames bounded
// @scale         O(V + C) across volumes and chapters
// @seams         in: tl_tools::guide::manifest | pairs: docs/master_guide_manifest.json
// @invariants    1. Four-Section Ontological Architecture strictly enforced:
//                   Section 1 (Always Pass), Section 2 (Never Fail),
//                   Section 3 (Invariants & Limits), Section 4 (Emergence).
//                2. External Consumer Purity: imports tl_tools via public API.
//                3. Zero Panics: graceful Result evaluation across fallible paths.
//                4. Strict <= 80-column line width discipline across documentation.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Tier 1 Integration verification harness evaluating the `guide::manifest`
//   sub-assembly. Validates 5-block JSON deserialization, canonical fallback
//   construction, volume and chapter specifications, and fault-tolerant error
//   reporting under invalid inputs.
//
//   ### For Game Designers & Non-Programmers:
//   Guarantees that book structure, volume definitions, chapter sequences, and
//   publishing parameters are loaded faithfully from data manifests.
//
//   ### For Systems Programmers & Engine Architects:
//   - Transpilation Alignment: Maps directly to OmniCode's 4-section verification
//     schema, allowing lossless round-trip transpilation between languages.
//   - Chassis Discipline: Verifies structural constraints without heap leaks.
// =============================================================================

//! # Four-Section Integration Suite for Master Guide Manifest Sub-Assembly
//!
//! Exercises `tl_tools::guide::manifest` across Always-Pass, Never-Fail,
//! Invariant, and Emergent execution tiers.

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Pre-Conditions, Constants, Test Fixtures & Harness Scaffolding )
// =============================================================================

use std::fs;
use std::path::PathBuf;

use tl_tools::cmd::resolve_repo_root;
use tl_tools::guide::manifest::{
    build_canonical_volumes, default_config, is_manifest_subsystem_ready,
    is_valid_volume_spec, load_manifest_and_config, load_volumes_from_manifest,
    save_default_manifest, self_audit, SnapshotSettings, VolumeSpec,
};

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Operational Test Constants & Suite Identifiers
// -----------------------------------------------------------------------------

/// Canonical test suite identifier matching engine convention.
pub const SUITE_NAME: &str = "Toolchain Guide Manifest Integration Surface";

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

/// Verifies nominal golden construction of canonical fallback volumes.
#[test]
fn test_section_1_nominal_canonical_volumes() {
    let docs = get_docs_dir();
    let volumes = build_canonical_volumes(&docs);

    assert_eq!(
        volumes.len(),
        6,
        "[SEC 1: ALWAYS PASS] Canonical volume builder must construct 6 volumes"
    );

    let expected_romans = ["I", "V", "II", "III", "IV", "VI"];
    for vol in &volumes {
        assert!(
            is_valid_volume_spec(vol),
            "[SEC 1: ALWAYS PASS] Volume {} must be valid",
            vol.roman
        );
        assert!(
            expected_romans.contains(&vol.roman.as_str()),
            "[SEC 1: ALWAYS PASS] Unexpected Roman numeral: {}",
            vol.roman
        );
    }
}

/// Verifies nominal golden initialization of default publication config.
#[test]
fn test_section_1_nominal_default_config() {
    let repo_root = resolve_repo_root();
    let config = default_config(&repo_root);

    assert_eq!(
        config.toc_levels, 2,
        "[SEC 1: ALWAYS PASS] Default toc_levels must be 2"
    );
    assert_eq!(
        config.sectnum_levels, 3,
        "[SEC 1: ALWAYS PASS] Default sectnum_levels must be 3"
    );
    assert_eq!(
        config.pdf_theme_name, "cornerstone-book",
        "[SEC 1: ALWAYS PASS] Default PDF theme must be cornerstone-book"
    );
    assert!(
        config.exclude_scan_dirs.contains(&"snapshots".to_string()),
        "[SEC 1: ALWAYS PASS] Default exclude dirs must contain snapshots"
    );
}

/// Verifies nominal golden loading of the live master guide manifest.
#[test]
fn test_section_1_nominal_live_manifest_loading() {
    let repo_root = resolve_repo_root();
    let docs_dir = repo_root.join("docs");
    let manifest_path = docs_dir.join("master_guide_manifest.json");

    assert!(
        manifest_path.is_file(),
        "Live manifest must exist at {:?}",
        manifest_path
    );

    let result = load_manifest_and_config(&repo_root, &docs_dir, &manifest_path);
    assert!(
        result.is_ok(),
        "[SEC 1: ALWAYS PASS] Loading live manifest must succeed: {:?}",
        result.err()
    );

    let (volumes, config) = result.unwrap();
    assert!(
        volumes.len() >= 6,
        "[SEC 1: ALWAYS PASS] Live manifest must contain at least 6 volumes, found {}",
        volumes.len()
    );
    assert_eq!(config.toc_levels, 2);
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.2: [ SECTION 2: NEVER FAIL (FAULT INVERSION & DEGRADATION) ]
// -----------------------------------------------------------------------------

/// Verifies graceful failure when loading a non-existent manifest path.
#[test]
fn test_section_2_never_fail_missing_manifest_path() {
    let repo_root = resolve_repo_root();
    let docs_dir = repo_root.join("docs");
    let missing = docs_dir.join("nonexistent_manifest_xyz.json");

    let result = load_manifest_and_config(&repo_root, &docs_dir, &missing);
    assert!(
        result.is_err(),
        "[SEC 2: NEVER FAIL] Loading nonexistent manifest must return typed Err"
    );

    let err_msg = result.unwrap_err();
    assert!(
        err_msg.contains("Unable to read guide manifest file"),
        "[SEC 2: NEVER FAIL] Error message must diagnose missing file: {}",
        err_msg
    );
}

/// Verifies graceful failure when parsing malformed JSON manifest content.
#[test]
fn test_section_2_never_fail_corrupted_json() {
    let repo_root = resolve_repo_root();
    let docs_dir = repo_root.join("docs");
    let scratch_dir = repo_root.join("build/scratch");
    let _ = fs::create_dir_all(&scratch_dir);
    let corrupted_path = scratch_dir.join("corrupted_manifest.json");

    fs::write(&corrupted_path, "{ broken_json: [ invalid")
        .expect("Must write corrupted test fixture");

    let result = load_manifest_and_config(&repo_root, &docs_dir, &corrupted_path);
    assert!(
        result.is_err(),
        "[SEC 2: NEVER FAIL] Corrupted JSON must yield typed Err, not panic"
    );

    let _ = fs::remove_file(&corrupted_path);
}

/// Verifies invalid volume predicate handling under empty or whitespace attributes.
#[test]
fn test_section_2_never_fail_invalid_volume_predicates() {
    let empty_roman = VolumeSpec {
        roman: "   ".to_string(),
        slug: "valid_slug".to_string(),
        title: "Valid Title".to_string(),
        description: "Desc".to_string(),
        scan_areas: vec![],
        chapters: vec![],
    };
    assert!(
        !is_valid_volume_spec(&empty_roman),
        "[SEC 2: NEVER FAIL] Empty Roman numeral must fail predicate"
    );

    let empty_slug = VolumeSpec {
        roman: "I".to_string(),
        slug: "".to_string(),
        title: "Valid Title".to_string(),
        description: "Desc".to_string(),
        scan_areas: vec![],
        chapters: vec![],
    };
    assert!(
        !is_valid_volume_spec(&empty_slug),
        "[SEC 2: NEVER FAIL] Empty slug must fail predicate"
    );

    let empty_title = VolumeSpec {
        roman: "I".to_string(),
        slug: "valid_slug".to_string(),
        title: "  ".to_string(),
        description: "Desc".to_string(),
        scan_areas: vec![],
        chapters: vec![],
    };
    assert!(
        !is_valid_volume_spec(&empty_title),
        "[SEC 2: NEVER FAIL] Empty title must fail predicate"
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.3: [ SECTION 3: INVARIANTS, SATURATION & LIMIT GUARDS ]
// -----------------------------------------------------------------------------

/// Verifies chapter numbering continuity across canonical volumes.
#[test]
fn test_section_3_invariants_chapter_spec_numbers() {
    let docs = get_docs_dir();
    let volumes = build_canonical_volumes(&docs);

    let mut seen_numbers = std::collections::HashSet::new();
    for vol in &volumes {
        for ch in &vol.chapters {
            assert!(
                ch.number > 0,
                "[SEC 3: INVARIANTS] Chapter number must be strictly positive: {}",
                ch.number
            );
            assert!(
                !ch.title.trim().is_empty(),
                "[SEC 3: INVARIANTS] Chapter title must not be blank"
            );
            assert!(
                seen_numbers.insert(ch.number),
                "[SEC 3: INVARIANTS] Duplicate chapter number: {}",
                ch.number
            );
        }
    }
}

/// Verifies snapshot settings defaults and limit bounds.
#[test]
fn test_section_3_invariants_snapshot_settings_defaults() {
    let settings = SnapshotSettings::default();

    assert!(
        !settings.manifest_path.is_empty(),
        "[SEC 3: INVARIANTS] Manifest path must be non-empty"
    );
    assert!(
        !settings.output_dir.is_empty(),
        "[SEC 3: INVARIANTS] Output directory must be non-empty"
    );
    assert!(
        settings.include_wholesale_appendix,
        "[SEC 3: INVARIANTS] Default snapshot settings must include appendix"
    );
    assert!(
        settings.show_line_numbers,
        "[SEC 3: INVARIANTS] Default snapshot settings must enable line numbers"
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.4: [ SECTION 4: EMERGENCE & COMPOSITE WORKFLOWS ]
// -----------------------------------------------------------------------------

/// Verifies composite manifest serialization, saving, and round-trip re-loading.
#[test]
fn test_section_4_emergence_manifest_round_trip() {
    let repo_root = resolve_repo_root();
    let docs = repo_root.join("docs");
    let scratch_dir = repo_root.join("build/scratch");
    let _ = fs::create_dir_all(&scratch_dir);
    let scratch_manifest = scratch_dir.join("test_roundtrip_manifest.json");

    let canonical = build_canonical_volumes(&docs);
    let save_res = save_default_manifest(&docs, &scratch_manifest, &canonical);
    assert!(
        save_res.is_ok(),
        "[SEC 4: EMERGENCE] Saving default manifest must succeed: {:?}",
        save_res.err()
    );

    let load_res = load_volumes_from_manifest(&docs, &scratch_manifest);
    assert!(
        load_res.is_ok(),
        "[SEC 4: EMERGENCE] Re-loading saved manifest must succeed: {:?}",
        load_res.err()
    );

    let reloaded_vols = load_res.unwrap();
    assert_eq!(
        reloaded_vols.len(),
        canonical.len(),
        "[SEC 4: EMERGENCE] Volume count must match after round-trip"
    );

    let _ = fs::remove_file(&scratch_manifest);
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// ( Preflight Diagnostics & Deep Verification Audits )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 4.1: Preflight Diagnostic Gates
// -----------------------------------------------------------------------------

/// Diagnostic self-audit verifying guide manifest internal consistency.
#[test]
fn test_preflight_diagnostics_self_audit() {
    assert!(
        !SUITE_NAME.is_empty(),
        "[BLOCK 4: PREFLIGHT] SUITE_NAME constant must not be empty"
    );
    let result = self_audit();
    assert!(
        result.is_ok(),
        "[BLOCK 4: PREFLIGHT] manifest::self_audit() must pass: {:?}",
        result.err()
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 4.2: Documentation Anchor Verification Gate
// -----------------------------------------------------------------------------

/// Audits physical documentation manifest on disk.
#[test]
fn test_preflight_documentation_anchors() {
    let root = resolve_repo_root();
    let manifest_path = root.join("docs/master_guide_manifest.json");
    assert!(
        manifest_path.is_file(),
        "docs/master_guide_manifest.json must exist"
    );
    let metadata = fs::metadata(&manifest_path).expect("Must read manifest metadata");
    assert!(
        metadata.len() > 100,
        "Manifest file must have meaningful content (> 100 bytes)"
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
        is_manifest_subsystem_ready(),
        "[BLOCK 5: EPILOGUE] is_manifest_subsystem_ready() must return true"
    );
}
