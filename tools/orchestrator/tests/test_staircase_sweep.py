"""
================================================================================
BLOCK 1: METADATA BLOCK
================================================================================
@project       Tenebris and Lux
@file          tools/orchestrator/test_staircase_sweep.py
@id            omnicode.tools.test_staircase_sweep
@layer         Layer 4: Verification Suite
@version       0.7.0-dev
@interface_rev Rev 5
@date          2026-11-13
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@system        OmniCode Orchestrator // Full-Staircase Bi-Directional Sweep Test
@role          Verifies Rise-Up, Fall-Down, Sovereign Handshake & 0.5B Floor Tripwire
@invariants    1. Zero external pip dependencies.
               2. All transitions must be observable moments.
               3. Crossing <= 7B from Cloud MUST trigger Sovereign Local Mode.
               4. Failing past 0.5B Min MUST trigger Level 0 Safe-Mode tripwire.
================================================================================
"""

import sys
import os

# Ensure repo root is on sys.path
sys.path.insert(0, os.getcwd())

from tools.orchestrator.agent.stepped_gearbox import SteppedGearbox

def run_staircase_verification():
    print("=" * 80)
    print("  OMNICODE 7-TIER STAIRCASE & SOVEREIGN LOCAL HANDSHAKE VERIFICATION")
    print("=" * 80)

    gearbox = SteppedGearbox()
    events = []

    def record_event(msg: str):
        events.append(msg)
        print(msg)

    # 1. Verify initial baseline
    print(f"\n[PHASE 1] Initial Baseline Check")
    state = gearbox.get_current_state()
    print("Initial Badge:", gearbox.format_status_badge())
    assert state["tier"] == "1.5b", f"Expected 1.5b, got {state['tier']}"
    assert state["mode"] == "ideal", f"Expected ideal mode, got {state['mode']}"
    assert state["domain"] == "LOCAL", f"Expected LOCAL domain, got {state['domain']}"
    print("✓ Initial state verified: 1.5B Ideal on Local Chassis.")

    # 2. Descend to Bedrock Floor (0.5B Min)
    print(f"\n[PHASE 2] Fall-Down to Local Bedrock Floor (0.5B Min)")
    gearbox.set_gear("0.5b", "min", "LOCAL")
    state = gearbox.get_current_state()
    print("Bedrock State:", gearbox.format_status_badge())
    assert state["tier"] == "0.5b" and state["mode"] == "min" and state["domain"] == "LOCAL"
    print("✓ 0.5B Min Bedrock Floor reached.")

    # 3. Test the Bedrock Tripwire: Can we fall below 0.5B Min?
    print(f"\n[PHASE 3] Testing Bedrock Floor Tripwire (Loss of Floor)")
    ok, msg = gearbox.fall_down("Simulated physical fault below 0.5B", record_event)
    assert not ok, "Expected fall_down below 0.5B Min to fail!"
    assert "CRITICAL TRIPWIRE ENGAGED: WE HAVE LOST THE FLOOR!" in msg
    print("✓ Bedrock Floor Tripwire verified: Level 0 Safe-Mode BIOS engaged.")

    # 4. Ascend through Local PC Envelope: 0.5B -> 1.5B -> 3B -> 7B
    print(f"\n[PHASE 4] Climbing the Local Staircase (0.5B -> 7B Max)")
    gearbox.set_gear("0.5b", "ideal", "LOCAL")
    for target in ["1.5b", "3b", "7b"]:
        gearbox.set_gear(target, "ideal", "LOCAL")
        assert gearbox.current_domain == "LOCAL", f"Expected LOCAL domain at {target}"
        print(f"  Reached Local Tier: {gearbox.format_status_badge()}")

    # Maximize 7B (Local PC Hardware Ceiling)
    gearbox.current_mode = "max"
    print(f"  At Local Hardware Ceiling: {gearbox.format_status_badge()}")
    assert gearbox.current_tier == "7b" and gearbox.current_mode == "max"
    print("✓ Local staircase verified up to 7B Physical Ceiling.")

    # 5. Cross Sovereign Boundary into Cloud: 14B -> 32B -> 72B Summit
    print(f"\n[PHASE 5] Crossing Sovereign Boundary into Cloud Domain")
    # Simulate active cloud connection for elevation test
    gearbox.cloud_bridge.cloud_enabled = True
    gearbox.cloud_bridge.ping_cloud_server = lambda: (True, 18.5, "Simulated Cloud Server Online (18.5ms)")

    ok, msg = gearbox.rise_up("Task requires macro-refactor beyond 16GB RAM", record_event)
    assert ok, f"Expected successful elevation to 14B: {msg}"
    assert gearbox.current_tier == "14b", f"Expected 14b, got {gearbox.current_tier}"
    assert gearbox.current_domain == "CLOUD", f"Expected CLOUD domain, got {gearbox.current_domain}"
    print(f"  Elevated to Cloud: {gearbox.format_status_badge()}")

    # Elevate to 32B Coder Flagship
    gearbox.set_gear("32b", "ideal", "CLOUD")
    print(f"  Cloud Coder Flagship: {gearbox.format_status_badge()}")

    # Elevate to 72B Strategic Architecture Summit
    gearbox.set_gear("72b", "max", "CLOUD")
    print(f"  Cloud Summit: {gearbox.format_status_badge()}")
    assert gearbox.current_tier == "72b" and gearbox.current_mode == "max"
    print("✓ Cloud spectrum verified up to 72B Cognitive Summit.")

    # 6. Test Observable Fall-Down & The Automatic Sovereign Local Handshake
    print(f"\n[PHASE 6] Testing Descent & Automatic Sovereign Local Handshake")
    # Start at 14B Min in Cloud
    gearbox.set_gear("14b", "min", "CLOUD")
    print(f"  Starting at Cloud Floor: {gearbox.format_status_badge()}")
    assert gearbox.current_domain == "CLOUD"

    # Step down: Should transition from 14B Cloud -> 7B LOCAL!
    ok, msg = gearbox.fall_down("Remote connection throttled / task fits local RAM", record_event)
    assert ok
    assert gearbox.current_tier == "7b", f"Expected 7b, got {gearbox.current_tier}"
    assert gearbox.current_domain == "LOCAL", f"CRITICAL: Expected domain to auto-trigger LOCAL! Got {gearbox.current_domain}"
    print(f"  Sovereign Handshake Triggered: {gearbox.format_status_badge()}")
    print("✓ Sovereign Local Handshake VERIFIED: Cloud space matched local capacity and snapped back to Local Mode!")

    print("\n" + "=" * 80)
    print("  ALL 6 VERIFICATION PHASES PASSED WITH ZERO VIOLATIONS.")
    print("=" * 80)

if __name__ == "__main__":
    run_staircase_verification()

