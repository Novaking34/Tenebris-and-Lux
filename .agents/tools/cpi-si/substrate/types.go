// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundaries )
// =============================================================================
// @project       OmniCode Framework / Big House Ecosystem
// @file          substrate/types.go
// @id            cog.agent.tools.cpi_si_cli.types
// @layer         Layer 0: Pure Foundation Types & Audit Verdict Contracts
// @archetype     pure_foundation | audit_and_verdict_types
// @version       0.1.3-dev
// @interface_rev Rev 5
// @created       2026-11-20
// @updated       2026-11-20
// @author        CreativeWorkzStudio LLC / Seanje Lenox-Wise & Silas Oren
// @repository    https://github.com/Novaking34/OmniCode_Framework
// @system        CPI-SI Portable Cognitive BIOS CLI & Substrate Tooling
// @role          Shared Severity Enums, CheckResult Receipts & Status Helpers
// @maturity      Rev 5 Operational Baseline | Wave A Tools Alignment
// @chassis       zero-external-deps | Pure Go stdlib (zero net/os/CGO imports)
// @scale         O(1) stack-allocated value structs & deterministic formatting
// @seams         exports: AuditSeverity, CheckResult, CheckPass, CheckFail
// @invariants    1. Universal 5-Block Lifecycle Protocol (Rev 5).
//                2. Zero external dependencies, zero I/O in Layer 0 types.
//                3. Every Block 4 Preflight* cog returns a typed CheckResult.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Establishes the foundational verdict types (`AuditSeverity` and
//   `CheckResult`) shared across all CPI-SI CLI substrate modules and preflight
//   gates so every cog speaks one uniform health receipt format.
//
//   ### For Architects, Operators & Non-Programmers:
//   Just like FaithNet nodes report clear pass/fail health receipts instead of
//   vague booleans, every substrate module here answers with its module ID,
//   the exact check performed, severity level, and a plain-English explanation.
//
//   ### For Systems Programmers & Engine Architects:
//   Pure value types mirroring FaithNet's `data.AuditResult` contract at
//   Layer 0 with zero heap pressure or OS coupling.
// =============================================================================

package substrate

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Module Anchor, Severity Enums, Public Structs & O(1) Predicates )
// =============================================================================
// Engine:        CPI-SI Portable Cognitive BIOS Tooling (cpi-si-cli)
// Stratum:       Layer 0: Pure Foundation Types & Verdict Contracts
// Module:        cog.agent.tools.cpi_si_cli.types
// Mission:       Define AuditSeverity, CheckResult, and pure O(1) predicates.
// =============================================================================

// --- Tier 1: Standard Go Library Packages ---
import "fmt"

// --- Tier 2: External Library Dependencies (None - 100% Go Stdlib) ---

// --- Tier 3: Internal Substrate Foundation Packages (None - Layer 0 Root) ---

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Pre-Conditions & Canonical Severity Levels
// -----------------------------------------------------------------------------

// AuditSeverity classifies the seriousness of a preflight or audit receipt.
//
// ### For Architects, Operators & Non-Programmers:
// Tells operators at a glance whether a check passed cleanly (`OK`), noticed a
// non-fatal advisory (`WARN`), hit a broken invariant (`CRITICAL`), or suffered
// an unrecoverable structural halt (`FATAL`).
//
// ### For Systems Programmers & Engine Architects:
// Compact enumeration of deterministic preflight severity bands matching the
// fourfold T4 / FaithNet audit posture.
type AuditSeverity int

const (
	// SeverityOK indicates nominal verified health (+1:PROCEED).
	SeverityOK AuditSeverity = iota
	// SeverityWarn indicates a non-fatal advisory (0:AWAIT review).
	SeverityWarn
	// SeverityCritical indicates a contract breach (-1:ROLLBACK / repair).
	SeverityCritical
	// SeverityFatal indicates an unrecoverable structural halt (NULL:HALT).
	SeverityFatal
)

// MaxSeverityCeiling bounds valid AuditSeverity enumerations.
const MaxSeverityCeiling = SeverityFatal

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Operational Logical Anchor
// -----------------------------------------------------------------------------

const (
	// ModuleIDTypes is the movement-resilient logical ID for substrate/types.
	ModuleIDTypes = "cog.agent.tools.cpi_si_cli.types"

	// CLIVersion identifies the active CPI-SI CLI substrate build coordinate.
	CLIVersion = "0.1.3-dev (go1.26.8 stdlib, zero external deps)"
)

// -----------------------------------------------------------------------------
// Sub-Assembly 2.3: Public Audit Verdict Receipt Ontology
// -----------------------------------------------------------------------------

// CheckResult captures a single deterministic preflight outcome.
//
// ### For Architects, Operators & Non-Programmers:
// Acts as a signed inspection card telling you which component was checked,
// whether it passed or failed, and why.
//
// ### For Systems Programmers & Engine Architects:
// Immutable value receipt produced by Block 4 `Preflight*` routines across all
// substrate packages.
type CheckResult struct {
	Passed   bool
	Severity AuditSeverity
	ModuleID string
	Check    string
	Message  string
}

// -----------------------------------------------------------------------------
// Sub-Assembly 2.4: Pure O(1) Predicates
// -----------------------------------------------------------------------------

// IsPass reports whether the check verdict passed cleanly at SeverityOK.
//
// ### For Architects, Operators & Non-Programmers:
// Simple true/false test confirming the health card contains zero failures.
//
// ### For Systems Programmers & Engine Architects:
// Requires both `Passed == true` and `Severity == SeverityOK` so degraded
// receipts never masquerade as full passes.
func (r CheckResult) IsPass() bool {
	return r.Passed && r.Severity == SeverityOK
}

// IsValidSeverity reports whether a severity value falls within known bounds.
//
// ### For Architects, Operators & Non-Programmers:
// Guards against out-of-range status codes in audit summaries.
//
// ### For Systems Programmers & Engine Architects:
// O(1) bounds check enforcing `[SeverityOK, SeverityFatal]`.
func IsValidSeverity(sev AuditSeverity) bool {
	return sev >= SeverityOK && sev <= MaxSeverityCeiling
}

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Verdict Constructors & Human-Readable Receipt Formatters )
// =============================================================================
// Engine:        CPI-SI Portable Cognitive BIOS Tooling (cpi-si-cli)
// Stratum:       Layer 0: Pure Foundation Types & Verdict Contracts
// Module:        cog.agent.tools.cpi_si_cli.types
// Mission:       Construct passing/failing CheckResult receipts cleanly.
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: Canonical Constructors
// -----------------------------------------------------------------------------

// CheckPass builds a passing CheckResult receipt for a verified module gate.
//
// ### For Architects, Operators & Non-Programmers:
// Records a clean `[PASS]` badge with a human-readable confirmation note.
//
// ### For Systems Programmers & Engine Architects:
// Pure O(1) value constructor setting `Passed: true` and `Severity:
// SeverityOK`.
func CheckPass(moduleID, check, msg string) CheckResult {
	return CheckResult{
		Passed:   true,
		Severity: SeverityOK,
		ModuleID: moduleID,
		Check:    check,
		Message:  msg,
	}
}

// CheckFail builds a failing CheckResult receipt preserving failure severity.
//
// ### For Architects, Operators & Non-Programmers:
// Records a `[FAIL]` receipt naming the broken gate and exact failure cause.
//
// ### For Systems Programmers & Engine Architects:
// Normalizes `SeverityOK` accidental inputs up to `SeverityCritical` so a fail
// constructor can never emit a contradictory OK severity.
func CheckFail(sev AuditSeverity, moduleID, check, msg string) CheckResult {
	if sev <= SeverityOK || sev > MaxSeverityCeiling {
		sev = SeverityCritical
	}
	return CheckResult{
		Passed:   false,
		Severity: sev,
		ModuleID: moduleID,
		Check:    check,
		Message:  msg,
	}
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.2: Deterministic Receipt Rendering
// -----------------------------------------------------------------------------

// String formats an AuditSeverity into its compact uppercase badge.
//
// ### For Architects, Operators & Non-Programmers:
// Turns numeric severity codes into readable labels (`OK`, `WARN`, `CRITICAL`,
// `FATAL`).
//
// ### For Systems Programmers & Engine Architects:
// Implements `fmt.Stringer` cleanly with `UNKNOWN` defensive fallback.
func (s AuditSeverity) String() string {
	switch s {
	case SeverityOK:
		return "OK"
	case SeverityWarn:
		return "WARN"
	case SeverityCritical:
		return "CRITICAL"
	case SeverityFatal:
		return "FATAL"
	default:
		return "UNKNOWN"
	}
}

// FormatLine renders a CheckResult as one scannable single-line status receipt.
//
// ### For Architects, Operators & Non-Programmers:
// Formats one clean line suitable for CLI preflight output and CI logs.
//
// ### For Systems Programmers & Engine Architects:
// Allocation-minimal formatter used directly by the `cpi-si preflight` runner.
func (r CheckResult) FormatLine() string {
	status := "FAIL"
	if r.IsPass() {
		status = "PASS"
	}
	return fmt.Sprintf("[%s:%s] %-34s | %s", status, r.Severity.String(), r.ModuleID, r.Message)
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// ( Contract Verification Cogs & Pre-Flight Validation Tripwires )
// =============================================================================
// Engine:        CPI-SI Portable Cognitive BIOS Tooling (cpi-si-cli)
// Stratum:       Layer 0: Pure Foundation Types & Verdict Contracts
// Module:        cog.agent.tools.cpi_si_cli.types
// Mission:       4-section self-audit proving verdict math & severity bounds.
// =============================================================================

// PreflightTypes executes a 4-section pre-flight self-audit for Layer 0 types.
//
// @triggers tests/unit/substrate/types_test.go
//
// ### For Architects, Operators & Non-Programmers:
// Runs four quick sanity checks proving pass cards report healthy, fail cards
// refuse to masquerade as healthy, and status lines format cleanly.
//
// ### For Systems Programmers & Engine Architects:
// Zero-allocation in-memory preflight covering Always Pass, Never Fail,
// Bounds/Severity, and Receipt Formatting emergence.
func PreflightTypes() CheckResult {
	const check = "types_preflight_4section"

	// Micro Check 1 (Always Pass): CheckPass reports Passed=true and SeverityOK.
	pass := CheckPass(ModuleIDTypes, check, "nominal ok")
	if !pass.IsPass() {
		return CheckFail(SeverityFatal, ModuleIDTypes, check, "Sec 1 CheckPass failed IsPass()")
	}

	// Micro Check 2 (Never Fail): CheckFail normalizes SeverityOK and stays
	// !IsPass().
	fail := CheckFail(SeverityOK, ModuleIDTypes, check, "forced failure")
	if fail.IsPass() || fail.Severity != SeverityCritical {
		return CheckFail(SeverityCritical, ModuleIDTypes, check, "Sec 2 CheckFail did not normalize OK severity")
	}

	// Micro Check 3 (Invariants & Limits): Out-of-range severity rejected.
	if IsValidSeverity(AuditSeverity(-1)) || IsValidSeverity(AuditSeverity(99)) {
		return CheckFail(SeverityCritical, ModuleIDTypes, check, "Sec 3 IsValidSeverity accepted out-of-range value")
	}

	// Micro Check 4 (Emergence): Receipt formats deterministically with badge.
	line := pass.FormatLine()
	if line == "" || fail.FormatLine() == line {
		return CheckFail(SeverityCritical, ModuleIDTypes, check, "Sec 4 FormatLine produced ambiguous or empty receipt")
	}

	return CheckPass(ModuleIDTypes, check, "Layer 0 CheckResult and severity math passed all 4 micro gates.")
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( Epilogue Readiness Handshake & Seal )
// =============================================================================
// Engine:        CPI-SI Portable Cognitive BIOS Tooling (cpi-si-cli)
// Stratum:       Layer 0: Pure Foundation Types & Verdict Contracts
// Module:        cog.agent.tools.cpi_si_cli.types
// Mission:       Expose synchronous readiness predicate for substrate/types.
// =============================================================================

// IsTypesReady reports whether Layer 0 foundation types pass their 4-gate
// check.
//
// ### For Architects, Operators & Non-Programmers:
// Quick go/no-go check confirming core status receipts are healthy.
//
// ### For Systems Programmers & Engine Architects:
// Delegates synchronously to `PreflightTypes().IsPass()`.
func IsTypesReady() bool {
	return PreflightTypes().IsPass()
}

// End of substrate/types.go
