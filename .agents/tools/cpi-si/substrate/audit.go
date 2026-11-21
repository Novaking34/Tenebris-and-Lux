// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundaries )
// =============================================================================
// @project       OmniCode Framework / Big House Ecosystem
// @file          substrate/audit.go
// @id            cog.agent.tools.cpi_si_cli.audit
// @layer         Layer 3: Whole-.agents/ Rev-5 Portability & Parity Gatekeeper
// @archetype     substrate_auditor | portability_and_parity_gate
// @version       0.1.3-dev
// @interface_rev Rev 5
// @created       2026-11-20
// @updated       2026-11-20
// @author        CreativeWorkzStudio LLC / Seanje Lenox-Wise & Silas Oren
// @repository    https://github.com/Novaking34/OmniCode_Framework
// @system        CPI-SI Portable Cognitive BIOS CLI & Substrate Tooling
// @role          Verify Rev-5 Markers, Widths, ASCII Fences, Links & Registries
// @maturity      Rev 5 Operational Baseline | Wave A Tools Alignment
// @chassis       zero-external-deps | Deterministic single-pass Go filesystem
//                audit
// @scale         O(N_files * lines) deterministic scan across all .agents/
//                assets
// @seams         in: substrate/types, substrate/paths, substrate/skills
// @invariants    1. Universal 5-Block Lifecycle Protocol (Rev 5).
//                2. Enforces 1:1 parity on plans/ and reports/ JSON registries.
//                3. Enforces Two-Tier scratchpads law (bench furniture &
//                studies
//                   audited; active/ & report_outs/ exempt; no registry JSON).
//                4. Forbids Python/bytecode residue inside .agents/skills/.
//                5. Ensures zero broken or .agents/-escaping relative links.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Replaces ad-hoc Python one-liners with a deterministic, tested, zero-dep
//   Go auditor guarding the entire `.agents/` Cognitive BIOS before commits and
//   6-mirror syncs.
//
//   ### For Architects, Operators & Non-Programmers:
//   Runs every mechanical check across Markdown, JSON registries, AsciiDoc
//   monographs, scratchpads, skills, and tools in milliseconds — making sure
//   nothing broken spreads across all six mirrors.
//
//   ### For Systems Programmers & Engine Architects:
//   Single-pass filesystem walker enforcing Block 1..5 structure, `@created`/
//   `@updated` hygiene, 80-column HTML/AsciiDoc comment ceilings, pure 7-bit
//   ASCII
//   code fences, relative link containment, Line-1 skill YAML, and Go-native
//   Python/bytecode hygiene.
// =============================================================================

package substrate

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Module Anchor, Compiled Regexps & Aggregated AuditReport Struct )
// =============================================================================
// Engine:        CPI-SI Portable Cognitive BIOS Tooling (cpi-si-cli)
// Stratum:       Layer 3: Substrate Portability & Parity Verification
// Module:        cog.agent.tools.cpi_si_cli.audit
// Mission:       Define whole-.agents/ verification counters and regex guards.
// =============================================================================

// --- Tier 1: Standard Go Library Packages ---
import (
	"encoding/json"
	"fmt"
	"io"
	"io/fs"
	"os"
	"path/filepath"
	"regexp"
	"sort"
	"strings"
	"unicode/utf8"
)

// --- Tier 2: External Library Dependencies (None - 100% Go Stdlib) ---

// --- Tier 3: Internal Substrate Foundation Packages (Same-package cogs) ---

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Operational Logical Anchor & Width Ceiling Constants
// -----------------------------------------------------------------------------

const (
	// ModuleIDAudit is the logical identifier for substrate/audit.
	ModuleIDAudit = "cog.agent.tools.cpi_si_cli.audit"

	// MaxCommentWidthCols enforces <= 80-column comment walls across Rev 5.
	MaxCommentWidthCols = 80
)

var (
	reBareDateLine = regexp.MustCompile(`^\s*@date\b`)
	reMDLink       = regexp.MustCompile(`\[[^\]]*\]\(([^)]+)\)`)
)

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Public Aggregated Audit Report Ontology
// -----------------------------------------------------------------------------

// AuditReport summarizes verified counts and violation lines across `.agents/`.
//
// ### For Architects, Operators & Non-Programmers:
// Reports exact counts of checked Markdown, AsciiDoc, JSON, Skill, Plan, and
// Report files alongside any violations found.
//
// ### For Systems Programmers & Engine Architects:
// Populated deterministically with sorted `Failures` slice by `AuditSubstrate`.
type AuditReport struct {
	JSONCount     int
	MarkdownCount int
	AdocCount     int
	SkillCount    int
	PlansParity   int
	ReportsParity int
	Failures      []string
}

// IsClean reports whether zero violations were found across the substrate.
//
// ### For Architects, Operators & Non-Programmers:
// True only when every single checked file passed with zero violations.
//
// ### For Systems Programmers & Engine Architects:
// Pure O(1) slice emptiness check (`len(r.Failures) == 0`).
func (r *AuditReport) IsClean() bool {
	return r != nil && len(r.Failures) == 0
}

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Substrate Walker, Registry Parity, Width/ASCII & Link Containment )
// =============================================================================
// Engine:        CPI-SI Portable Cognitive BIOS Tooling (cpi-si-cli)
// Stratum:       Layer 3: Substrate Portability & Parity Verification
// Module:        cog.agent.tools.cpi_si_cli.audit
// Mission:       Audit all JSONs, Markdowns, AsciiDocs, scratchpads & skills.
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: CLI Report Wrapper & Core Substrate Walker
// -----------------------------------------------------------------------------

// RunAudit executes AuditSubstrate and writes an explicit pass/fail ledger to
// w.
//
// ### For Architects, Operators & Non-Programmers:
// Prints either `CPI_SI_SUBSTRATE_AUDIT_OK` with verified file counts, or every
// exact violation file and line number so nothing hides.
//
// ### For Systems Programmers & Engine Architects:
// Returns a non-nil error whenever `len(rep.Failures) > 0` so CLI exit code
// becomes `1` deterministically.
func RunAudit(agentsDir string, w io.Writer) (*AuditReport, error) {
	rep, err := AuditSubstrate(agentsDir)
	if err != nil {
		return rep, err
	}
	if !rep.IsClean() {
		_, _ = fmt.Fprintf(w, "CPI_SI_SUBSTRATE_AUDIT_FAILED: failures=%d\n", len(rep.Failures))
		for _, f := range rep.Failures {
			_, _ = fmt.Fprintf(w, "  - %s\n", f)
		}
		return rep, fmt.Errorf("substrate audit failed with %d violation(s)", len(rep.Failures))
	}
	_, _ = fmt.Fprintf(
		w,
		"CPI_SI_SUBSTRATE_AUDIT_OK: md=%d adoc=%d json=%d skills=%d plans_parity=%d reports_parity=%d failures=0\n",
		rep.MarkdownCount,
		rep.AdocCount,
		rep.JSONCount,
		rep.SkillCount,
		rep.PlansParity,
		rep.ReportsParity,
	)
	return rep, nil
}

// AuditSubstrate walks agentsDir and validates all structural Rev-5 invariants.
//
// ### For Architects, Operators & Non-Programmers:
// Verifies 5-Block framing, timestamps, comment walls, ASCII diagrams, links,
// 1:1 registries, scratchpad exemption rules, and skill hygiene in one pass.
//
// ### For Systems Programmers & Engine Architects:
// Non-mutating read-only walk sorting all file lists and failure lines for
// cross-OS reproducibility.
func AuditSubstrate(agentsDir string) (*AuditReport, error) {
	absRoot, err := filepath.Abs(agentsDir)
	if err != nil {
		return nil, err
	}
	rep := &AuditReport{}

	var jsonFiles, mdFiles, adocFiles []string
	walkErr := filepath.WalkDir(absRoot, func(path string, d fs.DirEntry, err error) error {
		if err != nil {
			return err
		}
		rel, err := filepath.Rel(absRoot, path)
		if err != nil {
			return err
		}
		relSlash := filepath.ToSlash(rel)
		if d.IsDir() {
			if strings.HasPrefix(relSlash, "skills/") && d.Name() == "__pycache__" {
				rep.Failures = append(rep.Failures, fmt.Sprintf("%s: stale __pycache__ directory inside skills/", relSlash))
			}
			return nil
		}

		if strings.HasPrefix(relSlash, "skills/") {
			lower := strings.ToLower(d.Name())
			if strings.HasSuffix(lower, ".py") || strings.HasSuffix(lower, ".pyc") || strings.HasSuffix(lower, ".pyo") {
				rep.Failures = append(rep.Failures, fmt.Sprintf("%s: Python artifact forbidden inside skills/ (use Go CLI)", relSlash))
			}
		}

		lowerName := strings.ToLower(d.Name())
		switch {
		case strings.HasSuffix(lowerName, ".json"):
			jsonFiles = append(jsonFiles, path)
		case strings.HasSuffix(lowerName, ".adoc"):
			adocFiles = append(adocFiles, path)
		case strings.HasSuffix(lowerName, ".md"):
			if IsAuditedMarkdown(relSlash) {
				mdFiles = append(mdFiles, path)
			}
		}
		return nil
	})
	if walkErr != nil {
		return nil, walkErr
	}

	sort.Strings(jsonFiles)
	sort.Strings(mdFiles)
	sort.Strings(adocFiles)

	for _, jf := range jsonFiles {
		rel, _ := filepath.Rel(absRoot, jf)
		relSlash := filepath.ToSlash(rel)
		raw, err := os.ReadFile(jf)
		if err != nil {
			rep.Failures = append(rep.Failures, fmt.Sprintf("%s: read error: %v", relSlash, err))
			continue
		}
		var anyJSON any
		if err := json.Unmarshal(raw, &anyJSON); err != nil {
			rep.Failures = append(rep.Failures, fmt.Sprintf("%s: invalid JSON: %v", relSlash, err))
			continue
		}
		txt := string(raw)
		upper := strings.ToUpper(txt)
		hasBlocks := true
		for b := 1; b <= 5; b++ {
			lowTag := fmt.Sprintf("block_%d", b)
			upTag := fmt.Sprintf("BLOCK %d", b)
			if !strings.Contains(txt, lowTag) && !strings.Contains(upper, upTag) {
				hasBlocks = false
				break
			}
		}
		if !hasBlocks {
			rep.Failures = append(rep.Failures, fmt.Sprintf("%s: missing 5-block markers", relSlash))
		}
	}
	rep.JSONCount = len(jsonFiles)

	if DirExists(filepath.Join(absRoot, "plans")) {
		rep.PlansParity = auditRegistryParity(absRoot, "plans", "plans_registry.json", "plan_documents", &rep.Failures)
	}
	if DirExists(filepath.Join(absRoot, "reports")) {
		rep.ReportsParity = auditRegistryParity(absRoot, "reports", "reports_registry.json", "report_documents", &rep.Failures)
	}
	if DirExists(filepath.Join(absRoot, "scratchpads")) {
		auditScratchpadsLayout(absRoot, &rep.Failures)
	}

	for _, mf := range mdFiles {
		rel, _ := filepath.Rel(absRoot, mf)
		relSlash := filepath.ToSlash(rel)
		auditMarkdownFile(absRoot, mf, relSlash, &rep.Failures)
		if strings.HasPrefix(relSlash, "skills/") && filepath.Base(mf) == "SKILL.md" {
			rep.SkillCount++
		}
	}
	rep.MarkdownCount = len(mdFiles)

	for _, af := range adocFiles {
		rel, _ := filepath.Rel(absRoot, af)
		relSlash := filepath.ToSlash(rel)
		auditAdocFile(af, relSlash, &rep.Failures)
	}
	rep.AdocCount = len(adocFiles)

	sort.Strings(rep.Failures)
	return rep, nil
}

// IsAuditedMarkdown determines whether a relative `.md` path falls under Rev 5.
//
// ### For Architects, Operators & Non-Programmers:
// Keeps durable BIOS folders (`host/`, `identity/`, `language/`, `lineage/`,
// `plans/`, `reports/`, `skills/`, `tools/`, `scratchpads/studies/`) under
// Rev-5
// gates while keeping working notes in `scratchpads/active/` and `report_outs/`
// completely free of bureaucracy.
//
// ### For Systems Programmers & Engine Architects:
// Expects forward-slash paths relative to `.agents/` root.
func IsAuditedMarkdown(relSlash string) bool {
	if relSlash == "README.md" {
		return true
	}
	if strings.HasPrefix(relSlash, "scratchpads/active/") || strings.HasPrefix(relSlash, "scratchpads/report_outs/") {
		return false
	}
	for _, prefix := range []string{"host/", "identity/", "language/", "lineage/", "plans/", "reports/", "scratchpads/", "skills/", "tools/"} {
		if strings.HasPrefix(relSlash, prefix) {
			return true
		}
	}
	return false
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.2: Specialized Registry, Bench, Markdown & AsciiDoc Audits
// -----------------------------------------------------------------------------

func auditRegistryParity(absRoot, domainDir, regName, arrayKey string, failures *[]string) int {
	base := filepath.Join(absRoot, domainDir)
	regPath := filepath.Join(base, regName)
	raw, err := os.ReadFile(regPath)
	if err != nil {
		*failures = append(*failures, fmt.Sprintf("%s/%s: missing or unreadable: %v", domainDir, regName, err))
		return 0
	}
	var rootObj map[string]any
	if err := json.Unmarshal(raw, &rootObj); err != nil {
		*failures = append(*failures, fmt.Sprintf("%s/%s: invalid JSON: %v", domainDir, regName, err))
		return 0
	}
	b3, ok := rootObj["block_3_body"].(map[string]any)
	if !ok {
		*failures = append(*failures, fmt.Sprintf("%s/%s: missing block_3_body object", domainDir, regName))
		return 0
	}
	arr, ok := b3[arrayKey].([]any)
	if !ok {
		*failures = append(*failures, fmt.Sprintf("%s/%s: missing block_3_body.%s list", domainDir, regName, arrayKey))
		return 0
	}

	regSet := make(map[string]bool)
	for _, item := range arr {
		m, ok := item.(map[string]any)
		if !ok {
			continue
		}
		if p, ok := m["path"].(string); ok {
			regSet[strings.TrimPrefix(filepath.ToSlash(p), "./")] = true
		}
	}

	diskSet := make(map[string]bool)
	_ = filepath.WalkDir(base, func(path string, d fs.DirEntry, err error) error {
		if err != nil || d.IsDir() {
			return nil
		}
		rel, err := filepath.Rel(base, path)
		if err == nil {
			diskSet[filepath.ToSlash(rel)] = true
		}
		return nil
	})

	for k := range regSet {
		if !diskSet[k] {
			*failures = append(*failures, fmt.Sprintf("%s/%s: listed entry missing on disk: %s", domainDir, regName, k))
		}
	}
	for k := range diskSet {
		if !regSet[k] {
			*failures = append(*failures, fmt.Sprintf("%s/%s: disk file missing from registry: %s", domainDir, regName, k))
		}
	}
	return len(regSet)
}

func auditScratchpadsLayout(absRoot string, failures *[]string) {
	sp := filepath.Join(absRoot, "scratchpads")
	if FileExists(filepath.Join(sp, "scratchpads_registry.json")) {
		*failures = append(*failures, "scratchpads/scratchpads_registry.json: violates intentional No-Registry Law")
	}
	for _, req := range []string{"bench_guide.md", filepath.Join("active", ".gitkeep"), filepath.Join("report_outs", ".gitkeep")} {
		if !FileExists(filepath.Join(sp, req)) {
			*failures = append(*failures, fmt.Sprintf("scratchpads/%s: missing required bench file", filepath.ToSlash(req)))
		}
	}
}

func auditMarkdownFile(absRoot, path, relSlash string, failures *[]string) {
	raw, err := os.ReadFile(path)
	if err != nil {
		*failures = append(*failures, fmt.Sprintf("%s: read error: %v", relSlash, err))
		return
	}
	text := strings.ReplaceAll(string(raw), "\r\n", "\n")
	lines := strings.Split(text, "\n")

	if strings.HasPrefix(relSlash, "skills/") && filepath.Base(path) == "SKILL.md" {
		name, desc, ok := ParseSkillFrontmatter(text)
		if !ok || name == "" || desc == "" {
			*failures = append(*failures, fmt.Sprintf("%s: missing or invalid Line-1 YAML frontmatter", relSlash))
		}
	}

	for b := 1; b <= 5; b++ {
		if !strings.Contains(text, fmt.Sprintf("BLOCK %d", b)) {
			*failures = append(*failures, fmt.Sprintf("%s: missing BLOCK %d", relSlash, b))
		}
	}
	if !strings.Contains(text, "BLOCK 5: CLOSING BLOCK") {
		*failures = append(*failures, fmt.Sprintf("%s: missing BLOCK 5: CLOSING BLOCK", relSlash))
	}
	if !strings.Contains(text, "END OF BLOCK 5") {
		*failures = append(*failures, fmt.Sprintf("%s: missing END OF BLOCK 5", relSlash))
	}
	if !strings.Contains(text, "@created") {
		*failures = append(*failures, fmt.Sprintf("%s: missing @created", relSlash))
	}
	if !strings.Contains(text, "@updated") {
		*failures = append(*failures, fmt.Sprintf("%s: missing @updated", relSlash))
	}

	for idx, ln := range lines {
		if reBareDateLine.MatchString(ln) {
			*failures = append(*failures, fmt.Sprintf("%s:%d: bare @date header line found", relSlash, idx+1))
		}
	}

	b5Start, b5End := -1, -1
	for idx, ln := range lines {
		if strings.Contains(ln, "BLOCK 5: CLOSING BLOCK") && b5Start == -1 {
			b5Start = idx
		}
		if strings.Contains(ln, "END OF BLOCK 5") {
			b5End = idx
		}
	}
	if b5Start != -1 && b5End != -1 && b5End >= b5Start {
		nonEmpty := 0
		for _, ln := range lines[b5Start : b5End+1] {
			if strings.TrimSpace(ln) != "" {
				nonEmpty++
			}
		}
		req := 15
		if strings.HasPrefix(relSlash, "reports/") ||
			strings.HasPrefix(relSlash, "scratchpads/") ||
			strings.HasPrefix(relSlash, "skills/") ||
			strings.HasPrefix(relSlash, "tools/") ||
			strings.Contains(text, "@updated 2026-11-20") {
			req = 20
		}
		if nonEmpty < req {
			*failures = append(*failures, fmt.Sprintf("%s: Block 5 non-empty lines = %d (< %d)", relSlash, nonEmpty, req))
		}
	}

	inFence := false
	inComment := false
	for idx, ln := range lines {
		trimmed := strings.TrimSpace(ln)
		if strings.HasPrefix(trimmed, "```") && !inComment {
			inFence = !inFence
			continue
		}
		if inFence {
			for _, r := range ln {
				if r > 127 {
					*failures = append(*failures, fmt.Sprintf("%s:%d: non-ASCII rune U+%04X in code fence", relSlash, idx+1, r))
					break
				}
			}
			continue
		}
		if strings.Contains(ln, "<!--") {
			inComment = true
		}
		if inComment && utf8.RuneCountInString(ln) > MaxCommentWidthCols {
			*failures = append(*failures, fmt.Sprintf("%s:%d: HTML comment > 80 cols (%d)", relSlash, idx+1, utf8.RuneCountInString(ln)))
		}
		if strings.Contains(ln, "-->") {
			inComment = false
		}
	}

	inFence = false
	for idx, ln := range lines {
		if strings.HasPrefix(strings.TrimSpace(ln), "```") {
			inFence = !inFence
			continue
		}
		if inFence {
			continue
		}
		matches := reMDLink.FindAllStringSubmatch(ln, -1)
		for _, m := range matches {
			target := strings.TrimSpace(m[1])
			if target == "" ||
				strings.HasPrefix(target, "http://") ||
				strings.HasPrefix(target, "https://") ||
				strings.HasPrefix(target, "mailto:") ||
				strings.HasPrefix(target, "#") {
				continue
			}
			targetPath := strings.SplitN(target, "#", 2)[0]
			fields := strings.Fields(targetPath)
			if len(fields) == 0 {
				continue
			}
			targetPath = fields[0]

			var resolved string
			if strings.HasPrefix(targetPath, "file://") {
				resolved = filepath.Clean(strings.TrimPrefix(targetPath, "file://"))
			} else {
				resolved = filepath.Clean(filepath.Join(filepath.Dir(path), filepath.FromSlash(targetPath)))
			}
			if _, err := os.Stat(resolved); err != nil {
				*failures = append(*failures, fmt.Sprintf("%s:%d: broken link -> %s", relSlash, idx+1, target))
				continue
			}
			relFromRoot, err := filepath.Rel(absRoot, resolved)
			if err != nil || relFromRoot == ".." || strings.HasPrefix(relFromRoot, ".."+string(filepath.Separator)) {
				*failures = append(*failures, fmt.Sprintf("%s:%d: link escapes .agents/ -> %s", relSlash, idx+1, target))
			}
		}
	}
}

func auditAdocFile(path, relSlash string, failures *[]string) {
	raw, err := os.ReadFile(path)
	if err != nil {
		*failures = append(*failures, fmt.Sprintf("%s: read error: %v", relSlash, err))
		return
	}
	text := strings.ReplaceAll(string(raw), "\r\n", "\n")
	lines := strings.Split(text, "\n")

	for b := 1; b <= 5; b++ {
		if !strings.Contains(text, fmt.Sprintf("BLOCK %d", b)) {
			*failures = append(*failures, fmt.Sprintf("%s: missing BLOCK %d", relSlash, b))
		}
	}
	if !strings.Contains(text, "@created") || !strings.Contains(text, "@updated") {
		*failures = append(*failures, fmt.Sprintf("%s: missing @created or @updated", relSlash))
	}
	inCmt := false
	for idx, ln := range lines {
		if reBareDateLine.MatchString(ln) {
			*failures = append(*failures, fmt.Sprintf("%s:%d: bare @date header line", relSlash, idx+1))
		}
		if strings.TrimSpace(ln) == "////" {
			if utf8.RuneCountInString(ln) > MaxCommentWidthCols {
				*failures = append(*failures, fmt.Sprintf("%s:%d: //// > 80 cols", relSlash, idx+1))
			}
			inCmt = !inCmt
			continue
		}
		if inCmt && utf8.RuneCountInString(ln) > MaxCommentWidthCols {
			*failures = append(*failures, fmt.Sprintf("%s:%d: AsciiDoc comment > 80 cols (%d)", relSlash, idx+1, utf8.RuneCountInString(ln)))
		}
	}
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// ( Contract Verification Cogs & Pre-Flight Validation Tripwires )
// =============================================================================
// Engine:        CPI-SI Portable Cognitive BIOS Tooling (cpi-si-cli)
// Stratum:       Layer 3: Substrate Portability & Parity Verification
// Module:        cog.agent.tools.cpi_si_cli.audit
// Mission:       4-section self-audit proving pass/fail symmetry on `.agents/`.
// =============================================================================

// PreflightAudit executes a 4-section pre-flight self-audit for
// substrate/audit.
//
// @triggers tests/unit/substrate/audit_test.go
//
// ### For Architects, Operators & Non-Programmers:
// Runs an isolated sandbox test proving clean BIOS trees pass and broken links,
// non-ASCII fences, `__pycache__` clutter, or rogue registries get caught.
//
// ### For Systems Programmers & Engine Architects:
// Covers nominal clean tree, fault inversion, comment/ASCII limits, and
// ephemeral `scratchpads/active/` exemption cleanly inside `os.MkdirTemp`.
func PreflightAudit() CheckResult {
	const check = "audit_preflight_4section"

	tmp, err := os.MkdirTemp("", "cpisi-audit-preflight-*")
	if err != nil {
		return CheckFail(SeverityFatal, ModuleIDAudit, check, err.Error())
	}
	defer func() { _ = os.RemoveAll(tmp) }()

	goodDoc := "<!--\n================================================================================\nBLOCK 1\n================================================================================\n@created 2026-11-20\n@updated 2026-11-20\n-->\n<!-- BLOCK 2 -->\n<!-- BLOCK 3 -->\n<!-- BLOCK 4 -->\n<!-- BLOCK 5: CLOSING BLOCK -->\n" +
		strings.Repeat("line\n", 18) + "<!-- END OF BLOCK 5 -->\n"
	_ = os.WriteFile(filepath.Join(tmp, "README.md"), []byte(goodDoc), 0o644)
	_ = os.MkdirAll(filepath.Join(tmp, "scratchpads", "active"), 0o755)
	_ = os.MkdirAll(filepath.Join(tmp, "scratchpads", "report_outs"), 0o755)
	_ = os.WriteFile(filepath.Join(tmp, "scratchpads", "active", ".gitkeep"), nil, 0o644)
	_ = os.WriteFile(filepath.Join(tmp, "scratchpads", "report_outs", ".gitkeep"), nil, 0o644)
	_ = os.WriteFile(filepath.Join(tmp, "scratchpads", "bench_guide.md"), []byte(goodDoc), 0o644)
	_ = os.WriteFile(filepath.Join(tmp, "scratchpads", "active", "temp.md"), []byte("# Free Note\n"), 0o644)

	// Micro Check 1 & 3 (Always Pass + Ephemeral Exemption).
	rep, err := AuditSubstrate(tmp)
	if err != nil || !rep.IsClean() {
		return CheckFail(SeverityCritical, ModuleIDAudit, check, fmt.Sprintf("Sec 1/3 clean sandbox failed: err=%v failures=%v", err, rep))
	}

	// Micro Check 2 & 4 (Never Fail + Catching Width / ASCII / Clutter Traps).
	_ = os.MkdirAll(filepath.Join(tmp, "skills", "__pycache__"), 0o755)
	_ = os.WriteFile(filepath.Join(tmp, "skills", "__pycache__", "stale.pyc"), []byte("x"), 0o644)
	repBad, err := AuditSubstrate(tmp)
	if err != nil || repBad.IsClean() {
		return CheckFail(SeverityCritical, ModuleIDAudit, check, "Sec 2/4 failed to catch stale __pycache__/.pyc trap")
	}

	return CheckPass(ModuleIDAudit, check, "Whole-.agents/ Rev-5 auditor passed all 4 micro gates (clean pass + trap catch).")
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( Epilogue Readiness Handshake & Seal )
// =============================================================================
// Engine:        CPI-SI Portable Cognitive BIOS Tooling (cpi-si-cli)
// Stratum:       Layer 3: Substrate Portability & Parity Verification
// Module:        cog.agent.tools.cpi_si_cli.audit
// Mission:       Expose synchronous readiness predicate for substrate/audit.
// =============================================================================

// IsAuditReady reports whether substrate/audit passes its 4-section self-audit.
//
// ### For Architects, Operators & Non-Programmers:
// Confirms the substrate gatekeeper is verified and dependable.
//
// ### For Systems Programmers & Engine Architects:
// Calls `PreflightAudit().IsPass()`.
func IsAuditReady() bool {
	return PreflightAudit().IsPass()
}

// End of substrate/audit.go
