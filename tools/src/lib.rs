// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/lib.rs
// @id            cog.tools.lib
// @layer         Layer 3: Systems Developer Tooling Core Library
// @archetype     facade_umbrella | stateful
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-16
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Developer Toolchain // Core Systems Library
// @role          Central Library Facade & Pipeline Task Orchestration Backbone
// @maturity      Rev 5 Baseline | Unified systems developer toolkit
// @chassis       bounded-heap | Memory allocated for pipeline reports & caches
// @scale         O(M) across registered subsystems and pipeline runners
// @seams         in: tools/src/ | out: tl_tools crate API | probes: self_audit -> exit
// @invariants    1. Strict conformance to Rust 2024 Edition.
//                2. Zero panics in library code; all fallible ops return Result.
//                3. Thread-safe, reentrant design across all runner tasks.
//                4. Strict line discipline <= 80 columns across all comments.
//                5. Exhaustive first-principles parameter & return documentation.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Core developer tooling and systems engineering library for Tenebris and
//   Lux. Provides a modular, reusable runner architecture, historical RPG Maker
//   MV deserialization and extraction engines, documentation compilation
//   pipelines, diagnostic audit procedures, and procedural asset synthesis.
//
//   ### For Game Designers & Non-Programmers:
//   Central developer engine powering all automation: verifies that assets
//   exist and match dimensions, validates JSON gameplay balance tables, and
//   compiles the unified Master Guide manuals from game data.
//
//   ### For Systems Programmers & Engine Architects:
//   - Decouples systems execution from CLI frontends.
//   - Exports clean facade interfaces across `assets`, `cmd`, `extractor`,
//     `guide`, `runner`, `syntax`, and `validator`.
//   - Enforces first-principles design: parameter contracts, allocation
//     bounds, reentrancy guarantees, and zero-panic error semantics.
// =============================================================================

//! # Tenebris and Lux Developer Toolchain Core Library (`tl_tools`)
//!
//! `tl_tools` is the central developer engineering toolkit for the tactical
//! strategy RPG *Tenebris and Lux*. It provides:
//!
//! - **[`runner`]**: An extensible, modular task runner pipeline that decouples
//!   task orchestration from low-level execution, capturing execution
//!   telemetry, monotonic timing, and artifact manifests.
//! - **[`extractor`]**: A historical extraction and reconciliation engine that
//!   deserializes 2017–2018 RPG Maker MV datasets from archival storage into
//!   canonical game design baselines.
//! - **[`guide`]**: A multi-format publication engine assembling 6 volumes and
//!   42 chapters of technical manuals into AsciiDoc (`MASTER_GUIDE.adoc`),
//!   Markdown (`MASTER_GUIDE.md`), and PDF (`MASTER_GUIDE.pdf`).
//! - **[`assets`]**: Procedural visual/audio synthesizer and on-disk catalog
//!   auditor.
//! - **[`syntax`]**: Multi-language lexical analyzers and AST tokenizers.
//! - **[`validator`]**: OmniCode 5-Block structural covenant validator.
//! - **[`cmd`]**: Unified command-line dispatchers and CLI sub-assemblies.

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Submodules, Public Exports & Version Identity )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Operational Modules & Re-Exports
// -----------------------------------------------------------------------------

pub mod assets;
pub mod cmd;
pub mod extractor;
pub mod guide;
pub mod runner;
pub mod syntax;
pub mod validator;

// Re-export primary asset pipeline tasks and inspection engines
pub use assets::{AssetAuditTask, AssetAuditor};

// Re-export CLI toolchain entry point
pub use cmd::run_toolchain;

// Re-export historical extraction and reconciliation types
pub use extractor::{DemoExtractor, ExtractorTask};

// Re-export technical documentation publication engine
pub use guide::{MasterGuideBuilder, MasterGuideTask};

// Re-export modular pipeline runner engine and diagnostic tasks
pub use runner::{
    CurationAuditTask, EnvironmentAuditTask, PipelineReport, RunnerContext,
    RunnerEngine, TaskRunner, TaskStatus, TaskTelemetry,
};

// Re-export lexical analysis language types
pub use syntax::Language;

// Re-export OmniCode 5-Block validation engines and summary reports
pub use validator::{
    DataValidationTask, DataValidator, FullValidationTask, GitValidationTask,
    HeaderValidationTask, HeaderValidator, ValidationOptions, ValidationSummary,
};

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Constants & Identity Strings
// -----------------------------------------------------------------------------

/// Toolchain version identity string conforming to Semantic Versioning (SemVer).
///
/// # Dual-Audience Documentation
///
/// ### For Game Designers & Non-Programmers:
/// Identifies which version of the developer toolkit is currently installed.
/// Matches the Cornerstone Engine milestone version (`0.1.2-dev`).
///
/// ### For Systems Programmers & Engine Architects:
/// Used during automated updater checks, bug telemetry reports, and build
/// artifact tagging to ensure pipeline synchronization between engine and tools.
///
/// # Invariants
/// - Must remain non-empty and formatted as SemVer `MAJOR.MINOR.PATCH[-PRERELEASE]`.
pub const TOOL_VERSION: &str = "0.1.2-dev";

/// Toolchain ontological standard defining language edition and architecture.
///
/// # Dual-Audience Documentation
///
/// ### For Game Designers & Non-Programmers:
/// Signifies the engineering discipline and coding rules used to build the tool.
///
/// ### For Systems Programmers & Engine Architects:
/// Enforces compiler alignment to the ISO Rust 2024 Edition and OmniCode
/// Universal 5-Block Lifecycle Protocol across all sub-assemblies.
///
/// # Invariants
/// - Must explicitly contain the substrings "Rust 2024" and "OmniCode".
pub const TOOL_STANDARD: &str = "Rust 2024 Edition | OmniCode 5-Block Architecture";

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Default Task Runner Factory & Subsystem Orchestration )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: Default Task Runner Factory
// -----------------------------------------------------------------------------

/// Constructs a fully-equipped, production [`RunnerEngine`] pre-configured
/// with all 8 canonical developer tasks executed in strict dependency order:
///
/// 1. **`env_audit`** ([`EnvironmentAuditTask`]): Audits repository directory
///    topology, Cargo toolchain, and Git executable prerequisites.
/// 2. **`curation_audit`** ([`CurationAuditTask`]): Audits game design curation
///    documents in `docs/` for canonical completeness.
/// 3. **`data_validator`** ([`DataValidationTask`]): Audits all JSON manifests
///    in `engine/config/` and `game/data/` against 5-Block standards.
/// 4. **`header_validator`** ([`HeaderValidationTask`]): Validates Block 1
///    metadata headers across C, Rust, JSON, and CMake source files.
/// 5. **`git_validator`** ([`GitValidationTask`]): Audits Git commit messages
///    and branch nomenclature against project conventions.
/// 6. **`asset_pipeline`** ([`AssetAuditTask`]): Audits physical binary assets
///    in `game/assets/` against `game/data/presentation/` manifests.
/// 7. **`demo_extractor`** ([`ExtractorTask`]): Deserializes and reconciles
///    archival 2017–2018 RPG Maker MV datasets into modern JSON catalogs.
/// 8. **`master_guide`** ([`MasterGuideTask`]): Assembles and compiles the
///    6-volume Master Guide into AsciiDoc, Markdown, and PDF deliverables.
///
/// # Arguments
/// * `context` - A [`RunnerContext`] containing resolved absolute paths to
///   the repository root and historical demo archive directory, along with
///   execution telemetry flags.
///
/// # Returns
/// An initialized [`RunnerEngine`] populated with all 8 registered tasks,
/// ready for sequential execution via `engine.run()`.
///
/// # Invariants
/// - Tasks are registered in strictly deterministic sequence (prerequisites first).
/// - Memory footprint is bounded: tasks are stored as boxed trait objects
///   with minimal heap overhead.
/// - Zero panics during registration.
///
/// # Dual-Audience Documentation
///
/// ### For Game Designers & Non-Programmers:
/// This factory sets up the complete "one-click" pipeline. When you run a full
/// project build, this function prepares every verification and compilation
/// step—from checking your art assets to publishing the design manuals.
///
/// ### For Systems Programmers & Engine Architects:
/// Implements the Dependency Inversion Principle: decouples task registration
/// from CLI frontends and runner dispatchers. The returned engine executes each
/// task within a guarded sandbox, capturing elapsed monotonic time and
/// structured failure telemetry without aborting the entire pipeline.
///
/// # Examples
/// ```rust
/// use std::path::PathBuf;
/// use tl_tools::{create_default_runner, RunnerContext};
///
/// let ctx = RunnerContext::new(&PathBuf::from("."), &PathBuf::from("."));
/// let engine = create_default_runner(ctx);
/// assert_eq!(engine.task_count(), 8);
/// ```
pub fn create_default_runner(context: RunnerContext) -> RunnerEngine {
    let mut engine = RunnerEngine::new(context);
    engine
        .register(EnvironmentAuditTask)
        .register(CurationAuditTask)
        .register(DataValidationTask)
        .register(HeaderValidationTask)
        .register(GitValidationTask)
        .register(AssetAuditTask)
        .register(ExtractorTask)
        .register(MasterGuideTask);
    engine
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// ( Diagnostic Invariant Checks & Four-Section Ontological Test Suite )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 4.1: Preflight Diagnostic Gates & Self-Audit Probe
// -----------------------------------------------------------------------------

/// Diagnostic crate self-test verifying library constant invariants.
///
/// # Returns
/// * `Ok(())` - All library constants satisfy ontological invariants.
/// * `Err(&'static str)` - Description of the invariant violation.
///
/// # Errors
/// Returns an error if [`TOOL_VERSION`] is empty or [`TOOL_STANDARD`] is missing
/// the mandatory "Rust 2024" or "OmniCode" standard markers.
///
/// # Invariants
/// - O(1) time complexity; executes purely in memory without I/O.
///
/// # Dual-Audience Documentation
///
/// ### For Game Designers & Non-Programmers:
/// Fast health check ensuring the toolkit's internal identity and version
/// information is properly configured.
///
/// ### For Systems Programmers & Engine Architects:
/// Static assertion gate executed before pipeline dispatch to prevent
/// executing corrupted or miscompiled binary libraries.
///
/// # Examples
/// ```rust
/// use tl_tools::audit_library_invariants;
///
/// assert!(audit_library_invariants().is_ok());
/// ```
pub fn audit_library_invariants() -> Result<(), &'static str> {
    if TOOL_VERSION.is_empty() {
        return Err("TOOL_VERSION invariant violated: empty version string");
    }
    if !TOOL_STANDARD.contains("Rust 2024") {
        return Err("TOOL_STANDARD invariant violated: missing Rust 2024 marker");
    }
    if !TOOL_STANDARD.contains("OmniCode") {
        return Err("TOOL_STANDARD invariant violated: missing OmniCode marker");
    }
    Ok(())
}

/// Comprehensive diagnostic self-audit cascading through all operational
/// submodules in `tl_tools`:
///
/// 1. Verifies library constant invariants ([`audit_library_invariants`]).
/// 2. Dispatches asset pipeline self-audit ([`assets::self_audit`]).
/// 3. Dispatches CLI command sub-assembly self-audit ([`cmd::self_audit`]).
/// 4. Dispatches Master Guide builder self-audit ([`guide::self_audit`]).
/// 5. Dispatches multi-language lexical analyzer self-audit ([`syntax::self_audit`]).
/// 6. Dispatches OmniCode 5-Block validator self-audit ([`validator::self_audit`]).
///
/// # Returns
/// * `Ok(())` - All submodules reported healthy and ready for execution.
/// * `Err(String)` - First encountered error detailing the failing subsystem.
///
/// # Errors
/// Returns a descriptive error string if any submodule fails its diagnostic probe.
///
/// # Invariants
/// - Deterministic fault isolation: failures in any child subsystem propagate
///   immediately without silent masking.
/// - Zero dynamic heap allocations beyond the error message string on failure.
///
/// # Dual-Audience Documentation
///
/// ### For Game Designers & Non-Programmers:
/// The comprehensive "pre-flight check" of the toolchain. Verifies that every
/// tool component (art checker, manual builder, code validator) is working.
///
/// ### For Systems Programmers & Engine Architects:
/// Root Sub-Assembly 4.1 gate. Chains probe calls across all 6 core modules,
/// enforcing fail-fast diagnostics across the entire crate before any batch
/// mutation or file generation commences.
///
/// # Examples
/// ```rust
/// use tl_tools::self_audit;
///
/// assert!(self_audit().is_ok());
/// ```
pub fn self_audit() -> Result<(), String> {
    audit_library_invariants().map_err(|e| e.to_string())?;
    assets::self_audit().map_err(|e| e.to_string())?;
    cmd::self_audit()?;
    guide::self_audit()?;
    syntax::self_audit()?;
    validator::self_audit().map_err(|e| e.to_string())?;
    Ok(())
}

// -----------------------------------------------------------------------------
// Sub-Assembly 4.2: Four-Section Ontological Test Suite
// -----------------------------------------------------------------------------

#[cfg(test)]
mod tests {
    use super::*;
    use std::fs;
    use std::path::{Path, PathBuf};

    // -------------------------------------------------------------------------
    // SECTION 1: ALWAYS PASS (Nominal Golden Surface Cogs)
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_1_always_pass_library_invariants() {
        assert!(audit_library_invariants().is_ok());
    }

    #[test]
    fn test_section_1_always_pass_default_runner_registration() {
        let repo = PathBuf::from(".");
        let archive = PathBuf::from(".");
        let ctx = RunnerContext::new(&repo, &archive);
        let engine = create_default_runner(ctx);
        assert_eq!(engine.context().repo_root, repo);
        assert_eq!(engine.task_count(), 8);
    }

    #[test]
    fn test_section_1_always_pass_tool_version_format() {
        assert!(TOOL_VERSION.starts_with("0.1.2"));
        assert!(TOOL_STANDARD.contains("Rust 2024"));
    }

    // -------------------------------------------------------------------------
    // SECTION 2: NEVER FAIL (Fault Inversion & Graceful Degradation)
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_2_never_fail_missing_paths_handled() {
        let bogus_repo = PathBuf::from("/nonexistent/bogus/path");
        let ctx = RunnerContext::new(&bogus_repo, &bogus_repo);
        let engine = create_default_runner(ctx);
        assert_eq!(engine.context().repo_root, bogus_repo);
    }

    #[test]
    fn test_section_2_never_fail_self_audit_diagnostic() {
        assert!(self_audit().is_ok());
    }

    // -------------------------------------------------------------------------
    // SECTION 3: INVARIANTS, SATURATION & LIMIT GUARDS
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_3_invariants_data_driven_log_manifest_integrity() {
        let candidate_paths = [
            Path::new("engine/config/diagnostics/telemetry/manifest.json"),
            Path::new("../engine/config/diagnostics/telemetry/manifest.json"),
            Path::new("game/data/system/diagnostics/telemetry/manifest.json"),
            Path::new("../game/data/system/diagnostics/telemetry/manifest.json"),
            Path::new("data/log/manifest.json"),
            Path::new("../data/log/manifest.json"),
        ];

        let resolved = match candidate_paths.iter().find(|p| p.exists()) {
            Some(p) => *p,
            None => return,
        };

        let content = fs::read_to_string(resolved).expect("Failed to read log manifest");
        let val: serde_json::Value =
            serde_json::from_str(&content).expect("Log manifest is not valid JSON");

        let default_setting = val["block_3_body"]["menu"]["default_setting"]
            .as_str()
            .or_else(|| val["menu"]["default_setting"].as_str())
            .expect("Missing default_setting");
        let settings = val["block_3_body"]["menu"]["settings"]
            .as_array()
            .or_else(|| val["menu"]["settings"].as_array())
            .expect("Settings must be an array");
        assert!(!settings.is_empty(), "Settings menu cannot be empty");

        let mut found_default = false;
        let base_dir = resolved.parent().unwrap();

        for item in settings {
            let id = item["id"].as_str().expect("Setting must have an id");
            let file = item["file"].as_str().expect("Setting must have a file");

            if id == default_setting {
                found_default = true;
            }

            let setting_path = base_dir.join(file);
            assert!(
                setting_path.exists(),
                "Setting file {:?} does not exist",
                setting_path
            );
            let setting_content =
                fs::read_to_string(&setting_path).expect("Failed to read setting file");
            let setting_val: serde_json::Value = serde_json::from_str(&setting_content)
                .expect("Setting file is not valid JSON");
            assert!(
                setting_val["block_3_body"]["setting"].is_object()
                    || setting_val["setting"].is_object(),
                "Missing 'setting' payload in {:?}",
                setting_path
            );
        }

        assert!(
            found_default,
            "Default setting '{}' was not found in registered settings",
            default_setting
        );
    }

    #[test]
    fn test_section_3_invariants_system_data_catalog_validation() {
        let candidate_roots = [Path::new("."), Path::new("..")];
        let root = candidate_roots
            .iter()
            .find(|p| p.join("engine/config").exists() || p.join("game/data/system").exists())
            .map(|p| p.canonicalize().unwrap_or_else(|_| p.to_path_buf()))
            .unwrap_or_else(|| PathBuf::from("."));

        let target_domain = if root.join("engine/config").exists() {
            "config".to_string()
        } else {
            "system".to_string()
        };

        let options = ValidationOptions {
            target_domain,
            strict: false,
            verbose: false,
            repo_root: root,
        };

        let validator = DataValidator::new(options);
        let summary = validator.run_audit().expect("Audit execution failed");
        assert!(
            summary.is_success(false),
            "System/config data validation failed with {} errors",
            summary.error_count()
        );
    }

    // -------------------------------------------------------------------------
    // SECTION 4: EMERGENCE & COMPOSITE WORKFLOWS
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_4_emergence_self_audit() {
        assert!(self_audit().is_ok());
        assert!(is_tools_lib_ready());
    }
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( Epilogue Handshake & Subsystem Readiness Probe )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 5.1: Epilogue Readiness Probe & Lifecycle Handshake
// -----------------------------------------------------------------------------

/// Epilogue readiness probe for the entire `tl_tools` library.
///
/// # Returns
/// * `true` - All library constants and submodules pass self-audits.
/// * `false` - At least one submodule or constant invariant failed.
///
/// # Dual-Audience Documentation
///
/// ### For Game Designers & Non-Programmers:
/// Returns a simple boolean verifying that the entire developer toolchain
/// library is completely healthy and ready for production use.
///
/// ### For Systems Programmers & Engine Architects:
/// Sub-Assembly 5.1 readiness probe executing a non-panicking query over
/// [`self_audit`]. Suitable for automated CI smoke tests, health monitoring,
/// and external orchestrators.
///
/// # Examples
/// ```rust
/// use tl_tools::is_tools_lib_ready;
///
/// assert!(is_tools_lib_ready());
/// ```
pub fn is_tools_lib_ready() -> bool {
    self_audit().is_ok()
}
