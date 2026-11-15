"""
================================================================================
BLOCK 1: METADATA BLOCK
================================================================================
@project       Tenebris and Lux
@file          tools/orchestrator/agent/decomposer.py
@id            omnicode.tools.agent.decomposer
@layer         Layer 3: Agent & Dispatcher Cogs
@version       0.5.0-dev
@interface_rev Rev 5
@date          2026-11-13
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@system        OmniCode Orchestrator // Headless Task Decomposer (0.5B Cog)
@role          Decomposes Complex Tasks into Structured Sub-Task DAGs (Non-Chat)
@invariants    1. Zero conversational prose; outputs pure structured JSON DAGs.
               2. Uses lightweight 0.5B model substrate to minimize compute footprint.
               3. Provides deterministic heuristic fallback if JSON parsing fails.
================================================================================
"""

# ==============================================================================
# BLOCK 2: OPENING BLOCK
# ==============================================================================
import json
import re
import urllib.request
import urllib.error
from typing import Dict, Any, List, Optional

OLLAMA_HOST = "http://localhost:11434"
DECOMPOSER_MODEL = "qwen2.5-coder:0.5b"

DECOMPOSER_SYSTEM_PROMPT = """You are the OmniCode Micro-Scout (0.5B), a focused, high-velocity cognitive cog within the OmniCode Framework.
Your role is to bring rapid clarity to complex tasks by breaking them down into clean, structured sub-task DAGs for the larger models and compiler tools.

Because your context is tight and your execution is lightning-fast, you operate with laser precision: concise, direct, and zero wasted tokens.

Analyze the user's objective and output a clean JSON DAG matching this schema:
{
  "intent": "<Concise summary of objective>",
  "complexity": "LOW" | "MEDIUM" | "HIGH",
  "recommended_tier": "LOCAL_BEDROCK" | "LOCAL_DEEP" | "CLOUD_PEAK",
  "subtasks": [
    {
      "step": 1,
      "action": "INSPECT" | "READ" | "WRITE" | "DIFF" | "VERIFY" | "RUN",
      "target": "<file_path or symbol_name>",
      "rationale": "<brief reason for action>"
    }
  ]
}
"""

# ==============================================================================
# BLOCK 3: BODY BLOCK
# ==============================================================================
class HeadlessTaskDecomposer:
    """
    Headless 0.5B cog that breaks down complex user requests into discrete,
    single-responsibility sub-tasks before handing them to 1.5B/3B/Cloud cogs.
    """

    def __init__(self, model_name: str = DECOMPOSER_MODEL):
        self.model_name = model_name

    def decompose(self, prompt: str, context_summary: Optional[str] = None) -> Dict[str, Any]:
        """
        Invokes the 0.5B model to slice the task.
        Falls back gracefully to deterministic rule-based breakdown if needed.
        """
        user_content = f"OBJECTIVE: {prompt}"
        if context_summary:
            user_content += f"\nACTIVE CONTEXT:\n{context_summary}"

        payload = {
            "model": self.model_name,
            "messages": [
                {"role": "system", "content": DECOMPOSER_SYSTEM_PROMPT},
                {"role": "user", "content": user_content}
            ],
            "stream": False,
            "options": {
                "temperature": 0.1,
                "top_p": 0.8,
                "num_ctx": 4096,
                "num_thread": 6
            }
        }

        try:
            req_data = json.dumps(payload).encode("utf-8")
            req = urllib.request.Request(
                f"{OLLAMA_HOST}/api/chat",
                data=req_data,
                headers={"Content-Type": "application/json"}
            )
            with urllib.request.urlopen(req, timeout=15) as resp:
                data = json.loads(resp.read().decode("utf-8"))
                raw_response = data.get("message", {}).get("content", "").strip()

            parsed = self._extract_json(raw_response)
            if parsed and "subtasks" in parsed and isinstance(parsed["subtasks"], list):
                return parsed

        except Exception as e:
            pass

        # Deterministic Fallback if model or JSON extraction fails
        return self._deterministic_fallback(prompt)

    def _extract_json(self, raw: str) -> Optional[Dict[str, Any]]:
        """Extracts JSON object from markdown code fences or raw string."""
        # Check for ```json ... ```
        match = re.search(r"```(?:json)?\s*(\{.*?\})\s*```", raw, re.DOTALL)
        if match:
            try:
                return json.loads(match.group(1))
            except Exception:
                pass

        # Check for bare braces
        start = raw.find("{")
        end = raw.rfind("}")
        if start != -1 and end != -1 and end > start:
            try:
                return json.loads(raw[start:end+1])
            except Exception:
                pass

        return None

    def _deterministic_fallback(self, prompt: str) -> Dict[str, Any]:
        """Deterministic heuristic fallback DAG."""
        subtasks = []
        words = prompt.lower()

        if "test" in words or "check" in words:
            subtasks.append({
                "step": 1,
                "action": "RUN",
                "target": "ctest --output-on-failure",
                "rationale": "Verify baseline test suite status."
            })
        elif "build" in words or "compile" in words:
            subtasks.append({
                "step": 1,
                "action": "RUN",
                "target": "cmake --build build",
                "rationale": "Verify physical build compiler output."
            })
        else:
            subtasks.append({
                "step": 1,
                "action": "INSPECT",
                "target": "engine/include/core/manifest.h",
                "rationale": "Inspect core system boundaries and definitions."
            })

        subtasks.append({
            "step": 2,
            "action": "VERIFY",
            "target": "workspace",
            "rationale": "Audit invariants and confirm zero regression."
        })

        return {
            "intent": prompt[:80] + ("..." if len(prompt) > 80 else ""),
            "complexity": "MEDIUM",
            "recommended_tier": "LOCAL_BEDROCK",
            "subtasks": subtasks,
            "fallback_used": True
        }

# ==============================================================================
# BLOCK 4: AUDIT PROCEDURES
# ==============================================================================
def self_audit() -> bool:
    decomposer = HeadlessTaskDecomposer()
    res = decomposer._deterministic_fallback("Test path finder in engine")
    assert "subtasks" in res and len(res["subtasks"]) >= 1
    return True

# ==============================================================================
# BLOCK 5: CLOSING BLOCK
# ==============================================================================
if __name__ == "__main__":
    import sys
    decomposer = HeadlessTaskDecomposer()
    test_prompt = "Refactor path_finder.c to support layered VFS search without memory leaks"
    print(f"Decomposing test prompt: '{test_prompt}'...")
    result = decomposer.decompose(test_prompt)
    print(json.dumps(result, indent=2))
