// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundaries )
// =============================================================================
// @project       OmniCode Framework / Big House Ecosystem
// @file          tests/unit/substrate/bench_test.go
// @id            test.unit.substrate.bench
// @layer         Layer 4: Tier-2 Unit Verification Suite (Bench Spatial Triage)
// @archetype     verification_suite | unit_isolation
// @version       0.1.3-dev
// @interface_rev Rev 5
// @created       2026-11-20
// @updated       2026-11-20
// @author        CreativeWorkzStudio LLC / Seanje Lenox-Wise & Silas Oren
// @repository    https://github.com/Novaking34/OmniCode_Framework
// @system        CPI-SI Portable Cognitive BIOS CLI & Substrate Tooling
// @role          Strict 1:1 Tier-2 4-Section Unit Suite for substrate/bench.go
// @maturity      Rev 5 Operational Baseline | Wave A Tools Alignment
// @chassis       zero-external-deps | Isolated running writing bench bay tests
// @scale         O(N_bench_files) directory listing checks
// @seams         in: substrate, tests/common | target: substrate/bench.go
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
	"path/filepath"
	"strings"
	"testing"
)

// --- Tier 2: External Library Dependencies (None - 100% Go Stdlib) ---

// --- Tier 3: Internal Substrate Foundation Packages ---
import (
	"github.com/CreativeWorkzStudio/cpi-si-cli/substrate"
	"github.com/CreativeWorkzStudio/cpi-si-cli/tests/common"
)

const benchUnitSuiteName = "Tier 2 Unit: cog.agent.tools.cpi_si_cli.bench"

// =============================================================================
// BLOCK 3: BODY BLOCK / FOUR-SECTION ONTOLOGICAL TEST SUITE
// =============================================================================

// --- Section 1: Always Pass (Nominal Four-Bay Inventory) ---

func TestBenchUnit_Sec1_InspectAndRenderFourBays(t *testing.T) {
	agents := common.BuildMinimalCleanSubstrate(t)
	common.WriteFixtureFile(t, filepath.Join(agents, "scratchpads", "report_outs", "ready.md"), "# Ready\n")
	common.WriteFixtureFile(t, filepath.Join(agents, "scratchpads", "studies", "bridge.md"), "# Bridge\n")

	sum, err := substrate.InspectBench(agents)
	if err != nil || !sum.HasGuide || !sum.NoRegistryOK || sum.TotalWorkingNotes() != 2 {
		t.Fatalf("[SEC 1] unexpected bench summary: %+v err=%v", sum, err)
	}
	var buf bytes.Buffer
	if err := substrate.RunBench(agents, &buf); err != nil {
		t.Fatal(err)
	}
	out := buf.String()
	if !strings.Contains(out, "quick.md") || !strings.Contains(out, "ready.md") {
		t.Fatalf("[SEC 1] RunBench missing bay entries:\n%s", out)
	}
}

// --- Section 2: Never Fail (Detect Forbidden scratchpads_registry.json) ---

func TestBenchUnit_Sec2_DetectsForbiddenRegistryAndMissingScratchpads(t *testing.T) {
	agents := common.BuildMinimalCleanSubstrate(t)
	common.WriteFixtureFile(t, filepath.Join(agents, "scratchpads", "scratchpads_registry.json"), `{}`)
	sum, err := substrate.InspectBench(agents)
	if err != nil || sum.NoRegistryOK {
		t.Fatalf("[SEC 2] expected NoRegistryOK=false when registry exists: %+v err=%v", sum, err)
	}

	emptySum, err := substrate.InspectBench(t.TempDir())
	if err != nil || emptySum.HasGuide || emptySum.TotalWorkingNotes() != 0 {
		t.Fatalf("[SEC 2] absent scratchpads should degrade cleanly: %+v err=%v", emptySum, err)
	}
}

// --- Section 3: Invariants & Limits (.gitkeep Placeholder Suppression) ---

func TestBenchUnit_Sec3_SuppressesGitkeepPlaceholders(t *testing.T) {
	agents := t.TempDir()
	common.WriteFixtureFile(t, filepath.Join(agents, "scratchpads", "active", ".gitkeep"), "")
	common.WriteFixtureFile(t, filepath.Join(agents, "scratchpads", "report_outs", ".gitkeep"), "")

	var buf bytes.Buffer
	if err := substrate.RunBench(agents, &buf); err != nil {
		t.Fatal(err)
	}
	out := buf.String()
	if strings.Contains(out, ".gitkeep") || !strings.Contains(out, "(empty bay)") {
		t.Fatalf("[SEC 3] .gitkeep leaked into RunBench or empty bay missing:\n%s", out)
	}
}

// --- Section 4: Emergence (Sort Order, Preflight & Readiness) ---

func TestBenchUnit_Sec4_SortedOrderPreflightAndReadiness(t *testing.T) {
	agents := common.BuildMinimalCleanSubstrate(t)
	common.WriteFixtureFile(t, filepath.Join(agents, "scratchpads", "active", "z_note.md"), "z\n")
	common.WriteFixtureFile(t, filepath.Join(agents, "scratchpads", "active", "a_note.md"), "a\n")

	sum, err := substrate.InspectBench(agents)
	if err != nil || len(sum.Active) != 3 || sum.Active[0].Name != "a_note.md" || sum.Active[2].Name != "z_note.md" {
		t.Fatalf("[SEC 4] expected sorted active bay items, got %+v (err=%v)", sum.Active, err)
	}
	common.AssertCheckPass(t, substrate.PreflightBench(), "PreflightBench")
	if !substrate.IsBenchReady() {
		t.Fatalf("[SEC 4] IsBenchReady returned false")
	}
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// =============================================================================

func TestBenchUnit_Block4_SuiteSummaryAudit(t *testing.T) {
	t.Logf("%s: 4-bay spatial triage, .gitkeep filter & No-Registry law verified.", benchUnitSuiteName)
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================
// End of tests/unit/substrate/bench_test.go
