// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/runner.rs
// @id            tools.runner.orchestrator
// @layer         Layer 3: Developer Tooling & Pipeline Orchestration
// @version       0.1.0
// @interface_rev Rev 1
// @date          2026-10-29
// @updated       2026-10-29
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Modular task runner framework for Tenebris and Lux. Provides an extensible,
//   type-safe pipeline orchestrator decoupling high-level CLI dispatching from
//   isolated execution units (historical extraction, documentation compilers,
//   curation audits, and compilation suites).
//
// PEDAGOGICAL MISSION:
//   Demonstrates why a compiled, modular runner is architecturally superior to
//   scattered shell scripts or ad-hoc subprocess spawning:
//     1. Type-Safe Invariant Contracts: Each task explicitly declares its
//        pre-flight prerequisites, execution logic, and post-flight telemetry.
//     2. Zero-Process Overhead: Tasks execute within the same address space
//        where appropriate, avoiding fragile sub-shelling and process spawns.
//     3. Unified Telemetry: Standardized telemetry manifests capture execution
//        timings, diagnostic verdicts, and artifact manifests.
//
// AXIOMATIC INVARIANTS:
//   1. Monotonic elapsed timing on all task executions.
//   2. Preflight errors fail-fast before mutating any workspace state.
//   3. Strict <= 80-column line width discipline across all documentation.
//   4. Pure, deterministic report generation with no terminal escape leakage.
// =============================================================================

//! # Modular Task Runner Framework
//!
//! The `runner` module defines the core execution contracts for the
//! `tl_tools` developer suite. It models tasks as independent, unit-testable
//! components that satisfy the [`TaskRunner`] trait.

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES & TRAITS
// Stratum: Developer Tooling | Machine: tools.runner.rust
// =============================================================================

use std::fmt;
use std::path::{Path, PathBuf};
use std::time::Instant;

/// Execution status of an individual runner task.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum TaskStatus {
    /// Task completed with all invariants satisfied.
    Success,
    /// Task failed during execution or invariant validation.
    Failure,
    /// Task was skipped due to unmet preconditions.
    Skipped,
}

impl fmt::Display for TaskStatus {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            TaskStatus::Success => write!(f, "[PASS]"),
            TaskStatus::Failure => write!(f, "[FAIL]"),
            TaskStatus::Skipped => write!(f, "[SKIP]"),
        }
    }
}

/// Execution telemetry captured during a task run.
#[derive(Debug, Clone)]
pub struct TaskTelemetry {
    /// Human-readable identifier of the task.
    pub task_name: String,
    /// Wall-clock duration in milliseconds.
    pub duration_ms: u64,
    /// Final execution status.
    pub status: TaskStatus,
    /// Telemetry details or diagnostic message.
    pub details: String,
    /// List of file artifacts generated or verified by this task.
    pub artifacts: Vec<PathBuf>,
}

impl TaskTelemetry {
    /// Creates a successful telemetry record.
    pub fn success(name: &str, duration_ms: u64, details: &str) -> Self {
        Self {
            task_name: name.to_string(),
            duration_ms,
            status: TaskStatus::Success,
            details: details.to_string(),
            artifacts: Vec::new(),
        }
    }

    /// Creates a failure telemetry record.
    pub fn failure(name: &str, duration_ms: u64, details: &str) -> Self {
        Self {
            task_name: name.to_string(),
            duration_ms,
            status: TaskStatus::Failure,
            details: details.to_string(),
            artifacts: Vec::new(),
        }
    }

    /// Appends an artifact path to the telemetry record.
    pub fn with_artifact(mut self, path: PathBuf) -> Self {
        self.artifacts.push(path);
        self
    }
}

/// Shared context passed into all runner tasks.
#[derive(Debug, Clone)]
pub struct RunnerContext {
    /// Root path of the repository workspace.
    pub repo_root: PathBuf,
    /// Safe archive path containing historical RPG Maker datasets.
    pub archive_dir: PathBuf,
    /// Documentation directory (`docs/`).
    pub docs_dir: PathBuf,
    /// Target baseline documentation directory.
    pub output_dir: PathBuf,
    /// Verbose diagnostic output flag.
    pub verbose: bool,
}

impl RunnerContext {
    /// Constructs a standard runner context from a resolved repository root.
    pub fn new(repo_root: &Path, archive_dir: &Path) -> Self {
        Self {
            repo_root: repo_root.to_path_buf(),
            archive_dir: archive_dir.to_path_buf(),
            docs_dir: repo_root.join("docs"),
            output_dir: repo_root.join("docs/game_design/demo_baseline"),
            verbose: false,
        }
    }
}

/// Core interface contract for all modular developer tooling tasks.
///
/// Any utility (historical extraction, documentation compiler, C build runner,
/// or diagnostic validator) implements this trait to plug into the pipeline.
pub trait TaskRunner: Send + Sync {
    /// Unique identifier for CLI dispatching and logging.
    fn name(&self) -> &'static str;

    /// Architectural summary explaining the task's role.
    fn description(&self) -> &'static str;

    /// Pre-flight sanity check verifying environment prerequisites.
    ///
    /// Must return `Ok(())` before `run()` is invoked, or `Err` with a
    /// clear diagnostic message explaining why the task cannot proceed.
    fn preflight(&self, ctx: &RunnerContext) -> Result<(), String>;

    /// Executes the task logic and returns structured execution telemetry.
    fn run(&self, ctx: &RunnerContext) -> Result<TaskTelemetry, String>;

    /// Optional post-flight verification asserting artifact integrity.
    fn post_verify(
        &self,
        _ctx: &RunnerContext,
        _telemetry: &TaskTelemetry,
    ) -> Result<(), String> {
        Ok(())
    }
}

/// Summary report accumulating telemetry from multiple pipeline tasks.
#[derive(Debug, Default)]
pub struct PipelineReport {
    /// List of telemetry records in execution order.
    pub records: Vec<TaskTelemetry>,
}

impl PipelineReport {
    /// Adds a telemetry record to the report.
    pub fn add(&mut self, record: TaskTelemetry) {
        self.records.push(record);
    }

    /// Returns `true` if all executed tasks succeeded.
    pub fn is_success(&self) -> bool {
        self.records.iter().all(|r| r.status == TaskStatus::Success)
    }

    /// Formats a pedagogical report table matching the C pre-flight layout.
    pub fn display_summary(&self) {
        println!("================================================================================");
        println!("           Tenebris and Lux - Pipeline Runner Telemetry Report           ");
        println!("================================================================================");
        println!("{:<20} {:<8} {:<10} {}", "TASK NAME", "STATUS", "TIME (ms)", "DETAILS");
        println!("--------------------------------------------------------------------------------");
        for r in &self.records {
            println!(
                "{:<20} {:<8} {:<10} {}",
                r.task_name,
                r.status,
                r.duration_ms,
                r.details
            );
        }
        println!("================================================================================");
        let passed = self.records.iter().filter(|r| r.status == TaskStatus::Success).count();
        let total = self.records.len();
        println!("Pipeline Verdict: {}/{} tasks passed.", passed, total);
        println!("================================================================================");
    }
}

// =============================================================================
// BLOCK 3: BODY BLOCK / RUNNER ENGINE
// Stratum: Developer Tooling | Machine: tools.runner.rust
// =============================================================================

/// Pipeline orchestrator engine managing and executing registered tasks.
pub struct RunnerEngine {
    context: RunnerContext,
    tasks: Vec<Box<dyn TaskRunner>>,
}

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
// Stratum: Developer Tooling | Machine: tools.runner.rust
// =============================================================================

/// Self-audit task verifying workspace directory topology and prerequisites.
pub struct EnvironmentAuditTask;

impl TaskRunner for EnvironmentAuditTask {
    fn name(&self) -> &'static str {
        "env_audit"
    }

    fn description(&self) -> &'static str {
        "Verifies workspace directory topology, archives, and build manifest integrity"
    }

    fn preflight(&self, ctx: &RunnerContext) -> Result<(), String> {
        if !ctx.repo_root.is_dir() {
            return Err(format!("Repository root is invalid: {:?}", ctx.repo_root));
        }
        Ok(())
    }

    fn run(&self, ctx: &RunnerContext) -> Result<TaskTelemetry, String> {
        let checks = [
            ("Repository Root", ctx.repo_root.is_dir()),
            ("Docs Directory", ctx.docs_dir.is_dir()),
            ("Tools Directory", ctx.repo_root.join("tools").is_dir()),
            ("Archive Directory", ctx.archive_dir.is_dir()),
            ("Cargo Manifest", ctx.repo_root.join("tools/Cargo.toml").is_file()),
        ];

        let mut passed = 0;
        for (label, ok) in checks {
            let mark = if ok { "[PASS]" } else { "[FAIL]" };
            println!("  {} Check: {}", mark, label);
            if ok {
                passed += 1;
            }
        }

        if passed == checks.len() {
            Ok(TaskTelemetry::success(
                self.name(),
                0,
                &format!("All {} environment topology checks verified.", checks.len()),
            ))
        } else {
            Err(format!(
                "Environment check failed: only {}/{} passed.",
                passed,
                checks.len()
            ))
        }
    }
}

/// Task verifying the presence and integrity of game design curation docs.
pub struct CurationAuditTask;

impl TaskRunner for CurationAuditTask {
    fn name(&self) -> &'static str {
        "curation_audit"
    }

    fn description(&self) -> &'static str {
        "Validates the game design curation specification and triaging matrix"
    }

    fn preflight(&self, ctx: &RunnerContext) -> Result<(), String> {
        if !ctx.docs_dir.is_dir() {
            return Err(format!("Docs directory does not exist: {:?}", ctx.docs_dir));
        }
        Ok(())
    }

    fn run(&self, ctx: &RunnerContext) -> Result<TaskTelemetry, String> {
        let curation_doc = ctx
            .docs_dir
            .join("game_design/demo_baseline/game_design_curation.adoc");

        if curation_doc.exists() {
            println!("  [PASS] Curation specification found: {:?}", curation_doc);
            Ok(TaskTelemetry::success(
                self.name(),
                0,
                "Game design curation matrix verified.",
            )
            .with_artifact(curation_doc))
        } else {
            Err(format!("Curation document missing at: {:?}", curation_doc))
        }
    }
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK / TESTS & INVARIANT VERIFICATION
// Stratum: Developer Tooling | Machine: tools.runner.rust
// =============================================================================

#[cfg(test)]
mod tests {
    use super::*;

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

    #[test]
    fn test_runner_context_initialization() {
        let repo = PathBuf::from("/tmp/repo");
        let archive = PathBuf::from("/tmp/archive");
        let ctx = RunnerContext::new(&repo, &archive);
        assert_eq!(ctx.docs_dir, repo.join("docs"));
        assert_eq!(ctx.output_dir, repo.join("docs/game_design/demo_baseline"));
    }

    #[test]
    fn test_successful_task_execution() {
        let ctx = RunnerContext::new(&PathBuf::from("."), &PathBuf::from("."));
        let mut engine = RunnerEngine::new(ctx);
        engine.register(DummyTask { should_fail: false });

        let res = engine.run_task("dummy_task");
        assert!(res.is_ok());
        let tel = res.unwrap();
        assert_eq!(tel.status, TaskStatus::Success);
        assert_eq!(tel.task_name, "dummy_task");
    }

    #[test]
    fn test_preflight_failure_halts_task() {
        let ctx = RunnerContext::new(&PathBuf::from("."), &PathBuf::from("."));
        let mut engine = RunnerEngine::new(ctx);
        engine.register(DummyTask { should_fail: true });

        let res = engine.run_task("dummy_task");
        assert!(res.is_err());
        assert!(res.unwrap_err().contains("Preflight intentional error"));
    }
}
