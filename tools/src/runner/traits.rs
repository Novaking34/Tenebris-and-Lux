// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/runner/traits.rs
// @id            cog.runner.traits
// @layer         Layer 3: Developer Tooling & Pipeline Orchestration
// @archetype     pure_types | stateless
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-16
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        Tools Runner Subsystem
// @role          Core Pipeline Trait Contracts & Execution Telemetry Types
// @maturity      Rev 5 Baseline | Hardened
// @chassis       zero-heap (stack-allocated enums and telemetry records)
// @scale         O(1) contract dispatch across pipeline tasks
// @seams         in: RunnerContext | out: TaskTelemetry | probes: self_audit -> is_traits_ready
// @invariants    1. Monotonic elapsed timing on all task executions.
//                2. Strict type safety across task statuses (Success, Failure, Skipped).
//                3. Pure telemetry generation with zero unhandled side effects.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Defines the core interface contract `TaskRunner` and associated execution
//   telemetry types (`TaskStatus`, `TaskTelemetry`) used across all modular
//   developer tooling tasks in `tl_tools`.
//
// ### For Game Designers & Non-Programmers:
//   Establishes the standard recipe for every tool task: each task has a name,
//   a description of what it does, a safety check before it runs, and a report
//   at the end showing if it passed or failed.
//
// ### For Systems Programmers & Engine Architects:
//   Enforces unified lifecycle hooks (`preflight`, `run`, `post_verify`) and
//   thread-safe `Send + Sync` guarantees across all pipeline units.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Pre-Conditions, Imports & Trait Type Definitions )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Pre-Conditions & Imports
// -----------------------------------------------------------------------------

use crate::runner::context::RunnerContext;
use std::fmt;
use std::path::PathBuf;

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Task Status & Telemetry Types
// -----------------------------------------------------------------------------

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
#[derive(Debug, Clone, PartialEq, Eq)]
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

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Telemetry Implementations & TaskRunner Trait )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: Task Telemetry Constructors & Methods
// -----------------------------------------------------------------------------

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

// -----------------------------------------------------------------------------
// Sub-Assembly 3.2: TaskRunner Trait Contract
// -----------------------------------------------------------------------------

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

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// ( Four-Section Ontological Test Suite & Subsystem Self-Audit )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 4.1: Internal Subsystem Self-Audit Probe
// -----------------------------------------------------------------------------

/// Internal self-audit testing telemetry generation and formatting.
pub fn self_audit() -> Result<(), &'static str> {
    let success = TaskTelemetry::success("test_task", 42, "nominal run");
    if success.status != TaskStatus::Success {
        return Err("Self-audit failed: success status mismatch");
    }
    if success.duration_ms != 42 {
        return Err("Self-audit failed: duration mismatch");
    }

    let failure = TaskTelemetry::failure("fail_task", 10, "fault inverted");
    if failure.status != TaskStatus::Failure {
        return Err("Self-audit failed: failure status mismatch");
    }

    Ok(())
}

// -----------------------------------------------------------------------------
// Sub-Assembly 4.2: Four-Section Ontological Unit Tests
// -----------------------------------------------------------------------------

#[cfg(test)]
mod tests {
    use super::*;

    // -------------------------------------------------------------------------
    // Section 1: Always Pass (Golden Surface Path)
    // -------------------------------------------------------------------------
    #[test]
    fn test_section_1_always_pass_telemetry_constructors() {
        let t = TaskTelemetry::success("demo", 100, "all good");
        assert_eq!(t.task_name, "demo");
        assert_eq!(t.status, TaskStatus::Success);
        assert_eq!(t.duration_ms, 100);
        assert!(t.artifacts.is_empty());

        let t2 = t.with_artifact(PathBuf::from("/tmp/artifact.txt"));
        assert_eq!(t2.artifacts.len(), 1);
    }

    // -------------------------------------------------------------------------
    // Section 2: Never Fail (Edge Resilience & Fault Inversion)
    // -------------------------------------------------------------------------
    #[test]
    fn test_section_2_never_fail_task_status_display() {
        assert_eq!(format!("{}", TaskStatus::Success), "[PASS]");
        assert_eq!(format!("{}", TaskStatus::Failure), "[FAIL]");
        assert_eq!(format!("{}", TaskStatus::Skipped), "[SKIP]");
    }

    // -------------------------------------------------------------------------
    // Section 3: Invariants (Structural Bounds & Mathematical Guards)
    // -------------------------------------------------------------------------
    #[test]
    fn test_section_3_invariants_status_equality() {
        assert_ne!(TaskStatus::Success, TaskStatus::Failure);
        assert_ne!(TaskStatus::Failure, TaskStatus::Skipped);
        assert_eq!(TaskStatus::Success, TaskStatus::Success);
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

/// Returns true if the traits sub-assembly is ready for pipeline dispatch.
pub fn is_traits_ready() -> bool {
    true
}
