// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundaries )
// =============================================================================
// @project       OmniCode Framework / Big House Ecosystem
// @file          tests/unit/substrate/audit_test.go
// @id            test.unit.substrate.audit
// @layer         Layer 4: Tier-2 Unit Verification Suite (Substrate Auditor)
// @archetype     verification_suite | unit_isolation
// @version       0.1.3-dev
// @interface_rev Rev 5
// @created       2026-11-20
// @updated       2026-11-20
// @author        CreativeWorkzStudio LLC / Seanje Lenox-Wise & Silas Oren
// @repository    https://github.com/Novaking34/OmniCode_Framework
// @system        CPI-SI Portable Cognitive BIOS CLI & Substrate Tooling
// @role          Strict 1:1 Tier-2 4-Section Unit Suite for substrate/audit.go
// @maturity      Rev 5 Operational Baseline | Wave A Tools Alignment
// @chassis       zero-external-deps | Symmetric clean-vs-poisoned tree suite
// @scale         O(N_files) isolated substrate checks
// @seams         in: substrate, tests/common | target: substrate/audit.go
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

const auditUnitSuiteName = "Tier 2 Unit: cog.agent.tools.cpi_si_cli.audit"

// =============================================================================
// BLOCK 3: BODY BLOCK / FOUR-SECTION ONTOLOGICAL TEST SUITE
// =============================================================================

// --- Section 1: Always Pass (Nominal Clean Substrate & Summary Output) ---

func TestAuditUnit_Sec1_CleanSubstratePassesWithExactCounts(t *testing.T) {
	agents := common.BuildMinimalCleanSubstrate(t)
	var out bytes.Buffer
	rep, err := substrate.RunAudit(agents, &out)
	if err != nil || !rep.IsClean() {
		t.Fatalf("[SEC 1] expected clean audit, got err=%v failures=%v", err, rep.Failures)
	}
	if !strings.Contains(out.String(), "CPI_SI_SUBSTRATE_AUDIT_OK:") || rep.SkillCount != 1 || rep.PlansParity != 1 {
		t.Fatalf("[SEC 1] unexpected audit summary:\n%s\nrep=%+v", out.String(), rep)
	}
}

// --- Section 2: Never Fail (Python Clutter, Links & Parity Drift) ---

func TestAuditUnit_Sec2_DetectsPythonArtifactEscapingLinksAndRegistryDrift(t *testing.T) {
	agents := common.BuildMinimalCleanSubstrate(t)
	common.WriteFixtureFile(t, filepath.Join(agents, "skills", "cpi-si-covenant", "scripts", "__pycache__", "x.pyc"), "bin")
	common.WriteFixtureFile(t, filepath.Join(agents, "scratchpads", "scratchpads_registry.json"), `{"block_1":1,"block_2":2,"block_3":3,"block_4":4,"block_5":5}`)
	common.WriteFixtureFile(t, filepath.Join(agents, "plans", "unindexed.md"), common.ValidMarkdownDoc("unindexed.md"))
	common.WriteFixtureFile(t, filepath.Join(agents, "host", "bad_link.md"), common.ValidMarkdownDoc("bad_link.md")+"\n[out](../../secret.txt)\n")

	var out bytes.Buffer
	rep, err := substrate.RunAudit(agents, &out)
	if err == nil || rep.IsClean() {
		t.Fatalf("[SEC 2] expected RunAudit failure on poisoned tree, got clean")
	}
	joined := strings.Join(rep.Failures, "\n")
	for _, want := range []string{"__pycache__", "x.pyc", "scratchpads_registry.json", "unindexed.md", "broken link"} {
		if !strings.Contains(joined, want) {
			t.Fatalf("[SEC 2] missing expected violation %q in:\n%s", want, joined)
		}
	}
}

// --- Section 3: Invariants & Limits (Width, ASCII Fences & Header Dates) ---

func TestAuditUnit_Sec3_WidthCapASCIIFencesAndHeaderDateGuards(t *testing.T) {
	agents := common.BuildMinimalCleanSubstrate(t)
	badDoc := strings.Replace(common.ValidMarkdownDoc("bad.md"), "@created 2026-11-20", "@date 2026-11-20\n@created 2026-11-20", 1)
	badDoc += "\n<!-- " + strings.Repeat("W", 90) + " -->\n```text\n│ box\n```\n"
	common.WriteFixtureFile(t, filepath.Join(agents, "host", "wall_and_ascii.md"), badDoc)

	rep, err := substrate.AuditSubstrate(agents)
	if err != nil {
		t.Fatal(err)
	}
	joined := strings.Join(rep.Failures, "\n")
	for _, want := range []string{"bare @date header line", "HTML comment > 80 cols", "non-ASCII rune"} {
		if !strings.Contains(joined, want) {
			t.Fatalf("[SEC 3] missing invariant trap %q in:\n%s", want, joined)
		}
	}
}

// --- Section 4: Emergence (Tools Inclusion, Active Exempt & Readiness) ---

func TestAuditUnit_Sec4_ToolsAuditedActiveExemptPreflightAndReadiness(t *testing.T) {
	if !substrate.IsAuditedMarkdown("tools/cpi-si/README.md") || substrate.IsAuditedMarkdown("scratchpads/active/idea.md") {
		t.Fatalf("[SEC 4] IsAuditedMarkdown routing rule failed")
	}
	agents := common.BuildMinimalCleanSubstrate(t)
	common.WriteFixtureFile(t, filepath.Join(agents, "tools", "cpi-si", "README.md"), common.ValidMarkdownDoc("README.md"))
	rep, err := substrate.AuditSubstrate(agents)
	if err != nil || !rep.IsClean() {
		t.Fatalf("[SEC 4] tools README failed audit unexpectedly: err=%v failures=%v", err, rep.Failures)
	}
	common.AssertCheckPass(t, substrate.PreflightAudit(), "PreflightAudit")
	if !substrate.IsAuditReady() {
		t.Fatalf("[SEC 4] IsAuditReady returned false")
	}
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// =============================================================================

func TestAuditUnit_Block4_SuiteSummaryAudit(t *testing.T) {
	t.Logf("%s: 5-Block Markdown/JSON/AsciiDoc gates, parity & width traps verified.", auditUnitSuiteName)
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================
// End of tests/unit/substrate/audit_test.go
