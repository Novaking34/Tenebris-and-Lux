// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/bin/master_guide.rs
// @id            tools.bin.master_guide
// @layer         Layer 3: Systems Developer Tooling Standalone Binary
// @version       0.1.0
// @interface_rev Rev 1
// @date          2026-10-29
// @updated       2026-10-29
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Standalone compiler for the unified Tenebris and Lux Master Guide.
//   Leverages `tl_tools::guide::MasterGuideBuilder` to assemble 6 volumes and
//   42 chapters of technical documentation into MASTER_GUIDE.adoc,
//   MASTER_GUIDE.md (optimized for NotebookLM and Google Gemini), and compiles
//   MASTER_GUIDE.pdf via `asciidoctor-pdf`.
//
// PEDAGOGICAL MISSION:
//   Demonstrates modular separation in Rust developer tooling: the underlying
//   compilation logic lives in the reusable library (`tl_tools::guide`), while
//   this standalone binary provides a focused, single-purpose CLI executable
//   for documentation publishers and CI/CD automation pipelines.
//
// AXIOMATIC INVARIANTS:
//   1. Strict exit code semantics (0 = success, 1 = failure).
//   2. Zero missing chapter tolerance during pre-flight audit.
//   3. High-fidelity Markdown translation preserving code fences and callouts.
//   4. Strict <= 80-column line width discipline across all documentation.
// =============================================================================

//! # Standalone Master Guide Compiler Binary
//!
//! Provides a dedicated CLI binary for building the unified Master Guide
//! across AsciiDoc, Markdown, and PDF formats using [`tl_tools::guide`].

// =============================================================================
// BLOCK 2: OPENING BLOCK / IMPORTS & CONFIGURATION
// Stratum: Developer Tooling | Machine: tools.bin.master_guide
// =============================================================================

use std::path::PathBuf;
use std::process::ExitCode;
use tl_tools::guide::MasterGuideBuilder;

// =============================================================================
// BLOCK 3: BODY BLOCK / COMPILER INVOCATION
// Stratum: Developer Tooling | Machine: tools.bin.master_guide
// =============================================================================

/// Executes the standalone Master Guide compilation workflow.
fn run_master_guide_compilation(repo_root: &PathBuf) -> Result<(), String> {
    println!("============================================================");
    println!("    Tenebris and Lux - Standalone Master Guide Compiler     ");
    println!("============================================================");
    println!("Repository Root: {:?}", repo_root);

    let builder = MasterGuideBuilder::new(repo_root);
    let (adoc, md, pdf) = builder.build_all()?;

    println!("------------------------------------------------------------");
    println!("Master Guide Artifacts:");
    println!("  AsciiDoc: {:?}", adoc);
    println!("  Markdown: {:?}", md);
    println!("  PDF Book: {:?}", pdf);
    println!("============================================================");
    Ok(())
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK / CHAPTER AUDIT
// Stratum: Developer Tooling | Machine: tools.bin.master_guide
// =============================================================================

/// Performs a diagnostic audit of all chapter source files before compilation.
fn audit_chapters(repo_root: &PathBuf) -> Result<usize, String> {
    let builder = MasterGuideBuilder::new(repo_root);
    let mut total = 0;
    let mut missing = 0;

    for vol in &builder.volumes {
        for ch in &vol.chapters {
            total += 1;
            if !ch.source_path.exists() {
                eprintln!("  [MISSING CHAPTER] {:?}", ch.source_path);
                missing += 1;
            }
        }
    }

    if missing > 0 {
        Err(format!(
            "Chapter audit failed: {}/{} chapters missing on disk.",
            missing, total
        ))
    } else {
        println!(
            "  [PASS] Preflight Chapter Audit: all {} chapters present.",
            total
        );
        Ok(total)
    }
}

// =============================================================================
// BLOCK 5: CLOSING / CLI ENTRY BLOCK
// Stratum: Developer Tooling | Machine: tools.bin.master_guide
// =============================================================================

fn main() -> ExitCode {
    let repo_root = PathBuf::from("/home/professorseanex/Tenebris and Lux");

    if let Err(e) = audit_chapters(&repo_root) {
        eprintln!("Audit Error: {}", e);
        return ExitCode::FAILURE;
    }

    match run_master_guide_compilation(&repo_root) {
        Ok(()) => {
            println!("Master Guide Compiler completed successfully.");
            ExitCode::SUCCESS
        }
        Err(err) => {
            eprintln!("Compilation Error: {}", err);
            ExitCode::FAILURE
        }
    }
}
