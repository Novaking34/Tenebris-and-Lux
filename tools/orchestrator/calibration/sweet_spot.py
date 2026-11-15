"""
================================================================================
BLOCK 1: METADATA BLOCK
================================================================================
@project       Tenebris and Lux
@file          tools/orchestrator/calibration/sweet_spot.py
@id            omnicode.tools.calibration.sweet_spot
@layer         Layer 2: Calibration & Factory Cogs
@version       0.7.3-dev
@interface_rev Rev 5
@date          2026-11-13
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@system        OmniCode Orchestrator // Empirical Zero-Assumption PC Scanner
@role          Scans Hardware & Establishes the Physical Room (Min <= Ideal <= Max N)
@invariants    1. Zero external pip dependencies.
               2. Pure empirical calculation based on physical hardware telemetry.
               3. CPU Max is dynamic physical N (12 threads on Core 7 150U, not arbitrary 6).
               4. GPU VRAM accounts for driver hardware reservation (311 MiB) & non-headless headroom (350 MiB).
               5. Host RAM enforces non-headless desktop reserve (3072 MiB) protecting Wayland/X11 & IDE.
               6. The Room Invariant: min <= ideal <= max enforced across CPU, GPU, and Context.
               7. The Sweet Spot Toward the Max: Pushes capacity upward toward ceiling while guarding headroom.
               8. 3rd-Party Wrapper Minimum: Hard floor of 4096 tokens (prevents Continue.dev crash).
================================================================================
"""

# ==============================================================================
# BLOCK 2: OPENING BLOCK
# ==============================================================================
import json
import os
from typing import Dict, Any, Tuple, List
from ..probes.cpu_probe import probe_cpu_topology
from ..probes.gpu_probe import probe_gpu_telemetry
from ..probes.memory_probe import probe_system_memory
from ..types.config_schema import load_json_block_3

# 3rd-Party Wrapper Minimum Constants
CONTINUE_MIN_CTX = 4096          # Absolute floor for Continue.dev to prevent "message exceeds context limit"
WORKSPACE_IDEAL_CTX = 8192       # Recommended minimum for multi-file workspace coding
QWEN_NATIVE_PRETRAIN_CTX = 32768 # Qwen 2.5 Coder native pretraining context
QWEN_MAX_YARN_CTX = 131072       # Qwen 2.5 Coder YaRN extended maximum

# Architecture specifications for Qwen2.5-Coder spectrum
MODEL_ARCH_SPECS = {
    "0.5b": {
        "total_layers": 24,
        "hidden_size": 896,
        "num_heads": 14,
        "num_kv_heads": 2,
        "weight_bytes": 397 * 1024 * 1024,
        "is_local_ceiling": False,
        "base_model": "qwen2.5-coder:0.5b",
        "min_viable_ctx": 2048,
        "target_sweet_ctx": 8192,
        "max_ctx_ceiling": 8192
    },
    "1.5b": {
        "total_layers": 28,
        "hidden_size": 1536,
        "num_heads": 12,
        "num_kv_heads": 2,
        "weight_bytes": 986 * 1024 * 1024,
        "is_local_ceiling": False,
        "base_model": "qwen2.5-coder:1.5b",
        "min_viable_ctx": 4096,
        "target_sweet_ctx": 8192,
        "max_ctx_ceiling": 32768
    },
    "3b": {
        "total_layers": 36,
        "hidden_size": 2048,
        "num_heads": 16,
        "num_kv_heads": 2,
        "weight_bytes": 1930 * 1024 * 1024,
        "is_local_ceiling": False,
        "base_model": "qwen2.5-coder:3b",
        "min_viable_ctx": 4096,
        "target_sweet_ctx": 8192,
        "max_ctx_ceiling": 16384
    },
    "7b": {
        "total_layers": 28,
        "hidden_size": 3584,
        "num_heads": 28,
        "num_kv_heads": 4,
        "weight_bytes": 4700 * 1024 * 1024,
        "is_local_ceiling": True,
        "base_model": "qwen2.5-coder:7b",
        "min_viable_ctx": 4096,
        "target_sweet_ctx": 8192,
        "max_ctx_ceiling": 16384
    }
}

# ==============================================================================
# BLOCK 3: BODY BLOCK
# ==============================================================================
def calculate_kv_cache_bytes(tier: str, context_len: int) -> int:
    """
    Computes exact KV cache allocation in bytes:
    KV = 2 * layers * kv_heads * head_dim * context_len * sizeof(fp16)
    head_dim = hidden_size / num_heads
    """
    spec = MODEL_ARCH_SPECS.get(tier, MODEL_ARCH_SPECS["1.5b"])
    head_dim = spec["hidden_size"] // spec["num_heads"]
    return 2 * spec["total_layers"] * spec["num_kv_heads"] * head_dim * context_len * 2

def sweep_machine_capacity(tier: str = "1.5b") -> Dict[str, Any]:
    """
    Establishes 'The Room' and synthesizes the 'Sweet Spot Toward the Max':
    1. Probed Min: Safe bedrock floor (Continue >= 4096 tokens, safe minimal offload).
    2. Probed Max: Physical apex capacity (N=12 threads, maximum addressable VRAM/RAM).
    3. Probed Ideal: The Sweet Spot Toward the Max, pushing context and layers as close
       to maximum as physically possible while strictly honoring:
       - Non-headless VRAM headroom (>= 350 MB)
       - Non-headless Desktop RAM reserve (>= 3072 MB)
    """
    cpu = probe_cpu_topology()
    gpu = probe_gpu_telemetry()
    ram = probe_system_memory()

    tier_key = "1.5b"
    for k in MODEL_ARCH_SPECS:
        if k in tier.lower():
            tier_key = k
            break

    spec = MODEL_ARCH_SPECS[tier_key]
    total_layers = spec["total_layers"]
    max_capacity_n = cpu["max_capacity_n"]  # Dynamic N (12 on Core 7 150U)

    # Telemetry Grounding from GPU & Memory Probes
    vram_total_mb = gpu["vram_total_mb"]
    vram_reserved_mb = gpu.get("vram_hardware_reserved_mb", 311)
    addressable_vram_mb = gpu.get("addressable_vram_mb", max(0, vram_total_mb - vram_reserved_mb))
    non_headless_vram_floor_mb = gpu.get("non_headless_headroom_floor_mb", 350)
    usable_compute_vram_mb = gpu.get("usable_compute_vram_mb", max(0, addressable_vram_mb - non_headless_vram_floor_mb))

    ram_total_mb = ram["total_mb"]
    is_non_headless = ram.get("is_non_headless", True)
    desktop_reserved_ram_mb = ram.get("desktop_reserved_mb", 3072)
    safe_model_ram_cold_mb = ram.get("cold_max_model_ram_mb", max(0, ram_total_mb - desktop_reserved_ram_mb - 1024))

    layer_weight_mb = (spec["weight_bytes"] / (1024 * 1024)) / total_layers

    # --------------------------------------------------------------------------
    # 1. Establish the MINIMUM Boundary (The Floor of the Room)
    # --------------------------------------------------------------------------
    probed_min = {
        "num_ctx": spec["min_viable_ctx"],
        "recommended_gpu_layers": 0 if tier_key == "7b" else min(10, total_layers),
        "cpu_threads": 1,
        "description": "Absolute Minimum Viable Floor"
    }

    # --------------------------------------------------------------------------
    # 2. Establish the MAXIMUM Boundary (The Ceiling of the Room: N Threads)
    # --------------------------------------------------------------------------
    max_ctx = spec["max_ctx_ceiling"]
    max_gpu_layers = min(total_layers, int(usable_compute_vram_mb / layer_weight_mb))
    if tier_key == "7b":
        max_gpu_layers = min(total_layers, 5)

    probed_max = {
        "num_ctx": max_ctx,
        "recommended_gpu_layers": max_gpu_layers if max_gpu_layers > 0 else total_layers,
        "cpu_threads": max_capacity_n,  # True physical N (12)
        "description": f"Physical Ceiling (Full N={max_capacity_n} Thread Capacity, Apex Offload)"
    }

    # --------------------------------------------------------------------------
    # 3. Establish the IDEAL Boundary (The Sweet Spot Toward the Max)
    # --------------------------------------------------------------------------
    # CPU Allocation Strategy:
    # - 0.5B / 1.5B: P-Burst (4 threads @ 5.4 GHz) for minimum latency token velocity
    # - 3B: Hybrid (8 threads: 4 P + 4 E)
    # - 7B: Memory Bus Max (10 threads in DDR5 RAM to saturate memory bandwidth)
    if tier_key in ("0.5b", "1.5b"):
        ideal_threads = cpu["optimal_p_burst"]  # 4 threads @ 5.4 GHz
    elif tier_key == "3b":
        ideal_threads = cpu["optimal_hybrid"]   # 8 threads
    else:  # 7b
        ideal_threads = min(max_capacity_n, 10) # 10 threads saturating DDR5 memory bus

    # Aim context toward the max (16k or 8k sweet spot):
    target_sweet_ctx = spec["target_sweet_ctx"]
    sweet_kv_mb = calculate_kv_cache_bytes(tier_key, target_sweet_ctx) / (1024 * 1024)

    # Search for maximum GPU layers that respect the non-headless headroom floor
    if tier_key == "7b":
        # 7B: Offload 4-5 layers to GPU, keep majority in 16GB DDR5 RAM
        ideal_gpu_layers = 4
        model_vram_usage = (ideal_gpu_layers * layer_weight_mb)
        vram_headroom = addressable_vram_mb - model_vram_usage
    else:
        # Offload maximum layers while keeping headroom >= non_headless_vram_floor_mb (350 MB)
        avail_for_layers = addressable_vram_mb - non_headless_vram_floor_mb - sweet_kv_mb
        ideal_gpu_layers = min(total_layers, max(0, int(avail_for_layers / layer_weight_mb)))
        model_vram_usage = sweet_kv_mb + (ideal_gpu_layers * layer_weight_mb)
        vram_headroom = addressable_vram_mb - model_vram_usage

        # Fine-tune toward the max while preserving safety buffer
        while ideal_gpu_layers > 0 and vram_headroom < non_headless_vram_floor_mb:
            ideal_gpu_layers -= 1
            model_vram_usage = sweet_kv_mb + (ideal_gpu_layers * layer_weight_mb)
            vram_headroom = addressable_vram_mb - model_vram_usage

    # System RAM consumption verification:
    layers_in_ram = total_layers - ideal_gpu_layers
    ram_weight_mb = layers_in_ram * layer_weight_mb
    ram_usage_mb = ram_weight_mb + (0 if tier_key != "7b" else sweet_kv_mb)
    ram_safe = ram_usage_mb <= safe_model_ram_cold_mb

    # --------------------------------------------------------------------------
    # 4. Enforce THE ROOM INVARIANT: min <= ideal <= max N
    # --------------------------------------------------------------------------
    ideal_ctx_clamped = max(probed_min["num_ctx"], min(probed_max["num_ctx"], target_sweet_ctx))
    ideal_gpu_clamped = max(probed_min["recommended_gpu_layers"], min(probed_max["recommended_gpu_layers"], ideal_gpu_layers))
    ideal_cpu_clamped = max(probed_min["cpu_threads"], min(probed_max["cpu_threads"], ideal_threads))

    tripwires = []
    is_flagged = False

    # Check Continue compatibility
    if ideal_ctx_clamped < CONTINUE_MIN_CTX:
        is_flagged = True
        tripwires.append({
            "code": "FLAGGED_3RD_PARTY_WRAPPER_DEFICIT",
            "message": f"Context window ({ideal_ctx_clamped}) is below Continue minimum ({CONTINUE_MIN_CTX})!"
        })

    # Check if ideal attempted to breach the room
    if target_sweet_ctx < probed_min["num_ctx"] or target_sweet_ctx > probed_max["num_ctx"]:
        is_flagged = True
        tripwires.append({
            "code": "FLAGGED_MODEL_OUTSIDE_ROOM",
            "message": f"Context hypothesis ({target_sweet_ctx}) was outside the physical room [{probed_min['num_ctx']}, {probed_max['num_ctx']}]!"
        })

    # Check RAM safety breach
    if not ram_safe:
        is_flagged = True
        tripwires.append({
            "code": "FLAGGED_NON_HEADLESS_RAM_DEFICIT",
            "message": f"Model RAM allocation ({ram_usage_mb:.1f} MB) exceeds safe cold budget ({safe_model_ram_cold_mb} MB)!"
        })

    probed_ideal = {
        "num_ctx": ideal_ctx_clamped,
        "recommended_gpu_layers": ideal_gpu_clamped,
        "recommended_cpu_threads": ideal_cpu_clamped,
        "cpu_threads": ideal_cpu_clamped,
        "kv_cache_mb": round(sweet_kv_mb, 1),
        "vram_headroom_mb": round(vram_headroom, 1),
        "ram_usage_mb": round(ram_usage_mb, 1),
        "ram_headroom_mb": round(safe_model_ram_cold_mb - ram_usage_mb, 1),
        "continue_compatible": ideal_ctx_clamped >= CONTINUE_MIN_CTX,
        "is_flagged": is_flagged,
        "tripwires": tripwires,
        "description": "Sweet Spot Toward the Max (Guarding Non-Headless Headroom)"
    }

    # Load config hypothesis for threshing floor comparison
    config_ideal = {"num_ctx": 16384, "gpu_layers": 20, "cpu_threads": 6}
    try:
        config_path = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), "config", "staircase.json")
        body = load_json_block_3(config_path)
        tier_cfg = body.get("tiers", {}).get(tier_key, {})
        cfg_env = tier_cfg.get("local_envelope", {}).get("ideal", {})
        if cfg_env:
            config_ideal["num_ctx"] = cfg_env.get("num_ctx", 16384)
            config_ideal["gpu_layers"] = cfg_env.get("gpu_layers", 20)
            config_ideal["cpu_threads"] = cfg_env.get("cpu_threads", 6)
    except Exception:
        pass

    delta = {
        "ctx_diff": probed_ideal["num_ctx"] - config_ideal["num_ctx"],
        "gpu_layer_diff": probed_ideal["recommended_gpu_layers"] - config_ideal["gpu_layers"],
        "layer_diff": probed_ideal["recommended_gpu_layers"] - config_ideal["gpu_layers"],
        "cpu_thread_diff": probed_ideal["recommended_cpu_threads"] - config_ideal["cpu_threads"]
    }

    return {
        "tier": tier_key,
        "base_model": spec["base_model"],
        "hardware_telemetry": {
            "cpu_model": cpu["model_name"],
            "physical_cores": cpu["physical_cores"],
            "max_capacity_n": max_capacity_n,
            "p_threads": cpu["p_threads"],
            "e_threads": cpu["e_threads"],
            "p_freq_ghz": cpu["p_freq_ghz"],
            "e_freq_ghz": cpu["e_freq_ghz"],
            "gpu_name": gpu["name"],
            "vram_total_mb": vram_total_mb,
            "vram_hardware_reserved_mb": vram_reserved_mb,
            "addressable_vram_mb": addressable_vram_mb,
            "non_headless_vram_floor_mb": non_headless_vram_floor_mb,
            "usable_vram_mb": usable_compute_vram_mb,
            "ram_total_mb": ram_total_mb,
            "is_non_headless": is_non_headless,
            "desktop_reserved_ram_mb": desktop_reserved_ram_mb,
            "safe_model_ram_cold_mb": safe_model_ram_cold_mb
        },
        "the_room": {
            "min": probed_min,
            "ideal": probed_ideal,
            "max": probed_max
        },
        "probed_machine_ideal": probed_ideal,
        "config_ideal_hypothesis": config_ideal,
        "threshing_delta": delta,
        "delta": delta,
        "detected_gpu": gpu["name"],
        "detected_vram_mb": vram_total_mb,
        "usable_vram_mb": usable_compute_vram_mb,
        "detected_cpu": cpu["model_name"],
        "detected_ram_mb": ram_total_mb
    }

def scan_full_spectrum() -> Dict[str, Any]:
    """Scans and sweeps the entire local model spectrum from 0.5B to 7B establishing all rooms."""
    spectrum = {}
    for tier in ["0.5b", "1.5b", "3b", "7b"]:
        spectrum[tier] = sweep_machine_capacity(tier)
    return spectrum

# ==============================================================================
# BLOCK 4: AUDIT PROCEDURES
# ==============================================================================
def self_audit() -> bool:
    for t in ["0.5b", "1.5b", "3b", "7b"]:
        sweep = sweep_machine_capacity(t)
        room = sweep["the_room"]
        # Enforce Room Invariant: min <= ideal <= max
        assert room["min"]["num_ctx"] <= room["ideal"]["num_ctx"] <= room["max"]["num_ctx"], f"{t} context broke room invariant"
        assert room["min"]["cpu_threads"] <= room["ideal"]["cpu_threads"] <= room["max"]["cpu_threads"], f"{t} cpu threads broke room invariant"
        assert room["ideal"]["num_ctx"] >= CONTINUE_MIN_CTX or t == "0.5b"
        # Non-headless headroom check
        assert room["ideal"]["vram_headroom_mb"] >= 340.0, f"{t} VRAM headroom {room['ideal']['vram_headroom_mb']} breached non-headless floor"
    return True

# ==============================================================================
# BLOCK 5: CLOSING BLOCK
# ==============================================================================
if __name__ == "__main__":
    print("=" * 80)
    print("  OMNICODE THE ROOM ENGINE // SWEET SPOT TOWARD THE MAX SCANNER")
    print("=" * 80)
    for t in ["0.5b", "1.5b", "3b", "7b"]:
        s = sweep_machine_capacity(t)
        r = s["the_room"]
        hw = s["hardware_telemetry"]
        print(f"\n╔═══════════════════════════════════════════════════════════════════════════════╗")
        print(f"║ TIER {t.upper():<5} ROOM: [MIN] <= [IDEAL: SWEET SPOT TOWARD MAX] <= [MAX: APEX N]        ║")
        print(f"╠═══════════════════════════════════════════════════════════════════════════════╣")
        print(f"║  FLOOR (MIN): Ctx={r['min']['num_ctx']:<5} | GPU Layers={r['min']['recommended_gpu_layers']:<2} | CPU={r['min']['cpu_threads']:<2}                       ║")
        print(f"║  IDEAL:       Ctx={r['ideal']['num_ctx']:<5} | GPU Layers={r['ideal']['recommended_gpu_layers']:<2} | CPU={r['ideal']['cpu_threads']:<2}                       ║")
        print(f"║               VRAM Headroom: {r['ideal']['vram_headroom_mb']:<6} MB (Floor: {hw['non_headless_vram_floor_mb']} MB)                 ║")
        print(f"║               RAM Headroom:  {r['ideal']['ram_headroom_mb']:<6} MB (Desktop Reserve: {hw['desktop_reserved_ram_mb']} MB)          ║")
        print(f"║  APEX (MAX):  Ctx={r['max']['num_ctx']:<5} | GPU Layers={r['max']['recommended_gpu_layers']:<2} | CPU={r['max']['cpu_threads']:<2} (Full N={hw['max_capacity_n']} Threads)        ║")
        print(f"╚═══════════════════════════════════════════════════════════════════════════════╝")
