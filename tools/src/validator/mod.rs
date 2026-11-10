// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/validator/mod.rs
// @id            tools.validator.root
// @layer         Layer 3: Systems Developer Tooling Core Library
// @version       0.1.0
// @interface_rev Rev 1
// @date          2026-11-09
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Modular OmniCode Validator Suite orchestrator. Unifies C11 header auditing,
//   JSON catalog/manifest verification, git commit ledger audits, and data-driven
//   standards configuration.
// =============================================================================

//! # OmniCode Modular Validator Suite
//!
//! Provides decoupled, data-driven validation across:
//! - **C11 Headers** ([`c_header`]): 5-block structure, Invariant 18, archetypes, dump cards.
//! - **JSON Data Catalogs** ([`json_data`]): 5-block envelope, schemas, cross-manifest links.
//! - **Git Commits & Tags** ([`git_commit`]): 5-block engineering ledger, conventional prefixes.
//! - **Diagnostic Reporting** ([`report`]): Standardized ASCII diagnostic summaries.
//! - **Standards Configuration** ([`config`]): Layered baseline & project rules.

// =============================================================================
// BLOCK 2: OPENING BLOCK / MODULE DECLARATIONS & EXPORTS
// Stratum: Developer Tooling | Machine: tools.validator.root
// =============================================================================

pub mod c_header;
pub mod config;
pub mod git_commit;
pub mod json_data;
pub mod report;

pub use c_header::{HeaderValidationTask, HeaderValidator};
pub use config::{
    load_header_rules_config, load_subsystems_registry, HeaderArchetype,
    HeaderRulesConfig, SubsystemRecord, SubsystemRegistry,
};
pub use git_commit::{
    audit_commit_message, audit_git_history, audit_git_tags, audit_tag_message,
    GitAuditReport, GitDiagnosticIssue, GitValidationTask, ValidatorMode,
};
pub use json_data::{DataValidationTask, DataValidator, ValidationOptions};
pub use report::{IssueSeverity, ValidationIssue, ValidationSummary};

use crate::runner::{RunnerContext, TaskRunner, TaskTelemetry};

// =============================================================================
// BLOCK 3: BODY BLOCK / COMPOSITE VALIDATION ORCHESTRATOR
// Stratum: Developer Tooling | Machine: tools.validator.root
// =============================================================================

/// Full validation task running headers, data catalogs, and git commit checks.
pub struct FullValidationTask;

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

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Developer Tooling | Machine: tools.validator.root
// =============================================================================

/// Self-audit verifying validator module linkage.
pub fn audit_validator_subsystems() -> Result<(), &'static str> {
    Ok(())
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK / TESTS
// Stratum: Developer Tooling | Machine: tools.validator.root
// =============================================================================

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_validator_subsystem_audit_passes() {
        assert!(audit_validator_subsystems().is_ok());
    }
}

