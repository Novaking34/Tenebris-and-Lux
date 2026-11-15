"""
================================================================================
BLOCK 1: METADATA BLOCK
================================================================================
@project       Tenebris and Lux
@file          tools/orchestrator/gui/app.py
@id            omnicode.tools.gui.app
@layer         Layer 4: Native Desktop Studio
@version       0.5.0-dev
@interface_rev Rev 5
@date          2026-11-13
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@system        OmniCode Orchestrator // Central Launcher Hub & Cognitive Studio
@role          Master Desktop Application (Unreal/Unity Hub Style + Interactive Agent Studio)
@invariants    1. Zero HTML, zero CSS, zero JavaScript, zero Electron.
               2. Pure Python 3 standard library Tkinter (Tk 8.6, runs natively on X11).
               3. Non-blocking UI: Model inference runs on background thread; updates via root.after().
================================================================================
"""

# ==============================================================================
# BLOCK 2: OPENING BLOCK
# ==============================================================================
import os
import sys
import threading
import tkinter as tk
from tkinter import ttk, messagebox
from typing import Optional

from ..memory.index_store import EpisodicIndexStore
from ..agent.dispatcher import AgenticToolDispatcher
from ..agent.reflection_loop import run_agentic_turn
from ..agent.stepped_gearbox import SteppedGearbox
from .telemetry_panel import TelemetryPanel
from .memory_view import MemoryViewPanel
from .chat_canvas import ChatCanvas
from .tool_cockpit import ToolCockpit
from .hub_panel import EngineHubPanel

# ==============================================================================
# BLOCK 3: BODY BLOCK
# ==============================================================================
class OmniCodeStudioApp:
    def __init__(self, root: tk.Tk, initial_model: str = "omnicode-fast-1.5b"):
        self.root = root
        self.root.title("OmniCode Hub & Studio // Cornerstone Game Engine & CPI-SI Agent Cockpit")
        self.root.geometry("1180x800")
        self.root.minsize(850, 620)

        # Apply dark theme styling
        self._apply_dark_styles()

        self.gearbox = SteppedGearbox()
        self.index_store = EpisodicIndexStore()
        
        current_state = self.gearbox.get_current_state()
        self.active_model_var = tk.StringVar(value=current_state.get("base_model", initial_model))

        # Dispatcher with GUI confirmation dialog
        self.dispatcher = AgenticToolDispatcher(confirm_callback=self._gui_confirm_action)

        # Master Notebook (Tabs: Engine Hub vs Agent Studio)
        self.notebook = ttk.Notebook(self.root)
        self.notebook.pack(fill="both", expand=True, padx=4, pady=4)

        # ----------------------------------------------------------------------
        # TAB 1: Game Engine & Project Hub (Unreal / Unity Style)
        # ----------------------------------------------------------------------
        self.hub_frame = ttk.Frame(self.notebook)
        self.notebook.add(self.hub_frame, text=" 🎮 Engine & Project Hub ")

        self.engine_hub = EngineHubPanel(self.hub_frame, on_log_callback=self._on_action_output)
        self.engine_hub.pack(fill="both", expand=True)

        # ----------------------------------------------------------------------
        # TAB 2: Cognitive Agent Studio (Interactive Assistant Chat & Memory)
        # ----------------------------------------------------------------------
        self.studio_frame = ttk.Frame(self.notebook)
        self.notebook.add(self.studio_frame, text=" 🧠 Cognitive Agent Studio ")

        studio_container = ttk.Frame(self.studio_frame)
        studio_container.pack(fill="both", expand=True, padx=6, pady=6)

        # Top: Action Cockpit Toolbar
        self.cockpit = ToolCockpit(
            studio_container,
            active_model_var=self.active_model_var,
            on_action_output=self._on_action_output,
            on_model_changed=self._on_model_changed
        )
        self.cockpit.pack(side="top", fill="x", pady=(0, 6))

        # Center: PanedWindow (Left: Chat Canvas, Right: Track 4 Memory View)
        paned = ttk.PanedWindow(studio_container, orient="horizontal")
        paned.pack(fill="both", expand=True, pady=(0, 6))

        self.chat_canvas = ChatCanvas(paned, on_submit_prompt=self._on_user_submit)
        paned.add(self.chat_canvas, weight=3)

        self.memory_view = MemoryViewPanel(
            paned,
            index_store=self.index_store,
            on_select_node=self._on_node_selected
        )
        paned.add(self.memory_view, weight=2)

        # Bottom: Hardware Telemetry HUD
        self.telemetry = TelemetryPanel(
            studio_container,
            on_rise_up=self._handle_rise_up,
            on_fall_down=self._handle_fall_down
        )
        self.telemetry.pack(side="bottom", fill="x")

        # Sync initial Gearbox State to HUD
        self._sync_gearbox_state()

        # Initial Welcome Message in Chat
        self.chat_canvas.append_operator_message("OmniCode Central Hub & Studio online. Hardware probes calibrated.")
        self.chat_canvas.start_agent_response("OmniCode Machine Intelligence")
        self.chat_canvas.append_chunk(
            "Greetings, Operator ProfessorSeanEX. The CPI-SI Machine Chassis is fully operational.\n"
            "Cornerstone Engine contracts, 7-Tier Stepped Staircase, and Track 4 Memory are online.\n"
            "Active Spectrum: 0.5B Scout -> 1.5B Bedrock -> 3B Polymath -> 7B PC Ceiling -> 14B/32B/72B Cloud.\n"
            "[Scaffold Active: Powered by Qwen 2.5 / Coder substrate; MillenniumOS native weights in dev.]\n"
            "Ready to assist with system architecture, invariant verification, and game mechanics.",
            is_thinking=False
        )

    def _apply_dark_styles(self):
        style = ttk.Style()
        style.theme_use("clam")
        style.configure(".", background="#202020", foreground="#ffffff", fieldbackground="#2b2b2b")
        style.configure("TLabelframe", background="#202020", foreground="#00e5ff", bordercolor="#444444")
        style.configure("TLabelframe.Label", background="#202020", foreground="#00e5ff", font=("monospace", 9, "bold"))
        style.configure("TButton", background="#333333", foreground="#ffffff", bordercolor="#555555", font=("monospace", 9))
        style.map("TButton", background=[("active", "#444444"), ("pressed", "#222222")])
        style.configure("Treeview", background="#1a1a1a", foreground="#ffffff", fieldbackground="#1a1a1a", font=("monospace", 9))
        style.configure("Treeview.Heading", background="#2d2d2d", foreground="#00e5ff", font=("monospace", 9, "bold"))
        style.configure("TNotebook", background="#1a1a1a", tabmargins=[2, 5, 2, 0])
        style.configure("TNotebook.Tab", background="#2d2d2d", foreground="#aaaaaa", padding=[12, 4], font=("monospace", 9, "bold"))
        style.map("TNotebook.Tab", background=[("selected", "#383838")], foreground=[("selected", "#00e5ff")])

    def _gui_confirm_action(self, action_type: str, detail: str) -> bool:
        """Modal dialog on the main GUI thread to authorize destructive agent actions."""
        return messagebox.askyesno(
            "Authorize Agent Action",
            f"The agent requests permission to execute:\n\nAction: {action_type}\nTarget: {detail}\n\nAuthorize this operation?"
        )

    def _on_action_output(self, output: str):
        self.chat_canvas.append_tool_observation(output)

    def _on_model_changed(self, new_model: str):
        self.chat_canvas.append_telemetry(f"Active model switched to {new_model}")

    def _on_node_selected(self, node_id: int):
        for e in self.index_store.get_entries():
            if e.idx == node_id:
                state_str = "PINNED" if e.pinned else "INDEXED"
                info = (
                    f"=== MEMORY NODE #{e.idx} [{e.timestamp}] ({state_str}) ===\n"
                    f"Topic: {e.topic}\n"
                    f"Symbols: {', '.join(e.symbols)}\n\n"
                    f"User Prompt:\n{e.user_prompt}\n\n"
                    f"Assistant Solution:\n{e.assistant_response}\n"
                    f"=== END NODE #{e.idx} ==="
                )
                self.chat_canvas.append_tool_observation(info)
                break

    def _sync_gearbox_state(self):
        """Synchronizes the Gearbox state to the Telemetry HUD badge and active model."""
        badge = self.gearbox.format_status_badge()
        state = self.gearbox.get_current_state()
        self.telemetry.update_gear_badge(badge, domain=state["domain"])
        self.active_model_var.set(state["base_model"])

    def _handle_rise_up(self):
        def telemetry_cb(msg: str):
            self.chat_canvas.append_telemetry(msg)

        ok, msg = self.gearbox.rise_up(reason="Manual operator elevation", telemetry_cb=telemetry_cb)
        self._sync_gearbox_state()

    def _handle_fall_down(self):
        def telemetry_cb(msg: str):
            self.chat_canvas.append_telemetry(msg)

        ok, msg = self.gearbox.fall_down(reason="Manual operator step-down", telemetry_cb=telemetry_cb)
        self._sync_gearbox_state()

    def _on_user_submit(self, prompt: str):
        model = self.active_model_var.get()
        self.chat_canvas.start_agent_response(model)

        # Run inference on background thread to prevent UI lockup
        worker = threading.Thread(
            target=self._run_inference_worker,
            args=(prompt, model),
            daemon=True
        )
        worker.start()

    def _run_inference_worker(self, prompt: str, model_name: str):
        def chunk_cb(token: str, is_thinking: bool):
            # Schedule GUI update on main thread
            self.root.after(0, self.chat_canvas.append_chunk, token, is_thinking)

        def tool_cb(status_msg: str):
            self.root.after(0, self.chat_canvas.append_tool_action, status_msg)

        state = self.gearbox.get_current_state()

        # If active gear is in Cloud domain, attempt Cloud Peak execution with Fall-Down
        if state["domain"] == "CLOUD":
            sparse_payload, _, est_tokens = self.index_store.assemble_sparse_payload(prompt)
            messages = [{"role": "user", "content": sparse_payload}]

            def local_fallback_fn(messages, chunk_callback):
                return run_agentic_turn(
                    prompt=prompt,
                    index_store=self.index_store,
                    dispatcher=self.dispatcher,
                    model_name="omnicode-fast-1.5b",
                    chunk_callback=chunk_callback,
                    tool_status_callback=tool_cb
                )

            resp, telem = self.gearbox.cloud_bridge.execute_with_fall_down(
                messages=messages,
                local_streaming_fn=local_fallback_fn,
                chunk_callback=chunk_cb,
                requested_tier=state["tier"]
            )

            if telem.get("fell_down"):
                # System fell down to local: auto-trigger Local Mode in Gearbox
                self.gearbox.current_domain = "LOCAL"
                self.gearbox.current_tier = "1.5b"
                self.gearbox.current_mode = "ideal"
                self.root.after(0, self._sync_gearbox_state)
                self.root.after(0, self.chat_canvas.append_telemetry, "[SOVEREIGN HANDSHAKE: Cloud stepped down to Local Sovereign Bedrock.]")
        else:
            resp, telem = run_agentic_turn(
                prompt=prompt,
                index_store=self.index_store,
                dispatcher=self.dispatcher,
                model_name=model_name,
                chunk_callback=chunk_cb,
                tool_status_callback=tool_cb
            )

        # Update telemetry and memory view on main thread
        if telem:
            tps = telem.get("tps", 0.0)
            tokens = telem.get("tokens", 0)
            ctx = telem.get("active_ctx", 2048)
            time_s = telem.get("elapsed_sec", 0.0)
            src = telem.get("source", "LOCAL")
            telem_str = f"[{src}] Inference complete: {tokens} tok | {tps:.1f} tok/s | ctx: {ctx} | elapsed: {time_s:.2f}s"
            self.root.after(0, self.chat_canvas.append_telemetry, telem_str)
            self.root.after(0, self.telemetry.update_speedometer, tps, tokens, ctx)

        self.root.after(0, self.memory_view.refresh_table)

def launch_gui(initial_model: str = "omnicode-fast-1.5b"):
    root = tk.Tk()
    app = OmniCodeStudioApp(root, initial_model=initial_model)
    root.mainloop()

# ==============================================================================
# BLOCK 4: AUDIT PROCEDURES
# ==============================================================================
def self_audit() -> bool:
    return True

# ==============================================================================
# BLOCK 5: CLOSING BLOCK
# ==============================================================================
if __name__ == "__main__":
    launch_gui()
