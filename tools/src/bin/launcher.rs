// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/bin/launcher.rs
// @id            tools.bin.launcher
// @layer         Layer 3: Systems Developer Tooling Standalone Binary
// @version       1.0.0
// @interface_rev Rev 1
// @date          2026-11-03
// @updated       2026-11-03
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Unified system launcher and mode selector for Tenebris and Lux. Provides
//   an interactive terminal launcher and CLI router for desktop GUI (2.5D
//   isometric mode), headless CLI simulation, interactive state debugger,
//   diagnostic self-audit suite, automated verification, and system updates.
//
// PEDAGOGICAL MISSION:
//   Decouples the human operator and desktop environment from explicit binary
//   paths and working directory dependencies. Automatically resolves data
//   directories, validates pre-flight invariants, and invokes the game engine.
//
// AXIOMATIC INVARIANTS:
//   1. Strict exit code propagation (forwards child process exit code).
//   2. Fail-fast error reporting if game binary or data catalog is missing.
//   3. Deterministic working directory anchoring to guarantee data/ resolution.
//   4. Strict <= 80-column line width discipline across all documentation.
// =============================================================================

//! # Unified Game Launcher Binary
//!
//! Provides interactive and CLI launch profiles for [`tenebris-lux`].

// =============================================================================
// BLOCK 2: OPENING BLOCK / IMPORTS & TYPES
// Stratum: Developer Tooling | Machine: tools.bin.launcher
// =============================================================================

use std::env;
use std::io::{self, Write};
use std::path::PathBuf;
use std::process::{Command, ExitCode, Stdio};

const LAUNCHER_VERSION: &str = "1.0.0";

/// Selected launch profile action.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum LaunchMode {
    Gui,
    Cli,
    Debug,
    Audit,
    Validate,
    Update,
    Test,
    Install,
    Interactive,
    Help,
    Version,
}

/// Resolved runtime environment configuration.
#[derive(Debug, Clone)]
pub struct LauncherEnv {
    pub repo_root: PathBuf,
    pub game_binary: PathBuf,
    pub data_dir: PathBuf,
    pub mode: LaunchMode,
}

// =============================================================================
// BLOCK 3: BODY BLOCK / PATH RESOLUTION & EXECUTION LOGIC
// Stratum: Developer Tooling | Machine: tools.bin.launcher
// =============================================================================

impl LauncherEnv {
    /// Discovers game paths relative to current exe, repo root, or ~/.local.
    pub fn discover(mode: LaunchMode) -> Result<Self, String> {
        let current_dir =
            env::current_dir().unwrap_or_else(|_| PathBuf::from("."));
        let home_dir = env::var("HOME").unwrap_or_else(|_| ".".to_string());
        let home_path = PathBuf::from(&home_dir);

        let candidate_roots = [
            current_dir.clone(),
            home_path.join("Tenebris and Lux"),
            home_path.join(".local/share/tenebris-lux"),
        ];

        let mut resolved_root = None;
        let mut resolved_binary = None;
        let mut resolved_data = None;

        for root in &candidate_roots {
            let bin_candidates = [
                root.join("build/tenebris-lux"),
                root.join("bin/tenebris-lux"),
                root.join("tenebris-lux"),
                home_path.join(".local/bin/tenebris-lux"),
            ];

            for bin in &bin_candidates {
                if bin.is_file() {
                    resolved_binary = Some(bin.clone());
                    break;
                }
            }

            let data_candidate = if root.join("game/data/manifest.json").is_file() {
                Some(root.join("game/data"))
            } else if root.join("data/manifest.json").is_file() {
                Some(root.join("data"))
            } else {
                None
            };

            if let Some(data_path) = data_candidate {
                resolved_data = Some(data_path);
                resolved_root = Some(root.clone());
                break;
            }
        }

        let repo_root = resolved_root.unwrap_or(current_dir);
        let game_binary = resolved_binary.unwrap_or_else(|| {
            repo_root.join("build/tenebris-lux")
        });
        let data_dir = resolved_data.unwrap_or_else(|| repo_root.join("data"));

        Ok(Self {
            repo_root,
            game_binary,
            data_dir,
            mode,
        })
    }

    /// Launches the game binary with the given command line argument.
    pub fn spawn_game(&self, flag: &str) -> Result<i32, String> {
        if !self.game_binary.is_file() {
            return Err(format!(
                "Game binary not found at {:?}. Please run updater to compile.",
                self.game_binary
            ));
        }

        let has_data = self.repo_root.join("data/manifest.json").is_file();
        let working_dir = if has_data {
            &self.repo_root
        } else if self.data_dir.parent().is_some() {
            self.data_dir.parent().unwrap()
        } else {
            &self.repo_root
        };

        println!(">>> Launching: {:?} {}", self.game_binary, flag);
        println!(">>> Working Directory: {:?}", working_dir);

        let mut cmd = Command::new(&self.game_binary);
        if !flag.is_empty() {
            cmd.arg(flag);
        }
        cmd.current_dir(working_dir)
            .stdin(Stdio::inherit())
            .stdout(Stdio::inherit())
            .stderr(Stdio::inherit());

        match cmd.status() {
            Ok(status) => Ok(status.code().unwrap_or(0)),
            Err(e) => Err(format!("Failed to execute game binary: {}", e)),
        }
    }

    /// Invokes the automated verification test harness (ctest).
    pub fn run_tests(&self) -> Result<i32, String> {
        let build_dir = self.repo_root.join("build");
        if !build_dir.is_dir() {
            return Err("Build directory not found; compile first.".into());
        }

        println!(">>> Executing Automated Verification Suite (CTest)...");
        let status = Command::new("ctest")
            .arg("--test-dir")
            .arg(&build_dir)
            .arg("-C")
            .arg("Debug")
            .arg("--output-on-failure")
            .status()
            .map_err(|e| format!("Failed to run ctest: {}", e))?;

        Ok(status.code().unwrap_or(0))
    }

    /// Invokes the updater pipeline executable.
    pub fn run_updater(&self) -> Result<i32, String> {
        let home_dir = env::var("HOME").unwrap_or_else(|_| ".".to_string());
        let updater_candidates = [
            self.repo_root.join("tools/target/debug/updater"),
            self.repo_root.join("tools/target/release/updater"),
            PathBuf::from(&home_dir).join(".local/bin/tenebris-lux-updater"),
        ];

        for u in &updater_candidates {
            if u.is_file() {
                println!(">>> Running System Updater: {:?}", u);
                let status = Command::new(u)
                    .current_dir(&self.repo_root)
                    .stdin(Stdio::inherit())
                    .stdout(Stdio::inherit())
                    .stderr(Stdio::inherit())
                    .status()
                    .map_err(|e| format!("Failed to run updater: {}", e))?;
                return Ok(status.code().unwrap_or(0));
            }
        }

        /* Fallback: run via cargo */
        println!(">>> Invoking updater via cargo...");
        let cargo_status = Command::new("cargo")
            .arg("run")
            .arg("--manifest-path")
            .arg(self.repo_root.join("tools/Cargo.toml"))
            .arg("--bin")
            .arg("updater")
            .current_dir(&self.repo_root)
            .stdin(Stdio::inherit())
            .stdout(Stdio::inherit())
            .stderr(Stdio::inherit())
            .status()
            .map_err(|e| format!("Cargo updater execution failed: {}", e))?;

        Ok(cargo_status.code().unwrap_or(0))
    }
}

/// Displays the interactive terminal launcher dashboard.
fn run_interactive_dashboard(env: &LauncherEnv) -> Result<i32, String> {
    let display_var = env::var("DISPLAY").unwrap_or_default();
    let display_status = if !display_var.is_empty() {
        format!("X11 Connected ({}) [GUI Ready]", display_var)
    } else {
        "Headless Context (Terminal CLI Only)".to_string()
    };

    loop {
        println!("\n=======================================================");
        println!("       TENEBRIS AND LUX - UNIFIED LAUNCHER [v{}]",
                 LAUNCHER_VERSION);
        println!("  Tactical Strategy RPG & 2.5D Isometric Engine");
        println!("=======================================================");
        println!("  Installed Binary : {:?}", env.game_binary);
        println!("  Data Catalog     : {:?}", env.data_dir);
        println!("  Display Hardware : {}", display_status);
        println!("-------------------------------------------------------");
        println!("  [1] Launch Native Desktop GUI (2.5D Isometric Mode)");
        println!("  [2] Launch Terminal CLI Simulation Mode");
        println!("  [3] Launch Interactive State Debugger & Rewind Engine");
        println!("  [4] Run Diagnostic Pre-Flight Self-Audit (16 Subsystems)");
        println!("  [5] Run System Updater & Recompile Latest Build");
        println!("  [6] Execute Full Automated Verification Suite (CTest)");
        println!("  [7] View Master Guide Documentation");
        println!("  [8] Run Engine Config & Schema Validation Audit (engine/config/)");
        println!("  [0] Exit Launcher");
        println!("-------------------------------------------------------");
        print!("  Enter selection [1-8, 0]: ");
        io::stdout().flush().map_err(|e| e.to_string())?;

        let mut input = String::new();
        io::stdin().read_line(&mut input).map_err(|e| e.to_string())?;
        let choice = input.trim();

        match choice {
            "1" => return env.spawn_game("--gui"),
            "2" => return env.spawn_game("--cli"),
            "3" => return env.spawn_game("--debug"),
            "4" => return env.spawn_game("--audit"),
            "5" => {
                let rc = env.run_updater()?;
                if rc != 0 {
                    eprintln!("Updater returned non-zero exit code: {}", rc);
                }
            }
            "6" => {
                let _ = env.run_tests();
            }
            "7" => {
                let guide_path = env.repo_root.join("docs/MASTER_GUIDE.md");
                let pdf_path = env.repo_root.join("docs/MASTER_GUIDE.pdf");
                println!("\nMaster Guide Documentation Location:");
                println!("  Markdown: {:?}", guide_path);
                println!("  PDF Book: {:?}", pdf_path);
                println!("Tip: Read MASTER_GUIDE.md or open the PDF viewer.");
            }
            "8" => {
                let val_options = tl_tools::validator::ValidationOptions {
                    target_domain: "config".to_string(),
                    strict: false,
                    verbose: true,
                    repo_root: env.repo_root.clone(),
                };
                let validator = tl_tools::validator::DataValidator::new(val_options);
                match validator.run_audit() {
                    Ok(summary) => summary.print_summary("config"),
                    Err(e) => eprintln!("Validation error: {}", e),
                }
            }
            "0" | "q" | "exit" => {
                println!("Exiting launcher.");
                return Ok(0);
            }
            _ => {
                println!("Invalid input. Enter a choice between 0 and 8.");
            }
        }
    }
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Developer Tooling | Machine: tools.bin.launcher
// =============================================================================

fn audit_environment(env: &LauncherEnv) -> Result<(), String> {
    if !env.data_dir.join("manifest.json").is_file() {
        return Err(format!(
            "Data catalog manifest missing at {:?}",
            env.data_dir.join("manifest.json")
        ));
    }
    Ok(())
}

fn print_help() {
    println!("Tenebris and Lux Unified Launcher v{}", LAUNCHER_VERSION);
    println!("Usage: tenebris-lux-launcher [OPTIONS]");
    println!();
    println!("Options:");
    println!("  -g, --gui        Launch native desktop X11 GUI (2.5D view)");
    println!("  -c, --cli        Launch terminal CLI mode");
    println!("  -d, --debug      Launch interactive CLI state debugger");
    println!("  -a, --audit      Run comprehensive diagnostic self-audit");
    println!("  -u, --update     Run compilation updater & installer");
    println!("  -t, --test       Run CTest automated verification suite");
    println!("      --install    Install desktop entries & user binaries");
    println!("  -h, --help       Display this help message");
    println!("  -v, --version    Display version information");
    println!();
    println!("Running without options opens the interactive launcher menu.");
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK / CLI ENTRY POINT
// =============================================================================

fn main() -> ExitCode {
    let args: Vec<String> = env::args().collect();
    let mut mode = LaunchMode::Interactive;

    if args.len() > 1 {
        match args[1].as_str() {
            "-g" | "--gui" => mode = LaunchMode::Gui,
            "-c" | "--cli" => mode = LaunchMode::Cli,
            "-d" | "--debug" => mode = LaunchMode::Debug,
            "-a" | "--audit" => mode = LaunchMode::Audit,
            "--validate" => mode = LaunchMode::Validate,
            "-u" | "--update" => mode = LaunchMode::Update,
            "-t" | "--test" => mode = LaunchMode::Test,
            "--install" => mode = LaunchMode::Install,
            "-h" | "--help" => mode = LaunchMode::Help,
            "-v" | "--version" => mode = LaunchMode::Version,
            unknown => {
                eprintln!("Unknown launcher flag: '{}'", unknown);
                print_help();
                return ExitCode::FAILURE;
            }
        }
    }

    if mode == LaunchMode::Help {
        print_help();
        return ExitCode::SUCCESS;
    }

    if mode == LaunchMode::Version {
        println!("Tenebris and Lux Launcher v{}", LAUNCHER_VERSION);
        return ExitCode::SUCCESS;
    }

    let launcher_env = match LauncherEnv::discover(mode) {
        Ok(e) => e,
        Err(err) => {
            eprintln!("Launcher Environment Error: {}", err);
            return ExitCode::FAILURE;
        }
    };

    if let Err(audit_err) = audit_environment(&launcher_env) {
        eprintln!("Launcher Pre-Flight Error: {}", audit_err);
        return ExitCode::FAILURE;
    }

    let result = match launcher_env.mode {
        LaunchMode::Gui => launcher_env.spawn_game("--gui"),
        LaunchMode::Cli => launcher_env.spawn_game("--cli"),
        LaunchMode::Debug => launcher_env.spawn_game("--debug"),
        LaunchMode::Audit => launcher_env.spawn_game("--audit"),
        LaunchMode::Validate => {
            let val_options = tl_tools::validator::ValidationOptions {
                target_domain: "config".to_string(),
                strict: false,
                verbose: true,
                repo_root: launcher_env.repo_root.clone(),
            };
            let validator = tl_tools::validator::DataValidator::new(val_options);
            match validator.run_audit() {
                Ok(summary) => {
                    summary.print_summary("config");
                    if summary.is_success(false) {
                        Ok(0)
                    } else {
                        Ok(1)
                    }
                }
                Err(e) => Err(format!("Validation error: {}", e)),
            }
        }
        LaunchMode::Update | LaunchMode::Install => launcher_env.run_updater(),
        LaunchMode::Test => launcher_env.run_tests(),
        LaunchMode::Interactive => run_interactive_dashboard(&launcher_env),
        _ => Ok(0),
    };

    match result {
        Ok(code) => {
            if code == 0 {
                ExitCode::SUCCESS
            } else {
                ExitCode::from(code as u8)
            }
        }
        Err(err) => {
            eprintln!("[LAUNCHER ERROR] {}", err);
            ExitCode::FAILURE
        }
    }
}
