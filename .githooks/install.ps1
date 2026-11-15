<#
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          .githooks/install.ps1
@id            tools.githooks.installer_windows_ps
@layer         Developer Tooling & VCS Hooks (Layer 3 Infrastructure)
@version       0.1.2-dev
@interface_rev Rev 5
@date          2026-11-14
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Git Client-Side Hooks Installer (Windows PowerShell)
@role          Configures core.hooksPath for Windows developer environments
@invariants    1. Configures git core.hooksPath to repository .githooks.
               2. Verifies git availability and reports repository anchors.
               3. Full native Windows PowerShell 5.1 & PowerShell Core 7+ support.

PURPOSE & ARCHITECTURAL ROLE:
  Native Windows PowerShell installer configuring repository git hooks path,
  enabling cross-platform parity between Windows, Linux, and macOS developers.
================================================================================
#>

# ==============================================================================
# BLOCK 2: OPENING BLOCK / ENVIRONMENT & PLATFORM PROBING
# Stratum: Developer Tooling | Machine: tools.githooks.installer_windows_ps
# ==============================================================================

# ------------------------------------------------------------------------------
# 2.1 Git Command Availability Check
# ------------------------------------------------------------------------------
$gitCmd = Get-Command git -ErrorAction SilentlyContinue
if (-not $gitCmd) {
    Write-Error "[INSTALL ERROR] Git executable not found in PATH. Please install Git for Windows."
    exit 1
}

$repoRoot = git rev-parse --show-toplevel 2>$null
if (-not $repoRoot) {
    $repoRoot = (Get-Location).Path
}

$hooksDir = Join-Path $repoRoot ".githooks"
if (-not (Test-Path $hooksDir)) {
    Write-Error "[INSTALL ERROR] Directory not found: $hooksDir"
    exit 1
}

Write-Host "================================================================================" -ForegroundColor Cyan
Write-Host "   Tenebris and Lux - Git Hooks Installer (Windows PowerShell)                  " -ForegroundColor Cyan
Write-Host "================================================================================" -ForegroundColor Cyan
Write-Host " Host Platform:   Windows ($($PSVersionTable.PSVersion))"
Write-Host " Repository Root: $repoRoot"
Write-Host " Hooks Directory: $hooksDir"
Write-Host "--------------------------------------------------------------------------------"

# ==============================================================================
# BLOCK 3: BODY BLOCK / CONFIGURATION
# Stratum: Developer Tooling | Machine: tools.githooks.installer_windows_ps
# ==============================================================================

# ------------------------------------------------------------------------------
# 3.1 Point Git to Repository .githooks Directory
# ------------------------------------------------------------------------------
Write-Host "[STEP 1/1] Configuring git core.hooksPath to '.githooks'..." -ForegroundColor Yellow
git config core.hooksPath .githooks

# ==============================================================================
# BLOCK 4: AUDIT PROCEDURES / VERIFICATION HANDSHAKE
# Stratum: Developer Tooling | Machine: tools.githooks.installer_windows_ps
# ==============================================================================

$activeHooksPath = git config core.hooksPath
if ($activeHooksPath -ne ".githooks") {
    Write-Error "[INSTALL FAILED] core.hooksPath is '$activeHooksPath' (expected '.githooks')"
    exit 1
}

Write-Host "--------------------------------------------------------------------------------"
Write-Host " Verification: core.hooksPath = $activeHooksPath [OK]" -ForegroundColor Green

# ==============================================================================
# BLOCK 5: CLOSING BLOCK / LIFECYCLE SUMMARY & EXIT PROTOCOL
# Stratum: Developer Tooling | Machine: tools.githooks.installer_windows_ps
# ==============================================================================

Write-Host " [SUCCESS] Git client-side hooks installed and verified for Windows." -ForegroundColor Green
Write-Host "           Hooks active: commit-msg, pre-commit, pre-push"
Write-Host "           Note: Git for Windows will execute bash hook scripts via bash.exe."
Write-Host "================================================================================" -ForegroundColor Cyan
exit 0
