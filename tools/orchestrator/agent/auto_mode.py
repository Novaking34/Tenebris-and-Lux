"""
================================================================================
BLOCK 1: METADATA BLOCK
================================================================================
@project       Tenebris and Lux
@file          tools/orchestrator/agent/auto_mode.py
@id            omnicode.tools.agent.auto_mode
@layer         Layer 3: Agent & Dispatcher Cogs
@version       0.7.0-dev
@interface_rev Rev 5
@date          2026-11-13
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@system        OmniCode Orchestrator // Autonomous Multi-Tier Task Governor ("Auto Mode")
@role          Classifies Task Demands & Dynamically Switches Between Local and Cloud Models
@invariants    1. Zero external pip dependencies.
               2. All switches are observable moments (structured telemetry, never silent magic).
               3. The second task space matches local capacity (<= 7B), system auto-triggers Local Mode.
               4. Respects the 7-tier spectrum: 0.5B to 72B.
================================================================================
"""

# ==============================================================================
# BLOCK 2: OPENING BLOCK
# ==============================================================================
import re
from typing import Dict, Any, Tuple, Optional, Callable
from .stepped_gearbox import SteppedGearbox

# Task Archetype Constants
TASK_MICRO_SIFT = "MICRO_SIFT"              # 0.5B (Local) / 14B (Cloud)
TASK_INTERACTIVE_CODE = "INTERACTIVE_CODE"  # 1.5B (Local)
TASK_SYSTEMS_LOGIC = "SYSTEMS_LOGIC"        # 3B (Local)
TASK_INVARIANT_AUDIT = "INVARIANT_AUDIT"    # 7B (Local PC Ceiling)
TASK_MACRO_REFACTOR = "MACRO_REFACTOR"      # 14B (Cloud Workhorse)
TASK_CODER_FLAGSHIP = "CODER_FLAGSHIP"      # 32B (Cloud Coder Flagship)
TASK_STRATEGIC_SUMMIT = "STRATEGIC_SUMMIT"  # 72B (Cloud Summit)

# ==============================================================================
# BLOCK 3: BODY BLOCK
# ==============================================================================
class AutoModeGovernor:
    """
    Autonomous Governor managing 'Auto Mode' across the 7-tier continuum.
    Classifies task intent and dynamically switches models, ensuring that
    local sovereignty is prioritized whenever task demand <= 7B.
    """

    def __init__(self, gearbox: Optional[SteppedGearbox] = None):
        self.gearbox = gearbox or SteppedGearbox()
        self.decision_log = []

    def classify_task(self, prompt: str, estimated_tokens: int = 500) -> Tuple[str, str, str]:
        """
        Classifies incoming task into an archetype and determines the target tier and domain.
        Returns: (archetype, target_tier, target_domain)
        """
        lower = prompt.lower()

        # 1. Strategic Summit (72B): proofs, architecture manifesto, RPG lore sway matrix, cross-subsystem proofs
        if any(w in lower for w in ["mathematical proof", "lore matrix", "moral sway", "subsystem proofs", "ontological compass", "manifesto"]):
            return TASK_STRATEGIC_SUMMIT, "72b", "CLOUD"

        # 2. Macro Refactor or Massive Context (14B or 32B)
        if estimated_tokens > 20000 or any(w in lower for w in ["refactor all", "migrate subsystem", "rewrite module", "codebase sweep"]):
            return TASK_CODER_FLAGSHIP, "32b", "CLOUD"

        if estimated_tokens > 10000 or any(w in lower for w in ["multi-file refactor", "dependency audit"]):
            return TASK_MACRO_REFACTOR, "14b", "CLOUD"

        # 3. Local Invariant Audit & Deep Review (7B)
        if any(w in lower for w in ["audit", "invariant 18", "zero heap check", "controlled memory chassis", "memory layout"]):
            return TASK_INVARIANT_AUDIT, "7b", "LOCAL"

        # 4. Balanced Systems / C11 Logic Authoring (3B)
        if any(w in lower for w in ["write c", "c11", "c23", "implement", "dimetric", "letterbox", "rasterizer", "raymarching", "algebraic", "rust", "algorithm"]):
            return TASK_SYSTEMS_LOGIC, "3b", "LOCAL"

        # 5. Micro Sifting / DAG decomposition (0.5B)
        if estimated_tokens < 300 and any(w in lower for w in ["decompose", "sift", "split task", "parse json", "list files", "plan steps"]):
            return TASK_MICRO_SIFT, "0.5b", "LOCAL"

        # 6. Default Interactive Workhorse (1.5B)
        return TASK_INTERACTIVE_CODE, "1.5b", "LOCAL"

    def evaluate_and_route(
        self,
        prompt: str,
        estimated_tokens: int = 500,
        telemetry_cb: Optional[Callable[[str], None]] = None
    ) -> Dict[str, Any]:
        """
        Evaluates prompt in 'Auto Mode' and switches the gearbox to the optimal gear.
        Emits an observable transition if the model or domain shifts.
        """
        archetype, target_tier, target_domain = self.classify_task(prompt, estimated_tokens)
        current_state = self.gearbox.get_current_state()
        old_tier = current_state["tier"]
        old_domain = current_state["domain"]

        switched = False
        if old_tier != target_tier or old_domain != target_domain:
            self.gearbox.set_gear(target_tier, mode="ideal", domain=target_domain)
            switched = True

        new_state = self.gearbox.get_current_state()

        transition_msg = (
            f"[AUTO MODE GOVERNOR // OBSERVED TRANSITION]\n"
            f"  Task Archetype: {archetype} (Est. Tokens: {estimated_tokens})\n"
            f"  Routing Shift:  {old_tier} ({old_domain}) -> {new_state['tier']} ({new_state['domain']})\n"
            f"  Active Gear:    {new_state['tier_name']} [{new_state['domain']}]\n"
            f"  Target Model:   {new_state['base_model']}\n"
            f"  Rationale:      {'Sovereign Local Execution' if new_state['domain'] == 'LOCAL' else 'Cloud Elevation Boost'}\n"
        )
        self.decision_log.append(transition_msg)
        if telemetry_cb and switched:
            telemetry_cb(transition_msg)

        return {
            "archetype": archetype,
            "target_tier": target_tier,
            "domain": new_state["domain"],
            "base_model": new_state["base_model"],
            "switched": switched,
            "transition_msg": transition_msg,
            "gear_badge": self.gearbox.format_status_badge()
        }

# ==============================================================================
# BLOCK 4: AUDIT PROCEDURES
# ==============================================================================
def self_audit() -> bool:
    gov = AutoModeGovernor()
    
    # 1. Micro task should route to 0.5b Local
    res = gov.evaluate_and_route("decompose this plan into 3 steps", estimated_tokens=150)
    assert res["target_tier"] == "0.5b" and res["domain"] == "LOCAL"

    # 2. C11 authoring should route to 3b Local
    res = gov.evaluate_and_route("write C dimetric letterbox scaler", estimated_tokens=1200)
    assert res["target_tier"] == "3b" and res["domain"] == "LOCAL"

    # 3. Strategic Summit should route to 72b Cloud
    res = gov.evaluate_and_route("prove mathematical convergence of the moral sway matrix", estimated_tokens=2500)
    assert res["target_tier"] == "72b" and res["domain"] == "CLOUD"

    # 4. Sovereign Handshake: Subsequent local task must snap back to LOCAL
    res = gov.evaluate_and_route("write a quick unit test for math_utils", estimated_tokens=600)
    assert res["domain"] == "LOCAL", "Auto Mode must automatically return to LOCAL when task fits local envelope!"

    return True

# ==============================================================================
# BLOCK 5: CLOSING BLOCK
# ==============================================================================
if __name__ == "__main__":
    gov = AutoModeGovernor()
    print("Testing Auto Mode Governor...")
    if self_audit():
        print("✓ Auto Mode Governor Self-Audit PASSED.")

    queries = [
        ("decompose the build pipeline into a directed acyclic graph", 200),
        ("implement pure C11 integer raymarching for 640x360 canvas", 1500),
        ("audit memory footprint against Invariant 18 and zero-heap chassis", 3000),
        ("refactor all 44 engine headers across the repository", 25000),
        ("prove the mathematical equilibrium of the twin relics and moral sway", 4000)
    ]
    for q, tok in queries:
        res = gov.evaluate_and_route(q, tok, print)
        print(f"Result Badge: {res['gear_badge']}\n")
