"""
================================================================================
BLOCK 1: METADATA BLOCK
================================================================================
@project       Tenebris and Lux
@file          tools/orchestrator/gui/tool_cockpit.py
@id            omnicode.tools.gui.tool_cockpit
@layer         Layer 4: Native Desktop Studio
@version       0.4.0-dev
@interface_rev Rev 5
@date          2026-11-13
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@system        OmniCode Orchestrator // Exoskeleton Action Toolbar
@role          Provides One-Click Developer Cockpit Actions (Inspect, Verify, Build, Calibrate)
@invariants    1. Zero external pip dependencies (tkinter ttk + filedialog).
               2. Direct downward binding to Layer 2 exoskeleton and calibration cogs.
================================================================================
"""

# ==============================================================================
# BLOCK 2: OPENING BLOCK
# ==============================================================================
import os
import subprocess
import tkinter as tk
from tkinter import ttk, filedialog, messagebox
from typing import Callable, Optional
from ..exoskeleton.ast_inspector import inspect_file
from ..exoskeleton.invariant_verifier import check_file_invariants
from ..exoskeleton.compiler_tripwire import run_compiler_probe
from ..calibration.model_factory import build_and_register_1_5b_agent

# ==============================================================================
# BLOCK 3: BODY BLOCK
# ==============================================================================
class ToolCockpit(ttk.Frame):
    def __init__(
        self,
        parent,
        active_model_var: tk.StringVar,
        on_action_output: Callable[[str], None],
        on_model_changed: Optional[Callable[[str], None]] = None
    ):
        super().__init__(parent, padding=4)
        self.active_model_var = active_model_var
        self.on_action_output = on_action_output
        self.on_model_changed = on_model_changed

        # Model Selector
        ttk.Label(self, text="Model:", font=("monospace", 9, "bold")).pack(side="left", padx=(4, 2))
        models = [
            "qwen2.5-coder:0.5b",
            "omnicode-fast-1.5b",
            "omnicode-polymath",
            "omnicode-architect-7b"
        ]
        self.combo_model = ttk.Combobox(self, textvariable=self.active_model_var, values=models, width=22, state="readonly")
        self.combo_model.pack(side="left", padx=4)
        self.combo_model.bind("<<ComboboxSelected>>", self._on_combo_select)

        # Quick Action Buttons
        ttk.Button(self, text="🧩 Decompose (0.5B)", command=self._action_decompose).pack(side="left", padx=3)
        ttk.Button(self, text="🔍 Inspect File AST", command=self._action_inspect).pack(side="left", padx=3)
        ttk.Button(self, text="🛡️ Verify Invariants", command=self._action_verify).pack(side="left", padx=3)
        ttk.Button(self, text="⚙️ Build Engine", command=self._action_build).pack(side="left", padx=3)
        ttk.Button(self, text="⚡ Calibrate", command=self._action_calibrate).pack(side="left", padx=3)
        ttk.Button(self, text="🎮 Launch Game", command=self._action_launch_game).pack(side="right", padx=4)

    def _action_decompose(self):
        from ..agent.decomposer import HeadlessTaskDecomposer
        import json
        import tkinter.simpledialog as simpledialog
        prompt = simpledialog.askstring("0.5B Micro-Scout Decomposer", "Enter objective or complex task to slice:")
        if not prompt:
            return
        decomposer = HeadlessTaskDecomposer()
        dag = decomposer.decompose(prompt)
        formatted = f"=== 0.5B MICRO-SCOUT TASK DECOMPOSITION ===\nObjective: {prompt}\n\n" + json.dumps(dag, indent=2) + "\n=== END DAG ==="
        self.on_action_output(formatted)

    def _on_combo_select(self, event):
        model = self.active_model_var.get()
        if self.on_model_changed:
            self.on_model_changed(model)

    def _action_inspect(self):
        target = filedialog.askopenfilename(
            title="Select Source File to Skeletonize",
            filetypes=[("C/Rust/Python Sources", "*.c *.h *.rs *.py"), ("All Files", "*.*")]
        )
        if not target:
            return
        res = inspect_file(target)
        if res:
            skeleton, orig_l, new_l = res
            savings = (1.0 - (new_l / orig_l)) * 100.0 if orig_l > 0 else 0.0
            msg = f"=== AST SKELETON: {os.path.basename(target)} ===\nLines: {orig_l} -> {new_l} ({savings:.1f}% token reduction)\n\n{skeleton}\n=== END SKELETON ==="
            self.on_action_output(msg)
        else:
            messagebox.showerror("Inspect Error", f"Could not inspect '{target}'.")

    def _action_verify(self):
        target = filedialog.askopenfilename(
            title="Select File to Verify Against Bedrock Invariants",
            filetypes=[("C Source / Headers", "*.c *.h"), ("All Files", "*.*")]
        )
        if not target:
            return
        violations = check_file_invariants(target)
        if not violations:
            msg = f"[VERIFY PASS] '{os.path.basename(target)}' satisfies all bedrock invariants (5-Block, Zero Heap, Invariant 18)."
            messagebox.showinfo("Invariant Verification", msg)
            self.on_action_output(msg)
        else:
            lines = [f"• Line {v['line']}: [{v['rule']}] {v['msg']}" for v in violations]
            msg = f"[VERIFY FAILED] '{os.path.basename(target)}' has {len(violations)} violation(s):\n" + "\n".join(lines)
            messagebox.showwarning("Invariant Violations", msg)
            self.on_action_output(msg)

    def _action_build(self):
        ok, msg = run_compiler_probe()
        if ok:
            messagebox.showinfo("Compiler Probe", msg)
        else:
            messagebox.showwarning("Compiler Probe Failure", msg)
        self.on_action_output(f"=== COMPILER PROBE ===\n{msg}")

    def _action_calibrate(self):
        ok, msg = build_and_register_1_5b_agent()
        if ok:
            messagebox.showinfo("1.5B Calibration Complete", msg)
        else:
            messagebox.showerror("Calibration Failed", msg)
        self.on_action_output(f"=== MODEL FACTORY CALIBRATION ===\n{msg}")

    def _action_launch_game(self):
        canonical_bin = os.path.abspath("build/bin/tenebris-lux")
        symlink_bin = os.path.abspath("build/tenebris-lux")
        game_bin = canonical_bin if os.path.isfile(canonical_bin) else symlink_bin
        if os.path.isfile(game_bin):
            try:
                subprocess.Popen([game_bin])
                self.on_action_output(f"[GAME] Launched {game_bin} on X11 display.")
            except Exception as e:
                messagebox.showerror("Launch Failed", f"Failed starting game binary: {e}")
        else:
            messagebox.showwarning("Game Not Built", "Binary 'build/bin/tenebris-lux' or 'build/tenebris-lux' not found. Please click 'Build Engine' first.")

# ==============================================================================
# BLOCK 4: AUDIT PROCEDURES
# ==============================================================================
def self_audit() -> bool:
    return True

# ==============================================================================
# BLOCK 5: CLOSING BLOCK
# ==============================================================================
if __name__ == "__main__":
    root = tk.Tk()
    var = tk.StringVar(value="omnicode-fast-1.5b")
    cockpit = ToolCockpit(root, var, print)
    cockpit.pack(fill="x")
    root.mainloop()

