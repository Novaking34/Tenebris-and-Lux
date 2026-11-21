// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundaries )
// =============================================================================
// @project       OmniCode Framework / Big House Ecosystem
// @file          tests/unit/substrate/tail_test.go
// @id            test.unit.substrate.tail
// @layer         Layer 4: Tier-2 Unit Verification Suite (Tail Peek Reader)
// @archetype     verification_suite | unit_isolation
// @version       0.1.3-dev
// @interface_rev Rev 5
// @created       2026-11-20
// @updated       2026-11-20
// @author        CreativeWorkzStudio LLC / Seanje Lenox-Wise & Silas Oren
// @repository    https://github.com/Novaking34/OmniCode_Framework
// @system        CPI-SI Portable Cognitive BIOS CLI & Substrate Tooling
// @role          Strict 1:1 Tier-2 4-Section Unit Suite for substrate/tail.go
// @maturity      Rev 5 Operational Baseline | Wave A Tools Alignment
// @chassis       zero-external-deps | Isolated JSONL streaming & clamp suite
// @scale         O(N_lines) streaming record verification incl. >64KB lines
// @seams         in: substrate, tests/common | target: substrate/tail.go
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
	"encoding/json"
	"path/filepath"
	"strings"
	"testing"
	"time"
	"unicode/utf8"
)

// --- Tier 2: External Library Dependencies (None - 100% Go Stdlib) ---

// --- Tier 3: Internal Substrate Foundation Packages ---
import (
	"github.com/CreativeWorkzStudio/cpi-si-cli/substrate"
	"github.com/CreativeWorkzStudio/cpi-si-cli/tests/common"
)

const tailUnitSuiteName = "Tier 2 Unit: cog.agent.tools.cpi_si_cli.tail"

// =============================================================================
// BLOCK 3: BODY BLOCK / FOUR-SECTION ONTOLOGICAL TEST SUITE
// =============================================================================

// --- Section 1: Always Pass (Nominal Discovery, Extraction & --list-only) ---

func TestTailUnit_Sec1_DiscoverListAndExtractTail(t *testing.T) {
	brain := t.TempDir()
	now := time.Now()

	u1, _ := json.Marshal(map[string]any{"step_index": 0, "type": "USER_INPUT", "created_at": "2026-11-20T10:00:00Z", "content": "first ask"})
	a1, _ := json.Marshal(map[string]any{"step_index": 1, "type": "PLANNER_RESPONSE", "created_at": "2026-11-20T10:00:05Z", "content": "first answer"})
	emptyA, _ := json.Marshal(map[string]any{"step_index": 2, "type": "PLANNER_RESPONSE", "created_at": "2026-11-20T10:00:06Z", "content": ""})
	u2, _ := json.Marshal(map[string]any{"step_index": 3, "type": "USER_INPUT", "created_at": "2026-11-20T10:01:00Z", "content": "second ask"})
	a2, _ := json.Marshal(map[string]any{"step_index": 4, "type": "PLANNER_RESPONSE", "created_at": "2026-11-20T10:01:05Z", "content": "second answer"})

	common.WriteFakeTranscript(t, brain, "sess-old", []string{string(u1)}, now.Add(-2*time.Hour))
	common.WriteFakeTranscript(t, brain, "sess-prior", []string{string(u1), string(a1), string(emptyA), string(u2), string(a2)}, now.Add(-3*time.Minute))
	common.WriteFakeTranscript(t, brain, "sess-cur", []string{string(u1)}, now)

	var listBuf bytes.Buffer
	if err := substrate.RunTail(substrate.TailOptions{BrainDir: brain, ExcludeID: "sess-cur", ListOnly: true}, &listBuf); err != nil {
		t.Fatal(err)
	}
	if !strings.Contains(listBuf.String(), "sess-prior") || strings.Contains(listBuf.String(), "sess-cur") {
		t.Fatalf("[SEC 1] unexpected list output:\n%s", listBuf.String())
	}

	var tailBuf bytes.Buffer
	if err := substrate.RunTail(substrate.TailOptions{BrainDir: brain, ExcludeID: "sess-cur", Pairs: 1}, &tailBuf); err != nil {
		t.Fatal(err)
	}
	out := tailBuf.String()
	if !strings.Contains(out, "SESSION_ID: sess-prior") || !strings.Contains(out, "second answer") || strings.Contains(out, "first answer") {
		t.Fatalf("[SEC 1] unexpected tail excerpt:\n%s", out)
	}
}

// --- Section 2: Never Fail (Missing Brain, Bad Lines & Empty Log) ---

func TestTailUnit_Sec2_GracefulMissingBrainAndCorruptJSONL(t *testing.T) {
	var buf bytes.Buffer
	if err := substrate.RunTail(substrate.TailOptions{BrainDir: filepath.Join(t.TempDir(), "absent")}, &buf); err != nil {
		t.Fatal(err)
	}
	if strings.TrimSpace(buf.String()) != "NO_PRIOR_TRANSCRIPT_FOUND" {
		t.Fatalf("[SEC 2] expected NO_PRIOR_TRANSCRIPT_FOUND, got %q", buf.String())
	}

	brain := t.TempDir()
	good, _ := json.Marshal(map[string]any{"step_index": 7, "type": "USER_INPUT", "created_at": "2026-11-20T12:00:00Z", "content": "valid row"})
	common.WriteFakeTranscript(t, brain, "sess-noisy", []string{"{not-json", "", string(good)}, time.Now())

	steps, turns, err := substrate.ExtractTailTurns(filepath.Join(brain, "sess-noisy", ".system_generated", "logs", "transcript.jsonl"), 3, 200)
	if err != nil || steps != 3 || len(turns) != 1 || turns[0].Snippet != "valid row" {
		t.Fatalf("[SEC 2] expected corrupt JSONL resilience, got steps=%d turns=%+v err=%v", steps, turns, err)
	}
}

// --- Section 3: Invariants & Limits (Context Caps, >64KB & Runes) ---

func TestTailUnit_Sec3_ContextClampsHugeLinesAndUTF8Runes(t *testing.T) {
	p, c := substrate.ClampTailBounds(999, 999999)
	if p != substrate.MaxPairsCap || c != substrate.MaxCharsCap {
		t.Fatalf("[SEC 3] upper clamp expected (%d,%d), got (%d,%d)", substrate.MaxPairsCap, substrate.MaxCharsCap, p, c)
	}
	p, c = substrate.ClampTailBounds(0, 5)
	if p != substrate.DefaultPairs || c != substrate.MinCharsFloor {
		t.Fatalf("[SEC 3] lower clamp expected (%d,%d), got (%d,%d)", substrate.DefaultPairs, substrate.MinCharsFloor, p, c)
	}

	brain := t.TempDir()
	giantUTF8 := strings.Repeat("π", 72000)
	line, _ := json.Marshal(map[string]any{"step_index": 0, "type": "USER_INPUT", "created_at": "2026-11-20T12:00:00Z", "content": giantUTF8})
	path := common.WriteFakeTranscript(t, brain, "sess-huge", []string{string(line)}, time.Now())

	_, turns, err := substrate.ExtractTailTurns(path, 1, 80)
	if err != nil || len(turns) != 1 {
		t.Fatalf("[SEC 3] huge JSONL line failed: %v", err)
	}
	if !utf8.ValidString(turns[0].Snippet) || !strings.HasSuffix(turns[0].Snippet, "... [TRUNCATED]") {
		t.Fatalf("[SEC 3] rune truncation produced invalid UTF-8 or missed marker: %q", turns[0].Snippet)
	}
}

// --- Section 4: Emergence (Age Formatting, Preflight & Readiness) ---

func TestTailUnit_Sec4_AgeFormattingPreflightAndReadiness(t *testing.T) {
	now := time.Now()
	if got := substrate.FormatAge(now.Add(-90*time.Minute), now); got != "1h 30m ago" {
		t.Fatalf("[SEC 4] FormatAge mismatch: %q", got)
	}
	common.AssertCheckPass(t, substrate.PreflightTail(), "PreflightTail")
	if !substrate.IsTailReady() {
		t.Fatalf("[SEC 4] IsTailReady returned false")
	}
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// =============================================================================

func TestTailUnit_Block4_SuiteSummaryAudit(t *testing.T) {
	t.Logf("%s: >64KB JSONL streaming, UTF-8 rune truncation & caps verified.", tailUnitSuiteName)
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================
// End of tests/unit/substrate/tail_test.go
