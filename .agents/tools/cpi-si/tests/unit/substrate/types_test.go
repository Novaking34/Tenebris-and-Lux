// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundaries )
// =============================================================================
// @project       OmniCode Framework / Big House Ecosystem
// @file          tests/unit/substrate/types_test.go
// @id            test.unit.substrate.types
// @layer         Layer 4: Tier-2 Unit Verification Suite (Substrate Types)
// @archetype     verification_suite | unit_isolation
// @version       0.1.3-dev
// @interface_rev Rev 5
// @created       2026-11-20
// @updated       2026-11-20
// @author        CreativeWorkzStudio LLC / Seanje Lenox-Wise & Silas Oren
// @repository    https://github.com/Novaking34/OmniCode_Framework
// @system        CPI-SI Portable Cognitive BIOS CLI & Substrate Tooling
// @role          Strict 1:1 Tier-2 4-Section Unit Suite for substrate/types.go
// @maturity      Rev 5 Operational Baseline | Wave A Tools Alignment
// @chassis       zero-external-deps | Pure Layer 0 verdict & severity checks
// @scale         O(1) per verdict case
// @seams         in: substrate, tests/common | target: substrate/types.go
// @invariants    1. Universal 5-Block Lifecycle Protocol (Rev 5).
//                2. Covers Always Pass, Never Fail, Limits, Emergence.
// =============================================================================

package substrate_unit_test

// =============================================================================
// BLOCK 2: OPENING BLOCK
// =============================================================================

// --- Tier 1: Standard Go Library Packages ---
import (
	"strings"
	"testing"
)

// --- Tier 2: External Library Dependencies (None - 100% Go Stdlib) ---

// --- Tier 3: Internal Substrate Foundation Packages ---
import (
	"github.com/CreativeWorkzStudio/cpi-si-cli/substrate"
	"github.com/CreativeWorkzStudio/cpi-si-cli/tests/common"
)

const typesUnitSuiteName = "Tier 2 Unit: cog.agent.tools.cpi_si_cli.types"

// =============================================================================
// BLOCK 3: BODY BLOCK / FOUR-SECTION ONTOLOGICAL TEST SUITE
// =============================================================================

// --- Section 1: Always Pass (Nominal Golden Surface) ---

func TestTypesUnit_Sec1_CheckPassAndSeverityLabels(t *testing.T) {
	r := substrate.CheckPass(substrate.ModuleIDTypes, "nominal", "all clear")
	if !r.IsPass() || r.Severity != substrate.SeverityOK {
		t.Fatalf("[SEC 1] expected pass OK, got %+v", r)
	}
	if r.Severity.String() != "OK" {
		t.Fatalf("[SEC 1] expected severity label OK, got %q", r.Severity.String())
	}
}

// --- Section 2: Never Fail (Fault Inversion & Contradiction Prevention) ---

func TestTypesUnit_Sec2_CheckFailRejectsSeverityOKMasking(t *testing.T) {
	cases := []substrate.AuditSeverity{
		substrate.SeverityOK,
		substrate.AuditSeverity(-1),
		substrate.AuditSeverity(999),
	}
	for _, bad := range cases {
		r := substrate.CheckFail(bad, substrate.ModuleIDTypes, "fault", "forced error")
		if r.IsPass() || r.Severity != substrate.SeverityCritical {
			t.Fatalf("[SEC 2] bad sev %d was not normalized to Critical failure: %+v", bad, r)
		}
	}
}

// --- Section 3: Invariants & Limits (Valid Severity Enumeration Ceiling) ---

func TestTypesUnit_Sec3_SeverityRangeInvariants(t *testing.T) {
	for s := substrate.SeverityOK; s <= substrate.MaxSeverityCeiling; s++ {
		if !substrate.IsValidSeverity(s) || s.String() == "UNKNOWN" {
			t.Fatalf("[SEC 3] valid severity %d failed checks", s)
		}
	}
	if substrate.IsValidSeverity(substrate.AuditSeverity(-1)) ||
		substrate.IsValidSeverity(substrate.MaxSeverityCeiling+1) {
		t.Fatalf("[SEC 3] out-of-range severity accepted")
	}
}

// --- Section 4: Emergence (FormatLine Badges, Preflight & Readiness) ---

func TestTypesUnit_Sec4_FormatLinePreflightAndReadiness(t *testing.T) {
	line := substrate.CheckFail(substrate.SeverityFatal, substrate.ModuleIDTypes, "gate", "halted").FormatLine()
	if !strings.Contains(line, "[FAIL:FATAL]") || !strings.Contains(line, "halted") {
		t.Fatalf("[SEC 4] unexpected FormatLine: %q", line)
	}
	common.AssertCheckPass(t, substrate.PreflightTypes(), "PreflightTypes")
	if !substrate.IsTypesReady() {
		t.Fatalf("[SEC 4] IsTypesReady returned false")
	}
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// =============================================================================

func TestTypesUnit_Block4_SuiteSummaryAudit(t *testing.T) {
	t.Logf("%s: CheckResult constructors, severity clamps & readiness verified.", typesUnitSuiteName)
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================
// End of tests/unit/substrate/types_test.go
