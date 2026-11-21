#!/usr/bin/env bash
# ==============================================================================
# BLOCK 1: METADATA BLOCK
# ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
# ==============================================================================
# @project       OmniCode Framework / Big House Ecosystem
# @file          runners/macos/run.sh
# @id            cog.agent.tools.cpi_si_cli.runner.macos
# @layer         Layer 4: Cross-Platform Runner (macOS Apple Silicon / Intel)
# @archetype     zero_clutter_runner | macos_bash32_entrypoint
# @version       0.1.3-dev
# @interface_rev Rev 5
# @created       2026-11-20
# @updated       2026-11-20
# @author        CreativeWorkzStudio LLC / Seanje Lenox-Wise & Silas Oren
# @repository    https://github.com/Novaking34/OmniCode_Framework
# @system        CPI-SI Portable Cognitive BIOS CLI & Substrate Tooling
# @role          Dedicated macOS Launcher Caching Under ~/Library/Caches
# @maturity      Rev 5 Operational Baseline | Wave A Tools Alignment
# @chassis       stateless-wrapper | Pure bash 3.2+ & ~/Library/Caches target
# @scale         O(1) cached pass; O(N_go) atomic rebuild on source edits
# @seams         builds: ../../cmd/cpi-si/main.go | sets: CPI_SI_AGENTS_DIR
# @invariants    1. Universal 5-Block Lifecycle Protocol (Rev 5).
#                2. Strictly bash 3.2+ compatible (macOS default /bin/bash).
#                3. Avoids GNU-only find flags (`-quit`, `-printf`).
#                4. Supports --audit-only for non-mutating cross-OS checks.
#
# PURPOSE & ARCHITECTURAL ROLE:
#   macOS counterpart to `runners/run.sh`, matching FaithNet's dedicated
#   `macos/` layout while compiling `./cmd/cpi-si` into `~/Library/Caches`.
#
#   ### For Architects, Operators & Non-Programmers:
#   Gives macOS builders and Apple Silicon machines a first-class launcher that
#   respects macOS cache folders cleanly without touching `.agents/`.
#
#   ### For Systems Programmers & Engine Architects:
#   Uses BSD-safe `find ... | head -n 1` and pure POSIX `cd`/`pwd` resolution so
#   macOS `/bin/bash` (3.2) executes identically without Homebrew bash.
# ==============================================================================

# ==============================================================================
# BLOCK 2: OPENING BLOCK
# ( Strict Guards, macOS Library/Caches Target & Substrate Path Wiring )
# ==============================================================================
# Engine:        CPI-SI Portable Cognitive BIOS Tooling (cpi-si-cli)
# Stratum:       Layer 4: Cross-Platform Runner (macOS)
# Module:        cog.agent.tools.cpi_si_cli.runner.macos
# Mission:       Resolve macOS external cache directory and .agents/ root.
# ==============================================================================
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TOOL_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
AGENTS_ROOT="$(cd "$TOOL_ROOT/../.." && pwd)"
export CPI_SI_AGENTS_DIR="${CPI_SI_AGENTS_DIR:-$AGENTS_ROOT}"

CACHE_ROOT="${CPI_SI_CACHE_DIR:-${XDG_CACHE_HOME:-$HOME/Library/Caches}/cpi-si-cli}"
BIN="$CACHE_ROOT/cpi-si"

# ==============================================================================
# BLOCK 3: BODY BLOCK
# ( Audit Mode & BSD-Compatible Staleness Detection )
# ==============================================================================
# Engine:        CPI-SI Portable Cognitive BIOS Tooling (cpi-si-cli)
# Stratum:       Layer 4: Cross-Platform Runner (macOS)
# Module:        cog.agent.tools.cpi_si_cli.runner.macos
# Mission:       Validate layout in --audit-only or detect stale Go binary.
# ==============================================================================
if [ "${1:-}" = "--audit-only" ]; then
    if [ ! -f "$TOOL_ROOT/go.mod" ] || [ ! -f "$TOOL_ROOT/cmd/cpi-si/main.go" ]; then
        echo "[FAIL] macos/run.sh audit: missing go.mod or cmd/cpi-si/main.go" >&2
        exit 1
    fi
    if ! command -v go >/dev/null 2>&1; then
        echo "[FAIL] macos/run.sh audit: 'go' compiler missing from PATH" >&2
        exit 127
    fi
    echo "[PASS] runners/macos/run.sh audit OK (tool_root=$TOOL_ROOT, cache_bin=$BIN)"
    exit 0
fi

need_build=0
if [ ! -x "$BIN" ] || [ "${CPI_SI_REBUILD:-0}" = "1" ]; then
    need_build=1
else
    newer_file="$(find "$TOOL_ROOT" \( -name '*.go' -o -name 'go.mod' \) -newer "$BIN" -print 2>/dev/null | head -n 1 || true)"
    if [ -n "$newer_file" ]; then
        need_build=1
    fi
fi

# ==============================================================================
# BLOCK 4: AUDIT PROCEDURES BLOCK
# ( Go Compiler Check & Atomic Cache Build )
# ==============================================================================
# Engine:        CPI-SI Portable Cognitive BIOS Tooling (cpi-si-cli)
# Stratum:       Layer 4: Cross-Platform Runner (macOS)
# Module:        cog.agent.tools.cpi_si_cli.runner.macos
# Mission:       Compile cmd/cpi-si cleanly into external macOS cache dir.
# ==============================================================================
if [ "$need_build" -eq 1 ]; then
    if ! command -v go >/dev/null 2>&1; then
        echo "cpi-si macos runner error: 'go' toolchain not found in PATH" >&2
        exit 127
    fi
    mkdir -p "$CACHE_ROOT"
    TMP_BIN="$BIN.tmp.$$"
    (cd "$TOOL_ROOT" && go build -o "$TMP_BIN" ./cmd/cpi-si)
    mv -f "$TMP_BIN" "$BIN"
fi

# ==============================================================================
# BLOCK 5: CLOSING BLOCK
# ( Delegate Execution With Zero Binary Clutter in .agents/ )
# ==============================================================================
# Engine:        CPI-SI Portable Cognitive BIOS Tooling (cpi-si-cli)
# Stratum:       Layer 4: Cross-Platform Runner (macOS)
# Module:        cog.agent.tools.cpi_si_cli.runner.macos
# Mission:       Launch external binary passing through all CLI arguments.
# ==============================================================================
exec "$BIN" "$@"
# END OF BLOCK 5: CLOSING SEAL
