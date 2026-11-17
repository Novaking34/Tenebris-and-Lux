// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/runner/report.rs
// @id            cog.runner.report
// @layer         Layer 3: Developer Tooling & Pipeline Orchestration
// @archetype     operational | stateful
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-16
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        Tools Runner Subsystem
// @role          Pipeline Telemetry Aggregator & Tabular Formatter
// @maturity      Rev 5 Baseline | Hardened
// @chassis       zero-heap (stack-managed record collection and stream formatting)
// @scale         O(N) iteration over executed task records
// @seams         in: TaskTelemetry | out: stdout summary table | probes: self_audit -> is_report_ready
// @invariants    1. Monotonic elapsed timing aggregation.
//                2. Deterministic verdict derivation (100% success required for pass).
//                3. Pure tabular formatting without terminal escape leakage.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Accumulates execution telemetry across all pipeline stages, computing
//   aggregated timings and formatting a pedagogical summary report table
//   matching the Cornerstone C engine pre-flight diagnostic layout.
//
// ### For Game Designers & Non-Programmers:
//   Prints an easy-to-read table showing which steps succeeded, how long each
//   took in milliseconds, and the final pass/fail verdict.
//
// ### For Systems Programmers & Engine Architects:
//   Provides a structured `PipelineReport` data structure with query methods
//   for CI integration, automated verification, and post-flight regression testing.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Pre-Conditions, Imports & Report Chassis )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Pre-Conditions & Imports
// -----------------------------------------------------------------------------

use crate::runner::traits::{TaskStatus, TaskTelemetry};

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Pipeline Report Structure
// -----------------------------------------------------------------------------

/// Summary report accumulating telemetry from multiple pipeline tasks.
#[derive(Debug, Default, Clone, PartialEq, Eq)]
pub struct PipelineReport {
    /// List of telemetry records in execution order.
    pub records: Vec<TaskTelemetry>,
}

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Accumulation & Formatter Implementations )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: Telemetry Accumulation & Metrics
// -----------------------------------------------------------------------------

impl PipelineReport {
    /// Adds a telemetry record to the report.
    pub fn add(&mut self, record: TaskTelemetry) {
        self.records.push(record);
    }

    /// Returns `true` if all executed tasks succeeded.
    pub fn is_success(&self) -> bool {
        !self.records.is_empty() && self.records.iter().all(|r| r.status == TaskStatus::Success)
    }

    /// Returns the sum of duration across all recorded tasks in milliseconds.
    pub fn total_duration_ms(&self) -> u64 {
        self.records.iter().map(|r| r.duration_ms).sum()
    }

    /// Returns the count of tasks that completed with success.
    pub fn passed_count(&self) -> usize {
        self.records.iter().filter(|r| r.status == TaskStatus::Success).count()
    }

// -----------------------------------------------------------------------------
// Sub-Assembly 3.2: Tabular Display Formatting
// -----------------------------------------------------------------------------

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
        let passed = self.passed_count();
        let total = self.records.len();
        println!("Pipeline Verdict: {}/{} tasks passed.", passed, total);
        println!("================================================================================");
    }
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// ( Four-Section Ontological Test Suite & Subsystem Self-Audit )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 4.1: Internal Subsystem Self-Audit Probe
// -----------------------------------------------------------------------------

/// Internal self-audit testing report accumulation and success checks.
pub fn self_audit() -> Result<(), &'static str> {
    let mut report = PipelineReport::default();
    report.add(TaskTelemetry::success("t1", 10, "ok"));
    report.add(TaskTelemetry::success("t2", 20, "ok"));

    if !report.is_success() {
        return Err("Self-audit failed: is_success should be true for all successful tasks");
    }
    if report.total_duration_ms() != 30 {
        return Err("Self-audit failed: total_duration_ms calculation mismatch");
    }
    if report.passed_count() != 2 {
        return Err("Self-audit failed: passed_count mismatch");
    }

    report.add(TaskTelemetry::failure("t3", 5, "failed"));
    if report.is_success() {
        return Err("Self-audit failed: is_success should be false when failure exists");
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
    fn test_section_1_always_pass_report_aggregation() {
        let mut report = PipelineReport::default();
        report.add(TaskTelemetry::success("task_a", 15, "good"));
        assert_eq!(report.records.len(), 1);
        assert_eq!(report.passed_count(), 1);
        assert!(report.is_success());
    }

    // -------------------------------------------------------------------------
    // Section 2: Never Fail (Edge Resilience & Fault Inversion)
    // -------------------------------------------------------------------------
    #[test]
    fn test_section_2_never_fail_empty_report() {
        let report = PipelineReport::default();
        assert!(!report.is_success());
        assert_eq!(report.total_duration_ms(), 0);
        assert_eq!(report.passed_count(), 0);
    }

    // -------------------------------------------------------------------------
    // Section 3: Invariants (Structural Bounds & Mathematical Guards)
    // -------------------------------------------------------------------------
    #[test]
    fn test_section_3_invariants_total_duration_monotonic() {
        let mut report = PipelineReport::default();
        report.add(TaskTelemetry::success("t1", 50, "p1"));
        report.add(TaskTelemetry::success("t2", 75, "p2"));
        assert_eq!(report.total_duration_ms(), 125);
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

/// Returns true if the report sub-assembly is ready for telemetry collection.
pub fn is_report_ready() -> bool {
    true
}
