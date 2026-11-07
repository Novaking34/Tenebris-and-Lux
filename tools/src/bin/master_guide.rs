// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/bin/master_guide.rs
// @id            tools.bin.master_guide
// @layer         Layer 3: Systems Developer Tooling Standalone Binary
// @version       0.1.0-dev
// @interface_rev Rev 4
// @date          2026-11-07
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        Data-Driven Master Guide & Multi-Volume PDF Publisher
// @role          Standalone Master Guide & Multi-Volume Publisher CLI
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Standalone compiler for the unified Tenebris and Lux Master Guide and its
//   individual volumes. Leverages `tl_tools::guide::MasterGuideBuilder` to assemble
//   documentation into unified master compendiums and volume-specific books in
//   AsciiDoc (.adoc), Markdown (.md), and compiled publication PDFs (.pdf).
//
// PEDAGOGICAL MISSION:
//   Demonstrates modular CLI architecture with dual-scope publishing options:
//     --all:       Builds unified master guide AND all volume publications.
//     --master:    Builds the unified single-file compendium only.
//     --volumes:   Builds all individual volume books only in docs/volumes/.
//     --volume <N>: Builds a single specified volume book.
//     --scan:      Scans filesystem areas and reports discovered chapters.
//
// AXIOMATIC INVARIANTS:
//   1. Strict exit code semantics (0 = success, 1 = failure).
//   2. Zero missing chapter tolerance during pre-flight audit.
//   3. High-fidelity Markdown translation preserving callouts and code fences.
//   4. Strict <= 80-column line width discipline across all documentation.
// =============================================================================

//! # Standalone Master Guide & Volume Publisher Binary
//!
//! Provides a dedicated CLI binary for building the unified Master Guide
//! and individual volume publications across AsciiDoc, Markdown, and PDF
//! using [`tl_tools::guide`].

// =============================================================================
// BLOCK 2: OPENING BLOCK / IMPORTS & CLI CONFIGURATION
// Stratum: Developer Tooling | Machine: tools.bin.master_guide
// =============================================================================

use std::env;
use std::path::{Path, PathBuf};
use std::process::ExitCode;
use tl_tools::guide::MasterGuideBuilder;

/// Publishing mode selected by command-line arguments.
#[derive(Debug, Clone, PartialEq, Eq)]
enum BuildMode {
    All,
    MasterOnly,
    VolumesOnly,
    SingleVolume(String),
    ScanOnly,
    Help,
}

// =============================================================================
// BLOCK 3: BODY BLOCK / COMPILER INVOCATION WORKFLOWS
// Stratum: Developer Tooling | Machine: tools.bin.master_guide
// =============================================================================

/// Resolves the repository root dynamically by locating CMakeLists.txt or docs/.
fn resolve_repo_root() -> PathBuf {
    if let Ok(cwd) = env::current_dir() {
        let mut curr = cwd.clone();
        loop {
            if curr.join("CMakeLists.txt").is_file() && curr.join("docs").is_dir() {
                return curr;
            }
            if !curr.pop() {
                break;
            }
        }
    }
    PathBuf::from("/home/professorseanex/Tenebris and Lux")
}

/// Executes the Master Guide compilation workflow based on the chosen mode.
fn run_compilation(repo_root: &Path, mode: &BuildMode) -> Result<(), String> {
    println!("============================================================");
    println!("  Tenebris and Lux - Master Guide & Volume PDF Publisher    ");
    println!("============================================================");
    println!("Repository Root: {:?}", repo_root);

    let builder = MasterGuideBuilder::new(repo_root);

    match mode {
        BuildMode::All => {
            println!("Target Mode: ALL (Unified Compendium + All Volume Books)");
            let (adoc, md, pdf) = builder.build_master()?;
            println!("------------------------------------------------------------");
            println!("Unified Master Compendium Artifacts:");
            println!("  AsciiDoc: {:?}", adoc);
            println!("  Markdown: {:?}", md);
            println!("  PDF Book: {:?}", pdf);

            let vol_results = builder.build_volumes()?;
            println!("------------------------------------------------------------");
            println!("Volume Compendiums Generated ({} volumes):", vol_results.len());
            for (roman, va, vm, vp) in vol_results {
                println!("  Volume {:4} -> PDF: {:?}", roman, vp);
                println!("                ADOC: {:?}", va);
                println!("                MD:   {:?}", vm);
            }
        }
        BuildMode::MasterOnly => {
            println!("Target Mode: MASTER COMPENDIUM ONLY");
            let (adoc, md, pdf) = builder.build_master()?;
            println!("------------------------------------------------------------");
            println!("Master Guide Artifacts:");
            println!("  AsciiDoc: {:?}", adoc);
            println!("  Markdown: {:?}", md);
            println!("  PDF Book: {:?}", pdf);
        }
        BuildMode::VolumesOnly => {
            println!("Target Mode: VOLUME BOOKS ONLY");
            let vol_results = builder.build_volumes()?;
            println!("------------------------------------------------------------");
            println!("Volume Books Generated ({} volumes):", vol_results.len());
            for (roman, _va, _vm, vp) in vol_results {
                println!("  Volume {:4} -> {:?}", roman, vp);
            }
        }
        BuildMode::SingleVolume(identifier) => {
            println!("Target Mode: SINGLE VOLUME ({})", identifier);
            let (adoc, md, pdf) = builder.build_single_volume(identifier)?;
            println!("------------------------------------------------------------");
            println!("Volume Artifacts:");
            println!("  AsciiDoc: {:?}", adoc);
            println!("  Markdown: {:?}", md);
            println!("  PDF Book: {:?}", pdf);
        }
        BuildMode::ScanOnly => {
            println!("Target Mode: DYNAMIC AREA SCAN ONLY");
            println!("Scanning designated documentation areas in docs/...");
            let mut total_scanned = 0;
            for vol in &builder.volumes {
                println!("\nVolume {}: {} (Areas: {:?})", vol.roman, vol.title, vol.scan_areas);
                println!("  Registered Chapters ({} total):", vol.chapters.len());
                for ch in &vol.chapters {
                    let exists_marker = if ch.source_path.exists() { "[EXISTS]" } else { "[MISSING]" };
                    println!("    {:9} Ch #{:2}: {} ({:?})", exists_marker, ch.number, ch.title, ch.source_path);
                    total_scanned += 1;
                }
            }
            println!("\nTotal Documented Chapters: {}", total_scanned);
        }
        BuildMode::Help => {
            print_usage();
        }
    }

    println!("============================================================");
    Ok(())
}

fn print_usage() {
    println!("Usage: master_guide [OPTIONS]");
    println!();
    println!("Options:");
    println!("  --all             Build unified compendium AND all volume books (default)");
    println!("  --master          Build unified master compendium only (MASTER_GUIDE.*)");
    println!("  --volumes         Build individual volume books only in docs/volumes/");
    println!("  --volume <ROMAN>  Build a specific volume (e.g. --volume I, --volume VI)");
    println!("  --scan            Scan and print all chapters across documentation areas");
    println!("  --help, -h        Print this help menu");
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK / PRE-FLIGHT VERIFICATION
// Stratum: Developer Tooling | Machine: tools.bin.master_guide
// =============================================================================

/// Performs a diagnostic audit of all chapter source files before compilation.
fn audit_chapters(repo_root: &Path) -> Result<usize, String> {
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
            "  [PASS] Preflight Chapter Audit: all {} chapters present across {} volumes.",
            total,
            builder.volumes.len()
        );
        Ok(total)
    }
}

// =============================================================================
// BLOCK 5: CLOSING / CLI ENTRY BLOCK
// Stratum: Developer Tooling | Machine: tools.bin.master_guide
// =============================================================================

fn main() -> ExitCode {
    let repo_root = resolve_repo_root();
    let args: Vec<String> = env::args().collect();

    let mut mode = BuildMode::All;

    let mut i = 1;
    while i < args.len() {
        match args[i].as_str() {
            "--all" => mode = BuildMode::All,
            "--master" => mode = BuildMode::MasterOnly,
            "--volumes" => mode = BuildMode::VolumesOnly,
            "--scan" => mode = BuildMode::ScanOnly,
            "--help" | "-h" => mode = BuildMode::Help,
            "--volume" => {
                if i + 1 < args.len() {
                    mode = BuildMode::SingleVolume(args[i + 1].clone());
                    i += 1;
                } else {
                    eprintln!("Error: --volume requires a Roman numeral argument (e.g. --volume I)");
                    return ExitCode::FAILURE;
                }
            }
            unknown => {
                eprintln!("Unknown option: {}", unknown);
                print_usage();
                return ExitCode::FAILURE;
            }
        }
        i += 1;
    }

    if mode == BuildMode::Help {
        print_usage();
        return ExitCode::SUCCESS;
    }

    if mode != BuildMode::ScanOnly {
        if let Err(e) = audit_chapters(&repo_root) {
            eprintln!("Audit Error: {}", e);
            return ExitCode::FAILURE;
        }
    }

    match run_compilation(&repo_root, &mode) {
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
