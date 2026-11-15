"""
================================================================================
BLOCK 1: METADATA BLOCK
================================================================================
@project       Tenebris and Lux
@file          tools/orchestrator/gui/hub_panel.py
@id            omnicode.tools.gui.hub_panel
@layer         Layer 4: Native Desktop Studio
@version       0.5.0-dev
@interface_rev Rev 5
@date          2026-11-13
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@system        OmniCode Orchestrator // Central Game Engine & Project Hub
@role          Provides Unreal/Unity-Style Launcher Controls (Projects, Engine, Build, Playtest)
@invariants    1. Zero external pip dependencies (pure Tkinter/ttk).
               2. Direct command execution for engine build, ctest, and game launching.
               3. Clean status telemetry showing Cornerstone Engine milestone and git branch.
================================================================================
"""

# ==============================================================================
# BLOCK 2: OPENING BLOCK
# ==============================================================================
import os
import subprocess
import tkinter as tk
from tkinter import ttk, messagebox
from typing import Callable, Optional

# ==============================================================================
# BLOCK 3: BODY BLOCK
# ==============================================================================
class EngineHubPanel(ttk.Frame):
    """
    Central Project & Engine Launcher panel similar to Unreal Engine Launcher
    or Unity Hub, integrated directly into OmniCode Studio.
    """

    def __init__(self, parent, on_log_callback: Optional[Callable[[str], None]] = None):
        super().__init__(parent, padding=12)
        self.on_log = on_log_callback or (lambda msg: None)

        # Header Title
        title_frame = ttk.Frame(self)
        title_frame.pack(fill="x", pady=(0, 12))

        ttk.Label(
            title_frame,
            text="CORNERSTONE GAME ENGINE // CENTRAL PROJECT HUB",
            font=("monospace", 14, "bold"),
            foreground="#00e5ff"
        ).pack(side="left")

        ttk.Label(
            title_frame,
            text="Milestone: 0.1.1-dev (Rev 5) │ MillenniumOS Compatible",
            font=("monospace", 9),
            foreground="#aaaaaa"
        ).pack(side="right")

        # Project Card Section
        project_box = ttk.LabelFrame(self, text=" Active Project: Tenebris and Lux (Flagship RPG) ", padding=10)
        project_box.pack(fill="x", pady=(0, 12))

        desc_text = (
            "Perspective: 2D/2.5D Classic Tactical Grid (640x360 canvas) │ Sole Antagonist: The Dark Lord Usuru\n"
            "Bedrock Laws: ALLOC_ZERO_HEAP, ISO C23/C17/C11 Fall-Down Ladder, Invariant 18 Block Comments\n"
            "Root Workspace: " + os.getcwd()
        )
        ttk.Label(project_box, text=desc_text, font=("monospace", 9), justify="left").pack(anchor="w", pady=(0, 8))

        # Quick Control Action Buttons
        btn_row = ttk.Frame(project_box)
        btn_row.pack(fill="x")

        ttk.Button(btn_row, text="▶️ Launch Game (X11)", command=self._launch_game).pack(side="left", padx=(0, 6))
        ttk.Button(btn_row, text="⚙️ Build Engine (C11)", command=self._build_engine).pack(side="left", padx=6)
        ttk.Button(btn_row, text="🧪 Run CTest Suite", command=self._run_tests).pack(side="left", padx=6)
        ttk.Button(btn_row, text="🛡️ Audit Invariants", command=self._verify_invariants).pack(side="left", padx=6)
        ttk.Button(btn_row, text="🧹 Clean Rebuild", command=self._clean_rebuild).pack(side="left", padx=6)

        # Two-Column Section: Left = Engine Subsystems, Right = Fast Launch / Docs
        mid_pane = ttk.Frame(self)
        mid_pane.pack(fill="both", expand=True)

        # Subsystems Registry Tree
        subsys_box = ttk.LabelFrame(mid_pane, text=" Cornerstone Engine Subsystems (44 Headers) ", padding=8)
        subsys_box.pack(side="left", fill="both", expand=True, padx=(0, 6))

        columns = ("subsys", "layer", "status")
        self.tree_subsys = ttk.Treeview(subsys_box, columns=columns, show="headings", height=8)
        self.tree_subsys.heading("subsys", text="Subsystem")
        self.tree_subsys.heading("layer", text="Stratum")
        self.tree_subsys.heading("status", text="Rev 5 Modernization")
        self.tree_subsys.column("subsys", width=160)
        self.tree_subsys.column("layer", width=120)
        self.tree_subsys.column("status", width=180)

        subsys_data = [
            ("log/ (Flight Log)", "Layer 0: Core", "ROM Vault + Safe-Mode [DONE]"),
            ("debug/ (Probes/AWAIT)", "Layer 0: Core", "Probes + Safe-Mode [DONE]"),
            ("io/ (VFS & Master)", "Layer 0: Core", "Layered VFS [ACTIVE]"),
            ("core/ (Math/Time)", "Layer 0: Core", "Scalar & Dimetric [DONE]"),
            ("audio/ (7-Bus Mixer)", "Layer 0: Core", "Procedural Fallback [DONE]"),
            ("viewport/ (Rasterizer)", "Layer 2: Viewport", "640x360 Integer Canvas [DONE]"),
            ("game/ (TurnSim & Lore)", "Layer 1: Sim", "Twin Relics & Grid [DONE]")
        ]
        for item in subsys_data:
            self.tree_subsys.insert("", "end", values=item)
        self.tree_subsys.pack(fill="both", expand=True)

        # Right: Quick Access & Telemetry
        links_box = ttk.LabelFrame(mid_pane, text=" Developer Shortcuts & Resources ", padding=8)
        links_box.pack(side="right", fill="both", expand=True, padx=(6, 0))

        ttk.Label(links_box, text="Documentation & Master Guides:", font=("monospace", 9, "bold"), foreground="#00e5ff").pack(anchor="w")
        ttk.Label(links_box, text="• AGENTS.md (Rev 5 Bedrock Standards & Canon)", font=("monospace", 8)).pack(anchor="w", pady=2)
        ttk.Label(links_box, text="• docs/master_guide_manifest.json (6 Volumes)", font=("monospace", 8)).pack(anchor="w", pady=2)
        ttk.Label(links_box, text="• engine/config/standards/subsystems.json", font=("monospace", 8)).pack(anchor="w", pady=2)

        ttk.Label(links_box, text="\nModel Spectrum & Cockpit Access:", font=("monospace", 9, "bold"), foreground="#00e5ff").pack(anchor="w")
        ttk.Label(links_box, text="• 0.5B Scout: Laser-fast task DAG decomposition", font=("monospace", 8)).pack(anchor="w", pady=2)
        ttk.Label(links_box, text="• 1.5B Bedrock: Native 32k window workhorse", font=("monospace", 8)).pack(anchor="w", pady=2)
        ttk.Label(links_box, text="• 3B Polymath: C11 systems & logic author", font=("monospace", 8)).pack(anchor="w", pady=2)
        ttk.Label(links_box, text="• 7B Architect: Local PC hardware ceiling", font=("monospace", 8)).pack(anchor="w", pady=2)

    def _launch_game(self):
        canonical_bin = os.path.join(os.getcwd(), "build", "bin", "tenebris-lux")
        symlink_bin = os.path.join(os.getcwd(), "build", "tenebris-lux")
        bin_path = canonical_bin if os.path.exists(canonical_bin) else symlink_bin
        if not os.path.exists(bin_path):
            messagebox.showwarning("Binary Not Found", "Game binary not found at build/bin/tenebris-lux or build/tenebris-lux.\nPlease click 'Build Engine' first.")
            return
        self.on_log(f"Launching {bin_path}...")
        try:
            subprocess.Popen([bin_path])
        except Exception as e:
            messagebox.showerror("Launch Error", f"Failed to launch game: {str(e)}")

    def _build_engine(self):
        self.on_log("Executing: cmake --build build ...")
        res = subprocess.run(["cmake", "--build", "build"], capture_output=True, text=True)
        if res.returncode == 0:
            self.on_log("Build SUCCESS:\n" + res.stdout)
            messagebox.showinfo("Build Success", "Cornerstone Game Engine built cleanly!")
        else:
            self.on_log("Build FAILED:\n" + res.stderr)
            messagebox.showerror("Build Failed", res.stderr[:500])

    def _run_tests(self):
        self.on_log("Executing: ctest --test-dir build --output-on-failure ...")
        res = subprocess.run(["ctest", "--test-dir", "build", "--output-on-failure"], capture_output=True, text=True)
        self.on_log(f"Test Suite Result (exit {res.returncode}):\n{res.stdout}")
        if res.returncode == 0:
            messagebox.showinfo("Tests Passed", "All CTest verification suites passed cleanly!")
        else:
            messagebox.showwarning("Test Failures", "Some tests failed. Check log for details.")

    def _verify_invariants(self):
        from ..exoskeleton.compiler_tripwire import run_compiler_probe
        ok, msg = run_compiler_probe()
        self.on_log(f"Compiler Invariant Probe: {'PASS' if ok else 'FAIL'}\n{msg}")
        if ok:
            messagebox.showinfo("Invariants Verified", "All physical engine invariants verified clean!")
        else:
            messagebox.showerror("Invariant Failure", msg)

    def _clean_rebuild(self):
        if not messagebox.askyesno("Clean Rebuild", "Delete build/ directory and re-run CMake configuration?"):
            return
        self.on_log("Cleaning build directory...")
        subprocess.run(["rm", "-rf", "build"])
        res = subprocess.run(["cmake", "-B", "build", "-DCMAKE_BUILD_TYPE=Debug"], capture_output=True, text=True)
        if res.returncode == 0:
            self.on_log("CMake Configuration SUCCESS:\n" + res.stdout)
            messagebox.showinfo("Configured", "Clean build configured. Click 'Build Engine' to compile.")
        else:
            self.on_log("CMake Configuration FAILED:\n" + res.stderr)
            messagebox.showerror("Configure Failed", res.stderr)

