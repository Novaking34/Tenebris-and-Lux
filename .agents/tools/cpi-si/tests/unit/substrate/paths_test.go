// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundaries )
// =============================================================================
// @project       OmniCode Framework / Big House Ecosystem
// @file          tests/unit/substrate/paths_test.go
// @id            test.unit.substrate.paths
// @layer         Layer 4: Tier-2 Unit Verification Suite (Path Resolution)
// @archetype     verification_suite | unit_isolation
// @version       0.1.3-dev
// @interface_rev Rev 5
// @created       2026-11-20
// @updated       2026-11-20
// @author        CreativeWorkzStudio LLC / Seanje Lenox-Wise & Silas Oren
// @repository    https://github.com/Novaking34/OmniCode_Framework
// @system        CPI-SI Portable Cognitive BIOS CLI & Substrate Tooling
// @role          Strict 1:1 Tier-2 4-Section Unit Suite for substrate/paths.go
// @maturity      Rev 5 Operational Baseline | Wave A Tools Alignment
// @chassis       zero-external-deps | Isolated tempdir BIOS resolution checks
// @scale         O(depth) directory tree walk checks
// @seams         in: substrate, tests/common | target: substrate/paths.go
// @invariants    1. Universal 5-Block Lifecycle Protocol (Rev 5).
//                2. Covers Always Pass, Never Fail, Limits, Emergence.
// =============================================================================

package substrate_unit_test

// =============================================================================
// BLOCK 2: OPENING BLOCK
// =============================================================================

// --- Tier 1: Standard Go Library Packages ---
import (
	"os"
	"path/filepath"
	"testing"
)

// --- Tier 2: External Library Dependencies (None - 100% Go Stdlib) ---

// --- Tier 3: Internal Substrate Foundation Packages ---
import (
	"github.com/CreativeWorkzStudio/cpi-si-cli/substrate"
	"github.com/CreativeWorkzStudio/cpi-si-cli/tests/common"
)

const pathsUnitSuiteName = "Tier 2 Unit: cog.agent.tools.cpi_si_cli.paths"

// =============================================================================
// BLOCK 3: BODY BLOCK / FOUR-SECTION ONTOLOGICAL TEST SUITE
// =============================================================================

// --- Section 1: Always Pass (Nominal Explicit, Env & Walk-Up Resolution) ---

func TestPathsUnit_Sec1_ResolveExplicitAndWalkUp(t *testing.T) {
	tmp := t.TempDir()
	agents := filepath.Join(tmp, ".agents")
	common.WriteFixtureFile(t, filepath.Join(agents, "README.md"), "# BIOS\n")
	_ = os.MkdirAll(filepath.Join(agents, "skills"), 0o755)
	common.WriteFixtureFile(t, filepath.Join(tmp, "README.md"), "# Repo\n")

	if got, err := substrate.ResolveAgentsDir(tmp); err != nil || got != agents {
		t.Fatalf("[SEC 1] repo-root resolve expected %s, got %q (err=%v)", agents, got, err)
	}
	if got, err := substrate.ResolveAgentsDir(agents); err != nil || got != agents {
		t.Fatalf("[SEC 1] direct resolve expected %s, got %q (err=%v)", agents, got, err)
	}

	sub := filepath.Join(tmp, "deep", "nested")
	_ = os.MkdirAll(sub, 0o755)
	t.Setenv(substrate.EnvAgentsDir, "")
	origWD, err := os.Getwd()
	if err != nil {
		t.Fatal(err)
	}
	defer func() { _ = os.Chdir(origWD) }()
	if err := os.Chdir(sub); err != nil {
		t.Fatal(err)
	}
	if got, err := substrate.ResolveAgentsDir(""); err != nil || got != agents {
		t.Fatalf("[SEC 1] walk-up expected %s, got %q (err=%v)", agents, got, err)
	}
}

// --- Section 2: Never Fail (Reject Plain Non-BIOS Repos & Missing Paths) ---

func TestPathsUnit_Sec2_RejectsPlainRepoAndInvalidExplicit(t *testing.T) {
	tmp := t.TempDir()
	common.WriteFixtureFile(t, filepath.Join(tmp, "README.md"), "# Plain Repo\n")
	if _, err := substrate.ResolveAgentsDir(tmp); err == nil {
		t.Fatalf("[SEC 2] plain README repo should not pass IsAgentsRoot")
	}
	if _, err := substrate.ResolveAgentsDir(filepath.Join(tmp, "nonexistent")); err == nil {
		t.Fatalf("[SEC 2] missing explicit path unexpectedly succeeded")
	}
}

// --- Section 3: Invariants & Limits (File/Dir Disjointness & ~ Path) ---

func TestPathsUnit_Sec3_PredicatesAndHomeExpansion(t *testing.T) {
	tmp := t.TempDir()
	f := filepath.Join(tmp, "doc.md")
	common.WriteFixtureFile(t, f, "hi\n")
	if !substrate.FileExists(f) || substrate.DirExists(f) {
		t.Fatalf("[SEC 3] file predicate confused regular file with dir")
	}
	if !substrate.DirExists(tmp) || substrate.FileExists(tmp) {
		t.Fatalf("[SEC 3] dir predicate confused dir with regular file")
	}
	if substrate.ExpandHome("/abs/unchanged") != "/abs/unchanged" || substrate.ExpandHome("") != "" {
		t.Fatalf("[SEC 3] ExpandHome altered non-home path")
	}
}

// --- Section 4: Emergence (Env Precedence, Preflight & Readiness) ---

func TestPathsUnit_Sec4_EnvOverridePreflightAndReadiness(t *testing.T) {
	tmp := t.TempDir()
	agents := filepath.Join(tmp, "custom-agents")
	common.WriteFixtureFile(t, filepath.Join(agents, "README.md"), "# BIOS\n")
	_ = os.MkdirAll(filepath.Join(agents, "host"), 0o755)
	t.Setenv(substrate.EnvAgentsDir, agents)

	if got, err := substrate.ResolveAgentsDir(""); err != nil || got != agents {
		t.Fatalf("[SEC 4] env override expected %s, got %q (err=%v)", agents, got, err)
	}
	common.AssertCheckPass(t, substrate.PreflightPaths(), "PreflightPaths")
	if !substrate.IsPathsReady() {
		t.Fatalf("[SEC 4] IsPathsReady returned false")
	}
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// =============================================================================

func TestPathsUnit_Block4_SuiteSummaryAudit(t *testing.T) {
	t.Logf("%s: BIOS vs repo-root discrimination & env overrides verified.", pathsUnitSuiteName)
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================
// End of tests/unit/substrate/paths_test.go
