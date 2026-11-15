"""
================================================================================
BLOCK 1: METADATA BLOCK
================================================================================
@project       Tenebris and Lux
@file          tools/orchestrator/agent/stepped_gearbox.py
@id            omnicode.tools.agent.stepped_gearbox
@layer         Layer 3: Agent & Dispatcher Cogs
@version       0.7.0-dev
@interface_rev Rev 5
@date          2026-11-13
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@system        OmniCode Orchestrator // The Stepped Staircase Gearbox
@role          Orchestrates Smooth Rise-Up & Observable Fall-Down Transitions Across 7 Tiers
@invariants    1. Zero external pip dependencies.
               2. Every transition is an observable moment (emits structured telemetry, never silent crashes).
               3. The second cloud space matches local capability (<= 7B), system auto-triggers Local Mode.
               4. On this chassis, 14B, 32B, and 72B require Cloud Elevation.
               5. The absolute bedrock floor is 0.5B Min. If 0.5B Min fails, tripwire engages Level 0 Safe-Mode BIOS.
================================================================================
"""

# ==============================================================================
# BLOCK 2: OPENING BLOCK
# ==============================================================================
import os
import time
from typing import Dict, Any, Optional, Tuple, Callable
from ..types.config_schema import load_json_block_3
from ..probes.gpu_probe import probe_gpu_telemetry
from ..cloud.cloud_bridge import CloudNexusBridge

# ==============================================================================
# BLOCK 3: BODY BLOCK
# ==============================================================================
class SteppedGearbox:
    """
    State machine managing the continuous bi-directional staircase from
    0.5B (Bedrock Floor) to 72B (Cloud Summit), synthesizing Min, Ideal, and Max
    envelopes across Local and Cloud domains.
    """

    TIER_ORDER = ["0.5b", "1.5b", "3b", "7b", "14b", "32b", "72b"]
    SOVEREIGN_LOCAL_CEILING = "7b"
    CLOUD_ENTRY_FLOOR = "14b"

    def __init__(self, config_path: str = "tools/orchestrator/config/staircase.json"):
        self.config_path = config_path
        self.tiers: Dict[str, Any] = {}
        self.cloud_strategy: Dict[str, Any] = {}
        self._load_config()

        # Operational state: starts at 1.5B Ideal on Local Chassis
        self.current_tier = "1.5b"
        self.current_mode = "ideal"  # "min", "ideal", "max"
        self.current_domain = "LOCAL"  # "LOCAL" or "CLOUD"

        self.cloud_bridge = CloudNexusBridge()
        self.transition_log = []

    def _load_config(self):
        try:
            body = load_json_block_3(self.config_path)
            self.tiers = body.get("tiers", {})
            self.cloud_strategy = body.get("cloud_strategy", {})
        except Exception:
            self.tiers = {}
            self.cloud_strategy = {}

    def is_local_supported(self, tier: str) -> bool:
        """Determines if a tier can execute natively on this physical host."""
        tier_data = self.tiers.get(tier, {})
        return tier_data.get("local_supported", False)

    def get_current_state(self) -> Dict[str, Any]:
        """Returns the active operational profile for the current gear and domain."""
        tier_data = self.tiers.get(self.current_tier, {})
        
        # Select active envelope based on domain
        if self.current_domain == "LOCAL" and self.is_local_supported(self.current_tier):
            envelope = tier_data.get("local_envelope", {})
        else:
            envelope = tier_data.get("cloud_envelope", {})
            
        profile = envelope.get(self.current_mode, {})

        return {
            "tier": self.current_tier,
            "mode": self.current_mode,
            "domain": self.current_domain,
            "tier_name": tier_data.get("name", self.current_tier),
            "base_model": tier_data.get("base_model", "omnicode-fast-1.5b"),
            "local_supported": tier_data.get("local_supported", False),
            "role": tier_data.get("role", ""),
            "profile": profile
        }

    def format_status_badge(self) -> str:
        """Emits a concise HUD string for GUI and CLI status displays."""
        state = self.get_current_state()
        p = state["profile"]
        ctx = p.get("num_ctx", 2048)
        desc = p.get("description", "")
        return f"[GEAR: {state['tier_name']} | {state['domain']} | {self.current_mode.upper()} | Ctx: {ctx} | {desc}]"

    def rise_up(self, reason: str = "Demand escalated", telemetry_cb: Optional[Callable[[str], None]] = None) -> Tuple[bool, str]:
        """
        Elevates cognitive power step-by-step:
        MIN -> IDEAL -> MAX -> (Next Tier IDEAL).
        Crosses from Local to Cloud when exceeding Sovereign Local Ceiling (7B).
        """
        old_state_str = f"{self.current_tier}_{self.current_mode} ({self.current_domain})"

        if self.current_mode == "min":
            self.current_mode = "ideal"
        elif self.current_mode == "ideal":
            self.current_mode = "max"
        elif self.current_mode == "max":
            curr_idx = self.TIER_ORDER.index(self.current_tier)
            if curr_idx + 1 < len(self.TIER_ORDER):
                next_tier = self.TIER_ORDER[curr_idx + 1]

                # Check if stepping across the Sovereign Boundary into Cloud
                if not self.is_local_supported(next_tier):
                    alive, lat, msg = self.cloud_bridge.ping_cloud_server()
                    if not alive:
                        clamped_msg = (
                            f"[RISE UP CLAMPED] Cannot elevate to {next_tier}: Cloud is offline ({msg}). "
                            f"System clamped safely at {self.current_tier} Max on Local Chassis."
                        )
                        if telemetry_cb:
                            telemetry_cb(clamped_msg)
                        return False, clamped_msg

                    # Elevate into Cloud
                    self.current_domain = "CLOUD"
                    elevation_event = (
                        f"\n[OBSERVABLE MOMENT // CLOUD ELEVATION BOOST]\n"
                        f"  Trigger:  Local PC Ceiling ({self.current_tier} Max) reached.\n"
                        f"  Action:   Crossing Sovereign Boundary into Cloud Nexus.\n"
                        f"  Target:   {next_tier} (Remote Compute Activated)\n"
                    )
                    self.transition_log.append(elevation_event)
                    if telemetry_cb:
                        telemetry_cb(elevation_event)

                self.current_tier = next_tier
                self.current_mode = "ideal"
            else:
                summit_msg = "Already at the absolute summit (72B Cloud Max). Cannot rise further."
                if telemetry_cb:
                    telemetry_cb(summit_msg)
                return False, summit_msg

        new_state = self.get_current_state()
        new_state_str = f"{self.current_tier}_{self.current_mode} ({self.current_domain})"
        event_msg = (
            f"[OBSERVABLE MOMENT // RISE UP]\n"
            f"  Shift:    {old_state_str} -> {new_state_str}\n"
            f"  Reason:   {reason}\n"
            f"  New Gear: {new_state['tier_name']} [{new_state['domain']}]\n"
            f"  Profile:  {new_state['profile'].get('description', '')} (Ctx: {new_state['profile'].get('num_ctx', 2048)})\n"
        )
        self.transition_log.append(event_msg)
        if telemetry_cb:
            telemetry_cb(event_msg)
        return True, event_msg

    def fall_down(self, reason: str = "Hardware pressure detected", telemetry_cb: Optional[Callable[[str], None]] = None) -> Tuple[bool, str]:
        """
        Gracefully descends step-by-step:
        MAX -> IDEAL -> MIN -> (Previous Tier IDEAL).
        Auto-triggers Local Mode the moment cloud space matches local capacity (<= 7B).
        """
        old_state_str = f"{self.current_tier}_{self.current_mode} ({self.current_domain})"

        if self.current_mode == "max":
            self.current_mode = "ideal"
        elif self.current_mode == "ideal":
            self.current_mode = "min"
        elif self.current_mode == "min":
            curr_idx = self.TIER_ORDER.index(self.current_tier)
            if curr_idx > 0:
                prev_tier = self.TIER_ORDER[curr_idx - 1]

                # Check if stepping down into Local Sovereign Envelope (<= 7B)
                if self.current_domain == "CLOUD" and self.is_local_supported(prev_tier):
                    self.current_domain = "LOCAL"
                    sovereign_event = (
                        f"\n[OBSERVABLE MOMENT // SOVEREIGN LOCAL HANDSHAKE]\n"
                        f"  Trigger:  Operational space matches local hardware capability ({prev_tier} <= {self.SOVEREIGN_LOCAL_CEILING}).\n"
                        f"  Action:   Auto-triggering Local Mode. Reclaiming full physical sovereignty.\n"
                        f"  Result:   Zero network latency, zero external token burn, air-gapped execution.\n"
                    )
                    self.transition_log.append(sovereign_event)
                    if telemetry_cb:
                        telemetry_cb(sovereign_event)

                self.current_tier = prev_tier
                self.current_mode = "ideal"
            else:
                # 0.5B Min has failed! The bedrock floor is gone!
                critical_msg = (
                    "\n"
                    "╔════════════════════════════════════════════════════════════════════╗\n"
                    "║  [CRITICAL TRIPWIRE ENGAGED: WE HAVE LOST THE FLOOR!]              ║\n"
                    "║  0.5B Minimum Floor has encountered an unrecoverable fault.        ║\n"
                    "║  Halting LLM execution. Stepping down to Level 0 Safe-Mode BIOS.   ║\n"
                    "╚════════════════════════════════════════════════════════════════════╝\n"
                )
                self.transition_log.append(critical_msg)
                if telemetry_cb:
                    telemetry_cb(critical_msg)
                return False, critical_msg

        new_state = self.get_current_state()
        new_state_str = f"{self.current_tier}_{self.current_mode} ({self.current_domain})"
        event_msg = (
            f"[OBSERVABLE MOMENT // FALL DOWN]\n"
            f"  Shift:    {old_state_str} -> {new_state_str}\n"
            f"  Reason:   {reason}\n"
            f"  New Gear: {new_state['tier_name']} [{new_state['domain']}]\n"
            f"  Profile:  {new_state['profile'].get('description', '')} (Ctx: {new_state['profile'].get('num_ctx', 2048)})\n"
        )
        self.transition_log.append(event_msg)
        if telemetry_cb:
            telemetry_cb(event_msg)
        return True, event_msg

    def set_gear(self, tier: str, mode: str = "ideal", domain: Optional[str] = None) -> bool:
        """Manually shifts to a specific gear (with validation)."""
        if tier not in self.TIER_ORDER:
            return False
        if mode not in ["min", "ideal", "max"]:
            return False

        self.current_tier = tier
        self.current_mode = mode

        if domain:
            self.current_domain = domain
        else:
            # Auto-assign domain based on local support
            if self.is_local_supported(tier):
                self.current_domain = "LOCAL"
            else:
                self.current_domain = "CLOUD"

        return True

# ==============================================================================
# BLOCK 4: AUDIT PROCEDURES
# ==============================================================================
def self_audit() -> bool:
    gearbox = SteppedGearbox()
    assert gearbox.current_tier == "1.5b"
    assert gearbox.current_mode == "ideal"
    assert gearbox.current_domain == "LOCAL"

    # Test fall down within local
    ok, _ = gearbox.fall_down("Test Pressure")
    assert ok and gearbox.current_mode == "min"

    # Test rise up to ideal
    ok, _ = gearbox.rise_up("Test Recovery")
    assert ok and gearbox.current_mode == "ideal"

    # Test sovereign boundary handshake:
    # Set to 14B in Cloud, then fall down to 7B. Should auto-trigger LOCAL!
    gearbox.set_gear("14b", "min", "CLOUD")
    assert gearbox.current_domain == "CLOUD"

    ok, _ = gearbox.fall_down("Falling to Local Territory")
    assert ok
    assert gearbox.current_tier == "7b"
    assert gearbox.current_domain == "LOCAL", "Must auto-trigger LOCAL when descending to 7B!"

    return True

# ==============================================================================
# BLOCK 5: CLOSING BLOCK
# ==============================================================================
if __name__ == "__main__":
    gb = SteppedGearbox()
    print("Initial State:", gb.format_status_badge())
    print("\nRunning Self-Audit...")
    if self_audit():
        print("✓ Gearbox Self-Audit PASSED: Sovereign Handshake verified.")
    print("\n--- Simulating Fall Down Under Memory Pressure ---")
    gb.fall_down("Approaching KV Cache Cliff", print)
    gb.fall_down("Heavy X11 Rasterizer Allocations", print)
    print("\nState after downshifts:", gb.format_status_badge())
    print("\n--- Simulating Rise Up As Resources Free ---")
    gb.rise_up("Task requires wider multi-file context", print)
    print("\nState after elevation:", gb.format_status_badge())
