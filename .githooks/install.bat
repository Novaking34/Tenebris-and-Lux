@echo off
rem ============================================================================
rem BLOCK 1: METADATA BLOCK
rem ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
rem ============================================================================
rem @project       Tenebris and Lux
rem @file          .githooks/install.bat
rem @id            tools.githooks.installer_windows_cmd
rem @layer         Developer Tooling & VCS Hooks (Layer 3 Infrastructure)
rem @version       0.1.2-dev
rem @interface_rev Rev 5
rem @date          2026-11-14
rem @author        CreativeWorkzStudio LLC / ProfessorSeanEX
rem @repository    https://github.com/Novaking34/tenebris-lux
rem @system        Git Client-Side Hooks Installer (Windows Batch / CMD)
rem @role          Configures core.hooksPath for classic Windows CMD environments
rem @invariants    1. Configures git core.hooksPath to repository .githooks.
rem                2. Verifies git availability and reports repository anchors.
rem ============================================================================

rem ============================================================================
rem BLOCK 2: OPENING BLOCK / ENVIRONMENT & PLATFORM PROBING
rem Stratum: Developer Tooling | Machine: tools.githooks.installer_windows_cmd
rem ============================================================================

where git >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo [INSTALL ERROR] Git executable not found in PATH. Please install Git for Windows.
    exit /b 1
)

echo ================================================================================
echo    Tenebris and Lux - Git Hooks Installer (Windows Batch CMD)
echo ================================================================================

rem ============================================================================
rem BLOCK 3: BODY BLOCK / CONFIGURATION
rem Stratum: Developer Tooling | Machine: tools.githooks.installer_windows_cmd
rem ============================================================================

echo [STEP 1/1] Configuring git core.hooksPath to '.githooks'...
git config core.hooksPath .githooks
if %ERRORLEVEL% neq 0 (
    echo [INSTALL FAILED] Failed to set git config core.hooksPath.
    exit /b 1
)

rem ============================================================================
rem BLOCK 4: AUDIT PROCEDURES / VERIFICATION HANDSHAKE
rem Stratum: Developer Tooling | Machine: tools.githooks.installer_windows_cmd
rem ============================================================================

for /f "tokens=*" %%i in ('git config core.hooksPath') do set ACTIVE_HOOKS=%%i
echo --------------------------------------------------------------------------------
echo  Verification: core.hooksPath = %ACTIVE_HOOKS% [OK]

rem ============================================================================
rem BLOCK 5: CLOSING BLOCK / LIFECYCLE SUMMARY & EXIT PROTOCOL
rem Stratum: Developer Tooling | Machine: tools.githooks.installer_windows_cmd
rem ============================================================================

echo  [SUCCESS] Git client-side hooks installed and verified for Windows CMD.
echo            Hooks active: commit-msg, pre-commit, pre-push
echo ================================================================================
exit /b 0
