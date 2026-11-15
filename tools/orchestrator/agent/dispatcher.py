"""
================================================================================
BLOCK 1: METADATA BLOCK
================================================================================
@project       Tenebris and Lux
@file          tools/orchestrator/agent/dispatcher.py
@id            omnicode.tools.agent.dispatcher
@layer         Layer 3: Agent & Dispatcher Cogs
@version       0.4.0-dev
@interface_rev Rev 5
@date          2026-11-13
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@system        OmniCode Orchestrator // Exoskeleton Tool Dispatcher
@role          Parses, Authorizes and Executes Model-Emitted Tool Commands
@invariants    1. Operator-in-the-loop: Destructive tools require confirmation.
               2. Direct downward calls to Layer 2 exoskeleton cogs.
================================================================================
"""

# ==============================================================================
# BLOCK 2: OPENING BLOCK
# ==============================================================================
import os
import re
import subprocess
from typing import List, Dict, Any, Callable, Optional
from ..types.state import ToolCall
from ..exoskeleton.ast_inspector import inspect_file
from ..exoskeleton.invariant_verifier import check_file_invariants
from ..exoskeleton.compiler_tripwire import run_compiler_probe

# ==============================================================================
# BLOCK 3: BODY BLOCK
# ==============================================================================
class AgenticToolDispatcher:
    def __init__(self, confirm_callback: Optional[Callable[[str, str], bool]] = None):
        """
        confirm_callback: optional UI hook func(action_desc, prompt) -> bool.
        If None, falls back to terminal input().
        """
        self.confirm_callback = confirm_callback

    @staticmethod
    def extract_tools(text: str) -> List[ToolCall]:
        tools: List[ToolCall] = []

        # 1. [TOOL:INSPECT <path>]
        for p in re.findall(r'\[(?:TOOL:)?INSPECT:\s*(.+?)\]', text, re.IGNORECASE):
            tools.append(ToolCall(tool_type="inspect", args={"path": p.strip()}, raw_tag=f"[TOOL:INSPECT {p.strip()}]"))

        # 2. [TOOL:VERIFY <path>]
        for p in re.findall(r'\[(?:TOOL:)?VERIFY:\s*(.+?)\]', text, re.IGNORECASE):
            tools.append(ToolCall(tool_type="verify", args={"path": p.strip()}, raw_tag=f"[TOOL:VERIFY {p.strip()}]"))

        # 3. [TOOL:RUN <cmd>]
        for cmd in re.findall(r'\[(?:TOOL:)?RUN:\s*(.+?)\]', text, re.IGNORECASE):
            tools.append(ToolCall(tool_type="run", args={"command": cmd.strip()}, raw_tag=f"[TOOL:RUN {cmd.strip()}]"))

        # 4. [TOOL:READ <path>]
        for p in re.findall(r'\[(?:TOOL:)?READ:\s*(.+?)\]', text, re.IGNORECASE):
            tools.append(ToolCall(tool_type="read", args={"path": p.strip()}, raw_tag=f"[TOOL:READ {p.strip()}]"))

        # 5. [TOOL:WRITE <path>]\n<content>\n[TOOL:END]
        write_matches = re.findall(r'\[(?:TOOL:)?WRITE:\s*(.+?)\]\n([\s\S]*?)\[(?:TOOL:)?END\]', text, re.IGNORECASE)
        for p, content in write_matches:
            tools.append(ToolCall(tool_type="write", args={"path": p.strip(), "content": content}, raw_tag=f"[TOOL:WRITE {p.strip()}]"))

        # 6. [TOOL:DIFF]
        if re.search(r'\[(?:TOOL:)?DIFF\]', text, re.IGNORECASE):
            tools.append(ToolCall(tool_type="diff", args={}, raw_tag="[TOOL:DIFF]"))

        # 7. [TOOL:BUILD]
        if re.search(r'\[(?:TOOL:)?BUILD\]', text, re.IGNORECASE):
            tools.append(ToolCall(tool_type="build", args={}, raw_tag="[TOOL:BUILD]"))

        return tools

    def _request_permission(self, action_type: str, detail: str) -> bool:
        if self.confirm_callback:
            return self.confirm_callback(action_type, detail)
        # Terminal fallback
        try:
            print(f"\n┌─ [ACTION CONFIRMATION: {action_type}] ───────────────────┐")
            print(f"│ Detail: {detail}")
            print(f"└────────────────────────────────────────────────────────┘")
            ans = input("Authorize execution? [Y/n]: ").strip().lower()
            return ans in ("", "y", "yes")
        except Exception:
            return False

    def execute_tool(self, tool: ToolCall) -> str:
        t_type = tool.tool_type

        if t_type == "inspect":
            path = tool.args["path"]
            res = inspect_file(path)
            if res:
                skeleton, orig_l, new_l = res
                return f"[OBSERVATION: AST Skeleton for {path} ({orig_l} -> {new_l} lines):\n{skeleton}\n--- END SKELETON ---]"
            return f"[OBSERVATION: Could not inspect file '{path}'. File not found or unreadable.]"

        elif t_type == "verify":
            path = tool.args["path"]
            violations = check_file_invariants(path)
            if not violations:
                return f"[OBSERVATION: PASS. File '{path}' satisfies all bedrock invariants (5-Block, Zero Heap, Invariant 18).]"
            v_msgs = [f"• Line {v['line']}: [{v['rule']}] {v['msg']}" for v in violations]
            return f"[OBSERVATION: FAIL. File '{path}' has {len(violations)} violation(s):\n" + "\n".join(v_msgs) + "]"

        elif t_type == "build":
            ok, msg = run_compiler_probe()
            status = "PASS" if ok else "FAIL"
            return f"[OBSERVATION: Compiler Build Probe [{status}]:\n{msg}]"

        elif t_type == "read":
            path = os.path.abspath(os.path.expanduser(tool.args["path"]))
            if os.path.isfile(path):
                try:
                    with open(path, "r", encoding="utf-8", errors="replace") as f:
                        data = f.read()
                    return f"[OBSERVATION: Content of {tool.args['path']} ({len(data)} bytes):\n{data}\n--- END OF FILE ---]"
                except Exception as e:
                    return f"[OBSERVATION: Error reading {tool.args['path']}: {e}]"
            return f"[OBSERVATION: File '{tool.args['path']}' does not exist.]"

        elif t_type == "write":
            path = os.path.abspath(os.path.expanduser(tool.args["path"]))
            content = tool.args["content"]
            if not self._request_permission("WRITE FILE", f"{tool.args['path']} ({len(content)} bytes)"):
                return f"[OBSERVATION: Write to '{tool.args['path']}' was denied by operator.]"
            try:
                os.makedirs(os.path.dirname(path), exist_ok=True)
                with open(path, "w", encoding="utf-8") as f:
                    f.write(content)
                return f"[OBSERVATION: Successfully wrote file '{tool.args['path']}' ({len(content)} bytes).]"
            except Exception as e:
                return f"[OBSERVATION: Failed writing file '{tool.args['path']}': {e}]"

        elif t_type == "run":
            cmd = tool.args["command"]
            if not self._request_permission("SHELL COMMAND", cmd):
                return f"[OBSERVATION: Command '{cmd}' was denied by operator.]"
            try:
                proc = subprocess.run(cmd, shell=True, capture_output=True, text=True, timeout=60)
                out = proc.stdout + proc.stderr
                return f"[OBSERVATION: Command '{cmd}' finished with exit code {proc.returncode}:\n{out[:2000]}]"
            except Exception as e:
                return f"[OBSERVATION: Command '{cmd}' failed with error: {e}]"

        elif t_type == "diff":
            try:
                proc = subprocess.run("git diff", shell=True, capture_output=True, text=True, timeout=10)
                return f"[OBSERVATION: Current git diff:\n{proc.stdout}]"
            except Exception as e:
                return f"[OBSERVATION: Failed getting git diff: {e}]"

        return "[OBSERVATION: Unknown tool command]"

# ==============================================================================
# BLOCK 4: AUDIT PROCEDURES
# ==============================================================================
def self_audit() -> bool:
    tools = AgenticToolDispatcher.extract_tools("Let me check: [TOOL:INSPECT foo.c] and [TOOL:DIFF]")
    return len(tools) == 2 and tools[0].tool_type == "inspect" and tools[1].tool_type == "diff"

# ==============================================================================
# BLOCK 5: CLOSING BLOCK
# ==============================================================================
if __name__ == "__main__":
    print(f"Dispatcher audit: {'PASS' if self_audit() else 'FAIL'}")

