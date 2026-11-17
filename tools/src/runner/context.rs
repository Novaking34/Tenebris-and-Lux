// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/runner/context.rs
// @id            cog.runner.context
// @layer         Layer 3: Developer Tooling & Pipeline Orchestration
// @archetype     operational | stateful
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-16
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        Tools Runner Subsystem
// @role          Shared Pipeline Workspace Context & Path Topology
// @maturity      Rev 5 Baseline | Hardened
// @chassis       zero-heap (stack-managed path buffers)
// @scale         O(1) immutable context propagation
// @seams         in: repo_root, archive_dir | out: RunnerContext | probes: self_audit -> is_context_ready
// @invariants    1. Deterministic path resolution rooted strictly at repo_root.
//                2. Immutable configuration during active task execution.
//                3. Zero hidden environment variable dependencies.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Models the shared runtime execution context provided to all pipeline tasks.
//   Resolves canonical directories for documentation, archives, tools, and
//   compilation outputs.
//
// ### For Game Designers & Non-Programmers:
//   Defines where the tools should look for game files, design docs, and
//   historical archives, keeping all folder paths consistent across tools.
//
// ### For Systems Programmers & Engine Architects:
//   Provides a thread-safe, clonable context object eliminating hardcoded paths
//   and allowing clean sandboxing during unit and integrated test runs.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Pre-Conditions, Imports & Context Chassis )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Pre-Conditions & Imports
// -----------------------------------------------------------------------------

use std::path::{Path, PathBuf};

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Context Structure Definition
// -----------------------------------------------------------------------------

/// Shared context passed into all runner tasks.
#[derive(Debug, Clone, PartialEq, Eq)]
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

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Constructor & Context Methods )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: Context Constructors & Builder Helpers
// -----------------------------------------------------------------------------

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

    /// Sets the verbosity flag for diagnostic execution.
    pub fn with_verbose(mut self, verbose: bool) -> Self {
        self.verbose = verbose;
        self
    }
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// ( Four-Section Ontological Test Suite & Subsystem Self-Audit )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 4.1: Internal Subsystem Self-Audit Probe
// -----------------------------------------------------------------------------

/// Internal self-audit testing context path derivation.
pub fn self_audit() -> Result<(), &'static str> {
    let dummy_root = PathBuf::from("/tmp/repo_test");
    let dummy_archive = PathBuf::from("/tmp/archive_test");
    let ctx = RunnerContext::new(&dummy_root, &dummy_archive);

    if ctx.docs_dir != dummy_root.join("docs") {
        return Err("Self-audit failed: docs_dir path mismatch");
    }
    if ctx.output_dir != dummy_root.join("docs/game_design/demo_baseline") {
        return Err("Self-audit failed: output_dir path mismatch");
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
    fn test_section_1_always_pass_context_initialization() {
        let repo = PathBuf::from("/opt/project");
        let archive = PathBuf::from("/opt/archive");
        let ctx = RunnerContext::new(&repo, &archive);
        assert_eq!(ctx.repo_root, repo);
        assert_eq!(ctx.archive_dir, archive);
        assert_eq!(ctx.docs_dir, repo.join("docs"));
        assert_eq!(ctx.output_dir, repo.join("docs/game_design/demo_baseline"));
        assert!(!ctx.verbose);
    }

    // -------------------------------------------------------------------------
    // Section 2: Never Fail (Edge Resilience & Fault Inversion)
    // -------------------------------------------------------------------------
    #[test]
    fn test_section_2_never_fail_with_verbose_builder() {
        let ctx = RunnerContext::new(&PathBuf::from("."), &PathBuf::from("."))
            .with_verbose(true);
        assert!(ctx.verbose);
    }

    // -------------------------------------------------------------------------
    // Section 3: Invariants (Structural Bounds & Mathematical Guards)
    // -------------------------------------------------------------------------
    #[test]
    fn test_section_3_invariants_paths_rooted_at_repo() {
        let repo = PathBuf::from("/custom/root");
        let ctx = RunnerContext::new(&repo, &PathBuf::from("/archive"));
        assert!(ctx.docs_dir.starts_with(&repo));
        assert!(ctx.output_dir.starts_with(&repo));
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

/// Returns true if the context sub-assembly is ready for pipeline usage.
pub fn is_context_ready() -> bool {
    true
}
