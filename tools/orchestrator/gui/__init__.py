"""
================================================================================
BLOCK 1: METADATA BLOCK
================================================================================
@project       Tenebris and Lux
@file          tools/orchestrator/gui/__init__.py
@id            omnicode.tools.gui
@layer         Layer 4: Native Desktop Studio
@version       0.4.0-dev
@interface_rev Rev 5
@date          2026-11-13
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@system        OmniCode Orchestrator // Native Desktop Studio Cockpit
@role          Exports Tkinter Desktop Studio Application Window
@invariants    1. Zero HTML, zero CSS, zero JavaScript, zero Electron.
               2. Pure Python 3 standard library Tkinter (Tk 8.6, runs natively on X11 / DISPLAY :0).
================================================================================
"""

from .app import OmniCodeStudioApp, launch_gui

__all__ = [
    "OmniCodeStudioApp",
    "launch_gui"
]

