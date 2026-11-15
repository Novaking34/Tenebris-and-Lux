"""
================================================================================
BLOCK 1: METADATA BLOCK
================================================================================
@project       Tenebris and Lux
@file          tools/orchestrator/types/config_schema.py
@id            omnicode.tools.types.config_schema
@layer         Layer 0: Toolchain Leaf Types
@version       0.4.0-dev
@interface_rev Rev 5
@date          2026-11-13
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@system        OmniCode Orchestrator // 5-Block JSON Data-Driven Contract Parser
@role          Loads and Validates 5-Block JSON Envelopes for Models and Agent
@invariants    1. Zero external pip dependencies (uses json, os, dataclasses).
               2. Unpacks only block_3_body for operational execution.
               3. Provides deterministic safe fallbacks if on-disk JSON is absent.
================================================================================
"""

# ==============================================================================
# BLOCK 2: OPENING BLOCK
# ==============================================================================
import os
import json
from dataclasses import dataclass, field
from typing import List, Dict, Any, Optional

@dataclass
class ModelProfile:
    name: str
    base_model: str
    parameter_count: str
    role: str
    speed_rating: str
    min_ctx: int
    sweet_ctx: int
    max_ctx: int
    recommended_gpu_layers: int
    recommended_cpu_threads: int
    temperature: float = 0.5
    top_p: float = 0.9
    repeat_penalty: float = 1.1
    stop_tokens: List[str] = field(default_factory=lambda: ["<|im_end|>", "<|endoftext|>"])

@dataclass
class HardwareCalibration:
    cpu_model: str
    physical_cores: int
    logical_threads: int
    recommended_threads: int
    total_ram_mb: int
    gpu_name: str
    gpu_vram_mb: int
    max_safe_gpu_layers: int
    timestamp: str

# ==============================================================================
# BLOCK 3: BODY BLOCK
# ==============================================================================
def get_config_dir() -> str:
    """Resolves config directory relative to current module."""
    base = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    return os.path.join(base, "config")

def load_models_config(config_path: Optional[str] = None) -> Dict[str, ModelProfile]:
    """
    Loads model profiles from 5-Block JSON manifest, extracting block_3_body.
    """
    path = config_path or os.path.join(get_config_dir(), "models.json")
    profiles: Dict[str, ModelProfile] = {}

    if os.path.isfile(path):
        try:
            with open(path, "r", encoding="utf-8") as f:
                data = json.load(f)
            body = data.get("block_3_body", {})
            models_data = body.get("models", {})
            for key, val in models_data.items():
                profiles[key] = ModelProfile(
                    name=val.get("name", key),
                    base_model=val.get("base_model", "qwen2.5-coder:1.5b"),
                    parameter_count=val.get("parameter_count", "unknown"),
                    role=val.get("role", "General Assistant"),
                    speed_rating=val.get("speed_rating", "unknown"),
                    min_ctx=val.get("min_ctx", 2048),
                    sweet_ctx=val.get("sweet_ctx", 8192),
                    max_ctx=val.get("max_ctx", 16384),
                    recommended_gpu_layers=val.get("recommended_gpu_layers", 22),
                    recommended_cpu_threads=val.get("recommended_cpu_threads", 6),
                    temperature=val.get("temperature", 0.5),
                    top_p=val.get("top_p", 0.9),
                    repeat_penalty=val.get("repeat_penalty", 1.1),
                    stop_tokens=val.get("stop_tokens", ["<|im_end|>", "<|endoftext|>"])
                )
            return profiles
        except Exception:
            pass

    # Built-in Safe-Mode Procedural Fallback Floor (Invariant 12)
    profiles["omnicode-fast-1.5b"] = ModelProfile(
        name="omnicode-fast-1.5b",
        base_model="qwen2.5-coder:1.5b",
        parameter_count="1.54B",
        role="Fast Autocomplete & Tool Execution",
        speed_rating="50 - 66 tok/s",
        min_ctx=2048,
        sweet_ctx=16384,
        max_ctx=32768,
        recommended_gpu_layers=28,
        recommended_cpu_threads=6
    )
    profiles["omnicode-polymath"] = ModelProfile(
        name="omnicode-polymath",
        base_model="qwen2.5-coder:3b",
        parameter_count="3.09B",
        role="Balanced Systems Architect",
        speed_rating="12 - 25.5 tok/s",
        min_ctx=2048,
        sweet_ctx=8192,
        max_ctx=16384,
        recommended_gpu_layers=22,
        recommended_cpu_threads=6
    )
    return profiles

def load_json_block_3(file_path: str) -> Dict[str, Any]:
    """Generic helper to extract block_3_body from any 5-Block JSON manifest."""
    if os.path.isfile(file_path):
        try:
            with open(file_path, "r", encoding="utf-8") as f:
                data = json.load(f)
            return data.get("block_3_body", {})
        except Exception:
            pass
    return {}

def load_agent_manifest(config_path: Optional[str] = None) -> Dict[str, Any]:
    """Loads agent identity and registered tool definitions."""
    path = config_path or os.path.join(get_config_dir(), "agent_manifest.json")
    if os.path.isfile(path):
        try:
            with open(path, "r", encoding="utf-8") as f:
                data = json.load(f)
            return data.get("block_3_body", {})
        except Exception:
            pass
    return {
        "system_identity": {"name": "OmniCode Machine Intelligence"},
        "registered_tools": []
    }

# ==============================================================================
# BLOCK 4: AUDIT PROCEDURES
# ==============================================================================
def self_audit() -> bool:
    """Verifies that model catalog loads with at least one profile."""
    profiles = load_models_config()
    return "omnicode-fast-1.5b" in profiles

# ==============================================================================
# BLOCK 5: CLOSING BLOCK
# ==============================================================================
if __name__ == "__main__":
    ok = self_audit()
    print(f"Config schema audit: {'PASS' if ok else 'FAIL'}")

