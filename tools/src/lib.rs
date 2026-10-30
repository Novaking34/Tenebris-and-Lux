// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/lib.rs
// @id            tools.lib.core
// @layer         Layer 3: Systems Developer Tooling Core Library
// @version       0.1.0
// @interface_rev Rev 1
// @date          2026-10-29
// @updated       2026-10-29
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Core developer tooling and systems engineering library for Tenebris and
//   Lux. Provides a modular, reusable runner architecture, historical RPG Maker
//   MV deserialization and extraction engines, documentation compilation pipelines,
//   and diagnostic audit procedures.
//
// PEDAGOGICAL MISSION:
//   Demonstrates the OmniCode 5-Block Ontological Architecture within a compiled
//   Rust library. Establishes a zero-warning, type-safe backbone that decouples
//   systems execution from CLI frontends, enabling programmatic pipeline
//   invocation in CI/CD, interactive development, or automated test runners.
//
// AXIOMATIC INVARIANTS:
//   1. Strict conformance to Rust 2024 Edition.
//   2. Zero panics in library code; all fallible operations return typed `Result`.
//   3. Thread-safe, reentrant design across all runner tasks.
//   4. Strict line discipline <= 80 columns across all documentation.
// =============================================================================

//! # Tenebris and Lux Developer Toolchain Core Library (`tl_tools`)
//!
//! `tl_tools` is the central developer engineering toolkit for the tactical
//! strategy RPG *Tenebris and Lux*. It provides:
//!
//! - **[`runner`]**: An extensible, modular task runner pipeline that decouples
//!   task orchestration from low-level execution, capturing execution telemetry,
//!   monotonic timing, and artifact manifests.
//! - **[`extractor`]**: A historical extraction and reconciliation engine that
//!   deserializes 2017–2018 RPG Maker MV datasets from archival storage into
//!   canonical game design baselines.
//! - **[`guide`]**: A multi-format publication engine assembling 6 volumes and 42
//!   chapters of technical manuals into AsciiDoc (`MASTER_GUIDE.adoc`), Markdown
//!   (`MASTER_GUIDE.md`), and PDF (`MASTER_GUIDE.pdf`).

// =============================================================================
// BLOCK 2: OPENING BLOCK / MODULE EXPORTS & CONSTANTS
// Stratum: Developer Tooling | Machine: tools.lib.rust
// =============================================================================

pub mod extractor;
pub mod guide;
pub mod runner;

pub use extractor::{DemoExtractor, ExtractorTask};
pub use guide::{MasterGuideBuilder, MasterGuideTask};
pub use runner::{
    CurationAuditTask, EnvironmentAuditTask, PipelineReport, RunnerContext,
    RunnerEngine, TaskRunner, TaskStatus, TaskTelemetry,
};

/// Toolchain version identity string.
pub const TOOL_VERSION: &str = "0.1.0";

/// Toolchain ontological standard.
pub const TOOL_STANDARD: &str = "Rust 2024 Edition | OmniCode 5-Block Architecture";

// =============================================================================
// BLOCK 3: BODY BLOCK / FACTORY HELPERS
// Stratum: Developer Tooling | Machine: tools.lib.rust
// =============================================================================

/// Creates a standard, fully-equipped `RunnerEngine` pre-loaded with all
/// default developer tasks:
///   1. `env_audit` (Environment topology & prerequisites)
///   2. `curation_audit` (Game design curation doc verification)
///   3. `demo_extractor` (Historical 2017-2018 demo extraction)
///   4. `master_guide` (Unified AsciiDoc, Markdown & PDF publication)
pub fn create_default_runner(context: RunnerContext) -> RunnerEngine {
    let mut engine = RunnerEngine::new(context);
    engine
        .register(EnvironmentAuditTask)
        .register(CurationAuditTask)
        .register(ExtractorTask)
        .register(MasterGuideTask);
    engine
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Developer Tooling | Machine: tools.lib.rust
// =============================================================================

/// Diagnostic crate self-test verifying library constant invariants.
pub fn audit_library_invariants() -> Result<(), &'static str> {
    if TOOL_VERSION.is_empty() {
        return Err("TOOL_VERSION invariant violated: empty version string");
    }
    if !TOOL_STANDARD.contains("Rust 2024") {
        return Err("TOOL_STANDARD invariant violated: missing Rust 2024 marker");
    }
    Ok(())
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK / TESTS & INVARIANT VERIFICATION
// Stratum: Developer Tooling | Machine: tools.lib.rust
// =============================================================================

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_library_invariants_pass() {
        assert!(audit_library_invariants().is_ok());
    }

    #[test]
    fn test_default_runner_registration() {
        let repo = std::path::PathBuf::from(".");
        let archive = std::path::PathBuf::from(".");
        let ctx = RunnerContext::new(&repo, &archive);
        let engine = create_default_runner(ctx);
        assert_eq!(engine.context().repo_root, repo);
    }
}
