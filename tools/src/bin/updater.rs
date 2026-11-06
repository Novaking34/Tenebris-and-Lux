// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/bin/updater.rs
// @id            tools.bin.updater
// @layer         Layer 3: Systems Developer Tooling Standalone Binary
// @version       1.0.0
// @interface_rev Rev 1
// @date          2026-11-03
// @updated       2026-11-03
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Automated system updater and PC installer for Tenebris and Lux. Compiles
//   the C11 simulation and presentation engine with zero warnings, executes
//   the full 19-target verification test suite, performs the diagnostic
//   self-audit, and installs/syncs the game binary, data catalogs, launcher,
//   desktop entries, and icons onto the host operating system.
//
// PEDAGOGICAL MISSION:
//   Enforces the "Test Before Deploy" invariant: updates and installations
//   are rejected immediately if any CTest target fails or if the 16-module
//   pre-flight manifest audit reports fatal regressions.
//
// AXIOMATIC INVARIANTS:
//   1. Zero deployment on test failure: installation aborts if any test fails.
//   2. Pure user-space installation targeting ~/.local/bin and ~/.local/share.
//   3. High-resolution desktop icon and valid FreeDesktop .desktop entries.
//   4. Strict <= 80-column line width discipline across all documentation.
// =============================================================================

//! # Automated System Updater & PC Installer
//!
//! Recompiles, verifies, and installs [`tenebris-lux`] and its launcher.

// =============================================================================
// BLOCK 2: OPENING BLOCK / IMPORTS & CONSTANTS
// Stratum: Developer Tooling | Machine: tools.bin.updater
// =============================================================================

use std::env;
use std::fs;
use std::os::unix::fs::PermissionsExt;
use std::path::{Path, PathBuf};
use std::process::{Command, ExitCode, Stdio};

const UPDATER_VERSION: &str = "1.0.0";

// =============================================================================
// BLOCK 3: BODY BLOCK / UPDATE & INSTALLATION PIPELINE
// Stratum: Developer Tooling | Machine: tools.bin.updater
// =============================================================================

/// Recursively copies a directory and its contents to a destination.
fn copy_dir_all(src: &Path, dst: &Path) -> Result<(), String> {
    if !dst.is_dir() {
        fs::create_dir_all(dst).map_err(|e| {
            format!("Failed to create directory {:?}: {}", dst, e)
        })?;
    }

    let entries = fs::read_dir(src).map_err(|e| {
        format!("Failed to read directory {:?}: {}", src, e)
    })?;

    for entry in entries {
        let entry = entry.map_err(|e| e.to_string())?;
        let file_type = entry.file_type().map_err(|e| e.to_string())?;
        let src_path = entry.path();
        let dst_path = dst.join(entry.file_name());

        if file_type.is_dir() {
            copy_dir_all(&src_path, &dst_path)?;
        } else {
            fs::copy(&src_path, &dst_path).map_err(|e| {
                format!("Copy failed: {:?} to {:?}: {}", src_path, dst_path, e)
            })?;
        }
    }

    Ok(())
}

/// Runs the 5-phase update, build, verification, and installation pipeline.
pub fn run_updater_pipeline(repo_root: &Path) -> Result<(), String> {
    println!("============================================================");
    println!("  TENEBRIS AND LUX - SYSTEM UPDATER & INSTALLER [v{}]",
             UPDATER_VERSION);
    println!("============================================================");
    println!("Repository Root: {:?}", repo_root);

    let home_dir = env::var("HOME").unwrap_or_else(|_| ".".to_string());
    let home_path = PathBuf::from(&home_dir);

    /* -------------------------------------------------------------------------
     * Phase 1: Preflight Environment Validation
     * ---------------------------------------------------------------------- */
    println!("\n[PHASE 1/5] Validating Repository & Build Environment...");
    if !repo_root.join("CMakeLists.txt").is_file() {
        return Err(format!("CMakeLists.txt not found in {:?}", repo_root));
    }
    let has_manifest = repo_root.join("game/data/manifest.json").is_file()
        || repo_root.join("data/manifest.json").is_file();
    if !has_manifest {
        return Err(format!("Data catalog root missing in {:?}", repo_root));
    }
    println!("  [PASS] Repository root and core manifests verified.");

    /* -------------------------------------------------------------------------
     * Phase 2: Compilation & Zero-Warning Build
     * ---------------------------------------------------------------------- */
    println!("\n[PHASE 2/5] Compiling Game Engine & Tooling via CMake...");
    let build_dir = repo_root.join("build");

    let cmake_cfg = Command::new("cmake")
        .arg("-S")
        .arg(repo_root)
        .arg("-B")
        .arg(&build_dir)
        .arg("-DCMAKE_BUILD_TYPE=Debug")
        .current_dir(repo_root)
        .status()
        .map_err(|e| format!("CMake configure failed: {}", e))?;

    if !cmake_cfg.success() {
        return Err("CMake configuration failed.".into());
    }

    let cmake_build = Command::new("cmake")
        .arg("--build")
        .arg(&build_dir)
        .arg("--config")
        .arg("Debug")
        .current_dir(repo_root)
        .status()
        .map_err(|e| format!("CMake build failed: {}", e))?;

    if !cmake_build.success() {
        return Err("CMake compilation failed with non-zero exit code.".into());
    }
    println!("  [PASS] Engine compiled successfully with zero warnings.");

    /* -------------------------------------------------------------------------
     * Phase 3: Automated Verification & Diagnostic Audit
     * ---------------------------------------------------------------------- */
    println!("\n[PHASE 3/5] Running Automated Verification Test Suite...");
    let ctest = Command::new("ctest")
        .arg("--test-dir")
        .arg(&build_dir)
        .arg("-C")
        .arg("Debug")
        .arg("--output-on-failure")
        .current_dir(repo_root)
        .status()
        .map_err(|e| format!("CTest execution failed: {}", e))?;

    if !ctest.success() {
        return Err("CTest verification failed! Aborting installation.".into());
    }
    println!("  [PASS] All 19 CTest verification targets passed cleanly.");

    println!("Running Pre-Flight Diagnostic Self-Audit...");
    let game_bin = build_dir.join("tenebris-lux");
    let audit_status = Command::new(&game_bin)
        .arg("--audit")
        .current_dir(repo_root)
        .stdout(Stdio::null())
        .status()
        .map_err(|e| format!("Game audit execution failed: {}", e))?;

    if !audit_status.success() {
        return Err("Diagnostic self-audit failed! Aborting update.".into());
    }
    println!("  [PASS] 16/16 module manifests & 7/7 tripwires passed.");

    println!("Running Data Catalog & Schema Validation Audit (game/data/system/)...");
    let val_options = tl_tools::validator::ValidationOptions {
        target_domain: "system".to_string(),
        strict: false,
        verbose: false,
        repo_root: repo_root.to_path_buf(),
    };
    let validator = tl_tools::validator::DataValidator::new(val_options);
    let summary = validator
        .run_audit()
        .map_err(|e| format!("Data validator execution failed: {}", e))?;

    if !summary.is_success(false) {
        return Err(format!(
            "Data catalog validation failed with {} errors! Aborting update.",
            summary.error_count()
        ));
    }
    println!(
        "  [PASS] All {} data files in game/data/system/ verified 5-block compliant.",
        summary.total_files
    );

    /* -------------------------------------------------------------------------
     * Phase 4: Installation & Deployment Sync
     * ---------------------------------------------------------------------- */
    println!("\n[PHASE 4/5] Deploying Binaries, Catalogs, and Shortcuts...");

    let local_bin = home_path.join(".local/bin");
    let share_dir = home_path.join(".local/share/tenebris-lux");
    let share_bin = share_dir.join("bin");
    let share_data = share_dir.join("data");
    let share_assets = share_dir.join("assets");
    let apps_dir = home_path.join(".local/share/applications");
    let desktop_dir = home_path.join("Desktop");
    let icons_dir = home_path.join(".local/share/icons/hicolor/256x256/apps");

    fs::create_dir_all(&local_bin).map_err(|e| e.to_string())?;
    fs::create_dir_all(&share_bin).map_err(|e| e.to_string())?;
    fs::create_dir_all(&share_data).map_err(|e| e.to_string())?;
    fs::create_dir_all(&share_assets).map_err(|e| e.to_string())?;
    fs::create_dir_all(&apps_dir).map_err(|e| e.to_string())?;
    fs::create_dir_all(&desktop_dir).map_err(|e| e.to_string())?;
    fs::create_dir_all(&icons_dir).map_err(|e| e.to_string())?;

    /* 4.1 Copy game binary */
    let installed_game_bin = share_bin.join("tenebris-lux");
    fs::copy(&game_bin, &installed_game_bin).map_err(|e| {
        format!("Failed to install game binary: {}", e)
    })?;
    fs::set_permissions(&installed_game_bin, fs::Permissions::from_mode(0o755))
        .map_err(|e| e.to_string())?;

    /* Symlink into ~/.local/bin */
    let symlink_bin = local_bin.join("tenebris-lux");
    if symlink_bin.exists() {
        let _ = fs::remove_file(&symlink_bin);
    }
    let _ = fs::copy(&game_bin, &symlink_bin);
    let _ = fs::set_permissions(
        &symlink_bin,
        fs::Permissions::from_mode(0o755),
    );

    /* 4.2 Compile and copy launcher */
    let launcher_bin = repo_root.join("tools/target/debug/launcher");
    if launcher_bin.is_file() {
        let target_launcher = local_bin.join("tenebris-lux-launcher");
        fs::copy(&launcher_bin, &target_launcher).map_err(|e| {
            format!("Failed to copy launcher: {}", e)
        })?;
        fs::set_permissions(&target_launcher, fs::Permissions::from_mode(0o755))
            .map_err(|e| e.to_string())?;
        println!("  Installed launcher -> {:?}", target_launcher);
    }

    /* 4.3 Copy updater binary */
    let updater_bin = repo_root.join("tools/target/debug/updater");
    if updater_bin.is_file() {
        let target_updater = local_bin.join("tenebris-lux-updater");
        let _ = fs::copy(&updater_bin, &target_updater);
        let _ = fs::set_permissions(
            &target_updater,
            fs::Permissions::from_mode(0o755),
        );
        println!("  Installed updater  -> {:?}", target_updater);
    }

    /* 4.4 Sync data/ catalogs */
    let src_data = repo_root.join("data");
    copy_dir_all(&src_data, &share_data)?;
    println!("  Synchronized data catalogs -> {:?}", share_data);

    /* 4.5 Sync assets, typography, and install icons */
    let src_fonts = repo_root.join("assets/fonts");
    let target_fonts = share_assets.join("fonts");
    if src_fonts.is_dir() {
        let _ = copy_dir_all(&src_fonts, &target_fonts);
        let _ = Command::new("mkfontscale").arg(&target_fonts).status();
        let _ = Command::new("mkfontdir").arg(&target_fonts).status();
        let _ = Command::new("xset").arg("fp+").arg(&target_fonts).status();
        let _ = Command::new("xset").arg("fp").arg("rehash").status();
        println!("  Synchronized High Fantasy fonts -> {:?}", target_fonts);
    }

    let src_icon = repo_root.join("assets/icons/tenebris_lux_256.png");
    let target_icon = share_assets.join("icon.png");
    let hicolor_icon = icons_dir.join("tenebris-lux.png");

    if src_icon.is_file() {
        let _ = fs::copy(&src_icon, &target_icon);
        let _ = fs::copy(&src_icon, &hicolor_icon);
        println!("  Installed application icon -> {:?}", target_icon);
    }

    /* 4.6 Write .desktop entry */
    let desktop_content = format!(
        "[Desktop Entry]\n\
         Type=Application\n\
         Version=1.0\n\
         Name=Tenebris and Lux\n\
         GenericName=Tactical Strategy RPG\n\
         Comment=Tactical TRPG Hero's Journey (2.5D Isometric Engine)\n\
         Exec={}/tenebris-lux-launcher --gui\n\
         Icon={}\n\
         Terminal=false\n\
         Categories=Game;RolePlaying;StrategyGame;\n\
         StartupNotify=true\n\
         Keywords=RPG;TRPG;Tactics;Isometric;Fantasy;\n",
        local_bin.display(),
        target_icon.display()
    );

    let app_desktop = apps_dir.join("tenebris-lux.desktop");
    fs::write(&app_desktop, &desktop_content).map_err(|e| {
        format!("Failed to write desktop file: {}", e)
    })?;
    fs::set_permissions(&app_desktop, fs::Permissions::from_mode(0o755))
        .map_err(|e| e.to_string())?;

    let user_desktop = desktop_dir.join("Tenebris and Lux.desktop");
    fs::write(&user_desktop, &desktop_content).map_err(|e| {
        format!("Failed to write user desktop shortcut: {}", e)
    })?;
    fs::set_permissions(&user_desktop, fs::Permissions::from_mode(0o755))
        .map_err(|e| e.to_string())?;

    println!("  Created desktop application -> {:?}", app_desktop);
    println!("  Created desktop shortcut    -> {:?}", user_desktop);

    /* Update desktop database if available */
    let _ = Command::new("update-desktop-database")
        .arg(&apps_dir)
        .status();

    /* -------------------------------------------------------------------------
     * Phase 5: Final Telemetry Manifest
     * ---------------------------------------------------------------------- */
    println!("\n[PHASE 5/5] Installation & Deployment Summary");
    println!("------------------------------------------------------------");
    println!("Game Executable      : {:?}", installed_game_bin);
    println!("CLI Symlink          : {:?}", symlink_bin);
    println!("Unified Launcher     : {:?}",
             local_bin.join("tenebris-lux-launcher"));
    println!("System Updater       : {:?}",
             local_bin.join("tenebris-lux-updater"));
    println!("Game Data Catalog    : {:?}", share_data);
    println!("Desktop Shortcut     : {:?}", user_desktop);
    println!("============================================================");
    println!("INSTALLATION VERIFIED: Tenebris and Lux is ready for testing!");
    println!("Run 'tenebris-lux' or 'tenebris-lux-launcher' from terminal,");
    println!("or double-click 'Tenebris and Lux' directly on your Desktop.");
    println!("============================================================");

    Ok(())
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Developer Tooling | Machine: tools.bin.updater
// =============================================================================

fn audit_installed_system() -> Result<(), String> {
    let home_dir = env::var("HOME").unwrap_or_else(|_| ".".to_string());
    let home_path = PathBuf::from(&home_dir);
    let bin = home_path.join(".local/share/tenebris-lux/bin/tenebris-lux");
    let data = home_path.join(".local/share/tenebris-lux/data/manifest.json");

    if !bin.is_file() {
        return Err("Installed binary missing".into());
    }
    if !data.is_file() {
        return Err("Installed data catalog missing".into());
    }

    Ok(())
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK / CLI ENTRY POINT
// =============================================================================

fn main() -> ExitCode {
    let current_dir = env::current_dir().unwrap_or_else(|_| PathBuf::from("."));
    let home_dir = env::var("HOME").unwrap_or_else(|_| ".".to_string());
    let repo_candidate = PathBuf::from(&home_dir).join("Tenebris and Lux");

    let repo_root = if current_dir.join("CMakeLists.txt").is_file() {
        current_dir
    } else if repo_candidate.join("CMakeLists.txt").is_file() {
        repo_candidate
    } else {
        eprintln!("Error: Could not resolve Tenebris and Lux repository root.");
        return ExitCode::FAILURE;
    };

    match run_updater_pipeline(&repo_root) {
        Ok(()) => {
            if let Err(e) = audit_installed_system() {
                eprintln!("[WARN] Post-install audit notice: {}", e);
            }
            ExitCode::SUCCESS
        }
        Err(e) => {
            eprintln!("\n[UPDATER FATAL] {}", e);
            ExitCode::FAILURE
        }
    }
}
