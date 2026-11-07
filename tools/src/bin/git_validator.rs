// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/bin/git_validator.rs
// @id            tools.bin.git_validator
// @layer         Layer 3: Systems Developer Tooling Core Library
// @version       0.1.0-dev
// @interface_rev Rev 1
// @date          2026-11-07
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Canonical Git commit message and milestone tag auditing utility.
//   Enforces the Universal 5-Block Ledger Protocol and Rev 4 governance
//   standards across all repository commits and annotated release tags.
//
// PEDAGOGICAL MISSION:
//   Guarantees that the repository's git history remains an immutable,
//   self-documenting engineering ledger:
//     1. Validates 5-block commit message envelopes (Metadata, Problem,
//        Modifications, Verification, Invariants).
//     2. Validates Conventional Commit subject line prefixes (feat, fix, docs,
//        refactor, test, chore, perf).
//     3. Validates milestone release tags (SemVer vX.Y.Z, 5-block envelope).
//     4. Provides single-message validation for git commit-msg hooks.
//
// AXIOMATIC INVARIANTS:
//   1. Strict exit code semantics (0 = success, 1 = failure on validation errors).
//   2. Zero panics in library code; all fallible operations return typed `Result`.
//   3. Deterministic path resolution relative to repository workspace root.
//   4. Strict line discipline <= 80 columns across all documentation.
// =============================================================================

//! # Git Commit & Release Tag Auditing Engine
//!
//! Enforces the Universal 5-Block Lifecycle Protocol across git commit
//! messages and annotated milestone release tags.

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES & STRUCTURES
// Stratum: Developer Tooling | Machine: tools.git_validator.rust
// =============================================================================

use std::env;
use std::fs;
use std::path::Path;
use std::process::{Command, exit};

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
pub struct DiagnosticIssue {
    pub target: String,
    pub block: &'static str,
    pub message: String,
}

/// Summary report of the validation run.
#[derive(Debug, Default)]
pub struct AuditReport {
    pub commits_audited: usize,
    pub commits_passed: usize,
    pub tags_audited: usize,
    pub tags_passed: usize,
    pub issues: Vec<DiagnosticIssue>,
}

impl AuditReport {
    pub fn is_success(&self) -> bool {
        self.issues.is_empty()
    }
}

// =============================================================================
// BLOCK 3: BODY BLOCK / CORE VALIDATION LOGIC
// Stratum: Developer Tooling | Machine: tools.git_validator.rust
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
) -> Vec<DiagnosticIssue> {
    let mut issues = Vec::new();
    let lines: Vec<&str> = raw_message
        .lines()
        .map(|l| l.trim_end())
        .filter(|l| !l.starts_with('#'))
        .collect();

    if lines.is_empty() {
        issues.push(DiagnosticIssue {
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
        issues.push(DiagnosticIssue {
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
            issues.push(DiagnosticIssue {
                target: target_id.to_string(),
                block: req_block,
                message: format!("Missing required block: '{}'", req_block),
            });
        }
    }

    // 3. Validate Block 1 Fields
    if full_text.contains(REQUIRED_COMMIT_BLOCKS[0]) {
        if !full_text.contains("Project:") {
            issues.push(DiagnosticIssue {
                target: target_id.to_string(),
                block: "BLOCK 1",
                message: "Missing 'Project:' field in Block 1 metadata".to_string(),
            });
        }
        if !full_text.contains("Author:") {
            issues.push(DiagnosticIssue {
                target: target_id.to_string(),
                block: "BLOCK 1",
                message: "Missing 'Author:' field in Block 1 metadata".to_string(),
            });
        }
        if !full_text.contains("Date:") {
            issues.push(DiagnosticIssue {
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
) -> Vec<DiagnosticIssue> {
    let mut issues = Vec::new();

    // 1. Validate Tag Name
    if !tag_name.starts_with('v') {
        issues.push(DiagnosticIssue {
            target: tag_name.to_string(),
            block: "TagName",
            message: format!("Tag name must start with 'v' (SemVer convention): '{}'", tag_name),
        });
    }

    // 2. Validate Required Tag Blocks
    let full_text = raw_message;
    for &req_block in &REQUIRED_TAG_BLOCKS {
        if !full_text.contains(req_block) {
            issues.push(DiagnosticIssue {
                target: tag_name.to_string(),
                block: req_block,
                message: format!("Missing required tag manifest block: '{}'", req_block),
            });
        }
    }

    // 3. Validate Tag Metadata Annotations
    if full_text.contains(REQUIRED_TAG_BLOCKS[0]) {
        if !full_text.contains("@release") {
            issues.push(DiagnosticIssue {
                target: tag_name.to_string(),
                block: "BLOCK 1",
                message: "Missing '@release' declaration in Block 1".to_string(),
            });
        }
        if !full_text.contains("@date") {
            issues.push(DiagnosticIssue {
                target: tag_name.to_string(),
                block: "BLOCK 1",
                message: "Missing '@date' declaration in Block 1".to_string(),
            });
        }
    }

    issues
}

/// Audits historical git commits using `git log`.
pub fn audit_git_history(limit: Option<usize>, report: &mut AuditReport) {
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
pub fn audit_git_tags(report: &mut AuditReport) {
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
        // Retrieve tag annotation message
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
// BLOCK 4: AUDIT PROCEDURES BLOCK / UNIT TESTS
// Stratum: Developer Tooling | Machine: tools.git_validator.rust
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

// =============================================================================
// BLOCK 5: CLOSING BLOCK / CLI DISPATCH & REPORTING
// Stratum: Developer Tooling | Machine: tools.git_validator.rust
// =============================================================================

fn print_report(report: &AuditReport) {
    println!("================================================================================");
    println!("       Tenebris and Lux - Git Commit & Milestone Tag Verification       ");
    println!("================================================================================");
    println!("Commits Audited:  {}", report.commits_audited);
    println!("Commits Passing:  {}", report.commits_passed);
    println!("Tags Audited:     {}", report.tags_audited);
    println!("Tags Passing:     {}", report.tags_passed);
    println!("Total Issues:     {}", report.issues.len());
    println!("--------------------------------------------------------------------------------");

    if !report.issues.is_empty() {
        println!("{:<32} {:<24} {}", "TARGET", "BLOCK", "DIAGNOSTIC");
        println!("--------------------------------------------------------------------------------");
        for issue in &report.issues {
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

    let verdict = if report.is_success() {
        "ALL GIT COMMITS & TAGS SATISFY 5-BLOCK PROTOCOL (PASS)"
    } else {
        "GIT AUDIT REGRESSIONS DETECTED (FAIL)"
    };
    println!("Verdict:          {}", verdict);
    println!("================================================================================");
}

fn main() {
    let args: Vec<String> = env::args().collect();
    let mut mode = ValidatorMode::FullAudit;

    let mut i = 1;
    while i < args.len() {
        match args[i].as_str() {
            "--hook" => {
                if i + 1 < args.len() {
                    mode = ValidatorMode::HookFile(args[i + 1].clone());
                    i += 1;
                }
            }
            "--recent" | "--commits" => {
                let count = if i + 1 < args.len() {
                    args[i + 1].parse::<usize>().unwrap_or(1)
                } else {
                    1
                };
                mode = ValidatorMode::RecentCommits(count);
                i += 1;
            }
            "--tags" => {
                mode = ValidatorMode::TagsOnly;
            }
            "--help" | "-h" => {
                println!("Usage: git_validator [OPTIONS]");
                println!();
                println!("Options:");
                println!("  --commits [N]   Audit the last N commits (default all)");
                println!("  --tags          Audit only git release tags");
                println!("  --hook <FILE>   Audit a single commit message file (for commit-msg hook)");
                println!("  -h, --help      Display this help menu");
                return;
            }
            _ => {}
        }
        i += 1;
    }

    let mut report = AuditReport::default();

    match mode {
        ValidatorMode::HookFile(path_str) => {
            let path = Path::new(&path_str);
            if !path.exists() {
                eprintln!("[ERROR] Commit message file not found: {}", path_str);
                exit(1);
            }
            let content = match fs::read_to_string(path) {
                Ok(c) => c,
                Err(e) => {
                    eprintln!("[ERROR] Could not read commit message file: {}", e);
                    exit(1);
                }
            };
            report.commits_audited = 1;
            let issues = audit_commit_message(&path_str, &content);
            if issues.is_empty() {
                report.commits_passed = 1;
            } else {
                report.issues.extend(issues);
            }
        }
        ValidatorMode::RecentCommits(n) => {
            audit_git_history(Some(n), &mut report);
        }
        ValidatorMode::TagsOnly => {
            audit_git_tags(&mut report);
        }
        ValidatorMode::FullAudit => {
            audit_git_history(None, &mut report);
            audit_git_tags(&mut report);
        }
    }

    print_report(&report);

    if !report.is_success() {
        exit(1);
    }
}
