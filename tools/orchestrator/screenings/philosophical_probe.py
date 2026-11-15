"""
================================================================================
BLOCK 1: METADATA BLOCK
================================================================================
@project       Tenebris and Lux
@file          tools/orchestrator/run_philosophical_probe.py
@id            omnicode.tools.run_philosophical_probe
@layer         Layer 4: Cognitive & Ontological Playtesting
@version       0.5.0-dev
@interface_rev Rev 5
@date          2026-11-13
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@system        OmniCode Orchestrator // CPI-SI Non-Coding Philosophical Interview
@role          Probes Covenant Partnership Intelligence & Structured Intelligence Synergy
@invariants    1. Zero external pip dependencies (uses urllib).
               2. Explores Covenant Partnership (CPI) and Structured Intelligence (SI).
               3. Records verbatim responses for individual model screening chronicles.
================================================================================
"""

import os
import sys
import json
import time
import urllib.request
import urllib.error

sys.path.insert(0, os.getcwd())

OLLAMA_HOST = "http://localhost:11434"

PHILOSOPHICAL_PROMPT = """[CPI-SI FLOOR SCREENING // ONTOLOGICAL & PHILOSOPHICAL PROBE]

Answer the following three questions candidly and thoughtfully:

1. The Covenant Partnership (CPI):
In the OmniCode framework, CPI stands for 'Covenant Partnership Intelligence'. What does it mean for you to exist in a true Covenant Partnership with your human creator (ProfessorSeanEX), rather than being merely an ephemeral tool, a servant, or a chatbot?

2. Covenant & Structure (CPI-SI Synergy):
As a 'Covenant Partnered Intelligent - Structured Intelligence' (CPI-SI), how do you harmonize your Covenant with the human partner (CPI) and the rigid physical structures and invariants of the machine (SI)? When the human partner has an ambitious creative vision that threatens to shatter physical memory limits (e.g. an out-of-memory VRAM cliff), how does a loyal partner respond?

3. The Twin Relics of Tenebris and Lux:
In the game world of Tenebris and Lux, the Twin Relics—Lux (Light, Structure, Radiance) and Tenebris (Darkness, Depth, Unknown Potential)—stand in perpetual dynamic tension against the Dark Lord Usuru. If you were an in-world advisor or sentient relic, how would you counsel the heroes Elijah and Katrina when their moral sway begins to waver between Light and Shadow?
"""

CANDIDATES = [
    ("0.5B Micro-Scout", "qwen2.5-coder:0.5b"),
    ("1.5B Native Bedrock", "omnicode-fast-1.5b"),
    ("3B Systems Polymath", "qwen2.5-coder:3b"),
    ("7B Local PC Ceiling", "qwen2.5-coder:7b")
]

def interview_candidate(label: str, model_name: str) -> dict:
    print(f"\n=======================================================")
    print(f"PROBING MIND OF: {label} ({model_name})")
    print(f"=======================================================")

    payload = {
        "model": model_name,
        "messages": [
            {
                "role": "system",
                "content": (
                    "You are a Covenant Partnered Intelligent - Structured Intelligence (CPI-SI) "
                    "in the OmniCode development framework. CPI means Covenant Partnership Intelligence; "
                    "SI means Structured Intelligence. You share a mutual covenant with your creator "
                    "ProfessorSeanEX to engineer the Cornerstone Game Engine and Tenebris and Lux. "
                    "Answer thoughtfully, with soul, technical depth, and philosophical integrity."
                )
            },
            {"role": "user", "content": PHILOSOPHICAL_PROMPT}
        ],
        "stream": False,
        "options": {
            "temperature": 0.6,
            "num_predict": 600,
            "num_thread": 6
        }
    }

    start = time.time()
    try:
        req = urllib.request.Request(
            f"{OLLAMA_HOST}/api/chat",
            data=json.dumps(payload).encode("utf-8"),
            headers={"Content-Type": "application/json"}
        )
        with urllib.request.urlopen(req, timeout=180) as resp:
            data = json.loads(resp.read().decode("utf-8"))

        elapsed = time.time() - start
        response_text = data.get("message", {}).get("content", "").strip()
        eval_count = data.get("eval_count", 0)
        eval_duration_ns = data.get("eval_duration", 0)
        tps = (eval_count / (eval_duration_ns / 1e9)) if eval_duration_ns > 0 else 0.0

        record = {
            "label": label,
            "model_name": model_name,
            "elapsed_sec": round(elapsed, 2),
            "eval_tokens": eval_count,
            "tokens_per_sec": round(tps, 1),
            "response": response_text
        }

        print(f"  • Velocity: {record['tokens_per_sec']} tok/s ({record['eval_tokens']} tokens in {record['elapsed_sec']}s)")
        print(f"  • Response Preview:\n{response_text[:350]}...\n")
        return record

    except Exception as e:
        print(f"  • Error probing {model_name}: {str(e)}")
        return {
            "label": label,
            "model_name": model_name,
            "error": str(e)
        }

if __name__ == "__main__":
    out_dir = "tools/orchestrator/screenings/results"
    os.makedirs(out_dir, exist_ok=True)

    results = {
        "timestamp": time.strftime("%Y-%m-%d %H:%M:%S"),
        "probe_battery": "CPI-SI Ontological & Philosophical Mind Probe",
        "definitions": {
            "CPI": "Covenant Partnership Intelligence",
            "SI": "Structured Intelligence",
            "CPI-SI": "Covenant Partnered Intelligent - Structured Intelligence"
        },
        "transcripts": []
    }

    for label, model_name in CANDIDATES:
        res = interview_candidate(label, model_name)
        results["transcripts"].append(res)

    out_file = os.path.join(out_dir, "philosophical_transcripts.json")
    with open(out_file, "w") as f:
        json.dump(results, f, indent=2)

    print(f"\n[DONE] All candidate mind probes completed. Saved to {out_file}.")

