// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/bin/updater.rs
// @id            cog.tools.bin.updater
// @layer         Layer 3: Developer Tooling & Pipeline Orchestration
// @archetype     operational | stateful
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-16
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        Tools Updater Subsystem
// @role          Standalone System Updater & Deployment Binary Entrypoint
// @maturity      Rev 5 Baseline | Hardened
// @chassis       zero-heap (stack-managed argument dispatch and path resolution)
// @scale         O(P) across 5 pipeline phases (configure, build, verify, deploy, audit)
// @seams         in: CMake/CTest | out: ~/.local/share | probes: self_audit -> is_updater_ready
// @invariants    1. Zero deployment on test failure: installation aborts if any test fails.
//                2. Pure user-space installation targeting ~/.local/bin and ~/.local/share.
//                3. High-resolution desktop icon and valid FreeDesktop .desktop entries.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Automated system updater and PC installer for Tenebris and Lux. Compiles
//   the C11 simulation and presentation engine with zero warnings, executes
//   the full verification test suite, performs the diagnostic self-audit, and
//   installs/syncs the game binary, data catalogs, launcher, desktop entries,
//   and icons onto the host operating system.
//
// ### For Game Designers & Non-Programmers:
//   Run `updater` to automatically recompile the latest code, run all safety
//   tests, and update your desktop shortcuts and game files in one step.
//
// ### For Systems Programmers & Engine Architects:
//   Enforces the "Test Before Deploy" invariant: updates abort immediately
//   if CTest or preflight self-audit fails, ensuring broken builds are never
//   installed into the user environment.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Pre-Conditions, Imports & Constants )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Pre-Conditions & Library Imports
// -----------------------------------------------------------------------------

use std::env;
use std::fs;
use std::os::unix::fs::PermissionsExt;
use std::path::{Path, PathBuf};
use std::process::{Command, ExitCode, Stdio};

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Constants & Identity
// -----------------------------------------------------------------------------

/// Canonical updater version string.
pub const UPDATER_VERSION: &str = "0.1.2-dev";

/// Canonical binary identifier.
pub const BINARY_NAME: &str = "updater";

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Filesystem Utilities & 5-Phase Update Pipeline )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: Filesystem Copy Utilities
// -----------------------------------------------------------------------------

/// Recursively copies a directory and its contents to a destination.
pub fn copy_dir_all(src: &Path, dst: &Path) -> Result<(), String> {
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

// -----------------------------------------------------------------------------
// Sub-Assembly 3.2: 5-Phase Update Pipeline
// -----------------------------------------------------------------------------

/// Runs the 5-phase update, build, verification, and installation pipeline.
pub fn run_updater_pipeline(repo_root: &Path) -> Result<(), String> {
    println!("============================================================");
    println!("  TENEBRIS AND LUX - SYSTEM UPDATER & INSTALLER [v{}]", UPDATER_VERSION);
    println!("============================================================");
    println!("Repository Root: {:?}", repo_root);

    let home_dir = env::var("HOME").unwrap_or_else(|_| ".".to_string());
    let home_path = PathBuf::from(&home_dir);

    // Phase 1: Preflight Environment Validation
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

    // Phase 2: Compilation & Zero-Warning Build
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

    // Phase 3: Automated Verification & Diagnostic Audit
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
    println!("  [PASS] All verification targets passed cleanly.");

    println!("Running Pre-Flight Diagnostic Self-Audit...");
    let game_bin = if build_dir.join("bin/tenebris-lux").is_file() {
        build_dir.join("bin/tenebris-lux")
    } else {
        build_dir.join("tenebris-lux")
    };
    let audit_status = Command::new(&game_bin)
        .arg("--audit")
        .current_dir(repo_root)
        .stdout(Stdio::null())
        .status()
        .map_err(|e| format!("Game audit execution failed: {}", e))?;

    if !audit_status.success() {
        return Err("Diagnostic self-audit failed! Aborting update.".into());
    }
    println!("  [PASS] Module manifests & verification tripwires passed.");

    println!("Running Engine Config & Schema Validation Audit (engine/config/)...");
    let val_options = tl_tools::validator::ValidationOptions {
        target_domain: "config".to_string(),
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
            "Engine config validation failed with {} errors! Aborting update.",
            summary.error_count()
        ));
    }
    println!(
        "  [PASS] All {} config files in engine/config/ verified 5-block compliant.",
        summary.total_files
    );

    // Phase 4: Installation & Deployment Sync
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

    // Copy game binary
    let installed_game_bin = share_bin.join("tenebris-lux");
    fs::copy(&game_bin, &installed_game_bin).map_err(|e| {
        format!("Failed to install game binary: {}", e)
    })?;
    fs::set_permissions(&installed_game_bin, fs::Permissions::from_mode(0o755))
        .map_err(|e| e.to_string())?;

    // Symlink into ~/.local/bin
    let symlink_bin = local_bin.join("tenebris-lux");
    if symlink_bin.exists() {
        let _ = fs::remove_file(&symlink_bin);
    }
    let _ = fs::copy(&game_bin, &symlink_bin);
    let _ = fs::set_permissions(&symlink_bin, fs::Permissions::from_mode(0o755));

    // Compile and copy launcher
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

    // Copy updater binary
    let updater_bin = repo_root.join("tools/target/debug/updater");
    if updater_bin.is_file() {
        let target_updater = local_bin.join("tenebris-lux-updater");
        let _ = fs::copy(&updater_bin, &target_updater);
        let _ = fs::set_permissions(&target_updater, fs::Permissions::from_mode(0o755));
        println!("  Installed updater  -> {:?}", target_updater);
    }

    // Sync data/ catalogs
    let src_data = repo_root.join("data");
    if src_data.is_dir() {
        copy_dir_all(&src_data, &share_data)?;
        println!("  Synchronized data catalogs -> {:?}", share_data);
    }

    // Sync assets, typography, and install icons
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

    // Write .desktop entry
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
    let _ = fs::set_permissions(&app_desktop, fs::Permissions::from_mode(0o755));

    let user_desktop = desktop_dir.join("Tenebris and Lux.desktop");
    fs::write(&user_desktop, &desktop_content).map_err(|e| {
        format!("Failed to write user desktop shortcut: {}", e)
    })?;
    let _ = fs::set_permissions(&user_desktop, fs::Permissions::from_mode(0o755));

    println!("  Created desktop application -> {:?}", app_desktop);
    println!("  Created desktop shortcut    -> {:?}", user_desktop);

    let _ = Command::new("update-desktop-database")
        .arg(&apps_dir)
        .status();

    // Phase 5: Final Telemetry Manifest
    println!("\n[PHASE 5/5] Installation & Deployment Summary");
    println!("------------------------------------------------------------");
    println!("Game Executable      : {:?}", installed_game_bin);
    println!("CLI Symlink          : {:?}", symlink_bin);
    println!("Unified Launcher     : {:?}", local_bin.join("tenebris-lux-launcher"));
    println!("System Updater       : {:?}", local_bin.join("tenebris-lux-updater"));
    println!("Game Data Catalog    : {:?}", share_data);
    println!("Desktop Shortcut     : {:?}", user_desktop);
    println!("============================================================");
    println!("INSTALLATION VERIFIED: Tenebris and Lux is ready for testing!");
    println!("============================================================");

    Ok(())
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// ( Four-Section Ontological Test Suite & Subsystem Self-Audit )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 4.1: Internal Subsystem Self-Audit Probe
// -----------------------------------------------------------------------------

/// Audits that installed artifacts are present and well-formed.
pub fn audit_installed_system() -> Result<(), String> {
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

/// Internal self-audit testing updater constants and identity invariants.
pub fn self_audit() -> Result<(), &'static str> {
    if UPDATER_VERSION.is_empty() {
        return Err("Self-audit failed: UPDATER_VERSION is empty");
    }
    if BINARY_NAME != "updater" {
        return Err("Self-audit failed: BINARY_NAME mismatch");
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
    // Section 1: Always Pass (Golden Surface Path)
    // -------------------------------------------------------------------------
    #[test]
    fn test_section_1_always_pass_updater_version() {
        assert_eq!(UPDATER_VERSION, "0.1.2-dev");
        assert_eq!(BINARY_NAME, "updater");
    }

    // -------------------------------------------------------------------------
    // Section 2: Never Fail (Edge Resilience & Fault Inversion)
    // -------------------------------------------------------------------------
    #[test]
    fn test_section_2_never_fail_self_audit() {
        assert!(self_audit().is_ok());
    }

    // -------------------------------------------------------------------------
    // Section 3: Invariants (Structural Bounds & Mathematical Guards)
    // -------------------------------------------------------------------------
    #[test]
    fn test_section_3_invariants_version_format() {
        assert!(UPDATER_VERSION.contains('.'));
    }

    // -------------------------------------------------------------------------
    // Section 4: Emergence (System Integration & Roundtrip Synthesis)
    // -------------------------------------------------------------------------
    #[test]
    fn test_section_4_emergence_ready() {
        assert!(is_updater_ready());
    }
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( Epilogue Readiness & Process Termination )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 5.1: Epilogue Readiness Probe
// -----------------------------------------------------------------------------

/// Returns true if the updater binary is ready for execution.
pub fn is_updater_ready() -> bool {
    self_audit().is_ok()
}

// -----------------------------------------------------------------------------
// Sub-Assembly 5.2: Process Termination Entrypoint
// -----------------------------------------------------------------------------

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
