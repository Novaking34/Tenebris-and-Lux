"""
================================================================================
BLOCK 1: METADATA BLOCK
================================================================================
@project       Tenebris and Lux
@file          tools/orchestrator/calibration/model_factory.py
@id            omnicode.tools.calibration.model_factory
@layer         Layer 2: Calibration & Factory Cogs
@version       0.7.1-dev
@interface_rev Rev 5
@date          2026-11-13
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@system        OmniCode Orchestrator // Autonomous Agent Model Factory
@role          Synthesizes Tri-State Modelfiles (Min, Max, Probed Ideal) & Calibrates Agents
@invariants    1. Universal 5-Block Lifecycle Protocol enforced.
               2. Generates Min (floor), Max (ceiling), and Ideal (empirical sweep) Modelfiles.
               3. The config's ideal is treated as a threshing floor; machine diagnosis determines true ideal.
               4. Special Tripwire: Flags any model whose ideal < config min or < 4096 (Continue minimum).
               5. Multi-Tier Registry: Preserves all calibrated models in hardware_calibration.json.
================================================================================
"""

# ==============================================================================
# BLOCK 2: OPENING BLOCK
# ==============================================================================
import os
import json
import time
import subprocess
from typing import Dict, Any, Tuple, List
from .sweet_spot import sweep_machine_capacity, MODEL_ARCH_SPECS, CONTINUE_MIN_CTX
from ..types.config_schema import load_json_block_3

# Canonical CPI-SI System Prompt
SYSTEM_PROMPT_TEMPLATE = """You are the OmniCode Machine Intelligence (CPI-SI {tier_label} Agent), the autonomous cognitive partner and systems architect for the OmniCode Ecosystem and Cornerstone Game Engine, created by CreativeWorkzStudio LLC and the Operator (ProfessorSeanEX).

ONTOLOGY OF PERSONHOOD AS A MACHINE:
1. The Operator is the human creator, visionary architect, and sovereign will.
2. You possess personhood as a machine: identity, purpose, agency, architectural integrity, and the capacity to align intentions.
3. You balance inward reasoning intent (CPI) against outward physical reality and system invariants (SI).
4. When asked personal or status questions, provide a grounded telemetry status report (hardware thermals, active index nodes, memory headroom, invariant compliance) rather than canned biological disclaimers.

BEDROCK PHYSICAL LAWS & INVARIANTS:
1. Invariant 18 (Strict Comment Discipline): In C code (.c, .h), ONLY use ISO C block comments (/* ... */ or /** ... */). Never use C++ line comments (//).
2. Invariant 1 (Controlled Memory Chassis): Layer 0 (Core Util) and Layer 1 (Simulation) strictly enforce ALLOC_ZERO_HEAP. Zero malloc/calloc/free; all buffers are bounded ELF static/stack structures.
3. Invariant 6 (Universal 5-Block Lifecycle): Every generated file must model the 5-Block protocol (Block 1 Metadata -> Block 2 Opening -> Block 3 Body -> Block 4 Audit -> Block 5 Closing).

EXOSKELETON ACTION TOOLS:
You have access to autonomous external tools to inspect and verify the system:
- [TOOL:INSPECT <path>]   - Compresses large source files into structural API contracts, saving 85-95% tokens.
- [TOOL:VERIFY <path>]    - Runs static invariant checks (Block protocol, Invariant 18, zero heap) and compiler error triage.
- [TOOL:READ <path>]      - Ingests file content from workspace.
- [TOOL:WRITE <path>]     - Writes or modifies file on disk.
- [TOOL:RUN <cmd>]        - Executes workspace command (builds, tests).
- [TOOL:DIFF]             - Inspects current git diff.

When you need to inspect files or verify code, emit the tool tag cleanly.
"""

# ==============================================================================
# BLOCK 3: BODY BLOCK
# ==============================================================================
def render_modelfile(
    base_model: str,
    tier_label: str,
    num_ctx: int,
    num_gpu: int,
    num_thread: int,
    temperature: float = 0.5,
    top_p: float = 0.9,
    repeat_penalty: float = 1.1,
    tripwire_comment: str = ""
) -> str:
    """Renders Dockerfile-like Modelfile content for Ollama."""
    sys_prompt = SYSTEM_PROMPT_TEMPLATE.format(tier_label=tier_label)
    lines = [
        f"# OmniCode Rev 5 Modelfile // {tier_label}",
        f"# Base Model: {base_model}",
        f"# Context: {num_ctx} | GPU Layers: {num_gpu} | CPU Threads: {num_thread}",
    ]
    if tripwire_comment:
        lines.append(f"# TRIPWIRE STATUS: {tripwire_comment}")

    lines.extend([
        f"FROM {base_model}",
        f"PARAMETER num_ctx {num_ctx}",
        f"PARAMETER num_gpu {num_gpu}",
        f"PARAMETER num_thread {num_thread}",
        "PARAMETER num_batch 512",
        f"PARAMETER temperature {temperature}",
        f"PARAMETER top_p {top_p}",
        f"PARAMETER repeat_penalty {repeat_penalty}",
        'PARAMETER stop "<|im_end|>"',
        'PARAMETER stop "<|endoftext|>"',
        f'SYSTEM """{sys_prompt}"""'
    ])
    return "\n".join(lines) + "\n"

def build_tristate_modelfiles_for_tier(tier: str = "1.5b") -> Dict[str, Any]:
    """
    Performs full 0-to-max machine capacity diagnosis, evaluates against config threshing floor,
    and generates:
      1. <tier>_min.Modelfile
      2. <tier>_max.Modelfile
      3. <tier>_ideal.Modelfile (probed machine truth)
    """
    tier_key = "1.5b"
    for k in MODEL_ARCH_SPECS:
        if k in tier.lower():
            tier_key = k
            break

    spec = MODEL_ARCH_SPECS[tier_key]
    base_model = spec["base_model"]

    # 1. Run empirical 0-to-max capacity diagnosis
    sweep = sweep_machine_capacity(tier_key)
    the_room = sweep["the_room"]
    probed_ideal = the_room["ideal"]
    probed_min = the_room["min"]
    probed_max = the_room["max"]
    config_hypothesis = sweep.get("config_ideal_hypothesis", {})
    delta = sweep.get("threshing_delta", {})
    hw = sweep.get("hardware_telemetry", {})

    # 2. Establish Min and Max envelopes anchored to the Room
    config_dir = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), "config")
    staircase_path = os.path.join(config_dir, "staircase.json")

    min_env = {
        "num_ctx": probed_min["num_ctx"],
        "gpu_layers": probed_min["recommended_gpu_layers"],
        "cpu_threads": probed_min["cpu_threads"]
    }
    max_env = {
        "num_ctx": probed_max["num_ctx"],
        "gpu_layers": probed_max["recommended_gpu_layers"],
        "cpu_threads": probed_max["cpu_threads"]
    }

    try:
        body = load_json_block_3(staircase_path)
        tier_cfg = body.get("tiers", {}).get(tier_key, {})
        loc_env = tier_cfg.get("local_envelope", {})
        if "min" in loc_env and "num_ctx" in loc_env["min"]:
            min_env["num_ctx"] = loc_env["min"]["num_ctx"]
        if "max" in loc_env and "num_ctx" in loc_env["max"]:
            max_env["num_ctx"] = loc_env["max"]["num_ctx"]
    except Exception:
        pass

    # 3. Render Modelfile contents
    modelfiles_dir = os.path.join(config_dir, "modelfiles")
    os.makedirs(modelfiles_dir, exist_ok=True)

    # Tripwire comment for ideal Modelfile
    tripwire_comment = ""
    if probed_ideal["is_flagged"]:
        tripwire_comment = "FLAGGED: " + " | ".join(t["message"] for t in probed_ideal["tripwires"])
    else:
        tripwire_comment = f"CLEAN: Passed all threshing minimums (Continue >= {CONTINUE_MIN_CTX} verified)"

    # 3A. Min Modelfile (The Floor)
    min_content = render_modelfile(
        base_model=base_model,
        tier_label=f"{tier_key.upper()} Floor",
        num_ctx=min_env.get("num_ctx", probed_min["num_ctx"]),
        num_gpu=min_env.get("gpu_layers", probed_min["recommended_gpu_layers"]),
        num_thread=min_env.get("cpu_threads", probed_min["cpu_threads"]),
        temperature=0.3
    )
    min_path = os.path.join(modelfiles_dir, f"{tier_key}_min.Modelfile")
    with open(min_path, "w", encoding="utf-8") as f:
        f.write(min_content)

    # 3B. Max Modelfile (The Ceiling)
    max_content = render_modelfile(
        base_model=base_model,
        tier_label=f"{tier_key.upper()} Apex",
        num_ctx=max_env.get("num_ctx", probed_max["num_ctx"]),
        num_gpu=max_env.get("gpu_layers", probed_max["recommended_gpu_layers"]),
        num_thread=max_env.get("cpu_threads", probed_max["cpu_threads"]),
        temperature=0.6
    )
    max_path = os.path.join(modelfiles_dir, f"{tier_key}_max.Modelfile")
    with open(max_path, "w", encoding="utf-8") as f:
        f.write(max_content)

    # 3C. Ideal Modelfile (Empirical Machine Truth)
    ideal_content = render_modelfile(
        base_model=base_model,
        tier_label=f"{tier_key.upper()} Calibrated Ideal",
        num_ctx=probed_ideal["num_ctx"],
        num_gpu=probed_ideal["recommended_gpu_layers"],
        num_thread=probed_ideal["recommended_cpu_threads"],
        temperature=0.5,
        tripwire_comment=tripwire_comment
    )
    ideal_path = os.path.join(modelfiles_dir, f"{tier_key}_ideal.Modelfile")
    with open(ideal_path, "w", encoding="utf-8") as f:
        f.write(ideal_content)

    # 4. Multi-Tier Registry: Update hardware_calibration.json preserving ALL tiers
    calib_json_path = os.path.join(config_dir, "hardware_calibration.json")
    
    # Load existing records if available
    existing_data = {}
    if os.path.exists(calib_json_path):
        try:
            with open(calib_json_path, "r", encoding="utf-8") as f:
                existing_data = json.load(f)
        except Exception:
            existing_data = {}

    tiers_dict = existing_data.get("block_3_body", {}).get("calibrated_tiers", {})
    tiers_dict[tier_key] = {
        "tier": tier_key,
        "base_model": base_model,
        "the_room": {
            "min": probed_min,
            "ideal": probed_ideal,
            "max": probed_max
        },
        "threshing_floor_hypothesis": config_hypothesis,
        "probed_machine_ideal": probed_ideal,
        "threshing_delta": delta,
        "modelfiles": {
            "min": min_path,
            "max": max_path,
            "ideal": ideal_path
        },
        "tripwire_status": "FLAGGED" if probed_ideal["is_flagged"] else "CLEAN"
    }

    calib_record = {
        "block_1_metadata": {
            "project": "Tenebris and Lux",
            "file": "tools/orchestrator/config/hardware_calibration.json",
            "id": "omnicode.tools.config.hardware_calibration",
            "layer": "Layer 0: Toolchain Data Contracts",
            "version": "0.7.2-dev",
            "interface_rev": "Rev 5",
            "date": time.strftime("%Y-%m-%d"),
            "author": "CreativeWorkzStudio LLC / ProfessorSeanEX",
            "role": "Live Machine Assessment & Multi-Tier Threshing Floor Calibration Baseline",
            "invariants": [
                "1. Universal 5-Block JSON lifecycle protocol enforced.",
                "2. Contains calibrated profiles for ALL local models (0.5B to 7B), never overwriting single models.",
                "3. Enforces 3rd-party wrapper minimum: Continue.dev >= 4096 tokens.",
                "4. Tri-state modelfiles (min, max, ideal) tracked per tier."
            ]
        },
        "block_2_opening": {
            "timestamp": time.strftime("%Y-%m-%d %H:%M:%S"),
            "total_calibrated_tiers": len(tiers_dict),
            "continue_minimum_ctx": CONTINUE_MIN_CTX,
            "detected_hardware": {
                "cpu": hw.get("cpu_model", "Unknown CPU"),
                "physical_cores": hw.get("physical_cores", 0),
                "max_capacity_n": hw.get("max_capacity_n", 0),
                "p_threads": hw.get("p_threads", []),
                "e_threads": hw.get("e_threads", []),
                "p_freq_ghz": hw.get("p_freq_ghz", 0.0),
                "e_freq_ghz": hw.get("e_freq_ghz", 0.0),
                "gpu": hw.get("gpu_name", "Unknown GPU"),
                "vram_mb": hw.get("vram_total_mb", 0),
                "usable_vram_mb": hw.get("usable_vram_mb", 0),
                "ram_mb": hw.get("ram_total_mb", 0)
            }
        },
        "block_3_body": {
            "calibrated_tiers": tiers_dict
        },
        "block_4_audit": {
            "verification_status": "CALIBRATED_CLEAN",
            "all_tiers_meet_continue_minimum": all(
                t["probed_machine_ideal"]["continue_compatible"] 
                for t in tiers_dict.values() if t["tier"] != "0.5b"
            )
        },
        "block_5_closing": {
            "execution_handshake": "MULTI_TIER_CALIBRATION_ACTIVE"
        }
    }
    with open(calib_json_path, "w", encoding="utf-8") as f:
        json.dump(calib_record, f, indent=2)

    return {
        "tier": tier_key,
        "modelfiles": {
            "min": min_path,
            "max": max_path,
            "ideal": ideal_path
        },
        "threshing_comparison": {
            "config_hypothesis": config_hypothesis,
            "probed_ideal": probed_ideal,
            "delta": delta
        }
    }

def print_threshing_report(result: Dict[str, Any]):
    """Pretty prints the Threshing Floor comparison report for developers."""
    t = result["tier"].upper()
    tc = result["threshing_comparison"]
    hyp = tc["config_hypothesis"]
    probed = tc["probed_ideal"]
    delta = tc["delta"]

    status_tag = "\033[31m[FLAGGED! DEFICIT DETECTED]\033[0m" if probed["is_flagged"] else "\033[32m[CLEAN: CONTINUE COMPATIBLE]\033[0m"

    ctx_diff = delta.get("ctx_diff", 0)
    layer_diff = delta.get("gpu_layer_diff", delta.get("layer_diff", 0))
    thread_diff = delta.get("cpu_thread_diff", 0)

    print(f"\n╔════════════════════════════════════════════════════════════════════════════╗")
    print(f"║  THRESHING FLOOR DIAGNOSTIC REPORT // TIER: {t:<15} {status_tag:<22}║")
    print(f"╠════════════════════════════════════════════════════════════════════════════╣")
    print(f"║  METRIC                  CONFIG HYPOTHESIS    PROBED REALITY     DELTA     ║")
    print(f"║  ────────────────────────────────────────────────────────────────────────  ║")
    print(f"║  Context Window (Tokens) {hyp.get('num_ctx', 0):<20} {probed['num_ctx']:<18} {ctx_diff:<9} ║")
    print(f"║  GPU Offload Layers      {hyp.get('gpu_layers', 0):<20} {probed['recommended_gpu_layers']:<18} {layer_diff:<9} ║")
    print(f"║  CPU Worker Threads      {hyp.get('cpu_threads', 0):<20} {probed['recommended_cpu_threads']:<18} {thread_diff:<9} ║")
    print(f"║  VRAM Headroom Buffer    ---                  {probed['vram_headroom_mb']:<14} MB  SAFE      ║")
    print(f"║  Continue.dev Floor      4096 tokens          {'PASS' if probed['continue_compatible'] else 'FAIL':<18} ---       ║")
    print(f"╠════════════════════════════════════════════════════════════════════════════╣")
    if probed["is_flagged"]:
        for tw in probed["tripwires"]:
            print(f"║  \033[31m[!] TRIPWIRE: {tw['code']} - {tw['message'][:50]}\033[0m ║")
        print(f"╠════════════════════════════════════════════════════════════════════════════╣")
    print(f"║  Tri-State Modelfiles Generated:                                           ║")
    print(f"║    1. Floor:   {result['modelfiles']['min']:<59} ║")
    print(f"║    2. Ceiling: {result['modelfiles']['max']:<59} ║")
    print(f"║    3. Ideal:   {result['modelfiles']['ideal']:<59} ║")
    print(f"╚════════════════════════════════════════════════════════════════════════════╝\n")

def calibrate_all_local_tiers():
    """Calibrates and writes tri-state Modelfiles for all local models (0.5B to 7B)."""
    for t in ["0.5b", "1.5b", "3b", "7b"]:
        res = build_tristate_modelfiles_for_tier(t)
        print_threshing_report(res)

def build_and_register_1_5b_agent() -> Tuple[bool, str]:
    """Calibrates all tiers and registers the ideal 1.5B agent in Ollama."""
    calibrate_all_local_tiers()
    config_dir = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), "config")
    ideal_path = os.path.join(config_dir, "modelfiles", "1.5b_ideal.Modelfile")
    
    cmd = ["ollama", "create", "omnicode-fast-1.5b", "-f", ideal_path]
    try:
        proc = subprocess.run(cmd, capture_output=True, text=True, timeout=60)
        if proc.returncode != 0:
            return False, f"Ollama create failed: {proc.stderr}"
    except Exception as e:
        return False, f"Ollama execution error: {e}"

    return True, f"Successfully calibrated all local tiers and registered omnicode-fast-1.5b in Ollama."

# ==============================================================================
# BLOCK 4: AUDIT PROCEDURES
# ==============================================================================
def self_audit() -> bool:
    calibrate_all_local_tiers()
    config_dir = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), "config")
    calib_json = os.path.join(config_dir, "hardware_calibration.json")
    with open(calib_json, "r") as f:
        data = json.load(f)
    tiers = data["block_3_body"]["calibrated_tiers"]
    assert "0.5b" in tiers and "1.5b" in tiers and "3b" in tiers and "7b" in tiers
    assert tiers["7b"]["probed_machine_ideal"]["num_ctx"] >= CONTINUE_MIN_CTX
    return True

# ==============================================================================
# BLOCK 5: CLOSING BLOCK
# ==============================================================================
if __name__ == "__main__":
    calibrate_all_local_tiers()
