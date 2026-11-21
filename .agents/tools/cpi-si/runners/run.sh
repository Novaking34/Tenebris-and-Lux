#!/usr/bin/env bash
# ==============================================================================
# BLOCK 1: METADATA BLOCK
# ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
# ==============================================================================
# @project       OmniCode Framework / Big House Ecosystem
# @file          runners/run.sh
# @id            cog.agent.tools.cpi_si_cli.runner.posix
# @layer         Layer 4: Cross-Platform Runner (Linux & Portable POSIX)
# @archetype     zero_clutter_runner | posix_bash_entrypoint
# @version       0.1.3-dev
# @interface_rev Rev 5
# @created       2026-11-20
# @updated       2026-11-20
# @author        CreativeWorkzStudio LLC / Seanje Lenox-Wise & Silas Oren
# @repository    https://github.com/Novaking34/OmniCode_Framework
# @system        CPI-SI Portable Cognitive BIOS CLI & Substrate Tooling
# @role          Compile/Cache cmd/cpi-si Outside .agents/ and Execute Cleanly
# @maturity      Rev 5 Operational Baseline | Wave A Tools Alignment
# @chassis       stateless-wrapper | Binary isolated in external user cache
# @scale         O(1) cached pass; O(N_go) atomic rebuild on source changes
# @seams         builds: ../cmd/cpi-si/main.go | sets: CPI_SI_AGENTS_DIR
# @invariants    1. Universal 5-Block Lifecycle Protocol (Rev 5).
#                2. Never write compiled binaries inside .agents/ or repo root.
#                3. Compatible with POSIX/bash 3.2+ (no bash 4+ extensions).
#                4. Supports --audit-only for non-mutating runner verification.
#
# PURPOSE & ARCHITECTURAL ROLE:
#   Builds `cmd/cpi-si` on demand into an external user cache directory, keeps
#   `.agents/` 100% free of executable binaries, and runs any CLI subcommand.
#
#   ### For Architects, Operators & Non-Programmers:
#   Give this script any CLI verb (`boot`, `tail`, `bench`, `skills`, `audit`,
#   `preflight`), and it quietly keeps your Go binary fresh behind the scenes.
#
#   ### For Systems Programmers & Engine Architects:
#   Detects cache directory cleanly per OS (XDG cache on Linux; Library/Caches
#   on Darwin), avoids GNU-only `find -quit`, and swaps binaries atomically.
# ==============================================================================

# ==============================================================================
# BLOCK 2: OPENING BLOCK
# ( Strict Shell Guards, Cache Paths & Audit Mode Switch )
# ==============================================================================
# Engine:        CPI-SI Portable Cognitive BIOS Tooling (cpi-si-cli)
# Stratum:       Layer 4: Cross-Platform Runner (POSIX)
# Module:        cog.agent.tools.cpi_si_cli.runner.posix
# Mission:       Resolve TOOL_ROOT, AGENTS_ROOT, and external binary cache path.
# ==============================================================================
set -euo pipefail

RUNNER_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TOOL_ROOT="$(cd "$RUNNER_DIR/.." && pwd)"
AGENTS_ROOT="$(cd "$TOOL_ROOT/../.." && pwd)"
export CPI_SI_AGENTS_DIR="${CPI_SI_AGENTS_DIR:-$AGENTS_ROOT}"

UNAME_S="$(uname -s 2>/dev/null || echo Linux)"
if [ "$UNAME_S" = "Darwin" ]; then
    DEFAULT_CACHE="${XDG_CACHE_HOME:-$HOME/Library/Caches}/cpi-si-cli"
else
    DEFAULT_CACHE="${XDG_CACHE_HOME:-$HOME/.cache}/cpi-si-cli"
fi
CACHE_ROOT="${CPI_SI_CACHE_DIR:-$DEFAULT_CACHE}"
BIN="$CACHE_ROOT/cpi-si"

# ==============================================================================
# BLOCK 3: BODY BLOCK
# ( Pre-Flight Audit Option & Atomic External Cache Build )
# ==============================================================================
# Engine:        CPI-SI Portable Cognitive BIOS Tooling (cpi-si-cli)
# Stratum:       Layer 4: Cross-Platform Runner (POSIX)
# Module:        cog.agent.tools.cpi_si_cli.runner.posix
# Mission:       Handle --audit-only cleanly or rebuild stale external binary.
# ==============================================================================

# --- Sub-Assembly 3.1: Non-Mutating Runner Self-Audit Mode ---
if [ "${1:-}" = "--audit-only" ]; then
    if [ ! -f "$TOOL_ROOT/go.mod" ] || [ ! -f "$TOOL_ROOT/cmd/cpi-si/main.go" ]; then
        echo "[FAIL] run.sh audit: missing go.mod or cmd/cpi-si/main.go under $TOOL_ROOT" >&2
        exit 1
    fi
    if ! command -v go >/dev/null 2>&1; then
        echo "[FAIL] run.sh audit: 'go' compiler missing from PATH" >&2
        exit 127
    fi
    echo "[PASS] runners/run.sh audit OK (tool_root=$TOOL_ROOT, cache_bin=$BIN)"
    exit 0
fi

# --- Sub-Assembly 3.2: Source-Staleness Check & Atomic Build ---
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
# ( Toolchain Guard & Atomic Binary Promotion )
# ==============================================================================
# Engine:        CPI-SI Portable Cognitive BIOS Tooling (cpi-si-cli)
# Stratum:       Layer 4: Cross-Platform Runner (POSIX)
# Module:        cog.agent.tools.cpi_si_cli.runner.posix
# Mission:       Verify go compiler exists before invoking atomic build.
# ==============================================================================
if [ "$need_build" -eq 1 ]; then
    if ! command -v go >/dev/null 2>&1; then
        echo "cpi-si runner error: 'go' toolchain not found in PATH" >&2
        exit 127
    fi
    mkdir -p "$CACHE_ROOT"
    TMP_BIN="$BIN.tmp.$$"
    (cd "$TOOL_ROOT" && go build -o "$TMP_BIN" ./cmd/cpi-si)
    mv -f "$TMP_BIN" "$BIN"
fi

# ==============================================================================
# BLOCK 5: CLOSING BLOCK
# ( Execute Compiled Cache Binary With Zero .agents/ Residue )
# ==============================================================================
# Engine:        CPI-SI Portable Cognitive BIOS Tooling (cpi-si-cli)
# Stratum:       Layer 4: Cross-Platform Runner (POSIX)
# Module:        cog.agent.tools.cpi_si_cli.runner.posix
# Mission:       Hand control cleanly to the external cached binary.
# ==============================================================================
exec "$BIN" "$@"
# END OF BLOCK 5: CLOSING SEAL
