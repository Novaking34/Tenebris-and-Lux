"""
================================================================================
BLOCK 1: METADATA BLOCK
================================================================================
@project       Tenebris and Lux
@file          tools/orchestrator/agent/__init__.py
@id            omnicode.tools.agent
@layer         Layer 3: Agent & Dispatcher Cogs
@version       0.4.0-dev
@interface_rev Rev 5
@date          2026-11-13
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@system        OmniCode Orchestrator // CPI-SI Cognitive Dispatcher & Loop
@role          Exports Tool Dispatcher, Reflection Loop and Subagent Factory
@invariants    1. ALLOC_ZERO_CIRCULAR: Imports only from Layers 0, 1, 2 and standard library.
================================================================================
"""

from .dispatcher import AgenticToolDispatcher
from .reflection_loop import stream_model_response, run_agentic_turn
from .subagent_factory import SubagentFactory

__all__ = [
    "AgenticToolDispatcher",
    "stream_model_response",
    "run_agentic_turn",
    "SubagentFactory"
]

