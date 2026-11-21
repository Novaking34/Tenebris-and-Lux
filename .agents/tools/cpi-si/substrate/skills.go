// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundaries )
// =============================================================================
// @project       OmniCode Framework / Big House Ecosystem
// @file          substrate/skills.go
// @id            cog.agent.tools.cpi_si_cli.skills
// @layer         Layer 2: Operational Tooling (YAML Frontmatter & Skill Roster)
// @archetype     skill_discovery | yaml_frontmatter_parser
// @version       0.1.3-dev
// @interface_rev Rev 5
// @created       2026-11-20
// @updated       2026-11-20
// @author        CreativeWorkzStudio LLC / Seanje Lenox-Wise & Silas Oren
// @repository    https://github.com/Novaking34/OmniCode_Framework
// @system        CPI-SI Portable Cognitive BIOS CLI & Substrate Tooling
// @role          Parse Line-1 YAML Frontmatter & Group Cross-Cutting vs Domain
// @maturity      Rev 5 Operational Baseline | Wave A Tools Alignment
// @chassis       zero-external-deps | Pure stdlib string/line YAML parser
// @scale         O(N_skills) header scan across .agents/skills/*/SKILL.md
// @seams         in: substrate/types, substrate/paths, substrate/tail
// @invariants    1. Universal 5-Block Lifecycle Protocol (Rev 5).
//                2. Enforces Line-1 `---` YAML start for native Antigravity
//                load.
//                3. Distinguishes opt-in domain compasses from cross-cutting
//                craft.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Reads `.agents/skills/*/SKILL.md` frontmatter natively in zero-dependency
//   Go, verifying Antigravity discovery contracts (`name`, `description`) and
//   clearly separating cross-cutting pacing skills from opt-in domain manuals.
//
//   ### For Architects, Operators & Non-Programmers:
//   Makes sure domain manuals (like game engine or networking guides) stay opt-
//   in
//   for when we actually step onto those floors, while cross-cutting skills
//   stay
//   clearly highlighted across every turn.
//
//   ### For Systems Programmers & Engine Architects:
//   Implements bounded single-pass Line-1 YAML parser supporting inline quoted
//   strings and folded multiline scalars (`>-`, `>`, `|-`, `|`) with zero
//   third-
//   party YAML packages.
// =============================================================================

package substrate

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Module Anchor, Scope Enum & Skill Entry Contracts )
// =============================================================================
// Engine:        CPI-SI Portable Cognitive BIOS Tooling (cpi-si-cli)
// Stratum:       Layer 2: Operational Skill Roster Discovery
// Module:        cog.agent.tools.cpi_si_cli.skills
// Mission:       Parse Line-1 YAML frontmatter and classify skill scopes
// cleanly.
// =============================================================================

// --- Tier 1: Standard Go Library Packages ---
import (
	"fmt"
	"io"
	"os"
	"path/filepath"
	"sort"
	"strings"
)

// --- Tier 2: External Library Dependencies (None - 100% Go Stdlib) ---

// --- Tier 3: Internal Substrate Foundation Packages (Same-package helpers) ---

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Logical Anchor & Scope Constants
// -----------------------------------------------------------------------------

const (
	// ModuleIDSkills is the logical identifier for substrate/skills.
	ModuleIDSkills = "cog.agent.tools.cpi_si_cli.skills"

	// ScopeCrossCutting marks general pacing/craft skills active studio-wide.
	ScopeCrossCutting = "cross-cutting"

	// ScopeDomain marks opt-in floor manuals (e.g. FaithNet, Cornerstone).
	ScopeDomain = "domain"
)

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Public Skill Entry Ontology
// -----------------------------------------------------------------------------

// SkillEntry captures parsed discovery metadata for one `SKILL.md` file.
//
// ### For Architects, Operators & Non-Programmers:
// Holds a skill's canonical name, description, folder, and whether it is a
// cross-cutting studio skill or an opt-in domain compass.
//
// ### For Systems Programmers & Engine Architects:
// Populated deterministically from Line-1 YAML frontmatter plus Block 1
// `@archetype` / `@layer` scope classification.
type SkillEntry struct {
	DirName     string
	Name        string
	Description string
	Scope       string
	HasYAML     bool
	FilePath    string
}

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Frontmatter Parser, Scope Classifier & Grouped Roster Printer )
// =============================================================================
// Engine:        CPI-SI Portable Cognitive BIOS Tooling (cpi-si-cli)
// Stratum:       Layer 2: Operational Skill Roster Discovery
// Module:        cog.agent.tools.cpi_si_cli.skills
// Mission:       Discover and print .agents/skills/ without Python helpers.
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: Line-1 YAML Frontmatter Parser & Classifier
// -----------------------------------------------------------------------------

// ParseSkillFrontmatter validates Line-1 YAML delimiters and extracts
// name+desc.
//
// ### For Architects, Operators & Non-Programmers:
// Verifies a skill starts properly on line 1 with `---`, names itself cleanly,
// and provides a non-empty description for automatic agent discovery.
//
// ### For Systems Programmers & Engine Architects:
// Supports single-line scalar strings and folded multiline YAML blocks (`>-`,
// `>`, `|-`, `|`), returning `(name, desc, ok)`.
func ParseSkillFrontmatter(text string) (name, desc string, ok bool) {
	lines := strings.Split(strings.ReplaceAll(text, "\r\n", "\n"), "\n")
	if len(lines) < 3 || strings.TrimSpace(lines[0]) != "---" {
		return "", "", false
	}

	endIdx := -1
	for i := 1; i < len(lines); i++ {
		if strings.TrimSpace(lines[i]) == "---" {
			endIdx = i
			break
		}
	}
	if endIdx == -1 {
		return "", "", false
	}

	fm := lines[1:endIdx]
	for i := 0; i < len(fm); i++ {
		ln := fm[i]
		trimmed := strings.TrimSpace(ln)
		if strings.HasPrefix(trimmed, "name:") {
			name = cleanYAMLScalar(strings.TrimPrefix(trimmed, "name:"))
			continue
		}
		if strings.HasPrefix(trimmed, "description:") {
			rest := strings.TrimSpace(strings.TrimPrefix(trimmed, "description:"))
			if rest == ">-" || rest == ">" || rest == "|-" || rest == "|" {
				var parts []string
				for j := i + 1; j < len(fm); j++ {
					nextLn := fm[j]
					if len(nextLn) > 0 && (nextLn[0] == ' ' || nextLn[0] == '\t') {
						parts = append(parts, strings.TrimSpace(nextLn))
						i = j
					} else if strings.TrimSpace(nextLn) == "" {
						i = j
					} else {
						break
					}
				}
				desc = strings.Join(parts, " ")
			} else {
				desc = cleanYAMLScalar(rest)
			}
		}
	}
	return name, desc, name != "" && desc != ""
}

func cleanYAMLScalar(s string) string {
	s = strings.TrimSpace(s)
	if len(s) >= 2 && ((s[0] == '"' && s[len(s)-1] == '"') || (s[0] == '\'' && s[len(s)-1] == '\'')) {
		return s[1 : len(s)-1]
	}
	return s
}

func classifySkillScope(text, name, dirName string) string {
	lower := strings.ToLower(text)
	for _, ln := range strings.Split(lower, "\n") {
		if strings.Contains(ln, "@archetype") || strings.Contains(ln, "@layer") {
			if strings.Contains(ln, "domain_compass") || strings.Contains(ln, "domain compass") {
				return ScopeDomain
			}
		}
	}
	if dirName == "faithnet-workflows" || dirName == "cornerstone-workflows" ||
		name == "faithnet-workflows" || name == "cornerstone-workflows" {
		return ScopeDomain
	}
	return ScopeCrossCutting
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.2: Directory Discovery & Roster Renderer
// -----------------------------------------------------------------------------

// DiscoverSkills scans `<agentsDir>/skills/*/SKILL.md` in alphabetical order.
//
// ### For Architects, Operators & Non-Programmers:
// Lists every installed studio skill along with its YAML health and scope.
//
// ### For Systems Programmers & Engine Architects:
// Missing `skills/` directory returns `(nil, nil)` cleanly; sorts cross-cutting
// ahead of opt-in domain skills and alphabetically within each group.
func DiscoverSkills(agentsDir string) ([]SkillEntry, error) {
	skillsRoot := filepath.Join(agentsDir, "skills")
	entries, err := os.ReadDir(skillsRoot)
	if err != nil {
		if os.IsNotExist(err) {
			return nil, nil
		}
		return nil, fmt.Errorf("read skills dir: %w", err)
	}

	var skills []SkillEntry
	for _, entry := range entries {
		if !entry.IsDir() {
			continue
		}
		skillPath := filepath.Join(skillsRoot, entry.Name(), "SKILL.md")
		if !FileExists(skillPath) {
			continue
		}
		raw, err := os.ReadFile(skillPath)
		if err != nil {
			return nil, fmt.Errorf("read %s: %w", skillPath, err)
		}
		text := string(raw)
		name, desc, hasYAML := ParseSkillFrontmatter(text)
		scope := classifySkillScope(text, name, entry.Name())
		skills = append(skills, SkillEntry{
			DirName:     entry.Name(),
			Name:        name,
			Description: desc,
			Scope:       scope,
			HasYAML:     hasYAML,
			FilePath:    skillPath,
		})
	}

	sort.Slice(skills, func(i, j int) bool {
		if skills[i].Scope != skills[j].Scope {
			return skills[i].Scope < skills[j].Scope
		}
		return skills[i].DirName < skills[j].DirName
	})
	return skills, nil
}

// RunSkills prints the discovered skill roster grouped cleanly by scope to w.
//
// ### For Architects, Operators & Non-Programmers:
// Shows at one glance which skills carry across all studio tracks versus which
// manuals wait opt-in for specialized hardware or game-engine floors.
//
// ### For Systems Programmers & Engine Architects:
// Read-only stream renderer bounding descriptions safely via `TruncateRunes`.
func RunSkills(agentsDir string, w io.Writer) error {
	skills, err := DiscoverSkills(agentsDir)
	if err != nil {
		return err
	}

	var core, domain []SkillEntry
	for _, s := range skills {
		if s.Scope == ScopeDomain {
			domain = append(domain, s)
		} else {
			core = append(core, s)
		}
	}

	if _, err := fmt.Fprintf(w, "=== CPI-SI SKILL ROSTER (total=%d: cross-cutting=%d, opt-in domain=%d) ===\n", len(skills), len(core), len(domain)); err != nil {
		return err
	}
	if _, err := fmt.Fprintln(w, "\n[Core & Cross-Cutting Craft]"); err != nil {
		return err
	}
	for _, s := range core {
		_, _ = fmt.Fprintf(w, "  - %-34s yaml=%v | %s\n", s.Name, s.HasYAML, TruncateRunes(s.Description, 90))
	}
	if _, err := fmt.Fprintln(w, "\n[Opt-In Domain Compasses]"); err != nil {
		return err
	}
	for _, s := range domain {
		_, _ = fmt.Fprintf(w, "  - %-34s yaml=%v | %s\n", s.Name, s.HasYAML, TruncateRunes(s.Description, 90))
	}
	return nil
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// ( Contract Verification Cogs & Pre-Flight Validation Tripwires )
// =============================================================================
// Engine:        CPI-SI Portable Cognitive BIOS Tooling (cpi-si-cli)
// Stratum:       Layer 2: Operational Skill Roster Discovery
// Module:        cog.agent.tools.cpi_si_cli.skills
// Mission:       4-section self-audit proving YAML parsing & scope grouping.
// =============================================================================

// PreflightSkills executes a 4-section pre-flight self-audit for
// substrate/skills.
//
// @triggers tests/unit/substrate/skills_test.go
//
// ### For Architects, Operators & Non-Programmers:
// Verifies Line-1 YAML detection, rejects malformed headers missing `---`,
// parses folded multiline descriptions, and separates domain skills cleanly.
//
// ### For Systems Programmers & Engine Architects:
// Isolated tempdir fixture check with zero live filesystem edits.
func PreflightSkills() CheckResult {
	const check = "skills_preflight_4section"

	// Micro Check 1 (Always Pass): Folded >- YAML parses cleanly from line 1.
	name, desc, ok := ParseSkillFrontmatter("---\nname: alpha\ndescription: >-\n  line one\n  line two\n---\n")
	if !ok || name != "alpha" || desc != "line one line two" {
		return CheckFail(SeverityCritical, ModuleIDSkills, check, fmt.Sprintf("Sec 1 YAML parse mismatch: (%q,%q,%v)", name, desc, ok))
	}

	// Micro Check 2 (Never Fail): Missing Line-1 delimiter or unclosed YAML
	// rejected.
	if _, _, okBad := ParseSkillFrontmatter("# Not Line 1\n---\nname: x\ndescription: y\n---\n"); okBad {
		return CheckFail(SeverityCritical, ModuleIDSkills, check, "Sec 2 non-Line-1 YAML bypassed parser")
	}

	// Micro Check 3 (Invariants & Limits): Domain compass tag classifies
	// ScopeDomain.
	tmp, err := os.MkdirTemp("", "cpisi-skills-preflight-*")
	if err != nil {
		return CheckFail(SeverityFatal, ModuleIDSkills, check, err.Error())
	}
	defer func() { _ = os.RemoveAll(tmp) }()
	_ = os.MkdirAll(filepath.Join(tmp, "skills", "d1"), 0o755)
	_ = os.WriteFile(filepath.Join(tmp, "skills", "d1", "SKILL.md"), []byte("---\nname: d1\ndescription: domain\n---\n<!--\n@archetype domain_compass\n-->\n"), 0o644)

	// Micro Check 4 (Emergence): DiscoverSkills + RunSkills render domain group.
	found, err := DiscoverSkills(tmp)
	if err != nil || len(found) != 1 || found[0].Scope != ScopeDomain {
		return CheckFail(SeverityCritical, ModuleIDSkills, check, fmt.Sprintf("Sec 3/4 DiscoverSkills scope failed: %+v err=%v", found, err))
	}
	var out strings.Builder
	if err := RunSkills(tmp, &out); err != nil || !strings.Contains(out.String(), "opt-in domain=1") {
		return CheckFail(SeverityCritical, ModuleIDSkills, check, "Sec 4 RunSkills grouping mismatch")
	}

	return CheckPass(ModuleIDSkills, check, "Skill YAML parser & scope grouping passed all 4 micro gates.")
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( Epilogue Readiness Handshake & Seal )
// =============================================================================
// Engine:        CPI-SI Portable Cognitive BIOS Tooling (cpi-si-cli)
// Stratum:       Layer 2: Operational Skill Roster Discovery
// Module:        cog.agent.tools.cpi_si_cli.skills
// Mission:       Expose synchronous readiness predicate for substrate/skills.
// =============================================================================

// IsSkillsReady reports whether substrate/skills passes its 4-gate preflight.
//
// ### For Architects, Operators & Non-Programmers:
// Confirms skill discovery is ready across any `.agents/` mirror mount.
//
// ### For Systems Programmers & Engine Architects:
// Calls `PreflightSkills().IsPass()`.
func IsSkillsReady() bool {
	return PreflightSkills().IsPass()
}

// End of substrate/skills.go
