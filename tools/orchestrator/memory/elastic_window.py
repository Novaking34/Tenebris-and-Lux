"""
================================================================================
BLOCK 1: METADATA BLOCK
================================================================================
@project       Tenebris and Lux
@file          tools/orchestrator/memory/elastic_window.py
@id            omnicode.tools.memory.elastic_window
@layer         Layer 2: Memory & Context Cogs
@version       0.4.0-dev
@interface_rev Rev 5
@date          2026-11-13
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@system        OmniCode Orchestrator // Elastic Context Window Scaler
@role          Calculates Minimal Power-of-Two num_ctx to Maximize Token Velocity
@invariants    1. Snaps dynamically between min_ctx (2048) and max_ctx (16384/32768).
               2. Always reserves a 1024-token output generation headroom.
================================================================================
"""

# ==============================================================================
# BLOCK 2: OPENING BLOCK
# ==============================================================================
from typing import Dict, Optional
from ..types.config_schema import ModelProfile, load_models_config

# ==============================================================================
# BLOCK 3: BODY BLOCK
# ==============================================================================
def calculate_elastic_context(
    estimated_input_tokens: int,
    model_name: str,
    profiles: Optional[Dict[str, ModelProfile]] = None
) -> int:
    """
    Computes optimal power-of-two num_ctx for inference request.
    """
    if profiles is None:
        profiles = load_models_config()

    clean_name = model_name.split(":")[0]
    profile = profiles.get(clean_name, profiles.get("omnicode-fast-1.5b"))

    min_ctx = profile.min_ctx if profile else 2048
    max_ctx = profile.max_ctx if profile else 32768

    demand = estimated_input_tokens + 1024
    ctx = min_ctx
    while ctx < demand and ctx < max_ctx:
        ctx *= 2

    return min(ctx, max_ctx)

# ==============================================================================
# BLOCK 4: AUDIT PROCEDURES
# ==============================================================================
def self_audit() -> bool:
    ctx = calculate_elastic_context(500, "omnicode-fast-1.5b")
    return ctx == 2048

# ==============================================================================
# BLOCK 5: CLOSING BLOCK
# ==============================================================================
if __name__ == "__main__":
    print(f"Elastic ctx for 500 tokens: {calculate_elastic_context(500, 'omnicode-fast-1.5b')}")
    print(f"Elastic ctx for 2500 tokens: {calculate_elastic_context(2500, 'omnicode-fast-1.5b')}")

