"""
================================================================================
BLOCK 1: METADATA BLOCK
================================================================================
@project       Tenebris and Lux
@file          tools/orchestrator/probes/__init__.py
@id            omnicode.tools.probes
@layer         Layer 1: Hardware & System Probes
@version       0.7.2-dev
@interface_rev Rev 5
@date          2026-11-13
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@system        OmniCode Orchestrator // Hardware & System Probes Anchor
@role          Exports CPU, GPU, and Memory Probes via PEP 562 Lazy Loading
@invariants    1. Zero eager submodule imports: Prevents runpy RuntimeWarnings.
================================================================================
"""

__all__ = [
    "probe_cpu_topology",
    "get_cpu_load",
    "probe_gpu_telemetry",
    "probe_system_memory"
]

def __getattr__(name: str):
    if name in ("probe_cpu_topology", "get_cpu_load"):
        from . import cpu_probe
        return getattr(cpu_probe, name)
    elif name == "probe_gpu_telemetry":
        from . import gpu_probe
        return getattr(gpu_probe, name)
    elif name == "probe_system_memory":
        from . import memory_probe
        return getattr(memory_probe, name)
    raise AttributeError(f"module '{__name__}' has no attribute '{name}'")
