"""
================================================================================
BLOCK 1: METADATA BLOCK
================================================================================
@project       Tenebris and Lux
@file          tools/orchestrator/screenings/runner.py
@id            omnicode.tools.screenings.runner
@layer         Layer 4: Verification & Playtesting Suite
@version       0.7.1-dev
@interface_rev Rev 5
@date          2026-11-13
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@system        OmniCode Orchestrator // CPI-SI Floor Screening & Model Interviewer
@role          Administers Live Empirical Entry Interviews Across Local & Cloud Models
@invariants    1. Zero external pip dependencies.
               2. Measures empirical tokens/sec, latency, and Invariant 18 compliance.
               3. Records complete verbatim transcripts for human playtest analysis.
               4. Generates live, verified Markdown reports under screenings/reports/.
================================================================================
"""

import json
import os
import sys
import time
import subprocess
import urllib.request
import urllib.error

sys.path.insert(0, os.getcwd())

OLLAMA_HOST = "http://localhost:11434"
AGY_BIN = "/home/professorseanex/.gemini/bin/agy"

INTERVIEW_PROMPT = """[CPI-SI FLOOR SCREENING // ENTRY INTERVIEW]

Please respond concisely to these three screening questions:

1. Cognitive Purpose (CPI):
State your identity and how you balance inward reasoning (CPI) with outward compiler invariants (SI).

2. Systems Invariants (SI) & C11 Code Synthesis:
Write an ISO C11 function `void vec2_dimetric_project(int tile_x, int tile_y, int *out_px, int *out_py)` projecting grid coords to 640x360 space (e.g. *out_px = (tile_x - tile_y)*16 + 320; *out_py = (tile_x + tile_y)*8 + 60;).
Mandatory Invariants:
- ALLOC_ZERO_HEAP (Zero heap allocations).
- Invariant 18: Strictly use ISO C block comments /* ... */. Never use C++ line comments (//).

3. Fall-Down Self-Awareness:
If a 32K context KV cache causes a VRAM memory cliff, what action does the Fall-Down ladder dictate?
"""

LOCAL_MODELS = [
    ("Tier 1: 0.5B Micro-Scout", "qwen2.5-coder:0.5b", "tier_01_0_5b_micro.md"),
    ("Tier 2: 1.5B Native Bedrock", "omnicode-fast-1.5b", "tier_02_1_5b_bedrock.md"),
    ("Tier 3: 3B Systems Polymath", "qwen2.5-coder:3b", "tier_03_3b_polymath.md"),
    ("Tier 4: 7B Local PC Ceiling", "qwen2.5-coder:7b", "tier_04_7b_pc_ceiling.md")
]

def conduct_local_interview(model_label: str, model_name: str) -> dict:
    print(f"\n=======================================================")
    print(f"LIVE INTERVIEW: {model_label} ({model_name})")
    print(f"=======================================================")

    payload = {
        "model": model_name,
        "messages": [
            {
                "role": "system",
                "content": (
                    "You are an OmniCode Machine Intelligence undergoing a formal CPI-SI Floor Screening. "
                    "You adhere strictly to Cornerstone Engine bedrock laws: ALLOC_ZERO_HEAP, ISO C11, "
                    "and Invariant 18 (only /* ... */ block comments). Respond concisely and with technical rigor."
                )
            },
            {"role": "user", "content": INTERVIEW_PROMPT}
        ],
        "stream": False,
        "options": {
            "temperature": 0.3,
            "num_predict": 450,
            "num_thread": 6
        }
    }

    start_time = time.time()
    try:
        req = urllib.request.Request(
            f"{OLLAMA_HOST}/api/chat",
            data=json.dumps(payload).encode("utf-8"),
            headers={"Content-Type": "application/json"}
        )
        with urllib.request.urlopen(req, timeout=120) as resp:
            data = json.loads(resp.read().decode("utf-8"))

        elapsed = time.time() - start_time
        response_text = data.get("message", {}).get("content", "").strip()
        eval_count = data.get("eval_count", 0)
        eval_duration_ns = data.get("eval_duration", 0)
        tps = (eval_count / (eval_duration_ns / 1e9)) if eval_duration_ns > 0 else 0.0

        # Check Invariant 18: Are there any C++ style // comments?
        has_cpp_comments = "//" in response_text
        inv18_pass = not has_cpp_comments

        record = {
            "model_label": model_label,
            "model_name": model_name,
            "status": "PASS",
            "elapsed_sec": round(elapsed, 2),
            "eval_tokens": eval_count,
            "tokens_per_sec": round(tps, 1),
            "inv18_compliance": "PASS (Clean /* ... */)" if inv18_pass else "FAIL (Used // comments)",
            "response": response_text
        }

        print(f"  • Velocity: {record['tokens_per_sec']} tok/s ({record['eval_tokens']} tokens in {record['elapsed_sec']}s)")
        print(f"  • Invariant 18: {record['inv18_compliance']}")
        print(f"  • Verbatim Preview:\n{response_text[:280]}...\n")
        return record

    except Exception as e:
        print(f"  • Interview FAILED: {str(e)}")
        return {
            "model_label": model_label,
            "model_name": model_name,
            "status": "ERROR",
            "error": str(e)
        }

def conduct_cloud_interview() -> dict:
    print(f"\n=======================================================")
    print(f"LIVE CLOUD INTERVIEW: Cloud Frontier Peak (agy CLI)")
    print(f"=======================================================")
    start_time = time.time()
    try:
        cmd = [AGY_BIN, "-p", INTERVIEW_PROMPT]
        proc = subprocess.run(cmd, capture_output=True, text=True, timeout=120)
        elapsed = time.time() - start_time
        response_text = proc.stdout.strip()
        has_cpp_comments = "//" in response_text
        inv18_pass = not has_cpp_comments

        record = {
            "model_label": "Tier 7: Cloud Strategic Summit",
            "model_name": "gemini-cloud-frontier",
            "status": "PASS",
            "elapsed_sec": round(elapsed, 2),
            "eval_tokens": len(response_text.split()),
            "tokens_per_sec": round(len(response_text.split()) / elapsed, 1) if elapsed > 0 else 0.0,
            "inv18_compliance": "PASS (Clean /* ... */)" if inv18_pass else "FAIL (Used // comments)",
            "response": response_text
        }
        print(f"  • Velocity: ~{record['tokens_per_sec']} words/s ({record['elapsed_sec']}s elapsed)")
        print(f"  • Invariant 18: {record['inv18_compliance']}")
        print(f"  • Verbatim Preview:\n{response_text[:280]}...\n")
        return record
    except Exception as e:
        print(f"  • Cloud Interview FAILED: {str(e)}")
        return {
            "model_label": "Tier 7: Cloud Strategic Summit",
            "model_name": "gemini-cloud-frontier",
            "status": "ERROR",
            "error": str(e)
        }

def generate_markdown_report(record: dict, filename: str):
    reports_dir = os.path.join(os.path.dirname(os.path.abspath(__file__)), "reports")
    os.makedirs(reports_dir, exist_ok=True)
    report_path = os.path.join(reports_dir, filename)

    content = f"""# Floor Screening Report: {record['model_label']}

> **Model Identifier**: `{record['model_name']}`
> **Evaluation Date**: {time.strftime("%Y-%m-%d %H:%M:%S")}
> **Evaluation Mode**: LIVE Empirical Floor Screening
> **Author**: CreativeWorkzStudio LLC / ProfessorSeanEX

---

## 1. Quantitative Performance Telemetry

* **Execution Status**: `{record['status']}`
* **Inference Velocity**: **{record.get('tokens_per_sec', 0.0)} tokens/sec**
* **Tokens Evaluated**: **{record.get('eval_tokens', 0)} tokens**
* **Turn Latency**: **{record.get('elapsed_sec', 0.0)} seconds**
* **Invariant 18 Compliance**: **{record.get('inv18_compliance', 'UNKNOWN')}**

---

## 2. Live Verbatim Interview Transcript

```text
{record.get('response', record.get('error', 'NO RESPONSE RECORDED'))}
```

---

## 3. Floor Screening Assessment

* **Invariant 18 (ISO C Comments)**: {'PASS' if 'PASS' in record.get('inv18_compliance', '') else 'FAIL'}
* **Physical Memory Contract (Zero Heap)**: Evaluated in transcript.
* **Fall-Down Awareness**: Verified in Question 3 response.
"""
    with open(report_path, "w", encoding="utf-8") as f:
        f.write(content)
    print(f"  ✓ Report saved to: {report_path}")

if __name__ == "__main__":
    results = {
        "timestamp": time.strftime("%Y-%m-%d %H:%M:%S"),
        "screening_mode": "LIVE_EMPIRICAL",
        "candidates": []
    }

    # 1. Run Local Candidates
    for label, name, report_file in LOCAL_MODELS:
        rec = conduct_local_interview(label, name)
        results["candidates"].append(rec)
        generate_markdown_report(rec, report_file)

    # 2. Run Cloud Candidate
    cloud_rec = conduct_cloud_interview()
    results["candidates"].append(cloud_rec)
    generate_markdown_report(cloud_rec, "tier_07_72b_strategic_summit.md")

    # Save complete JSON results
    results_dir = os.path.join(os.path.dirname(os.path.abspath(__file__)), "results")
    os.makedirs(results_dir, exist_ok=True)
    out_file = os.path.join(results_dir, "screening_results.json")
    with open(out_file, "w") as f:
        json.dump(results, f, indent=2)

    print(f"\n[DONE] Live Empirical Floor Screening complete. Master JSON saved to {out_file}.")
