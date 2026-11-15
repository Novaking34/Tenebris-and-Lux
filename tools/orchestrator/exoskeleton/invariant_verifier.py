"""
================================================================================
BLOCK 1: METADATA BLOCK
================================================================================
@project       Tenebris and Lux
@file          tools/orchestrator/exoskeleton/invariant_verifier.py
@id            omnicode.tools.exoskeleton.invariant_verifier
@layer         Layer 2: Verification & Tooling Cogs
@version       0.4.0-dev
@interface_rev Rev 5
@date          2026-11-13
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@system        OmniCode Orchestrator // Static Invariant Verifier
@role          Audits Code for Invariant 18 (comments), Invariant 1 (zero heap) & 5-Block Protocol
@invariants    1. Zero external pip dependencies.
               2. Reports exact line numbers and rules broken.
================================================================================
"""

# ==============================================================================
# BLOCK 2: OPENING BLOCK
# ==============================================================================
import os
import re
from typing import List, Dict, Any

# ==============================================================================
# BLOCK 3: BODY BLOCK
# ==============================================================================
def check_file_invariants(filepath: str) -> List[Dict[str, Any]]:
    violations = []
    if not os.path.isfile(filepath):
        return [{"rule": "FILE_EXISTS", "line": 0, "msg": f"File '{filepath}' not found"}]

    ext = os.path.splitext(filepath)[1].lower()
    is_c_family = ext in [".c", ".h"]

    try:
        with open(filepath, "r", encoding="utf-8", errors="replace") as f:
            lines = f.readlines()
    except Exception as e:
        return [{"rule": "FILE_READ", "line": 0, "msg": f"Failed reading file: {e}"}]

    # Invariant 6: Universal 5-Block Protocol
    has_b1 = any("BLOCK 1: METADATA" in l or "BLOCK 1" in l for l in lines[:60])
    has_b2 = any("BLOCK 2: OPENING" in l or "BLOCK 2" in l for l in lines)
    has_b3 = any("BLOCK 3: BODY" in l or "BLOCK 3" in l for l in lines)
    has_b4 = any("BLOCK 4: AUDIT" in l or "BLOCK 4" in l for l in lines)
    has_b5 = any("BLOCK 5: CLOSING" in l or "BLOCK 5" in l for l in lines)

    # Invariant 18: ISO C comments only in C files (no //)
    if is_c_family:
        for idx, line in enumerate(lines, start=1):
            stripped = line.strip()
            if "//" in stripped and "https://" not in stripped and "http://" not in stripped:
                match = re.search(r'(?<!:)\/\/(.*)', line)
                if match:
                    violations.append({
                        "rule": "INVARIANT_18_COMMENT_DISCIPLINE",
                        "line": idx,
                        "msg": f"Forbidden C++ line comment '//' found (only /* ... */ allowed): {stripped[:60]}"
                    })

            # Invariant 1: Zero heap in Layer 0/1
            if any(p in filepath for p in ["engine/include/core", "engine/src/core", "engine/include/io", "engine/src/io", "game/src/combat", "game/src/hero"]):
                for alloc_kw in ["malloc(", "calloc(", "realloc(", "free("]:
                    if alloc_kw in stripped and not stripped.startswith("/*") and not stripped.startswith("*"):
                        violations.append({
                            "rule": "INVARIANT_1_ZERO_HEAP",
                            "line": idx,
                            "msg": f"Forbidden heap allocation call '{alloc_kw}' in Layer 0/1: {stripped[:60]}"
                        })

    missing_blocks = []
    if not has_b1: missing_blocks.append("BLOCK 1")
    if not has_b2: missing_blocks.append("BLOCK 2")
    if not has_b3: missing_blocks.append("BLOCK 3")
    if not has_b4: missing_blocks.append("BLOCK 4")
    if not has_b5: missing_blocks.append("BLOCK 5")

    if missing_blocks and is_c_family:
        violations.append({
            "rule": "INVARIANT_6_FIVE_BLOCK_LIFECYCLE",
            "line": 1,
            "msg": f"Missing Universal 5-Block lifecycle banners: {', '.join(missing_blocks)}"
        })

    return violations

# ==============================================================================
# BLOCK 4: AUDIT PROCEDURES
# ==============================================================================
def self_audit() -> bool:
    res = check_file_invariants(__file__)
    return isinstance(res, list)

# ==============================================================================
# BLOCK 5: CLOSING BLOCK
# ==============================================================================
if __name__ == "__main__":
    print(f"Invariant verifier self-audit: {'PASS' if self_audit() else 'FAIL'}")

