"""
================================================================================
BLOCK 1: METADATA BLOCK
================================================================================
@project       Tenebris and Lux
@file          tools/orchestrator/gui/memory_view.py
@id            omnicode.tools.gui.memory_view
@layer         Layer 4: Native Desktop Studio
@version       0.4.0-dev
@interface_rev Rev 5
@date          2026-11-13
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@system        OmniCode Orchestrator // Addressed Memory Table View
@role          Interactive Treeview Displaying Track 4 Episodic Nodes
@invariants    1. Zero external pip dependencies (tkinter ttk Treeview).
               2. Direct downward binding to EpisodicIndexStore.
================================================================================
"""

# ==============================================================================
# BLOCK 2: OPENING BLOCK
# ==============================================================================
import tkinter as tk
from tkinter import ttk, messagebox
from typing import Optional, Callable
from ..memory.index_store import EpisodicIndexStore

# ==============================================================================
# BLOCK 3: BODY BLOCK
# ==============================================================================
class MemoryViewPanel(ttk.LabelFrame):
    def __init__(self, parent, index_store: EpisodicIndexStore, on_select_node: Optional[Callable[[int], None]] = None):
        super().__init__(parent, text="Track 4 Addressed Index Memory (Episodic Store)", padding=8)
        self.index_store = index_store
        self.on_select_node = on_select_node

        cols = ("id", "time", "state", "topic", "symbols")
        self.tree = ttk.Treeview(self, columns=cols, show="headings", height=6)
        self.tree.heading("id", text="ID")
        self.tree.heading("time", text="Time")
        self.tree.heading("state", text="State")
        self.tree.heading("topic", text="Topic / Invariant")
        self.tree.heading("symbols", text="Symbols / Files")

        self.tree.column("id", width=45, anchor="center")
        self.tree.column("time", width=75, anchor="center")
        self.tree.column("state", width=85, anchor="center")
        self.tree.column("topic", width=280, anchor="w")
        self.tree.column("symbols", width=180, anchor="w")

        scrollbar = ttk.Scrollbar(self, orient="vertical", command=self.tree.yview)
        self.tree.configure(yscrollcommand=scrollbar.set)

        self.tree.pack(side="left", fill="both", expand=True)
        scrollbar.pack(side="right", fill="y")

        # Action Buttons
        btn_frame = ttk.Frame(self)
        btn_frame.pack(side="bottom", fill="x", pady=4)

        ttk.Button(btn_frame, text="📌 Pin Node", command=self._pin_selected).pack(side="left", padx=2)
        ttk.Button(btn_frame, text="🔓 Unpin Node", command=self._unpin_selected).pack(side="left", padx=2)
        ttk.Button(btn_frame, text="🗑️ Drop Node", command=self._drop_selected).pack(side="left", padx=2)
        ttk.Button(btn_frame, text="🔄 Refresh", command=self.refresh_table).pack(side="right", padx=2)

        self.tree.bind("<Double-1>", self._on_double_click)

    def refresh_table(self):
        for item in self.tree.get_children():
            self.tree.delete(item)

        for e in self.index_store.get_entries():
            state_str = "PINNED" if e.pinned else "INDEXED"
            kw = ", ".join(e.symbols[:3])
            if e.files:
                kw += f" @{os.path.basename(e.files[0])}"
            self.tree.insert("", "end", values=(f"#{e.idx}", e.timestamp, state_str, e.topic, kw))

    def _get_selected_id(self) -> Optional[int]:
        selected = self.tree.selection()
        if not selected:
            return None
        val = self.tree.item(selected[0], "values")[0]
        try:
            return int(val.replace("#", ""))
        except ValueError:
            return None

    def _pin_selected(self):
        idx = self._get_selected_id()
        if idx is not None:
            self.index_store.pin_entry(idx)
            self.refresh_table()

    def _unpin_selected(self):
        idx = self._get_selected_id()
        if idx is not None:
            self.index_store.unpin_entry(idx)
            self.refresh_table()

    def _drop_selected(self):
        idx = self._get_selected_id()
        if idx is not None:
            self.index_store.drop_entry(idx)
            self.refresh_table()

    def _on_double_click(self, event):
        idx = self._get_selected_id()
        if idx is not None and self.on_select_node:
            self.on_select_node(idx)

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
    store = EpisodicIndexStore()
    view = MemoryViewPanel(root, store)
    view.pack(fill="both", expand=True)
    root.mainloop()

