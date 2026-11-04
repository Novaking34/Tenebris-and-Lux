// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/bin/demo_extractor.rs
// @id            tools.bin.demo_extractor
// @layer         Layer 3: Systems Developer Tooling Standalone Binary
// @version       0.1.0
// @interface_rev Rev 1
// @date          2026-10-29
// @updated       2026-10-29
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Dedicated binary target for standalone historical RPG Maker MV demo data
//   extraction. Leverages the core `tl_tools::extractor` library to inspect,
//   deserialize, and validate historical 2017–2018 demo assets without
//   requiring the full multi-tool CLI dispatcher.
//
// PEDAGOGICAL MISSION:
//   Demonstrates how standalone binaries in a modular Rust crate can serve
//   as thin execution harnesses over a shared core (`tl_tools::extractor`).
//   Preserves exact behavioral parity with the CLI runner while allowing direct
//   invocation in specialized automated scripts or CI/CD pipelines.
//
// AXIOMATIC INVARIANTS:
//   1. Strict exit code semantics (0 = success, 1 = failure).
//   2. Fail-fast error reporting if external archives are unreachable.
//   3. Zero mutation of source archival JSON datasets.
//   4. Strict <= 80-column line width discipline across all documentation.
// =============================================================================

//! # Standalone Demo Extractor Binary
//!
//! Provides a dedicated CLI binary entry point for extracting and verifying
//! the 2017–2018 RPG Maker MV demo datasets using the [`tl_tools::extractor`]
//! library module.

// =============================================================================
// BLOCK 2: OPENING BLOCK / IMPORTS & TYPES
// Stratum: Developer Tooling | Machine: tools.bin.demo_extractor
// =============================================================================

use std::path::{Path, PathBuf};
use std::process::ExitCode;
use tl_tools::extractor::DemoExtractor;

// =============================================================================
// BLOCK 3: BODY BLOCK / EXECUTION LOGIC
// Stratum: Developer Tooling | Machine: tools.bin.demo_extractor
// =============================================================================

/// Executes the standalone extraction workflow.
fn run_standalone_extractor(
    archive_base: &Path,
    output_dir: &Path,
) -> Result<(), String> {
    println!("============================================================");
    println!("     Tenebris and Lux - Standalone Demo Extractor Binary    ");
    println!("============================================================");
    println!("Archive Base: {:?}", archive_base);
    println!("Output Dir:   {:?}", output_dir);

    let extractor = DemoExtractor::new(archive_base, output_dir);
    let summary = extractor.run()?;

    println!("------------------------------------------------------------");
    println!("Extraction Summary:");
    println!("  Actors:   {}", summary.actor_count);
    println!("  Classes:  {}", summary.class_count);
    println!("  Skills:   {}", summary.skill_count);
    println!("  Items:    {}", summary.item_count);
    println!("  Weapons:  {}", summary.weapon_count);
    println!("  Armors:   {}", summary.armor_count);
    println!("  Enemies:  {}", summary.enemy_count);
    println!("  Troops:   {}", summary.troop_count);
    println!("============================================================");
    Ok(())
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK / PREFLIGHT AUDIT
// Stratum: Developer Tooling | Machine: tools.bin.demo_extractor
// =============================================================================

/// Validates that the external archive directory exists and contains data.
fn audit_environment(archive_base: &Path) -> Result<(), String> {
    if !archive_base.is_dir() {
        return Err(format!(
            "External archive directory not found at: {:?}",
            archive_base
        ));
    }
    let f1 = archive_base.join("Old Game Data/data");
    let f2 = archive_base.join("Old_Game_Data_2/data");
    if !f1.is_dir() && !f2.is_dir() {
        return Err(format!(
            "Neither folder 1 nor folder 2 found inside: {:?}",
            archive_base
        ));
    }
    Ok(())
}

// =============================================================================
// BLOCK 5: CLOSING / CLI ENTRY BLOCK
// Stratum: Developer Tooling | Machine: tools.bin.demo_extractor
// =============================================================================

fn main() -> ExitCode {
    let archive_base = PathBuf::from(
        "/home/professorseanex/Tenebris_Lux_Old_Game_Data_Archive",
    );
    let output_dir = PathBuf::from(
        "/home/professorseanex/Tenebris and Lux/docs/game_design/demo_baseline",
    );

    if let Err(e) = audit_environment(&archive_base) {
        eprintln!("Preflight Audit Error: {}", e);
        return ExitCode::FAILURE;
    }

    match run_standalone_extractor(&archive_base, &output_dir) {
        Ok(()) => {
            println!("Demo Extractor completed successfully.");
            ExitCode::SUCCESS
        }
        Err(err) => {
            eprintln!("Extraction Execution Error: {}", err);
            ExitCode::FAILURE
        }
    }
}
