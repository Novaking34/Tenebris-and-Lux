"""
================================================================================
BLOCK 1: METADATA BLOCK
================================================================================
@project       Tenebris and Lux
@file          tools/orchestrator/agent/reflection_loop.py
@id            omnicode.tools.agent.reflection_loop
@layer         Layer 3: Agent & Dispatcher Cogs
@version       0.4.0-dev
@interface_rev Rev 5
@date          2026-11-13
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@system        OmniCode Orchestrator // Autonomous Reflection & Streaming Loop
@role          Streams Model Responses, Isolates Reasoning, and Executes Tool Loops
@invariants    1. Zero external pip dependencies (uses urllib.request).
               2. Elastic context scaling applied to each request.
               3. Provides live chunk streaming callback for GUI / CLI.
================================================================================
"""

# ==============================================================================
# BLOCK 2: OPENING BLOCK
# ==============================================================================
import json
import time
import urllib.request
import urllib.error
from typing import List, Dict, Any, Optional, Callable, Tuple
from ..memory.elastic_window import calculate_elastic_context
from ..memory.index_store import EpisodicIndexStore
from .dispatcher import AgenticToolDispatcher

OLLAMA_HOST = "http://localhost:11434"

# ==============================================================================
# BLOCK 3: BODY BLOCK
# ==============================================================================
def stream_model_response(
    messages: List[Dict[str, str]],
    model_name: str,
    estimated_tokens: int = 500,
    chunk_callback: Optional[Callable[[str, bool], None]] = None,
    optimal_threads: int = 6
) -> Tuple[Optional[str], Dict[str, Any]]:
    """
    Streams inference from Ollama, separating internal reasoning from output.
    chunk_callback(text_token, is_thinking)
    """
    url = f"{OLLAMA_HOST}/api/chat"
    active_num_ctx = calculate_elastic_context(estimated_tokens, model_name)

    payload = {
        "model": model_name,
        "messages": messages,
        "stream": True,
        "options": {
            "temperature": 0.5,
            "top_p": 0.9,
            "repeat_penalty": 1.1,
            "num_ctx": active_num_ctx,
            "num_batch": 512,
            "num_thread": optimal_threads
        }
    }

    try:
        data = json.dumps(payload).encode("utf-8")
        req = urllib.request.Request(url, data=data, headers={"Content-Type": "application/json"})
    except Exception as e:
        return None, {"error": str(e)}

    in_think_block = False
    full_response = ""
    start_time = time.time()
    total_tokens = 0
    prompt_eval_tokens = 0
    eval_duration_ns = 0

    try:
        with urllib.request.urlopen(req, timeout=120) as resp:
            for raw_line in resp:
                if not raw_line:
                    continue
                chunk = json.loads(raw_line.decode("utf-8"))

                if "message" in chunk and "content" in chunk["message"]:
                    token = chunk["message"]["content"]
                    full_response += token

                    if "<think>" in token:
                        in_think_block = True
                        token = token.replace("<think>", "")

                    if "</think>" in token:
                        parts = token.split("</think>")
                        if chunk_callback:
                            chunk_callback(parts[0], True)
                        in_think_block = False
                        token = parts[1] if len(parts) > 1 else ""

                    if chunk_callback and token:
                        chunk_callback(token, in_think_block)

                if chunk.get("done", False):
                    total_tokens = chunk.get("eval_count", 0)
                    eval_duration_ns = chunk.get("eval_duration", 0)
                    prompt_eval_tokens = chunk.get("prompt_eval_count", 0)

        elapsed = time.time() - start_time
        tps = (total_tokens / (eval_duration_ns / 1e9)) if eval_duration_ns > 0 else 0.0

        telemetry = {
            "tokens": total_tokens,
            "tps": round(tps, 2),
            "prompt_tokens": prompt_eval_tokens,
            "active_ctx": active_num_ctx,
            "elapsed_sec": round(elapsed, 2)
        }
        return full_response, telemetry

    except Exception as e:
        return None, {"error": str(e)}

def run_agentic_turn(
    prompt: str,
    index_store: EpisodicIndexStore,
    dispatcher: AgenticToolDispatcher,
    model_name: str = "omnicode-fast-1.5b",
    chunk_callback: Optional[Callable[[str, bool], None]] = None,
    tool_status_callback: Optional[Callable[[str], None]] = None
) -> Tuple[Optional[str], Dict[str, Any]]:
    """
    Executes a multi-turn agentic cycle:
    1. Assembles sparse payload from episodic memory.
    2. Streams primary response.
    3. Detects and executes tool actions.
    4. If tools executed, feeds observations back for self-healing reflection.
    """
    sparse_payload, paged_ids, est_tokens = index_store.assemble_sparse_payload(prompt)
    messages = [{"role": "user", "content": sparse_payload}]

    resp, telem = stream_model_response(
        messages=messages,
        model_name=model_name,
        estimated_tokens=est_tokens,
        chunk_callback=chunk_callback
    )

    if not resp:
        return None, telem

    index_store.record_turn(prompt, resp, [])

    tools = dispatcher.extract_tools(resp)
    if tools:
        if tool_status_callback:
            tool_status_callback(f"Executing {len(tools)} tool action(s)...")

        observations = []
        for t in tools:
            obs = dispatcher.execute_tool(t)
            observations.append(obs)

        obs_prompt = "TOOL OBSERVATIONS:\n" + "\n\n".join(observations) + "\n\nAnalyze observations, perform invariant check or confirm completion."
        if tool_status_callback:
            tool_status_callback("Reflecting on tool observations...")

        followup_payload, _, followup_tokens = index_store.assemble_sparse_payload(obs_prompt)
        followup_messages = [{"role": "user", "content": followup_payload}]

        followup_resp, followup_telem = stream_model_response(
            messages=followup_messages,
            model_name=model_name,
            estimated_tokens=followup_tokens,
            chunk_callback=chunk_callback
        )

        if followup_resp:
            index_store.record_turn(obs_prompt, followup_resp, [])
            return resp + "\n\n" + followup_resp, followup_telem

    return resp, telem

# ==============================================================================
# BLOCK 4: AUDIT PROCEDURES
# ==============================================================================
def self_audit() -> bool:
    return True

# ==============================================================================
# BLOCK 5: CLOSING BLOCK
# ==============================================================================
if __name__ == "__main__":
    print("Reflection loop module ready.")

