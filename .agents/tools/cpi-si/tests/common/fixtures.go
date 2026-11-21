// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundaries )
// =============================================================================
// @project       OmniCode Framework / Big House Ecosystem
// @file          tests/common/fixtures.go
// @id            test.common.fixtures
// @layer         Layer 4: Shared Test Harness, Assertions & Temp Fixtures
// @archetype     verification_harness | shared_fixtures
// @version       0.1.3-dev
// @interface_rev Rev 5
// @created       2026-11-20
// @updated       2026-11-20
// @author        CreativeWorkzStudio LLC / Seanje Lenox-Wise & Silas Oren
// @repository    https://github.com/Novaking34/OmniCode_Framework
// @system        CPI-SI Portable Cognitive BIOS CLI & Substrate Tooling
// @role          Reusable Sandbox Builders & Preflight Receipt Assertions
// @maturity      Rev 5 Operational Baseline | Wave A Tools Alignment
// @chassis       zero-external-deps | Isolated t.TempDir() helpers only
// @scale         O(1) deterministic sandbox generation
// @seams         in: substrate | used_by: tests/unit/substrate, tests/unit/cmd
// @invariants    1. Universal 5-Block Lifecycle Protocol (Rev 5).
//                2. Every fixture uses t.TempDir() so zero live files mutate.
//                3. Pure Go stdlib only.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Provides uniform assertion helpers and isolated `.agents/` fixture builders
//   used across all 9 micro unit suites inside `tests/unit/`.
//
//   ### For Architects, Operators & Non-Programmers:
//   Mirrors `FaithNet/tests/common/` so test assertions and disposable test
//   sandboxes stay consistent instead of duplicated in every test file.
//
//   ### For Systems Programmers & Engine Architects:
//   Exports `AssertCheckPass`, `AssertCheckFail`, `WriteFixtureFile`,
//   `WriteFakeTranscript`, `ValidMarkdownDoc`, and
//   `BuildMinimalCleanSubstrate`.
// =============================================================================

package common

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Tiered Imports & Module Anchor )
// =============================================================================
// Engine:        CPI-SI Portable Cognitive BIOS Tooling (cpi-si-cli)
// Stratum:       Layer 4: Shared Verification Harness
// Module:        test.common.fixtures
// Mission:       Expose uniform sandbox constructors and receipt assertions.
// =============================================================================

// --- Tier 1: Standard Go Library Packages ---
import (
	"os"
	"path/filepath"
	"strings"
	"testing"
	"time"
)

// --- Tier 2: External Library Dependencies (None - 100% Go Stdlib) ---

// --- Tier 3: Internal Substrate Foundation Packages ---
import "github.com/CreativeWorkzStudio/cpi-si-cli/substrate"

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Logical Anchor
// -----------------------------------------------------------------------------

const (
	// ModuleIDCommonFixtures is the logical ID for shared test fixtures.
	ModuleIDCommonFixtures = "test.common.fixtures"
)

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Receipt Assertions & Isolated Tempdir Sandbox Constructors )
// =============================================================================
// Engine:        CPI-SI Portable Cognitive BIOS Tooling (cpi-si-cli)
// Stratum:       Layer 4: Shared Verification Harness
// Module:        test.common.fixtures
// Mission:       Supply clean assertions and isolated temp BIOS trees.
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: Preflight CheckResult Assertions
// -----------------------------------------------------------------------------

// AssertCheckPass fails the active test immediately if res did not pass
// cleanly.
//
// ### For Architects, Operators & Non-Programmers:
// Verifies a module's Block 4 self-audit receipt returned `[PASS:OK]`.
//
// ### For Systems Programmers & Engine Architects:
// Calls `t.Helper()` so failure output points to the caller line directly.
func AssertCheckPass(t *testing.T, res substrate.CheckResult, label string) {
	t.Helper()
	if !res.IsPass() {
		t.Fatalf("%s expected PASS:OK, got %s", label, res.FormatLine())
	}
}

// AssertCheckFail fails the active test immediately if res unexpectedly passed.
//
// ### For Architects, Operators & Non-Programmers:
// Confirms fault-injection checks truly failed instead of passing silently.
//
// ### For Systems Programmers & Engine Architects:
// Enforces symmetric negative testing across Section 2 Never-Fail gates.
func AssertCheckFail(t *testing.T, res substrate.CheckResult, label string) {
	t.Helper()
	if res.IsPass() {
		t.Fatalf("%s expected FAIL, unexpectedly passed: %s", label, res.FormatLine())
	}
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.2: Isolated Sandbox Fixture Writers
// -----------------------------------------------------------------------------

// WriteFixtureFile creates parent directories and writes body atomically inside
// t.
//
// ### For Architects, Operators & Non-Programmers:
// Helper used by unit tests to create temporary config files safely.
//
// ### For Systems Programmers & Engine Architects:
// Creates directories with `0o755` and files with `0o644` inside test tempdirs.
func WriteFixtureFile(t *testing.T, path, content string) {
	t.Helper()
	if err := os.MkdirAll(filepath.Dir(path), 0o755); err != nil {
		t.Fatalf("mkdir %s: %v", path, err)
	}
	if err := os.WriteFile(path, []byte(content), 0o644); err != nil {
		t.Fatalf("write %s: %v", path, err)
	}
}

// WriteFakeTranscript generates a timestamped transcript.jsonl inside brainDir.
//
// ### For Architects, Operators & Non-Programmers:
// Builds fake conversation logs with controlled modification times for testing
// `cpi-si tail` without touching live user sessions.
//
// ### For Systems Programmers & Engine Architects:
// Writes newline-delimited JSONL records and applies `os.Chtimes` cleanly.
func WriteFakeTranscript(t *testing.T, brainDir, sessionID string, lines []string, mod time.Time) string {
	t.Helper()
	p := filepath.Join(brainDir, sessionID, ".system_generated", "logs", "transcript.jsonl")
	WriteFixtureFile(t, p, strings.Join(lines, "\n")+"\n")
	if err := os.Chtimes(p, mod, mod); err != nil {
		t.Fatalf("chtimes %s: %v", p, err)
	}
	return p
}

// ValidMarkdownDoc returns a minimal Rev-5 5-Block Markdown fixture string.
//
// ### For Architects, Operators & Non-Programmers:
// Supplies a valid baseline document satisfying Block 1..5, dates, width, and
// 20+ Block 5 lines for substrate auditor tests.
//
// ### For Systems Programmers & Engine Architects:
// Pure deterministic template generator using `2026-11-20` timestamps and
// strictly `<= 80`-col HTML banners.
func ValidMarkdownDoc(name string) string {
	return `<!--
================================================================================
BLOCK 1: METADATA BLOCK
================================================================================
@file ` + name + `
@created 2026-11-20
@updated 2026-11-20
================================================================================
-->

<!-- BLOCK 2: OPENING BLOCK -->

<!-- BLOCK 3: BODY BLOCK -->

<!-- BLOCK 4: AUDIT PROCEDURES BLOCK -->

<!-- BLOCK 5: CLOSING BLOCK -->
1. line one
2. line two
3. line three
4. line four
5. line five
6. line six
7. line seven
8. line eight
9. line nine
10. line ten
11. line eleven
12. line twelve
13. line thirteen
14. line fourteen
15. line fifteen
16. line sixteen
17. line seventeen
18. line eighteen
19. line nineteen
<!-- END OF BLOCK 5: CLOSING SEAL -->
`
}

// BuildMinimalCleanSubstrate constructs a valid temporary `.agents/` root tree.
//
// ### For Architects, Operators & Non-Programmers:
// Sets up a minimal clean BIOS folder in a temporary sandbox containing host,
// operator, Silas identity, plans/reports registries, scratchpads, and 1 skill.
//
// ### For Systems Programmers & Engine Architects:
// Shared starting point for `boot`, `bench`, `audit`, and `cmd` binary suites.
func BuildMinimalCleanSubstrate(t *testing.T) string {
	t.Helper()
	agents := t.TempDir()
	WriteFixtureFile(t, filepath.Join(agents, "README.md"), ValidMarkdownDoc("README.md"))
	WriteFixtureFile(t, filepath.Join(agents, "host", "hardware_calibration.json"), `{
  "block_1_metadata": {}, "block_2_opening": {
    "active_platform": "linux",
    "non_headless_guards": {"desktop_ram_reserve_mb": 3072, "vram_display_floor_mb": 350, "usable_compute_vram_mb": 1387},
    "active_host_summary": {"hostname": "test-box", "os": "Ubuntu 26.04.1 LTS", "ram_mb": 15160}
  }, "block_3_body": {}, "block_4_audit": {}, "block_5_closing": {}
}`)
	WriteFixtureFile(t, filepath.Join(agents, "identity", "profiles", "operator", "operator_database.json"), `{
  "block_1_metadata": {}, "block_2_opening": {"active_operator_id": "seanje_lenox_wise", "registered_operator_count": 1},
  "block_3_body": {}, "block_4_audit": {}, "block_5_closing": {}
}`)
	WriteFixtureFile(t, filepath.Join(agents, "identity", "profiles", "silas_oren", "profile.json"), `{
  "block_1_metadata": {}, "block_2_opening": {"identity": {"name": "Silas Oren", "name_status": "sealed", "family_role": "Joiner", "version": "0.1.3-dev"}},
  "block_3_body": {}, "block_4_audit": {}, "block_5_closing": {}
}`)
	WriteFixtureFile(t, filepath.Join(agents, "plans", "plans_registry.json"), `{
  "block_1_metadata": {}, "block_2_opening": {},
  "block_3_body": {"plan_documents": [{"path": "plans_registry.json"}]},
  "block_4_audit": {}, "block_5_closing": {}
}`)
	WriteFixtureFile(t, filepath.Join(agents, "reports", "reports_registry.json"), `{
  "block_1_metadata": {}, "block_2_opening": {},
  "block_3_body": {"report_documents": [{"path": "reports_registry.json"}]},
  "block_4_audit": {}, "block_5_closing": {}
}`)
	WriteFixtureFile(t, filepath.Join(agents, "scratchpads", "active", ".gitkeep"), "")
	WriteFixtureFile(t, filepath.Join(agents, "scratchpads", "report_outs", ".gitkeep"), "")
	WriteFixtureFile(t, filepath.Join(agents, "scratchpads", "bench_guide.md"), ValidMarkdownDoc("bench_guide.md"))
	WriteFixtureFile(t, filepath.Join(agents, "scratchpads", "active", "quick.md"), "# Ephemeral note\n")
	WriteFixtureFile(
		t,
		filepath.Join(agents, "skills", "cpi-si-covenant", "SKILL.md"),
		"---\nname: cpi-si-covenant\ndescription: Cross-cutting pacing compass.\n---\n"+ValidMarkdownDoc("SKILL.md"),
	)
	return agents
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// =============================================================================
// Pure test support helpers exercised by every suite inside tests/unit/.

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================
// End of tests/common/fixtures.go
