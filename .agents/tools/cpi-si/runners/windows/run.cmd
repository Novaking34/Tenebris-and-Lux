@echo off
REM ============================================================================
REM BLOCK 1: METADATA BLOCK
REM ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Rules )
REM ============================================================================
REM @project       OmniCode Framework / Big House Ecosystem
REM @file          runners/windows/run.cmd
REM @id            cog.agent.tools.cpi_si_cli.runner.windows.cmd
REM @layer         Layer 4: Cross-Platform Runner (Windows CMD Shim -> ps1)
REM @archetype     zero_clutter_runner | windows_cmd_wrapper
REM @version       0.1.3-dev
REM @interface_rev Rev 5
REM @created       2026-11-20
REM @updated       2026-11-20
REM @author        CreativeWorkzStudio LLC / Seanje Lenox-Wise & Silas Oren
REM @repository    https://github.com/Novaking34/OmniCode_Framework
REM @system        CPI-SI Portable Cognitive BIOS CLI & Substrate Tooling
REM @role          Zero-Friction CMD Entrypoint Delegating to run.ps1
REM @maturity      Rev 5 Operational Baseline | Wave A Tools Alignment
REM @chassis       stateless-wrapper | Delegates cache build & run to run.ps1
REM @seams         calls: ./run.ps1
REM @invariants    1. Universal 5-Block Lifecycle Protocol (Rev 5).
REM                2. Preserves exact exit code from PowerShell runner.
REM                3. Translates --audit-only cleanly to -AuditOnly.
REM
REM PURPOSE & ARCHITECTURAL ROLE:
REM   Provides a clean native Command Prompt (`run.cmd`) entrypoint on Windows
REM   so users in cmd.exe or batch tooling can run `run.cmd boot` identically
REM   to PowerShell or Bash without PowerShell invocation boilerplate.
REM
REM   ### For Architects, Operators & Non-Programmers:
REM   Click or run `run.cmd audit` from Command Prompt without remembering
REM   `-ExecutionPolicy Bypass` flags.
REM
REM   ### For Systems Programmers & Engine Architects:
REM   Prefers `pwsh.exe` if installed, falling back cleanly to Windows built-in
REM   `powershell.exe` while forwarding remaining arguments and exit status.
REM ============================================================================

REM ============================================================================
REM BLOCK 2: OPENING BLOCK
REM ( Local Variable Scope & PowerShell Executable Discovery )
REM ============================================================================
setlocal EnableExtensions
set "SCRIPT_DIR=%~dp0"
set "PS_SCRIPT=%SCRIPT_DIR%run.ps1"
set "PS_EXE=powershell"
where pwsh >nul 2>nul && set "PS_EXE=pwsh"

REM ============================================================================
REM BLOCK 3: BODY BLOCK
REM ( Normalize --audit-only vs General Subcommand Passthrough )
REM ============================================================================
if "%~1"=="--audit-only" (
    "%PS_EXE%" -NoProfile -ExecutionPolicy Bypass -File "%PS_SCRIPT%" -AuditOnly
    set "EXIT_CODE=%ERRORLEVEL%"
    goto :block4_audit
)
"%PS_EXE%" -NoProfile -ExecutionPolicy Bypass -File "%PS_SCRIPT%" %*
set "EXIT_CODE=%ERRORLEVEL%"

REM ============================================================================
REM BLOCK 4: AUDIT PROCEDURES BLOCK
REM ( Propagate Non-Zero Runtime / Usage Exit Status Faithfully )
REM ============================================================================
:block4_audit
if "%EXIT_CODE%"=="" set "EXIT_CODE=1"

REM ============================================================================
REM BLOCK 5: CLOSING BLOCK
REM ( End Local Scope & Exit With Underlying Runner Status )
REM ============================================================================
endlocal & exit /b %EXIT_CODE%
REM END OF BLOCK 5: CLOSING SEAL
