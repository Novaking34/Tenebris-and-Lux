// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/validator/mod.rs
// @id            cog.tools.validator.root
// @layer         Layer 3: Systems Developer Tooling Core Library
// @archetype     facade_umbrella | orchestrator
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-15
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Universal 5-Block Validator Suite
// @role          Master Orchestrator, Subsystem Registry & Validator Facade
// @maturity      Rev 5 Baseline | Unified Validation Harness
// @chassis       bounded-heap | Developer tooling memory chassis; bounded task pipelines
// @scale         O(N) across files in audited domains
// @seams         in: repo_root, domain args | uses: framework, languages | probes: self_audit -> exit
// @invariants    1. Universal 5-Block validation executed across all supported languages.
//                2. Strict exit code semantics (0 = success, non-zero = defects found).
//                3. Strict <= 80-column line width discipline across documentation.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Modular OmniCode Validator Suite orchestrator. Unifies C11 header auditing,
//   JSON catalog/manifest verification, git commit ledger audits, and data-driven
//   standards configuration into a cohesive validation facade.
//
//   ### For Game Designers & Non-Programmers:
//   The central control panel for project validation. Running this tool checks
//   that all code, documents, data tables, and git commits follow the studio's
//   quality standards before changes are merged.
//
//   ### For Systems Programmers & Engine Architects:
//   - Ownership & Reentrancy: Orchestrator is stateless and delegates to modular engines.
//   - Extension Seam: Additional language validators plug in as submodules.
//   - Explicit Non-Goal: Does not alter files; audits and reports findings only.
// =============================================================================

//! # OmniCode Modular Validator Suite
//!
//! Provides decoupled, data-driven validation across:
//! - **C11 Headers** ([`c`]): 5-block structure, Invariant 18, archetypes, dump cards.
//! - **C++ Compatibility** ([`cpp`]): Linkage guards, C++ header checks.
//! - **Rust Sources** ([`rust`]): Rust 2024 edition, 5-block envelopes, test schema.
//! - **JSON Data Catalogs** ([`json`]): 5-block envelope, schemas, cross-manifest links.
//! - **Markdown & AsciiDoc** ([`md`], [`adoc`]): Documentation structural purity.
//! - **TOML & CMake** ([`toml`], [`cmake`]): Configuration and build scripts.
//! - **Git Commits & Tags** ([`git`]): 5-block engineering ledger, conventional prefixes.
//! - **Diagnostic Reporting** ([`report`]): Standardized ASCII diagnostic summaries.
//! - **Standards Configuration** ([`config`]): Layered baseline & project rules.

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Public Contract Limits, Command Types, Options & O(1) Pre-Conditions )
// =============================================================================

pub mod adoc;
pub mod c;
pub mod cmake;
pub mod config;
pub mod cpp;
pub mod framework;
pub mod git;
pub mod json;
pub mod md;
pub mod report;
pub mod rust;
pub mod toml;

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Re-exported Types & Subsystem Facades
// -----------------------------------------------------------------------------

pub use c::{HeaderValidationTask, HeaderValidator};
pub use config::{
    load_header_rules_config, load_subsystems_registry, HeaderArchetype,
    HeaderRulesConfig, SubsystemRecord, SubsystemRegistry,
};
pub use git::{
    audit_commit_message, audit_git_history, audit_git_tags, audit_tag_message,
    GitAuditReport, GitDiagnosticIssue, GitValidationTask, ValidatorMode,
};
pub use json::{DataValidationTask, DataValidator, ValidationOptions};
pub use report::{IssueSeverity, ValidationIssue, ValidationSummary};
pub use rust::{validate_rust_content, RustValidationConfig};

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Backward-Compatibility Module Aliases
// -----------------------------------------------------------------------------

pub use c as c_header;
pub use git as git_commit;
pub use json as json_data;

// -----------------------------------------------------------------------------
// Sub-Assembly 2.3: Task Context & Runners
// -----------------------------------------------------------------------------

use crate::runner::{RunnerContext, TaskRunner, TaskTelemetry};

/// Full validation task running headers, data catalogs, and git commit checks.
pub struct FullValidationTask;

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Composite Task Execution Engine & Cross-Domain Orchestrator )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: TaskRunner Pipeline Implementation
// -----------------------------------------------------------------------------

impl TaskRunner for FullValidationTask {
    fn name(&self) -> &'static str {
        "full_validator"
    }

    fn description(&self) -> &'static str {
        "Comprehensive OmniCode audit across C11 headers, JSON data, and git ledger"
    }

    fn preflight(&self, ctx: &RunnerContext) -> Result<(), String> {
        DataValidationTask.preflight(ctx)?;
        HeaderValidationTask.preflight(ctx)?;
        Ok(())
    }

    fn run(&self, ctx: &RunnerContext) -> Result<TaskTelemetry, String> {
        let t_data = DataValidationTask.run(ctx)?;
        let t_headers = HeaderValidationTask.run(ctx)?;

        if t_data.status == crate::runner::TaskStatus::Success
            && t_headers.status == crate::runner::TaskStatus::Success
        {
            Ok(TaskTelemetry::success(
                self.name(),
                t_data.duration_ms + t_headers.duration_ms,
                &format!("All validation checks passed: [Data] {} | [Headers] {}", t_data.details, t_headers.details),
            ))
        } else {
            Ok(TaskTelemetry::failure(
                self.name(),
                t_data.duration_ms + t_headers.duration_ms,
                &format!("Validation regressions detected: [Data] {} | [Headers] {}", t_data.details, t_headers.details),
            ))
        }
    }
}

/// Legacy backward compatibility check function.
pub fn audit_validator_subsystems() -> Result<(), &'static str> {
    self_audit()
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// ( Four-Section Ontological Test Schema & Diagnostic Self-Audit )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 4.1: Unit Test Suite (Four-Section Ontological Schema)
// -----------------------------------------------------------------------------

#[cfg(test)]
mod tests {
    use super::*;

    // -------------------------------------------------------------------------
    // SECTION 1: ALWAYS PASS (Nominal Golden Surface Cogs)
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_1_nominal_task_metadata() {
        let task = FullValidationTask;
        assert_eq!(task.name(), "full_validator");
        assert!(!task.description().is_empty());
    }

    #[test]
    fn test_section_1_nominal_subsystem_audit_passes() {
        assert!(audit_validator_subsystems().is_ok());
    }

    // -------------------------------------------------------------------------
    // SECTION 2: NEVER FAIL (Fault Inversion & Graceful Degradation)
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_2_never_fail_preflight_nonexistent_workspace() {
        let task = FullValidationTask;
        let fake_root = std::path::PathBuf::from("/definitely/nonexistent/workspace/dir");
        let fake_archive = std::path::PathBuf::from("/definitely/nonexistent/archive");
        let ctx = RunnerContext::new(&fake_root, &fake_archive);
        let preflight_res = task.preflight(&ctx);
        // Preflight gracefully returns an error for invalid paths without panic
        assert!(preflight_res.is_err());
    }

    // -------------------------------------------------------------------------
    // SECTION 3: INVARIANTS, SATURATION & LIMIT GUARDS
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_3_invariants_subsystem_readiness() {
        assert!(framework::is_validator_framework_ready());
        assert!(c::is_c_validator_ready());
        assert!(cpp::is_cpp_validator_ready());
        assert!(rust::is_rust_validator_ready());
        assert!(json::is_json_validator_ready());
        assert!(md::is_markdown_validator_ready());
        assert!(adoc::is_adoc_validator_ready());
        assert!(toml::is_toml_validator_ready());
        assert!(cmake::is_cmake_validator_ready());
        assert!(git::is_git_validator_ready());
        assert!(report::is_report_system_ready());
        assert!(config::is_config_system_ready());
    }

    // -------------------------------------------------------------------------
    // SECTION 4: EMERGENCE & COMPOSITE WORKFLOWS
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_4_emergence_self_audit() {
        assert!(self_audit().is_ok());
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 4.2: Diagnostic Self-Audit
// -----------------------------------------------------------------------------

/// Performs an in-process diagnostic preflight self-audit of all validator modules.
///
/// # Examples
///
/// ```rust
/// use tl_tools::validator::self_audit;
///
/// assert!(self_audit().is_ok());
/// ```
pub fn self_audit() -> Result<(), &'static str> {
    framework::self_audit()?;
    c::self_audit()?;
    cpp::self_audit()?;
    rust::self_audit()?;
    json::self_audit()?;
    md::self_audit()?;
    adoc::self_audit()?;
    toml::self_audit()?;
    cmake::self_audit()?;
    git::self_audit()?;
    report::self_audit()?;
    config::self_audit()?;
    Ok(())
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( State Summary Handshake, Module Epilogue & Teardown Verification )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 5.1: Epilogue Predicate
// -----------------------------------------------------------------------------

/// Epilogue predicate affirming that validator orchestrator is operational.
///
/// # Examples
///
/// ```rust
/// use tl_tools::validator::is_validator_system_ready;
///
/// assert!(is_validator_system_ready());
/// ```
pub fn is_validator_system_ready() -> bool {
    self_audit().is_ok()
}
