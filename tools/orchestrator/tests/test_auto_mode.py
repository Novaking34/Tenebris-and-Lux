"""
================================================================================
BLOCK 1: METADATA BLOCK
================================================================================
@project       Tenebris and Lux
@file          tools/orchestrator/tests/test_auto_mode.py
@id            omnicode.tools.tests.test_auto_mode
@layer         Layer 4: Verification Suite
@version       0.7.0-dev
@interface_rev Rev 5
@date          2026-11-13
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@system        OmniCode Orchestrator // Auto Mode & Model Switching Verification
@role          Verifies Autonomous Model Routing Across 7 Tiers and Sovereign Handshake
@invariants    1. Zero external pip dependencies.
               2. All transitions must be observable.
               3. The second task fits local space (<= 7B), system must route Local.
================================================================================
"""

import os
import sys

# Ensure repo root is on sys.path
sys.path.insert(0, os.getcwd())

from tools.orchestrator.agent.auto_mode import AutoModeGovernor

def run_auto_mode_tests():
    print("=" * 80)
    print("  AUTO MODE GOVERNOR & DYNAMIC SWITCHING VERIFICATION")
    print("=" * 80)

    gov = AutoModeGovernor()

    # 1. Micro Sifting Task
    print("\n[PHASE 1] Testing Micro-Sifting Routing (0.5B Local)...")
    res1 = gov.evaluate_and_route("decompose the build pipeline into a directed acyclic graph", estimated_tokens=180)
    assert res1["target_tier"] == "0.5b" and res1["domain"] == "LOCAL"
    print(f"  • Routed to: {res1['target_tier']} ({res1['domain']}) | Archetype: {res1['archetype']}")
    print("✓ Phase 1 PASSED: Micro task routed to 0.5B Local Scout.")

    # 2. Interactive Systems Logic (3B Local)
    print("\n[PHASE 2] Testing C11 Systems Logic Routing (3B Local)...")
    res2 = gov.evaluate_and_route("implement pure C11 integer raymarching for 640x360 canvas", estimated_tokens=1500)
    assert res2["target_tier"] == "3b" and res2["domain"] == "LOCAL"
    print(f"  • Routed to: {res2['target_tier']} ({res2['domain']}) | Archetype: {res2['archetype']}")
    print("✓ Phase 2 PASSED: C11 systems logic routed to 3B Local Polymath.")

    # 3. Invariant Audit (7B Local Ceiling)
    print("\n[PHASE 3] Testing Invariant Audit Routing (7B Local Ceiling)...")
    res3 = gov.evaluate_and_route("audit memory footprint against Invariant 18 and zero-heap chassis", estimated_tokens=3000)
    assert res3["target_tier"] == "7b" and res3["domain"] == "LOCAL"
    print(f"  • Routed to: {res3['target_tier']} ({res3['domain']}) | Archetype: {res3['archetype']}")
    print("✓ Phase 3 PASSED: Invariant audit routed to 7B Local Ceiling.")

    # 4. Cloud Elevation: Macro Refactor (32B Cloud)
    print("\n[PHASE 4] Testing Cloud Elevation for Heavy Refactor (32B Cloud)...")
    res4 = gov.evaluate_and_route("refactor all 44 engine headers across the repository", estimated_tokens=25000)
    assert res4["target_tier"] == "32b" and res4["domain"] == "CLOUD"
    print(f"  • Routed to: {res4['target_tier']} ({res4['domain']}) | Archetype: {res4['archetype']}")
    print("✓ Phase 4 PASSED: Heavy context refactor elevated to 32B Cloud Flagship.")

    # 5. Cloud Summit: Mathematical Proofs & Lore (72B Cloud)
    print("\n[PHASE 5] Testing Cloud Summit for Strategic Architecture & Lore (72B Cloud)...")
    res5 = gov.evaluate_and_route("prove mathematical convergence of the moral sway matrix across the 5-route ending manifesto", estimated_tokens=4000)
    assert res5["target_tier"] == "72b" and res5["domain"] == "CLOUD"
    print(f"  • Routed to: {res5['target_tier']} ({res5['domain']}) | Archetype: {res5['archetype']}")
    print("✓ Phase 5 PASSED: Strategic proof elevated to 72B Cloud Summit.")

    # 6. Automatic Sovereign Local Handshake
    print("\n[PHASE 6] Testing Automatic Sovereign Local Handshake...")
    # Follow up with a simple local code request
    res6 = gov.evaluate_and_route("write a quick unit test for vec2_dimetric_project", estimated_tokens=800)
    assert res6["domain"] == "LOCAL", "Must automatically return to LOCAL when task fits local hardware!"
    print(f"  • Reclaimed Sovereignty: {res6['target_tier']} ({res6['domain']}) | Archetype: {res6['archetype']}")
    print("✓ Phase 6 PASSED: Auto Mode automatically snapped back to Local sovereign hardware!")

    print("\n" + "=" * 80)
    print("  ALL AUTO MODE TESTS PASSED WITH ZERO VIOLATIONS.")
    print("=" * 80)

if __name__ == "__main__":
    run_auto_mode_tests()

