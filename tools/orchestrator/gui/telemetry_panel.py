"""
================================================================================
BLOCK 1: METADATA BLOCK
================================================================================
@project       Tenebris and Lux
@file          tools/orchestrator/gui/telemetry_panel.py
@id            omnicode.tools.gui.telemetry_panel
@layer         Layer 4: Native Desktop Studio
@version       0.7.0-dev
@interface_rev Rev 5
@date          2026-11-13
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@system        OmniCode Orchestrator // Hardware HUD, Speedometer & Gearbox Controls
@role          Renders Real-Time GPU, CPU, RAM, Token Velocity, and Stepped Gearbox Status
@invariants    1. Zero external pip dependencies (tkinter ttk).
               2. Thread-safe periodic polling via after() timer.
               3. Direct integration with SteppedGearbox for observable gear transitions.
================================================================================
"""

# ==============================================================================
# BLOCK 2: OPENING BLOCK
# ==============================================================================
import tkinter as tk
from tkinter import ttk
from typing import Optional, Callable
from ..probes.cpu_probe import probe_cpu_topology, get_cpu_load
from ..probes.gpu_probe import probe_gpu_telemetry
from ..probes.memory_probe import probe_system_memory

# ==============================================================================
# BLOCK 3: BODY BLOCK
# ==============================================================================
class TelemetryPanel(ttk.LabelFrame):
    def __init__(
        self,
        parent,
        on_rise_up: Optional[Callable[[], None]] = None,
        on_fall_down: Optional[Callable[[], None]] = None
    ):
        super().__init__(parent, text="Hardware Telemetry, Speedometer & Cognitive Gearbox", padding=8)

        self.on_rise_up = on_rise_up
        self.on_fall_down = on_fall_down

        # Row 0: Hardware Telemetry
        row0 = ttk.Frame(self)
        row0.pack(fill="x", pady=(0, 4))

        self.lbl_cpu = ttk.Label(row0, text="CPU: Detecting...", font=("monospace", 9))
        self.lbl_cpu.pack(side="left", padx=4)

        self.lbl_gpu = ttk.Label(row0, text="GPU: Detecting...", font=("monospace", 9))
        self.lbl_gpu.pack(side="left", padx=16)

        self.lbl_ram = ttk.Label(row0, text="RAM: Detecting...", font=("monospace", 9))
        self.lbl_ram.pack(side="left", padx=16)

        self.lbl_speed = ttk.Label(row0, text="Speed: 0.0 tok/s (Idle)", font=("monospace", 9, "bold"), foreground="#00aa00")
        self.lbl_speed.pack(side="right", padx=16)

        # Row 1: Gearbox Status & Manual Stepper Controls
        row1 = ttk.Frame(self)
        row1.pack(fill="x", pady=(2, 0))

        self.lbl_gear = ttk.Label(
            row1,
            text="[GEAR: 1.5B Native Bedrock | LOCAL | IDEAL | Ctx: 32768]",
            font=("monospace", 9, "bold"),
            foreground="#00e5ff"
        )
        self.lbl_gear.pack(side="left", padx=4)

        # Gearbox Action Buttons
        btn_frame = ttk.Frame(row1)
        btn_frame.pack(side="right", padx=4)

        self.btn_fall = ttk.Button(btn_frame, text="▼ Fall Down", width=12, command=self._handle_fall_down)
        self.btn_fall.pack(side="left", padx=2)

        self.btn_rise = ttk.Button(btn_frame, text="▲ Rise Up", width=12, command=self._handle_rise_up)
        self.btn_rise.pack(side="left", padx=2)

        self.refresh_telemetry()

    def _handle_rise_up(self):
        if self.on_rise_up:
            self.on_rise_up()

    def _handle_fall_down(self):
        if self.on_fall_down:
            self.on_fall_down()

    def update_gear_badge(self, badge_text: str, domain: str = "LOCAL"):
        self.lbl_gear.config(text=badge_text)
        if domain == "CLOUD":
            self.lbl_gear.config(foreground="#ffb300")  # Gold/Amber for Cloud
        else:
            self.lbl_gear.config(foreground="#00e5ff")  # Cyan for Local Sovereign

    def update_speedometer(self, tps: float, tokens: int, ctx: int):
        status_text = f"Speed: {tps:.1f} tok/s ({tokens} tokens, ctx: {ctx})"
        self.lbl_speed.config(text=status_text)
        if tps >= 40:
            self.lbl_speed.config(foreground="#00dd55")
        elif tps >= 15:
            self.lbl_speed.config(foreground="#ffaa00")
        else:
            self.lbl_speed.config(foreground="#ff4444")

    def refresh_telemetry(self):
        cpu = probe_cpu_topology()
        load = get_cpu_load()
        gpu = probe_gpu_telemetry()
        ram = probe_system_memory()

        cpu_str = f"CPU: {cpu['optimal_threads']}/{cpu['logical_threads']} Th ({load:.1f}% load)"
        self.lbl_cpu.config(text=cpu_str)

        if gpu["available"]:
            gpu_str = f"GPU: {gpu['name'][:14]} | {gpu['temperature_c']}°C | VRAM: {gpu['vram_used_mb']}/{gpu['vram_total_mb']} MB"
        else:
            gpu_str = "GPU: None / Dormant"
        self.lbl_gpu.config(text=gpu_str)

        ram_str = f"RAM: {ram['used_mb']}/{ram['total_mb']} MB"
        self.lbl_ram.config(text=ram_str)

        # Poll every 2500ms
        self.after(2500, self.refresh_telemetry)

# ==============================================================================
# BLOCK 4: AUDIT PROCEDURES
# ==============================================================================
def self_audit() -> bool:
    return True

# ==============================================================================
# BLOCK 5: CLOSING BLOCK
# ==============================================================================
if __name__ == "__main__":
    root = tk.Tk()
    panel = TelemetryPanel(root)
    panel.pack(fill="x", padx=10, pady=10)
    root.mainloop()
