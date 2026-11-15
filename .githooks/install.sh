#!/usr/bin/env bash
# ==============================================================================
# BLOCK 1: METADATA BLOCK
# ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
# ==============================================================================
# @project       Tenebris and Lux
# @file          .githooks/install.sh
# @id            tools.githooks.installer_posix
# @layer         Developer Tooling & VCS Hooks (Layer 3 Infrastructure)
# @version       0.1.2-dev
# @interface_rev Rev 5
# @date          2026-11-14
# @author        CreativeWorkzStudio LLC / ProfessorSeanEX
# @repository    https://github.com/Novaking34/tenebris-lux
# @system        Git Client-Side Hooks Installer (Linux & macOS POSIX)
# @role          Configures core.hooksPath and sets executable permissions
# @invariants    1. Sets git core.hooksPath to repository .githooks.
#                2. Ensures chmod +x on all client-side hook scripts.
#                3. Compatible across Linux and macOS (Darwin) POSIX environments.
#
# PURPOSE & ARCHITECTURAL ROLE:
#   One-command installer for local developers on Linux and macOS to configure
#   git hooks path and permissions, integrating local repository hygiene with
#   the OmniCode 5-Block ledger protocol.
# ==============================================================================

# ==============================================================================
# BLOCK 2: OPENING BLOCK / ENVIRONMENT & PLATFORM PROBING
# Stratum: Developer Tooling | Machine: tools.githooks.installer_posix
# ==============================================================================

# ------------------------------------------------------------------------------
# 2.1 Git Binary Discovery
# ------------------------------------------------------------------------------
if ! command -v git >/dev/null 2>&1; then
    echo "[INSTALL ERROR] Git executable not found in PATH. Please install git." >&2
    exit 1
fi

REPO_ROOT="$(git rev-parse --show-toplevel 2>/dev/null || pwd)"
HOOKS_DIR="$REPO_ROOT/.githooks"

if [ ! -d "$HOOKS_DIR" ]; then
    echo "[INSTALL ERROR] Directory not found: $HOOKS_DIR" >&2
    exit 1
fi

# ------------------------------------------------------------------------------
# 2.2 Host OS Detection (Linux / macOS Darwin)
# ------------------------------------------------------------------------------
OS_NAME="$(uname -s 2>/dev/null || echo 'Unknown')"
echo "================================================================================"
echo "   Tenebris and Lux - Git Hooks Installer (Linux & macOS POSIX)                 "
echo "================================================================================"
echo " Host Platform:   $OS_NAME"
echo " Repository Root: $REPO_ROOT"
echo " Hooks Directory: $HOOKS_DIR"
echo "--------------------------------------------------------------------------------"

# ==============================================================================
# BLOCK 3: BODY BLOCK / CONFIGURATION & PERMISSION HARDENING
# Stratum: Developer Tooling | Machine: tools.githooks.installer_posix
# ==============================================================================

# ------------------------------------------------------------------------------
# 3.1 Point Git to Repository .githooks Directory
# ------------------------------------------------------------------------------
echo "[STEP 1/2] Configuring git core.hooksPath to '.githooks'..."
git config core.hooksPath .githooks

# ------------------------------------------------------------------------------
# 3.2 Ensure Executable Bit Across All Hook Scripts
# ------------------------------------------------------------------------------
echo "[STEP 2/2] Setting executable permissions (chmod +x) on hook scripts..."
for HOOK in "$HOOKS_DIR"/*; do
    if [ -f "$HOOK" ]; then
        case "$HOOK" in
            *.sh|*.ps1|*.bat)
                chmod +x "$HOOK" 2>/dev/null || true
                ;;
            *)
                # Files without extension (commit-msg, pre-commit, pre-push)
                chmod +x "$HOOK" 2>/dev/null || true
                ;;
        esac
    fi
done

# ==============================================================================
# BLOCK 4: AUDIT PROCEDURES / VERIFICATION HANDSHAKE
# Stratum: Developer Tooling | Machine: tools.githooks.installer_posix
# ==============================================================================

ACTIVE_HOOKS_PATH="$(git config core.hooksPath)"
if [ "$ACTIVE_HOOKS_PATH" != ".githooks" ]; then
    echo "[INSTALL FAILED] core.hooksPath is '$ACTIVE_HOOKS_PATH' (expected '.githooks')" >&2
    exit 1
fi

echo "--------------------------------------------------------------------------------"
echo " Verification: core.hooksPath = $ACTIVE_HOOKS_PATH [OK]"

# ==============================================================================
# BLOCK 5: CLOSING BLOCK / LIFECYCLE SUMMARY & EXIT PROTOCOL
# Stratum: Developer Tooling | Machine: tools.githooks.installer_posix
# ==============================================================================

echo " [SUCCESS] Git client-side hooks installed and verified for $OS_NAME."
echo "           Hooks active: commit-msg, pre-commit, pre-push"
echo "================================================================================"
exit 0
