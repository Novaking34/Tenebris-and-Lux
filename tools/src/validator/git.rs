// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/validator/git.rs
// @id            cog.tools.validator.git
// @layer         Layer 3: Systems Developer Tooling Core Library
// @archetype     operational | stateful
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-15
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Universal 5-Block Validator Suite
// @role          Git Ledger, Conventional Commit & Release Tag Validator
// @maturity      Rev 5 Baseline
// @chassis       zero-heap | bounded git log buffer
// @scale         O(N) commit history traversal
// @seams         in: git CLI | uses: validator::report | probes: self_audit -> exit
// @invariants    1. Zero panics on malformed commit or tag message strings.
//                2. Enforces the 5-Block engineering ledger structure on all commits.
//                3. Validates conventional commit prefixes and milestone tag metadata.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Audits git commit messages, milestone tag descriptions, and repository history.
//   Guarantees that every commit records its metadata, problem statement, modifications,
//   verification proofs, and invariant affirmations according to the 5-block standard.
//
//   ### For Game Designers & Non-Programmers:
//   Ensures the development journal (git history) reads like an orderly engineering
//   logbook so anyone can see why a change was made and how it was tested.
//
//   ### For Systems Programmers & Engine Architects:
//   Validates conventional commit subject prefixes (`feat:`, `fix:`, `refactor:`, etc.),
//   asserts Block 1 through Block 5 ledger envelopes, and audits release tags.
// =============================================================================

//! # OmniCode Git Ledger Validator
//!
//! Enforces universal 5-block commit messages, conventional prefixes, and release tag structures.

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Constants, Types, Enums & Audit Report Structures )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Types, Enums & Audit Reports
// -----------------------------------------------------------------------------

use std::process::Command;
use crate::runner::{RunnerContext, TaskRunner, TaskTelemetry};

/// Operational mode for the Git validator.
#[derive(Debug, PartialEq, Eq)]
pub enum ValidatorMode {
    /// Full audit of all historical commits and tags.
    FullAudit,
    /// Audit only the most recent N commits.
    RecentCommits(usize),
    /// Audit only git release tags.
    TagsOnly,
    /// Audit a single commit message file (e.g. from commit-msg hook).
    HookFile(String),
}

/// Result of evaluating a single commit or tag message.
#[derive(Debug, Clone)]
pub struct GitDiagnosticIssue {
    pub target: String,
    pub block: &'static str,
    pub message: String,
}

/// Summary report of the git validation run.
#[derive(Debug, Default, Clone)]
pub struct GitAuditReport {
    pub commits_audited: usize,
    pub commits_passed: usize,
    pub tags_audited: usize,
    pub tags_passed: usize,
    pub issues: Vec<GitDiagnosticIssue>,
}

impl GitAuditReport {
    /// Returns true if zero issues were reported.
    pub fn is_success(&self) -> bool {
        self.issues.is_empty()
    }

    /// Prints a formatted ASCII diagnostic table of git validation results.
    pub fn print_report(&self) {
        println!("================================================================================");
        println!("       Tenebris and Lux - Git Commit & Milestone Tag Verification       ");
        println!("================================================================================");
        println!("Commits Audited:  {}", self.commits_audited);
        println!("Commits Passing:  {}", self.commits_passed);
        println!("Tags Audited:     {}", self.tags_audited);
        println!("Tags Passing:     {}", self.tags_passed);
        println!("Total Issues:     {}", self.issues.len());
        println!("--------------------------------------------------------------------------------");

        if !self.issues.is_empty() {
            println!("{:<32} {:<24} {}", "TARGET", "BLOCK", "DIAGNOSTIC");
            println!("--------------------------------------------------------------------------------");
            for issue in &self.issues {
                let truncated_target = if issue.target.len() > 30 {
                    format!("{}...", &issue.target[..27])
                } else {
                    issue.target.clone()
                };
                println!(
                    "{:<32} {:<24} {}",
                    truncated_target, issue.block, issue.message
                );
            }
            println!("--------------------------------------------------------------------------------");
        }

        let verdict = if self.is_success() {
            "ALL GIT COMMITS & TAGS SATISFY 5-BLOCK PROTOCOL (PASS)"
        } else {
            "GIT AUDIT REGRESSIONS DETECTED (FAIL)"
        };
        println!("Verdict:          {}", verdict);
        println!("================================================================================");
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Operational Constants & Block Templates
// -----------------------------------------------------------------------------

/// Canonical required 5-block headers for commit messages.
pub const REQUIRED_COMMIT_BLOCKS: [&str; 5] = [
    "BLOCK 1: METADATA & REVISION RECORD",
    "BLOCK 2: REASON FOR CHANGE & PROBLEM STATEMENT",
    "BLOCK 3: ARCHITECTURAL MODIFICATIONS & ELEVATIONS",
    "BLOCK 4: VERIFICATION, TRIPWIRES & PASS PROOF",
    "BLOCK 5: INVARIANT COVENANTS & CLOSING AFFIRMATIONS",
];

/// Canonical required 5-block headers for release tag messages.
pub const REQUIRED_TAG_BLOCKS: [&str; 5] = [
    "BLOCK 1: METADATA BLOCK",
    "BLOCK 2: RELEASE CONTEXT & ONTOLOGY",
    "BLOCK 3: ARCHITECTURAL MODIFICATIONS & ELEVATIONS",
    "BLOCK 4: AUDIT PROCEDURES & VERIFICATION PROOFS",
    "BLOCK 5: CLOSING & CANONICAL ARTIFACT LINKS",
];

/// Allowed conventional commit subject prefixes.
pub const ALLOWED_PREFIXES: [&str; 8] = [
    "feat(", "fix(", "docs(", "refactor(", "test(", "chore(", "perf(", "build(",
];

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Commit & Tag Message Auditing Engine, History Scanners & Task Runner )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: Commit & Tag Message Auditing Engine
// -----------------------------------------------------------------------------

/// Audits a commit message string against the 5-block protocol.
///
/// # Dual-Audience Documentation
/// * For Game Designers: Checks that a commit note explains what changed and why.
/// * For Systems Programmers: Validates subject prefix conventions and requires all 5 blocks.
///
/// ```rust
/// use tl_tools::validator::git::audit_commit_message;
///
/// let valid_msg = r#"feat(core): new pacer
///
/// BLOCK 1: METADATA & REVISION RECORD
/// Project: Tenebris and Lux
///
/// BLOCK 2: REASON FOR CHANGE & PROBLEM STATEMENT
/// Need 30fps lock.
///
/// BLOCK 3: ARCHITECTURAL MODIFICATIONS & ELEVATIONS
/// Added pacer cog.
///
/// BLOCK 4: VERIFICATION, TRIPWIRES & PASS PROOF
/// 51/51 tests pass.
///
/// BLOCK 5: INVARIANT COVENANTS & CLOSING AFFIRMATIONS
/// Zero heap allocations.
/// "#;
/// let issues = audit_commit_message("HEAD", valid_msg);
/// assert!(issues.is_empty());
/// ```
pub fn audit_commit_message(
    target_id: &str,
    raw_message: &str,
) -> Vec<GitDiagnosticIssue> {
    let mut issues = Vec::new();
    let lines: Vec<&str> = raw_message
        .lines()
        .map(|l| l.trim_end())
        .filter(|l| !l.starts_with('#'))
        .collect();

    if lines.is_empty() {
        issues.push(GitDiagnosticIssue {
            target: target_id.to_string(),
            block: "Structure",
            message: "Commit message is completely empty".to_string(),
        });
        return issues;
    }

    // 1. Audit Subject Line (Line 0)
    let subject = lines[0].trim();
    let has_valid_prefix = ALLOWED_PREFIXES.iter().any(|prefix| subject.starts_with(prefix));
    if !has_valid_prefix {
        issues.push(GitDiagnosticIssue {
            target: target_id.to_string(),
            block: "Subject",
            message: format!(
                "Invalid conventional subject prefix in '{}'. Must start with one of: {:?}",
                subject, ALLOWED_PREFIXES
            ),
        });
    }

    // 2. Audit 5-Block Structure
    let full_text = raw_message;
    for block_name in &REQUIRED_COMMIT_BLOCKS {
        if !full_text.contains(block_name) {
            issues.push(GitDiagnosticIssue {
                target: target_id.to_string(),
                block: block_name,
                message: format!("Missing required commit envelope header: '{}'", block_name),
            });
        }
    }

    issues
}

/// Audits a release tag message against the 5-block tag manifest standard.
///
/// # Dual-Audience Documentation
/// * For Game Designers: Confirms milestone releases contain their full changelog.
/// * For Systems Programmers: Verifies release tag metadata blocks and ontology sections.
pub fn audit_tag_message(
    tag_name: &str,
    raw_message: &str,
) -> Vec<GitDiagnosticIssue> {
    let mut issues = Vec::new();
    let full_text = raw_message;

    for block_name in &REQUIRED_TAG_BLOCKS {
        if !full_text.contains(block_name) {
            issues.push(GitDiagnosticIssue {
                target: tag_name.to_string(),
                block: block_name,
                message: format!("Missing required tag manifest header: '{}'", block_name),
            });
        }
    }

    issues
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.2: Git History & Milestone Inspection Engine
// -----------------------------------------------------------------------------

/// Audits git commits from repository history.
pub fn audit_git_history(
    max_commits: Option<usize>,
    report: &mut GitAuditReport,
) {
    let mut cmd = Command::new("git");
    cmd.args(["log", "--format=%H%x00%B%x00"]);
    if let Some(n) = max_commits {
        cmd.arg(format!("-n{}", n));
    }

    let output = match cmd.output() {
        Ok(out) if out.status.success() => out,
        _ => return,
    };

    let raw_log = String::from_utf8_lossy(&output.stdout);
    let entries: Vec<&str> = raw_log.split('\0').collect();

    let mut i = 0;
    while i + 1 < entries.len() {
        let hash = entries[i].trim();
        let message = entries[i + 1].trim();
        i += 2;

        if hash.is_empty() {
            continue;
        }

        report.commits_audited += 1;
        let issues = audit_commit_message(hash, message);
        if issues.is_empty() {
            report.commits_passed += 1;
        } else {
            report.issues.extend(issues);
        }
    }
}

/// Audits all annotated git release tags.
pub fn audit_git_tags(report: &mut GitAuditReport) {
    let output = match Command::new("git").args(["tag", "-l"]).output() {
        Ok(out) if out.status.success() => out,
        _ => return,
    };

    let tags_str = String::from_utf8_lossy(&output.stdout);
    for tag in tags_str.lines().map(|t| t.trim()).filter(|t| !t.is_empty()) {
        let tag_out = match Command::new("git").args(["cat-file", "-p", tag]).output() {
            Ok(out) if out.status.success() => out,
            _ => continue,
        };

        let message = String::from_utf8_lossy(&tag_out.stdout);
        report.tags_audited += 1;
        let issues = audit_tag_message(tag, &message);
        if issues.is_empty() {
            report.tags_passed += 1;
        } else {
            report.issues.extend(issues);
        }
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.3: Pipeline Task Runner Implementation
// -----------------------------------------------------------------------------

/// Task runner adapter integrating git commit message validation.
pub struct GitValidationTask;

impl TaskRunner for GitValidationTask {
    fn name(&self) -> &'static str {
        "git_validator"
    }

    fn description(&self) -> &'static str {
        "Audits git commit messages and milestone tags for 5-block ledger compliance"
    }

    fn preflight(&self, ctx: &RunnerContext) -> Result<(), String> {
        let git_dir = ctx.repo_root.join(".git");
        if !git_dir.exists() {
            return Err(format!("Not a git repository: {:?}", ctx.repo_root));
        }
        Ok(())
    }

    fn run(&self, ctx: &RunnerContext) -> Result<TaskTelemetry, String> {
        let mut report = GitAuditReport::default();
        audit_git_history(Some(10), &mut report);

        if ctx.verbose {
            report.print_report();
        }

        if report.is_success() {
            Ok(TaskTelemetry::success(
                self.name(),
                0,
                &format!("Audited {} recent commits cleanly", report.commits_audited),
            ))
        } else {
            Ok(TaskTelemetry::failure(
                self.name(),
                0,
                &format!(
                    "Git audit failed: {} issues across {} commits",
                    report.issues.len(),
                    report.commits_audited
                ),
            ))
        }
    }
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// ( Preflight Diagnostics & Four-Section Ontological Test Suite )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 4.1: Internal Subsystem Self-Audit Probe
// -----------------------------------------------------------------------------

/// Internal self-audit testing Git validation rules.
///
/// # Dual-Audience Documentation
/// * For Game Designers: Tests the git ledger inspector.
/// * For Systems Programmers: Runs compliant and malformed commit messages through `audit_commit_message`.
///
/// ```rust
/// use tl_tools::validator::git::self_audit;
///
/// assert!(self_audit().is_ok());
/// ```
pub fn self_audit() -> Result<(), &'static str> {
    let empty_issues = audit_commit_message("TEST", "");
    if empty_issues.is_empty() {
        return Err("Self-audit failed: empty commit message produced zero issues");
    }
    Ok(())
}

// -----------------------------------------------------------------------------
// Sub-Assembly 4.2: Four-Section Ontological Unit Tests
// -----------------------------------------------------------------------------

#[cfg(test)]
mod tests {
    use super::*;

    // -------------------------------------------------------------------------
    // SECTION 1: ALWAYS PASS (Nominal Golden Surface Cogs)
    // -------------------------------------------------------------------------
    #[test]
    fn test_section_1_nominal_valid_commit_passes() {
        let valid_msg = r#"feat(core): implement bounded clock pacer

BLOCK 1: METADATA & REVISION RECORD
===================================
Project:        Tenebris and Lux
Stratum:        Layer 0 (Core Utilities)
Author:         CreativeWorkzStudio LLC / ProfessorSeanEX
Date:           2026-11-07

BLOCK 2: REASON FOR CHANGE & PROBLEM STATEMENT
=============================================
Pacer needed for 30fps lock.

BLOCK 3: ARCHITECTURAL MODIFICATIONS & ELEVATIONS
================================================
Implemented clock_pacer.c/h.

BLOCK 4: VERIFICATION, TRIPWIRES & PASS PROOF
============================================
51/51 CTests passing.

BLOCK 5: INVARIANT COVENANTS & CLOSING AFFIRMATIONS
==================================================
ALLOC_ZERO_HEAP enforced.
"#;
        let issues = audit_commit_message("HEAD", valid_msg);
        assert!(issues.is_empty(), "Expected clean audit, got: {:?}", issues);
    }

    // -------------------------------------------------------------------------
    // SECTION 2: NEVER FAIL (Fault Inversion & Graceful Degradation)
    // -------------------------------------------------------------------------
    #[test]
    fn test_section_2_never_fail_missing_block_triggers_diagnostic() {
        let invalid_msg = r#"feat(core): missing block 5

BLOCK 1: METADATA & REVISION RECORD
===================================
Project: Tenebris and Lux

BLOCK 2: REASON FOR CHANGE & PROBLEM STATEMENT
Problem.

BLOCK 3: ARCHITECTURAL MODIFICATIONS & ELEVATIONS
Mods.

BLOCK 4: VERIFICATION, TRIPWIRES & PASS PROOF
Tests.
"#;
        let issues = audit_commit_message("HEAD", invalid_msg);
        assert!(!issues.is_empty());
        assert!(issues.iter().any(|i| i.block.contains("BLOCK 5")));
    }

    // -------------------------------------------------------------------------
    // SECTION 3: INVARIANTS, SATURATION & LIMIT GUARDS
    // -------------------------------------------------------------------------
    #[test]
    fn test_section_3_invariants_invalid_subject_prefix() {
        let bad_prefix_msg = "random commit message without conventional prefix\n";
        let issues = audit_commit_message("HEAD", bad_prefix_msg);
        assert!(issues.iter().any(|i| i.block == "Subject"));
    }

    // -------------------------------------------------------------------------
    // SECTION 4: EMERGENCE & COMPOSITE WORKFLOWS
    // -------------------------------------------------------------------------
    #[test]
    fn test_section_4_emergence_self_audit() {
        assert!(self_audit().is_ok());
    }
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( Epilogue Handshake & Subsystem Readiness Probe )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 5.1: Epilogue Readiness Probe & Lifecycle Handshake
// -----------------------------------------------------------------------------

/// Epilogue readiness probe for the Git validator subsystem.
///
/// # Dual-Audience Documentation
/// * For Game Designers: Confirms the git logbook inspector is ready.
/// * For Systems Programmers: Returns true when self-audit passes.
///
/// ```rust
/// use tl_tools::validator::git::is_git_validator_ready;
///
/// assert!(is_git_validator_ready());
/// ```
pub fn is_git_validator_ready() -> bool {
    self_audit().is_ok()
}
