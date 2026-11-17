// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/cmd/audit.rs
// @id            cog.tools.cmd.audit
// @layer         Layer 3: Developer Toolchain Command Sub-Assemblies
// @archetype     operational | stateless
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-15
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Developer Toolchain // Diagnostic Audit Sub-Assembly
// @role          Audits Toolchain Topology, Environment Prerequisites & Constants
// @maturity      Rev 5 Baseline | In-process diagnostic audit harness
// @chassis       bounded-heap | Developer tooling memory chassis; bounded stacks
// @scale         O(1) filesystem anchors; validates root and archive topology
// @seams         in: repo_root, archive_dir | uses: runner::* | probes: exit
// @invariants    1. Strict exit code semantics (0 = success, 1 = failure).
//                2. Universal 5-Block Lifecycle Protocol enforced.
//                3. Zero process spawning for in-process diagnostic sweeps.
//                4. Strict <= 80-column line width discipline across documentation.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Defines the diagnostic environment and toolchain self-audit command handler
//   for the OmniCode developer toolchain (`tl_tools`). Verifies foundational
//   workspace topology, build tool availability, and library invariants.
//
//   ### For Game Designers & Non-Programmers:
//   Acts as a quick health check for the project workspace. If you just pulled
//   fresh changes or moved the folder, running the audit command checks whether
//   all directories, archives, and build toolchains are in place before you run
//   game builds or content generators.
//
//   ### For Systems Programmers & Engine Architects:
//   - Ownership & Reentrancy: Operates purely statelessly on passed filesystem
//     references with zero persistent side effects.
//   - Extension Seam: Additional environment checks register as tasks inside
//     the `runner` pipeline without modifying this dispatcher contract.
//   - Explicit Non-Goal: Does not build binaries or mutate files; only verifies
//     presence, readability, and architectural invariants.
// =============================================================================

//! # Diagnostic Audit Command Handler
//!
//! Provides workspace topology, toolchain constants, and runtime prerequisite
//! verification for the OmniCode developer toolchain.

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Public Contract Limits, Command Types, Options & O(1) Pre-Conditions )
// =============================================================================

use std::path::{Path, PathBuf};
use crate::audit_library_invariants;
use crate::runner::{EnvironmentAuditTask, RunnerContext, TaskRunner};

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Operational Constants & Target Defaults
// -----------------------------------------------------------------------------

/// Canonical archive directory path for legacy game data.
pub const DEFAULT_ARCHIVE_DIR: &str =
    "/home/professorseanex/Tenebris_Lux_Old_Game_Data_Archive";

/// Canonical tools subdirectory relative to repository root.
pub const TOOLS_SUBDIR: &str = "tools";

/// Canonical docs subdirectory relative to repository root.
pub const DOCS_SUBDIR: &str = "docs";

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Types, Enums & Command Options
// -----------------------------------------------------------------------------

/// Execution parameters for the diagnostic audit command.
///
/// # Examples
///
/// ```rust
/// use std::path::PathBuf;
/// use tl_tools::cmd::audit::AuditArgs;
///
/// let args = AuditArgs::default();
/// assert_eq!(args.verbose, false);
/// assert_eq!(args.repo_root, PathBuf::from("."));
/// ```
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct AuditArgs {
    /// Workspace root directory to audit.
    pub repo_root: PathBuf,
    /// Path to the historical game data archive.
    pub archive_dir: PathBuf,
    /// Enable detailed progress and diagnostic reporting.
    pub verbose: bool,
}

impl Default for AuditArgs {
    fn default() -> Self {
        Self {
            repo_root: PathBuf::from("."),
            archive_dir: PathBuf::from(DEFAULT_ARCHIVE_DIR),
            verbose: false,
        }
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 2.3: Verification Helpers & Invariant Predicates
// -----------------------------------------------------------------------------

/// Evaluates whether a candidate path represents a valid workspace root.
///
/// Requires the presence of `tools/Cargo.toml` and `docs/`.
///
/// # Examples
///
/// ```rust
/// use std::path::Path;
/// use tl_tools::cmd::resolve_repo_root;
/// use tl_tools::cmd::audit::is_valid_audit_workspace;
///
/// let root = resolve_repo_root();
/// assert!(is_valid_audit_workspace(&root));
/// assert!(!is_valid_audit_workspace(Path::new("/nonexistent/workspace")));
/// ```
pub fn is_valid_audit_workspace(candidate: &Path) -> bool {
    candidate.is_dir()
        && candidate.join("tools/Cargo.toml").is_file()
        && candidate.join("docs").is_dir()
}

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Primary Execution Engine, Command Handlers & CLI Dispatchers )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: Primary Execution Engine
// -----------------------------------------------------------------------------

/// Executes the diagnostic environment and toolchain self-audit.
///
/// Verifies library constant invariants and workspace directory topology.
///
/// # Examples
///
/// ```rust
/// use tl_tools::cmd::resolve_repo_root;
/// use tl_tools::cmd::audit::{execute, AuditArgs};
///
/// let root = resolve_repo_root();
/// let args = AuditArgs {
///     repo_root: root,
///     ..Default::default()
/// };
/// assert!(execute(&args).is_ok());
/// ```
pub fn execute(args: &AuditArgs) -> Result<(), String> {
    if !is_valid_audit_workspace(&args.repo_root) {
        return Err(format!(
            "Invalid workspace root: {:?} (missing tools/Cargo.toml or docs/)",
            args.repo_root
        ));
    }

    println!("============================================================");
    println!("     Tenebris and Lux - Diagnostic Self-Audit               ");
    println!("============================================================");
    println!("Repository Root: {:?}", args.repo_root);
    println!("Archive Dir:     {:?}", args.archive_dir);

    // 1. Audit core library invariants
    print!("Auditing library constant invariants... ");
    audit_library_invariants()?;
    println!("[PASS]");

    // 2. Audit runtime environment prerequisites
    println!("Auditing workspace topology and build tools...");
    let mut ctx = RunnerContext::new(&args.repo_root, &args.archive_dir);
    ctx.verbose = args.verbose;

    let task = EnvironmentAuditTask;
    let tel = task.run(&ctx)?;
    println!("  [SUMMARY] {}", tel.details);

    println!("============================================================");
    println!("[SUCCESS] Toolchain topology and prerequisites are fully operational.");
    Ok(())
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.2: Standalone CLI Dispatcher
// -----------------------------------------------------------------------------

/// Standalone CLI dispatcher for environment audits.
///
/// Parses command-line arguments and dispatches to [`execute`].
///
/// # Examples
///
/// ```rust
/// use tl_tools::cmd::resolve_repo_root;
/// use tl_tools::cmd::audit::dispatch_standalone_audit;
///
/// let root = resolve_repo_root();
/// let args = vec!["--verbose".to_string()];
/// assert!(dispatch_standalone_audit(&args, &root).is_ok());
/// ```
pub fn dispatch_standalone_audit(cli_args: &[String], repo_root: &Path) -> Result<(), String> {
    let verbose = cli_args.iter().any(|a| a == "--verbose" || a == "-v");
    let mut archive_dir = PathBuf::from(DEFAULT_ARCHIVE_DIR);

    let mut i = 0;
    while i < cli_args.len() {
        if (cli_args[i] == "--archive-dir" || cli_args[i] == "-a") && i + 1 < cli_args.len() {
            archive_dir = PathBuf::from(&cli_args[i + 1]);
            i += 1;
        }
        i += 1;
    }

    let args = AuditArgs {
        repo_root: repo_root.to_path_buf(),
        archive_dir,
        verbose,
    };
    execute(&args)
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
    use crate::cmd::resolve_repo_root;

    // -------------------------------------------------------------------------
    // SECTION 1: ALWAYS PASS (Nominal Golden Surface Cogs)
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_1_nominal_defaults_and_construction() {
        let args = AuditArgs::default();
        assert_eq!(args.repo_root, PathBuf::from("."));
        assert_eq!(args.archive_dir, PathBuf::from(DEFAULT_ARCHIVE_DIR));
        assert_eq!(args.verbose, false);
    }

    #[test]
    fn test_section_1_nominal_workspace_validation_passes() {
        let root = resolve_repo_root();
        assert!(is_valid_audit_workspace(&root));
    }

    #[test]
    fn test_section_1_nominal_execution_passes() {
        let root = resolve_repo_root();
        let args = AuditArgs {
            repo_root: root,
            ..Default::default()
        };
        assert!(execute(&args).is_ok());
    }

    // -------------------------------------------------------------------------
    // SECTION 2: NEVER FAIL (Fault Inversion & Graceful Degradation)
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_2_never_fail_nonexistent_workspace() {
        let fake_path = Path::new("/definitely/nonexistent/workspace/dir");
        assert!(!is_valid_audit_workspace(fake_path));

        let args = AuditArgs {
            repo_root: fake_path.to_path_buf(),
            ..Default::default()
        };
        let res = execute(&args);
        assert!(res.is_err());
        let err_msg = res.unwrap_err();
        assert!(err_msg.contains("Invalid workspace root"));
    }

    #[test]
    fn test_section_2_never_fail_empty_args_dispatch() {
        let root = resolve_repo_root();
        let cli_args: Vec<String> = Vec::new();
        assert!(dispatch_standalone_audit(&cli_args, &root).is_ok());
    }

    // -------------------------------------------------------------------------
    // SECTION 3: INVARIANTS, SATURATION & LIMIT GUARDS
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_3_invariants_constants_non_empty() {
        assert!(!DEFAULT_ARCHIVE_DIR.is_empty());
        assert!(!TOOLS_SUBDIR.is_empty());
        assert!(!DOCS_SUBDIR.is_empty());
    }

    #[test]
    fn test_section_3_invariants_library_invariants_pass() {
        assert!(audit_library_invariants().is_ok());
    }

    // -------------------------------------------------------------------------
    // SECTION 4: EMERGENCE & COMPOSITE WORKFLOWS
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_4_emergence_verbose_cli_dispatch() {
        let root = resolve_repo_root();
        let cli_args = vec![
            "-v".to_string(),
            "--archive-dir".to_string(),
            DEFAULT_ARCHIVE_DIR.to_string(),
        ];
        assert!(dispatch_standalone_audit(&cli_args, &root).is_ok());
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 4.2: Diagnostic Self-Audit
// -----------------------------------------------------------------------------

/// Performs an in-process diagnostic preflight self-audit of audit module invariants.
///
/// Verifies operational constants and library integrity.
///
/// # Examples
///
/// ```rust
/// use tl_tools::cmd::audit::self_audit;
///
/// assert!(self_audit().is_ok());
/// ```
pub fn self_audit() -> Result<(), String> {
    if DEFAULT_ARCHIVE_DIR.is_empty() {
        return Err("DEFAULT_ARCHIVE_DIR must not be empty".to_string());
    }
    if TOOLS_SUBDIR.is_empty() {
        return Err("TOOLS_SUBDIR must not be empty".to_string());
    }
    if DOCS_SUBDIR.is_empty() {
        return Err("DOCS_SUBDIR must not be empty".to_string());
    }
    audit_library_invariants().map_err(|e| e.to_string())?;
    Ok(())
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( State Summary Handshake, Module Epilogue & Teardown Verification )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 5.1: Epilogue Predicate
// -----------------------------------------------------------------------------

/// Verifies that the audit sub-assembly is ready for command invocation.
///
/// # Examples
///
/// ```rust
/// use tl_tools::cmd::audit::is_audit_subsystem_ready;
///
/// assert!(is_audit_subsystem_ready());
/// ```
pub fn is_audit_subsystem_ready() -> bool {
    self_audit().is_ok()
}
