"""
================================================================================
BLOCK 1: METADATA BLOCK
================================================================================
@project       Tenebris and Lux
@file          tools/orchestrator/exoskeleton/compiler_tripwire.py
@id            omnicode.tools.exoskeleton.compiler_tripwire
@layer         Layer 2: Verification & Tooling Cogs
@version       0.4.0-dev
@interface_rev Rev 5
@date          2026-11-13
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@system        OmniCode Orchestrator // Compiler Diagnostic Triage Probe
@role          Executes cmake/ninja/cargo Builds and Compresses Compiler Errors
@invariants    1. Zero external pip dependencies.
               2. Extracts concise file:line:col error messages to save LLM tokens.
================================================================================
"""

# ==============================================================================
# BLOCK 2: OPENING BLOCK
# ==============================================================================
import os
import subprocess
from typing import Tuple, Optional

# ==============================================================================
# BLOCK 3: BODY BLOCK
# ==============================================================================
def find_workspace_root() -> str:
    """Finds repository root directory containing CMakeLists.txt."""
    current = os.path.dirname(os.path.abspath(__file__))
    while current and current != os.path.dirname(current):
        if os.path.isfile(os.path.join(current, "CMakeLists.txt")):
            return current
        current = os.path.dirname(current)
    return os.getcwd()

def run_compiler_probe(workspace_root: Optional[str] = None) -> Tuple[bool, str]:
    root = workspace_root or find_workspace_root()
    build_dir = os.path.join(root, "build")

    if os.path.isdir(build_dir):
        cmd = "cmake --build build -- -j6"
    elif os.path.isfile(os.path.join(root, "Cargo.toml")):
        cmd = "cargo check"
    else:
        return True, "No CMake build/ directory or Cargo.toml found in workspace."

    try:
        proc = subprocess.run(cmd, shell=True, cwd=root, capture_output=True, text=True, timeout=60)
        output = proc.stdout + proc.stderr
        if proc.returncode == 0:
            return True, f"Build succeeded cleanly under '{cmd}'."
        else:
            error_lines = []
            for l in output.splitlines():
                if ": error:" in l or ": warning:" in l or "error[" in l or "FAILED:" in l:
                    error_lines.append(l.strip())
            summary = "\n".join(error_lines[:25]) if error_lines else output[:1000]
            return False, f"Build failed with exit code {proc.returncode}:\n{summary}"
    except Exception as e:
        return False, f"Compiler probe exception: {e}"

# ==============================================================================
# BLOCK 4: AUDIT PROCEDURES
# ==============================================================================
def self_audit() -> bool:
    root = find_workspace_root()
    return os.path.isdir(root)

# ==============================================================================
# BLOCK 5: CLOSING BLOCK
# ==============================================================================
if __name__ == "__main__":
    ok, msg = run_compiler_probe()
    print(f"Compiler probe: {'PASS' if ok else 'FAIL'}\n{msg}")

