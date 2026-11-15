"""
================================================================================
BLOCK 1: METADATA BLOCK
================================================================================
@project       Tenebris and Lux
@file          tools/orchestrator/memory/__init__.py
@id            omnicode.tools.memory
@layer         Layer 2: Memory & Context Cogs
@version       0.4.0-dev
@interface_rev Rev 5
@date          2026-11-13
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@system        OmniCode Orchestrator // Addressed Virtual Memory & Episodic Store
@role          Exports Track 4 Addressed Index Memory and Elastic Window Scaler
@invariants    1. ALLOC_ZERO_CIRCULAR: Imports only from Layer 0 types and standard library.
================================================================================
"""

from .index_store import EpisodicIndexStore
from .elastic_window import calculate_elastic_context
from .symbol_indexer import extract_code_symbols

__all__ = [
    "EpisodicIndexStore",
    "calculate_elastic_context",
    "extract_code_symbols"
]

