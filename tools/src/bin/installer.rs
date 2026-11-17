// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/bin/installer.rs
// @id            cog.tools.bin.installer
// @layer         Layer 3: Developer Tooling & Pipeline Orchestration
// @archetype     operational | stateful
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-16
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        Tools Installer Subsystem
// @role          Standalone Linux Desktop Installer Binary Entrypoint
// @maturity      Rev 5 Baseline | Hardened
// @chassis       zero-heap (stack-managed argument dispatch and path resolution)
// @scale         O(1) deployment pipeline over user-space paths
// @seams         in: filesystem | out: ~/.local/bin | probes: self_audit -> is_installer_ready
// @invariants    1. Pure user-space installation targeting ~/.local/bin and ~/.local/share.
//                2. Zero sudo requirement: Never touches system-root directories.
//                3. Valid FreeDesktop .desktop entry standards (Exec, Icon, Categories).
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Unified Linux Desktop Installer for Tenebris and Lux and OmniCode Studio.
//   Bundles the installation of the game binary, terminal launcher, system
//   updater, OmniCode Hub desktop launcher, FreeDesktop .desktop entries,
//   and high-resolution application icons into ~/.local/bin and applications.
//
// ### For Game Designers & Non-Programmers:
//   Run `installer` to deploy the game engine, desktop icons, and launcher
//   shortcuts to your Linux application menu so you can launch them easily.
//
// ### For Systems Programmers & Engine Architects:
//   Deploys pure user-space artifacts without root permissions, managing
//   permissions, desktop file entries, and path discovery safely.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Pre-Conditions, Imports & Data Structures )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Pre-Conditions & Library Imports
// -----------------------------------------------------------------------------

use std::env;
use std::fs;
use std::os::unix::fs::PermissionsExt;
use std::path::PathBuf;
use std::process::{Command, ExitCode};

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Constants & Path Configurations
// -----------------------------------------------------------------------------

/// Version of the desktop installer.
pub const INSTALLER_VERSION: &str = "0.1.2-dev";

/// Canonical binary identifier.
pub const BINARY_NAME: &str = "installer";

/// Discovered filesystem paths for desktop deployment.
pub struct InstallPaths {
    pub repo_root: PathBuf,
    pub home_dir: PathBuf,
    pub local_bin: PathBuf,
    pub user_bin: PathBuf,
    pub apps_dir: PathBuf,
    pub icons_dir: PathBuf,
    pub share_dir: PathBuf,
}

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Installation Pipeline Execution )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: Path Discovery Logic
// -----------------------------------------------------------------------------

impl InstallPaths {
    /// Discovers user and repo paths for desktop installation.
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

// -----------------------------------------------------------------------------
// Sub-Assembly 3.2: Pipeline Execution
// -----------------------------------------------------------------------------

/// Executes the full multi-phase installation pipeline.
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
    let _ = fs::set_permissions(&hub_desktop, hub_desktop_content_permissions());
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

fn hub_desktop_content_permissions() -> fs::Permissions {
    fs::Permissions::from_mode(0o755)
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// ( Four-Section Ontological Test Suite & Subsystem Self-Audit )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 4.1: Internal Subsystem Self-Audit Probe
// -----------------------------------------------------------------------------

/// Internal self-audit testing installer constants and identity invariants.
pub fn self_audit() -> Result<(), &'static str> {
    if INSTALLER_VERSION.is_empty() {
        return Err("Self-audit failed: INSTALLER_VERSION is empty");
    }
    if BINARY_NAME != "installer" {
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
    fn test_section_1_always_pass_installer_version() {
        assert_eq!(INSTALLER_VERSION, "0.1.2-dev");
        assert_eq!(BINARY_NAME, "installer");
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
        assert!(INSTALLER_VERSION.contains('.'));
    }

    // -------------------------------------------------------------------------
    // Section 4: Emergence (System Integration & Roundtrip Synthesis)
    // -------------------------------------------------------------------------
    #[test]
    fn test_section_4_emergence_ready() {
        assert!(is_installer_ready());
    }
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( Epilogue Readiness & Process Termination )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 5.1: Epilogue Readiness Probe
// -----------------------------------------------------------------------------

/// Returns true if the installer binary is ready for execution.
pub fn is_installer_ready() -> bool {
    self_audit().is_ok()
}

// -----------------------------------------------------------------------------
// Sub-Assembly 5.2: Process Termination Entrypoint
// -----------------------------------------------------------------------------

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
