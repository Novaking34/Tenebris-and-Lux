// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundaries )
// =============================================================================
// @project       OmniCode Framework / Big House Ecosystem
// @file          tests/unit/cmd/main_cli_test.go
// @id            test.unit.cmd.main_cli
// @layer         Layer 4: Tier-2 Unit Verification Suite (Compiled CLI Binary)
// @archetype     verification_suite | unit_isolation
// @version       0.1.3-dev
// @interface_rev Rev 5
// @created       2026-11-20
// @updated       2026-11-20
// @author        CreativeWorkzStudio LLC / Seanje Lenox-Wise & Silas Oren
// @repository    https://github.com/Novaking34/OmniCode_Framework
// @system        CPI-SI Portable Cognitive BIOS CLI & Substrate Tooling
// @role          Strict 1:1 Tier-2 4-Section Unit Suite for cmd/cpi-si/main.go
// @maturity      Rev 5 Operational Baseline | Wave A Tools Alignment
// @chassis       zero-external-deps | Compiles cmd/cpi-si to t.TempDir()
//                cleanly
// @scale         O(1) binary build + CLI process exit-code probes
// @seams         in: substrate, tests/common | target: cmd/cpi-si/main.go
// @invariants    1. Universal 5-Block Lifecycle Protocol (Rev 5).
//                2. Compiles binary strictly inside t.TempDir() (zero repo
//                bins).
//                3. Covers Always Pass, Never Fail, Limits, Emergence.
// =============================================================================

package cmd_unit_test

// =============================================================================
// BLOCK 2: OPENING BLOCK
// =============================================================================

// --- Tier 1: Standard Go Library Packages ---
import (
	"os"
	"os/exec"
	"path/filepath"
	"runtime"
	"strings"
	"testing"
)

// --- Tier 2: External Library Dependencies (None - 100% Go Stdlib) ---

// --- Tier 3: Internal Substrate Foundation Packages ---
import (
	"github.com/CreativeWorkzStudio/cpi-si-cli/substrate"
	"github.com/CreativeWorkzStudio/cpi-si-cli/tests/common"
)

const cmdCLIUnitSuiteName = "Tier 2 Unit: cmd.cpi_si.cli (Compiled Binary Entrypoint)"

func locateModuleRoot(t *testing.T) string {
	t.Helper()
	dir, err := os.Getwd()
	if err != nil {
		t.Fatal(err)
	}
	for {
		if _, err := os.Stat(filepath.Join(dir, "go.mod")); err == nil {
			return dir
		}
		parent := filepath.Dir(dir)
		if parent == dir {
			t.Fatalf("could not locate go.mod starting from %s", dir)
		}
		dir = parent
	}
}

func buildTempBinary(t *testing.T) string {
	t.Helper()
	modRoot := locateModuleRoot(t)
	binName := "cpi-si-test"
	if runtime.GOOS == "windows" {
		binName += ".exe"
	}
	binPath := filepath.Join(t.TempDir(), binName)
	cmd := exec.Command("go", "build", "-o", binPath, "./cmd/cpi-si")
	cmd.Dir = modRoot
	if out, err := cmd.CombinedOutput(); err != nil {
		t.Fatalf("go build ./cmd/cpi-si failed: %v\n%s", err, string(out))
	}
	return binPath
}

// =============================================================================
// BLOCK 3: BODY BLOCK / FOUR-SECTION ONTOLOGICAL TEST SUITE
// =============================================================================

// --- Section 1: Always Pass (Real Binary Help, Version & Preflight) ---

func TestCmdCLIUnit_Sec1_CompiledBinaryNominalVerbs(t *testing.T) {
	bin := buildTempBinary(t)

	out, err := exec.Command(bin, "version").CombinedOutput()
	if err != nil || !strings.Contains(string(out), substrate.CLIVersion) {
		t.Fatalf("[SEC 1] binary version failed: err=%v out=%s", err, string(out))
	}

	preOut, err := exec.Command(bin, "preflight").CombinedOutput()
	if err != nil || !strings.Contains(string(preOut), "8/8 Block 4 gates passed") {
		t.Fatalf("[SEC 1] binary preflight failed: err=%v out=%s", err, string(preOut))
	}
}

// --- Section 2: Never Fail (Real Process Exit Codes 2 and 1) ---

func TestCmdCLIUnit_Sec2_BinaryExitCodesForUsageAndRuntimeFailures(t *testing.T) {
	bin := buildTempBinary(t)

	cmdBadVerb := exec.Command(bin, "no-such-cmd")
	err := cmdBadVerb.Run()
	if exitErr, ok := err.(*exec.ExitError); !ok || exitErr.ExitCode() != substrate.ExitUsageError {
		t.Fatalf("[SEC 2] unknown verb expected exit 2, got err=%v", err)
	}

	cmdMissingAgents := exec.Command(bin, "audit", "--agents-dir", t.TempDir())
	err = cmdMissingAgents.Run()
	if exitErr, ok := err.(*exec.ExitError); !ok || exitErr.ExitCode() != substrate.ExitRuntimeFail {
		t.Fatalf("[SEC 2] missing agents dir expected exit 1, got err=%v", err)
	}
}

// --- Section 3: Invariants & Limits (Zero Binary Artifacts in Module Root) ---

func TestCmdCLIUnit_Sec3_ZeroBinaryResidueInModuleRoot(t *testing.T) {
	modRoot := locateModuleRoot(t)
	for _, forbidden := range []string{"cpi-si", "cpi-si.exe", "main"} {
		if _, err := os.Stat(filepath.Join(modRoot, forbidden)); err == nil {
			t.Fatalf("[SEC 3] forbidden binary artifact %q found inside %s", forbidden, modRoot)
		}
	}
}

// --- Section 4: Emergence (End-to-End Clean Sandbox Binary Execution) ---

func TestCmdCLIUnit_Sec4_CleanSandboxEndToEndFromBinary(t *testing.T) {
	bin := buildTempBinary(t)
	agents := common.BuildMinimalCleanSubstrate(t)

	for _, args := range [][]string{
		{"boot", "--agents-dir", agents, "--skip-tail-probe"},
		{"bench", "--agents-dir", agents},
		{"skills", "--agents-dir", agents},
		{"audit", "--agents-dir", agents},
	} {
		out, err := exec.Command(bin, args...).CombinedOutput()
		if err != nil {
			t.Fatalf("[SEC 4] binary %v failed: err=%v out=%s", args, err, string(out))
		}
	}
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// =============================================================================

func TestCmdCLIUnit_Block4_SuiteSummaryAudit(t *testing.T) {
	t.Logf("%s: real compiled binary exit codes (0/1/2), clean tree & 8/8 preflight verified.", cmdCLIUnitSuiteName)
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================
// End of tests/unit/cmd/main_cli_test.go
