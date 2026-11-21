// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundaries )
// =============================================================================
// @project       OmniCode Framework / Big House Ecosystem
// @file          substrate/cli.go
// @id            cog.agent.tools.cpi_si_cli.cli
// @layer         Layer 3: Subcommand Router & Self-Preflight Orchestrator
// @archetype     cli_dispatcher | deterministic_exit_router
// @version       0.1.3-dev
// @interface_rev Rev 5
// @created       2026-11-20
// @updated       2026-11-20
// @author        CreativeWorkzStudio LLC / Seanje Lenox-Wise & Silas Oren
// @repository    https://github.com/Novaking34/OmniCode_Framework
// @system        CPI-SI Portable Cognitive BIOS CLI & Substrate Tooling
// @role          Dispatch boot|tail|bench|skills|audit|preflight|version|help
// @maturity      Rev 5 Operational Baseline | Wave A Tools Alignment
// @chassis       zero-external-deps | Pure stdlib flag.ContinueOnError router
// @scale         O(1) subcommand dispatch + 8-gate preflight orchestration
// @seams         in: all substrate cogs | used_by: cmd/cpi-si/main.go
// @invariants    1. Universal 5-Block Lifecycle Protocol (Rev 5).
//                2. Deterministic exit codes: 0 ok, 1 runtime/audit fail, 2
//                CLI.
//                3. Never calls os.Exit inside testable RunCLI core.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Implements the testable subcommand dispatcher (`RunCLI`) powering `cmd/cpi-
//   si`.
//   Separates routing logic cleanly from `os.Exit` so both unit suites and the
//   compiled CLI binary test identical flag parsing, stdout/stderr, and exits.
//
//   ### For Architects, Operators & Non-Programmers:
//   Provides the single unified command interface (`boot`, `tail`, `bench`,
//   `skills`, `audit`, `preflight`, `version`, `help`) that works identically
//   from Linux, macOS, and Windows runners.
//
//   ### For Systems Programmers & Engine Architects:
//   Uses isolated `flag.FlagSet` instances with `flag.ContinueOnError` and
//   explicit `io.Writer` injection to keep concurrent `-race` testing pure.
// =============================================================================

package substrate

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Module Anchor, Exit Constants & Gate Slice Contract )
// =============================================================================
// Engine:        CPI-SI Portable Cognitive BIOS Tooling (cpi-si-cli)
// Stratum:       Layer 3: Orchestrated CLI Dispatcher
// Module:        cog.agent.tools.cpi_si_cli.cli
// Mission:       Route CPI-SI CLI verbs cleanly with explicit exit codes.
// =============================================================================

// --- Tier 1: Standard Go Library Packages ---
import (
	"bytes"
	"flag"
	"fmt"
	"io"
	"strings"
)

// --- Tier 2: External Library Dependencies (None - 100% Go Stdlib) ---

// --- Tier 3: Internal Substrate Foundation Packages (Same-package cogs) ---

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Logical Anchor & Deterministic Exit Codes
// -----------------------------------------------------------------------------

const (
	// ModuleIDCLI is the logical identifier for substrate/cli.
	ModuleIDCLI = "cog.agent.tools.cpi_si_cli.cli"

	// ExitOK marks successful zero-violation execution.
	ExitOK = 0

	// ExitRuntimeFail marks an audit violation, corrupt JSON, or I/O failure.
	ExitRuntimeFail = 1

	// ExitUsageError marks unknown verbs or malformed CLI flags.
	ExitUsageError = 2
)

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Subcommand Dispatcher, Gate Runner & Help Text Renderer )
// =============================================================================
// Engine:        CPI-SI Portable Cognitive BIOS Tooling (cpi-si-cli)
// Stratum:       Layer 3: Orchestrated CLI Dispatcher
// Module:        cog.agent.tools.cpi_si_cli.cli
// Mission:       Execute subcommands cleanly into injected stdout/stderr
// writers.
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: Primary Verb Router
// -----------------------------------------------------------------------------

// RunCLI parses subcommands and writes output strictly to stdout / stderr.
//
// ### For Architects, Operators & Non-Programmers:
// Handles `boot`, `tail`, `bench`, `skills`, `audit`, `preflight`, `version`,
// and `help` without hiding errors or mutating repository files.
//
// ### For Systems Programmers & Engine Architects:
// Purely reentrant dispatcher safe under `go test -race` across concurrent
// tests.
func RunCLI(args []string, stdout, stderr io.Writer) int {
	if len(args) == 0 {
		PrintUsage(stdout)
		return ExitOK
	}

	sub := args[0]
	rest := args[1:]

	switch sub {
	case "help", "-h", "--help":
		PrintUsage(stdout)
		return ExitOK
	case "version", "--version":
		_, _ = fmt.Fprintf(stdout, "cpi-si-cli %s\n", CLIVersion)
		return ExitOK
	case "tail":
		return runTailCmd(rest, stdout, stderr)
	case "boot":
		return runBootCmd(rest, stdout, stderr)
	case "bench":
		return runSimpleSubstrateCmd(rest, stdout, stderr, RunBench)
	case "skills":
		return runSimpleSubstrateCmd(rest, stdout, stderr, RunSkills)
	case "audit":
		return runAuditCmd(rest, stdout, stderr)
	case "preflight":
		return RunPreflightCmd(rest, stdout, stderr)
	default:
		_, _ = fmt.Fprintf(stderr, "unknown subcommand: %q\n\n", sub)
		PrintUsage(stderr)
		return ExitUsageError
	}
}

// PrintUsage writes canonical CPI-SI CLI help synopsis to w.
//
// ### For Architects, Operators & Non-Programmers:
// Lists every available subcommand and key flag clearly in one screen.
//
// ### For Systems Programmers & Engine Architects:
// Used both on empty invocation and after invalid verb exit code `2`.
func PrintUsage(w io.Writer) {
	_, _ = fmt.Fprintln(w, "cpi-si: 0-dependency Go CLI for the CPI-SI Portable Cognitive BIOS")
	_, _ = fmt.Fprintln(w, "Usage:")
	_, _ = fmt.Fprintln(w, "  cpi-si boot      [--agents-dir PATH] [--brain-dir PATH] [--exclude ID] [--skip-tail-probe]")
	_, _ = fmt.Fprintln(w, "  cpi-si tail      [--brain-dir PATH] [--exclude ID] [--pairs 3] [--max-chars 700] [--list-only]")
	_, _ = fmt.Fprintln(w, "  cpi-si bench     [--agents-dir PATH]")
	_, _ = fmt.Fprintln(w, "  cpi-si skills    [--agents-dir PATH]")
	_, _ = fmt.Fprintln(w, "  cpi-si audit     [--agents-dir PATH]")
	_, _ = fmt.Fprintln(w, "  cpi-si preflight")
	_, _ = fmt.Fprintln(w, "  cpi-si version | help")
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.2: Per-Verb Flag Parsers & All-Gate Preflight Runner
// -----------------------------------------------------------------------------

func runTailCmd(args []string, stdout, stderr io.Writer) int {
	fs := flag.NewFlagSet("tail", flag.ContinueOnError)
	fs.SetOutput(stderr)
	brainDir := fs.String("brain-dir", DefaultBrainDir, "Antigravity brain directory")
	excludeID := fs.String("exclude", "", "Current conversation ID to skip")
	pairs := fs.Int("pairs", DefaultPairs, "Recent user/assistant pairs (clamped 1..6)")
	maxChars := fs.Int("max-chars", DefaultMaxChars, "Max runes per snippet (clamped 50..1500)")
	listOnly := fs.Bool("list-only", false, "List top 5 most recent prior transcripts without turn bodies")
	if err := fs.Parse(args); err != nil {
		return ExitUsageError
	}
	err := RunTail(TailOptions{
		BrainDir:  *brainDir,
		ExcludeID: *excludeID,
		Pairs:     *pairs,
		MaxChars:  *maxChars,
		ListOnly:  *listOnly,
	}, stdout)
	if err != nil {
		_, _ = fmt.Fprintf(stderr, "tail error: %v\n", err)
		return ExitRuntimeFail
	}
	return ExitOK
}

func runBootCmd(args []string, stdout, stderr io.Writer) int {
	fs := flag.NewFlagSet("boot", flag.ContinueOnError)
	fs.SetOutput(stderr)
	agentsDirFlag := fs.String("agents-dir", "", "Explicit path to .agents/ root")
	brainDir := fs.String("brain-dir", DefaultBrainDir, "Antigravity brain directory")
	excludeID := fs.String("exclude", "", "Current conversation ID to skip during tail check")
	skipTail := fs.Bool("skip-tail-probe", false, "Skip prior transcript metadata check")
	if err := fs.Parse(args); err != nil {
		return ExitUsageError
	}
	agentsDir, err := ResolveAgentsDir(*agentsDirFlag)
	if err != nil {
		_, _ = fmt.Fprintf(stderr, "boot error: %v\n", err)
		return ExitRuntimeFail
	}
	err = RunBoot(agentsDir, BootOptions{
		BrainDir:      *brainDir,
		ExcludeID:     *excludeID,
		SkipTailProbe: *skipTail,
	}, stdout)
	if err != nil {
		_, _ = fmt.Fprintf(stderr, "boot error: %v\n", err)
		return ExitRuntimeFail
	}
	return ExitOK
}

func runSimpleSubstrateCmd(args []string, stdout, stderr io.Writer, fn func(string, io.Writer) error) int {
	fs := flag.NewFlagSet("cmd", flag.ContinueOnError)
	fs.SetOutput(stderr)
	agentsDirFlag := fs.String("agents-dir", "", "Explicit path to .agents/ root")
	if err := fs.Parse(args); err != nil {
		return ExitUsageError
	}
	agentsDir, err := ResolveAgentsDir(*agentsDirFlag)
	if err != nil {
		_, _ = fmt.Fprintf(stderr, "resolve error: %v\n", err)
		return ExitRuntimeFail
	}
	if err := fn(agentsDir, stdout); err != nil {
		_, _ = fmt.Fprintf(stderr, "command error: %v\n", err)
		return ExitRuntimeFail
	}
	return ExitOK
}

func runAuditCmd(args []string, stdout, stderr io.Writer) int {
	fs := flag.NewFlagSet("audit", flag.ContinueOnError)
	fs.SetOutput(stderr)
	agentsDirFlag := fs.String("agents-dir", "", "Explicit path to .agents/ root")
	if err := fs.Parse(args); err != nil {
		return ExitUsageError
	}
	agentsDir, err := ResolveAgentsDir(*agentsDirFlag)
	if err != nil {
		_, _ = fmt.Fprintf(stderr, "audit error: %v\n", err)
		return ExitRuntimeFail
	}
	if _, err := RunAudit(agentsDir, stdout); err != nil {
		return ExitRuntimeFail
	}
	return ExitOK
}

// RunPreflightCmd executes all 8 substrate Block 4 self-audit cogs cleanly.
//
// ### For Architects, Operators & Non-Programmers:
// Runs every module's built-in 4-section self-audit (`types`, `paths`, `tail`,
// `bench`, `skills`, `boot`, `audit`, `cli`) in under a second and prints a
// clear
// 8/8 readiness summary.
//
// ### For Systems Programmers & Engine Architects:
// Rejects extraneous flags via FlagSet cleanly, invokes every Block 4 gate
// once,
// and returns `ExitRuntimeFail` if any CheckResult fails `IsPass()`.
func RunPreflightCmd(args []string, stdout, stderr io.Writer) int {
	fs := flag.NewFlagSet("preflight", flag.ContinueOnError)
	fs.SetOutput(stderr)
	if err := fs.Parse(args); err != nil {
		return ExitUsageError
	}

	checks := []func() CheckResult{
		PreflightTypes,
		PreflightPaths,
		PreflightTail,
		PreflightBench,
		PreflightSkills,
		PreflightBoot,
		PreflightAudit,
		PreflightCLI,
	}

	passed := 0
	for _, fn := range checks {
		res := fn()
		_, _ = fmt.Fprintln(stdout, res.FormatLine())
		if res.IsPass() {
			passed++
		}
	}
	_, _ = fmt.Fprintf(stdout, "PREFLIGHT_SUMMARY: %d/%d Block 4 gates passed\n", passed, len(checks))
	if passed != len(checks) {
		return ExitRuntimeFail
	}
	return ExitOK
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// ( Contract Verification Cogs & Pre-Flight Validation Tripwires )
// =============================================================================
// Engine:        CPI-SI Portable Cognitive BIOS Tooling (cpi-si-cli)
// Stratum:       Layer 3: Orchestrated CLI Dispatcher
// Module:        cog.agent.tools.cpi_si_cli.cli
// Mission:       4-section self-audit proving exit codes 0, 2, and help
// routing.
// =============================================================================

// PreflightCLI executes a 4-section pre-flight self-audit for substrate/cli.
//
// @triggers tests/unit/substrate/cli_test.go
//
// ### For Architects, Operators & Non-Programmers:
// Checks in memory that empty args, `version`, invalid verbs, and bad flags
// each produce their exact expected exit code cleanly.
//
// ### For Systems Programmers & Engine Architects:
// Avoids recursion into `RunPreflightCmd`; inspects `ExitOK` and
// `ExitUsageError`
// contracts cleanly via `bytes.Buffer`.
func PreflightCLI() CheckResult {
	const check = "cli_preflight_4section"

	// Micro Check 1 (Always Pass): Empty args & version return ExitOK cleanly.
	var out, errBuf bytes.Buffer
	if code := RunCLI(nil, &out, &errBuf); code != ExitOK || !strings.Contains(out.String(), "Usage:") {
		return CheckFail(SeverityCritical, ModuleIDCLI, check, fmt.Sprintf("Sec 1 help exit mismatch: %d", code))
	}
	out.Reset()
	if code := RunCLI([]string{"version"}, &out, &errBuf); code != ExitOK || !strings.Contains(out.String(), CLIVersion) {
		return CheckFail(SeverityCritical, ModuleIDCLI, check, fmt.Sprintf("Sec 1 version exit mismatch: %d", code))
	}

	// Micro Check 2 (Never Fail): Unknown verb returns ExitUsageError (2).
	out.Reset()
	errBuf.Reset()
	if code := RunCLI([]string{"bogus-verb"}, &out, &errBuf); code != ExitUsageError {
		return CheckFail(SeverityCritical, ModuleIDCLI, check, fmt.Sprintf("Sec 2 unknown verb expected 2, got %d", code))
	}

	// Micro Check 3 (Invariants & Limits): Malformed flag returns ExitUsageError
	// (2).
	out.Reset()
	errBuf.Reset()
	if code := RunCLI([]string{"tail", "--unknown-flag"}, &out, &errBuf); code != ExitUsageError {
		return CheckFail(SeverityCritical, ModuleIDCLI, check, fmt.Sprintf("Sec 3 bad flag expected 2, got %d", code))
	}

	// Micro Check 4 (Emergence): PrintUsage advertises every live subcommand.
	out.Reset()
	PrintUsage(&out)
	usage := out.String()
	for _, verb := range []string{"boot", "tail", "bench", "skills", "audit", "preflight", "version"} {
		if !strings.Contains(usage, verb) {
			return CheckFail(SeverityCritical, ModuleIDCLI, check, fmt.Sprintf("Sec 4 PrintUsage missing verb %q", verb))
		}
	}

	return CheckPass(ModuleIDCLI, check, "CLI dispatcher & exit-code matrix passed all 4 micro gates.")
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( Epilogue Readiness Handshake & Seal )
// =============================================================================
// Engine:        CPI-SI Portable Cognitive BIOS Tooling (cpi-si-cli)
// Stratum:       Layer 3: Orchestrated CLI Dispatcher
// Module:        cog.agent.tools.cpi_si_cli.cli
// Mission:       Expose synchronous readiness predicate for substrate/cli.
// =============================================================================

// IsCLIReady reports whether substrate/cli passes its 4-section self-audit.
//
// ### For Architects, Operators & Non-Programmers:
// Confirms the full CPI-SI command dispatcher is ready for binary launch.
//
// ### For Systems Programmers & Engine Architects:
// Calls `PreflightCLI().IsPass()`.
func IsCLIReady() bool {
	return PreflightCLI().IsPass()
}

// End of substrate/cli.go
