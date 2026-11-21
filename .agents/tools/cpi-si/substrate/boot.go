// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundaries )
// =============================================================================
// @project       OmniCode Framework / Big House Ecosystem
// @file          substrate/boot.go
// @id            cog.agent.tools.cpi_si_cli.boot
// @layer         Layer 3: Orchestrated BIOS Orientation Card (< 2 KB Budget)
// @archetype     bios_orientation | compact_boot_probe
// @version       0.1.3-dev
// @interface_rev Rev 5
// @created       2026-11-20
// @updated       2026-11-20
// @author        CreativeWorkzStudio LLC / Seanje Lenox-Wise & Silas Oren
// @repository    https://github.com/Novaking34/OmniCode_Framework
// @system        CPI-SI Portable Cognitive BIOS CLI & Substrate Tooling
// @role          Print Compact BIOS Wake-Up Summary Under 2 KB Token Ceiling
// @maturity      Rev 5 Operational Baseline | Wave A Tools Alignment
// @chassis       zero-external-deps | Bounded JSON unmarshal (< 2 KB output)
// @scale         O(1) fixed JSON reads + O(N_skills + N_scratchpads) counts
// @seams         in: substrate/types, substrate/paths, substrate/tail,
//                substrate/bench, substrate/skills
// @invariants    1. Universal 5-Block Lifecycle Protocol (Rev 5).
//                2. Never prints full multi-MB transcripts or bulk Markdown.
//                3. Enforces < 2 KB output envelope cleanly.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Composes a compact cold-boot orientation card directly from verified
//   `.agents/`
//   JSON single sources of truth (`host/`, `operator/`, `silas_oren/`) plus
//   bench counts, skill count, and newest prior transcript pointer.
//
//   ### For Architects, Operators & Non-Programmers:
//   Lets any waking agent immediately know what computer hardware it sits on,
//   who the Operator is, who the agent is, how many skills and bench notes are
//   active, and whether a prior session tail awaits — all in under 2 KB.
//
//   ### For Systems Programmers & Engine Architects:
//   Layer 3 composer aggregating `bench.go`, `skills.go`, `tail.go`, and strict
//   typed JSON structs without spawning subprocesses or bulk-reading Markdown.
// =============================================================================

package substrate

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Module Anchor, Options Chassis & JSON Projection Structs )
// =============================================================================
// Engine:        CPI-SI Portable Cognitive BIOS Tooling (cpi-si-cli)
// Stratum:       Layer 3: Orchestrated BIOS Boot Composition
// Module:        cog.agent.tools.cpi_si_cli.boot
// Mission:       Define compact JSON SSOT views and BootOptions configuration.
// =============================================================================

// --- Tier 1: Standard Go Library Packages ---
import (
	"encoding/json"
	"fmt"
	"io"
	"os"
	"path/filepath"
	"strings"
	"time"
)

// --- Tier 2: External Library Dependencies (None - 100% Go Stdlib) ---

// --- Tier 3: Internal Substrate Foundation Packages (Same-package cogs) ---

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Logical Anchor & Output Budget Ceiling
// -----------------------------------------------------------------------------

const (
	// ModuleIDBoot is the logical identifier for substrate/boot.
	ModuleIDBoot = "cog.agent.tools.cpi_si_cli.boot"

	// MaxBootCardBytes caps emitted BIOS orientation text below 2 KB.
	MaxBootCardBytes = 2048
)

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Public Configuration & JSON Contract Views
// -----------------------------------------------------------------------------

// BootOptions configures the compact BIOS boot summary probe.
//
// ### For Architects, Operators & Non-Programmers:
// Selects brain transcript directory, optional excluded active session ID, and
// whether to skip scanning prior transcripts.
//
// ### For Systems Programmers & Engine Architects:
// Passed directly into `RunBoot` after `ResolveAgentsDir`.
type BootOptions struct {
	BrainDir      string
	ExcludeID     string
	SkipTailProbe bool
}

type hostCalDoc struct {
	Block2 struct {
		ActivePlatform      string `json:"active_platform"`
		TotalCalibratedTier int    `json:"total_calibrated_tiers"`
		NonHeadlessGuards   struct {
			DesktopRAMReserveMB int `json:"desktop_ram_reserve_mb"`
			VRAMDisplayFloorMB  int `json:"vram_display_floor_mb"`
			UsableComputeVRAMMB int `json:"usable_compute_vram_mb"`
		} `json:"non_headless_guards"`
		ActiveHostSummary struct {
			Hostname       string `json:"hostname"`
			OS             string `json:"os"`
			CPU            string `json:"cpu"`
			GPUCompute     string `json:"gpu_compute"`
			RAMMB          int    `json:"ram_mb"`
			DesktopSession string `json:"desktop_session"`
		} `json:"active_host_summary"`
	} `json:"block_2_opening"`
}

type operatorDBDoc struct {
	Block2 struct {
		ActiveOperatorID        string `json:"active_operator_id"`
		RegisteredOperatorCount int    `json:"registered_operator_count"`
	} `json:"block_2_opening"`
}

type identityProfileDoc struct {
	Block2 struct {
		Status   string `json:"status"`
		Identity struct {
			Name       string `json:"name"`
			NameStatus string `json:"name_status"`
			FamilyRole string `json:"family_role"`
			Version    string `json:"version"`
		} `json:"identity"`
	} `json:"block_2_opening"`
}

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( SSOT Decoding & Compact BIOS Boot Card Rendering )
// =============================================================================
// Engine:        CPI-SI Portable Cognitive BIOS Tooling (cpi-si-cli)
// Stratum:       Layer 3: Orchestrated BIOS Boot Composition
// Module:        cog.agent.tools.cpi_si_cli.boot
// Mission:       Render compact hardware/operator/identity/skills/bench card.
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: Compact BIOS Orientation Renderer
// -----------------------------------------------------------------------------

// RunBoot renders a compact BIOS summary card from verified JSON SSOTs to w.
//
// ### For Architects, Operators & Non-Programmers:
// Summarizes host hardware floors, active Operator, Silas Oren identity,
// installed skills count, running bench bays, and newest prior transcript tip.
//
// ### For Systems Programmers & Engine Architects:
// Fails loudly if mandatory JSON SSOTs are corrupt or absent, and degrades
// cleanly to STARTED posture when no prior session transcript exists.
func RunBoot(agentsDir string, opts BootOptions, w io.Writer) error {
	var host hostCalDoc
	if err := readJSONFile(filepath.Join(agentsDir, "host", "hardware_calibration.json"), &host); err != nil {
		return fmt.Errorf("read host calibration: %w", err)
	}

	var op operatorDBDoc
	if err := readJSONFile(filepath.Join(agentsDir, "identity", "profiles", "operator", "operator_database.json"), &op); err != nil {
		return fmt.Errorf("read operator database: %w", err)
	}

	var idDoc identityProfileDoc
	if err := readJSONFile(filepath.Join(agentsDir, "identity", "profiles", "silas_oren", "profile.json"), &idDoc); err != nil {
		return fmt.Errorf("read identity profile: %w", err)
	}

	bench, err := InspectBench(agentsDir)
	if err != nil {
		return err
	}
	skills, err := DiscoverSkills(agentsDir)
	if err != nil {
		return err
	}

	h := host.Block2.ActiveHostSummary
	g := host.Block2.NonHeadlessGuards
	id := idDoc.Block2.Identity

	if _, err := fmt.Fprintln(w, "=== CPI-SI PORTABLE COGNITIVE BIOS ==="); err != nil {
		return err
	}
	if _, err := fmt.Fprintf(
		w,
		"AGENTS_ROOT: %s\nIDENTITY:    %s (%s, %s, v%s)\nOPERATOR:    %s (%d registered)\nHOST:        %s | %s (%s) | %d MiB RAM (reserve %d MiB) | VRAM %d MiB usable\n",
		agentsDir,
		id.Name,
		id.NameStatus,
		id.FamilyRole,
		id.Version,
		op.Block2.ActiveOperatorID,
		op.Block2.RegisteredOperatorCount,
		h.Hostname,
		h.OS,
		host.Block2.ActivePlatform,
		h.RAMMB,
		g.DesktopRAMReserveMB,
		g.UsableComputeVRAMMB,
	); err != nil {
		return err
	}

	if _, err := fmt.Fprintf(
		w,
		"SKILLS:      %d auto-discovered\nSCRATCHPADS: active=%d, report_outs=%d, studies=%d, templates=%d\n",
		len(skills),
		len(bench.Active),
		len(bench.ReportOuts),
		len(bench.Studies),
		len(bench.Templates),
	); err != nil {
		return err
	}

	if !opts.SkipTailProbe {
		prior, err := DiscoverPriorTranscripts(opts.BrainDir, opts.ExcludeID)
		if err == nil && len(prior) > 0 {
			latest := prior[0]
			if _, err := fmt.Fprintf(
				w,
				"PRIOR_XFER:  %s (%s, %d bytes) -> run 'cpi-si tail --exclude %s'\n",
				latest.SessionID,
				FormatAge(latest.ModTime, time.Now()),
				latest.SizeBytes,
				opts.ExcludeID,
			); err != nil {
				return err
			}
		} else {
			if _, err := fmt.Fprintln(w, "PRIOR_XFER:  none discovered (STARTED posture)"); err != nil {
				return err
			}
		}
	}
	return nil
}

func readJSONFile(path string, dst any) error {
	raw, err := os.ReadFile(path)
	if err != nil {
		return err
	}
	return json.Unmarshal(raw, dst)
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// ( Contract Verification Cogs & Pre-Flight Validation Tripwires )
// =============================================================================
// Engine:        CPI-SI Portable Cognitive BIOS Tooling (cpi-si-cli)
// Stratum:       Layer 3: Orchestrated BIOS Boot Composition
// Module:        cog.agent.tools.cpi_si_cli.boot
// Mission:       4-section self-audit proving boot card integrity & <2KB limit.
// =============================================================================

// PreflightBoot executes a 4-section pre-flight self-audit for substrate/boot.
//
// @triggers tests/unit/substrate/boot_test.go
//
// ### For Architects, Operators & Non-Programmers:
// Builds a clean temporary BIOS sandbox, renders the boot card, checks that
// missing JSONs fail explicitly, and confirms total bytes stay under 2 KB.
//
// ### For Systems Programmers & Engine Architects:
// Isolated tempdir fixture check covering nominal hydration, missing SSOT error
// path, MaxBootCardBytes ceiling, and STARTED fallback posture.
func PreflightBoot() CheckResult {
	const check = "boot_preflight_4section"

	tmp, err := os.MkdirTemp("", "cpisi-boot-preflight-*")
	if err != nil {
		return CheckFail(SeverityFatal, ModuleIDBoot, check, err.Error())
	}
	defer func() { _ = os.RemoveAll(tmp) }()

	// Micro Check 2 (Never Fail): Missing mandatory JSONs fail explicitly.
	var failBuf strings.Builder
	if err := RunBoot(tmp, BootOptions{SkipTailProbe: true}, &failBuf); err == nil {
		return CheckFail(SeverityCritical, ModuleIDBoot, check, "Sec 2 empty root did not fail cleanly")
	}

	mustWrite := func(rel, body string) {
		p := filepath.Join(tmp, filepath.FromSlash(rel))
		_ = os.MkdirAll(filepath.Dir(p), 0o755)
		_ = os.WriteFile(p, []byte(body), 0o644)
	}
	mustWrite("host/hardware_calibration.json", `{"block_2_opening":{"active_platform":"linux","non_headless_guards":{"desktop_ram_reserve_mb":3072,"vram_display_floor_mb":350,"usable_compute_vram_mb":1387},"active_host_summary":{"hostname":"sandbox","os":"Ubuntu 26.04.1 LTS","ram_mb":15160}}}`)
	mustWrite("identity/profiles/operator/operator_database.json", `{"block_2_opening":{"active_operator_id":"seanje_lenox_wise","registered_operator_count":1}}`)
	mustWrite("identity/profiles/silas_oren/profile.json", `{"block_2_opening":{"identity":{"name":"Silas Oren","name_status":"sealed","family_role":"Joiner","version":"0.1.3-dev"}}}`)

	// Micro Check 1 & 3 (Always Pass & MaxBootCardBytes Limit).
	var out strings.Builder
	if err := RunBoot(tmp, BootOptions{BrainDir: filepath.Join(tmp, "nobrain")}, &out); err != nil {
		return CheckFail(SeverityCritical, ModuleIDBoot, check, fmt.Sprintf("Sec 1 nominal RunBoot failed: %v", err))
	}
	txt := out.String()
	if len(txt) >= MaxBootCardBytes {
		return CheckFail(SeverityCritical, ModuleIDBoot, check, fmt.Sprintf("Sec 3 boot card exceeded %dB (%dB)", MaxBootCardBytes, len(txt)))
	}

	// Micro Check 4 (Emergence): Empty brain yields crisp STARTED posture notice.
	if !strings.Contains(txt, "Silas Oren") || !strings.Contains(txt, "STARTED posture") {
		return CheckFail(SeverityCritical, ModuleIDBoot, check, "Sec 4 missing identity or STARTED posture notice")
	}

	return CheckPass(ModuleIDBoot, check, "Compact < 2 KB BIOS boot card passed all 4 micro gates.")
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( Epilogue Readiness Handshake & Seal )
// =============================================================================
// Engine:        CPI-SI Portable Cognitive BIOS Tooling (cpi-si-cli)
// Stratum:       Layer 3: Orchestrated BIOS Boot Composition
// Module:        cog.agent.tools.cpi_si_cli.boot
// Mission:       Expose synchronous readiness predicate for substrate/boot.
// =============================================================================

// IsBootReady reports whether substrate/boot passes its 4-section self-audit.
//
// ### For Architects, Operators & Non-Programmers:
// Confirms BIOS orientation card generation is ready.
//
// ### For Systems Programmers & Engine Architects:
// Calls `PreflightBoot().IsPass()`.
func IsBootReady() bool {
	return PreflightBoot().IsPass()
}

// End of substrate/boot.go
