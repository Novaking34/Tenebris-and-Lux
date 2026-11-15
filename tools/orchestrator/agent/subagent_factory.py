"""
================================================================================
BLOCK 1: METADATA BLOCK
================================================================================
@project       Tenebris and Lux
@file          tools/orchestrator/agent/subagent_factory.py
@id            omnicode.tools.agent.subagent_factory
@layer         Layer 3: Agent & Dispatcher Cogs
@version       0.4.0-dev
@interface_rev Rev 5
@date          2026-11-13
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@system        OmniCode Orchestrator // Subagent Spawner Factory
@role          Instantiates Specialized Subagent Personas with Bounded Contexts
@invariants    1. Zero external pip dependencies.
               2. Subagents inherit the CPI-SI Covenant Tensor.
================================================================================
"""

# ==============================================================================
# BLOCK 2: OPENING BLOCK
# ==============================================================================
from typing import Dict, Any, Optional
from ..types.config_schema import ModelProfile, load_models_config

# ==============================================================================
# BLOCK 3: BODY BLOCK
# ==============================================================================
class SubagentFactory:
    """
    Factory to instantiate tailored subagent configurations on demand.
    """
    @staticmethod
    def get_subagent_spec(role: str) -> Dict[str, Any]:
        profiles = load_models_config()
        role_lower = role.lower()

        if "fast" in role_lower or "tool" in role_lower or "1.5" in role_lower:
            model = "omnicode-fast-1.5b"
            system_role = "Specialized Fast Execution Subagent: High-velocity tool dispatch, AST navigation, and syntax formatting."
        elif "architect" in role_lower or "deep" in role_lower or "7" in role_lower:
            model = "omnicode-architect-7b"
            system_role = "Specialized Systems Architect Subagent: Deep ontological reasoning, mathematical proofs, and architectural synthesis."
        else:
            model = "omnicode-polymath"
            system_role = "Specialized Polymath Subagent: Core C11/C23 systems programming, game simulation logic, and balanced iteration."

        profile = profiles.get(model, profiles["omnicode-fast-1.5b"])

        return {
            "model_name": model,
            "system_role": system_role,
            "min_ctx": profile.min_ctx,
            "sweet_ctx": profile.sweet_ctx,
            "max_ctx": profile.max_ctx,
            "threads": profile.recommended_cpu_threads,
            "gpu_layers": profile.recommended_gpu_layers
        }

# ==============================================================================
# BLOCK 4: AUDIT PROCEDURES
# ==============================================================================
def self_audit() -> bool:
    spec = SubagentFactory.get_subagent_spec("fast")
    return spec["model_name"] == "omnicode-fast-1.5b"

# ==============================================================================
# BLOCK 5: CLOSING BLOCK
# ==============================================================================
if __name__ == "__main__":
    print(f"Subagent factory self-audit: {'PASS' if self_audit() else 'FAIL'}")

