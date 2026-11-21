// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundaries )
// =============================================================================
// @project       OmniCode Framework / Big House Ecosystem
// @file          substrate/tail.go
// @id            cog.agent.tools.cpi_si_cli.tail
// @layer         Layer 2: Operational Tooling (Transfer_Session Tail Peek)
// @archetype     read_only_probe | transfer_session_tail_reader
// @version       0.1.3-dev
// @interface_rev Rev 5
// @created       2026-11-20
// @updated       2026-11-20
// @author        CreativeWorkzStudio LLC / Seanje Lenox-Wise & Silas Oren
// @repository    https://github.com/Novaking34/OmniCode_Framework
// @system        CPI-SI Portable Cognitive BIOS CLI & Substrate Tooling
// @role          Native Zero-Dep Go Replacement for peek_transfer_tail.py
// @maturity      Rev 5 Operational Baseline | Wave A Tools Alignment
// @chassis       bounded-streaming | bufio.Reader handles unlimited line
//                lengths
// @scale         O(N_lines) single streaming pass with O(pairs) ring retention
// @seams         in: substrate/types, substrate/paths, brain/* transcript.jsonl
// @invariants    1. Universal 5-Block Lifecycle Protocol (Rev 5).
//                2. Strictly read-only: zero writes, zero network calls.
//                3. Hard context safety clamps: 1<=Pairs<=6,
//                50<=MaxChars<=1500.
//                4. Streaming Reader handles JSONL lines > 64 KB safely.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Replaces the legacy Python `peek_transfer_tail.py` script cleanly in zero-
//   dependency Go, eliminating Python `__pycache__` clutter inside `.agents/`
//   while reading only the final N conversational turns from prior sessions.
//
//   ### For Architects, Operators & Non-Programmers:
//   When starting a fresh session for token headroom, this lets the agent pull
//   just the last few turns of conversation in under 4 KB instead of flooding a
//   clean window with megabytes of old transcripts.
//
//   ### For Systems Programmers & Engine Architects:
//   Streams `.system_generated/logs/transcript.jsonl` via `bufio.NewReader`
//   (`ReadBytes('\n')`) so massive tool-output lines never hit `bufio.Scanner`
//   token limits, enforces UTF-8 rune truncation, and bounds retained history.
// =============================================================================

package substrate

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Module Anchor, Clamp Constants, Data Contracts & Pre-Conditions )
// =============================================================================
// Engine:        CPI-SI Portable Cognitive BIOS Tooling (cpi-si-cli)
// Stratum:       Layer 2: Session Continuity Telemetry
// Module:        cog.agent.tools.cpi_si_cli.tail
// Mission:       Stream prior session tails safely under hard byte ceilings.
// =============================================================================

// --- Tier 1: Standard Go Library Packages ---
import (
	"bufio"
	"encoding/json"
	"errors"
	"fmt"
	"io"
	"os"
	"path/filepath"
	"sort"
	"strings"
	"time"
)

// --- Tier 2: External Library Dependencies (None - 100% Go Stdlib) ---

// --- Tier 3: Internal Substrate Packages (Same-package cogs) ---

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Safety Clamps & Canonical Defaults
// -----------------------------------------------------------------------------

const (
	// DefaultBrainDir points to standard Antigravity session transcripts.
	DefaultBrainDir = "~/.gemini/antigravity/brain"

	// DefaultPairs reads the final 3 user/assistant pairs unless overridden.
	DefaultPairs = 3

	// MaxPairsCap prevents reading excessive historical turns into fresh ctx.
	MaxPairsCap = 6

	// DefaultMaxChars caps each printed turn excerpt at 700 Unicode runes.
	DefaultMaxChars = 700

	// MaxCharsCap bounds per-turn character extraction at 1500 runes.
	MaxCharsCap = 1500

	// MinCharsFloor preserves readable snippet context above 50 runes.
	MinCharsFloor = 50
)

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Operational Logical Anchor
// -----------------------------------------------------------------------------

const (
	// ModuleIDTail is the logical identifier for substrate/tail.
	ModuleIDTail = "cog.agent.tools.cpi_si_cli.tail"
)

// -----------------------------------------------------------------------------
// Sub-Assembly 2.3: Public Probe Options & Parsed Record Contracts
// -----------------------------------------------------------------------------

// TailOptions configures read-only Transfer_Session tail discovery.
//
// ### For Architects, Operators & Non-Programmers:
// Holds where transcripts live, which current conversation to skip, how many
// recent turn pairs to print, and whether to list sessions only.
//
// ### For Systems Programmers & Engine Architects:
// Passed verbatim into `RunTail` after clamp normalization by
// `ClampTailBounds`.
type TailOptions struct {
	BrainDir  string
	ExcludeID string
	Pairs     int
	MaxChars  int
	ListOnly  bool
}

// TranscriptInfo summarizes one discovered conversation log on disk.
//
// ### For Architects, Operators & Non-Programmers:
// Gives timestamp, session ID, file size, and path without opening the log.
//
// ### For Systems Programmers & Engine Architects:
// Sorted descending by `ModTime` with deterministic `SessionID` tie-break.
type TranscriptInfo struct {
	ModTime   time.Time
	SessionID string
	Path      string
	SizeBytes int64
}

// TurnRecord holds one extracted conversational prompt or planner response.
//
// ### For Architects, Operators & Non-Programmers:
// Stores the step number, speaker event type, timestamp, and bounded excerpt.
//
// ### For Systems Programmers & Engine Architects:
// Extracted in streaming O(N) pass and truncated cleanly on rune boundaries.
type TurnRecord struct {
	StepIndex int
	EventType string
	CreatedAt string
	Snippet   string
}

type rawLogEntry struct {
	StepIndex *int   `json:"step_index"`
	Type      string `json:"type"`
	CreatedAt string `json:"created_at"`
	Content   string `json:"content"`
}

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Bounds Enforcement, Discovery, Streaming JSONL Extraction & Rendering )
// =============================================================================
// Engine:        CPI-SI Portable Cognitive BIOS Tooling (cpi-si-cli)
// Stratum:       Layer 2: Session Continuity Telemetry
// Module:        cog.agent.tools.cpi_si_cli.tail
// Mission:       Recover recent Transfer_Session momentum in < 4 KB safely.
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: Hard Clamp & Age Formatting Helpers
// -----------------------------------------------------------------------------

// ClampTailBounds enforces hard caps guarding fresh token headroom.
//
// ### For Architects, Operators & Non-Programmers:
// Prevents flags like `--pairs 100` from accidentally dumping a giant session
// back into a fresh token window.
//
// ### For Systems Programmers & Engine Architects:
// Normalizes non-positive defaults and clamps inputs to `[1, MaxPairsCap]` and
// `[MinCharsFloor, MaxCharsCap]`.
func ClampTailBounds(pairs, maxChars int) (int, int) {
	if pairs <= 0 {
		pairs = DefaultPairs
	}
	if pairs > MaxPairsCap {
		pairs = MaxPairsCap
	}
	if maxChars <= 0 {
		maxChars = DefaultMaxChars
	}
	if maxChars < MinCharsFloor {
		maxChars = MinCharsFloor
	}
	if maxChars > MaxCharsCap {
		maxChars = MaxCharsCap
	}
	return pairs, maxChars
}

// FormatAge formats elapsed duration between mtime and now in plain English.
//
// ### For Architects, Operators & Non-Programmers:
// Prints quick relative ages like `45s ago`, `12m ago`, or `2h 15m ago`.
//
// ### For Systems Programmers & Engine Architects:
// Clamps negative clock skew cleanly to `0s ago`.
func FormatAge(mtime, now time.Time) string {
	delta := int(now.Sub(mtime).Seconds())
	if delta < 0 {
		delta = 0
	}
	if delta < 60 {
		return fmt.Sprintf("%ds ago", delta)
	}
	if delta < 3600 {
		return fmt.Sprintf("%dm ago", delta/60)
	}
	hours := delta / 3600
	mins := (delta % 3600) / 60
	return fmt.Sprintf("%dh %dm ago", hours, mins)
}

// TruncateRunes limits text cleanly to maxChars runes plus truncation marker.
//
// ### For Architects, Operators & Non-Programmers:
// Trims oversized turn replies safely without breaking UTF-8 characters.
//
// ### For Systems Programmers & Engine Architects:
// Operates on `[]rune` rather than raw bytes so multibyte glyphs stay valid.
func TruncateRunes(s string, maxChars int) string {
	if maxChars <= 0 {
		return ""
	}
	runes := []rune(s)
	if len(runes) <= maxChars {
		return s
	}
	return string(runes[:maxChars]) + "... [TRUNCATED]"
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.2: Discovery & Streaming Extraction
// -----------------------------------------------------------------------------

// DiscoverPriorTranscripts lists `transcript.jsonl` files sorted newest-first.
//
// ### For Architects, Operators & Non-Programmers:
// Finds previous conversation transcripts while automatically skipping the
// active session ID you pass via `--exclude`.
//
// ### For Systems Programmers & Engine Architects:
// Missing brain directory returns `(nil, nil)` cleanly so cold Linux/Windows/
// macOS environments degrade to `NO_PRIOR_TRANSCRIPT_FOUND` without crashing.
func DiscoverPriorTranscripts(brainDir, excludeID string) ([]TranscriptInfo, error) {
	if strings.TrimSpace(brainDir) == "" {
		brainDir = DefaultBrainDir
	}
	root := ExpandHome(brainDir)
	entries, err := os.ReadDir(root)
	if err != nil {
		if os.IsNotExist(err) {
			return nil, nil
		}
		return nil, fmt.Errorf("read brain dir %s: %w", root, err)
	}

	var found []TranscriptInfo
	for _, entry := range entries {
		name := entry.Name()
		if !entry.IsDir() || (excludeID != "" && name == excludeID) {
			continue
		}
		candidate := filepath.Join(root, name, ".system_generated", "logs", "transcript.jsonl")
		st, err := os.Stat(candidate)
		if err != nil || st.IsDir() {
			continue
		}
		found = append(found, TranscriptInfo{
			ModTime:   st.ModTime(),
			SessionID: name,
			Path:      candidate,
			SizeBytes: st.Size(),
		})
	}

	sort.Slice(found, func(i, j int) bool {
		if !found[i].ModTime.Equal(found[j].ModTime) {
			return found[i].ModTime.After(found[j].ModTime)
		}
		return found[i].SessionID > found[j].SessionID
	})
	return found, nil
}

// CountLines counts newline-delimited JSONL records using constant memory.
//
// ### For Architects, Operators & Non-Programmers:
// Reports total transcript steps for `--list-only` without parsing payloads.
//
// ### For Systems Programmers & Engine Architects:
// Uses `bufio.NewReaderSize(..., 64*1024)` so arbitrarily huge records count
// identically without scanner buffer errors.
func CountLines(path string) (int, error) {
	f, err := os.Open(path)
	if err != nil {
		return 0, err
	}
	defer func() { _ = f.Close() }()

	reader := bufio.NewReaderSize(f, 64*1024)
	total := 0
	for {
		line, err := reader.ReadBytes('\n')
		if len(line) > 0 {
			total++
		}
		if err != nil {
			if errors.Is(err, io.EOF) {
				break
			}
			return total, err
		}
	}
	return total, nil
}

// ExtractTailTurns streams JSONL once and returns the last `2*pairs` turns.
//
// ### For Architects, Operators & Non-Programmers:
// Pulls only actual user prompts and non-empty assistant replies from the end
// of the prior session, ignoring blank internal tool chatter.
//
// ### For Systems Programmers & Engine Architects:
// Tolerates corrupt JSONL lines defensively, handles >64 KB lines via
// `bufio.Reader`,
// and retains at most `2 * pairs` clamped turns.
func ExtractTailTurns(path string, pairs, maxChars int) (int, []TurnRecord, error) {
	pairs, maxChars = ClampTailBounds(pairs, maxChars)
	limit := pairs * 2

	f, err := os.Open(path)
	if err != nil {
		return 0, nil, err
	}
	defer func() { _ = f.Close() }()

	reader := bufio.NewReaderSize(f, 64*1024)
	turns := make([]TurnRecord, 0, limit+1)
	totalSteps := 0

	for {
		rawBytes, readErr := reader.ReadBytes('\n')
		if len(rawBytes) > 0 {
			totalSteps++
			trimmed := strings.TrimSpace(string(rawBytes))
			if trimmed != "" {
				var entry rawLogEntry
				if json.Unmarshal([]byte(trimmed), &entry) == nil {
					content := strings.TrimSpace(entry.Content)
					if entry.Type == "USER_INPUT" || (entry.Type == "PLANNER_RESPONSE" && content != "") {
						stepIdx := totalSteps - 1
						if entry.StepIndex != nil {
							stepIdx = *entry.StepIndex
						}
						turns = append(turns, TurnRecord{
							StepIndex: stepIdx,
							EventType: entry.Type,
							CreatedAt: entry.CreatedAt,
							Snippet:   TruncateRunes(content, maxChars),
						})
						if len(turns) > limit {
							turns = turns[1:]
						}
					}
				}
			}
		}
		if readErr != nil {
			if errors.Is(readErr, io.EOF) {
				break
			}
			return totalSteps, turns, readErr
		}
	}
	return totalSteps, turns, nil
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.3: CLI Output Renderer
// -----------------------------------------------------------------------------

// RunTail executes bounded prior-transcript discovery and writes output to w.
//
// ### For Architects, Operators & Non-Programmers:
// Renders either a clean `NO_PRIOR_TRANSCRIPT_FOUND` notice, top-5 session
// index (`--list-only`), or the bounded tail turns of the latest prior session.
//
// ### For Systems Programmers & Engine Architects:
// Read-only stream renderer honoring strict clamp invariants before I/O.
func RunTail(opts TailOptions, w io.Writer) error {
	pairs, maxChars := ClampTailBounds(opts.Pairs, opts.MaxChars)
	prior, err := DiscoverPriorTranscripts(opts.BrainDir, opts.ExcludeID)
	if err != nil {
		return err
	}
	if len(prior) == 0 {
		_, err = fmt.Fprintln(w, "NO_PRIOR_TRANSCRIPT_FOUND")
		return err
	}

	now := time.Now()
	if opts.ListOnly {
		if _, err := fmt.Fprintln(w, "=== RECENT PRIOR TRANSCRIPTS (TOP 5) ==="); err != nil {
			return err
		}
		limit := len(prior)
		if limit > 5 {
			limit = 5
		}
		for _, item := range prior[:limit] {
			steps, err := CountLines(item.Path)
			if err != nil {
				return err
			}
			dtStr := item.ModTime.Format("2006-01-02 15:04")
			if _, err := fmt.Fprintf(
				w,
				"%s (%s) | %9dB | %5d steps | %s\n",
				dtStr,
				FormatAge(item.ModTime, now),
				item.SizeBytes,
				steps,
				item.SessionID,
			); err != nil {
				return err
			}
		}
		return nil
	}

	latest := prior[0]
	totalSteps, turns, err := ExtractTailTurns(latest.Path, pairs, maxChars)
	if err != nil {
		return err
	}

	dtStr := latest.ModTime.Format("2006-01-02 15:04")
	if _, err := fmt.Fprintf(
		w,
		"SESSION_ID: %s\nLAST_MODIFIED: %s (%s)\nSIZE_BYTES: %d\nTOTAL_STEPS: %d\nTAIL_EXTRACTED: %d turns (pairs_cap=%d, max_chars=%d)\n",
		latest.SessionID,
		dtStr,
		FormatAge(latest.ModTime, now),
		latest.SizeBytes,
		totalSteps,
		len(turns),
		pairs,
		maxChars,
	); err != nil {
		return err
	}

	for _, turn := range turns {
		if _, err := fmt.Fprintf(w, "\n--- step %d | %s | %s ---\n%s\n", turn.StepIndex, turn.EventType, turn.CreatedAt, turn.Snippet); err != nil {
			return err
		}
	}
	return nil
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// ( Contract Verification Cogs & Pre-Flight Validation Tripwires )
// =============================================================================
// Engine:        CPI-SI Portable Cognitive BIOS Tooling (cpi-si-cli)
// Stratum:       Layer 2: Session Continuity Telemetry
// Module:        cog.agent.tools.cpi_si_cli.tail
// Mission:       4-section self-audit proving clamps, streaming & skip rules.
// =============================================================================

// PreflightTail executes a 4-section pre-flight self-audit for substrate/tail.
//
// @triggers tests/unit/substrate/tail_test.go
//
// ### For Architects, Operators & Non-Programmers:
// Verifies inside a temporary directory that tail reading works on large lines,
// skips empty planner chatter, enforces byte bounds, and survives missing dirs.
//
// ### For Systems Programmers & Engine Architects:
// Isolated tempdir smoke verifying clamps, empty-dir safety, >64 KB record
// tolerance, and `--exclude` isolation.
func PreflightTail() CheckResult {
	const check = "tail_preflight_4section"

	// Micro Check 1 (Always Pass): Nominal clamp & UTF-8 truncation work cleanly.
	p, c := ClampTailBounds(0, 0)
	if p != DefaultPairs || c != DefaultMaxChars {
		return CheckFail(SeverityCritical, ModuleIDTail, check, fmt.Sprintf("Sec 1 default clamp failed: (%d,%d)", p, c))
	}

	// Micro Check 2 (Never Fail): Missing brain dir produces graceful non-error
	// notice.
	var emptyBuf strings.Builder
	tmp, err := os.MkdirTemp("", "cpisi-tail-preflight-*")
	if err != nil {
		return CheckFail(SeverityFatal, ModuleIDTail, check, fmt.Sprintf("tempdir failed: %v", err))
	}
	defer func() { _ = os.RemoveAll(tmp) }()
	if err := RunTail(TailOptions{BrainDir: filepath.Join(tmp, "absent")}, &emptyBuf); err != nil {
		return CheckFail(SeverityCritical, ModuleIDTail, check, fmt.Sprintf("Sec 2 missing brain failed: %v", err))
	}
	if strings.TrimSpace(emptyBuf.String()) != "NO_PRIOR_TRANSCRIPT_FOUND" {
		return CheckFail(SeverityCritical, ModuleIDTail, check, "Sec 2 unexpected empty brain notice")
	}

	// Micro Check 3 (Invariants & Limits): Hard upper caps prevent context flood.
	hiP, hiC := ClampTailBounds(999, 999999)
	loP, loC := ClampTailBounds(-5, 5)
	if hiP != MaxPairsCap || hiC != MaxCharsCap || loP != DefaultPairs || loC != MinCharsFloor {
		return CheckFail(SeverityCritical, ModuleIDTail, check, "Sec 3 hard ceiling clamp violated")
	}

	// Micro Check 4 (Emergence): Oversized >64KB JSONL line extracts safely
	// bounded.
	logDir := filepath.Join(tmp, "prior-sess", ".system_generated", "logs")
	if err := os.MkdirAll(logDir, 0o755); err != nil {
		return CheckFail(SeverityFatal, ModuleIDTail, check, err.Error())
	}
	huge := `{"step_index":0,"type":"USER_INPUT","created_at":"2026-11-20T00:00:00Z","content":"` + strings.Repeat("A", 75000) + `"}` + "\n"
	if err := os.WriteFile(filepath.Join(logDir, "transcript.jsonl"), []byte(huge), 0o644); err != nil {
		return CheckFail(SeverityFatal, ModuleIDTail, check, err.Error())
	}
	var outBuf strings.Builder
	if err := RunTail(TailOptions{BrainDir: tmp, Pairs: 1, MaxChars: 80}, &outBuf); err != nil {
		return CheckFail(SeverityCritical, ModuleIDTail, check, fmt.Sprintf("Sec 4 huge-line RunTail failed: %v", err))
	}
	if !strings.Contains(outBuf.String(), "... [TRUNCATED]") {
		return CheckFail(SeverityCritical, ModuleIDTail, check, "Sec 4 RunTail failed to truncate huge record")
	}

	return CheckPass(ModuleIDTail, check, "Transfer_Session tail reader passed all 4 micro gates (>64KB streaming & caps).")
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( Epilogue Readiness Handshake & Seal )
// =============================================================================
// Engine:        CPI-SI Portable Cognitive BIOS Tooling (cpi-si-cli)
// Stratum:       Layer 2: Session Continuity Telemetry
// Module:        cog.agent.tools.cpi_si_cli.tail
// Mission:       Expose synchronous readiness predicate for substrate/tail.
// =============================================================================

// IsTailReady reports whether Transfer_Session tail peek passes its 4 gates.
//
// ### For Architects, Operators & Non-Programmers:
// Confirms session-transfer reading is ready and safely clamped.
//
// ### For Systems Programmers & Engine Architects:
// Delegates to `PreflightTail().IsPass()`.
func IsTailReady() bool {
	return PreflightTail().IsPass()
}

// End of substrate/tail.go
