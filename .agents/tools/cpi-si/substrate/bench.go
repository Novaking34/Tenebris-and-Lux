// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundaries )
// =============================================================================
// @project       OmniCode Framework / Big House Ecosystem
// @file          substrate/bench.go
// @id            cog.agent.tools.cpi_si_cli.bench
// @layer         Layer 2: Operational Tooling (Running Writing Bench Triage)
// @archetype     bench_triage | disposability_check
// @version       0.1.3-dev
// @interface_rev Rev 5
// @created       2026-11-20
// @updated       2026-11-20
// @author        CreativeWorkzStudio LLC / Seanje Lenox-Wise & Silas Oren
// @repository    https://github.com/Novaking34/OmniCode_Framework
// @system        CPI-SI Portable Cognitive BIOS CLI & Substrate Tooling
// @role          Zero-Body-Read Spatial Triage for .agents/scratchpads/
// @maturity      Rev 5 Operational Baseline | Wave A Tools Alignment
// @chassis       zero-external-deps | Pure metadata stat pass (zero body reads)
// @scale         O(N_bench_files) shallow directory listing per bay
// @seams         in: substrate/types, substrate/paths, .agents/scratchpads/
// @invariants    1. Universal 5-Block Lifecycle Protocol (Rev 5).
//                2. Suppresses .gitkeep sentinels from working note counts.
//                3. Enforces Intentional No-Registry Law cleanly.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Gives Silas an instant spatial view of all four bays in
//   `.agents/scratchpads/`
//   (`active/`, `report_outs/`, `studies/`, `bench-templates/`) without wasting
//   tokens reading draft bodies prematurely.
//
//   ### For Architects, Operators & Non-Programmers:
//   Acts like glancing across your workshop table at boot: see at once what
//   active thinking notes or staged report-outs are sitting on the bench and
//   confirm no heavy parity registry drifted into a disposable workspace.
//
//   ### For Systems Programmers & Engine Architects:
//   Performs shallow `os.ReadDir` sweeps over the four bays, strips `.gitkeep`,
//   checks `scratchpads_registry.json` absence, and emits crisp byte counts.
// =============================================================================

package substrate

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Module Anchor, Data Contracts & O(1) Readiness Helpers )
// =============================================================================
// Engine:        CPI-SI Portable Cognitive BIOS Tooling (cpi-si-cli)
// Stratum:       Layer 2: Running Writing Bench Spatial Triage
// Module:        cog.agent.tools.cpi_si_cli.bench
// Mission:       Expose bench inventory and No-Registry invariant status.
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

// --- Tier 3: Internal Substrate Packages (Same-package cogs) ---

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Operational Logical Anchor
// -----------------------------------------------------------------------------

const (
	// ModuleIDBench is the logical identifier for substrate/bench.
	ModuleIDBench = "cog.agent.tools.cpi_si_cli.bench"
)

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Public Running Bench Inventory Ontology
// -----------------------------------------------------------------------------

// BenchItem describes one disposable file sitting inside a scratchpads bay.
//
// ### For Architects, Operators & Non-Programmers:
// Names the file and its size in bytes so you can decide whether to open it.
//
// ### For Systems Programmers & Engine Architects:
// Lightweight value record populated without opening file contents.
type BenchItem struct {
	Name      string
	SizeBytes int64
}

// BenchSummary holds the spatial status of `.agents/scratchpads/`.
//
// ### For Architects, Operators & Non-Programmers:
// Reports whether `bench_guide.md` exists, whether the No-Registry Law holds,
// and what notes sit inside each of the four workbench bays.
//
// ### For Systems Programmers & Engine Architects:
// Consumed both by `RunBench` CLI triage and `RunBoot` BIOS orientation.
type BenchSummary struct {
	HasGuide     bool
	NoRegistryOK bool
	Active       []BenchItem
	ReportOuts   []BenchItem
	Studies      []BenchItem
	Templates    []BenchItem
}

// -----------------------------------------------------------------------------
// Sub-Assembly 2.3: Pure O(1) Predicates
// -----------------------------------------------------------------------------

// TotalWorkingNotes returns the combined count of active and staged drafts.
//
// ### For Architects, Operators & Non-Programmers:
// Shows how many temporary working sheets currently wait on the bench.
//
// ### For Systems Programmers & Engine Architects:
// Sums `len(Active) + len(ReportOuts)` cleanly in O(1).
func (s BenchSummary) TotalWorkingNotes() int {
	return len(s.Active) + len(s.ReportOuts)
}

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Bay Inspection & Clean Triage Printing )
// =============================================================================
// Engine:        CPI-SI Portable Cognitive BIOS Tooling (cpi-si-cli)
// Stratum:       Layer 2: Running Writing Bench Spatial Triage
// Module:        cog.agent.tools.cpi_si_cli.bench
// Mission:       List workbench bays cleanly without reading file payloads.
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: Shallow Bay Directory Scanners
// -----------------------------------------------------------------------------

// InspectBench catalogs all four bays under `<agentsDir>/scratchpads/`.
//
// ### For Architects, Operators & Non-Programmers:
// Scans `active/`, `report_outs/`, `studies/`, and `bench-templates/` while
// filtering out `.gitkeep` folder placeholders.
//
// ### For Systems Programmers & Engine Architects:
// Tolerates absent subdirectories cleanly (`nil, nil`) so partial fixtures or
// cold mirrors never panic.
func InspectBench(agentsDir string) (BenchSummary, error) {
	spRoot := filepath.Join(agentsDir, "scratchpads")
	summary := BenchSummary{
		HasGuide:     FileExists(filepath.Join(spRoot, "bench_guide.md")),
		NoRegistryOK: !FileExists(filepath.Join(spRoot, "scratchpads_registry.json")),
	}
	var err error
	if summary.Active, err = listBayItems(filepath.Join(spRoot, "active")); err != nil {
		return summary, err
	}
	if summary.ReportOuts, err = listBayItems(filepath.Join(spRoot, "report_outs")); err != nil {
		return summary, err
	}
	if summary.Studies, err = listBayItems(filepath.Join(spRoot, "studies")); err != nil {
		return summary, err
	}
	if summary.Templates, err = listBayItems(filepath.Join(spRoot, "bench-templates")); err != nil {
		return summary, err
	}
	return summary, nil
}

func listBayItems(dir string) ([]BenchItem, error) {
	entries, err := os.ReadDir(dir)
	if err != nil {
		if os.IsNotExist(err) {
			return nil, nil
		}
		return nil, err
	}
	var items []BenchItem
	for _, entry := range entries {
		name := entry.Name()
		if entry.IsDir() || name == ".gitkeep" {
			continue
		}
		info, err := entry.Info()
		if err != nil {
			continue
		}
		items = append(items, BenchItem{Name: name, SizeBytes: info.Size()})
	}
	sort.Slice(items, func(i, j int) bool { return items[i].Name < items[j].Name })
	return items, nil
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.2: Triage Summary Printer
// -----------------------------------------------------------------------------

// RunBench renders a scannable overview of `.agents/scratchpads/` to w.
//
// ### For Architects, Operators & Non-Programmers:
// Prints each workbench bay, whether it's empty or holding active notes, and
// confirms `bench_guide=true` + `no_registry_law=true`.
//
// ### For Systems Programmers & Engine Architects:
// Read-only stream writer checking direct write errors on the header before
// listing deterministic alphabetical bay slices.
func RunBench(agentsDir string, w io.Writer) error {
	s, err := InspectBench(agentsDir)
	if err != nil {
		return err
	}
	if _, err := fmt.Fprintf(
		w,
		"=== CPI-SI RUNNING WRITING BENCH (.agents/scratchpads/) ===\nbench_guide=%v | no_registry_law=%v\n",
		s.HasGuide,
		s.NoRegistryOK,
	); err != nil {
		return err
	}
	printBay(w, "active/ (working notes, exempt from Rev-5)", s.Active)
	printBay(w, "report_outs/ (staged briefings, exempt from Rev-5)", s.ReportOuts)
	printBay(w, "studies/ (disposable alignment bridges, Rev-5)", s.Studies)
	printBay(w, "bench-templates/ (fast starter blueprints, Rev-5)", s.Templates)
	return nil
}

func printBay(w io.Writer, label string, items []BenchItem) {
	_, _ = fmt.Fprintf(w, "\n[%s] count=%d\n", label, len(items))
	if len(items) == 0 {
		_, _ = fmt.Fprintln(w, "  (empty bay)")
		return
	}
	for _, it := range items {
		_, _ = fmt.Fprintf(w, "  - %-48s (%dB)\n", it.Name, it.SizeBytes)
	}
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// ( Contract Verification Cogs & Pre-Flight Validation Tripwires )
// =============================================================================
// Engine:        CPI-SI Portable Cognitive BIOS Tooling (cpi-si-cli)
// Stratum:       Layer 2: Running Writing Bench Spatial Triage
// Module:        cog.agent.tools.cpi_si_cli.bench
// Mission:       4-section self-audit proving No-Registry Law & .gitkeep
// filter.
// =============================================================================

// PreflightBench executes a 4-section pre-flight self-audit for
// substrate/bench.
//
// @triggers tests/unit/substrate/bench_test.go
//
// ### For Architects, Operators & Non-Programmers:
// Verifies inside a clean temp directory that `.gitkeep` stays hidden, real
// notes appear accurately, and accidental registry JSONs trip
// NoRegistryOK=false.
//
// ### For Systems Programmers & Engine Architects:
// Isolated tempdir verification with zero mutations to live `.agents/`.
func PreflightBench() CheckResult {
	const check = "bench_preflight_4section"

	tmp, err := os.MkdirTemp("", "cpisi-bench-preflight-*")
	if err != nil {
		return CheckFail(SeverityFatal, ModuleIDBench, check, err.Error())
	}
	defer func() { _ = os.RemoveAll(tmp) }()

	sp := filepath.Join(tmp, "scratchpads")
	for _, bay := range []string{"active", "report_outs", "studies", "bench-templates"} {
		if err := os.MkdirAll(filepath.Join(sp, bay), 0o755); err != nil {
			return CheckFail(SeverityFatal, ModuleIDBench, check, err.Error())
		}
	}
	_ = os.WriteFile(filepath.Join(sp, "bench_guide.md"), []byte("# Guide\n"), 0o644)
	_ = os.WriteFile(filepath.Join(sp, "active", ".gitkeep"), nil, 0o644)
	_ = os.WriteFile(filepath.Join(sp, "active", "spike.md"), []byte("note\n"), 0o644)

	// Micro Check 1 (Always Pass): Nominal bench detects spike.md and guide.
	s, err := InspectBench(tmp)
	if err != nil || !s.HasGuide || !s.NoRegistryOK || len(s.Active) != 1 {
		return CheckFail(SeverityCritical, ModuleIDBench, check, fmt.Sprintf("Sec 1 bench inspect mismatch: %+v err=%v", s, err))
	}

	// Micro Check 2 (Never Fail): Accidental scratchpads_registry.json flags
	// false.
	_ = os.WriteFile(filepath.Join(sp, "scratchpads_registry.json"), []byte("{}"), 0o644)
	sBad, _ := InspectBench(tmp)
	if sBad.NoRegistryOK {
		return CheckFail(SeverityCritical, ModuleIDBench, check, "Sec 2 failed to detect forbidden scratchpads_registry.json")
	}
	_ = os.Remove(filepath.Join(sp, "scratchpads_registry.json"))

	// Micro Check 3 (Invariants & Limits): .gitkeep never leaks into
	// TotalWorkingNotes.
	if s.TotalWorkingNotes() != 1 || s.Active[0].Name != "spike.md" {
		return CheckFail(SeverityCritical, ModuleIDBench, check, "Sec 3 .gitkeep containment invariant broken")
	}

	// Micro Check 4 (Emergence): RunBench renders crisp output hiding .gitkeep.
	var out strings.Builder
	if err := RunBench(tmp, &out); err != nil || strings.Contains(out.String(), ".gitkeep") {
		return CheckFail(SeverityCritical, ModuleIDBench, check, "Sec 4 RunBench leaked .gitkeep or errored")
	}

	return CheckPass(ModuleIDBench, check, "Scratchpads bench triage passed all 4 micro gates (.gitkeep filtered & No-Registry checked).")
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( Epilogue Readiness Handshake & Seal )
// =============================================================================
// Engine:        CPI-SI Portable Cognitive BIOS Tooling (cpi-si-cli)
// Stratum:       Layer 2: Running Writing Bench Spatial Triage
// Module:        cog.agent.tools.cpi_si_cli.bench
// Mission:       Expose synchronous readiness predicate for substrate/bench.
// =============================================================================

// IsBenchReady reports whether substrate/bench passes its 4-section self-audit.
//
// ### For Architects, Operators & Non-Programmers:
// Confirms bench triage logic is verified and safe to run.
//
// ### For Systems Programmers & Engine Architects:
// Calls `PreflightBench().IsPass()`.
func IsBenchReady() bool {
	return PreflightBench().IsPass()
}

// End of substrate/bench.go
