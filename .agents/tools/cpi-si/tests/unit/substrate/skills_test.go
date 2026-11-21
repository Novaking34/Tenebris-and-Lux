// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundaries )
// =============================================================================
// @project       OmniCode Framework / Big House Ecosystem
// @file          tests/unit/substrate/skills_test.go
// @id            test.unit.substrate.skills
// @layer         Layer 4: Tier-2 Unit Verification Suite (Skill Discovery)
// @archetype     verification_suite | unit_isolation
// @version       0.1.3-dev
// @interface_rev Rev 5
// @created       2026-11-20
// @updated       2026-11-20
// @author        CreativeWorkzStudio LLC / Seanje Lenox-Wise & Silas Oren
// @repository    https://github.com/Novaking34/OmniCode_Framework
// @system        CPI-SI Portable Cognitive BIOS CLI & Substrate Tooling
// @role          Strict 1:1 Tier-2 4-Section Unit Suite for substrate/skills.go
// @maturity      Rev 5 Operational Baseline | Wave A Tools Alignment
// @chassis       zero-external-deps | Line-1 YAML & scope grouping tests
// @scale         O(N_skills) skill directory scan
// @seams         in: substrate, tests/common | target: substrate/skills.go
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

const skillsUnitSuiteName = "Tier 2 Unit: cog.agent.tools.cpi_si_cli.skills"

// =============================================================================
// BLOCK 3: BODY BLOCK / FOUR-SECTION ONTOLOGICAL TEST SUITE
// =============================================================================

// --- Section 1: Always Pass (Inline & Folded >- YAML Discovery) ---

func TestSkillsUnit_Sec1_InlineAndFoldedYAMLParsing(t *testing.T) {
	agents := t.TempDir()
	common.WriteFixtureFile(
		t,
		filepath.Join(agents, "skills", "cpi-si-covenant", "SKILL.md"),
		"---\nname: cpi-si-covenant\ndescription: >-\n  Walk with me\n  across every substrate.\n---\n",
	)
	common.WriteFixtureFile(
		t,
		filepath.Join(agents, "skills", "cornerstone-workflows", "SKILL.md"),
		"---\nname: cornerstone-workflows\ndescription: \"Engine domain manual.\"\n---\n<!--\n@archetype domain_compass | game_engine\n-->\n",
	)

	found, err := substrate.DiscoverSkills(agents)
	if err != nil || len(found) != 2 {
		t.Fatalf("[SEC 1] expected 2 skills, got %d (err=%v)", len(found), err)
	}
	if !found[0].HasYAML || !found[1].HasYAML || found[0].Scope != substrate.ScopeCrossCutting || found[1].Scope != substrate.ScopeDomain {
		t.Fatalf("[SEC 1] unexpected parsed skill entries: %+v", found)
	}
}

// --- Section 2: Never Fail (Reject Shifted or Unclosed Frontmatter) ---

func TestSkillsUnit_Sec2_RejectsInvalidFrontmatterAndEmptySkills(t *testing.T) {
	badInputs := []string{
		"",
		"# Heading before YAML\n---\nname: bad\ndescription: bad\n---\n",
		"---\nname: unclosed\ndescription: missing fence\n",
		"---\nname: missing_desc\n---\n",
	}
	for idx, raw := range badInputs {
		if _, _, ok := substrate.ParseSkillFrontmatter(raw); ok {
			t.Fatalf("[SEC 2] case %d unexpectedly passed frontmatter parser", idx)
		}
	}
	if skills, err := substrate.DiscoverSkills(t.TempDir()); err != nil || len(skills) != 0 {
		t.Fatalf("[SEC 2] missing skills dir should return empty slice cleanly, got %+v (err=%v)", skills, err)
	}
}

// --- Section 3: Invariants & Limits (Scope Classification Rules) ---

func TestSkillsUnit_Sec3_DomainVsCrossCuttingScopeBoundaries(t *testing.T) {
	agents := t.TempDir()
	common.WriteFixtureFile(
		t,
		filepath.Join(agents, "skills", "custom-craft", "SKILL.md"),
		"---\nname: custom-craft\ndescription: Shared craft skill.\n---\n<!--\n@archetype operational_skill | cross_cutting\n-->\n",
	)
	common.WriteFixtureFile(
		t,
		filepath.Join(agents, "skills", "audio-domain", "SKILL.md"),
		"---\nname: audio-domain\ndescription: Audio opt-in manual.\n---\n<!--\n@archetype domain_compass | audio_floor\n-->\n",
	)
	skills, err := substrate.DiscoverSkills(agents)
	if err != nil || len(skills) != 2 {
		t.Fatalf("[SEC 3] expected 2 skills, got %d (err=%v)", len(skills), err)
	}
	if skills[0].Scope != substrate.ScopeCrossCutting || skills[1].Scope != substrate.ScopeDomain {
		t.Fatalf("[SEC 3] scope boundary failed: %+v", skills)
	}
}

// --- Section 4: Emergence (Grouped Roster Output, Preflight & Readiness) ---

func TestSkillsUnit_Sec4_RosterRendererPreflightAndReadiness(t *testing.T) {
	agents := common.BuildMinimalCleanSubstrate(t)
	var buf bytes.Buffer
	if err := substrate.RunSkills(agents, &buf); err != nil {
		t.Fatal(err)
	}
	if !strings.Contains(buf.String(), "cross-cutting=1, opt-in domain=0") {
		t.Fatalf("[SEC 4] unexpected roster output:\n%s", buf.String())
	}
	common.AssertCheckPass(t, substrate.PreflightSkills(), "PreflightSkills")
	if !substrate.IsSkillsReady() {
		t.Fatalf("[SEC 4] IsSkillsReady returned false")
	}
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// =============================================================================

func TestSkillsUnit_Block4_SuiteSummaryAudit(t *testing.T) {
	t.Logf("%s: Line-1 YAML parser & cross-cutting vs domain grouping verified.", skillsUnitSuiteName)
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================
// End of tests/unit/substrate/skills_test.go
