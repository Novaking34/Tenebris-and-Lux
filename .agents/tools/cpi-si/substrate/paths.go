// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundaries )
// =============================================================================
// @project       OmniCode Framework / Big House Ecosystem
// @file          substrate/paths.go
// @id            cog.agent.tools.cpi_si_cli.paths
// @layer         Layer 1: Portable Substrate Root Resolution & FS Predicates
// @archetype     portable_discovery | substrate_root_resolver
// @version       0.1.3-dev
// @interface_rev Rev 5
// @created       2026-11-20
// @updated       2026-11-20
// @author        CreativeWorkzStudio LLC / Seanje Lenox-Wise & Silas Oren
// @repository    https://github.com/Novaking34/OmniCode_Framework
// @system        CPI-SI Portable Cognitive BIOS CLI & Substrate Tooling
// @role          Resolve Active .agents/ Substrate Across Any of 6 Mirror Roots
// @maturity      Rev 5 Operational Baseline | Wave A Tools Alignment
// @chassis       zero-external-deps | Pure Go stdlib os/path/filepath/strings
// @scale         O(depth) bounded parent directory walk
// @seams         in: substrate/types | exports: ResolveAgentsDir, FileExists,
//                DirExists, ExpandHome
// @invariants    1. Universal 5-Block Lifecycle Protocol (Rev 5).
//                2. Distinguish .agents/README.md from repo root README.md.
//                3. Read-only inspection only; never mutates the filesystem.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Discovers which `.agents/` Portable Cognitive BIOS mount is active from
//   explicit CLI flags, `CPI_SI_AGENTS_DIR`, or current working directory
//   hierarchy across any of the 6 Big House mirror mounts.
//
//   ### For Architects, Operators & Non-Programmers:
//   Lets you run `cpi-si` from any subfolder in OmniCode, FaithNet,
//   Tenebris-and-Lux, or `~/.gemini/` without manually typing the full
//   `.agents`
//   path every time.
//
//   ### For Systems Programmers & Engine Architects:
//   Implements strict BIOS signature checks (`README.md` + `host/`|`skills/`|
//   `identity/`) so an ordinary repository root `README.md` never causes a
//   false positive match.
// =============================================================================

package substrate

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Module Anchor, Precedence Constants & Environment Key Registry )
// =============================================================================
// Engine:        CPI-SI Portable Cognitive BIOS Tooling (cpi-si-cli)
// Stratum:       Layer 1: Portable Root Resolution
// Module:        cog.agent.tools.cpi_si_cli.paths
// Mission:       Locate the active .agents/ substrate cleanly on any OS.
// =============================================================================

// --- Tier 1: Standard Go Library Packages ---
import (
	"fmt"
	"os"
	"path/filepath"
	"strings"
)

// --- Tier 2: External Library Dependencies (None - 100% Go Stdlib) ---

// --- Tier 3: Internal Substrate Foundation Packages (Same-package types) ---

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Environment Keys & Canonical Sentinels
// -----------------------------------------------------------------------------

const (
	// EnvAgentsDir overrides automatic parent-walk .agents/ discovery.
	EnvAgentsDir = "CPI_SI_AGENTS_DIR"

	// ModuleIDPaths is the logical identifier for substrate/paths.
	ModuleIDPaths = "cog.agent.tools.cpi_si_cli.paths"
)

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Pure O(1) Filesystem Predicates
// -----------------------------------------------------------------------------

// FileExists reports whether path exists on disk and is a regular file.
//
// ### For Architects, Operators & Non-Programmers:
// Safe file presence test returning false for missing paths or folders.
//
// ### For Systems Programmers & Engine Architects:
// Wraps `os.Stat` with `!st.IsDir()` guard cleanly.
func FileExists(path string) bool {
	st, err := os.Stat(path)
	return err == nil && !st.IsDir()
}

// DirExists reports whether path exists on disk and is a directory.
//
// ### For Architects, Operators & Non-Programmers:
// Safe directory presence test returning false for regular files.
//
// ### For Systems Programmers & Engine Architects:
// Wraps `os.Stat` with `st.IsDir()` guard cleanly.
func DirExists(path string) bool {
	st, err := os.Stat(path)
	return err == nil && st.IsDir()
}

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Substrate Root Discovery & Cross-Platform Path Expansion )
// =============================================================================
// Engine:        CPI-SI Portable Cognitive BIOS Tooling (cpi-si-cli)
// Stratum:       Layer 1: Portable Root Resolution
// Module:        cog.agent.tools.cpi_si_cli.paths
// Mission:       Resolve active .agents/ root and home-relative paths safely.
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: .agents/ Root Resolution Precedence Chain
// -----------------------------------------------------------------------------

// ResolveAgentsDir finds the portable `.agents/` root directory cleanly.
//
// Precedence order:
//  1. Explicit CLI flag (`explicit` or `<explicit>/.agents`).
//  2. `CPI_SI_AGENTS_DIR` environment variable.
//  3. Upward walk from `os.Getwd()` inspecting `<dir>/.agents` or `<dir>`.
//
// ### For Architects, Operators & Non-Programmers:
// Automatically locates your `.agents/` Cognitive BIOS whether you call the CLI
// from the repo root, a deep source folder, or inside `~/.gemini/`.
//
// ### For Systems Programmers & Engine Architects:
// Guards against confusing repository-root `README.md` with `.agents/README.md`
// via `IsAgentsRoot` structural checks.
func ResolveAgentsDir(explicit string) (string, error) {
	if trimmed := strings.TrimSpace(explicit); trimmed != "" {
		abs, err := filepath.Abs(trimmed)
		if err != nil {
			return "", fmt.Errorf("resolve explicit agents dir: %w", err)
		}
		if IsAgentsRoot(abs) {
			return abs, nil
		}
		nested := filepath.Join(abs, ".agents")
		if IsAgentsRoot(nested) {
			return nested, nil
		}
		return "", fmt.Errorf("%q does not look like a .agents substrate root", abs)
	}

	if envDir := strings.TrimSpace(os.Getenv(EnvAgentsDir)); envDir != "" {
		abs, err := filepath.Abs(envDir)
		if err == nil && IsAgentsRoot(abs) {
			return abs, nil
		}
	}

	cwd, err := os.Getwd()
	if err != nil {
		return "", fmt.Errorf("determine working directory: %w", err)
	}

	cur := cwd
	for {
		if filepath.Base(cur) == ".agents" && IsAgentsRoot(cur) {
			return cur, nil
		}
		candidate := filepath.Join(cur, ".agents")
		if IsAgentsRoot(candidate) {
			return candidate, nil
		}
		parent := filepath.Dir(cur)
		if parent == cur {
			break
		}
		cur = parent
	}

	return "", fmt.Errorf("could not locate .agents substrate starting from %s", cwd)
}

// IsAgentsRoot reports whether dir satisfies the structural `.agents/` marker.
//
// ### For Architects, Operators & Non-Programmers:
// Confirms a folder really holds an OmniCode `.agents/` substrate rather than
// just any folder containing a `README.md`.
//
// ### For Systems Programmers & Engine Architects:
// Requires `README.md` plus at least one signature directory (`host`,
// `skills`, or `identity`).
func IsAgentsRoot(dir string) bool {
	if !FileExists(filepath.Join(dir, "README.md")) {
		return false
	}
	return DirExists(filepath.Join(dir, "host")) ||
		DirExists(filepath.Join(dir, "skills")) ||
		DirExists(filepath.Join(dir, "identity"))
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.2: Cross-Platform Home Prefix Expansion
// -----------------------------------------------------------------------------

// ExpandHome resolves a leading `~` segment to the user's home directory.
//
// ### For Architects, Operators & Non-Programmers:
// Allows defaults like `~/.gemini/antigravity/brain` to work reliably across
// Linux, macOS, and Windows PowerShell runs without shell expansion quirks.
//
// ### For Systems Programmers & Engine Architects:
// Handles exact `~`, `~/...`, and `~\...` prefixes via `os.UserHomeDir()`.
func ExpandHome(path string) string {
	if path == "" || path[0] != '~' {
		return path
	}
	home, err := os.UserHomeDir()
	if err != nil || home == "" {
		return path
	}
	if path == "~" {
		return home
	}
	if strings.HasPrefix(path, "~/") || strings.HasPrefix(path, `~\`) {
		return filepath.Join(home, path[2:])
	}
	return path
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// ( Contract Verification Cogs & Pre-Flight Validation Tripwires )
// =============================================================================
// Engine:        CPI-SI Portable Cognitive BIOS Tooling (cpi-si-cli)
// Stratum:       Layer 1: Portable Root Resolution
// Module:        cog.agent.tools.cpi_si_cli.paths
// Mission:       4-section self-audit proving BIOS vs repo-root discrimination.
// =============================================================================

// PreflightPaths executes a 4-section pre-flight self-audit for
// substrate/paths.
//
// @triggers tests/unit/substrate/paths_test.go
//
// ### For Architects, Operators & Non-Programmers:
// Verifies inside a clean temporary sandbox that `.agents/` discovery finds
// real
// BIOS mounts, rejects plain non-BIOS folders, and expands home paths cleanly.
//
// ### For Systems Programmers & Engine Architects:
// Self-contained isolated tempdir probe (auto-cleaned via `defer os.RemoveAll`)
// touching zero repository paths.
func PreflightPaths() CheckResult {
	const check = "paths_preflight_4section"

	tmp, err := os.MkdirTemp("", "cpisi-paths-preflight-*")
	if err != nil {
		return CheckFail(SeverityFatal, ModuleIDPaths, check, fmt.Sprintf("create temp sandbox: %v", err))
	}
	defer func() { _ = os.RemoveAll(tmp) }()

	// Micro Check 1 (Always Pass): Nominal nested .agents/ resolves accurately.
	agents := filepath.Join(tmp, ".agents")
	if err := os.MkdirAll(filepath.Join(agents, "skills"), 0o755); err != nil {
		return CheckFail(SeverityFatal, ModuleIDPaths, check, fmt.Sprintf("mkdir skills: %v", err))
	}
	if err := os.WriteFile(filepath.Join(agents, "README.md"), []byte("# BIOS\n"), 0o644); err != nil {
		return CheckFail(SeverityFatal, ModuleIDPaths, check, fmt.Sprintf("write README: %v", err))
	}
	if err := os.WriteFile(filepath.Join(tmp, "README.md"), []byte("# Repo\n"), 0o644); err != nil {
		return CheckFail(SeverityFatal, ModuleIDPaths, check, fmt.Sprintf("write repo README: %v", err))
	}

	got, err := ResolveAgentsDir(tmp)
	if err != nil || got != agents {
		return CheckFail(SeverityCritical, ModuleIDPaths, check, fmt.Sprintf("Sec 1 resolve failed: got=%q err=%v", got, err))
	}

	// Micro Check 2 (Never Fail): Plain directory with README.md alone rejected.
	emptyRepo := filepath.Join(tmp, "plain-repo")
	if err := os.MkdirAll(emptyRepo, 0o755); err == nil {
		_ = os.WriteFile(filepath.Join(emptyRepo, "README.md"), []byte("# Plain\n"), 0o644)
		if _, err := ResolveAgentsDir(emptyRepo); err == nil {
			return CheckFail(SeverityCritical, ModuleIDPaths, check, "Sec 2 non-BIOS README bypassed IsAgentsRoot")
		}
	}

	// Micro Check 3 (Invariants & Limits): FileExists vs DirExists stay disjoint.
	if FileExists(agents) || !DirExists(agents) || !FileExists(filepath.Join(agents, "README.md")) {
		return CheckFail(SeverityCritical, ModuleIDPaths, check, "Sec 3 FileExists/DirExists predicate mismatch")
	}

	// Micro Check 4 (Emergence): ExpandHome expands ~/ cleanly and leaves /abs
	// intact.
	expanded := ExpandHome("~/probe")
	if expanded == "~/probe" && os.Getenv("HOME") != "" {
		return CheckFail(SeverityCritical, ModuleIDPaths, check, "Sec 4 ExpandHome failed to resolve ~ prefix")
	}
	if ExpandHome("/abs/only") != "/abs/only" {
		return CheckFail(SeverityCritical, ModuleIDPaths, check, "Sec 4 ExpandHome altered absolute path")
	}

	return CheckPass(ModuleIDPaths, check, "Layer 1 .agents/ discovery and path guards passed all 4 micro gates.")
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( Epilogue Readiness Handshake & Seal )
// =============================================================================
// Engine:        CPI-SI Portable Cognitive BIOS Tooling (cpi-si-cli)
// Stratum:       Layer 1: Portable Root Resolution
// Module:        cog.agent.tools.cpi_si_cli.paths
// Mission:       Expose readiness check confirming path discovery works.
// =============================================================================

// IsPathsReady reports whether substrate/paths passes its 4-gate preflight.
//
// ### For Architects, Operators & Non-Programmers:
// Confirms `.agents/` lookup is ready before running higher CLI commands.
//
// ### For Systems Programmers & Engine Architects:
// Calls `PreflightPaths().IsPass()`.
func IsPathsReady() bool {
	return PreflightPaths().IsPass()
}

// End of substrate/paths.go
