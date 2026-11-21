// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundaries )
// =============================================================================
// @project       OmniCode Framework / Big House Ecosystem
// @file          tests/unit/substrate/boot_test.go
// @id            test.unit.substrate.boot
// @layer         Layer 4: Tier-2 Unit Verification Suite (BIOS Boot Card)
// @archetype     verification_suite | unit_isolation
// @version       0.1.3-dev
// @interface_rev Rev 5
// @created       2026-11-20
// @updated       2026-11-20
// @author        CreativeWorkzStudio LLC / Seanje Lenox-Wise & Silas Oren
// @repository    https://github.com/Novaking34/OmniCode_Framework
// @system        CPI-SI Portable Cognitive BIOS CLI & Substrate Tooling
// @role          Strict 1:1 Tier-2 4-Section Unit Suite for substrate/boot.go
// @maturity      Rev 5 Operational Baseline | Wave A Tools Alignment
// @chassis       zero-external-deps | Isolated BIOS JSON & < 2KB ceiling test
// @scale         O(1) fixed SSOT boot card verification
// @seams         in: substrate, tests/common | target: substrate/boot.go
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
)

// --- Tier 2: External Library Dependencies (None - 100% Go Stdlib) ---

// --- Tier 3: Internal Substrate Foundation Packages ---
import (
	"github.com/CreativeWorkzStudio/cpi-si-cli/substrate"
	"github.com/CreativeWorkzStudio/cpi-si-cli/tests/common"
)

const bootUnitSuiteName = "Tier 2 Unit: cog.agent.tools.cpi_si_cli.boot"

// =============================================================================
// BLOCK 3: BODY BLOCK / FOUR-SECTION ONTOLOGICAL TEST SUITE
// =============================================================================

// --- Section 1: Always Pass (Nominal BIOS Orientation Hydration) ---

func TestBootUnit_Sec1_NominalBootCardHydration(t *testing.T) {
	agents := common.BuildMinimalCleanSubstrate(t)
	var buf bytes.Buffer
	if err := substrate.RunBoot(agents, substrate.BootOptions{SkipTailProbe: true}, &buf); err != nil {
		t.Fatal(err)
	}
	out := buf.String()
	for _, want := range []string{"Silas Oren", "seanje_lenox_wise", "test-box", "SKILLS:      1", "active=1"} {
		if !strings.Contains(out, want) {
			t.Fatalf("[SEC 1] boot card missing %q:\n%s", want, out)
		}
	}
}

// --- Section 2: Never Fail (Missing / Corrupt Mandatory JSON SSOTs) ---

func TestBootUnit_Sec2_FailsLoudlyOnMissingOrCorruptJSON(t *testing.T) {
	var buf bytes.Buffer
	if err := substrate.RunBoot(t.TempDir(), substrate.BootOptions{SkipTailProbe: true}, &buf); err == nil {
		t.Fatalf("[SEC 2] missing hardware_calibration.json unexpectedly passed")
	}

	agents := common.BuildMinimalCleanSubstrate(t)
	common.WriteFixtureFile(t, filepath.Join(agents, "identity", "profiles", "silas_oren", "profile.json"), "{corrupt")
	buf.Reset()
	if err := substrate.RunBoot(agents, substrate.BootOptions{SkipTailProbe: true}, &buf); err == nil {
		t.Fatalf("[SEC 2] corrupt profile.json unexpectedly passed")
	}
}

// --- Section 3: Invariants & Limits (Strict < 2 KB Token Budget Envelope) ---

func TestBootUnit_Sec3_EnforcesUnder2KBEnvelope(t *testing.T) {
	agents := common.BuildMinimalCleanSubstrate(t)
	brain := t.TempDir()
	row, _ := json.Marshal(map[string]any{"step_index": 0, "type": "USER_INPUT", "created_at": "2026-11-20T12:00:00Z", "content": "hi"})
	common.WriteFakeTranscript(t, brain, "sess-prev", []string{string(row)}, time.Now())

	var buf bytes.Buffer
	if err := substrate.RunBoot(agents, substrate.BootOptions{BrainDir: brain, ExcludeID: "sess-cur"}, &buf); err != nil {
		t.Fatal(err)
	}
	if buf.Len() >= substrate.MaxBootCardBytes {
		t.Fatalf("[SEC 3] boot card size %d exceeded ceiling %d", buf.Len(), substrate.MaxBootCardBytes)
	}
}

// --- Section 4: Emergence (STARTED vs PRIOR_XFER, Preflight & Ready) ---

func TestBootUnit_Sec4_TransferVsStartedHintPreflightAndReadiness(t *testing.T) {
	agents := common.BuildMinimalCleanSubstrate(t)
	brain := t.TempDir()

	var startedBuf bytes.Buffer
	if err := substrate.RunBoot(agents, substrate.BootOptions{BrainDir: filepath.Join(brain, "none")}, &startedBuf); err != nil {
		t.Fatal(err)
	}
	if !strings.Contains(startedBuf.String(), "STARTED posture") {
		t.Fatalf("[SEC 4] expected STARTED posture notice:\n%s", startedBuf.String())
	}

	row, _ := json.Marshal(map[string]any{"step_index": 0, "type": "USER_INPUT", "created_at": "2026-11-20T12:00:00Z", "content": "hi"})
	common.WriteFakeTranscript(t, brain, "sess-prior", []string{string(row)}, time.Now())
	var xferBuf bytes.Buffer
	if err := substrate.RunBoot(agents, substrate.BootOptions{BrainDir: brain, ExcludeID: "sess-cur"}, &xferBuf); err != nil {
		t.Fatal(err)
	}
	if !strings.Contains(xferBuf.String(), "PRIOR_XFER:  sess-prior") || !strings.Contains(xferBuf.String(), "tail --exclude sess-cur") {
		t.Fatalf("[SEC 4] expected PRIOR_XFER hint:\n%s", xferBuf.String())
	}

	common.AssertCheckPass(t, substrate.PreflightBoot(), "PreflightBoot")
	if !substrate.IsBootReady() {
		t.Fatalf("[SEC 4] IsBootReady returned false")
	}
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// =============================================================================

func TestBootUnit_Block4_SuiteSummaryAudit(t *testing.T) {
	t.Logf("%s: < 2KB BIOS card hydration, STARTED/PRIOR_XFER cues & fail paths verified.", bootUnitSuiteName)
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================
// End of tests/unit/substrate/boot_test.go
