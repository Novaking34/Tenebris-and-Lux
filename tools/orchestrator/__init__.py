"""
================================================================================
BLOCK 1: METADATA BLOCK
================================================================================
@project       Tenebris and Lux
@file          tools/orchestrator/__init__.py
@id            omnicode.tools.orchestrator
@layer         Layer 4: Toolchain Integration
@version       0.7.0-dev
@interface_rev Rev 5
@date          2026-11-13
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@system        OmniCode Orchestrator // Package Root Anchor
@role          Modular CPI-SI Cognitive Machine Chassis & Exoskeleton Suite
@invariants    1. Universal 5-Block Lifecycle Protocol enforced.
               2. Strict downward dependency flow (Layer 0 -> 1 -> 2 -> 3 -> 4).
               3. Zero eager submodule imports: PEP 562 lazy loading prevents runpy RuntimeWarnings.
================================================================================
"""

__all__ = [
    "build_and_register_1_5b_agent",
    "build_tristate_modelfiles_for_tier",
    "launch_gui"
]

def __getattr__(name: str):
    if name in ("build_and_register_1_5b_agent", "build_tristate_modelfiles_for_tier"):
        from .calibration import model_factory
        return getattr(model_factory, name)
    elif name == "launch_gui":
        from .gui.app import launch_gui
        return launch_gui
    raise AttributeError(f"module '{__name__}' has no attribute '{name}'")
