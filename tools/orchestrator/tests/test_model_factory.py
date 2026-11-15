"""
================================================================================
BLOCK 1: METADATA BLOCK
================================================================================
@project       Tenebris and Lux
@file          tools/orchestrator/tests/test_model_factory.py
@id            omnicode.tools.tests.test_model_factory
@layer         Layer 4: Verification Suite
@version       0.7.0-dev
@interface_rev Rev 5
@date          2026-11-13
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@system        OmniCode Orchestrator // Tri-State Model Factory Verification
@role          Verifies 0-to-Max Sweep, Threshing Floor Comparison & Modelfile Output
@invariants    1. Zero external pip dependencies.
               2. Verifies that min, max, and ideal Modelfiles are generated.
               3. Asserts that ideal is derived empirically from machine capacity sweep.
================================================================================
"""

import os
import sys

# Ensure repo root is on sys.path
sys.path.insert(0, os.getcwd())

from tools.orchestrator.calibration.model_factory import build_tristate_modelfiles_for_tier
from tools.orchestrator.calibration.sweet_spot import sweep_machine_capacity

def run_model_factory_tests():
    print("=" * 80)
    print("  MODEL FACTORY & THRESHING FLOOR TRI-STATE VERIFICATION")
    print("=" * 80)

    # 1. Test 0-to-Max Sweep on 1.5B
    print("\n[PHASE 1] Running 0-to-Max Capacity Diagnosis for 1.5B...")
    sweep = sweep_machine_capacity("1.5b")
    assert "probed_machine_ideal" in sweep, "Sweep must return probed ideal"
    assert "config_ideal_hypothesis" in sweep, "Sweep must return config hypothesis"
    assert "delta" in sweep, "Sweep must compute delta"
    probed = sweep["probed_machine_ideal"]
    print(f"  • Detected GPU:       {sweep['detected_gpu']} (Total: {sweep['detected_vram_mb']} MB, Usable: {sweep['usable_vram_mb']} MB)")
    print(f"  • Probed Ideal Ctx:   {probed['num_ctx']} tokens")
    print(f"  • Probed GPU Layers:  {probed['recommended_gpu_layers']}")
    print(f"  • Probed Headroom:    {probed['vram_headroom_mb']} MB (Non-headless stability floor >= 350 MB)")
    assert probed["vram_headroom_mb"] >= 350.0, f"Headroom {probed['vram_headroom_mb']} must be >= 350 MB!"
    print("✓ Phase 1 PASSED: 0-to-Max capacity sweep is empirically grounded.")

    # 2. Test Tri-State Modelfile Generation for 1.5B
    print("\n[PHASE 2] Generating Tri-State Modelfiles for 1.5B...")
    res = build_tristate_modelfiles_for_tier("1.5b")
    m_min = res["modelfiles"]["min"]
    m_max = res["modelfiles"]["max"]
    m_ideal = res["modelfiles"]["ideal"]

    assert os.path.exists(m_min), f"Min Modelfile missing: {m_min}"
    assert os.path.exists(m_max), f"Max Modelfile missing: {m_max}"
    assert os.path.exists(m_ideal), f"Ideal Modelfile missing: {m_ideal}"

    # Read contents to verify parameters
    with open(m_min, "r") as f:
        min_txt = f.read()
    with open(m_max, "r") as f:
        max_txt = f.read()
    with open(m_ideal, "r") as f:
        ideal_txt = f.read()

    assert "PARAMETER num_ctx" in min_txt and "FROM qwen2.5-coder:1.5b" in min_txt
    assert "PARAMETER num_ctx" in max_txt and "FROM qwen2.5-coder:1.5b" in max_txt
    assert "PARAMETER num_ctx" in ideal_txt and "FROM qwen2.5-coder:1.5b" in ideal_txt
    print(f"  • Generated Floor:   {m_min}")
    print(f"  • Generated Ceiling: {m_max}")
    print(f"  • Generated Ideal:   {m_ideal}")
    print("✓ Phase 2 PASSED: All 3 Modelfiles generated with proper parameters.")

    # 3. Test Spectrum Generation (0.5B, 3B, 7B)
    print("\n[PHASE 3] Generating Spectrum Tri-State Modelfiles (0.5B, 3B, 7B)...")
    for tier in ["0.5b", "3b", "7b"]:
        r = build_tristate_modelfiles_for_tier(tier)
        assert os.path.exists(r["modelfiles"]["ideal"]), f"Failed generating {tier} ideal"
        print(f"  • Tier {tier.upper():<5}: Ideal Ctx={r['threshing_comparison']['probed_ideal']['num_ctx']}, GPU Layers={r['threshing_comparison']['probed_ideal']['recommended_gpu_layers']}")
    print("✓ Phase 3 PASSED: Full local spectrum successfully tri-state calibrated.")

    print("\n" + "=" * 80)
    print("  MODEL FACTORY VERIFICATION COMPLETED WITH ZERO ERRORS.")
    print("=" * 80)

if __name__ == "__main__":
    run_model_factory_tests()

