// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/bin/git_validator.rs
// @id            tools.bin.git_validator
// @layer         Layer 3: Systems Developer Tooling Standalone Binary
// @version       0.1.0-dev
// @interface_rev Rev 1
// @date          2026-11-07
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Canonical Git commit message and milestone tag auditing CLI shim.
//   Delegates to the modular `tl_tools::validator::git_commit` engine.
// =============================================================================

//! # Git Commit & Release Tag Auditing CLI
//!
//! Enforces the Universal 5-Block Lifecycle Protocol across git commit
//! messages and annotated milestone release tags.

// =============================================================================
// BLOCK 2: OPENING BLOCK / IMPORTS & CLI CONFIGURATION
// Stratum: Developer Tooling | Machine: tools.bin.git_validator
// =============================================================================

use std::env;
use std::fs;
use std::path::Path;
use std::process::exit;

use tl_tools::validator::{
    audit_commit_message, audit_git_history, audit_git_tags, GitAuditReport, ValidatorMode,
};

// =============================================================================
// BLOCK 3: BODY BLOCK / CLI DISPATCH
// Stratum: Developer Tooling | Machine: tools.bin.git_validator
// =============================================================================

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

    let mut report = GitAuditReport::default();

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

    report.print_report();

    if !report.is_success() {
        exit(1);
    }
}
