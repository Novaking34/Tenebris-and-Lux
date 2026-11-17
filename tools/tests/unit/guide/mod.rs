// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/tests/unit/guide/mod.rs
// @id            test.unit.tools.guide.coordinator
// @layer         Layer 4: Verification Suite
// @archetype     verification_suite | unit_isolation
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-16
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Developer Toolchain // Tier 2 Unit Guide Suite
// @role          Unit Isolation Test Suite Coordinator for Master Guide Sub-Assembly
// @maturity      Rev 5 Baseline | 4-Section Ontological Testing
// @chassis       bounded-heap | Test runner execution chassis; stack frames bounded
// @scale         O(1) coordinator registration over guide sub-assemblies
// @seams         in: tl_tools::guide | exports: compiler, manifest, pdf, snapshot, theme
// @invariants    1. Four-Tier Macro Strata: Enforces Tier 2 Unit isolation testing.
//                2. External Consumer Purity: imports tl_tools via public API.
//                3. Zero Mocks: exercises real guide compilers and layout engines.
//                4. Strict <= 80-column line width discipline across documentation.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Unit test coordinator module for the `guide` subsystem. Dispatches fine-grained
//   unit isolation verification across compiler mechanics, manifest parsing,
//   single-page PDF layout constraints, code snapshot plates, and Asciidoctor
//   styling themes.
//
//   ### For Game Designers & Non-Programmers:
//   Coordinates isolated checks on the inner gears of the Master Guide compiler,
//   ensuring that all 114 chapters and 6 volumes format cleanly for publication.
//
//   ### For Systems Programmers & Engine Architects:
//   - Subsystem Boundary: Aggregates `compiler`, `manifest`, `pdf`, `snapshot`,
//     and `theme` submodules under a single deterministic verification umbrella.
//   - Audit Rigor: Implements preflight diagnostic gates and clean-state epilogues
//     aligned with Cornerstone C11 test conventions.
// =============================================================================

//! # Tier 2 Unit Isolation Suite for Master Guide Sub-Assembly
//!
//! Organizes component-level unit isolation tests for `tl_tools::guide`:
//! - `compiler`: AsciiDoc volume generation, area scanning, and output staging.
//! - `manifest`: Chapter catalogue parsing, volume definitions, and title lookups.
//! - `pdf`: Page budgeting, layout rules, and geometry constraints.
//! - `snapshot`: Source code plate rendering and syntax styling.
//! - `theme`: Asciidoctor CSS themes, palette definitions, and fonts.

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Pre-Conditions, Constants, Test Fixtures & Harness Scaffolding )
// =============================================================================

use crate::common;

pub mod compiler;
pub mod manifest;
pub mod pdf;
pub mod snapshot;
pub mod theme;

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Operational Test Constants & Suite Identifiers
// -----------------------------------------------------------------------------

/// Canonical suite identifier matching engine testing convention.
pub const SUITE_NAME: &str = "Tier 2: Unit Guide Sub-Assembly Coordinator";

// =============================================================================
// BLOCK 3: BODY BLOCK / SECTION AUDIT COORDINATION
// =============================================================================

// Submodules implement Section 1 (Always Pass), Section 2 (Never Fail),
// Section 3 (Invariants), and Section 4 (Emergence) tests.

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// ( Preflight Diagnostics & Deep Verification Audits )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 4.1: Preflight Diagnostic Gates
// -----------------------------------------------------------------------------

/// Diagnostic self-audit verifying coordinator suite constant and common harness.
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
// Sub-Assembly 4.2: Documentation Anchor Verification Gate
// -----------------------------------------------------------------------------

/// Audits physical documentation directory anchors required for guide compilation.
#[test]
fn test_preflight_guide_anchors() {
    let root = common::resolve_repo_root();
    assert!(
        root.join("docs/master_guide_manifest.json").is_file(),
        "[BLOCK 4: PREFLIGHT] docs/master_guide_manifest.json must exist"
    );
    assert!(
        root.join("docs/adoc_pdf_layout_rules.json").is_file(),
        "[BLOCK 4: PREFLIGHT] docs/adoc_pdf_layout_rules.json must exist"
    );
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( Test Suite Epilogue & Summary Handshake )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 5.1: Epilogue & Clean State Handshake
// -----------------------------------------------------------------------------

/// Verifies that running Guide unit tests leaves zero transient artifacts.
#[test]
fn test_epilogue_clean_state_handshake() {
    let root = common::resolve_repo_root();
    assert!(
        !root.join("docs/scratch_transient.tmp").exists(),
        "[BLOCK 5: EPILOGUE] Zero transient test artifacts leaked to docs/"
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
