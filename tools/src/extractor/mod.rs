// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/extractor/mod.rs
// @id            cog.extractor
// @layer         Layer 3: Developer Tooling & Pipeline Orchestration
// @archetype     facade_umbrella | stateful
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-16
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        Tools Extractor Subsystem
// @role          Facade Coordinator & Pipeline Task Integration
// @maturity      Rev 5 Baseline | Hardened
// @chassis       zero-heap (stack-allocated facades and dispatch routing)
// @scale         O(1) facade dispatch over sub-assemblies
// @seams         in: TaskRunner | out: TaskTelemetry | probes: self_audit -> is_extractor_ready
// @invariants    1. Zero mutable global state in extraction coordination.
//                2. Strict delegation to child modules without leaking internal state.
//                3. Preflight errors fail-fast before attempting archive reads.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Modular archival extraction engine for the 2017-2018 RPG Maker MV demo
//   datasets of "Tenebris and Lux". Coordinates schema deserialization,
//   archival ingestion, and integrates with the `TaskRunner` pipeline.
//
// ### For Game Designers & Non-Programmers:
//   Serves as the primary coordinator for extracting old demo data into
//   canonical documentation and verified game assets.
//
// ### For Systems Programmers & Engine Architects:
//   Exposes a clean facade providing `ExtractorTask` as an in-memory task runner,
//   with comprehensive preflight checks and atomic execution guarantees.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Pre-Conditions, Module Declarations & Task Types )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Module Declarations & Public Re-exports
// -----------------------------------------------------------------------------

pub mod converter;
pub mod models;

pub use converter::{is_converter_ready, DemoExtractor};
pub use models::{
    is_models_ready, ActorData, ArmorData, ClassData, DamageData, EnemyData, ExtractionSummary,
    ItemData, LearningData, SkillData, SystemData, TroopData, WeaponData,
};

use crate::runner::{RunnerContext, TaskRunner, TaskStatus, TaskTelemetry};

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Pipeline Task Chassis
// -----------------------------------------------------------------------------

/// Modular runner task wrapping the DemoExtractor.
pub struct ExtractorTask;

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( TaskRunner Implementation & Orchestration )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: TaskRunner Contract Implementation
// -----------------------------------------------------------------------------

impl TaskRunner for ExtractorTask {
    fn name(&self) -> &'static str {
        "demo_extractor"
    }

    fn description(&self) -> &'static str {
        "Extracts and validates historical 2017-2018 RPG Maker MV demo baseline"
    }

    fn preflight(&self, ctx: &RunnerContext) -> Result<(), String> {
        if !ctx.archive_dir.is_dir() {
            return Err(format!(
                "Archive directory does not exist: {:?}",
                ctx.archive_dir
            ));
        }
        let f1 = ctx.archive_dir.join("Old Game Data/data");
        let f2 = ctx.archive_dir.join("Old_Game_Data_2/data");
        if !f1.is_dir() && !f2.is_dir() {
            return Err("Neither archive data directory (f1 or f2) is accessible.".to_string());
        }
        Ok(())
    }

    fn run(&self, ctx: &RunnerContext) -> Result<TaskTelemetry, String> {
        let extractor = DemoExtractor::new(&ctx.archive_dir, &ctx.output_dir);
        let summary = extractor.run()?;
        let details = format!(
            "Extracted: {} actors, {} classes, {} skills, {} items, {} enemies",
            summary.actor_count,
            summary.class_count,
            summary.skill_count,
            summary.item_count,
            summary.enemy_count
        );
        Ok(TaskTelemetry::success(self.name(), 0, &details))
    }

    fn post_verify(&self, _ctx: &RunnerContext, telemetry: &TaskTelemetry) -> Result<(), String> {
        if telemetry.status != TaskStatus::Success {
            return Err("Extraction failed status check.".to_string());
        }
        Ok(())
    }
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// ( Four-Section Ontological Test Suite & Subsystem Self-Audit )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 4.1: Internal Subsystem Self-Audit Probe
// -----------------------------------------------------------------------------

/// Comprehensive self-audit checking child modules and task readiness.
pub fn self_audit() -> Result<(), String> {
    models::self_audit().map_err(|e| e.to_string())?;
    converter::self_audit().map_err(|e| e.to_string())?;

    let task = ExtractorTask;
    if task.name() != "demo_extractor" {
        return Err("Self-audit failed: task name mismatch".to_string());
    }

    Ok(())
}

// -----------------------------------------------------------------------------
// Sub-Assembly 4.2: Four-Section Ontological Unit Tests
// -----------------------------------------------------------------------------

#[cfg(test)]
mod tests {
    use super::*;
    use std::path::PathBuf;

    // -------------------------------------------------------------------------
    // Section 1: Always Pass (Golden Surface Path)
    // -------------------------------------------------------------------------
    #[test]
    fn test_section_1_always_pass_task_metadata() {
        let task = ExtractorTask;
        assert_eq!(task.name(), "demo_extractor");
        assert!(task.description().contains("2017-2018"));
    }

    // -------------------------------------------------------------------------
    // Section 2: Never Fail (Edge Resilience & Fault Inversion)
    // -------------------------------------------------------------------------
    #[test]
    fn test_section_2_never_fail_preflight_missing_archive() {
        let ctx = RunnerContext::new(
            &PathBuf::from("/nonexistent/repo"),
            &PathBuf::from("/nonexistent/archive"),
        );
        let task = ExtractorTask;
        let res = task.preflight(&ctx);
        assert!(res.is_err());
    }

    // -------------------------------------------------------------------------
    // Section 3: Invariants (Structural Bounds & Mathematical Guards)
    // -------------------------------------------------------------------------
    #[test]
    fn test_section_3_invariants_post_verify_status_guard() {
        let ctx = RunnerContext::new(&PathBuf::from("."), &PathBuf::from("."));
        let task = ExtractorTask;
        let failed_tel = TaskTelemetry::failure("demo_extractor", 10, "Simulated failure");
        assert!(task.post_verify(&ctx, &failed_tel).is_err());

        let success_tel = TaskTelemetry::success("demo_extractor", 10, "Success");
        assert!(task.post_verify(&ctx, &success_tel).is_ok());
    }

    // -------------------------------------------------------------------------
    // Section 4: Emergence (System Integration & Roundtrip Synthesis)
    // -------------------------------------------------------------------------
    #[test]
    fn test_section_4_emergence_self_audit() {
        assert!(self_audit().is_ok());
    }
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( Epilogue Readiness & Subsystem Handshake )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 5.1: Epilogue Readiness Probe
// -----------------------------------------------------------------------------

/// Returns true if all extractor sub-assemblies are ready.
pub fn is_extractor_ready() -> bool {
    is_models_ready() && is_converter_ready()
}
