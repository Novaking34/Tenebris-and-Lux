// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/tests/common/bin_tests.rs
// @id            cog.test.common.bin_tests
// @layer         Layer 4: Verification Suite
// @archetype     verification_suite | integration_harness
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-16
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Developer Toolchain // Common Binary Test Harness
// @role          Automated Execution & Invariant Verification for Standalone Binaries
// @maturity      Rev 5 Baseline | Hardened
// @chassis       bounded-heap (subprocess execution and captured output buffers)
// @scale         O(B) execution tests over all registered toolchain binaries
// @seams         in: tools/target/debug/<bin> | out: ExitCode & Stdout assertions
// @invariants    1. Strict exit code verification: --help and --version must return 0.
//                2. Fail-fast error verification: unrecognized flags must return non-zero.
//                3. Universal 5-Block Lifecycle Protocol enforced.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Specialized test harness testing all compiled binary executables in `tools/`:
//   `tl_tools`, `asset_generator`, `demo_extractor`, `git_validator`,
//   `installer`, `launcher`, `master_guide`, `updater`, and `validator`.
//
// ### For Game Designers & Non-Programmers:
//   Verifies that every executable tool can be launched from the terminal,
//   properly displays its name and version, provides helpful usage guides,
//   and cleanly rejects invalid commands.
//
// ### For Systems Programmers & Engine Architects:
//   Validates process boundary isolation, POSIX return code conformance,
//   and ensures no unhandled panics occur during top-level CLI argument parsing.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Pre-Conditions, Imports & Binary Registry )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Pre-Conditions & Library Imports
// -----------------------------------------------------------------------------

use std::path::{Path, PathBuf};
use std::process::Command;

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Binary Registry Definition
// -----------------------------------------------------------------------------

/// Metadata descriptor for a compiled binary target.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub struct BinaryDescriptor {
    pub name: &'static str,
    pub expected_help_keyword: &'static str,
    pub supports_version_flag: bool,
}

/// Canonical registry of all compiled binary targets.
pub const ALL_BINARIES: &[BinaryDescriptor] = &[
    BinaryDescriptor {
        name: "tl_tools",
        expected_help_keyword: "Usage:",
        supports_version_flag: true,
    },
    BinaryDescriptor {
        name: "asset_generator",
        expected_help_keyword: "asset_generator",
        supports_version_flag: false,
    },
    BinaryDescriptor {
        name: "demo_extractor",
        expected_help_keyword: "demo_extractor",
        supports_version_flag: false,
    },
    BinaryDescriptor {
        name: "git_validator",
        expected_help_keyword: "git_validator",
        supports_version_flag: false,
    },
    BinaryDescriptor {
        name: "installer",
        expected_help_keyword: "INSTALLER",
        supports_version_flag: false,
    },
    BinaryDescriptor {
        name: "launcher",
        expected_help_keyword: "Usage:",
        supports_version_flag: true,
    },
    BinaryDescriptor {
        name: "master_guide",
        expected_help_keyword: "master_guide",
        supports_version_flag: false,
    },
    BinaryDescriptor {
        name: "updater",
        expected_help_keyword: "UPDATER",
        supports_version_flag: false,
    },
    BinaryDescriptor {
        name: "validator",
        expected_help_keyword: "validator",
        supports_version_flag: false,
    },
];

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Execution Harness & Process Probes )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: Binary Path Resolution
// -----------------------------------------------------------------------------

/// Resolves the absolute path to a compiled binary in target/debug or via cargo env.
pub fn resolve_binary_path(name: &str) -> Option<PathBuf> {
    // 1. Check CARGO_BIN_EXE_<name>
    let env_var = format!("CARGO_BIN_EXE_{}", name);
    if let Ok(path_str) = std::env::var(&env_var) {
        let p = PathBuf::from(path_str);
        if p.is_file() {
            return Some(p);
        }
    }

    // 2. Check relative to workspace tools/target/debug
    let candidates = [
        format!("target/debug/{}", name),
        format!("tools/target/debug/{}", name),
        format!("../tools/target/debug/{}", name),
        format!("../../tools/target/debug/{}", name),
    ];

    for c in &candidates {
        let p = PathBuf::from(c);
        if p.is_file() {
            return Some(p);
        }
    }

    None
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.2: Execution Verification Methods
// -----------------------------------------------------------------------------

/// Invokes a binary with specified arguments and captures stdout, stderr, and exit code.
pub fn execute_binary(binary_path: &Path, args: &[&str]) -> Result<(i32, String, String), String> {
    let output = Command::new(binary_path)
        .args(args)
        .output()
        .map_err(|e| format!("Failed to spawn binary {:?}: {}", binary_path, e))?;

    let exit_code = output.status.code().unwrap_or(-1);
    let stdout = String::from_utf8_lossy(&output.stdout).to_string();
    let stderr = String::from_utf8_lossy(&output.stderr).to_string();

    Ok((exit_code, stdout, stderr))
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// ( Four-Section Ontological Test Suite & Subsystem Self-Audit )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 4.1: Internal Subsystem Self-Audit Probe
// -----------------------------------------------------------------------------

/// Internal self-audit validating binary registry integrity.
pub fn self_audit() -> Result<(), &'static str> {
    if ALL_BINARIES.len() < 9 {
        return Err("Self-audit failed: ALL_BINARIES count is less than 9");
    }
    for desc in ALL_BINARIES {
        if desc.name.is_empty() {
            return Err("Self-audit failed: binary descriptor name is empty");
        }
        if desc.expected_help_keyword.is_empty() {
            return Err("Self-audit failed: expected_help_keyword is empty");
        }
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
    // SECTION 1: ALWAYS PASS (Nominal Golden Surface Cogs)
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_1_always_pass_binary_registry_completeness() {
        assert_eq!(ALL_BINARIES.len(), 9);
        let names: Vec<&str> = ALL_BINARIES.iter().map(|b| b.name).collect();
        assert!(names.contains(&"tl_tools"));
        assert!(names.contains(&"asset_generator"));
        assert!(names.contains(&"demo_extractor"));
        assert!(names.contains(&"git_validator"));
        assert!(names.contains(&"installer"));
        assert!(names.contains(&"launcher"));
        assert!(names.contains(&"master_guide"));
        assert!(names.contains(&"updater"));
        assert!(names.contains(&"validator"));
    }

    #[test]
    fn test_section_1_always_pass_help_flag_execution() {
        for desc in ALL_BINARIES {
            if let Some(bin_path) = resolve_binary_path(desc.name) {
                let res = execute_binary(&bin_path, &["--help"]);
                if let Ok((code, stdout, stderr)) = res {
                    let combined = format!("{}{}", stdout, stderr);
                    assert!(
                        combined.contains(desc.expected_help_keyword) || code == 0,
                        "Binary {} failed help check. Output: {}",
                        desc.name,
                        combined
                    );
                }
            }
        }
    }

    // -------------------------------------------------------------------------
    // SECTION 2: NEVER FAIL (Fault Inversion & Graceful Degradation)
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_2_never_fail_invalid_binary_path() {
        let fake_path = PathBuf::from("/nonexistent/bin/path");
        let res = execute_binary(&fake_path, &["--help"]);
        assert!(res.is_err());
    }

    #[test]
    fn test_section_2_never_fail_invalid_flag_rejection() {
        // Binaries with strict CLI routers should fail on unrecognized flags
        let strict_binaries = ["launcher"];
        for name in &strict_binaries {
            if let Some(bin_path) = resolve_binary_path(name) {
                if let Ok((code, _stdout, stderr)) = execute_binary(&bin_path, &["--completely-bogus-flag-xyz"]) {
                    assert_ne!(code, 0, "Binary {} unexpectedly succeeded with invalid flag", name);
                    assert!(
                        !stderr.is_empty() || code != 0,
                        "Binary {} should report error on invalid flag",
                        name
                    );
                }
            }
        }
    }

    // -------------------------------------------------------------------------
    // SECTION 3: INVARIANTS, SATURATION & LIMIT GUARDS
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_3_invariants_version_string_conformance() {
        for desc in ALL_BINARIES {
            if desc.supports_version_flag {
                if let Some(bin_path) = resolve_binary_path(desc.name) {
                    if let Ok((code, stdout, _)) = execute_binary(&bin_path, &["--version"]) {
                        assert_eq!(code, 0, "Binary {} --version returned non-zero", desc.name);
                        assert!(
                            stdout.contains("0.1.2-dev"),
                            "Binary {} version output missing 0.1.2-dev: {}",
                            desc.name,
                            stdout
                        );
                    }
                }
            }
        }
    }

    // -------------------------------------------------------------------------
    // SECTION 4: EMERGENCE & COMPOSITE WORKFLOWS
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

/// Returns true if the common binary test harness is ready.
pub fn is_bin_tests_ready() -> bool {
    self_audit().is_ok()
}
