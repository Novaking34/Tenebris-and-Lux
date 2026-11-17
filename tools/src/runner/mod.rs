// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/runner/mod.rs
// @id            cog.runner
// @layer         Layer 3: Developer Tooling & Pipeline Orchestration
// @archetype     facade_umbrella | stateful
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-16
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        Tools Runner Subsystem
// @role          Facade Coordinator & Pipeline Orchestration Engine
// @maturity      Rev 5 Baseline | Hardened
// @chassis       zero-heap (stack-managed registration vectors and context passing)
// @scale         O(M) across registered tasks in pipeline
// @seams         in: TaskRunner implementations | out: PipelineReport | probes: self_audit -> is_runner_ready
// @invariants    1. Monotonic elapsed timing on all task executions.
//                2. Preflight errors fail-fast before mutating any workspace state.
//                3. Pure, deterministic report generation with no terminal escape leakage.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Modular task runner framework for Tenebris and Lux. Provides an extensible,
//   type-safe pipeline orchestrator decoupling high-level CLI dispatching from
//   isolated execution units (historical extraction, documentation compilers,
//   curation audits, and compilation suites).
//
// ### For Game Designers & Non-Programmers:
//   The central pipeline manager that runs tasks in order, stops if any safety
//   check fails, and prints a summary table showing pass/fail status.
//
// ### For Systems Programmers & Engine Architects:
//   Coordinates registered `TaskRunner` implementations in sequence or on demand,
//   managing preflight, timing, telemetry capture, and invariant verification.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Pre-Conditions, Module Declarations & Engine Chassis )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Module Declarations & Public Re-exports
// -----------------------------------------------------------------------------

pub mod context;
pub mod report;
pub mod tasks;
pub mod traits;

pub use context::{is_context_ready, RunnerContext};
pub use report::{is_report_ready, PipelineReport};
pub use tasks::{is_tasks_ready, CurationAuditTask, EnvironmentAuditTask};
pub use traits::{is_traits_ready, TaskRunner, TaskStatus, TaskTelemetry};

use std::time::Instant;

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Pipeline Orchestrator Chassis
// -----------------------------------------------------------------------------

/// Pipeline orchestrator engine managing and executing registered tasks.
pub struct RunnerEngine {
    context: RunnerContext,
    tasks: Vec<Box<dyn TaskRunner>>,
}

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Registration & Execution Engine )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: Engine Constructor & Task Registration
// -----------------------------------------------------------------------------

impl RunnerEngine {
    /// Initializes a new runner engine with the specified context.
    pub fn new(context: RunnerContext) -> Self {
        Self {
            context,
            tasks: Vec::new(),
        }
    }

    /// Registers a new task runner into the engine.
    pub fn register<T: TaskRunner + 'static>(&mut self, task: T) -> &mut Self {
        self.tasks.push(Box::new(task));
        self
    }

    /// Returns a reference to the active runner context.
    pub fn context(&self) -> &RunnerContext {
        &self.context
    }

    /// Returns the number of registered tasks in this runner engine instance.
    ///
    /// # Returns
    /// An unsigned integer representing the count of queued [`TaskRunner`] items.
    ///
    /// # Dual-Audience Documentation
    ///
    /// ### For Game Designers & Non-Programmers:
    /// Reports how many pipeline tasks are scheduled to run in this workflow.
    ///
    /// ### For Systems Programmers & Engine Architects:
    /// O(1) query over the internal `Vec<Box<dyn TaskRunner>>` capacity.
    pub fn task_count(&self) -> usize {
        self.tasks.len()
    }

// -----------------------------------------------------------------------------
// Sub-Assembly 3.2: Execution Engine
// -----------------------------------------------------------------------------

    /// Executes an individual task by name.
    pub fn run_task(&self, name: &str) -> Result<TaskTelemetry, String> {
        let task = self
            .tasks
            .iter()
            .find(|t| t.name() == name)
            .ok_or_else(|| format!("Task '{}' not found in runner registry", name))?;

        println!(">>> [PREFLIGHT] Validating prerequisites for: {}", task.name());
        task.preflight(&self.context)?;

        println!(">>> [EXECUTE] Running task: {} ({})", task.name(), task.description());
        let start = Instant::now();
        let mut telemetry = task.run(&self.context)?;
        let elapsed = start.elapsed().as_millis() as u64;
        telemetry.duration_ms = elapsed;

        println!(">>> [POSTFLIGHT] Verifying invariants for: {}", task.name());
        task.post_verify(&self.context, &telemetry)?;

        Ok(telemetry)
    }

    /// Executes all registered tasks in sequence as a unified pipeline.
    pub fn run_pipeline(&self) -> Result<PipelineReport, String> {
        let mut report = PipelineReport::default();
        println!("=== Starting Tenebris and Lux Unified Toolchain Pipeline ===");

        for task in &self.tasks {
            println!("\n--- Step: {} ---", task.name());
            let start = Instant::now();
            let preflight_res = task.preflight(&self.context);

            if let Err(pre_err) = preflight_res {
                let elapsed = start.elapsed().as_millis() as u64;
                eprintln!("  [PREFLIGHT FAIL] {}", pre_err);
                report.add(TaskTelemetry::failure(task.name(), elapsed, &pre_err));
                continue;
            }

            match task.run(&self.context) {
                Ok(mut tel) => {
                    tel.duration_ms = start.elapsed().as_millis() as u64;
                    if let Err(verify_err) = task.post_verify(&self.context, &tel) {
                        eprintln!("  [POST-VERIFY FAIL] {}", verify_err);
                        tel.status = TaskStatus::Failure;
                        tel.details = format!("Post-verify failure: {}", verify_err);
                    }
                    report.add(tel);
                }
                Err(run_err) => {
                    let elapsed = start.elapsed().as_millis() as u64;
                    eprintln!("  [RUN FAIL] {}", run_err);
                    report.add(TaskTelemetry::failure(task.name(), elapsed, &run_err));
                }
            }
        }

        report.display_summary();
        if report.is_success() {
            Ok(report)
        } else {
            Err("One or more pipeline tasks failed invariant verification.".to_string())
        }
    }
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// ( Four-Section Ontological Test Suite & Subsystem Self-Audit )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 4.1: Internal Subsystem Self-Audit Probe
// -----------------------------------------------------------------------------

/// Internal self-audit testing runner subsystem components.
pub fn self_audit() -> Result<(), String> {
    traits::self_audit().map_err(|e| e.to_string())?;
    context::self_audit().map_err(|e| e.to_string())?;
    report::self_audit().map_err(|e| e.to_string())?;
    tasks::self_audit().map_err(|e| e.to_string())?;

    Ok(())
}

// -----------------------------------------------------------------------------
// Sub-Assembly 4.2: Four-Section Ontological Unit Tests
// -----------------------------------------------------------------------------

#[cfg(test)]
mod tests {
    use super::*;
    use std::path::PathBuf;

    struct DummyTask {
        should_fail: bool,
    }

    impl TaskRunner for DummyTask {
        fn name(&self) -> &'static str {
            "dummy_task"
        }

        fn description(&self) -> &'static str {
            "Test dummy verifying pipeline dispatch invariants"
        }

        fn preflight(&self, _ctx: &RunnerContext) -> Result<(), String> {
            if self.should_fail {
                Err("Preflight intentional error".to_string())
            } else {
                Ok(())
            }
        }

        fn run(&self, _ctx: &RunnerContext) -> Result<TaskTelemetry, String> {
            Ok(TaskTelemetry::success(self.name(), 5, "Dummy executed successfully"))
        }
    }

    // -------------------------------------------------------------------------
    // Section 1: Always Pass (Golden Surface Path)
    // -------------------------------------------------------------------------
    #[test]
    fn test_section_1_always_pass_successful_task_execution() {
        let ctx = RunnerContext::new(&PathBuf::from("."), &PathBuf::from("."));
        let mut engine = RunnerEngine::new(ctx);
        engine.register(DummyTask { should_fail: false });

        let res = engine.run_task("dummy_task");
        assert!(res.is_ok());
        let tel = res.unwrap();
        assert_eq!(tel.status, TaskStatus::Success);
        assert_eq!(tel.task_name, "dummy_task");
    }

    // -------------------------------------------------------------------------
    // Section 2: Never Fail (Edge Resilience & Fault Inversion)
    // -------------------------------------------------------------------------
    #[test]
    fn test_section_2_never_fail_preflight_failure_halts_task() {
        let ctx = RunnerContext::new(&PathBuf::from("."), &PathBuf::from("."));
        let mut engine = RunnerEngine::new(ctx);
        engine.register(DummyTask { should_fail: true });

        let res = engine.run_task("dummy_task");
        assert!(res.is_err());
        assert!(res.unwrap_err().contains("Preflight intentional error"));
    }

    // -------------------------------------------------------------------------
    // Section 3: Invariants (Structural Bounds & Mathematical Guards)
    // -------------------------------------------------------------------------
    #[test]
    fn test_section_3_invariants_unknown_task_error() {
        let ctx = RunnerContext::new(&PathBuf::from("."), &PathBuf::from("."));
        let engine = RunnerEngine::new(ctx);
        let res = engine.run_task("nonexistent");
        assert!(res.is_err());
        assert!(res.unwrap_err().contains("not found"));
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

/// Returns true if all runner sub-assemblies are ready.
pub fn is_runner_ready() -> bool {
    is_traits_ready() && is_context_ready() && is_report_ready() && is_tasks_ready()
}
