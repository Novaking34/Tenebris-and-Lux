"""
================================================================================
BLOCK 1: METADATA BLOCK
================================================================================
@project       Tenebris and Lux
@file          tools/orchestrator/calibration/__init__.py
@id            omnicode.tools.calibration
@layer         Layer 2: Calibration & Factory Cogs
@version       0.7.0-dev
@interface_rev Rev 5
@date          2026-11-13
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@system        OmniCode Orchestrator // Empirical Calibrator & Model Factory
@role          Exports Autonomous Hardware Sweeps and Agent Generation Cogs
@invariants    1. Zero eager submodule imports: Uses PEP 562 lazy module loading to prevent runpy RuntimeWarnings.
================================================================================
"""

__all__ = [
    "run_micro_benchmark",
    "calculate_hardware_sweet_spot",
    "sweep_machine_capacity",
    "build_and_register_1_5b_agent",
    "build_tristate_modelfiles_for_tier"
]

def __getattr__(name: str):
    if name == "run_micro_benchmark":
        from .sweep_runner import run_micro_benchmark
        return run_micro_benchmark
    elif name in ("calculate_hardware_sweet_spot", "sweep_machine_capacity"):
        from . import sweet_spot
        return getattr(sweet_spot, name)
    elif name in ("build_and_register_1_5b_agent", "build_tristate_modelfiles_for_tier"):
        from . import model_factory
        return getattr(model_factory, name)
    raise AttributeError(f"module '{__name__}' has no attribute '{name}'")
