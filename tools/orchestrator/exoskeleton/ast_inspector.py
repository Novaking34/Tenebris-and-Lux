"""
================================================================================
BLOCK 1: METADATA BLOCK
================================================================================
@project       Tenebris and Lux
@file          tools/orchestrator/exoskeleton/ast_inspector.py
@id            omnicode.tools.exoskeleton.ast_inspector
@layer         Layer 2: Verification & Tooling Cogs
@version       0.4.0-dev
@interface_rev Rev 5
@date          2026-11-13
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@system        OmniCode Orchestrator // AST Skeleton & API Contract Extractor
@role          Compresses Source Files to High-Density API Contracts (65-95% Token Savings)
@invariants    1. Zero external pip dependencies.
               2. Preserves Block 1 Metadata and Invariants intact.
               3. Preserves all type definitions, structs, enums, unions, typedefs.
               4. Preserves function signatures and docstrings, replacing bodies with ';'.
================================================================================
"""

# ==============================================================================
# BLOCK 2: OPENING BLOCK
# ==============================================================================
import os
import re
from typing import Tuple, Optional, List

# ==============================================================================
# BLOCK 3: BODY BLOCK
# ==============================================================================
def skeletonize_c(content: str) -> Tuple[str, int, int]:
    lines = content.splitlines()
    orig_lines = len(lines)
    output: List[str] = []
    in_block1 = False
    in_block_comment = False
    i = 0
    n = len(lines)

    while i < n:
        line = lines[i]
        stripped = line.strip()

        if "BLOCK 1: METADATA BLOCK" in line or (stripped.startswith("/*") and "@project" in content[:1000]):
            in_block1 = True

        if in_block1:
            output.append(line)
            if "*/" in line:
                in_block1 = False
            i += 1
            continue

        if stripped.startswith("#"):
            output.append(line)
            i += 1
            continue

        if any(stripped.startswith(k) for k in ["typedef", "struct", "union", "enum"]):
            decl_lines = [line]
            brace_count = line.count("{") - line.count("}")
            while i + 1 < n and (brace_count > 0 or not stripped.endswith(";")):
                i += 1
                line = lines[i]
                stripped = line.strip()
                decl_lines.append(line)
                brace_count += line.count("{") - line.count("}")
                if brace_count == 0 and ";" in line:
                    break
            output.extend(decl_lines)
            i += 1
            continue

        if "BLOCK " in line and ("=====" in line or "-----" in line):
            output.append(line)
            i += 1
            continue

        if stripped.startswith("/*") or stripped.startswith("/**") or in_block_comment:
            output.append(line)
            if "*/" in line:
                in_block_comment = False
            else:
                in_block_comment = True
            i += 1
            continue

        if "(" in line and not stripped.startswith("//") and not stripped.startswith("/*"):
            if stripped.endswith(";"):
                output.append(line)
                i += 1
                continue

            sig_lines = [line]
            has_brace = "{" in line
            while i + 1 < n and not has_brace and not stripped.endswith(";"):
                i += 1
                line = lines[i]
                stripped = line.strip()
                sig_lines.append(line)
                if "{" in line:
                    has_brace = True
                    break

            if has_brace:
                body_start = i
                brace_depth = "".join(sig_lines).count("{") - "".join(sig_lines).count("}")
                while i + 1 < n and brace_depth > 0:
                    i += 1
                    line = lines[i]
                    brace_depth += line.count("{") - line.count("}")

                body_lines_count = i - body_start + 1
                clean_sig = "\n".join(sig_lines)
                clean_sig = re.sub(r'\s*\{\s*$', '', clean_sig)
                output.append(f"{clean_sig}; /* [BODY STRIPPED: {body_lines_count} lines] */")
                i += 1
                continue
            else:
                output.extend(sig_lines)
                i += 1
                continue

        if stripped.endswith(";") or not stripped:
            if stripped:
                output.append(line)
        i += 1

    result = "\n".join(output)
    return result, orig_lines, len(output)

def skeletonize_rust(content: str) -> Tuple[str, int, int]:
    lines = content.splitlines()
    orig_lines = len(lines)
    output: List[str] = []
    i = 0
    n = len(lines)

    while i < n:
        line = lines[i]
        stripped = line.strip()

        if stripped.startswith("#[") or stripped.startswith("///") or stripped.startswith("//!") or stripped.startswith("/*"):
            output.append(line)
            i += 1
            continue

        if stripped.startswith("use ") or stripped.startswith("pub use "):
            output.append(line)
            i += 1
            continue

        if any(k in stripped for k in ["struct ", "enum ", "type ", "trait "]):
            decl_lines = [line]
            brace_count = line.count("{") - line.count("}")
            while i + 1 < n and brace_count > 0:
                i += 1
                line = lines[i]
                decl_lines.append(line)
                brace_count += line.count("{") - line.count("}")
            output.extend(decl_lines)
            i += 1
            continue

        if "fn " in stripped:
            sig_lines = [line]
            has_brace = "{" in line
            while i + 1 < n and not has_brace and not stripped.endswith(";"):
                i += 1
                line = lines[i]
                stripped = line.strip()
                sig_lines.append(line)
                if "{" in line:
                    has_brace = True
                    break

            if has_brace:
                body_start = i
                brace_depth = "".join(sig_lines).count("{") - "".join(sig_lines).count("}")
                while i + 1 < n and brace_depth > 0:
                    i += 1
                    line = lines[i]
                    brace_depth += line.count("{") - line.count("}")

                body_lines_count = i - body_start + 1
                clean_sig = "\n".join(sig_lines)
                clean_sig = re.sub(r'\s*\{\s*$', '', clean_sig)
                output.append(f"{clean_sig}; // [BODY STRIPPED: {body_lines_count} lines]")
                i += 1
                continue
            else:
                output.extend(sig_lines)
                i += 1
                continue

        if not stripped:
            output.append("")
        i += 1

    return "\n".join(output), orig_lines, len(output)

def skeletonize_python(content: str) -> Tuple[str, int, int]:
    lines = content.splitlines()
    orig_lines = len(lines)
    output: List[str] = []
    i = 0
    n = len(lines)
    in_docstring = False

    while i < n:
        line = lines[i]
        stripped = line.strip()

        if '"""' in stripped or "'''" in stripped:
            output.append(line)
            delim = '"""' if '"""' in stripped else "'''"
            if stripped.count(delim) < 2:
                in_docstring = True
                while i + 1 < n and in_docstring:
                    i += 1
                    line = lines[i]
                    output.append(line)
                    if delim in line:
                        in_docstring = False
            i += 1
            continue

        if stripped.startswith("import ") or stripped.startswith("from "):
            output.append(line)
            i += 1
            continue

        if stripped.startswith("class "):
            output.append(line)
            i += 1
            continue

        if stripped.startswith("def ") or " def " in line:
            indent = len(line) - len(line.lstrip())
            sig_lines = [line]
            while i + 1 < n and not stripped.endswith(":"):
                i += 1
                line = lines[i]
                stripped = line.strip()
                sig_lines.append(line)

            output.extend(sig_lines)

            if i + 1 < n and ('"""' in lines[i + 1] or "'''" in lines[i + 1]):
                i += 1
                doc_line = lines[i]
                output.append(doc_line)
                delim = '"""' if '"""' in doc_line else "'''"
                if doc_line.count(delim) < 2:
                    while i + 1 < n:
                        i += 1
                        dline = lines[i]
                        output.append(dline)
                        if delim in dline:
                            break

            body_indent = " " * (indent + 4)
            output.append(f"{body_indent}...")

            while i + 1 < n:
                next_line = lines[i + 1]
                if not next_line.strip():
                    i += 1
                    continue
                next_indent = len(next_line) - len(next_line.lstrip())
                if next_indent > indent:
                    i += 1
                else:
                    break
            i += 1
            continue

        if not stripped:
            output.append("")
        i += 1

    return "\n".join(output), orig_lines, len(output)

def inspect_file(filepath: str) -> Optional[Tuple[str, int, int]]:
    if not os.path.isfile(filepath):
        return None
    try:
        with open(filepath, "r", encoding="utf-8", errors="replace") as f:
            content = f.read()
    except Exception:
        return None

    ext = os.path.splitext(filepath)[1].lower()
    if ext in [".c", ".h", ".cpp", ".hpp"]:
        return skeletonize_c(content)
    elif ext in [".rs"]:
        return skeletonize_rust(content)
    elif ext in [".py"]:
        return skeletonize_python(content)
    else:
        lines = content.splitlines()
        return content, len(lines), len(lines)

# ==============================================================================
# BLOCK 4: AUDIT PROCEDURES
# ==============================================================================
def self_audit() -> bool:
    c_sample = "int foo(int x) { return x * 2; }"
    skel, orig, new_l = skeletonize_c(c_sample)
    return "foo(int x);" in skel

# ==============================================================================
# BLOCK 5: CLOSING BLOCK
# ==============================================================================
if __name__ == "__main__":
    print(f"AST inspector self-audit: {'PASS' if self_audit() else 'FAIL'}")

