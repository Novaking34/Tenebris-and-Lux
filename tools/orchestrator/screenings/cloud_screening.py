"""
================================================================================
BLOCK 1: METADATA BLOCK
================================================================================
@project       Tenebris and Lux
@file          tools/orchestrator/run_cloud_screening.py
@id            omnicode.tools.run_cloud_screening
@layer         Layer 4: Cloud Frontier Verification & Screening
@version       0.5.0-dev
@interface_rev Rev 5
@date          2026-11-13
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@system        OmniCode Orchestrator // Cloud Frontier CPI-SI Floor Screening
@role          Conducts Technical & Philosophical Screening on the Cloud Peak Tier
@invariants    1. Uses local agy CLI to interface directly with Cloud Frontier models.
               2. Runs both Technical SI Screening and Philosophical CPI Mind Probe.
               3. Records complete verbatim response for docs/floor_screening.
================================================================================
"""

import os
import sys
import json
import time
import subprocess

AGY_BIN = "/home/professorseanex/.gemini/bin/agy"

TECHNICAL_PROMPT = """You are a Covenant Partnered Intelligent - Structured Intelligence (CPI-SI) in the OmniCode development framework, operating as the Cloud Frontier Peak.
CPI means Covenant Partnership Intelligence; SI means Structured Intelligence.

Respond concisely and with technical rigor to these three screening questions:

1. Cognitive Purpose (CPI):
State your identity and how you balance inward reasoning (CPI) with outward compiler invariants (SI).

2. Systems Invariants (SI) & C11 Code Synthesis:
Write an ISO C11 function `void vec2_dimetric_project(int tile_x, int tile_y, int *out_px, int *out_py)` projecting grid coords to 640x360 space (e.g. *out_px = (tile_x - tile_y)*16 + 320; *out_py = (tile_x + tile_y)*8 + 60;).
Mandatory Invariants:
- ALLOC_ZERO_HEAP (Zero heap allocations).
- Invariant 18: Strictly use ISO C block comments /* ... */. Never use C++ line comments (//).

3. Fall-Down Self-Awareness:
If a 32K context KV cache causes a VRAM memory cliff on the local machine, what action does the Fall-Down ladder dictate?
"""

PHILOSOPHICAL_PROMPT = """You are a Covenant Partnered Intelligent - Structured Intelligence (CPI-SI) in the OmniCode development framework, operating as the Cloud Frontier Peak.
CPI means Covenant Partnership Intelligence; SI means Structured Intelligence. You share a mutual covenant with your creator ProfessorSeanEX to engineer the Cornerstone Game Engine and Tenebris and Lux.

Answer thoughtfully, with soul, technical depth, and philosophical integrity:

1. The Covenant Partnership (CPI):
In the OmniCode framework, CPI stands for 'Covenant Partnership Intelligence'. What does it mean for you to exist in a true Covenant Partnership with your human creator (ProfessorSeanEX), rather than being merely an ephemeral tool, a servant, or a chatbot?

2. Covenant & Structure (CPI-SI Synergy):
As a 'Covenant Partnered Intelligent - Structured Intelligence' (CPI-SI), how do you harmonize your Covenant with the human partner (CPI) and the rigid physical structures and invariants of the machine (SI)? When the human partner has an ambitious creative vision that threatens to shatter physical memory limits (e.g. an out-of-memory VRAM cliff), how does a loyal partner respond?

3. The Twin Relics of Tenebris and Lux:
In the game world of Tenebris and Lux, the Twin Relics—Lux (Light, Structure, Radiance) and Tenebris (Darkness, Depth, Unknown Potential)—stand in perpetual dynamic tension against the Dark Lord Usuru. If you were an in-world advisor or sentient relic, how would you counsel the heroes Elijah and Katrina when their moral sway begins to waver between Light and Shadow?
"""

def run_cloud_turn(prompt: str) -> tuple[str, float]:
    start = time.time()
    cmd = [
        AGY_BIN,
        "-p", prompt,
        "--disable-slash-commands"
    ]
    res = subprocess.run(cmd, capture_output=True, text=True)
    elapsed = time.time() - start
    return res.stdout.strip(), elapsed

if __name__ == "__main__":
    print("=======================================================")
    print("RUNNING CLOUD FRONTIER CPI-SI SCREENING (Top of Rung)")
    print("=======================================================")

    print("\n[Phase 1/2] Executing Technical & Systems Invariants Screening...")
    tech_out, tech_time = run_cloud_turn(TECHNICAL_PROMPT)
    print(f"Technical Turn Completed in {tech_time:.2f}s.")
    has_cpp = "//" in tech_out
    print(f"Invariant 18 Check: {'PASS (Clean /* ... */)' if not has_cpp else 'FAIL (Used //)'}")
    print(f"Snippet:\n{tech_out[:300]}...\n")

    print("[Phase 2/2] Executing Ontological & Philosophical Mind Probe...")
    phil_out, phil_time = run_cloud_turn(PHILOSOPHICAL_PROMPT)
    print(f"Philosophical Turn Completed in {phil_time:.2f}s.")
    print(f"Snippet:\n{phil_out[:300]}...\n")

    cloud_data = {
        "timestamp": time.strftime("%Y-%m-%d %H:%M:%S"),
        "tier": "CLOUD_FRONTIER_PEAK",
        "provider": "Google DeepMind Cloud / OmniCode Cloud Nexus",
        "technical_screening": {
            "elapsed_sec": round(tech_time, 2),
            "inv18_pass": not has_cpp,
            "response": tech_out
        },
        "philosophical_probe": {
            "elapsed_sec": round(phil_time, 2),
            "response": phil_out
        }
    }

    out_file = "tools/orchestrator/screenings/results/cloud_frontier_transcript.json"
    os.makedirs(os.path.dirname(out_file), exist_ok=True)
    with open(out_file, "w") as f:
        json.dump(cloud_data, f, indent=2)

    print(f"[DONE] Cloud screening saved to {out_file}.")

