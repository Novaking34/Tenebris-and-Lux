"""
================================================================================
BLOCK 1: METADATA BLOCK
================================================================================
@project       Tenebris and Lux
@file          tools/orchestrator/probes/gpu_probe.py
@id            omnicode.tools.probes.gpu
@layer         Layer 1: Hardware & System Probes
@version       0.7.2-dev
@interface_rev Rev 5
@date          2026-11-13
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@system        OmniCode Orchestrator // GPU Hardware, VRAM & Non-Headless Session Guard
@role          Probes Physical VRAM, Driver Hardware Reservation, Compute Apps & Usable Headroom
@invariants    1. Zero external pip dependencies (invokes nvidia-smi with timeout).
               2. Safe graceful fallback if GPU is absent or sleeping.
               3. Non-headless stability floor: Reserves at least 350 MB VRAM for driver/desktop safety.
               4. Distinguishes physical VRAM, driver-reserved VRAM (e.g. 311 MiB), and live process usage.
================================================================================
"""

# ==============================================================================
# BLOCK 2: OPENING BLOCK
# ==============================================================================
import os
import subprocess
from typing import Dict, Any, List

# Minimum VRAM safety headroom required to prevent CUDA OOM, driver crashes, or desktop stutter
NON_HEADLESS_VRAM_HEADROOM_MIN_MB = 350
CUDA_CONTEXT_OVERHEAD_BASE_MB = 150

# ==============================================================================
# BLOCK 3: BODY BLOCK
# ==============================================================================
def probe_gpu_telemetry() -> Dict[str, Any]:
    """
    Queries nvidia-smi with deep inspection of FB memory, hardware driver reservations,
    active compute processes, and non-headless display buffers to establish true
    usable VRAM budget for local model execution.
    """
    result: Dict[str, Any] = {
        "available": False,
        "name": "No Dedicated GPU",
        "driver_version": "N/A",
        "cuda_version": "N/A",
        "temperature_c": 0,
        "clock_mhz": 0,
        "max_clock_mhz": 0,
        "power_draw_w": 0.0,
        "vram_total_mb": 0,
        "vram_hardware_reserved_mb": 0,
        "vram_used_mb": 0,
        "vram_free_mb": 0,
        "addressable_vram_mb": 0,
        "non_headless_headroom_floor_mb": NON_HEADLESS_VRAM_HEADROOM_MIN_MB,
        "usable_compute_vram_mb": 0,
        "active_compute_apps": [],
        "existing_model_process_vram_mb": 0
    }

    # 1. Query general GPU telemetry via CSV format
    try:
        cmd_csv = [
            "nvidia-smi",
            "--query-gpu=name,driver_version,temperature.gpu,clocks.current.graphics,clocks.max.graphics,power.draw,memory.used,memory.total,memory.free",
            "--format=csv,noheader,nounits"
        ]
        proc = subprocess.run(cmd_csv, capture_output=True, text=True, timeout=2.5)
        if proc.returncode == 0 and proc.stdout.strip():
            parts = [p.strip() for p in proc.stdout.strip().split(",")]
            if len(parts) >= 9:
                result["available"] = True
                result["name"] = parts[0]
                result["driver_version"] = parts[1]
                result["temperature_c"] = int(float(parts[2]))
                result["clock_mhz"] = int(float(parts[3]))
                result["max_clock_mhz"] = int(float(parts[4]))
                result["power_draw_w"] = float(parts[5])
                result["vram_used_mb"] = int(float(parts[6]))
                result["vram_total_mb"] = int(float(parts[7]))
                result["vram_free_mb"] = int(float(parts[8]))
    except Exception:
        pass

    if not result["available"]:
        return result

    # 2. Query detailed Memory XML/Text to extract Hardware Driver Reservation
    try:
        cmd_mem = ["nvidia-smi", "-q", "-d", "MEMORY"]
        proc_mem = subprocess.run(cmd_mem, capture_output=True, text=True, timeout=2.5)
        if proc_mem.returncode == 0 and proc_mem.stdout:
            for line in proc_mem.stdout.splitlines():
                line_str = line.strip()
                if "Reserved" in line_str and "MiB" in line_str:
                    # e.g., "Reserved : 311 MiB"
                    parts = line_str.split(":")
                    if len(parts) >= 2:
                        val_str = parts[1].replace("MiB", "").strip()
                        result["vram_hardware_reserved_mb"] = int(val_str)
                        break
    except Exception:
        pass

    # If reserved not explicitly parsed, default to standard driver reserve (~311 MB on 2GB MX570)
    if result["vram_hardware_reserved_mb"] == 0 and result["vram_total_mb"] > 0:
        result["vram_hardware_reserved_mb"] = 311

    # Addressable memory is Total minus Driver Hardware Reservation
    result["addressable_vram_mb"] = max(0, result["vram_total_mb"] - result["vram_hardware_reserved_mb"])

    # 3. Query Active Compute Applications (e.g. existing llama-server or Ollama processes)
    try:
        cmd_apps = [
            "nvidia-smi",
            "--query-compute-apps=pid,process_name,used_memory",
            "--format=csv,noheader,nounits"
        ]
        proc_apps = subprocess.run(cmd_apps, capture_output=True, text=True, timeout=2.5)
        if proc_apps.returncode == 0 and proc_apps.stdout.strip():
            apps: List[Dict[str, Any]] = []
            model_vram_held = 0
            for row in proc_apps.stdout.strip().splitlines():
                r_parts = [rp.strip() for rp in row.split(",")]
                if len(r_parts) >= 3:
                    app_record = {
                        "pid": int(r_parts[0]),
                        "name": r_parts[1],
                        "used_mb": int(float(r_parts[2]))
                    }
                    apps.append(app_record)
                    if "llama" in r_parts[1].lower() or "ollama" in r_parts[1].lower():
                        model_vram_held += app_record["used_mb"]
            result["active_compute_apps"] = apps
            result["existing_model_process_vram_mb"] = model_vram_held
    except Exception:
        pass

    # 4. Compute True Usable Compute VRAM for model allocation
    # From a cold/clean state (where old model is unloaded):
    # usable = addressable_vram_mb - NON_HEADLESS_VRAM_HEADROOM_MIN_MB
    cold_usable = max(0, result["addressable_vram_mb"] - NON_HEADLESS_VRAM_HEADROOM_MIN_MB)
    result["usable_compute_vram_mb"] = cold_usable

    return result

# ==============================================================================
# BLOCK 4: AUDIT PROCEDURES
# ==============================================================================
def self_audit() -> bool:
    telemetry = probe_gpu_telemetry()
    assert isinstance(telemetry, dict), "GPU telemetry must be dict"
    if telemetry["available"]:
        assert telemetry["vram_total_mb"] > 0, "Total VRAM must be positive"
        assert telemetry["addressable_vram_mb"] > 0, "Addressable VRAM must be positive"
        assert telemetry["usable_compute_vram_mb"] <= telemetry["addressable_vram_mb"]
    return True

# ==============================================================================
# BLOCK 5: CLOSING BLOCK
# ==============================================================================
if __name__ == "__main__":
    g = probe_gpu_telemetry()
    print("=" * 80)
    print("  CORNERSTONE GPU HARDWARE TELEMETRY & VRAM CHASSIS PROBE")
    print("=" * 80)
    print(f"  GPU Name:               {g['name']}")
    print(f"  Driver Version:         {g['driver_version']}")
    print(f"  Physical VRAM Total:    {g['vram_total_mb']} MiB")
    print(f"  Hardware Reserved VRAM: {g['vram_hardware_reserved_mb']} MiB")
    print(f"  Addressable VRAM:       {g['addressable_vram_mb']} MiB")
    print(f"  Non-Headless Headroom:  {g['non_headless_headroom_floor_mb']} MiB (Stability Floor)")
    print(f"  True Usable for Models: {g['usable_compute_vram_mb']} MiB")
    print(f"  Current Free VRAM:      {g['vram_free_mb']} MiB")
    print(f"  Existing Model VRAM:    {g['existing_model_process_vram_mb']} MiB")
    print(f"  Temperature:            {g['temperature_c']} C | Power: {g['power_draw_w']} W")
    if g["active_compute_apps"]:
        print("\n  Active Compute Processes:")
        for app in g["active_compute_apps"]:
            print(f"    - PID {app['pid']}: {app['name']} ({app['used_mb']} MiB)")
    print("=" * 80)
