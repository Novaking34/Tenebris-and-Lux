// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundaries )
// =============================================================================
// @project       OmniCode Framework / Big House Ecosystem
// @file          tests/unit/substrate/cli_test.go
// @id            test.unit.substrate.cli
// @layer         Layer 4: Tier-2 Unit Verification Suite (CLI Router)
// @archetype     verification_suite | unit_isolation
// @version       0.1.3-dev
// @interface_rev Rev 5
// @created       2026-11-20
// @updated       2026-11-20
// @author        CreativeWorkzStudio LLC / Seanje Lenox-Wise & Silas Oren
// @repository    https://github.com/Novaking34/OmniCode_Framework
// @system        CPI-SI Portable Cognitive BIOS CLI & Substrate Tooling
// @role          Strict 1:1 Tier-2 4-Section Unit Suite for substrate/cli.go
// @maturity      Rev 5 Operational Baseline | Wave A Tools Alignment
// @chassis       zero-external-deps | In-memory RunCLI exit-code & routing test
// @scale         O(1) deterministic command dispatch suite
// @seams         in: substrate, tests/common | target: substrate/cli.go
// @invariants    1. Universal 5-Block Lifecycle Protocol (Rev 5).
//                2. Covers Always Pass, Never Fail, Limits, Emergence.
// =============================================================================

package substrate_unit_test

// =============================================================================
// BLOCK 2: OPENING BLOCK
// =============================================================================

// --- Tier 1: Standard Go Library Packages ---
import (
	"bytes"
	"strings"
	"testing"
)

// --- Tier 2: External Library Dependencies (None - 100% Go Stdlib) ---

// --- Tier 3: Internal Substrate Foundation Packages ---
import (
	"github.com/CreativeWorkzStudio/cpi-si-cli/substrate"
	"github.com/CreativeWorkzStudio/cpi-si-cli/tests/common"
)

const cliUnitSuiteName = "Tier 2 Unit: cog.agent.tools.cpi_si_cli.cli"

// =============================================================================
// BLOCK 3: BODY BLOCK / FOUR-SECTION ONTOLOGICAL TEST SUITE
// =============================================================================

// --- Section 1: Always Pass (Help, Version & All 8 Preflight Gates) ---

func TestCLIUnit_Sec1_HelpVersionAndPreflight(t *testing.T) {
	var out, errBuf bytes.Buffer
	if code := substrate.RunCLI(nil, &out, &errBuf); code != substrate.ExitOK || !strings.Contains(out.String(), "Usage:") {
		t.Fatalf("[SEC 1] empty invocation failed: code=%d out=%s", code, out.String())
	}
	out.Reset()
	if code := substrate.RunCLI([]string{"version"}, &out, &errBuf); code != substrate.ExitOK || !strings.Contains(out.String(), substrate.CLIVersion) {
		t.Fatalf("[SEC 1] version failed: code=%d out=%s", code, out.String())
	}
	out.Reset()
	if code := substrate.RunCLI([]string{"preflight"}, &out, &errBuf); code != substrate.ExitOK || !strings.Contains(out.String(), "8/8 Block 4 gates passed") {
		t.Fatalf("[SEC 1] preflight expected 8/8 PASS, got code=%d out=\n%s", code, out.String())
	}
}

// --- Section 2: Never Fail (Unknown Verbs, Bad Flags & Missing Roots) ---

func TestCLIUnit_Sec2_RejectsUnknownCommandsAndBadFlags(t *testing.T) {
	var out, errBuf bytes.Buffer
	if code := substrate.RunCLI([]string{"nonexistent"}, &out, &errBuf); code != substrate.ExitUsageError {
		t.Fatalf("[SEC 2] unknown verb expected code %d, got %d", substrate.ExitUsageError, code)
	}
	errBuf.Reset()
	if code := substrate.RunCLI([]string{"boot", "--bogus-flag"}, &out, &errBuf); code != substrate.ExitUsageError {
		t.Fatalf("[SEC 2] bad flag expected code %d, got %d", substrate.ExitUsageError, code)
	}
	errBuf.Reset()
	if code := substrate.RunCLI([]string{"audit", "--agents-dir", t.TempDir()}, &out, &errBuf); code != substrate.ExitRuntimeFail {
		t.Fatalf("[SEC 2] missing .agents expected code %d, got %d", substrate.ExitRuntimeFail, code)
	}
}

// --- Section 3: Invariants & Limits (Deterministic Exit Code Separation) ---

func TestCLIUnit_Sec3_ExitCodeSeparationInvariants(t *testing.T) {
	if substrate.ExitOK != 0 || substrate.ExitRuntimeFail != 1 || substrate.ExitUsageError != 2 {
		t.Fatalf("[SEC 3] exit code constants shifted unexpectedly")
	}
}

// --- Section 4: Emergence (Clean Tree Dispatch, Preflight & Ready) ---

func TestCLIUnit_Sec4_CleanSandboxCommandsPreflightAndReadiness(t *testing.T) {
	agents := common.BuildMinimalCleanSubstrate(t)
	for _, verb := range []string{"bench", "skills", "audit"} {
		var out, errBuf bytes.Buffer
		if code := substrate.RunCLI([]string{verb, "--agents-dir", agents}, &out, &errBuf); code != substrate.ExitOK {
			t.Fatalf("[SEC 4] verb %q failed code=%d stdout=%s stderr=%s", verb, code, out.String(), errBuf.String())
		}
	}
	common.AssertCheckPass(t, substrate.PreflightCLI(), "PreflightCLI")
	if !substrate.IsCLIReady() {
		t.Fatalf("[SEC 4] IsCLIReady returned false")
	}
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// =============================================================================

func TestCLIUnit_Block4_SuiteSummaryAudit(t *testing.T) {
	t.Logf("%s: 8/8 preflight runner, exit codes (0/1/2) & sandbox verbs verified.", cliUnitSuiteName)
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================
// End of tests/unit/substrate/cli_test.go
