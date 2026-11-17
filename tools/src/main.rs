// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/main.rs
// @id            cog.tools.main
// @layer         Layer 3: Systems Developer Tooling Core Library
// @archetype     operational | stateful
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-16
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Developer Toolchain // Unified CLI Orchestrator
// @role          Central Entrypoint & Toolchain Dispatcher
// @maturity      Rev 5 Baseline | Unified multi-domain CLI dispatcher
// @chassis       bounded-heap | Memory allocated for command arguments & reports
// @scale         O(1) argument dispatch to specialized sub-assemblies
// @seams         in: std::env::args | uses: tl_tools::cmd | probes: self_audit -> exit
// @invariants    1. Strict exit code semantics (0 = success, 1 = failure).
//                2. Fail-fast preflight validation before any mutation.
//                3. Universal 5-Block Lifecycle Protocol enforced.
//                4. Strict line discipline <= 80 columns across all comments.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Central binary entry point and command router for `tl_tools`.
//
//   ### For Game Designers & Non-Programmers:
//   Provides the primary command-line tool `tl_tools` used to audit assets,
//   compile the Master Guide book, extract legacy data, and validate code.
//
//   ### For Systems Programmers & Engine Architects:
//   - Zero-overhead dispatch to `tl_tools::cmd::run_toolchain`.
//   - Safe argument boundary mapping and POSIX-compliant exit code returns.
//   - Rigorous first-principles parameter documentation and error contracts.
// =============================================================================

//! # Developer Toolchain CLI Entry Point & Pipeline Orchestrator
//!
//! Parses top-level arguments and dispatches tasks to modular command
//! sub-assemblies defined in [`tl_tools::cmd`].

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Pre-Conditions, Imports & Router Configuration )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Pre-Conditions & Library Imports
// -----------------------------------------------------------------------------

use std::env;
use std::process::ExitCode;
use tl_tools::cmd::run_toolchain;

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: CLI Configuration & Signals
// -----------------------------------------------------------------------------

/// Canonical tool binary name used for process identification and usage help.
///
/// # Dual-Audience Documentation
///
/// ### For Game Designers & Non-Programmers:
/// This string specifies the command name typed in the shell (e.g. `tl_tools
/// validate`). If you see documentation referencing `tl_tools`, it corresponds
/// directly to this canonical executable.
///
/// ### For Systems Programmers & Engine Architects:
/// Used by the argument tokenizer to identify the calling process invocation,
/// format diagnostic error messages, and generate deterministic `--help`
/// banners across POSIX-compliant execution environments.
///
/// # Invariants
/// - Must remain non-empty and match the package binary name in `Cargo.toml`.
/// - Must be ASCII alphanumeric with underscores only (no whitespace).
pub const BINARY_NAME: &str = "tl_tools";

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( CLI Argument Extraction & Router Execution )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: Command-Line Argument Extraction & Router Execution
// -----------------------------------------------------------------------------

/// Dispatches CLI execution with an explicit sequence of command-line arguments.
///
/// # Arguments
/// * `args` - A slice of `String` arguments representing process invocation
///   tokens, where `args[0]` is conventionally the binary path and `args[1..]`
///   are subcommands and flags.
///
/// # Returns
/// * `Ok(())` - Command executed successfully to completion.
/// * `Err(String)` - Human-readable diagnostic description of why execution
///   failed (e.g., unrecognized subcommand, missing file path, validation error).
///
/// # Errors
/// Propagates errors from [`run_toolchain`], including syntax errors, failed
/// validation checks, I/O errors during extraction, or invalid flags.
///
/// # Invariants
/// - Does not mutate global process state or call `std::process::exit`.
/// - Zero panics; all failures are returned as typed `Err` strings.
///
/// # Dual-Audience Documentation
///
/// ### For Game Designers & Non-Programmers:
/// This function is the nerve center of the tool. When you type `tl_tools
/// validate`, this function parses that word and routes it to the validator,
/// reporting clear errors if a file is missing or formatted incorrectly.
///
/// ### For Systems Programmers & Engine Architects:
/// Decouples argument ingestion from the physical process environment
/// (`std::env::args`), permitting hermetic unit testing of the entire command
/// dispatch tree without launching child processes.
///
/// # Examples
/// ```rust
/// use tl_tools::cmd::run_toolchain;
///
/// let args = vec!["tl_tools".to_string(), "--help".to_string()];
/// assert!(run_toolchain(&args).is_ok());
/// ```
pub fn run_with_args(args: &[String]) -> Result<(), String> {
    run_toolchain(args)
}

/// Gathers physical process arguments from the OS and dispatches the toolchain.
///
/// # Returns
/// * `Ok(())` - Dispatched command completed successfully.
/// * `Err(String)` - Dispatched command encountered a fatal error.
///
/// # Errors
/// Returns an error if any subcommand dispatched by [`run_toolchain`] fails.
///
/// # Dual-Audience Documentation
///
/// ### For Game Designers & Non-Programmers:
/// Gathers whatever options you typed on the command line and sends them to
/// the toolchain router.
///
/// ### For Systems Programmers & Engine Architects:
/// Thin bridge between `std::env::args` and [`run_with_args`]. Allocates a
/// temporary argument vector on the heap and delegates execution.
fn run() -> Result<(), String> {
    let args: Vec<String> = env::args().collect();
    run_with_args(&args)
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// ( Preflight Diagnostics & Four-Section Ontological Test Suite )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 4.1: Preflight Diagnostic Gates & Self-Audit Probe
// -----------------------------------------------------------------------------

/// Diagnostic self-audit verifying CLI binary constants and environment bounds.
///
/// # Returns
/// * `Ok(())` - All compile-time and runtime binary constants satisfy invariants.
/// * `Err(&'static str)` - Invariant breach detected in binary metadata.
///
/// # Errors
/// Returns an error message if `BINARY_NAME` is empty or exceeds bounded length.
///
/// # Invariants
/// - Must execute deterministically without disk or network I/O.
/// - Big-O time complexity is O(1).
///
/// # Dual-Audience Documentation
///
/// ### For Game Designers & Non-Programmers:
/// A fast diagnostic safety check proving that the tool binary is built
/// properly and ready to run.
///
/// ### For Systems Programmers & Engine Architects:
/// Sub-Assembly 4.1 preflight gate verifying that binary metadata strings have
/// not suffered corruption or link-time truncation before dispatch begins.
pub fn self_audit() -> Result<(), &'static str> {
    if BINARY_NAME.is_empty() {
        return Err("BINARY_NAME must be non-empty");
    }
    if BINARY_NAME.len() > 32 {
        return Err("BINARY_NAME exceeds maximum bounded length (32 bytes)");
    }
    Ok(())
}

// -----------------------------------------------------------------------------
// Sub-Assembly 4.2: Four-Section Ontological Test Suite
// -----------------------------------------------------------------------------

#[cfg(test)]
mod tests {
    use super::*;

    // -------------------------------------------------------------------------
    // SECTION 1: ALWAYS PASS (Nominal Golden Surface Cogs)
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_1_always_pass_binary_name() {
        assert_eq!(BINARY_NAME, "tl_tools");
    }

    #[test]
    fn test_section_1_always_pass_run_with_args_help() {
        let args = vec!["tl_tools".to_string(), "--help".to_string()];
        assert!(run_with_args(&args).is_ok());
    }

    // -------------------------------------------------------------------------
    // SECTION 2: NEVER FAIL (Fault Inversion & Graceful Degradation)
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_2_never_fail_self_audit() {
        assert!(self_audit().is_ok());
    }

    #[test]
    fn test_section_2_never_fail_empty_args_shows_help() {
        let args = vec!["tl_tools".to_string()];
        assert!(run_with_args(&args).is_ok());
    }

    // -------------------------------------------------------------------------
    // SECTION 3: INVARIANTS, SATURATION & LIMIT GUARDS
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_3_invariants_name_length() {
        assert!(!BINARY_NAME.is_empty());
        assert!(BINARY_NAME.len() <= 32);
    }

    #[test]
    fn test_section_3_invariants_name_alphanumeric() {
        assert!(BINARY_NAME.chars().all(|c| c.is_ascii_alphanumeric() || c == '_'));
    }

    // -------------------------------------------------------------------------
    // SECTION 4: EMERGENCE & COMPOSITE WORKFLOWS
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_4_emergence_ready() {
        assert!(is_main_ready());
    }
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( Epilogue Handshake & Process Termination )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 5.1: Epilogue Readiness Probe & Lifecycle Handshake
// -----------------------------------------------------------------------------

/// Epilogue readiness probe for the main CLI dispatcher.
///
/// # Returns
/// * `true` - The binary self-audit passes and the dispatcher is ready.
/// * `false` - Preflight checks failed; dispatcher cannot safely execute.
///
/// # Dual-Audience Documentation
///
/// ### For Game Designers & Non-Programmers:
/// Returns a simple yes/no signal confirming whether the command-line tool is
/// fully functional.
///
/// ### For Systems Programmers & Engine Architects:
/// Sub-Assembly 5.1 lifecycle handshake providing an un-panicked boolean probe
/// suitable for integration into automated smoke-test suites and orchestrators.
pub fn is_main_ready() -> bool {
    self_audit().is_ok()
}

// -----------------------------------------------------------------------------
// Sub-Assembly 5.2: Process Termination & Exit Code Mapping
// -----------------------------------------------------------------------------

/// Process entry point returning standard POSIX exit status codes.
///
/// # Returns
/// * `ExitCode::SUCCESS` (0) on successful toolchain execution.
/// * `ExitCode::FAILURE` (1) when any error is encountered during dispatch.
///
/// # Dual-Audience Documentation
///
/// ### For Game Designers & Non-Programmers:
/// When running `tl_tools` in batch scripts or CI, this function tells the
/// operating system whether the command succeeded or failed.
///
/// ### For Systems Programmers & Engine Architects:
/// Root process boundary enforcing invariant 1: strict POSIX exit code
/// semantics (0 = success, 1 = failure), printing error messages to `stderr`.
fn main() -> ExitCode {
    match run() {
        Ok(()) => ExitCode::SUCCESS,
        Err(err) => {
            eprintln!("Error: {}", err);
            ExitCode::FAILURE
        }
    }
}
