"""
================================================================================
BLOCK 1: METADATA BLOCK
================================================================================
@project       Tenebris and Lux
@file          tools/orchestrator/main.py
@id            omnicode.tools.orchestrator.main
@layer         Layer 4: Execution Entrypoint
@version       0.5.0-dev
@interface_rev Rev 5
@date          2026-11-13
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@system        OmniCode Orchestrator // Unified CLI, Native Cockpit & Service Launcher
@role          Dispatches CLI REPL, Native Tkinter GUI Studio, Service Bridge or Hardware Scan
@invariants    1. Universal 5-Block Lifecycle Protocol enforced.
               2. Zero external pip dependencies.
               3. Supports dual topologies: Native Desktop Cockpit and Wire-Compatible Service Bridge.
               4. Full spectrum navigation: 0.5B (Decomposer) -> 1.5B (Bedrock) -> 3B -> 7B (PC Ceiling).
================================================================================
"""

# ==============================================================================
# BLOCK 2: OPENING BLOCK
# ==============================================================================
import os
import sys
import json
import readline
from typing import List

from .types.config_schema import load_models_config
from .calibration.model_factory import build_and_register_1_5b_agent
from .calibration.sweet_spot import scan_full_spectrum, calculate_hardware_sweet_spot
from .exoskeleton.ast_inspector import inspect_file
from .exoskeleton.invariant_verifier import check_file_invariants
from .exoskeleton.compiler_tripwire import run_compiler_probe
from .memory.index_store import EpisodicIndexStore
from .agent.dispatcher import AgenticToolDispatcher
from .agent.reflection_loop import run_agentic_turn
from .agent.decomposer import HeadlessTaskDecomposer
from .agent.stepped_gearbox import SteppedGearbox
from .server.service_adapter import launch_service_adapter

HISTORY_FILE = os.path.expanduser("~/.omnicode_history")

# ==============================================================================
# BLOCK 3: BODY BLOCK
# ==============================================================================
def run_terminal_repl(model_name: str = "omnicode-fast-1.5b"):
    print("\033[35m\033[1m╔════════════════════════════════════════════════════════════════════════════╗\033[0m")
    print("\033[35m\033[1m║   OMNICODE ORCHESTRATOR // CPI-SI TERMINAL REPL (tools/orchestrator)       ║\033[0m")
    print("\033[35m\033[1m╚════════════════════════════════════════════════════════════════════════════╝\033[0m")
    gearbox = SteppedGearbox()
    index_store = EpisodicIndexStore()
    dispatcher = AgenticToolDispatcher()
    decomposer = HeadlessTaskDecomposer()
    
    # Sync initial gear model
    state = gearbox.get_current_state()
    current_model = state.get("base_model", model_name)

    print(f" Active Gear:  \033[36m\033[1m{gearbox.format_status_badge()}\033[0m")
    print(f" Commands:     \033[1mgear\033[0m │ \033[1mrise\033[0m │ \033[1mfall\033[0m │ \033[1mstaircase\033[0m │ \033[1mscan\033[0m │ \033[1mdecompose\033[0m │ \033[1mgui\033[0m\n")

    while True:
        try:
            user_input = input(f"\033[35m\033[1momni [{current_model}]> \033[0m").strip()
        except (EOFError, KeyboardInterrupt):
            print("\nExiting OmniCode session.")
            break

        if not user_input:
            continue

        cmd = user_input.lower()
        if cmd in ("exit", "quit", ":q"):
            break
        elif cmd in ("clear", "cls"):
            os.system("clear")
            continue
        elif cmd == "gear":
            print(f"\nActive Gearbox State: {gearbox.format_status_badge()}\n")
            continue
        elif cmd == "rise":
            ok, msg = gearbox.rise_up(reason="Manual CLI elevation", telemetry_cb=print)
            state = gearbox.get_current_state()
            current_model = state["base_model"]
            print(f"Active Gear: {gearbox.format_status_badge()}\n")
            continue
        elif cmd == "fall":
            ok, msg = gearbox.fall_down(reason="Manual CLI step-down", telemetry_cb=print)
            state = gearbox.get_current_state()
            current_model = state["base_model"]
            print(f"Active Gear: {gearbox.format_status_badge()}\n")
            continue
        elif cmd == "staircase":
            print("\n=== OMNICODE 7-TIER COGNITIVE STAIRCASE ===")
            for t in gearbox.TIER_ORDER:
                t_data = gearbox.tiers.get(t, {})
                loc = "LOCAL" if t_data.get("local_supported") else "CLOUD"
                active = " [ACTIVE]" if t == gearbox.current_tier else ""
                print(f"  • Tier {t.upper():<5} ({loc:<5}): {t_data.get('name', '')}{active}")
            print(f"Sovereign Boundary: <= {gearbox.SOVEREIGN_LOCAL_CEILING} (Auto-triggers Local Mode)\n")
            continue
        elif cmd == "index":
            print(f"\n{index_store.format_index_table()}\n")
            continue
        elif cmd == "scan":
            spectrum = scan_full_spectrum()
            print("\n=== EMPIRICAL ZERO-ASSUMPTION PC SCAN ===")
            for tier, cfg in spectrum.items():
                ceil = " [LOCAL CEILING]" if cfg["is_local_ceiling"] else ""
                print(f"Tier {tier.upper()}{ceil}: GPU={cfg['recommended_gpu_layers']}/{cfg['total_layers']} | CPU={cfg['cpu_layers']} | Ctx={cfg['min_ctx']}-{cfg['max_ctx']} (KV: ~{cfg['kv_cache_est_mb']} MB)")
            print()
            continue
        elif cmd.startswith("decompose "):
            query = user_input[10:].strip()
            print(f"\n[0.5B Headless Decomposer] Slicing task...")
            dag = decomposer.decompose(query)
            print(json.dumps(dag, indent=2) + "\n")
            continue
        elif cmd == "gui":
            print("Launching Native Desktop Studio GUI...")
            from .gui.app import launch_gui
            launch_gui(initial_model=current_model)
            continue
        elif cmd in ("serve", "service"):
            print("Starting OmniCode Service Adapter on port 11435...")
            server = launch_service_adapter(11435)
            try:
                server.serve_forever()
            except KeyboardInterrupt:
                server.server_close()
            continue
        elif cmd.startswith("inspect "):
            target = user_input[8:].strip()
            res = inspect_file(target)
            if res:
                skel, o_l, n_l = res
                pct = (1.0 - (n_l / o_l)) * 100.0 if o_l > 0 else 0.0
                print(f"\n=== AST SKELETON: {target} ({o_l} -> {n_l} lines, {pct:.1f}% savings) ===\n{skel}\n")
            else:
                print(f"Error: Could not inspect '{target}'.\n")
            continue
        elif cmd.startswith("verify"):
            flags = user_input[6:].strip()
            if "--build" in flags or flags == "build":
                ok, msg = run_compiler_probe()
                print(f"\nCompiler Probe: {'PASS' if ok else 'FAIL'}\n{msg}\n")
            else:
                target = flags.strip() or "engine/src/core/math_utils.c"
                violations = check_file_invariants(target)
                if not violations:
                    print(f"\n[PASS] '{target}' satisfies all bedrock invariants.\n")
                else:
                    print(f"\n[FAIL] '{target}' has {len(violations)} violation(s):")
                    for v in violations:
                        print(f"  • Line {v['line']}: [{v['rule']}] {v['msg']}")
                    print()
            continue
        elif cmd == "calibrate":
            ok, msg = build_and_register_1_5b_agent()
            print(f"\n{msg}\n")
            continue
        elif cmd.startswith("model "):
            m_target = user_input[6:].strip().lower()
            if m_target in ("0.5", "0.5b", "sifter", "micro"):
                current_model = "qwen2.5-coder:0.5b"
            elif m_target in ("fast", "1.5b", "1.5", "bedrock"):
                current_model = "omnicode-fast-1.5b"
            elif m_target in ("polymath", "3b", "3"):
                current_model = "omnicode-polymath"
            elif m_target in ("architect", "7b", "7", "ceiling"):
                current_model = "omnicode-architect-7b"
            else:
                current_model = m_target
            print(f"Switched model to: {current_model}\n")
            continue

        # Run multi-turn agentic turn
        def chunk_cb(token: str, is_thinking: bool):
            if is_thinking:
                sys.stdout.write(f"\033[90m\033[3m{token}\033[0m")
            else:
                sys.stdout.write(token)
            sys.stdout.flush()

        def status_cb(msg: str):
            print(f"\n\033[33m[*] {msg}\033[0m")

        print()
        resp, telem = run_agentic_turn(
            prompt=user_input,
            index_store=index_store,
            dispatcher=dispatcher,
            model_name=current_model,
            chunk_callback=chunk_cb,
            tool_status_callback=status_cb
        )
        if telem:
            tps = telem.get("tps", 0.0)
            toks = telem.get("tokens", 0)
            ctx = telem.get("active_ctx", 2048)
            el = telem.get("elapsed_sec", 0.0)
            print(f"\n\n\033[90m── [ Telemetry: {toks} tok | {tps:.1f} tok/s | active ctx: {ctx} | elapsed: {el:.2f}s ] ──\033[0m\n")

    try:
        readline.write_history_file(HISTORY_FILE)
    except Exception:
        pass

# ==============================================================================
# BLOCK 4: AUDIT PROCEDURES
# ==============================================================================
def self_audit() -> bool:
    configs = load_models_config()
    return "omnicode-fast-1.5b" in configs

# ==============================================================================
# BLOCK 5: CLOSING PROCEDURES & MAIN ENTRYPOINT
# ==============================================================================
def main():
    args = sys.argv[1:]

    if "--scan" in args:
        spectrum = scan_full_spectrum()
        print("=== EMPIRICAL ZERO-ASSUMPTION PC SCAN ===")
        for tier, cfg in spectrum.items():
            ceil = " [LOCAL CEILING]" if cfg["is_local_ceiling"] else ""
            print(f"Tier {tier.upper()}{ceil}: GPU={cfg['recommended_gpu_layers']}/{cfg['total_layers']} | CPU={cfg['cpu_layers']} | Ctx={cfg['min_ctx']}-{cfg['max_ctx']} (KV: ~{cfg['kv_cache_est_mb']} MB)")
        sys.exit(0)

    if "--serve" in args or "serve" in args:
        server = launch_service_adapter(11435)
        try:
            server.serve_forever()
        except KeyboardInterrupt:
            server.server_close()
        sys.exit(0)

    if "--decompose" in args:
        idx = args.index("--decompose")
        if idx + 1 < len(args):
            query = " ".join(args[idx + 1:])
            decomposer = HeadlessTaskDecomposer()
            dag = decomposer.decompose(query)
            print(json.dumps(dag, indent=2))
            sys.exit(0)
        sys.exit(1)

    if "--calibrate" in args:
        ok, msg = build_and_register_1_5b_agent()
        print(msg)
        sys.exit(0 if ok else 1)

    if "--inspect" in args:
        idx = args.index("--inspect")
        if idx + 1 < len(args):
            res = inspect_file(args[idx + 1])
            if res:
                print(res[0])
                sys.exit(0)
        sys.exit(1)

    if "--verify" in args:
        idx = args.index("--verify")
        target = args[idx + 1] if idx + 1 < len(args) else ""
        if target == "--build" or not target:
            ok, msg = run_compiler_probe()
            print(msg)
            sys.exit(0 if ok else 1)
        else:
            violations = check_file_invariants(target)
            for v in violations:
                print(f"Line {v['line']}: [{v['rule']}] {v['msg']}")
            sys.exit(0 if not violations else 1)

    # Launch GUI if requested
    if "--gui" in args:
        from .gui.app import launch_gui
        launch_gui()
        return

    # Check for piped input
    if not sys.stdin.isatty():
        piped = sys.stdin.read().strip()
        query = " ".join(args) if args else "Analyze input:"
        composed = f"{query}\n\n```\n{piped}\n```"
        store = EpisodicIndexStore()
        disp = AgenticToolDispatcher()
        run_agentic_turn(composed, store, disp, chunk_callback=lambda t, th: sys.stdout.write(t))
        print()
        return

    # Single-shot prompt
    if args and not args[0].startswith("-"):
        query = " ".join(args)
        store = EpisodicIndexStore()
        disp = AgenticToolDispatcher()
        run_agentic_turn(query, store, disp, chunk_callback=lambda t, th: sys.stdout.write(t))
        print()
        return

    # Interactive REPL
    run_terminal_repl()

if __name__ == "__main__":
    main()
