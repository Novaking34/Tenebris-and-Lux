"""
================================================================================
BLOCK 1: METADATA BLOCK
================================================================================
@project       Tenebris and Lux
@file          tools/orchestrator/probes/memory_probe.py
@id            omnicode.tools.probes.memory
@layer         Layer 1: Hardware & System Probes
@version       0.7.2-dev
@interface_rev Rev 5
@date          2026-11-13
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@system        OmniCode Orchestrator // Host RAM & Non-Headless Session Guard
@role          Monitors Linux /proc/meminfo & Computes Interactive Desktop Working Memory Reservations
@invariants    1. Zero external pip dependencies.
               2. Safe integer extraction from /proc/meminfo.
               3. Non-headless stability floor: Reserves at least 3072 MB RAM for desktop/IDE/browser.
               4. Computes safe model RAM budget preventing swapping and Linux OOM-killer invocations.
================================================================================
"""

# ==============================================================================
# BLOCK 2: OPENING BLOCK
# ==============================================================================
import os
import subprocess
from typing import Dict, Any

# Memory reservations in Megabytes
NON_HEADLESS_DESKTOP_RESERVATION_MB = 3072  # 3.0 GB reserved for Wayland/X11, IDE, browser, audio
HEADLESS_SERVER_RESERVATION_MB = 512        # 512 MB minimal OS floor for headless CLI environments

# ==============================================================================
# BLOCK 3: BODY BLOCK
# ==============================================================================
def detect_non_headless_session() -> Dict[str, Any]:
    """
    Detects whether the host is running in an interactive graphical desktop session
    (Wayland / X11 / Desktop Environment) vs a headless CLI / SSH shell.
    """
    display = os.environ.get("DISPLAY", "")
    wayland_display = os.environ.get("WAYLAND_DISPLAY", "")
    session_type = os.environ.get("XDG_SESSION_TYPE", "")
    desktop_session = os.environ.get("DESKTOP_SESSION", "")

    is_non_headless = bool(display or wayland_display or (session_type and session_type != "tty"))

    compositor_found = False
    compositor_name = "None"
    known_compositors = ["gnome-shell", "kwin", "mutter", "sway", "weston", "Xorg", "wayfire", "hyprland"]

    # Check running processes if proc is accessible
    if os.path.exists("/proc"):
        try:
            # Fast check through ps or /proc
            cmd = ["ps", "-e", "-o", "comm="]
            proc = subprocess.run(cmd, capture_output=True, text=True, timeout=1.0)
            if proc.returncode == 0:
                running_comms = set(proc.stdout.split())
                for comp in known_compositors:
                    if comp in running_comms:
                        compositor_found = True
                        compositor_name = comp
                        is_non_headless = True
                        break
        except Exception:
            pass

    return {
        "is_non_headless": is_non_headless,
        "session_type": session_type if session_type else ("wayland" if wayland_display else ("x11" if display else "headless")),
        "display": display,
        "wayland_display": wayland_display,
        "desktop_session": desktop_session,
        "compositor": compositor_name
    }

def probe_system_memory() -> Dict[str, Any]:
    """
    Parses /proc/meminfo to report comprehensive RAM metrics, applies the non-headless
    working memory reservation, and calculates the true safe budget for local LLM execution.
    """
    meminfo: Dict[str, int] = {}

    if os.path.isfile("/proc/meminfo"):
        try:
            with open("/proc/meminfo", "r") as f:
                for line in f:
                    parts = line.split(":")
                    if len(parts) >= 2:
                        key = parts[0].strip()
                        val_str = parts[1].strip().split()[0]
                        try:
                            meminfo[key] = int(val_str) // 1024  # Convert kB to MB
                        except ValueError:
                            pass
        except Exception:
            pass

    total_mb = meminfo.get("MemTotal", 0)
    avail_mb = meminfo.get("MemAvailable", 0)
    free_mb = meminfo.get("MemFree", 0)
    buffers_mb = meminfo.get("Buffers", 0)
    cached_mb = meminfo.get("Cached", 0)
    swap_total_mb = meminfo.get("SwapTotal", 0)
    swap_free_mb = meminfo.get("SwapFree", 0)
    swap_used_mb = max(0, swap_total_mb - swap_free_mb)

    used_mb = max(0, total_mb - avail_mb)
    percent_used = (used_mb / total_mb * 100.0) if total_mb > 0 else 0.0

    # Non-headless environment inspection
    env_info = detect_non_headless_session()
    is_non_headless = env_info["is_non_headless"]

    desktop_reserved_mb = NON_HEADLESS_DESKTOP_RESERVATION_MB if is_non_headless else HEADLESS_SERVER_RESERVATION_MB

    # Calculate safe model RAM budget
    # 1. Cold budget (theoretical max memory an LLM can consume without starving the desktop baseline)
    cold_max_model_ram_mb = max(0, total_mb - desktop_reserved_mb - 1024)
    # 2. Live available model RAM (memory that can be allocated RIGHT NOW without breaching desktop floor)
    live_safe_model_ram_mb = max(0, avail_mb - desktop_reserved_mb)

    # Determine memory pressure status
    if avail_mb <= 2048:
        pressure_status = "CRITICAL_DESKTOP_THREAT"
    elif avail_mb <= 3500:
        pressure_status = "MODERATE_PRESSURE"
    else:
        pressure_status = "NOMINAL_HEALTHY"

    return {
        "total_mb": total_mb,
        "available_mb": avail_mb,
        "used_mb": used_mb,
        "free_mb": free_mb,
        "buffers_mb": buffers_mb,
        "cached_mb": cached_mb,
        "swap_total_mb": swap_total_mb,
        "swap_used_mb": swap_used_mb,
        "percent_used": round(percent_used, 1),
        "is_non_headless": is_non_headless,
        "session_type": env_info["session_type"],
        "compositor": env_info["compositor"],
        "desktop_reserved_mb": desktop_reserved_mb,
        "cold_max_model_ram_mb": cold_max_model_ram_mb,
        "live_safe_model_ram_mb": live_safe_model_ram_mb,
        "pressure_status": pressure_status
    }

# ==============================================================================
# BLOCK 4: AUDIT PROCEDURES
# ==============================================================================
def self_audit() -> bool:
    mem = probe_system_memory()
    assert mem["total_mb"] > 0, "Total RAM must be positive"
    assert mem["desktop_reserved_mb"] > 0, "Desktop reservation must be positive"
    assert mem["cold_max_model_ram_mb"] <= mem["total_mb"], "Model RAM budget cannot exceed total RAM"
    return True

# ==============================================================================
# BLOCK 5: CLOSING BLOCK
# ==============================================================================
if __name__ == "__main__":
    m = probe_system_memory()
    print("=" * 80)
    print("  CORNERSTONE HOST RAM TELEMETRY & DESKTOP WORKING MEMORY PROBE")
    print("=" * 80)
    print(f"  Physical RAM Total:     {m['total_mb']} MB (~{m['total_mb']/1024:.1f} GB)")
    print(f"  RAM Available Now:      {m['available_mb']} MB ({100 - m['percent_used']:.1f}% free)")
    print(f"  Session Environment:    {'NON-HEADLESS DESKTOP' if m['is_non_headless'] else 'HEADLESS SERVER'} ({m['session_type']}, Compositor: {m['compositor']})")
    print(f"  Desktop Reserved Floor: {m['desktop_reserved_mb']} MB (Protected working memory)")
    print(f"  Cold Max Model RAM:     {m['cold_max_model_ram_mb']} MB (Maximum allowable local model size)")
    print(f"  Live Safe Model RAM:    {m['live_safe_model_ram_mb']} MB (Currently uncommitted for model execution)")
    print(f"  Memory Pressure State:  {m['pressure_status']}")
    print(f"  Swap Activity:          {m['swap_used_mb']} / {m['swap_total_mb']} MB used")
    print("=" * 80)
