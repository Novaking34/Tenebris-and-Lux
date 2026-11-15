"""
================================================================================
BLOCK 1: METADATA BLOCK
================================================================================
@project       Tenebris and Lux
@file          tools/orchestrator/gui/chat_canvas.py
@id            omnicode.tools.gui.chat_canvas
@layer         Layer 4: Native Desktop Studio
@version       0.4.0-dev
@interface_rev Rev 5
@date          2026-11-13
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@system        OmniCode Orchestrator // Cognitive Chat & Streaming Canvas
@role          Dual-Stream Chat Canvas Isolating Internal Reasoning from Code
@invariants    1. Zero external pip dependencies (tkinter Text + Scrollbar).
               2. Rich color tagging for <think>, [TOOL], and [OBSERVATION].
================================================================================
"""

# ==============================================================================
# BLOCK 2: OPENING BLOCK
# ==============================================================================
import tkinter as tk
from tkinter import ttk
from typing import Callable, Optional

# ==============================================================================
# BLOCK 3: BODY BLOCK
# ==============================================================================
class ChatCanvas(ttk.Frame):
    def __init__(self, parent, on_submit_prompt: Callable[[str], None]):
        super().__init__(parent)
        self.on_submit_prompt = on_submit_prompt

        # Split pane: Chat History Top, Input Bottom
        self.text_display = tk.Text(
            self,
            wrap="word",
            bg="#181818",
            fg="#e0e0e0",
            insertbackground="#ffffff",
            font=("monospace", 10),
            padx=10,
            pady=10,
            state="disabled"
        )
        scrollbar = ttk.Scrollbar(self, orient="vertical", command=self.text_display.yview)
        self.text_display.configure(yscrollcommand=scrollbar.set)

        self.text_display.pack(side="top", fill="both", expand=True)
        scrollbar.pack(side="right", fill="y", before=self.text_display)

        # Configure style tags
        self.text_display.tag_config("operator_hdr", foreground="#ff00ff", font=("monospace", 10, "bold"))
        self.text_display.tag_config("agent_hdr", foreground="#00e5ff", font=("monospace", 10, "bold"))
        self.text_display.tag_config("thinking", foreground="#888888", font=("monospace", 9, "italic"))
        self.text_display.tag_config("tool_action", foreground="#ffaa00", font=("monospace", 9, "bold"))
        self.text_display.tag_config("tool_obs", foreground="#00ff88", font=("monospace", 9))
        self.text_display.tag_config("telemetry", foreground="#666666", font=("monospace", 8))

        # Bottom Input Area
        input_frame = ttk.Frame(self, padding=4)
        input_frame.pack(side="bottom", fill="x", pady=4)

        self.input_entry = tk.Text(
            input_frame,
            height=3,
            wrap="word",
            bg="#222222",
            fg="#ffffff",
            insertbackground="#ffffff",
            font=("monospace", 10),
            padx=6,
            pady=6
        )
        self.input_entry.pack(side="left", fill="both", expand=True, padx=(0, 6))
        self.input_entry.bind("<Return>", self._handle_enter)

        btn_send = ttk.Button(input_frame, text="Send 🚀\n(Enter)", command=self._submit)
        btn_send.pack(side="right", fill="y")

    def _handle_enter(self, event):
        if event.state & 0x0001:  # Shift + Enter: newline
            return None
        self._submit()
        return "break"

    def _submit(self):
        text = self.input_entry.get("1.0", "end-1c").strip()
        if not text:
            return
        self.input_entry.delete("1.0", "end")
        self.append_operator_message(text)
        self.on_submit_prompt(text)

    def append_operator_message(self, message: str):
        self.text_display.config(state="normal")
        self.text_display.insert("end", f"\n[OPERATOR] ❯❯❯\n", "operator_hdr")
        self.text_display.insert("end", f"{message}\n")
        self.text_display.see("end")
        self.text_display.config(state="disabled")

    def start_agent_response(self, model_name: str):
        self.text_display.config(state="normal")
        self.text_display.insert("end", f"\n[{model_name}] ❯❯❯\n", "agent_hdr")
        self.text_display.see("end")
        self.text_display.config(state="disabled")

    def append_chunk(self, token: str, is_thinking: bool):
        self.text_display.config(state="normal")
        tag = "thinking" if is_thinking else None
        if tag:
            self.text_display.insert("end", token, tag)
        else:
            self.text_display.insert("end", token)
        self.text_display.see("end")
        self.text_display.config(state="disabled")

    def append_tool_action(self, action_desc: str):
        self.text_display.config(state="normal")
        self.text_display.insert("end", f"\n⚡ {action_desc}\n", "tool_action")
        self.text_display.see("end")
        self.text_display.config(state="disabled")

    def append_tool_observation(self, obs: str):
        self.text_display.config(state="normal")
        self.text_display.insert("end", f"{obs}\n", "tool_obs")
        self.text_display.see("end")
        self.text_display.config(state="disabled")

    def append_telemetry(self, telem_str: str):
        self.text_display.config(state="normal")
        self.text_display.insert("end", f"── {telem_str} ──\n", "telemetry")
        self.text_display.see("end")
        self.text_display.config(state="disabled")

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
    chat = ChatCanvas(root, lambda p: print("Prompt:", p))
    chat.pack(fill="both", expand=True)
    root.mainloop()

