// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundaries )
// =============================================================================
// @project       OmniCode Framework / Big House Ecosystem
// @file          cmd/cpi-si/main.go
// @id            cmd.cpi_si.cli
// @layer         Layer 4: Thin Imperative CPI-SI Substrate CLI Entrypoint
// @archetype     operational | cli_entrypoint
// @version       0.1.3-dev
// @interface_rev Rev 5
// @created       2026-11-20
// @updated       2026-11-20
// @author        CreativeWorkzStudio LLC / Seanje Lenox-Wise & Silas Oren
// @repository    https://github.com/Novaking34/OmniCode_Framework
// @system        CPI-SI Portable Cognitive BIOS CLI & Substrate Tooling
// @role          Thin Process Entrypoint Binding OS Args to substrate.RunCLI
// @maturity      Rev 5 Operational Baseline | Wave A Tools Alignment
// @chassis       zero-external-deps | Pure Go stdlib binary delegating to Layer
//                3
// @scale         O(1) boot dispatch into substrate package cogs
// @seams         in: github.com/CreativeWorkzStudio/cpi-si-cli/substrate |
//                run_by: ../../runners/{run.sh,macos/run.sh,windows/run.ps1}
// @invariants    1. Universal 5-Block Lifecycle Protocol (Rev 5).
//                2. Keeps all testable logic cleanly inside substrate package.
//                3. Compiled externally by runners/ — zero binaries in
//                .agents/.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Thin OS process wrapper (`cmd/cpi-si`) handing `os.Args[1:]`, `os.Stdout`,
//   and `os.Stderr` straight into `substrate.RunCLI` and exiting with the
//   deterministic status code returned.
//
//   ### For Architects, Operators & Non-Programmers:
//   Just like `cmd/faithnet-node` in FaithNet keeps the executable wrapper
//   clean
//   and thin over the tested packages underneath, `cmd/cpi-si/main.go` gives
//   Linux, macOS, and Windows runners one clean Go entrypoint.
//
//   ### For Systems Programmers & Engine Architects:
//   Follows the FaithNet 5-Block CLI pattern including Block 4 `PreflightMain`
//   and Block 5 `IsMainReady` + `ExecuteToolSpecCard`.
// =============================================================================

package main

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( CLI Module Anchor, Tiered Imports & Defaults )
// =============================================================================
// Engine:        CPI-SI Portable Cognitive BIOS Tooling (cpi-si-cli)
// Stratum:       Layer 4: Thin Process Entrypoint
// Module:        cmd.cpi_si.cli
// Mission:       Expose thin OS process entrypoint over substrate cogs.
// =============================================================================

// --- Tier 1: Standard Go Library Packages ---
import (
	"fmt"
	"os"
	"strings"
)

// --- Tier 2: External Library Dependencies (None - 100% Go Stdlib) ---

// --- Tier 3: Internal Substrate Foundation Packages ---
import "github.com/CreativeWorkzStudio/cpi-si-cli/substrate"

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Operational Logical Anchor
// -----------------------------------------------------------------------------

const (
	// ModuleIDMain is the canonical logical ID for cmd/cpi-si/main.
	ModuleIDMain = "cmd.cpi_si.cli"
)

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Thin Imperative Process Execution Entrypoint )
// =============================================================================
// Engine:        CPI-SI Portable Cognitive BIOS Tooling (cpi-si-cli)
// Stratum:       Layer 4: Thin Process Entrypoint
// Module:        cmd.cpi_si.cli
// Mission:       Delegate OS arguments directly to substrate.RunCLI cleanly.
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: OS Process Entrypoint
// -----------------------------------------------------------------------------

func main() {
	os.Exit(substrate.RunCLI(os.Args[1:], os.Stdout, os.Stderr))
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// ( Contract Verification Cogs & Pre-Flight Validation Tripwires )
// =============================================================================
// Engine:        CPI-SI Portable Cognitive BIOS Tooling (cpi-si-cli)
// Stratum:       Layer 4: Thin Process Entrypoint
// Module:        cmd.cpi_si.cli
// Mission:       Verify Layer 3 substrate readiness and tool spec card intact.
// =============================================================================

// PreflightMain verifies CLI Layer-4 readiness over the substrate package.
//
// @triggers tests/unit/cmd/main_cli_test.go
//
// ### For Architects, Operators & Non-Programmers:
// Confirms the underlying CLI dispatcher and tool specification card both pass
// before the binary executes on any platform.
//
// ### For Systems Programmers & Engine Architects:
// Chains `substrate.IsCLIReady()` with spec-card structural assertions.
func PreflightMain() substrate.CheckResult {
	const check = "main_cli_preflight_4section"

	// Micro Check 1 (Always Pass): CLI layer reports healthy.
	if !substrate.IsCLIReady() {
		return substrate.CheckFail(substrate.SeverityCritical, ModuleIDMain, check, "Sec 1 substrate.IsCLIReady returned false")
	}

	// Micro Check 2 (Never Fail): Spec card renders cleanly without empty output.
	card := ExecuteToolSpecCard()
	if strings.TrimSpace(card) == "" {
		return substrate.CheckFail(substrate.SeverityCritical, ModuleIDMain, check, "Sec 2 ExecuteToolSpecCard returned empty string")
	}

	// Micro Check 3 (Invariants & Limits): CLIVersion and 0-deps notice present.
	if !strings.Contains(card, "cpi-si-cli") || !strings.Contains(card, "0-Deps") {
		return substrate.CheckFail(substrate.SeverityCritical, ModuleIDMain, check, "Sec 3 spec card missing identity or 0-Deps badge")
	}

	// Micro Check 4 (Emergence): Ready handshake integrates seamlessly.
	return substrate.CheckPass(ModuleIDMain, check, "Thin Layer-4 cmd/cpi-si entrypoint verified over Layer-3 substrate.")
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( Epilogue Readiness Handshake & 60-Column Specification Card )
// =============================================================================
// Engine:        CPI-SI Portable Cognitive BIOS Tooling (cpi-si-cli)
// Stratum:       Layer 4: Thin Process Entrypoint
// Module:        cmd.cpi_si.cli
// Mission:       Expose Layer-4 readiness predicate and 60-column spec card.
// =============================================================================

// IsMainReady reports whether cmd/cpi-si passes its Layer-4 preflight gate.
//
// ### For Architects, Operators & Non-Programmers:
// Quick health gate confirming the CLI entrypoint is ready.
//
// ### For Systems Programmers & Engine Architects:
// Delegates directly to `PreflightMain().IsPass()`.
func IsMainReady() bool {
	return PreflightMain().IsPass()
}

// -----------------------------------------------------------------------------
// Sub-Assembly 5.1: 60-Column Specification Telemetry Card
// -----------------------------------------------------------------------------

// ExecuteToolSpecCard renders a compact 60-column CPI-SI CLI spec card.
//
// ### For Architects, Operators & Non-Programmers:
// Displays the clean identity badge for the CPI-SI portable substrate tool.
//
// ### For Systems Programmers & Engine Architects:
// Matches the 60-column spec box discipline used across `FaithNet`.
func ExecuteToolSpecCard() string {
	return fmt.Sprintf(
		"+------------------------------------------------------------+\n"+
			"| CPI-SI PORTABLE SUBSTRATE CLI (cpi-si-cli)                 |\n"+
			"+------------------------------------------------------------+\n"+
			"| Module:   %-48s |\n"+
			"| Deps:     0-Deps Pure Go Stdlib (Linux / macOS / Windows)  |\n"+
			"| Verbs:    boot | tail | bench | skills | audit | preflight |\n"+
			"+------------------------------------------------------------+",
		ModuleIDMain+" (v0.1.3-dev, zero deps)",
	)
}

// End of cmd/cpi-si/main.go
