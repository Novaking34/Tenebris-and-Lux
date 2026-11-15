// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/bin/installer.rs
// @id            tools.bin.installer
// @layer         Layer 3: Systems Developer Tooling Standalone Binary
// @version       1.0.0
// @interface_rev Rev 1
// @date          2026-11-13
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Unified Linux Desktop Installer for Tenebris and Lux and OmniCode Studio.
//   Bundles the installation of the game binary, terminal launcher, system
//   updater, OmniCode Hub desktop launcher, FreeDesktop .desktop entries,
//   and high-resolution application icons into ~/.local/bin and applications.
//
// PEDAGOGICAL MISSION:
//   Provides a single-command deployment pipeline that seamlessly integrates
//   both the game engine (Cornerstone) and the AI development studio (OmniCode)
//   into the host Linux desktop environment (GNOME, KDE, XFCE).
//
// AXIOMATIC INVARIANTS:
//   1. Pure user-space installation targeting ~/.local/bin and ~/.local/share.
//   2. Zero sudo requirement: Never touches system-root directories.
//   3. Valid FreeDesktop .desktop entry standards (Exec, Icon, Categories).
//   4. Strict <= 80-column line width discipline across all documentation.
// =============================================================================

//! # Unified Linux Desktop Installer
//!
//! Installs [`tenebris-lux`], the OmniCode Studio Hub, launchers, and icons.

// =============================================================================
// BLOCK 2: OPENING BLOCK / IMPORTS & CONSTANTS
// Stratum: Developer Tooling | Machine: tools.bin.installer
// =============================================================================

use std::env;
use std::fs;
use std::os::unix::fs::PermissionsExt;
use std::path::PathBuf;
use std::process::{Command, ExitCode};

const INSTALLER_VERSION: &str = "1.0.0";

// =============================================================================
// BLOCK 3: BODY BLOCK / INSTALLATION PIPELINE
// Stratum: Developer Tooling | Machine: tools.bin.installer
// =============================================================================

pub struct InstallPaths {
    pub repo_root: PathBuf,
    pub home_dir: PathBuf,
    pub local_bin: PathBuf,
    pub user_bin: PathBuf,
    pub apps_dir: PathBuf,
    pub icons_dir: PathBuf,
    pub share_dir: PathBuf,
}

impl InstallPaths {
    pub fn discover() -> Result<Self, String> {
        let current_dir = env::current_dir().unwrap_or_else(|_| PathBuf::from("."));
        let home_str = env::var("HOME").map_err(|_| "HOME environment variable not set")?;
        let home_dir = PathBuf::from(&home_str);

        // Find repo root
        let mut repo_root = current_dir.clone();
        if !repo_root.join("CMakeLists.txt").is_file() {
            let candidate = home_dir.join("Tenebris and Lux");
            if candidate.join("CMakeLists.txt").is_file() {
                repo_root = candidate;
            }
        }

        let local_bin = home_dir.join(".local/bin");
        let user_bin = home_dir.join("bin");
        let apps_dir = home_dir.join(".local/share/applications");
        let icons_dir = home_dir.join(".local/share/icons/hicolor/256x256/apps");
        let share_dir = home_dir.join(".local/share/tenebris-lux");

        Ok(Self {
            repo_root,
            home_dir,
            local_bin,
            user_bin,
            apps_dir,
            icons_dir,
            share_dir,
        })
    }
}

pub fn run_install_pipeline(paths: &InstallPaths) -> Result<(), String> {
    println!("============================================================");
    println!("  OMNICODE & CORNERSTONE - LINUX DESKTOP INSTALLER [v{}]", INSTALLER_VERSION);
    println!("============================================================");
    println!("Repository Root: {:?}", paths.repo_root);
    println!("Target Prefix:   ~/.local and ~/bin\n");

    // Phase 1: Create Directories
    println!("[1/5] Creating user-space system directories...");
    fs::create_dir_all(&paths.local_bin).map_err(|e| e.to_string())?;
    fs::create_dir_all(&paths.user_bin).map_err(|e| e.to_string())?;
    fs::create_dir_all(&paths.apps_dir).map_err(|e| e.to_string())?;
    fs::create_dir_all(&paths.icons_dir).map_err(|e| e.to_string())?;
    fs::create_dir_all(&paths.share_dir).map_err(|e| e.to_string())?;
    println!("  [PASS] Target directories ready.");

    // Phase 2: Deploy Game Engine & Launchers
    println!("\n[2/5] Deploying CLI Binaries & Launchers...");

    let game_bin = if paths.repo_root.join("build/bin/tenebris-lux").is_file() {
        paths.repo_root.join("build/bin/tenebris-lux")
    } else {
        paths.repo_root.join("build/tenebris-lux")
    };
    if game_bin.is_file() {
        let target_game = paths.local_bin.join("tenebris-lux");
        let _ = fs::copy(&game_bin, &target_game);
        let _ = fs::set_permissions(&target_game, fs::Permissions::from_mode(0o755));
        println!("  Installed game engine -> {:?}", target_game);
    } else {
        println!("  [NOTE] Game binary not yet compiled at build/bin/tenebris-lux or build/tenebris-lux.");
    }

    // Deploy omni script wrapper
    let omni_wrapper = format!(
        "#!/usr/bin/env bash\n\
         # OmniCode Unified Launcher & Studio Router\n\
         REPO_DIR=\"{}\"\n\
         export PYTHONPATH=\"$REPO_DIR:$PYTHONPATH\"\n\
         exec python3 -m tools.orchestrator.main \"$@\"\n",
        paths.repo_root.display()
    );

    let omni_local_bin = paths.local_bin.join("omni");
    let omni_user_bin = paths.user_bin.join("omni");
    fs::write(&omni_local_bin, &omni_wrapper).map_err(|e| e.to_string())?;
    let _ = fs::set_permissions(&omni_local_bin, fs::Permissions::from_mode(0o755));
    fs::write(&omni_user_bin, &omni_wrapper).map_err(|e| e.to_string())?;
    let _ = fs::set_permissions(&omni_user_bin, fs::Permissions::from_mode(0o755));
    println!("  Installed OmniCode CLI -> {:?} and {:?}", omni_local_bin, omni_user_bin);

    // Phase 3: Deploy Application Icons
    println!("\n[3/5] Installing High-Resolution Desktop Icons...");
    let icon_src = paths.repo_root.join("assets/icons/tenebris_lux_256.png");
    let game_icon_dst = paths.icons_dir.join("tenebris-lux.png");
    let hub_icon_dst = paths.icons_dir.join("omnicode-hub.png");

    if icon_src.is_file() {
        let _ = fs::copy(&icon_src, &game_icon_dst);
        let _ = fs::copy(&icon_src, &hub_icon_dst);
        println!("  Installed icon -> {:?}", game_icon_dst);
        println!("  Installed icon -> {:?}", hub_icon_dst);
    } else {
        println!("  [NOTE] Default icon assets/icons/tenebris_lux_256.png not found.");
    }

    // Phase 4: Install FreeDesktop .desktop Application Entries
    println!("\n[4/5] Registering Linux Desktop Application Entries...");

    // 4.1: Game Desktop Entry
    let game_desktop = paths.apps_dir.join("tenebris-lux.desktop");
    let game_desktop_content = format!(
        "[Desktop Entry]\n\
         Type=Application\n\
         Version=1.0\n\
         Name=Tenebris and Lux\n\
         GenericName=Tactical Strategy RPG\n\
         Comment=Classic 2D/2.5D Tactical RPG powered by Cornerstone Game Engine\n\
         Exec={}/tenebris-lux\n\
         Icon=tenebris-lux\n\
         Terminal=false\n\
         Categories=Game;RolePlaying;Strategy;\n\
         Keywords=RPG;Tactics;Cornerstone;Tenebris;Lux;\n",
        paths.local_bin.display()
    );
    fs::write(&game_desktop, game_desktop_content).map_err(|e| e.to_string())?;
    let _ = fs::set_permissions(&game_desktop, fs::Permissions::from_mode(0o755));
    println!("  Registered: {:?}", game_desktop);

    // 4.2: OmniCode Studio Hub Desktop Entry
    let hub_desktop = paths.apps_dir.join("omnicode-hub.desktop");
    let hub_desktop_content = format!(
        "[Desktop Entry]\n\
         Type=Application\n\
         Version=1.0\n\
         Name=OmniCode Hub & Studio\n\
         GenericName=Game Engine Launcher & Cognitive AI Studio\n\
         Comment=Central Project Launcher, Engine Controls and CPI-SI Agent Studio\n\
         Exec={}/omni --gui\n\
         Icon=omnicode-hub\n\
         Terminal=false\n\
         Categories=Development;IDE;Game;\n\
         Keywords=Engine;AI;Agent;Cornerstone;OmniCode;Tenebris;Studio;\n",
        paths.local_bin.display()
    );
    fs::write(&hub_desktop, hub_desktop_content).map_err(|e| e.to_string())?;
    let _ = fs::set_permissions(&hub_desktop, fs::Permissions::from_mode(0o755));
    println!("  Registered: {:?}", hub_desktop);

    // Phase 5: Update Desktop Database
    println!("\n[5/5] Refreshing Linux Application Mime & Desktop Database...");
    let _ = Command::new("update-desktop-database")
        .arg(&paths.apps_dir)
        .status();

    println!("\n============================================================");
    println!("  INSTALLATION COMPLETE!");
    println!("  You can now launch:");
    println!("    • OmniCode Hub & Studio from your Linux App Menu or via `omni --gui`");
    println!("    • Tenebris and Lux from your Linux App Menu or via `tenebris-lux`");
    println!("    • OmniCode CLI Terminal REPL via `omni`");
    println!("============================================================");

    Ok(())
}

fn main() -> ExitCode {
    let paths = match InstallPaths::discover() {
        Ok(p) => p,
        Err(e) => {
            eprintln!("Error discovering installation paths: {}", e);
            return ExitCode::FAILURE;
        }
    };

    if let Err(e) = run_install_pipeline(&paths) {
        eprintln!("\nInstallation failed: {}", e);
        ExitCode::FAILURE
    } else {
        ExitCode::SUCCESS
    }
}

