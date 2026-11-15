"""
================================================================================
BLOCK 1: METADATA BLOCK
================================================================================
@project       Tenebris and Lux
@file          tools/orchestrator/memory/symbol_indexer.py
@id            omnicode.tools.memory.symbol_indexer
@layer         Layer 2: Memory & Context Cogs
@version       0.4.0-dev
@interface_rev Rev 5
@date          2026-11-13
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@system        OmniCode Orchestrator // AST Symbol & Topic Extractor
@role          Extracts Identifiers, Types, and Topics for Associative Memory Paging
@invariants    1. Zero external pip dependencies.
               2. Safe token filtering with standard English/programming stop words.
================================================================================
"""

# ==============================================================================
# BLOCK 2: OPENING BLOCK
# ==============================================================================
import re
from typing import List, Set

STOP_WORDS: Set[str] = {
    "the", "and", "for", "with", "this", "that", "write", "make", "create",
    "function", "please", "what", "from", "have", "here", "need", "does",
    "return", "using", "into", "then", "when", "some", "more", "like", "will",
    "about", "also", "been", "could", "would", "should", "their", "there",
    "these", "those", "which", "where", "while", "class", "import"
}

# ==============================================================================
# BLOCK 3: BODY BLOCK
# ==============================================================================
def extract_code_symbols(text: str, max_symbols: int = 8) -> List[str]:
    """
    Extracts code identifiers, types, function names, and meaningful keywords.
    """
    words = re.findall(r'\b[A-Za-z_][A-Za-z0-9_]{2,}\b', text)
    filtered = [w for w in set(words) if w.lower() not in STOP_WORDS]
    # Sort by length descending to prioritize rich domain names
    filtered.sort(key=lambda s: len(s), reverse=True)
    return filtered[:max_symbols]

# ==============================================================================
# BLOCK 4: AUDIT PROCEDURES
# ==============================================================================
def self_audit() -> bool:
    syms = extract_code_symbols("math_clamp_int in Cornerstone Engine Layer 0")
    return "math_clamp_int" in syms

# ==============================================================================
# BLOCK 5: CLOSING BLOCK
# ==============================================================================
if __name__ == "__main__":
    s = extract_code_symbols("math_clamp_int with ALLOC_ZERO_HEAP in Cornerstone")
    print(f"Extracted symbols: {s}")

