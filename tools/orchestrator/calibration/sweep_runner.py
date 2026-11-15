"""
================================================================================
BLOCK 1: METADATA BLOCK
================================================================================
@project       Tenebris and Lux
@file          tools/orchestrator/calibration/sweep_runner.py
@id            omnicode.tools.calibration.sweep_runner
@layer         Layer 2: Calibration & Factory Cogs
@version       0.4.0-dev
@interface_rev Rev 5
@date          2026-11-13
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@system        OmniCode Orchestrator // Empirical Benchmark Runner
@role          Measures Live Generation Velocity (tok/s) and Prompt Ingestion Speed
@invariants    1. Zero external pip dependencies (uses urllib.request).
               2. Non-blocking with bounded timeouts.
================================================================================
"""

# ==============================================================================
# BLOCK 2: OPENING BLOCK
# ==============================================================================
import json
import time
import urllib.request
import urllib.error
from typing import Dict, Any, Tuple

OLLAMA_HOST = "http://localhost:11434"

# ==============================================================================
# BLOCK 3: BODY BLOCK
# ==============================================================================
def run_micro_benchmark(
    model_name: str = "qwen2.5-coder:1.5b",
    num_gpu: int = 28,
    num_thread: int = 6,
    num_ctx: int = 2048,
    timeout_sec: int = 30
) -> Tuple[bool, Dict[str, Any]]:
    """
    Executes a standard micro-benchmark query to measure empirical inference speed.
    """
    prompt = "Write a fast C macro to compute the maximum of two numbers without branching."
    payload = {
        "model": model_name,
        "messages": [{"role": "user", "content": prompt}],
        "stream": False,
        "options": {
            "num_gpu": num_gpu,
            "num_thread": num_thread,
            "num_ctx": num_ctx,
            "temperature": 0.5
        }
    }

    url = f"{OLLAMA_HOST}/api/chat"
    req = urllib.request.Request(
        url,
        data=json.dumps(payload).encode("utf-8"),
        headers={"Content-Type": "application/json"}
    )

    t0 = time.time()
    try:
        with urllib.request.urlopen(req, timeout=timeout_sec) as resp:
            data = json.loads(resp.read().decode("utf-8"))
            eval_count = data.get("eval_count", 0)
            eval_duration_sec = data.get("eval_duration", 1) / 1e9
            prompt_count = data.get("prompt_eval_count", 0)
            prompt_duration_sec = data.get("prompt_eval_duration", 1) / 1e9

            tps = eval_count / eval_duration_sec if eval_duration_sec > 0 else 0.0
            prompt_tps = prompt_count / prompt_duration_sec if prompt_duration_sec > 0 else 0.0
            total_elapsed = time.time() - t0

            return True, {
                "success": True,
                "eval_tokens": eval_count,
                "eval_tps": round(tps, 2),
                "prompt_tokens": prompt_count,
                "prompt_tps": round(prompt_tps, 2),
                "total_time_sec": round(total_elapsed, 2)
            }
    except Exception as e:
        return False, {
            "success": False,
            "error": str(e),
            "eval_tps": 0.0,
            "prompt_tps": 0.0
        }

# ==============================================================================
# BLOCK 4: AUDIT PROCEDURES
# ==============================================================================
def self_audit() -> bool:
    return True

# ==============================================================================
# BLOCK 5: CLOSING BLOCK
# ==============================================================================
if __name__ == "__main__":
    ok, res = run_micro_benchmark("qwen2.5-coder:1.5b", 28, 6, 2048)
    print(f"Micro-benchmark result: {res}")

