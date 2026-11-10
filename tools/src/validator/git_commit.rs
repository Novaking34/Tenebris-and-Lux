// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/validator/git_commit.rs
// @id            tools.validator.git_commit
// @layer         Layer 3: Systems Developer Tooling Core Library
// @version       0.1.0-dev
// @interface_rev Rev 1
// @date          2026-11-09
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Canonical Git commit message and milestone tag auditing module.
//   Enforces the Universal 5-Block Ledger Protocol and governance standards
//   across all repository commits and annotated release tags.
// =============================================================================

//! # Git Commit & Release Tag Auditing Engine
//!
//! Enforces the Universal 5-Block Lifecycle Protocol across git commit
//! messages and annotated milestone release tags.

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES & STRUCTURES
// Stratum: Developer Tooling | Machine: tools.validator.git_commit
// =============================================================================

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
    pub fn is_success(&self) -> bool {
        self.issues.is_empty()
    }

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

// =============================================================================
// BLOCK 3: BODY BLOCK / CORE VALIDATION LOGIC
// Stratum: Developer Tooling | Machine: tools.validator.git_commit
// =============================================================================

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

/// Audits a commit message string against the 5-block protocol.
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
            block: "Subject",
            message: "Commit message is completely empty".to_string(),
        });
        return issues;
    }

    // 1. Validate Subject Line
    let subject = lines[0].trim();
    let has_valid_prefix = ALLOWED_PREFIXES
        .iter()
        .any(|prefix| subject.starts_with(prefix));
    if !has_valid_prefix {
        issues.push(GitDiagnosticIssue {
            target: target_id.to_string(),
            block: "Subject",
            message: format!(
                "Subject line does not start with valid conventional prefix (feat, fix, docs, refactor, etc.): '{}'",
                subject
            ),
        });
    }

    // 2. Validate Required 5 Blocks
    let full_text = raw_message;
    for &req_block in &REQUIRED_COMMIT_BLOCKS {
        if !full_text.contains(req_block) {
            issues.push(GitDiagnosticIssue {
                target: target_id.to_string(),
                block: req_block,
                message: format!("Missing required block: '{}'", req_block),
            });
        }
    }

    // 3. Validate Block 1 Fields
    if full_text.contains(REQUIRED_COMMIT_BLOCKS[0]) {
        if !full_text.contains("Project:") {
            issues.push(GitDiagnosticIssue {
                target: target_id.to_string(),
                block: "BLOCK 1",
                message: "Missing 'Project:' field in Block 1 metadata".to_string(),
            });
        }
        if !full_text.contains("Author:") {
            issues.push(GitDiagnosticIssue {
                target: target_id.to_string(),
                block: "BLOCK 1",
                message: "Missing 'Author:' field in Block 1 metadata".to_string(),
            });
        }
        if !full_text.contains("Date:") {
            issues.push(GitDiagnosticIssue {
                target: target_id.to_string(),
                block: "BLOCK 1",
                message: "Missing 'Date:' field in Block 1 metadata".to_string(),
            });
        }
    }

    issues
}

/// Audits an annotated tag message against the 5-block tag manifest protocol.
pub fn audit_tag_message(
    tag_name: &str,
    raw_message: &str,
) -> Vec<GitDiagnosticIssue> {
    let mut issues = Vec::new();

    // 1. Validate Tag Name
    if !tag_name.starts_with('v') {
        issues.push(GitDiagnosticIssue {
            target: tag_name.to_string(),
            block: "TagName",
            message: format!("Tag name must start with 'v' (SemVer convention): '{}'", tag_name),
        });
    }

    // 2. Validate Required Tag Blocks
    let full_text = raw_message;
    for &req_block in &REQUIRED_TAG_BLOCKS {
        if !full_text.contains(req_block) {
            issues.push(GitDiagnosticIssue {
                target: tag_name.to_string(),
                block: req_block,
                message: format!("Missing required tag manifest block: '{}'", req_block),
            });
        }
    }

    // 3. Validate Tag Metadata Annotations
    if full_text.contains(REQUIRED_TAG_BLOCKS[0]) {
        if !full_text.contains("@release") {
            issues.push(GitDiagnosticIssue {
                target: tag_name.to_string(),
                block: "BLOCK 1",
                message: "Missing '@release' declaration in Block 1".to_string(),
            });
        }
        if !full_text.contains("@date") {
            issues.push(GitDiagnosticIssue {
                target: tag_name.to_string(),
                block: "BLOCK 1",
                message: "Missing '@date' declaration in Block 1".to_string(),
            });
        }
    }

    issues
}

/// Audits historical git commits using `git log`.
pub fn audit_git_history(limit: Option<usize>, report: &mut GitAuditReport) {
    let mut cmd = Command::new("git");
    cmd.args(["log", "--format=%H%x1f%s%x1f%B%x1e"]);
    if let Some(n) = limit {
        cmd.arg(format!("-n{}", n));
    }

    let output = match cmd.output() {
        Ok(out) => out,
        Err(e) => {
            eprintln!("[ERROR] Failed to execute git log: {}", e);
            return;
        }
    };

    if !output.status.success() {
        eprintln!("[ERROR] git log returned non-zero status");
        return;
    }

    let stdout = String::from_utf8_lossy(&output.stdout);
    let entries: Vec<&str> = stdout.split('\x1e').filter(|e| !e.trim().is_empty()).collect();

    for entry in entries {
        let parts: Vec<&str> = entry.split('\x1f').collect();
        if parts.len() < 3 {
            continue;
        }
        let hash = &parts[0][..7.min(parts[0].len())];
        let subject = parts[1].trim();
        let body = parts[2];

        report.commits_audited += 1;
        let commit_label = format!("{} | {}", hash, subject);
        let commit_issues = audit_commit_message(&commit_label, body);

        if commit_issues.is_empty() {
            report.commits_passed += 1;
        } else {
            report.issues.extend(commit_issues);
        }
    }
}

/// Audits all git release tags using `git tag` and `git cat-file`.
pub fn audit_git_tags(report: &mut GitAuditReport) {
    let output = match Command::new("git").args(["tag", "-l"]).output() {
        Ok(out) => out,
        Err(e) => {
            eprintln!("[ERROR] Failed to list git tags: {}", e);
            return;
        }
    };

    if !output.status.success() {
        eprintln!("[ERROR] git tag -l returned non-zero status");
        return;
    }

    let stdout = String::from_utf8_lossy(&output.stdout);
    let tag_names: Vec<&str> = stdout.lines().map(|l| l.trim()).filter(|l| !l.is_empty()).collect();

    for tag in tag_names {
        report.tags_audited += 1;
        let tag_out = match Command::new("git").args(["cat-file", "-p", tag]).output() {
            Ok(out) => out,
            Err(_) => continue,
        };
        let tag_content = String::from_utf8_lossy(&tag_out.stdout);

        let tag_issues = audit_tag_message(tag, &tag_content);
        if tag_issues.is_empty() {
            report.tags_passed += 1;
        } else {
            report.issues.extend(tag_issues);
        }
    }
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK / TASK RUNNER IMPLEMENTATION
// Stratum: Developer Tooling | Machine: tools.validator.git_commit
// =============================================================================

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
// BLOCK 5: CLOSING BLOCK / TESTS
// Stratum: Developer Tooling | Machine: tools.validator.git_commit
// =============================================================================

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_valid_5_block_commit_message_passes() {
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

    #[test]
    fn test_missing_block_triggers_diagnostic() {
        let invalid_msg = r#"feat(core): missing block 5

BLOCK 1: METADATA & REVISION RECORD
===================================
Project:        Tenebris and Lux
Author:         CreativeWorkzStudio LLC
Date:           2026-11-07

BLOCK 2: REASON FOR CHANGE & PROBLEM STATEMENT
=============================================
Problem.

BLOCK 3: ARCHITECTURAL MODIFICATIONS & ELEVATIONS
================================================
Mods.

BLOCK 4: VERIFICATION, TRIPWIRES & PASS PROOF
============================================
Tests.
"#;
        let issues = audit_commit_message("HEAD", invalid_msg);
        assert!(!issues.is_empty());
        assert!(issues.iter().any(|i| i.block.contains("BLOCK 5")));
    }

    #[test]
    fn test_invalid_subject_prefix_triggers_diagnostic() {
        let bad_prefix_msg = r#"random commit without conventional prefix

BLOCK 1: METADATA & REVISION RECORD
===================================
Project:        Tenebris and Lux
Author:         CreativeWorkzStudio LLC
Date:           2026-11-07

BLOCK 2: REASON FOR CHANGE & PROBLEM STATEMENT
=============================================
Text.

BLOCK 3: ARCHITECTURAL MODIFICATIONS & ELEVATIONS
================================================
Text.

BLOCK 4: VERIFICATION, TRIPWIRES & PASS PROOF
============================================
Text.

BLOCK 5: INVARIANT COVENANTS & CLOSING AFFIRMATIONS
==================================================
Text.
"#;
        let issues = audit_commit_message("HEAD", bad_prefix_msg);
        assert!(issues.iter().any(|i| i.block == "Subject"));
    }

    #[test]
    fn test_valid_tag_manifest_passes() {
        let valid_tag = r#"object 8541f53
type commit
tag v0.1.0-alpha
tagger CreativeWorkzStudio LLC <author@example.com> 1762548000 -0600

Release v0.1.0-alpha: Test Release

================================================================================
BLOCK 1: METADATA BLOCK
================================================================================
@release       v0.1.0-alpha
@date          2026-11-07
@stratum       Layer 0

================================================================================
BLOCK 2: RELEASE CONTEXT & ONTOLOGY
================================================================================
Release context.

================================================================================
BLOCK 3: ARCHITECTURAL MODIFICATIONS & ELEVATIONS
================================================================================
Modifications.

================================================================================
BLOCK 4: AUDIT PROCEDURES & VERIFICATION PROOFS
================================================================================
Audits.

================================================================================
BLOCK 5: CLOSING & CANONICAL ARTIFACT LINKS
================================================================================
Closing.
"#;
        let issues = audit_tag_message("v0.1.0-alpha", valid_tag);
        assert!(issues.is_empty(), "Expected clean tag audit, got: {:?}", issues);
    }
}
